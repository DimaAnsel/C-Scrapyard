#include <stdint.h>

// todo remove debug imports
#include <string.h>
#include <stdio.h>

/**
 * Reads a value beginning at an arbitrary position.
 *
 * @param[in,out] arr Pointer to byte from which to read. Updated to first byte of following section.
 * @param[in,out] start Bit from which to start. Updated to bit of following section. Range 0-7.
 * @param[in] size Number of bits to read. Range 1-64.
 *
 * @return Parsed value.
 */
static uint64_t extract_bits(uint8_t** src, uint8_t* start, uint8_t size) {
    if (*start >= 8 || size == 0 || size > 64) {
        printf("invalid start or size\n");
        return 0x0;
    }
    // return value
    uint64_t toRet = 0x0;
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

    if (newStart != 0) {
        if (newArrOffset == 0) { // case I: single byte, non-even end
            // copy & return
            mask = (mask ^ ((0x1 << (8 - newStart))- 1)) & 0xFF;
            toRet = (*tempPtr & mask) >> (8 - newStart); // shift right to end
            return toRet;
        } else { // case III: multi-byte, non-even end
            toRet = (*tempPtr & mask) << shift;
            do {
                shift -= 8;
                tempPtr++;
                toRet |= (*tempPtr & 0xFF) << shift;
            } while (shift > 7);
            // copy end
            mask = (0xFF ^ ((0x1 << (8 - newStart)) - 1)) & 0xFF;
            tempPtr++;
            toRet |= ((*tempPtr) & mask) >> (8 - newStart); // shift right to end
            return toRet;
        }
    } else {
        if (newArrOffset == 1) { // case II: single byte, even end
            // copy & return
            toRet = *tempPtr & mask; // no shift, already in correct location
            return toRet;
        } else { // case IV: multi-byte, even end
            toRet = (*tempPtr & mask) << shift; // first byte
            do {
                shift -= 8;
                tempPtr++;
                toRet |= (*tempPtr & 0xFF) << shift;
            } while (shift > 7);
            return toRet;
        }
    }
}

/**
 * Puts a value into an arbitrary position.
 *
 * @param[in,out] dst Pointer to first byte in which to set data. Updated to first byte of following section.
 * @param[in,out] start Bit from which to start. Updated to first bit of following section. Range 0-7.
 * @param[in] dst_size Number of bytes free in dst.
 * @param[in] size Number of bits to write. Range 1-64.
 * @param[in] val Value to be written.
 */
