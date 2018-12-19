/**
 * @file huffman.h
 *
 * Interface for {@link huffman.c}.
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __HUFFMAN_H_
#define __HUFFMAN_H_

/**
 * Number of bits required for word size in file header.
 */
#define HUFFMAN_WORD_SIZE_NUM_BITS 6

/**
 * Maximum value of {@link HuffmanHeader#uniqueWords}. Note that compression
 * algorithm supports 2^64 words although this constant is set to 2^64 - 1.
 */
#define HUFFMAN_MAX_UNIQUE_WORDS ((uint64_t)0xFFFFFFFFFFFFFFFF)

/**
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
	ERR_INSUFFICIENT_SPACE
} HuffmanError;

/**
 * @file
 * @struct HuffmanHeader
 *
 * Metadata information for compressed data.
 */
typedef struct HuffmanHeader_struct {
	/**
	 * Word size in bits used for compression. Range 2 - 64.
	 *
	 * Stored value is same as actual value.
	 * Value in file is actual value - 1 to fit within 6 bits.
	 */
	uint8_t wordSize;
	/**
	 * Number of bits of padding (0's) added to end of data to end on a
	 * word boundary. Range 0 - 63.
	 *
	 * Stored value is same as actual value.
	 * Value in file is same as actual value.
	 */
	uint8_t padBits;
	/**
	 * Number of unique words in data, including word with
	 * {@link HuffmanHeader#padBits}. Range 0 to 2^64 - 1.
	 *
	 * Stored value is actual value - 1 to fit in uint64_t since actual value
	 * should never equal 0.
	 * Value in file is actual value - 1 to fit within 64 bits.
	 */
	uint64_t uniqueWords;
} HuffmanHeader;

#endif // __HUFFMAN_H_

#ifdef __cplusplus
}
#endif
