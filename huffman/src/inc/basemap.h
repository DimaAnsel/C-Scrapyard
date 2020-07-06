/**
 * @file basemap.h
 *
 * Interface for {@link basemap.c}.
 *
 * @date	2019-07-02
 * @author	Noah Ansel
 */


#ifdef __cplusplus
extern "C" {
#endif

#ifndef __BASEMAP_H_
/**Include guard.*/
#define __BASEMAP_H_

#include <stdint.h>

// One-hot model
uint64_t one_hot_get_compressed_size(uint64_t, uint64_t, uint8_t);
uint64_t one_hot_get_compressed_val(uint64_t, uint64_t, uint8_t);
HuffmanError one_hot_parse_compressed_idx(uint64_t*, uint8_t**, uint8_t*, uint8_t, uint64_t, uint8_t);

// Fixed-depth tree model
uint64_t fix_depth_tree_get_compressed_size(uint64_t, uint64_t, uint8_t);
uint64_t fix_depth_tree_get_compressed_val(uint64_t, uint64_t, uint8_t);
HuffmanError fix_depth_tree_parse_compressed_idx(uint64_t*, uint8_t**, uint8_t*, uint8_t, uint64_t, uint8_t);

// Log-depth tree model
uint64_t log_depth_tree_get_compressed_size(uint64_t, uint64_t, uint8_t);
uint64_t log_depth_tree_get_compressed_val(uint64_t, uint64_t, uint8_t);
HuffmanError log_depth_tree_parse_compressed_idx(uint64_t*, uint8_t**, uint8_t*, uint8_t, uint64_t, uint8_t);

#endif // __BASEMAP_H_

#ifdef __cplusplus
}
#endif
