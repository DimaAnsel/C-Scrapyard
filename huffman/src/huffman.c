/**
 * @file huffman.c
 *
 * Compact implementation of Huffman coding. Supports word sizes of 2 - 60 bits
 * and file sizes up to ~16,000,000 TB (although that will probably take a while
 * to process).
 *
 * @date	2018-12-22
 * @author	Noah Ansel
 */

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
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
 * @param[in,out] dstSize Number of bytes free in dst. Updated to remaining number of bytes on success.
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
 * @param[in,out] dstSize Number of bytes free in dst. Updated to remaining number of bytes on success.
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
	uint64_t maxWords = (((uint64_t)0x1) << header->wordSize);
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
	THROW_ERR(put_bits(&currDst, &currBit, &newSize, (uint64_t)header->wordSize, (uint8_t)HUFFMAN_WORD_SIZE_NUM_BITS))
	// pad bits
	THROW_ERR(put_bits(&currDst, &currBit, &newSize, (uint64_t)header->padBits, log2wordSize))
	// unique words
	THROW_ERR(put_bits(&currDst, &currBit, &newSize, header->uniqueWords - 1, header->wordSize))

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
 * @param[in,out] srcSize Length of src in bytes. Updated to remaining number of bytes on success.
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
	if (header == NULL || src == NULL || *src == NULL || start == NULL || srcSize == NULL) {
		return ERR_NULL_PTR;
	}
	if (*srcSize < 2) { // require minimum 6 + 1 + 2 bits = 1 byte + 1 bit
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
	if (temp < HUFFMAN_MIN_WORD_SIZE || temp > HUFFMAN_MAX_WORD_SIZE) {
		return ERR_INVALID_DATA;
	}

	// apply offset to obtain actual value
	header->wordSize = (uint8_t)temp;

	// Ensure enough space is available
	uint8_t log2wordSize = log2_ceil_u8(header->wordSize);
	// Number of bits required to store header
	uint8_t reqBits = HUFFMAN_WORD_SIZE_NUM_BITS + log2wordSize + header->wordSize;
	uint8_t reqBytes = reqBits / 8;
	if (*srcSize < reqBytes || (*srcSize == reqBytes && (reqBits % 8) > 0)) {
		return ERR_INSUFFICIENT_SPACE;
	}

	// Parse padBits
	size = log2_ceil_u8(header->wordSize);
	THROW_ERR(extract_bits(&temp, &tempPtr, &tempStart, size))
	// Ensure padBits in valid range
	if (temp >= (uint64_t)header->wordSize) {
		return ERR_INVALID_DATA;
	}
	header->padBits = (uint8_t)temp;

	// parse uniqueWords
	size = header->wordSize;
	THROW_ERR(extract_bits(&temp, &tempPtr, &tempStart, size))
	header->uniqueWords = temp + 1;

	// success, update val
	(*src) = tempPtr;
	(*start) = tempStart;
	(*srcSize) -= reqBytes;

	return ERR_NO_ERR;
}

/**
 * @ingroup HuffmanHelpers
 * Obtains pointer to location of table value.
 *
 * @param[in] table Reference to table.
 * @param[in] idx   Index of value to be retrieved.
 *
 * @return Pointer to value at provided index.
 */
static inline uint64_t* get_table_value(uint64_t* table,
										uint64_t idx) {
	return &table[2 * idx];
}

/**
 * @ingroup HuffmanHelpers
 * Obtains pointer to location of table id.
 *
 * @param[in] table Reference to table.
 * @param[in] idx   Index of id to be retrieved.
 *
 * @return Pointer to id at provided index.
 */
static inline uint64_t* get_table_id(uint64_t* table,
								 	 uint64_t idx) {
	return &table[2 * idx + 1];
}

/**
 * @ingroup HuffmanHelpers
 * Hashing function for table used in {@link generate_table}.
 *
 * @param[in] val    Value to be hashed
 * @param[in] maxVal Maximum value (table size).
 *
 * @return Hashing function result.
 */
