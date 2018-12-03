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

#endif // __HUFFMAN_H_

#ifdef __cplusplus
}
#endif
