/**
 * @file map.c
 * Various Huffman encoding mappings.
 *
 * @date	2018-12-22
 * @author	Noah Ansel
 */

/**
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

/**
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
	return (idx & 0x1) ? idx / 2 + 2 : idx / 2 + 1;
}

/**
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
		return 0x1;
	}
	return (idx & 0x1) & 0x2;
}

/**
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