static inline uint64_t get_hash(uint64_t val,
								uint64_t maxVal) {
	// todo implement variable FNV with folding
	return val % maxVal;
}

/**
 * @ingroup HuffmanHelpers
 * Searches hash table for entry that is empty or matches desired id.
 *
 * @param[out] dstIdx        Matching or unoccupied index.
 * @param[in]  table         Table to be searched.
 * @param[in]  searchId      Desired id.
 * @param[in]  assumeNoMatch If true, algorithm assumes table does not contain
 *							 matching id (for example, when resizing table).
 *
 * @return {@link ERR_NO_ERR} if no error occurred.\n
 *		   {@link ERR_NULL_PTR} if dstIdx or table are null.
 *		   {@link ERR_INSUFFICIENT_SPACE} if table is full and no entry with
 *				matching value was found. Indicates a larger table is needed.
 */
static HuffmanError search_table(uint64_t* dstIdx,
								 HuffmanHashTable* table,
								 uint64_t searchId,
								 bool assumeNoMatch) {
	if (dstIdx == NULL || table == NULL || table->table == NULL) {
		return ERR_NULL_PTR;
	}

	uint64_t* id, *val;
	uint64_t curr = get_hash(searchId, table->size);
	uint64_t last = (curr + (table->size - 1)) % table->size;

	// todo analyze hash & step function performance
	while (curr != last) {
		// Check for empty
		val = get_table_value(table->table, curr);
		if (*val == 0) {
			*dstIdx = curr;
			return ERR_NO_ERR;
		}

		// Check for identical id
		if (!assumeNoMatch) { // todo measure effect of skipping this step during resize
			id = get_table_id(table->table, curr);
			if (*id == searchId) {
				*dstIdx = curr;
				return ERR_NO_ERR;
			}
		}

		// Not found, move to next index
		curr = (curr + 1) % table->size; // todo consider different step functions
	}
	// Table full and id not found
	return ERR_INSUFFICIENT_SPACE;
}

/**
 * @ingroup HuffmanHelpers
 * Attempts to resize a table to a new, larger size.
 *
 * @warning Must be able to allocate new table prior to releasing existing table.
 *
 * @param[in,out] table     Table to be resized. Updated with new table
 *							pointer & size if successful.
 * @param[in]     newSize   Maximum number of entries in resized table.
 *
 * @return {@link ERR_NO_ERR} if no error occurred.\n
 *		   {@link ERR_NULL_PTR} if table is null or points to null.\n
 *		   {@link ERR_INVALID_VALUE} if sizes are 0 or new table size is less
 *				than existing table size.
 *		   {@link ERR_INSUFFICIENT_SPACE} if unable to allocate new table.
 */
static HuffmanError resize_table(HuffmanHashTable* table, uint64_t newSize) {
	if (table == NULL || table->table == NULL) {
		return ERR_NULL_PTR;
	}

	if (table->size == 0 || newSize == 0 || newSize <= table->size) {
		return ERR_INVALID_VALUE;
	}

	// Init pointers and allocate
	uint64_t* oldTable = table->table;
	HuffmanHashTable newTable;
	newTable.size = newSize;
	newTable.table = (uint64_t*) malloc(2 * sizeof(uint64_t) * newSize);
	if (!newTable.table) {
		return ERR_INSUFFICIENT_SPACE;
	}
	memset(newTable.table, 0x00, 2 * sizeof(uint64_t) * newTable.size);

	uint64_t currIdx, dstIdx;
	uint64_t val, id;
	uint64_t *dstVal, *dstId;
	HuffmanError err;

	for (currIdx = 0; currIdx < table->size; currIdx++) {
		val = *get_table_value(oldTable, currIdx);

		// Entry found
		if (val) {
			id = *get_table_id(oldTable, currIdx);

			err = search_table(&dstIdx, &newTable, id, true);
			if (err) {
				// Should be unreachable
				free(newTable.table);
				return err;
			}

			// Copy entry
			dstVal = get_table_value(newTable.table, dstIdx);
			dstId  = get_table_id(newTable.table, dstIdx);
			*dstVal = val;
			*dstId = id;
		}
	}

	// Release and migrate pointer
	free(oldTable);
	table->table = newTable.table;
	table->size = newTable.size;
	return ERR_NO_ERR;
}