static void put_bits(uint8_t** dst, uint8_t* start, uint8_t dst_size, uint64_t val, uint8_t size) {
	if (*start >= 8) {
		printf("start bad\n");
	}
	if (*start >= 8 || size == 0 || size > 64 || dst_size == 0) {
	    printf("invalid start or size\n");
	    return;
	}

    // offset to new arr
    uint8_t newArrOffset = (*start + size) / 8;
    // new start bit
    uint8_t newStart = (*start + size) % 8;
    // current shift val; only used in case III,IV
    uint8_t shift = (newArrOffset > 0) ? size - (8 - *start): 0;
    // mask
    uint8_t mask = 0xFF ^ ((1 << (8 - *start)) - 1);

    // verify have space to write data
    if (newArrOffset > dst_size || (newArrOffset == dst_size && newStart > 0)) {
    	return;
    }

    // clip val to bottom [size] bits
    val = val & ((1 << size) - 1);

    // use this for further ops
    uint8_t* tempPtr = *dst;

    // must be done after init mask
    (*dst) += newArrOffset;
    (*start) = newStart;

    if (newArrOffset == 0) {
        // Case I: single byte, non-even end
    	*tempPtr = ((*tempPtr) & mask) | (val << (8 - newStart));
    	return;
    } else if (newArrOffset == 1 && newStart == 0) {
    	// Case II: single byte, even end
    	*tempPtr = ((*tempPtr) & mask) | val;
    	return;
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
}

static void test_put_bits(uint8_t* arr, uint8_t* temp, uint8_t start, uint8_t num, uint64_t val) {
    printf("arr=%d start=%u num=%d val=x%x\n", (int)((void*)arr - (void*)temp), start, num, val);
    put_bits(&arr, &start, 128, val, num);
    printf("arr=%d start=%u\n", (int)((void*)arr - (void*)temp), start);
    printf("contents: [");
    while (temp != arr) {
    	printf("x%x,", *temp);
    	temp++;
    }
    if (start > 0) {
    	printf("x%x,", *temp);
    }
    printf("]\n");
}

int main(void) {

    uint8_t orig[128];
    for (int i = 0; i < 128; i++) {
        orig[i] = 0x55 ^ (uint8_t) i;
    }
    uint8_t* arr, *temp;
    uint8_t start;
    uint8_t num;
    uint64_t val;

//    arr = orig;
//    start = 1;
//    num = 2;
//    printf("arr=%d start=%u num=%u\n", (int)((void*)arr - (void*)orig), start, num);
//    printf("ret=x%x\n", extract_bits(&arr, &start, num));
//    num = 3;
//    printf("arr=%d start=%u num=%u\n", (int)((void*)arr - (void*)orig), start, num);
//    printf("ret=x%x\n", extract_bits(&arr, &start, num));
//    num = 2;
//    printf("arr=%d start=%u num=%u\n", (int)((void*)arr - (void*)orig), start, num);
//    printf("ret=x%x\n", extract_bits(&arr, &start, num));
//    printf("\n");
//
//    num = 4;
//    printf("arr=%d start=%u num=%u\n", (int)((void*)arr - (void*)orig), start, num);
//    printf("ret=x%x\n", extract_bits(&arr, &start, num));
//    printf("arr=%d start=%u num=%u\n", (int)((void*)arr - (void*)orig), start, num);
//    printf("ret=x%x\n", extract_bits(&arr, &start, num));
//    printf("\n");
//
//    num = 5;
//    printf("arr=%d start=%u num=%u\n", (int)((void*)arr - (void*)orig), start, num);
//    printf("ret=x%x\n", extract_bits(&arr, &start, num));
//    num = 3;
//    printf("arr=%d start=%u num=%u\n", (int)((void*)arr - (void*)orig), start, num);
//    printf("ret=x%x\n", extract_bits(&arr, &start, num));
//    printf("\n");
//
//    num = 1;
//    printf("arr=%d start=%u num=%u\n", (int)((void*)arr - (void*)orig), start, num);
//    printf("ret=x%x\n", extract_bits(&arr, &start, num));
//    num = 6;
//    printf("arr=%d start=%u num=%u\n", (int)((void*)arr - (void*)orig), start, num);
//    printf("ret=x%x\n", extract_bits(&arr, &start, num));
//    num = 1;
//    printf("arr=%d start=%u num=%u\n", (int)((void*)arr - (void*)orig), start, num);
//    printf("ret=x%x\n", extract_bits(&arr, &start, num));
//    printf("\n");
//
//    num = 7;
//    printf("arr=%d start=%u num=%u\n", (int)((void*)arr - (void*)orig), start, num);
//    printf("ret=x%x\n", extract_bits(&arr, &start, num));
//    num = 1;
//    printf("arr=%d start=%u num=%u\n", (int)((void*)arr - (void*)orig), start, num);
//    printf("ret=x%x\n", extract_bits(&arr, &start, num));
//    printf("\n");
//
//    printf("arr=%d start=%u num=%u\n", (int)((void*)arr - (void*)orig), start, num);
//    printf("ret=x%x\n", extract_bits(&arr, &start, num));
//    num = 7;
//    printf("arr=%d start=%u num=%u\n", (int)((void*)arr - (void*)orig), start, num);
//    printf("ret=x%x\n", extract_bits(&arr, &start, num));
//    printf("\n");
//
//    num = 3;
//    printf("arr=%d start=%u num=%u\n", (int)((void*)arr - (void*)orig), start, num);
//    printf("ret=x%x\n", extract_bits(&arr, &start, num));
//    num = 24-start;
//    printf("arr=%d start=%u num=%u\n", (int)((void*)arr - (void*)orig), start, num);
//    printf("ret=x%x\n", extract_bits(&arr, &start, num));
//    printf("\n");
//
//    num = 3;
//    printf("arr=%d start=%u num=%u\n", (int)((void*)arr - (void*)orig), start, num);
//    printf("ret=x%x\n", extract_bits(&arr, &start, num));
//    num = 24-start-6;
//    printf("arr=%d start=%u num=%u\n", (int)((void*)arr - (void*)orig), start, num);
//    printf("ret=x%x\n", extract_bits(&arr, &start, num));
//    printf("\n");
//
//    printf("arr=%d start=%u\n", (int)((void*)arr - (void*)orig), start);


//     for (uint8_t i = 1; i <= 16; i++) {
//         printf("test %u\n", i);
//         arr = (uint8_t*)orig + (i / 8);
//         start = i % 8;
//         printf("arr=%d start=%u\n", (int)((void*)arr - (void*)orig), start);
//         printf("ret=%x\n", extract_bits(&arr, &start, i));
//         printf("arr=%d start=%u\n", (int)((void*)arr - (void*)orig), start);
//     }

    memset(orig, 0x00, 128);

    temp = arr = orig;
    start = 0;
    val = 0x55;
    num = 8;
    test_put_bits(arr, temp, start, num, val);

    temp = arr = orig;
    start = 0;
    val = 0xAAA;
    num = 12;
    test_put_bits(arr, temp, start, num, val);

    temp = arr = orig;
    start = 4;
    val = 0x555;
    num = 12;
    test_put_bits(arr, temp, start, num, val);

    memset(orig, 0x00, 4);
    temp = arr = orig;
    start = 2;
    val = 0x7FF;
    num = 11;
    test_put_bits(arr, temp, start, num, val);



    return 0;
}

