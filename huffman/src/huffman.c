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

/**
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
 * Puts a value into an arbitrary position.
 *
 * @param[in,out] dst      Pointer to first byte in which to set data. Updated to first byte of following section.
 * @param[in,out] start    Bit from which to start. Updated to first bit of following section. Range 0-7.
 * @param[in]     dst_size Number of bytes free in dst.
 * @param[in]     size     Number of bits to write. Range 1-64.
 * @param[in]     val      Value to be written.
 *
 * @return {@link ERR_NO_ERR} if no error occurred.\n
 * 		   {@link ERR_NULL_PTR} if dst or start are null or if value of dst is null.\n
 * 		   {@link ERR_INVALID_VALUE} if start or size out of accepted range.\n
 * 		   {@link ERR_INSUFFICIENT_SPACE} if function requires more than dst_size bytes to write data.
 */
static HuffmanError put_bits(uint8_t** dst,
							 uint8_t* start,
							 uint8_t dst_size,
							 uint64_t val,
							 uint8_t size) {
	if (dst == NULL || *dst == NULL || start == NULL) {
		return ERR_NULL_PTR;
	}
	if (*start >= 8 || size == 0 || size > 64) {
		return ERR_INVALID_VALUE;
	}
	if (dst_size == 0) {
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
	if (newArrOffset > dst_size || (newArrOffset == dst_size && newStart > 0)) {
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

#ifdef __cplusplus
}
#endif