/**
 * @ingroup HuffmanHelpers
 * Adds a word to the table, or increments if already in table. Also handles
 * resizing table.
 *
 * @see generate_table
 *
 * @param[in,out] table    Table to be updated
 * @param[out]    numWords Number of words in table
 * @param[in]     word     Word to be added/incremented
 * @param[in]     maxSize  Maximum size of table
 *
 * @return {@link ERR_NO_ERR} if no error occurred.\n
 *		   {@link ERR_NULL_PTR} if table or numWords are null.\n
 *         {@link ERR_INSUFFICIENT_SPACE} if unable to allocate
 *      		sufficient memory for table.\n
 *         {@link ERR_OVERFLOW} if more than {@link HUFFMAN_MAX_UINT64}
 *      		of the same word are found.\n
 *         Other errors as raised by {@link extract_bits}.
 */
static HuffmanError add_to_table(HuffmanHashTable* table,
								 uint64_t* numWords,
								 uint64_t word,
								 uint64_t maxSize) {
	if (table == NULL || table->table == NULL || numWords == NULL) {
		return ERR_NULL_PTR;
	}

	if (table->size == 0 || maxSize < table->size) {
		return ERR_INVALID_VALUE;
	}

	HuffmanError err;
	uint64_t idx;
	uint64_t *dstVal, *dstId;

	// Find in hash table
	err = search_table(&idx, table, word, false);

	// Check for full table, resize if necessary
	if (err == ERR_INSUFFICIENT_SPACE) {
		if (table->size < maxSize) {
			// Resize table
			uint64_t newSize = (table->size * 2 <= maxSize) ? table->size * 2 : maxSize;
			err = resize_table(table, newSize);
			if (err) {
				// Error occurred in resizing
				return err;
			}

			// Find in new hash table
			err = search_table(&idx, table, word, false);
			if (err) {
				// Should be unreachable
				return err;
			}
		} else {
			// Cannot resize table
			return ERR_INSUFFICIENT_SPACE;
		}
	} else if (err != ERR_NO_ERR) {
		// Should be unreachable
		return err;
	}

	// Add to table, check for value overflow
	dstVal = get_table_value(table->table, idx);
	dstId = get_table_id(table->table, idx);
	if (*dstVal == HUFFMAN_MAX_UINT64) {
		return ERR_OVERFLOW;
	} else if (*dstVal == 0) {
		if (*numWords == HUFFMAN_MAX_UINT64) {
			return ERR_OVERFLOW;
		}
		(*numWords)++;
		*dstId = word;
	}
	*dstVal = *dstVal + 1;

	return ERR_NO_ERR;
}

/**
 * @ingroup HuffmanHelpers
 * Generates and populates hash table of word frequencies.
 *
 * @warning This allocates a table that must be freed later.
 *
 * @param[out] hdr      Header populated with metadata.
 * @param[out] dst      Pointer to table. Table data must be freed by calling function.
 * @param[in]  src      Data to be converted.
 * @param[in]  srcSize  Size of data in bytes.
 * @param[in]  wordSize Word size used for compression.
 *
 * @return {@link ERR_NO_ERR} if no error occurred.\n
 *         {@link ERR_NULL_PTR} if a parameter is null.\n
 *         {@link ERR_INVALID_VALUE} if srcSize or wordSize are out of accepted range.\n
 *         {@link ERR_INSUFFICIENT_SPACE} if unable to allocate
 *      		sufficient memory for table.\n
 *         {@link ERR_OVERFLOW} if more than {@link HUFFMAN_MAX_UINT64}
 *      		of the same word are found.\n
 *         Other errors as raised by {@link extract_bits}.
 */
