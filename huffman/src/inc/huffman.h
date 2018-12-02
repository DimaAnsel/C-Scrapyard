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
	ERR_NO_ERR,
	ERR_NULL_PTR,
	ERR_INVALID_VALUE,
	ERR_INVALID_SIZE,
	ERR_INVALID_INDEX,
	ERR_INSUFFICIENT_SPACE
} HuffmanError;

#endif // __HUFFMAN_H_

#ifdef __cplusplus
}
#endif
