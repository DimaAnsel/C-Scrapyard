/**
 * @file huffman.c
 *
 * Compact implementation of Huffman coding.
 */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "huffman.h"

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////
///
/// @defgroup HuffmanHelpers Huffman helper functions
/// Helper functions for Huffman compression framework in {@link huffman.c}.
///
////////////////////////////////////////////////////////////////

/**
 * @ingroup HuffmanHelpers
 * Enables throwing of {@link HuffmanError} to calling function if error
 * occurred.
 *
 * @param[in] f Function to be called.
 */
#define THROW_ERR(f) err = (f); if (err != ERR_NO_ERR) { return err; }

/**
 * @ingroup HuffmanHelpers
 * Ceiling of log base 2 for uint64_t value.
 *
 * @param[in] num Value to be processed.
 * @return ceil(log2(num))
 */
static uint8_t log2_ceil_u64(uint64_t num) {
	if (num == 1) {
		return 1;
	}
	uint8_t ret = 0;
	bool leftmost = true;
	for (int8_t i = 63; i >= 0; i--) {
		if ((num >> (uint64_t)i) & (uint64_t)0x1) {
			if (leftmost) { // leftmost bit
				ret = (uint8_t)i;
				leftmost = false;
			} else { // not even power of 2
				ret++;
				break;
			}
		}
	}
	return ret;
}

/**
 * @ingroup HuffmanHelpers
 * Ceiling of log base 2 for uint8_t value.
 *
 * @param[in] num Value to be processed.
 * @return ceil(log2(num))
 */
static uint8_t log2_ceil_u8(uint8_t num) {
	if (num == 1) {
		return 1;
	}
	uint8_t ret = 0;
	bool leftmost = true;
	for (int8_t i = 7; i >= 0; i--) {
		if ((num >> (uint8_t)i) & (uint8_t)0x1) {
			if (leftmost) { // leftmost bit
				ret = (uint8_t)i;
				leftmost = false;
			} else { // not even power of 2
				ret++;
				break;
			}
		}
	}
	return ret;
}

/**
 * @ingroup HuffmanHelpers
 * Reads a value beginning at an arbitrary position.
 *
 * @param[out]    dst   Destination for parsed value. Will be completely overwritten regardless of size.
 * @param[in,out] src   Pointer to byte from which to read. Updated to first byte of following section.
 * @param[in,out] start Bit from which to start. Updated to bit of following section. Range 0-7.
 * @param[in]     size  Number of bits to read. Range 1-64.
 *
 * @return {@link ERR_NO_ERR} if no error occurred.\n
 * 		   {@link ERR_NULL_PTR} if dst, src, or start are null or if value of src is null.\n
 * 		   {@link ERR_INVALID_VALUE} if start or size out of accepted range.
 */
static HuffmanError extract_bits(uint64_t* dst,
								 uint8_t** src,
								 uint8_t* start,
								 uint8_t size) {
	if (dst == NULL || src == NULL || *src == NULL || start == NULL) {
		return ERR_NULL_PTR;
	}
	if (*start >= 8 || size == 0 || size > 64) {
		return ERR_INVALID_VALUE;
	}

	// return value
	*dst = 0x0;
	// mask for copying incomplete bytes; default to case II,III,IV
	uint8_t mask = ((0x1 << (8 - *start)) - 1) & 0xFF;

	// offset to new arr
	uint8_t newArrOffset = (*start + size) / 8;
	// new start bit
	uint8_t newStart = (*start + size) % 8;
	// current shift val; only used in case III,IV
	uint8_t shift = (newArrOffset > 0) ? ((newArrOffset - 1) * 8) + newStart : 0;

	// use this for further ops
	uint8_t* tempPtr = *src;

	// must be done after init mask
	(*src) += newArrOffset;
	(*start) = newStart;

	if (newArrOffset == 0) {
		// Case I: single byte, non-even end
		mask = (mask ^ ((0x1 << (8 - newStart)) - 1)) & 0xFF;
		*dst = (*tempPtr & mask) >> (8 - newStart); // shift right to end
		return ERR_NO_ERR;
	} else if (newArrOffset == 1 && newStart == 0) {
		// Case II: single byte, even end
		*dst = *tempPtr & mask; // no shift, already in correct location
		return ERR_NO_ERR;
	}

	// Case III/IV: multi-byte
	*dst = ((uint64_t)(*tempPtr & mask)) << shift; // first byte
	while (shift > 7) {
		shift -= 8;
		tempPtr++;
		*dst |= ((uint64_t)(*tempPtr & 0xFF)) << shift;
	}

	// Case IV: non-even end
	if (newStart != 0) {
		// copy end
		mask = (0xFF ^ ((0x1 << (8 - newStart)) - 1)) & 0xFF;
		tempPtr++;
		*dst |= ((*tempPtr) & mask) >> (8 - newStart); // shift right to end
	}
	return ERR_NO_ERR;
}

