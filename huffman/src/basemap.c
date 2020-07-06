/**
 * @file basemap.c
 * Various Huffman encoding mappings.
 *
 * @date	2018-12-22
 * @author	Noah Ansel
 */

#ifdef __cplusplus
extern "C" {
#endif

// Includes
#include "huffman.h"
#include "basemap.h"

/////////// helpers

/**
 * Gets ceiling of uint64 division.
 */
static uint64_t div_ceil_u64(uint64_t dividend, uint64_t divisor) {
	if (dividend % divisor != 0) {
		return 1 + (dividend / divisor);
	}
	return dividend / divisor;
}

////////////////////////////////////////////////////////////////
///
/// @defgroup HuffmanBaseMaps Huffman Basic Mapping Functions
/// Basic mapping functions used for Huffman compression.
///
////////////////////////////////////////////////////////////////

/**
 * @ingroup HuffmanBaseMaps
 * Mapping table for one-hot encoding.
 */
HuffmanCompressor OneHot = {
	getSize: one_hot_get_compressed_size,
	getVal: one_hot_get_compressed_val,
	parseIdx: one_hot_parse_compressed_idx
};

/**
 * @ingroup HuffmanBaseMaps
 * Determines number of bits needed for given word using one-hot encoding
 * mapping.
 *
 * @param[in] idx    Index of word in frequency table (0 being most frequent).
 * @param[in] maxIdx Total number of unique words. Actual value is maxIdx + 1.
 * @param[in] depth  Unused.
 *
 * @return Number of bits required for given word.
 */
uint64_t one_hot_get_compressed_size(uint64_t idx, uint64_t maxIdx, uint8_t depth) {
	return idx + 1;
}

/**
 * @ingroup HuffmanBaseMaps
 * Determines value for given word using one-hot encoding mapping.
 *
 * @param[in] idx    Index of word in frequency table (0 being most frequent).
 * @param[in] maxIdx Total number of unique words. Actual value is maxIdx + 1.
 * @param[in] depth  Unused.
 *
 * @return Value of word using one-hot encoding (always 0x1).
 */
uint64_t one_hot_get_compressed_val(uint64_t idx,
									uint64_t maxIdx,
									uint8_t depth) {
	return (uint64_t)0x1;
}

HuffmanError one_hot_parse_compressed_idx(uint64_t* dst,
										  uint8_t** src,
										  uint8_t* start,
										  uint8_t size,
										  uint64_t maxIdx,
										  uint8_t depth) {
	return ERR_NO_ERR;
}

/**
 * @ingroup HuffmanBaseMaps
 * Determines number of bits needed for given word using fixed-depth tree
 * encoding mapping.
 *
 * @param[in] idx    Index of word in frequency table (0 being most frequent).
 * @param[in] maxIdx Total number of unique words.. Actual value is maxIdx + 1
 * @param[in] depth  Depth of left branches of tree.
 *
 * @return Number of bits required for given word.
 */
uint64_t fix_depth_tree_get_compressed_size(uint64_t idx,
											uint64_t maxIdx,
											uint8_t depth) {
	if (idx == 0) {
		return 1;
	}
	static uint8_t cached_depth = depth;
	static uint64_t pow2 = ((uint64_t) 1) << ((uint64_t) depth); // 2^k
	if (cached_depth != depth) { // TODO see if caching actually helps here
		cached_depth = depth;
		pow2 = ((uint64_t) 1) << ((uint64_t) depth);
	}
	return 1 + depth + div_ceil_u64(idx, pow2);
}

/**
 * @ingroup HuffmanBaseMaps
 * Determines value for given word using fixed-depth tree encoding mapping.
 *
 * @param[in] idx    Index of word in frequency table (0 being most frequent).
 * @param[in] maxIdx Total number of unique words. Actual value is maxIdx + 1.
 * @param[in] depth  Depth of left branches of tree.
 *
 * @return Value of word using fixed-depth tree encoding.
 */
uint64_t fix_depth_tree_get_compressed_val(uint64_t idx,
										   uint64_t maxIdx,
										   uint8_t depth) {
	if (idx == 0) {
		return 1;
	}
	static uint8_t cached_depth = depth;
	static uint64_t pow2 = ((uint64_t) 1) << ((uint64_t) depth); // 2^k
	if (cached_depth != depth) { // TODO see if caching actually helps here
		cached_depth = depth;
		pow2 = ((uint64_t) 1) << ((uint64_t) depth);
	}
	if (idx % pow2 == 0) {
		return pow2;
	}
	return pow2 * 2 - (idx % pow2); // 2^(k+1) - (i % (2^k))
}

/**
 * @ingroup HuffmanBaseMaps
 * Determines value for given word using log-depth tree encoding mapping.
 *
 * @param[in] idx    Index of word in frequency table (0 being most frequent).
 * @param[in] maxIdx Total number of unique words. Actual value is maxIdx + 1.
 * @param[in] depth  Unused.
 *
 * @return Value of word using log-depth tree encoding.
 */
uint64_t log_depth_tree_get_compressed_size(uint64_t idx,
											uint64_t maxIdx,
											uint8_t depth) {
	if (idx == 0) {
		return 0x1;
	}
	return 0;
}

/**
 * @ingroup HuffmanBaseMaps
 * Determines number of bits needed for given word using log-depth tree
 * encoding mapping.
 *
 * @param[in] idx    Index of word in frequency table (0 being most frequent).
 * @param[in] maxIdx Total number of unique words. Actual value is maxIdx + 1.
 * @param[in] depth  Unused.
 *
 * @return Value of word using log-depth tree encoding.
 */
uint64_t log_depth_tree_get_compressed_val(uint64_t idx,
										   uint64_t maxIdx,
										   uint8_t depth) {
	if (idx == 0) {
		return 0x1;
	}
	return 0;
}

#ifdef __cplusplus
}
#endif
