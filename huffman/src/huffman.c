#include <stdint.h>

/**
 * Reads a value beginning at an arbitrary position.
 *
 * @param[in,out] arr Pointer to byte from which to read. Updated to first byte of following section.
 * @param[in,out] start Bit from which to start. Updated to bit of following section. Range 0-7.
 * @param[in] size Number of bits to read. Range 1-64.
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
    uint8_t* tempPtr = *arr;

    // must be done after init mask
    (*arr) += newArrOffset;
    (*start) = newStart;

    if (newStart != 0) {
        if (newArrOffset == 0) { // case I: single byte, non-even end
            // copy & return
            mask = (mask ^ ((0x1 << (8 - newStart))- 1)) & 0xFF;
            toRet = (*tempPtr & mask) >> (8 - newStart); // shift right to end
            return toRet;
        } else { // case III: multi-byte, non-even end
            // shift += newStart;
            toRet = (*tempPtr & mask) << shift;
            do {
                shift -= 8;
                tempPtr++;
                toRet |= (*tempPtr & 0xFF) << shift;
            } while (shift > 7);
            mask = (0xFF ^ ((0x1 << (8 - newStart)) - 1)) & 0xFF;
            tempPtr++;
            toRet |= ((*tempPtr) & mask) >> (8 - newStart); // shift right to end
            return toRet; // TODO fix this
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

int main(void) {

    uint8_t orig[128];
    for (int i = 0; i < 128; i++) {
        orig[i] = 0x55 ^ (uint8_t) i;
    }
    uint8_t* arr;
    uint8_t start;
    uint8_t num;

    arr = orig;
    start = 1;
    num = 2;
    printf("arr=%d start=%u num=%u\n", (int)((void*)arr - (void*)orig), start, num);
    printf("ret=x%x\n", get_value(&arr, &start, num));
    num = 3;
    printf("arr=%d start=%u num=%u\n", (int)((void*)arr - (void*)orig), start, num);
    printf("ret=x%x\n", get_value(&arr, &start, num));
    num = 2;
    printf("arr=%d start=%u num=%u\n", (int)((void*)arr - (void*)orig), start, num);
    printf("ret=x%x\n", get_value(&arr, &start, num));
    printf("\n");

    num = 4;
    printf("arr=%d start=%u num=%u\n", (int)((void*)arr - (void*)orig), start, num);
    printf("ret=x%x\n", get_value(&arr, &start, num));
    printf("arr=%d start=%u num=%u\n", (int)((void*)arr - (void*)orig), start, num);
    printf("ret=x%x\n", get_value(&arr, &start, num));
    printf("\n");

    num = 5;
    printf("arr=%d start=%u num=%u\n", (int)((void*)arr - (void*)orig), start, num);
    printf("ret=x%x\n", get_value(&arr, &start, num));
    num = 3;
    printf("arr=%d start=%u num=%u\n", (int)((void*)arr - (void*)orig), start, num);
    printf("ret=x%x\n", get_value(&arr, &start, num));
    printf("\n");

    num = 1;
    printf("arr=%d start=%u num=%u\n", (int)((void*)arr - (void*)orig), start, num);
    printf("ret=x%x\n", get_value(&arr, &start, num));
    num = 6;
    printf("arr=%d start=%u num=%u\n", (int)((void*)arr - (void*)orig), start, num);
    printf("ret=x%x\n", get_value(&arr, &start, num));
    num = 1;
    printf("arr=%d start=%u num=%u\n", (int)((void*)arr - (void*)orig), start, num);
    printf("ret=x%x\n", get_value(&arr, &start, num));
    printf("\n");

    num = 7;
    printf("arr=%d start=%u num=%u\n", (int)((void*)arr - (void*)orig), start, num);
    printf("ret=x%x\n", get_value(&arr, &start, num));
    num = 1;
    printf("arr=%d start=%u num=%u\n", (int)((void*)arr - (void*)orig), start, num);
    printf("ret=x%x\n", get_value(&arr, &start, num));
    printf("\n");

    printf("arr=%d start=%u num=%u\n", (int)((void*)arr - (void*)orig), start, num);
    printf("ret=x%x\n", get_value(&arr, &start, num));
    num = 7;
    printf("arr=%d start=%u num=%u\n", (int)((void*)arr - (void*)orig), start, num);
    printf("ret=x%x\n", get_value(&arr, &start, num));
    printf("\n");

    num = 3;
    printf("arr=%d start=%u num=%u\n", (int)((void*)arr - (void*)orig), start, num);
    printf("ret=x%x\n", get_value(&arr, &start, num));
    num = 24-start;
    printf("arr=%d start=%u num=%u\n", (int)((void*)arr - (void*)orig), start, num);
    printf("ret=x%x\n", get_value(&arr, &start, num));
    printf("\n");

    num = 3;
    printf("arr=%d start=%u num=%u\n", (int)((void*)arr - (void*)orig), start, num);
    printf("ret=x%x\n", get_value(&arr, &start, num));
    num = 24-start-6;
    printf("arr=%d start=%u num=%u\n", (int)((void*)arr - (void*)orig), start, num);
    printf("ret=x%x\n", get_value(&arr, &start, num));
    printf("\n");

    printf("arr=%d start=%u\n", (int)((void*)arr - (void*)orig), start);


    // for (uint8_t i = 1; i <= 16; i++) {
    //     printf("test %u\n", i);
    //     arr = (uint8_t*)orig + (i / 8);
    //     start = i % 8;
    //     printf("arr=%d start=%u\n", (int)((void*)arr - (void*)orig), start);
    //     printf("ret=%x\n", get_value(&arr, &start, i));
    //     printf("arr=%d start=%u\n", (int)((void*)arr - (void*)orig), start);
    // }

    return 0;
}