static HuffmanError generate_table(HuffmanHeader* hdr,
								   HuffmanHashTable* dst,
								   uint8_t* src,
								   uint64_t srcSize,
								   uint8_t wordSize) {
	if (hdr == NULL || dst == NULL || src == NULL) {
		return ERR_NULL_PTR;
	}

	if (srcSize < 1 ||
			wordSize < HUFFMAN_MIN_WORD_SIZE ||
			wordSize > HUFFMAN_MAX_WORD_SIZE) {
		return ERR_INVALID_VALUE;
	}

	HuffmanHashTable table;
	uint8_t* currPtr = src;
	uint8_t  currBit = 0;
	uint8_t* maxPtr;
	uint64_t numWords = 0;
	uint64_t currWord;
	HuffmanError err;

	// Determine how many bits in last word (complicated formula to avoid int overflow)
	uint8_t finalBits = (uint8_t) ((uint64_t) 8 * (srcSize % (uint64_t) wordSize)
			% (uint64_t) wordSize);
	uint8_t padBits = wordSize - finalBits;

	// Set maximum pointer
	if (finalBits == 0) {
		maxPtr = &src[srcSize];
	} else {
		maxPtr = &src[srcSize - ((finalBits + 7) / 8)];
	}

	// Max size range 16 to 16 * 2^59 bytes
	// NOTE: fails if wordSize = 60 and 2^60 unique words found
	uint64_t maxSize = (wordSize < 59) ? ((uint64_t)1) << wordSize : ((uint64_t)1) << 59;

	// Table initially 1/256th to all of max size, depending on word size
	table.size = 1 << (wordSize - wordSize / 4);

	// Initialize table
	table.table = (uint64_t*) malloc(2 * sizeof(uint64_t) * table.size);
	if (!table.table) {
		return ERR_INSUFFICIENT_SPACE;
	}
	memset(table.table, 0x0, 2 * sizeof(uint64_t) * table.size);

	// Parse all complete words in file
	while (currPtr != maxPtr) {
		// Get next word
		err = extract_bits(&currWord, &currPtr, &currBit, wordSize);
		if (err != ERR_NO_ERR) {
			free(table.table);
			return err;
		}

		err = add_to_table(&table, &numWords, currWord, maxSize);
		if (err) {
			free(table.table);
			return err;
		}
	}

	// Handle incomplete word & padding
	if (finalBits != 0) {
		uint64_t highIdx, lowIdx;
		uint64_t *highVal, *lowVal;
		uint64_t *id;
		uint64_t highWord;
		bool skipLow = false, skipHigh = false;

		// Get next word
		err = extract_bits(&currWord, &currPtr, &currBit, finalBits);
		if (err != ERR_NO_ERR) {
			free(table.table);
			return err;
		}
		currWord = currWord << padBits;

		// Find index if padding with 0's
		err = search_table(&lowIdx, &table, currWord, false);
		if (err) { // Full table & not in table
			skipLow = true;
		} else {
			lowVal = get_table_value(table.table, lowIdx);
		}

		// Find index if padding with 1's
		highWord = currWord | (1 << padBits) - 1;
		err = search_table(&highIdx, &table, highWord, false);
		if (err) { // Full table & not in table
			skipHigh = true;
		} else {
			highVal = get_table_value(table.table, highIdx);
		}

		// Choose which padding to use:
		// * If both possible, choose most common one (or lower in case of tie)
		// * If one possible, choose that one
		// * If none possible, choose lower (must resize table)
		if (skipHigh || *lowVal >= *highVal) {
			err = add_to_table(&table, &numWords, currWord, maxSize);
		} else {
			err = add_to_table(&table, &numWords, highWord, maxSize);
		}

		// Check for error
		if (err) {
			free(table.table);
			return err;
		}

	}

	// Update header
	hdr->wordSize = wordSize;
	hdr->padBits = padBits;
	hdr->uniqueWords = numWords;
	// Copy table metadata
	dst->table = table.table;
	dst->size = table.size;

	return ERR_NO_ERR;
}

#ifdef __cplusplus
}
#endif