/**
 * @ingroup HuffmanHelpers
 * Puts a value into an arbitrary position.
 *
 * @param[in,out] dst     Pointer to first byte in which to set data. Updated to first byte of following section.
 * @param[in,out] start   Bit from which to start. Updated to first bit of following section. Range 0-7.
 * @param[in,out] dstSize Number of bytes free in dst. Updated to number of bytes remaining.
 * @param[in]     val     Value to be written.
 * @param[in]     size    Number of bits to write. Range 1-64.
 *
 * @return {@link ERR_NO_ERR} if no error occurred.\n
 * 		   {@link ERR_NULL_PTR} if dst or start are null or if value of dst is null.\n
 * 		   {@link ERR_INVALID_VALUE} if start or size out of accepted range.\n
 * 		   {@link ERR_INSUFFICIENT_SPACE} if function requires more than dstSize bytes to write data.
 */
static HuffmanError put_bits(uint8_t** dst,
							 uint8_t* start,
							 uint64_t* dstSize,
							 uint64_t val,
							 uint8_t size) {
	if (dst == NULL || *dst == NULL || start == NULL || dstSize == NULL) {
		return ERR_NULL_PTR;
	}
	if (*start >= 8 || size == 0 || size > 64) {
		return ERR_INVALID_VALUE;
	}
	if (*dstSize == 0) {
		return ERR_INSUFFICIENT_SPACE;
	}

	// offset to new arr
	uint8_t newArrOffset = (*start + size) / 8;
	// new start bit
	uint8_t newStart = (*start + size) % 8;
	// current shift val; only used in case III,IV
	uint8_t shift = (newArrOffset > 0) ? size - (8 - *start) : 0;
	// mask
	uint8_t mask = 0xFF ^ ((1 << (8 - *start)) - 1);

	// verify have space to write data
	if (newArrOffset > *dstSize || (newArrOffset == *dstSize && newStart > 0)) {
		return ERR_INSUFFICIENT_SPACE;
	}

	// clip val to bottom [size] bits
	if (size < 64) {
		val = val & (((uint64_t)1 << size) - 1);
	}

	// use this for further ops
	uint8_t* tempPtr = *dst;

	// must be done after init mask
	(*dst) += newArrOffset;
	(*start) = newStart;
	(*dstSize) = (*dstSize) - (uint64_t)newArrOffset;

	if (newArrOffset == 0) {
		// Case I: single byte, non-even end
		*tempPtr = ((*tempPtr) & mask) | (val << (8 - newStart));
		return ERR_NO_ERR;
	} else if (newArrOffset == 1 && newStart == 0) {
		// Case II: single byte, even end
		*tempPtr = ((*tempPtr) & mask) | val;
		return ERR_NO_ERR;
	}

	// Case III/IV: multi-byte
	*tempPtr = ((*tempPtr) & mask) | (val >> shift);

	while (shift > 7) {
		shift -= 8;
		tempPtr++;
		*tempPtr = (val >> shift) & 0xFF;
	}

	// Case IV: multi-byte, non-even end
	if (newStart > 0) {
		tempPtr++;
		*tempPtr = (val << (8 - newStart)) & 0xFF;
	}
	return ERR_NO_ERR;
}

/**
 * @ingroup HuffmanHelpers
 * Constructs a header for a Huffman compressed data. Does not include
 * value map. File header size is
 * {@link HUFFMAN_WORD_SIZE_NUM_BITS} + ceil(log2(wordSize)) + wordSize bits.
 *
 * @see parse_header
 *
 * @param[in,out] dst     Pointer to first byte in which to set data. Updated to first byte of following section.
 * @param[out]    start   Starting bit of next section. Range 0-7.
 * @param[in,out] dstSize Number of bytes free in dst.
 * @param[in]     header  Header data from which to generate output.
 *
 * @return {@link ERR_NO_ERR} if no error occurred.\n
 * 		   {@link ERR_NULL_PTR} if a parameter is null, or if value of dst is null.\n
 * 		   {@link ERR_INVALID_VALUE} if start or any member of header are out of accepted range.\n
 * 		   {@link ERR_INSUFFICIENT_SPACE} if function requires more than dstSize bytes to write data.\n
 * 		   Other errors as raised by {@link put_bits}.
 */
