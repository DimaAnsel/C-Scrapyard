/**
 * @file huffman.h
 *
 * Interface for {@link huffman.c}.
 *
 * @date	2018-12-22
 * @author	Noah Ansel
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __HUFFMAN_H_
/**Include guard.*/
#define __HUFFMAN_H_

// Includes
#include <stdint.h>

////////////////////////////////////////////////////////////////
///
/// @defgroup HuffmanConstants Huffman Constants
/// Constants for Huffman compression framework in {@link huffman.c}.
///
////////////////////////////////////////////////////////////////

/**
 * @ingroup HuffmanConstants
 * Minimum supported word size for Huffman encoding.
 */
#define HUFFMAN_MIN_WORD_SIZE 2

/**
 * @ingroup HuffmanConstants
 * Maximum supported word size for Huffman encoding.
 */
#define HUFFMAN_MAX_WORD_SIZE 60

/**
 * @ingroup HuffmanConstants
 * Number of bits required for word size in file header.
 */
#define HUFFMAN_WORD_SIZE_NUM_BITS 6

/**
 * @ingroup HuffmanConstants
 * Maximum value stored in uint64_t type, used for overflow checking.
 */
#define HUFFMAN_MAX_UINT64 ((uint64_t)0xFFFFFFFFFFFFFFFF)

/**
 * @ingroup HuffmanConstants
 * @enum HuffmanError
 * Return codes for functions in {@link huffman.c}.
 */
typedef enum HuffmanError_enum {
	/**No error occurred.*/
	ERR_NO_ERR,
	/**Function received null parameter/value when expecting non-null.*/
	ERR_NULL_PTR,
	/**Function received an invalid parameter value.*/
	ERR_INVALID_VALUE,
	/**The destination does not contain enough space to store function result.*/
	ERR_INSUFFICIENT_SPACE,
	/**The compressed source contained invalid data.*/
	ERR_INVALID_DATA,
	/**
	 * Counter overflowed.
	 * This can occur if source contains too many copies of a given word.
	 */
	ERR_OVERFLOW
} HuffmanError;

/**
 * @struct HuffmanHeader
 * Metadata information for compressed data.
 */
typedef struct HuffmanHeader_struct {
	/**
	 * Word size in bits used for compression. Range 2 - 60.
	 *
	 * Stored value is same as actual value.
	 * Value in file is same as actual value.
	 */
	uint8_t wordSize;
	/**
	 * Number of bits of padding (0's) added to end of data to end on a
	 * word boundary. Range 0 - 59.
	 *
	 * Stored value is same as actual value.
	 * Value in file is same as actual value.
	 */
	uint8_t padBits;
	/**
	 * Number of unique words in data, including word with
	 * {@link HuffmanHeader#padBits}. Range 1 to 2^60.
	 *
	 * Stored value is same as actual value.
	 * Value in file is actual value - 1 to fit within 60 bits.
	 */
	uint64_t uniqueWords;
} HuffmanHeader;

typedef struct HuffmanHashTable_struct {
	/**
	 * Maximum capacity of this table.
	 */
	uint64_t size;
	/**
	 * Pointer to table.
	 */
	uint64_t* table;
} HuffmanHashTable;

/**
 * Standard interface to get size of value in bits for index
 * using a given mapping.
 *
 * @see basemap.c
 */
typedef uint64_t (*get_compressed_size_fcn) (uint64_t idx,
											 uint64_t maxIdx,
											 uint8_t depth);

/**
 * Standard interface to get value for index using a given mapping.
 *
 * @see basemap.c
 */
typedef uint64_t (*get_compressed_val_fcn) (uint64_t idx,
											uint64_t maxIdx,
											uint8_t depth);

/**
 * Standard interface to get index referenced by compressed value
 * using a given mapping. Also updates
 *
 * @see basemap.c
 */
typedef HuffmanError (*parse_compressed_idx_fcn) (uint64_t* dst,
												  uint8_t** src,
												  uint8_t* start,
												  uint8_t size,
												  uint64_t maxIdx,
												  uint8_t depth);

#endif // __HUFFMAN_H_

#ifdef __cplusplus
}
#endif