static HuffmanError build_header(uint8_t** dst,
								 uint8_t* start,
								 uint64_t* dstSize,
								 HuffmanHeader* header) {
	// Input validation
	if (dst == NULL || *dst == NULL || start == NULL || dstSize == NULL || header == NULL) {
		return ERR_NULL_PTR;
	}
	uint64_t maxWords = (header->wordSize == 64) ? HUFFMAN_MAX_UNIQUE_WORDS : (((uint64_t)0x1) << header->wordSize) - 1;
	if (header->wordSize < HUFFMAN_MIN_WORD_SIZE || header->wordSize > HUFFMAN_MAX_WORD_SIZE ||
			header->uniqueWords > maxWords || header->padBits >= header->wordSize) {
		return ERR_INVALID_VALUE;
	}
	// Ensure enough space is available
	uint8_t log2wordSize = log2_ceil_u8(header->wordSize);
	// Number of bits required to store header
	uint8_t reqBits = HUFFMAN_WORD_SIZE_NUM_BITS + log2wordSize + header->wordSize;
	uint8_t reqBytes = reqBits / 8;
	if (*dstSize < reqBytes || (*dstSize == reqBytes && (reqBits % 8) > 0)) {
		return ERR_INSUFFICIENT_SPACE;
	}

	HuffmanError err;
	uint8_t* currDst = *dst;
	uint8_t currBit = 0;
	uint64_t newSize = *dstSize;

	// word size
	THROW_ERR(put_bits(&currDst, &currBit, &newSize, (uint64_t)header->wordSize - 1, (uint8_t)HUFFMAN_WORD_SIZE_NUM_BITS))
	// pad bits
	THROW_ERR(put_bits(&currDst, &currBit, &newSize, (uint64_t)header->padBits, log2wordSize))
	// unique words
	THROW_ERR(put_bits(&currDst, &currBit, &newSize, header->uniqueWords, header->wordSize))

	// update after all successful
	(*dst) = currDst;
	(*start) = currBit;
	(*dstSize) = newSize;

	return err;
}

/**
 * @ingroup HuffmanHelpers
 * Parses a header from Huffman compressed data into a {@link HuffmanHeader}
 * struct. Does not include value map.
 *
 * @see build_header
 *
 * @param[out]    header  Destination for parsed values.
 * @param[in,out] src     Pointer to byte from which to read. Updated to first byte of following section.
 * @param[out]    start   Starting bit of next section. Range 0-7.
 * @param[in,out] srcSize Length of src in bytes.
 *
 * @return {@link ERR_NO_ERR} if no error occurred.\n
 *         {@link ERR_NULL_PTR} if a parameter is null, or if value of src is null.\n
 *         {@link ERR_INVALID_VALUE} if start or srcSize are out of accepted range.\n
 *         {@link ERR_INSUFFICIENT_SPACE} if header occupies more bytes than srcSize.\n
 *         {@link ERR_INVALID_DATA} if the provided data contains invalid values.\n
 *         Other errors as raised by {@link extract_bits}.
 */
static HuffmanError parse_header(HuffmanHeader* header,
								 uint8_t** src,
								 uint8_t* start,
								 uint64_t* srcSize) {
	if (header == NULL || src == NULL || *src == NULL || start == NULL) {
		return ERR_NULL_PTR;
	}
	if (srcSize < 2) { // require minimum 6 + 1 + 2 bits = 1 byte + 1 bit
		return ERR_INVALID_VALUE;
	}
	uint64_t temp = 0;
	uint8_t size;
	uint8_t* tempPtr = *src;
	uint8_t tempStart = 0;
	uint64_t newSize = *srcSize;
	HuffmanError err;

	// parse wordSize
	size = HUFFMAN_WORD_SIZE_NUM_BITS;
	THROW_ERR(extract_bits(&temp, &tempPtr, &tempStart, size))

	// invalid wordSize
	if (temp < HUFFMAN_MIN_WORD_SIZE - 1 || temp > HUFFMAN_MAX_WORD_SIZE - 1) {
		return ERR_INVALID_DATA;
	}

	// apply offset to obtain actual value
	header->wordSize = (uint8_t)temp + 1;

	// Ensure enough space is available
	uint8_t log2wordSize = log2_ceil_u8(header->wordSize);
	// Number of bits required to store header
	uint8_t reqBits = HUFFMAN_WORD_SIZE_NUM_BITS + log2wordSize + header->wordSize;
	uint8_t reqBytes = reqBits / 8;
	if (*srcSize < reqBytes || (*srcSize == reqBytes && (reqBits % 8) > 0)) {
		return ERR_INSUFFICIENT_SPACE;
	}


	// parse padBits
	size = log2_ceil_u8(header->wordSize);
	THROW_ERR(extract_bits(&temp, &tempPtr, &tempStart, size))
	header->padBits = (uint8_t)temp;

	// parse uniqueWords
	size = header->wordSize;
	THROW_ERR(extract_bits(&temp, &tempPtr, &tempStart, size))
	header->uniqueWords = temp;

	// success, update val
	(*src) = tempPtr;
	(*start) = tempStart;
	(*srcSize) -= reqBytes;

	return ERR_NO_ERR;
}

#ifdef __cplusplus
}
#endif
