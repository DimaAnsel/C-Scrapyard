/**
 * @file huffman_test.cc
 *
 * Unit tests to verify functionality of {@link huffman.c}.
 * 
 * @date    2017-08-23
 * @author  Noah Ansel
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "../src/inc/huffman.h"
#include "../src/huffman.c"
#include "gtest/gtest.h"

/**
 * Capacity of hash table used in unit tests.
 */
#define TEST_TABLE_SIZE (uint64_t) 20

/**
 * Small volume data size in bytes.
 */
#define HUFFMAN_TEST_SMALL_VOLUME (uint64_t) (1024)

/**
 * Medium volume data size in bytes.
 */
#define HUFFMAN_TEST_MEDIUM_VOLUME (uint64_t) (1048576)

/**
 * Large volume data size in bytes.
 */
#define HUFFMAN_TEST_LARGE_VOLUME (uint64_t) (1048576 * 20)

/**
 * Test for Huffman coding implementation.
 */
class HuffmanTest: public ::testing::Test {
protected:
	/**
	 * @todo document
	 */
	virtual void SetUp() {
	}
};

/**
 * Tests input validation for {@link extract_bits}.
 */
TEST_F(HuffmanTest, extract_bits_errs) {
	uint8_t testArr[16];
	uint8_t* testPtr = testArr;
	uint8_t* nullTest = NULL;
	uint8_t size = 3;
	uint8_t start = 3;
	uint64_t result = 0;

	// null checks
	EXPECT_EQ(ERR_NULL_PTR, extract_bits(NULL, &testPtr, &start, size));
	EXPECT_EQ(ERR_NULL_PTR, extract_bits(&result, NULL, &start, size));
	EXPECT_EQ(ERR_NULL_PTR, extract_bits(&result, &nullTest, &start, size));
	EXPECT_EQ(ERR_NULL_PTR, extract_bits(&result, &nullTest, NULL, size));

	// input validation
	EXPECT_EQ(ERR_INVALID_VALUE, extract_bits(&result, &testPtr, &start, 0));
	EXPECT_EQ(ERR_INVALID_VALUE, extract_bits(&result, &testPtr, &start, 65));
	start = 8;
	EXPECT_EQ(ERR_INVALID_VALUE, extract_bits(&result, &testPtr, &start, 65));
}

/**
 * Validates output of {@link extract_bits} for case 1 (single byte, non-even end).
 */
TEST_F(HuffmanTest, extract_bits_case1) {
	uint8_t testArr[16];
	uint8_t* testPtr;
	uint8_t size;
	uint8_t start;
	uint64_t result = 0;

	for (uint8_t i = 0; i < 16; i++) {
		testArr[i] = (((0x55 ^ i) & 0xF) << 4) | (i & 0xF);
	}

	// start 0
	testPtr = testArr;
	start = 0;
	size = 3;
	EXPECT_EQ(ERR_NO_ERR, extract_bits(&result, &testPtr, &start, size));
	EXPECT_EQ(0x2, result);
	EXPECT_EQ(testArr, testPtr);
	EXPECT_EQ(3, start);

	// start 1
	testPtr = testArr + 1;
	start = 1;
	size = 6;
	EXPECT_EQ(ERR_NO_ERR, extract_bits(&result, &testPtr, &start, size));
	EXPECT_EQ(0x20, result);
	EXPECT_EQ(testArr + 1, testPtr);
	EXPECT_EQ(7, start);

	// start 2
	testPtr = testArr + 2;
	start = 2;
	size = 4;
	EXPECT_EQ(ERR_NO_ERR, extract_bits(&result, &testPtr, &start, size));
	EXPECT_EQ(0xC, result);
	EXPECT_EQ(testArr + 2, testPtr);
	EXPECT_EQ(6, start);

	// start 3
	testPtr = testArr + 2;
	start = 3;
	size = 1;
	EXPECT_EQ(ERR_NO_ERR, extract_bits(&result, &testPtr, &start, size));
	EXPECT_EQ(0x1, result);
	EXPECT_EQ(testArr + 2, testPtr);
	EXPECT_EQ(4, start);

	// start 4
	testPtr = testArr + 4;
	start = 4;
	size = 3;
	EXPECT_EQ(ERR_NO_ERR, extract_bits(&result, &testPtr, &start, size));
	EXPECT_EQ(0x2, result);
	EXPECT_EQ(testArr + 4, testPtr);
	EXPECT_EQ(7, start);

	// start 5
	testPtr = testArr + 5;
	start = 5;
	size = 1;
	EXPECT_EQ(ERR_NO_ERR, extract_bits(&result, &testPtr, &start, size));
	EXPECT_EQ(0x1, result);
	EXPECT_EQ(testArr + 5, testPtr);
	EXPECT_EQ(6, start);

	// start 6
	testPtr = testArr + 4;
	start = 6;
	size = 1;
	EXPECT_EQ(ERR_NO_ERR, extract_bits(&result, &testPtr, &start, size));
	EXPECT_EQ(0x0, result);
	EXPECT_EQ(testArr + 4, testPtr);
	EXPECT_EQ(7, start);
}

/**
 * Validates output of {@link extract_bits} for case 2 (single byte, even end).
 */
TEST_F(HuffmanTest, extract_bits_case2) {
	uint8_t testArr[16];
	uint8_t* testPtr;
	uint8_t size;
	uint8_t start;
	uint64_t result = 0;
	uint8_t i;

	for (i = 0; i < 16; i++) {
		testArr[i] = (((0x55 ^ i) & 0xF) << 4) | (i & 0xF);
	}

	for (i = 0; i < 8; i++) {
		// start 0
		testPtr = testArr + i;
		start = 0;
		size = 8;
		EXPECT_EQ(ERR_NO_ERR, extract_bits(&result, &testPtr, &start, size));
		EXPECT_EQ(testArr[i], result);
		EXPECT_EQ(testArr + i + 1, testPtr);
		EXPECT_EQ(0, start);
	}
}

/**
 * Validates output of {@link extract_bits} for case 3 (multi-byte, non-even end).
 */
TEST_F(HuffmanTest, extract_bits_case3) {
	uint8_t testArr[16];
	uint8_t* testPtr;
	uint8_t size;
	uint8_t start;
	uint64_t result = 0;

	for (uint8_t i = 0; i < 16; i++) {
		testArr[i] = (((0x55 ^ i) & 0xF) << 4) | (i & 0xF);
	}

	// start 0, 2 byte
	testPtr = testArr;
	start = 0;
	size = 11;
	EXPECT_EQ(ERR_NO_ERR, extract_bits(&result, &testPtr, &start, size));
	EXPECT_EQ(0x282, result);
	EXPECT_EQ(testArr + 1, testPtr);
	EXPECT_EQ(3, start);

	// start 0, 3 byte
	testPtr = testArr;
	start = 0;
	size = 22;
	EXPECT_EQ(ERR_NO_ERR, extract_bits(&result, &testPtr, &start, size));
	EXPECT_EQ(0x14105C, result);
	EXPECT_EQ(testArr + 2, testPtr);
	EXPECT_EQ(6, start);

	// start 1, 4 byte
	testPtr = testArr;
	start = 1;
	size = 27;
	EXPECT_EQ(ERR_NO_ERR, extract_bits(&result, &testPtr, &start, size));
	EXPECT_EQ(0x5041726, result);
	EXPECT_EQ(testArr + 3, testPtr);
	EXPECT_EQ(4, start);

	// start 1, max size
	testPtr = testArr;
	start = 1;
	size = 64;
	EXPECT_EQ(ERR_NO_ERR, extract_bits(&result, &testPtr, &start, size));
	EXPECT_EQ(0xA082E4C6280A6C4F, result);
	EXPECT_EQ(testArr + 8, testPtr);
	EXPECT_EQ(1, start);

	// start 2, 5 byte
	testPtr = testArr + 3;
	start = 2;
	size = 35;
	EXPECT_EQ(ERR_NO_ERR, extract_bits(&result, &testPtr, &start, size));
	EXPECT_EQ(0x46280A6C4, result);
	EXPECT_EQ(testArr + 7, testPtr);
	EXPECT_EQ(5, start);

	// start 2, 3 byte
	testPtr = testArr + 2;
	start = 2;
	size = 21;
	EXPECT_EQ(ERR_NO_ERR, extract_bits(&result, &testPtr, &start, size));
	EXPECT_EQ(0x19318A, result);
	EXPECT_EQ(testArr + 4, testPtr);
	EXPECT_EQ(7, start);

	// start 3, 2 byte
	testPtr = testArr + 8;
	start = 3;
	size = 6;
	EXPECT_EQ(ERR_NO_ERR, extract_bits(&result, &testPtr, &start, size));
	EXPECT_EQ(0x31, result);
	EXPECT_EQ(testArr + 9, testPtr);
	EXPECT_EQ(1, start);

	// start 4, 3 byte
	testPtr = testArr + 9;
	start = 4;
	size = 14;
	EXPECT_EQ(ERR_NO_ERR, extract_bits(&result, &testPtr, &start, size));
	EXPECT_EQ(0x27EB, result);
	EXPECT_EQ(testArr + 11, testPtr);
	EXPECT_EQ(2, start);

	// start 5, 2 byte
	testPtr = testArr + 12;
	start = 5;
	size = 9;
	EXPECT_EQ(ERR_NO_ERR, extract_bits(&result, &testPtr, &start, size));
	EXPECT_EQ(0x123, result);
	EXPECT_EQ(testArr + 13, testPtr);
	EXPECT_EQ(6, start);

	// start 5, 6 byte
	testPtr = testArr;
	start = 5;
	size = 38;
	EXPECT_EQ(ERR_NO_ERR, extract_bits(&result, &testPtr, &start, size));
	EXPECT_EQ(0x020B9318A0, result);
	EXPECT_EQ(testArr + 5, testPtr);
	EXPECT_EQ(3, start);

	// start 6, 7 byte
	testPtr = testArr + 9;
	start = 6;
	size = 44;
	EXPECT_EQ(ERR_NO_ERR, extract_bits(&result, &testPtr, &start, size));
	EXPECT_EQ(0x7EBAE7236FA, result);
	EXPECT_EQ(testArr + 15, testPtr);
	EXPECT_EQ(2, start);

	// start 6, max size
	testPtr = testArr + 2;
	start = 6;
	size = 64;
	EXPECT_EQ(ERR_NO_ERR, extract_bits(&result, &testPtr, &start, size));
	EXPECT_EQ(0x98C5014D89F6327E, result);
	EXPECT_EQ(testArr + 10, testPtr);
	EXPECT_EQ(6, start);

	// start 7, 2 byte
	testPtr = testArr + 7;
	start = 7;
	size = 2;
	EXPECT_EQ(ERR_NO_ERR, extract_bits(&result, &testPtr, &start, size));
	EXPECT_EQ(0x3, result);
	EXPECT_EQ(testArr + 8, testPtr);
	EXPECT_EQ(1, start);
}

/**
 * Validates output of {@link extract_bits} for case 4 (multi-byte, even end).
 */
TEST_F(HuffmanTest, extract_bits_case4) {
	uint8_t testArr[128];
	uint8_t* testPtr;
	uint8_t size;
	uint8_t start;
	uint64_t result = 0;

	for (uint8_t i = 0; i < 128; i++) {
		testArr[i] = (((0x55 ^ i) & 0xF) << 4) | (i & 0xF);
	}

	// start 0, 2 bytes
	testPtr = testArr;
	start = 0;
	size = 16;
	EXPECT_EQ(ERR_NO_ERR, extract_bits(&result, &testPtr, &start, size));
	EXPECT_EQ(0x5041, result);
	EXPECT_EQ(testArr + 2, testPtr);
	EXPECT_EQ(0, start);


	// start 0, full size
	testPtr = testArr;
	start = 0;
	size = 16;
	EXPECT_EQ(ERR_NO_ERR, extract_bits(&result, &testPtr, &start, size));
	EXPECT_EQ(0x5041, result);
	EXPECT_EQ(testArr + 2, testPtr);
	EXPECT_EQ(0, start);

	// start 1, 4 bytes
	testPtr = testArr + 8;
	start = 1;
	size = 31;
	EXPECT_EQ(ERR_NO_ERR, extract_bits(&result, &testPtr, &start, size));
	EXPECT_EQ(0x58C9FAEB, result);
	EXPECT_EQ(testArr + 12, testPtr);
	EXPECT_EQ(0, start);

	// start 2, 3 bytes
	testPtr = testArr + 2;
	start = 2;
	size = 22;
	EXPECT_EQ(ERR_NO_ERR, extract_bits(&result, &testPtr, &start, size));
	EXPECT_EQ(0x326314, result);
	EXPECT_EQ(testArr + 5, testPtr);
	EXPECT_EQ(0, start);

	// start 3, 5 bytes
	testPtr = testArr;
	start = 3;
	size = 37;
	EXPECT_EQ(ERR_NO_ERR, extract_bits(&result, &testPtr, &start, size));
	EXPECT_EQ(0x1041726314, result);
	EXPECT_EQ(testArr + 5, testPtr);
	EXPECT_EQ(0, start);

	// start 4, 7 bytes
	testPtr = testArr + 3;
	start = 4;
	size = 52;
	EXPECT_EQ(ERR_NO_ERR, extract_bits(&result, &testPtr, &start, size));
	EXPECT_EQ(0x314053627D8C9, result);
	EXPECT_EQ(testArr + 10, testPtr);
	EXPECT_EQ(0, start);

	// start 6, 6 bytes
	testPtr = testArr + 1;
	start = 6;
	size = 42;
	EXPECT_EQ(ERR_NO_ERR, extract_bits(&result, &testPtr, &start, size));
	EXPECT_EQ(0x17263140536, result);
	EXPECT_EQ(testArr + 7, testPtr);
	EXPECT_EQ(0, start);

	// start 7, 2 bytes
	testPtr = testArr + 3;
	start = 7;
	size = 9;
	EXPECT_EQ(ERR_NO_ERR, extract_bits(&result, &testPtr, &start, size));
	EXPECT_EQ(0x114, result);
	EXPECT_EQ(testArr + 5, testPtr);
	EXPECT_EQ(0, start);

	// start 7, 8 bytes
	testPtr = testArr + 2;
	start = 7;
	size = 57;
	EXPECT_EQ(ERR_NO_ERR, extract_bits(&result, &testPtr, &start, size));
	EXPECT_EQ(0x06314053627D8C9, result);
	EXPECT_EQ(testArr + 10, testPtr);
	EXPECT_EQ(0, start);
}

/**
 * Tests input validation for {@link put_bits}.
 */
TEST_F(HuffmanTest, put_bits_errs) {
	uint8_t testArr[128];
	uint8_t* testPtr = testArr;
	uint8_t* nullTest = NULL;
	uint8_t size = 3;
	uint8_t start = 3;
	uint64_t dst_size = 128;
	uint64_t value = 0;

	// null checks
	EXPECT_EQ(ERR_NULL_PTR, put_bits(NULL, &start, &dst_size, value, size));
	EXPECT_EQ(ERR_NULL_PTR, put_bits(&nullTest, &start, &dst_size, value, size));
	EXPECT_EQ(ERR_NULL_PTR, put_bits(&testPtr, NULL, &dst_size, value, size));
	EXPECT_EQ(ERR_NULL_PTR, put_bits(&testPtr, &start, NULL, value, size));

	// input validation
	EXPECT_EQ(ERR_INVALID_VALUE, put_bits(&testPtr, &start, &dst_size, value, 0));
	EXPECT_EQ(ERR_INVALID_VALUE, put_bits(&testPtr, &start, &dst_size, value, 65));
	start = 8;
	EXPECT_EQ(ERR_INVALID_VALUE, put_bits(&testPtr, &start, &dst_size, value, size));

	// size validation
	start = 3;
	dst_size = 0;
	EXPECT_EQ(ERR_INSUFFICIENT_SPACE,
			put_bits(&testPtr, &start, &dst_size, value, size));
	dst_size = 1;
	EXPECT_EQ(ERR_INSUFFICIENT_SPACE, put_bits(&testPtr, &start, &dst_size, value, 6));
	start = 0;
	EXPECT_EQ(ERR_INSUFFICIENT_SPACE, put_bits(&testPtr, &start, &dst_size, value, 9));
	start = 7;
	dst_size = 2;
	EXPECT_EQ(ERR_INSUFFICIENT_SPACE, put_bits(&testPtr, &start, &dst_size, value, 10));
}

/**
 * Validates output for {@link put_bits} for case 1 (single byte, non-even end).
 */
TEST_F(HuffmanTest, put_bits_case1) {
	uint8_t testArr[2];
	uint8_t* testPtr;
	uint8_t size;
	uint8_t start;
	uint64_t value;
	uint64_t dst_size;

	// start 0
	testArr[0] = 0xA5;
	testPtr = testArr;
	start = 0;
	size = 5;
	value = 0x08;
	dst_size = 2;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &dst_size, value, size));
	EXPECT_EQ(0x40, testArr[0]);
	EXPECT_EQ(testArr, testPtr);
	EXPECT_EQ(2, dst_size);
	EXPECT_EQ(5, start);

	// start 1, limited size
	testArr[0] = 0x11;
	testPtr = testArr;
	start = 1;
	size = 6;
	value = 0x35;
	dst_size = 1;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &dst_size, value, size));
	EXPECT_EQ(0x6A, testArr[0]);
	EXPECT_EQ(testArr, testPtr);
	EXPECT_EQ(1, dst_size);
	EXPECT_EQ(7, start);

	// start 2
	testArr[0] = 0x61;
	testPtr = testArr;
	start = 2;
	size = 4;
	value = 0xD;
	dst_size = 2;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &dst_size, value, size));
	EXPECT_EQ(0x74, testArr[0]);
	EXPECT_EQ(testArr, testPtr);
	EXPECT_EQ(2, dst_size);
	EXPECT_EQ(6, start);

	// start 3
	testArr[0] = 0xB7;
	testPtr = testArr;
	start = 3;
	size = 2;
	value = 0x1;
	dst_size = 2;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &dst_size, value, size));
	EXPECT_EQ(0xA8, testArr[0]);
	EXPECT_EQ(testArr, testPtr);
	EXPECT_EQ(2, dst_size);
	EXPECT_EQ(5, start);

	// start 4
	testArr[0] = 0x10;
	testPtr = testArr;
	start = 4;
	size = 2;
	value = 0x2;
	dst_size = 16;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &dst_size, value, size));
	EXPECT_EQ(0x18, testArr[0]);
	EXPECT_EQ(testArr, testPtr);
	EXPECT_EQ(16, dst_size);
	EXPECT_EQ(6, start);

	// start 5, clip
	testArr[0] = 0xA9;
	testPtr = testArr;
	start = 5;
	size = 2;
	value = 0x5;
	dst_size = 16;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &dst_size, value, size));
	EXPECT_EQ(0xAA, testArr[0]);
	EXPECT_EQ(testArr, testPtr);
	EXPECT_EQ(16, dst_size);
	EXPECT_EQ(7, start);

	// start 6
	testArr[0] = 0xFF;
	testPtr = testArr;
	start = 6;
	size = 1;
	value = 0x0;
	dst_size = 16;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &dst_size, value, size));
	EXPECT_EQ(0xFC, testArr[0]);
	EXPECT_EQ(testArr, testPtr);
	EXPECT_EQ(16, dst_size);
	EXPECT_EQ(7, start);
}

/**
 * Validates output for {@link put_bits} for case 2 (single byte, even end).
 */
TEST_F(HuffmanTest, put_bits_case2) {
	uint8_t testArr[16];
	uint8_t* testPtr;
	uint8_t size;
	uint8_t start;
	uint64_t value;
	uint8_t i;
	uint64_t dst_size;

	// start 0
	testPtr = testArr;
	start = 0;
	size = 8;
	value = 0x96;
	dst_size = 16;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &dst_size, value, size));
	EXPECT_EQ(0x96, testArr[0]);
	EXPECT_EQ(testArr + 1, testPtr);
	EXPECT_EQ(15, dst_size);
	EXPECT_EQ(0, start);

	// start 1, bit 0 = 0, clip value bits
	testArr[0] = 0x00;
	testPtr = testArr;
	start = 1;
	size = 7;
	value = 0xB5;
	dst_size = 16;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &dst_size, value, size));
	EXPECT_EQ(0x35, testArr[0]);
	EXPECT_EQ(testArr + 1, testPtr);
	EXPECT_EQ(15, dst_size);
	EXPECT_EQ(0, start);

	// start 1, bit 0 = 1
	testArr[0] = 0xFF;
	testPtr = testArr;
	start = 1;
	size = 7;
	value = 0x35;
	dst_size = 16;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &dst_size, value, size));
	EXPECT_EQ(0xB5, testArr[0]);
	EXPECT_EQ(testArr + 1, testPtr);
	EXPECT_EQ(15, dst_size);
	EXPECT_EQ(0, start);

	// start 2
	testArr[0] = 0x70;
	testPtr = testArr;
	start = 2;
	size = 6;
	value = 0x15;
	dst_size = 16;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &dst_size, value, size));
	EXPECT_EQ(0x55, testArr[0]);
	EXPECT_EQ(testArr + 1, testPtr);
	EXPECT_EQ(15, dst_size);
	EXPECT_EQ(0, start);

	// start 3, clip value
	testArr[0] = 0x39;
	testPtr = testArr;
	start = 3;
	size = 5;
	value = 0xFED06;
	dst_size = 16;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &dst_size, value, size));
	EXPECT_EQ(0x26, testArr[0]);
	EXPECT_EQ(testArr + 1, testPtr);
	EXPECT_EQ(15, dst_size);
	EXPECT_EQ(0, start);

	// start 4, overwrite
	testArr[0] = 0xC3;
	testPtr = testArr;
	start = 4;
	size = 4;
	value = 0x2;
	dst_size = 16;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &dst_size, value, size));
	EXPECT_EQ(0xC2, testArr[0]);
	EXPECT_EQ(testArr + 1, testPtr);
	EXPECT_EQ(15, dst_size);
	EXPECT_EQ(0, start);

	// start 5, overwrite
	testArr[0] = 0x06;
	testPtr = testArr;
	start = 5;
	size = 3;
	value = 0xA;
	dst_size = 16;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &dst_size, value, size));
	EXPECT_EQ(0x02, testArr[0]);
	EXPECT_EQ(testArr + 1, testPtr);
	EXPECT_EQ(15, dst_size);
	EXPECT_EQ(0, start);

	// start 6
	testArr[0] = 0xC9;
	testPtr = testArr;
	start = 6;
	size = 2;
	value = 0x2;
	dst_size = 16;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &dst_size, value, size));
	EXPECT_EQ(0xCA, testArr[0]);
	EXPECT_EQ(testArr + 1, testPtr);
	EXPECT_EQ(15, dst_size);
	EXPECT_EQ(0, start);

	// start 7
	testArr[0] = 0xFF;
	testPtr = testArr;
	start = 7;
	size = 1;
	value = 0x0;
	dst_size = 16;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &dst_size, value, size));
	EXPECT_EQ(0xFE, testArr[0]);
	EXPECT_EQ(testArr + 1, testPtr);
	EXPECT_EQ(15, dst_size);
	EXPECT_EQ(0, start);
}

/**
 * Validates output for {@link put_bits} for case 3 (multi-byte, non-even end).
 */
TEST_F(HuffmanTest, put_bits_case3) {
	uint8_t testArr[16];
	uint8_t* testPtr;
	uint8_t size;
	uint8_t start;
	uint64_t value;
	uint64_t dst_size;

	// start 0, 7 byte, overwrite
	memset(testArr, 0xFF, 16);
	testPtr = testArr;
	start = 0;
	size = 51;
	value = 0x314053627D8C9;
	dst_size = 16;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &dst_size, value, size));
	EXPECT_EQ(0x62, testArr[0]);
	EXPECT_EQ(0x80, testArr[1]);
	EXPECT_EQ(0xA6, testArr[2]);
	EXPECT_EQ(0xC4, testArr[3]);
	EXPECT_EQ(0xFB, testArr[4]);
	EXPECT_EQ(0x19, testArr[5]);
	EXPECT_EQ(0x20, testArr[6]);
	EXPECT_EQ(testArr + 6, testPtr);
	EXPECT_EQ(10, dst_size);
	EXPECT_EQ(3, start);

	// start 1, 5 byte, even byte size
	testArr[0] = 0x86;
	testPtr = testArr;
	start = 1;
	size = 32;
	value = 0x50417263;
	dst_size = 16;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &dst_size, value, size));
	EXPECT_EQ(0xA8, testArr[0]);
	EXPECT_EQ(0x20, testArr[1]);
	EXPECT_EQ(0xB9, testArr[2]);
	EXPECT_EQ(0x31, testArr[3]);
	EXPECT_EQ(testArr + 4, testPtr);
	EXPECT_EQ(12, dst_size);
	EXPECT_EQ(1, start);

	// start 2, 3 byte
	testArr[0] = 0x9B;
	testPtr = testArr;
	start = 2;
	size = 19;
	value = 0x58C9F;
	dst_size = 16;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &dst_size, value, size));
	EXPECT_EQ(0xAC, testArr[0]);
	EXPECT_EQ(0x64, testArr[1]);
	EXPECT_EQ(0xF8, testArr[2]);
	EXPECT_EQ(testArr + 2, testPtr);
	EXPECT_EQ(14, dst_size);
	EXPECT_EQ(5, start);

	// start 2, max size
	testArr[0] = 0x40;
	testPtr = testArr;
	start = 2;
	size = 64;
	value = 0x1A2B3C4D5E6F7089;
	dst_size = 16;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &dst_size, value, size));
	EXPECT_EQ(0x46, testArr[0]);
	EXPECT_EQ(0x8A, testArr[1]);
	EXPECT_EQ(0xCF, testArr[2]);
	EXPECT_EQ(0x13, testArr[3]);
	EXPECT_EQ(0x57, testArr[4]);
	EXPECT_EQ(0x9B, testArr[5]);
	EXPECT_EQ(0xDC, testArr[6]);
	EXPECT_EQ(0x22, testArr[7]);
	EXPECT_EQ(0x40, testArr[8]);
	EXPECT_EQ(testArr + 8, testPtr);
	EXPECT_EQ(8, dst_size);
	EXPECT_EQ(2, start);

	// start 3, 4 byte
	testArr[0] = 0xA5;
	testArr[3] = 0xFF;
	testPtr = testArr;
	start = 3;
	size = 25;
	value = 0x1C5A691;
	dst_size = 16;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &dst_size, value, size));
	EXPECT_EQ(0xBC, testArr[0]);
	EXPECT_EQ(0x5A, testArr[1]);
	EXPECT_EQ(0x69, testArr[2]);
	EXPECT_EQ(0x10, testArr[3]);
	EXPECT_EQ(testArr + 3, testPtr);
	EXPECT_EQ(13, dst_size);
	EXPECT_EQ(4, start);

	// start 4, 8 byte
	testArr[0] = 0xC3;
	testPtr = testArr;
	start = 4;
	size = 59;
	value = 0x35386B43F5941E3;
	dst_size = 16;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &dst_size, value, size));
	EXPECT_EQ(0xC6, testArr[0]);
	EXPECT_EQ(0xA7, testArr[1]);
	EXPECT_EQ(0x0D, testArr[2]);
	EXPECT_EQ(0x68, testArr[3]);
	EXPECT_EQ(0x7E, testArr[4]);
	EXPECT_EQ(0xB2, testArr[5]);
	EXPECT_EQ(0x83, testArr[6]);
	EXPECT_EQ(0xC6, testArr[7]);
	EXPECT_EQ(testArr + 7, testPtr);
	EXPECT_EQ(9, dst_size);
	EXPECT_EQ(7, start);

	// start 5, 2 byte
	testArr[0] = 0x00;
	testPtr = testArr;
	start = 5;
	size = 9;
	value = 0x14A;
	dst_size = 16;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &dst_size, value, size));
	EXPECT_EQ(0x05, testArr[0]);
	EXPECT_EQ(0x28, testArr[1]);
	EXPECT_EQ(testArr + 1, testPtr);
	EXPECT_EQ(15, dst_size);
	EXPECT_EQ(6, start);

	// start 5, max size
	testArr[0] = 0x92;
	testPtr = testArr;
	start = 5;
	size = 64;
	value = 0xF0E1D2C3B4A59687;
	dst_size = 16;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &dst_size, value, size));
	EXPECT_EQ(0x97, testArr[0]);
	EXPECT_EQ(0x87, testArr[1]);
	EXPECT_EQ(0x0E, testArr[2]);
	EXPECT_EQ(0x96, testArr[3]);
	EXPECT_EQ(0x1D, testArr[4]);
	EXPECT_EQ(0xA5, testArr[5]);
	EXPECT_EQ(0x2C, testArr[6]);
	EXPECT_EQ(0xB4, testArr[7]);
	EXPECT_EQ(0x38, testArr[8]);
	EXPECT_EQ(testArr + 8, testPtr);
	EXPECT_EQ(8, dst_size);
	EXPECT_EQ(5, start);

	// start 6, 2 byte
	testArr[0] = 0x7B;
	testPtr = testArr;
	start = 6;
	size = 5;
	value = 0x06;
	dst_size = 16;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &dst_size, value, size));
	EXPECT_EQ(0x78, testArr[0]);
	EXPECT_EQ(0xC0, testArr[1]);
	EXPECT_EQ(testArr + 1, testPtr);
	EXPECT_EQ(15, dst_size);
	EXPECT_EQ(3, start);

	// start 7, 6 byte
	testArr[0] = 0x56;
	testPtr = testArr;
	start = 7;
	size = 40;
	value = 0x84217BDE6A;
	dst_size = 16;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &dst_size, value, size));
	EXPECT_EQ(0x57, testArr[0]);
	EXPECT_EQ(0x08, testArr[1]);
	EXPECT_EQ(0x42, testArr[2]);
	EXPECT_EQ(0xF7, testArr[3]);
	EXPECT_EQ(0xBC, testArr[4]);
	EXPECT_EQ(0xD4, testArr[5]);
	EXPECT_EQ(testArr + 5, testPtr);
	EXPECT_EQ(11, dst_size);
	EXPECT_EQ(7, start);
}

/**
 * Validates output for {@link put_bits} for case 4 (multi-byte, even end).
 */
TEST_F(HuffmanTest, put_bits_case4) {
	uint8_t testArr[16];
	uint8_t* testPtr;
	uint8_t size;
	uint8_t start;
	uint64_t value;
	uint64_t dst_size;

	// start 0, 6 bytes, overwrite
	memset(testArr, 0xFF, 16);
	testPtr = testArr;
	start = 0;
	size = 48;
	value = 0x1574689AEB6C;
	dst_size = 16;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &dst_size, value, size));
	EXPECT_EQ(0x15, testArr[0]);
	EXPECT_EQ(0x74, testArr[1]);
	EXPECT_EQ(0x68, testArr[2]);
	EXPECT_EQ(0x9A, testArr[3]);
	EXPECT_EQ(0xEB, testArr[4]);
	EXPECT_EQ(0x6C, testArr[5]);
	EXPECT_EQ(testArr + 6, testPtr);
	EXPECT_EQ(10, dst_size);
	EXPECT_EQ(0, start);

	// start 0, full size, limited size
	testPtr = testArr;
	start = 0;
	size = 64;
	value = 0xFEDCBA9876543210;
	dst_size = 8;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &dst_size, value, size));
	EXPECT_EQ(0xFE, testArr[0]);
	EXPECT_EQ(0xDC, testArr[1]);
	EXPECT_EQ(0xBA, testArr[2]);
	EXPECT_EQ(0x98, testArr[3]);
	EXPECT_EQ(0x76, testArr[4]);
	EXPECT_EQ(0x54, testArr[5]);
	EXPECT_EQ(0x32, testArr[6]);
	EXPECT_EQ(0x10, testArr[7]);
	EXPECT_EQ(testArr + 8, testPtr);
	EXPECT_EQ(0, dst_size);
	EXPECT_EQ(0, start);

	// start 1, 7 bytes
	testArr[0] = 0x46;
	testArr[6] = 0xF0;
	testPtr = testArr;
	start = 1;
	size = 55;
	value = 0xE2194DAF1E3508;
	dst_size = 16;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &dst_size, value, size));
	EXPECT_EQ(0x62, testArr[0]);
	EXPECT_EQ(0x19, testArr[1]);
	EXPECT_EQ(0x4D, testArr[2]);
	EXPECT_EQ(0xAF, testArr[3]);
	EXPECT_EQ(0x1E, testArr[4]);
	EXPECT_EQ(0x35, testArr[5]);
	EXPECT_EQ(0x08, testArr[6]);
	EXPECT_EQ(testArr + 7, testPtr);
	EXPECT_EQ(9, dst_size);
	EXPECT_EQ(0, start);

	// start 2, 5 bytes
	testArr[0] = 0x81;
	testPtr = testArr;
	start = 2;
	size = 38;
	value = 0x29ECA7F031;
	dst_size = 16;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &dst_size, value, size));
	EXPECT_EQ(0xA9, testArr[0]);
	EXPECT_EQ(0xEC, testArr[1]);
	EXPECT_EQ(0xA7, testArr[2]);
	EXPECT_EQ(0xF0, testArr[3]);
	EXPECT_EQ(0x31, testArr[4]);
	EXPECT_EQ(testArr + 5, testPtr);
	EXPECT_EQ(11, dst_size);
	EXPECT_EQ(0, start);


	// start 3, 2 bytes
	testArr[0] = 0xF3;
	testArr[1] = 0x01;
	testPtr = testArr;
	start = 3;
	size = 13;
	value = 0x0C52;
	dst_size = 16;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &dst_size, value, size));
	EXPECT_EQ(0xEC, testArr[0]);
	EXPECT_EQ(0x52, testArr[1]);
	EXPECT_EQ(testArr + 2, testPtr);
	EXPECT_EQ(14, dst_size);
	EXPECT_EQ(0, start);

	// start 4, 4 bytes
	testArr[0] = 0x55;
	testPtr = testArr;
	start = 4;
	size = 28;
	value = 0x6B54D3C;
	dst_size = 16;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &dst_size, value, size));
	EXPECT_EQ(0x56, testArr[0]);
	EXPECT_EQ(0xB5, testArr[1]);
	EXPECT_EQ(0x4D, testArr[2]);
	EXPECT_EQ(0x3C, testArr[3]);
	EXPECT_EQ(testArr + 4, testPtr);
	EXPECT_EQ(12, dst_size);
	EXPECT_EQ(0, start);

	// start 5, 3 bytes, start mid-array
	testArr[10] = 0x66;
	testPtr = testArr + 10;
	start = 5;
	size = 19;
	value = 0x194DEA;
	dst_size = 16;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &dst_size, value, size));
	EXPECT_EQ(0x61, testArr[10]);
	EXPECT_EQ(0x4D, testArr[11]);
	EXPECT_EQ(0xEA, testArr[12]);
	EXPECT_EQ(testArr + 13, testPtr);
	EXPECT_EQ(13, dst_size);
	EXPECT_EQ(0, start);

	// start 6, 2 byte
	testArr[0] = 0xE6;
	testPtr = testArr;
	start = 6;
	size = 10;
	value = 0x12A;
	dst_size = 16;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &dst_size, value, size));
	EXPECT_EQ(0xE5, testArr[0]);
	EXPECT_EQ(0x2A, testArr[1]);
	EXPECT_EQ(testArr + 2, testPtr);
	EXPECT_EQ(14, dst_size);
	EXPECT_EQ(0, start);

	// start 7, 5 bytes, clip, start mid-array
	testArr[3] = 0x38;
	testPtr = testArr + 3;
	start = 7;
	size = 33;
	value = 0x85EAC5F684;
	dst_size = 13;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &dst_size, value, size));
	EXPECT_EQ(0x39, testArr[3]);
	EXPECT_EQ(0xEA, testArr[4]);
	EXPECT_EQ(0xC5, testArr[5]);
	EXPECT_EQ(0xF6, testArr[6]);
	EXPECT_EQ(0x84, testArr[7]);
	EXPECT_EQ(testArr + 8, testPtr);
	EXPECT_EQ(8, dst_size);
	EXPECT_EQ(0, start);

	// start 7, 8 bytes
	testArr[0] = 0xF3;
	testPtr = testArr;
	start = 7;
	size = 57;
	value = 0x03A5B9E8C1D7368;
	dst_size = 16;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &dst_size, value, size));
	EXPECT_EQ(0xF2, testArr[0]);
	EXPECT_EQ(0x3A, testArr[1]);
	EXPECT_EQ(0x5B, testArr[2]);
	EXPECT_EQ(0x9E, testArr[3]);
	EXPECT_EQ(0x8C, testArr[4]);
	EXPECT_EQ(0x1D, testArr[5]);
	EXPECT_EQ(0x73, testArr[6]);
	EXPECT_EQ(0x68, testArr[7]);
	EXPECT_EQ(testArr + 8, testPtr);
	EXPECT_EQ(8, dst_size);
	EXPECT_EQ(0, start);
}

/**
 * Tests input validation for {@link build_header}.
 */
TEST_F(HuffmanTest, build_header_errs) {
	uint8_t testArr[128];
	uint8_t* testPtr = testArr;
	uint8_t* nullTest = NULL;
	uint8_t start;
	uint64_t dst_size = 128;
	HuffmanHeader header = {
			.wordSize = 9,
			.padBits = 0,
			.uniqueWords = 3
	};

	// null check
	EXPECT_EQ(ERR_NULL_PTR, build_header(NULL, &start, &dst_size, &header));
	EXPECT_EQ(ERR_NULL_PTR, build_header(&nullTest, &start, &dst_size, &header));
	EXPECT_EQ(ERR_NULL_PTR, build_header(&testPtr, NULL, &dst_size, &header));
	EXPECT_EQ(ERR_NULL_PTR, build_header(&testPtr, &start, NULL, &header));
	EXPECT_EQ(ERR_NULL_PTR, build_header(&testPtr, &start, &dst_size, NULL));

	// input validation
	header.wordSize = 1;
	EXPECT_EQ(ERR_INVALID_VALUE, build_header(&testPtr, &start, &dst_size, &header));
	header.wordSize = 65;
	EXPECT_EQ(ERR_INVALID_VALUE, build_header(&testPtr, &start, &dst_size, &header));
	header.wordSize = 9;
	header.padBits = 9;
	EXPECT_EQ(ERR_INVALID_VALUE, build_header(&testPtr, &start, &dst_size, &header));
	header.wordSize = 3;
	header.padBits = 0;
	header.uniqueWords = 9;
	EXPECT_EQ(ERR_INVALID_VALUE, build_header(&testPtr, &start, &dst_size, &header));

	// size validation
	dst_size = 2; // (6 + 5 + 9) / 8 = 20 / 8 = 2 R 4
	header.padBits = 3;
	header.wordSize = 9;
	header.uniqueWords = 1;
	EXPECT_EQ(ERR_INSUFFICIENT_SPACE, build_header(&testPtr, &start, &dst_size, &header));
	header.wordSize = 60;
	dst_size = 8; // (6 + 6 + 60) / 8 = 72 / 8 = 9 R 0
	EXPECT_EQ(ERR_INSUFFICIENT_SPACE, build_header(&testPtr, &start, &dst_size, &header));
	dst_size = 2;
	EXPECT_EQ(ERR_INSUFFICIENT_SPACE, build_header(&testPtr, &start, &dst_size, &header));
	dst_size = 0;
	EXPECT_EQ(ERR_INSUFFICIENT_SPACE, build_header(&testPtr, &start, &dst_size, &header));
}

/**
 * Validates output for {@link log2_ceil_u64}.
 */
TEST_F(HuffmanTest, log2_ceil_u64) {
	// case 1: 1
	EXPECT_EQ(1, log2_ceil_u64(1));

	// case 2: power of 2
	for (uint8_t i = 1; i < 64; i++) {
		EXPECT_EQ(i, log2_ceil_u64((uint64_t)0x1 << i));
	}

	// case 3: not even power of 2
	for (uint8_t i = 0; i < 62; i++) {
		EXPECT_EQ(i + 3, log2_ceil_u64((uint64_t)0x5 << i));
	}
}

/**
 * Validates output for {@link log2_ceil_u64}.
 */
TEST_F(HuffmanTest, log2_ceil_u8) {
	// case 1: 1
	EXPECT_EQ(1, log2_ceil_u8(1));

	// case 2: power of 2
	for (uint8_t i = 1; i < 8; i++) {
		EXPECT_EQ(i, log2_ceil_u8(0x1 << i));
	}

	// case 3: not even power of 2
	for (uint8_t i = 0; i < 6; i++) {
		EXPECT_EQ(i + 3, log2_ceil_u8((uint8_t)0x5 << i));
	}
}

/**
 * Validates output for {@link build_header}.
 */
TEST_F(HuffmanTest, build_header) {
	uint8_t testArr[128];
	uint8_t* testPtr = testArr;
	uint8_t start;
	uint64_t dst_size = 128;
	HuffmanHeader header;
	uint64_t numBits;
	uint8_t wordSize;

	uint8_t* tempPtr;
	uint8_t tempStart;
	uint64_t temp;

	for (wordSize = 2; wordSize <= HUFFMAN_MAX_WORD_SIZE; wordSize++) {
		testPtr = testArr;
		tempPtr = testArr;
		tempStart = 0;
		dst_size = 128;
		header.wordSize = wordSize;
		header.padBits = wordSize >> 1;
		header.uniqueWords = ((uint64_t)wordSize - 1) * (uint64_t)wordSize;
		numBits = HUFFMAN_WORD_SIZE_NUM_BITS + (uint64_t)log2_ceil_u8(wordSize) + (uint64_t)wordSize;

		EXPECT_EQ(ERR_NO_ERR, build_header(&testPtr, &start, &dst_size, &header));
		// validate contents
		extract_bits(&temp, &tempPtr, &tempStart, HUFFMAN_WORD_SIZE_NUM_BITS);
		EXPECT_EQ((uint64_t)wordSize, temp);
		extract_bits(&temp, &tempPtr, &tempStart, (uint64_t)log2_ceil_u8(wordSize));
		EXPECT_EQ((uint64_t)header.padBits, temp);
		extract_bits(&temp, &tempPtr, &tempStart, (uint64_t)wordSize);
		EXPECT_EQ((uint64_t)header.uniqueWords - 1, temp);

		// validate variables
		EXPECT_EQ(&testArr[numBits / 8], testPtr);
		EXPECT_EQ(128 - (numBits / 8), dst_size);
		EXPECT_EQ(numBits % 8, start);
	}
}

/**
 * Tests input validation for {@link parse_header}.
 */
TEST_F(HuffmanTest, parse_header_errs) {
	uint8_t testArr[128];
	uint8_t* testPtr = testArr;
	uint8_t* nullTest = NULL;
	uint8_t start;
	uint64_t srcSize = 128;
	HuffmanHeader header;
	uint64_t wordSize;

	// null check
	EXPECT_EQ(ERR_NULL_PTR, parse_header(NULL, &testPtr, &start, &srcSize));
	EXPECT_EQ(ERR_NULL_PTR, parse_header(&header, NULL, &start, &srcSize));
	EXPECT_EQ(ERR_NULL_PTR, parse_header(&header, &nullTest, &start, &srcSize));
	EXPECT_EQ(ERR_NULL_PTR, parse_header(&header, &testPtr, NULL, &srcSize));
	EXPECT_EQ(ERR_NULL_PTR, parse_header(&header, &testPtr, &start, NULL));

	// input validation
	srcSize = 1;
	EXPECT_EQ(ERR_INVALID_VALUE, parse_header(&header, &testPtr, &start, &srcSize));

	// invalid wordSize
	srcSize = 128;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &srcSize, (uint64_t)0, (uint8_t)HUFFMAN_WORD_SIZE_NUM_BITS));
	testPtr = testArr;
	start = 0;
	srcSize = 128;
	EXPECT_EQ(ERR_INVALID_DATA, parse_header(&header, &testPtr, &start, &srcSize));

	// invalid padBits
	wordSize = 5;
	testPtr = testArr;
	start = 0;
	srcSize = 128;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &srcSize, wordSize, (uint8_t)HUFFMAN_WORD_SIZE_NUM_BITS));
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &srcSize, (uint64_t)6, log2_ceil_u8(wordSize)));
	testPtr = testArr;
	start = 0;
	srcSize = 128;
	EXPECT_EQ(ERR_INVALID_DATA, parse_header(&header, &testPtr, &start, &srcSize));

	// size validation
	// case 1: even byte boundary
	wordSize = 13; // 14 when parsed; total size: 6 + ceil(log2(14)) + 14 = 6 + 4 + 14 = 24 bits
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &srcSize, wordSize, (uint8_t)HUFFMAN_WORD_SIZE_NUM_BITS));
	testPtr = testArr;
	start = 0;
	srcSize = 2;
	EXPECT_EQ(ERR_INSUFFICIENT_SPACE, parse_header(&header, &testPtr, &start, &srcSize));
	// case 2: not byte boundary
	wordSize = 22; // 23 when parsed; total size: 6 + ceil(log2(23)) + 23 = 6 + 5 + 23 = 34 bits = 4 bytes + 2 bits
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &srcSize, wordSize, (uint8_t)HUFFMAN_WORD_SIZE_NUM_BITS));
	testPtr = testArr;
	start = 0;
	srcSize = 4;
	EXPECT_EQ(ERR_INSUFFICIENT_SPACE, parse_header(&header, &testPtr, &start, &srcSize));
}

/**
 * Validates output for {@link parse_header}.
 */
TEST_F(HuffmanTest, parse_header) {
	uint8_t testArr[128];
	uint8_t* testPtr = testArr;
	uint8_t start;
	uint64_t srcSize = 128;
	HuffmanHeader header;
	uint64_t wordSize;
	uint64_t padBits;
	uint64_t uniqueWords;


	// case 1: min size
	wordSize = 2;
	padBits = 1;
	uniqueWords = 3; // 2^2 - 1
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &srcSize, wordSize, (uint8_t)HUFFMAN_WORD_SIZE_NUM_BITS));
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &srcSize, padBits, log2_ceil_u64(wordSize)));
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &srcSize, uniqueWords - 1, (uint8_t)wordSize));
	testPtr = testArr;
	srcSize = 2;
	EXPECT_EQ(ERR_NO_ERR, parse_header(&header, &testPtr, &start, &srcSize));
	EXPECT_EQ(testArr + 1, testPtr);
	EXPECT_EQ(1, start);
	EXPECT_EQ(1, srcSize);
	EXPECT_EQ(wordSize, header.wordSize);
	EXPECT_EQ(padBits, header.padBits);
	EXPECT_EQ(uniqueWords, header.uniqueWords);

	// case 2: even byte size, limited space
	wordSize = 14;
	padBits = 10;
	uniqueWords = 0x53C;
	testPtr = testArr;
	start = 0;
	srcSize = 128;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &srcSize, wordSize, (uint8_t)HUFFMAN_WORD_SIZE_NUM_BITS));
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &srcSize, padBits, log2_ceil_u64(wordSize)));
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &srcSize, uniqueWords - 1, (uint8_t)wordSize));
	testPtr = testArr;
	srcSize = 3;
	EXPECT_EQ(ERR_NO_ERR, parse_header(&header, &testPtr, &start, &srcSize));
	EXPECT_EQ(testArr + 3, testPtr);
	EXPECT_EQ(0, start);
	EXPECT_EQ(0, srcSize);
	EXPECT_EQ(wordSize, header.wordSize);
	EXPECT_EQ(padBits, header.padBits);
	EXPECT_EQ(uniqueWords, header.uniqueWords);

	// case 3: non-even byte size, limited space
	wordSize = 24; // 6 + 5 + 24 = 35 bits = 4 bytes 3 bits
	padBits = 13;
	uniqueWords = 0x94F;
	testPtr = testArr;
	start = 0;
	srcSize = 128;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &srcSize, wordSize, (uint8_t)HUFFMAN_WORD_SIZE_NUM_BITS));
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &srcSize, padBits, log2_ceil_u64(wordSize)));
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &srcSize, uniqueWords - 1, (uint8_t)wordSize));
	testPtr = testArr;
	srcSize = 5;
	EXPECT_EQ(ERR_NO_ERR, parse_header(&header, &testPtr, &start, &srcSize));
	EXPECT_EQ(testArr + 4, testPtr);
	EXPECT_EQ(3, start);
	EXPECT_EQ(1, srcSize);
	EXPECT_EQ(wordSize, header.wordSize);
	EXPECT_EQ(padBits, header.padBits);
	EXPECT_EQ(uniqueWords, header.uniqueWords);

	// case 4: max size
	wordSize = HUFFMAN_MAX_WORD_SIZE; // 6 + 6 + 64 = 76 bits = 9 bytes 4 bits
	padBits = 45;
	uniqueWords = 0xA8E403921893B6C;
	testPtr = testArr;
	start = 0;
	srcSize = 128;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &srcSize, wordSize, (uint8_t)HUFFMAN_WORD_SIZE_NUM_BITS));
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &srcSize, padBits, log2_ceil_u64(wordSize)));
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, &srcSize, uniqueWords - 1, (uint8_t)wordSize));
	testPtr = testArr;
	srcSize = 10;
	EXPECT_EQ(ERR_NO_ERR, parse_header(&header, &testPtr, &start, &srcSize));
	EXPECT_EQ(testArr + 9, testPtr);
	EXPECT_EQ(0, start);
	EXPECT_EQ(1, srcSize);
	EXPECT_EQ(wordSize, header.wordSize);
	EXPECT_EQ(padBits, header.padBits);
	EXPECT_EQ(uniqueWords, header.uniqueWords);
}

/**
 * Validates output for {@link get_table_value}.
 */
TEST_F(HuffmanTest, get_table_value) {
	uint64_t table[2 * TEST_TABLE_SIZE];
	for (uint64_t i = 0; i < TEST_TABLE_SIZE; i++) {
		EXPECT_EQ(&table[2 * i], get_table_value(table, i));
	}
}

/**
 * Validates output for {@link get_table_id}.
 */
TEST_F(HuffmanTest, get_table_id) {
	uint64_t table[2 * TEST_TABLE_SIZE];
	for (uint64_t i = 0; i < TEST_TABLE_SIZE; i++) {
		EXPECT_EQ(&table[2 * i + 1], get_table_id(table, i));
	}
}

/**
 * Validates error handling of {@link search_table}.
 */
TEST_F(HuffmanTest, search_table_errs) {
	uint64_t tableDat[2 * TEST_TABLE_SIZE];
	uint64_t dst;

	HuffmanHashTable table;
	table.size = TEST_TABLE_SIZE;
	table.table = tableDat;

	// Full table
	for (int i = 0; i < 2 * TEST_TABLE_SIZE; i++) {
		table.table[i] = i + 1;
	}
	// Case I: entry not in table
	EXPECT_EQ(ERR_INSUFFICIENT_SPACE, search_table(&dst, &table, TEST_TABLE_SIZE * 5, false));
	// Case II: entry in table but assuming no match exists
	EXPECT_EQ(ERR_INSUFFICIENT_SPACE, search_table(&dst, &table, (uint64_t)3, true));
}

/**
 * Validates output for {@link search_table}.
 */
TEST_F(HuffmanTest, search_table) {
	uint64_t tableDat[2 * TEST_TABLE_SIZE];
	uint64_t dst;
	uint64_t* val, *id;

	HuffmanHashTable table;
	table.size = TEST_TABLE_SIZE;
	table.table = tableDat;

	// Case I: full table
	memset(table.table, 0x00, 2 * TEST_TABLE_SIZE * sizeof(uint64_t));
	for (uint64_t i = 0; i < TEST_TABLE_SIZE; i++) {
		EXPECT_EQ(ERR_NO_ERR, search_table(&dst, &table, i * 10 / 3, true));
		val = get_table_value(table.table, dst);
		*val = i + 1;
		id = get_table_id(table.table, dst);
		*id = i * 10 / 3;
	}
	for (uint64_t i = 0; i < TEST_TABLE_SIZE; i++) {
		EXPECT_EQ(ERR_NO_ERR, search_table(&dst, &table, i * 10 / 3, false));
		EXPECT_EQ(i + 1, *get_table_value(table.table, dst));
		EXPECT_EQ(i * 10 / 3, *get_table_id(table.table, dst));
	}

	// Case II: partially full table
	memset(table.table, 0x00, 2 * TEST_TABLE_SIZE * sizeof(uint64_t));
	for (uint64_t i = 0; i < TEST_TABLE_SIZE / 2; i++) {
		EXPECT_EQ(ERR_NO_ERR, search_table(&dst, &table, i * 10 / 3 + 3, true));
		val = get_table_value(table.table, dst);
		*val = TEST_TABLE_SIZE - i;
		id = get_table_id(table.table, dst);
		*id = i * 10 / 3 + 3;
	}
	for (uint64_t i = 0; i < TEST_TABLE_SIZE / 2; i++) {
		// Occupied
		EXPECT_EQ(ERR_NO_ERR, search_table(&dst, &table, i * 10 / 3 + 3, false));
		EXPECT_EQ(TEST_TABLE_SIZE - i, *get_table_value(table.table, dst));
		EXPECT_EQ(i * 10 / 3 + 3, *get_table_id(table.table, dst));
		// Unoccupied
		EXPECT_EQ(ERR_NO_ERR, search_table(&dst, &table, i * 10 / 3 + 2, false));
		EXPECT_EQ(0, *get_table_value(table.table, dst));
		EXPECT_EQ(0, *get_table_id(table.table, dst));
	}
}

/**
 * Validates error handling of {@link resize_table}.
 */
TEST_F(HuffmanTest, resize_table_errs) {
	uint64_t tableDat[1];

	HuffmanHashTable table;
	table.size = TEST_TABLE_SIZE;
	table.table = NULL;

	EXPECT_EQ(ERR_NULL_PTR, resize_table(NULL, 2 * TEST_TABLE_SIZE));
	EXPECT_EQ(ERR_NULL_PTR, resize_table(&table, 2 * TEST_TABLE_SIZE));

	table.table = tableDat;
	table.size = 0;
	EXPECT_EQ(ERR_INVALID_VALUE, resize_table(&table, TEST_TABLE_SIZE));
	table.size = TEST_TABLE_SIZE;
	EXPECT_EQ(ERR_INVALID_VALUE, resize_table(&table, (uint64_t)0));
	EXPECT_EQ(ERR_INVALID_VALUE, resize_table(&table, TEST_TABLE_SIZE - 1));
	EXPECT_EQ(ERR_INVALID_VALUE, resize_table(&table, TEST_TABLE_SIZE));
}

/**
 * Validates output of {@link resize_table}.
 */
TEST_F(HuffmanTest, resize_table) {
	uint64_t* oldTable;
	uint64_t dst;
	uint64_t* val, *id;

	HuffmanHashTable table;

	// Case I: full table
	table.size = TEST_TABLE_SIZE;
	oldTable = (uint64_t*) malloc(2 * TEST_TABLE_SIZE * sizeof(uint64_t));
	ASSERT_NE((uint64_t*)NULL, oldTable);
	table.table = oldTable;
	memset(table.table, 0x00, 2 * TEST_TABLE_SIZE * sizeof(uint64_t));
	for (uint64_t i = 0; i < TEST_TABLE_SIZE; i++) {
		EXPECT_EQ(ERR_NO_ERR, search_table(&dst, &table, i * 10 / 3, true));
		val = get_table_value(table.table, dst);
		*val = i + 1;
		id = get_table_id(table.table, dst);
		*id = i * 10 / 3;
	}

	EXPECT_EQ(ERR_NO_ERR, resize_table(&table, TEST_TABLE_SIZE + 3));
	EXPECT_NE(oldTable, table.table);
	EXPECT_EQ(TEST_TABLE_SIZE + 3, table.size);

	// Validate contents
	for (uint64_t i = 0; i < TEST_TABLE_SIZE; i++) {
		EXPECT_EQ(ERR_NO_ERR, search_table(&dst, &table, i * 10 / 3, false));
		EXPECT_EQ(i + 1, *get_table_value(table.table, dst));
		EXPECT_EQ(i * 10 / 3, *get_table_id(table.table, dst));
		// Unoccupied
		EXPECT_EQ(ERR_NO_ERR, search_table(&dst, &table, i * 10 / 3 + 1, false));
		EXPECT_EQ(0, *get_table_value(table.table, dst));
		EXPECT_EQ(0, *get_table_id(table.table, dst));
	}
	free(table.table);


	// Case II: partially full table
	table.size = TEST_TABLE_SIZE;
	oldTable = (uint64_t*) malloc(2 * TEST_TABLE_SIZE * sizeof(uint64_t));
	ASSERT_NE((uint64_t*)NULL, oldTable);
	table.table = oldTable;
	memset(table.table, 0x00, 2 * TEST_TABLE_SIZE * sizeof(uint64_t));
	for (uint64_t i = 0; i < TEST_TABLE_SIZE / 2; i++) {
		EXPECT_EQ(ERR_NO_ERR, search_table(&dst, &table, i * 10 / 3 + 3, true));
		val = get_table_value(table.table, dst);
		*val = TEST_TABLE_SIZE - i;
		id = get_table_id(table.table, dst);
		*id = i * 10 / 3 + 3;
	}

	EXPECT_EQ(ERR_NO_ERR, resize_table(&table, TEST_TABLE_SIZE + 3));
	EXPECT_NE(oldTable, table.table);
	EXPECT_EQ(TEST_TABLE_SIZE + 3, table.size);

	// Validate contents
	for (uint64_t i = 0; i < TEST_TABLE_SIZE / 2; i++) {
		// Occupied
		EXPECT_EQ(ERR_NO_ERR, search_table(&dst, &table, i * 10 / 3 + 3, false));
		EXPECT_EQ(TEST_TABLE_SIZE - i, *get_table_value(table.table, dst));
		EXPECT_EQ(i * 10 / 3 + 3, *get_table_id(table.table, dst));
		// Unoccupied
		EXPECT_EQ(ERR_NO_ERR, search_table(&dst, &table, i * 10 / 3 + 2, false));
		EXPECT_EQ(0, *get_table_value(table.table, dst));
		EXPECT_EQ(0, *get_table_id(table.table, dst));
	}
	free(table.table);
}

/**
 * Validates error handling of {@link add_to_table}.
 */
TEST_F(HuffmanTest, add_to_table_errs) {
	EXPECT_FALSE(true);
}

/**
 * Validates output of {@link add_to_table}.
 */
TEST_F(HuffmanTest, add_to_table) {
	EXPECT_FALSE(true);
}

/**
 * Validates error handling of {@link generate_table}.
 */
TEST_F(HuffmanTest, generate_table_errs) {
	HuffmanHeader header;
	HuffmanHashTable table;
	uint8_t srcDummy[1];
	uint8_t* src;
	uint64_t srcSize;
	uint8_t wordSize;

	srcSize = 32;
	wordSize = 32;

	// Null pointer
	EXPECT_EQ(ERR_NULL_PTR, generate_table(NULL, &table, srcDummy, srcSize, wordSize));
	EXPECT_EQ(ERR_NULL_PTR, generate_table(&header, NULL, srcDummy, srcSize, wordSize));
	EXPECT_EQ(ERR_NULL_PTR, generate_table(&header, &table, NULL, srcSize, wordSize));

	// Invalid parameters
	wordSize = HUFFMAN_MIN_WORD_SIZE - 1;
	EXPECT_EQ(ERR_INVALID_VALUE, generate_table(&header, &table, srcDummy, srcSize, wordSize));
	wordSize = HUFFMAN_MAX_WORD_SIZE + 1;
	EXPECT_EQ(ERR_INVALID_VALUE, generate_table(&header, &table, srcDummy, srcSize, wordSize));
	wordSize = 32;
	srcSize = 0;
	EXPECT_EQ(ERR_INVALID_VALUE, generate_table(&header, &table, srcDummy, srcSize, wordSize));

	// todo test ERR_OVERFLOW if possible (probably not possible)
}

/**
 * Validates output of {@link generate_table}.
 */
TEST_F(HuffmanTest, generate_table) {
	HuffmanHeader header;
	HuffmanHashTable table;
	uint8_t* src;
	uint64_t srcSize;
	uint8_t wordSize;
	uint64_t i, dstIdx;

	// Test 0: word size 2, small volume, no padding
	srcSize = HUFFMAN_TEST_SMALL_VOLUME;
	wordSize = 2;
	table.table = NULL;
	src = (uint8_t*) malloc(srcSize + 1);
	ASSERT_NE((uint8_t*)NULL, src);
	for (i = 0; i < srcSize; i++) {
		src[i] = 0x1B; // [0, 1, 2, 3]
	}
	EXPECT_EQ(ERR_NO_ERR, generate_table(&header, &table, src, srcSize, wordSize));
	ASSERT_NE((uint64_t*)NULL, table.table);
	for (i = 0; i < 4; i++) {
		EXPECT_EQ(ERR_NO_ERR, search_table(&dstIdx, &table, i, false));
		EXPECT_EQ(srcSize, *get_table_value(table.table, dstIdx));
	}
	free(src);
	free(table.table);

//	// Test 1: word size 2, large volume
//	srcSize = HUFFMAN_TEST_LARGE_VOLUME;
//	wordSize = 2;
//	src = (uint8_t*) malloc(srcSize + 1);
//	ASSERT_NE((uint8_t*)NULL, src);
//	for (i = 0; i < srcSize; i++) {
//		src[i] = 0x0;
//	}
//	EXPECT_EQ(ERR_NO_ERR, generate_table(&header, &table, src, srcSize, wordSize));
//	free(src);
//
//	// Test 2: word size 24, medium volume
//	srcSize = HUFFMAN_TEST_MEDIUM_VOLUME;
//	wordSize = 24;
//	src = (uint8_t*) malloc(srcSize + 1);
//	ASSERT_NE((uint8_t*)NULL, src);
//	for (i = 0; i < srcSize; i++) {
//		src[i] = 0x0;
//	}
//	EXPECT_EQ(ERR_NO_ERR, generate_table(&header, &table, src, srcSize, wordSize));
//	free(src);
//
//	// Test 3: word size 32, medium volume
//	srcSize = HUFFMAN_TEST_MEDIUM_VOLUME;
//	wordSize = 32;
//	src = (uint8_t*) malloc(srcSize + 1);
//	ASSERT_NE((uint8_t*)NULL, src);
//	for (i = 0; i < srcSize; i++) {
//		src[i] = 0x0;
//	}
//	EXPECT_EQ(ERR_NO_ERR, generate_table(&header, &table, src, srcSize, wordSize));
//	free(src);
//
//	// Test 4: word size 3, large volume, padding
//	srcSize = HUFFMAN_TEST_LARGE_VOLUME;
//	wordSize = 3;
//	src = (uint8_t*) malloc(srcSize + 1);
//	ASSERT_NE((uint8_t*)NULL, src);
//	for (i = 0; i < srcSize; i++) {
//		src[i] = 0x0;
//	}
//	EXPECT_EQ(ERR_NO_ERR, generate_table(&header, &table, src, srcSize, wordSize));
//	free(src);
//
//	// Test 5: word size 8 + 5 = 13, medium volume, padding
//	srcSize = HUFFMAN_TEST_MEDIUM_VOLUME;
//	wordSize = 13;
//	src = (uint8_t*) malloc(srcSize + 1);
//	ASSERT_NE((uint8_t*)NULL, src);
//	for (i = 0; i < srcSize; i++) {
//		src[i] = 0x0;
//	}
//	EXPECT_EQ(ERR_NO_ERR, generate_table(&header, &table, src, srcSize, wordSize));
//	free(src);
//
//	// Test 6: word size 16 + 7 = 23, medium volume, padding
//	srcSize = HUFFMAN_TEST_MEDIUM_VOLUME;
//	wordSize = 23;
//	src = (uint8_t*) malloc(srcSize + 1);
//	ASSERT_NE((uint8_t*)NULL, src);
//	for (i = 0; i < srcSize; i++) {
//		src[i] = 0x0;
//	}
//	EXPECT_EQ(ERR_NO_ERR, generate_table(&header, &table, src, srcSize, wordSize));
//	free(src);
//
//	// Test 7: word size 24 + 6 = 30, small volume, no padding
//	srcSize = HUFFMAN_TEST_SMALL_VOLUME;
//	wordSize = 30;
//	src = (uint8_t*) malloc(srcSize + 1);
//	ASSERT_NE((uint8_t*)NULL, src);
//	for (i = 0; i < srcSize; i++) {
//		src[i] = 0x0;
//	}
//	EXPECT_EQ(ERR_NO_ERR, generate_table(&header, &table, src, srcSize, wordSize));
//	free(src);
//
//	// Test 8: word size 32 + 4 = 36, small volume, padding
//	srcSize = HUFFMAN_TEST_SMALL_VOLUME;
//	wordSize = 36;
//	src = (uint8_t*) malloc(srcSize + 1);
//	ASSERT_NE((uint8_t*)NULL, src);
//	for (i = 0; i < srcSize; i++) {
//		src[i] = 0x0;
//	}
//	EXPECT_EQ(ERR_NO_ERR, generate_table(&header, &table, src, srcSize, wordSize));
//	free(src);
//
//	// Test 9: word size 48, medium volume, padding
//	srcSize = HUFFMAN_TEST_MEDIUM_VOLUME;
//	wordSize = 48;
//	src = (uint8_t*) malloc(srcSize + 1);
//	ASSERT_NE((uint8_t*)NULL, src);
//	for (i = 0; i < srcSize; i++) {
//		src[i] = 0x0;
//	}
//	EXPECT_EQ(ERR_NO_ERR, generate_table(&header, &table, src, srcSize, wordSize));
//	free(src);
//
//	// Test 10: word size 56 + 3 = 59, small volume, no padding
//	srcSize = HUFFMAN_TEST_SMALL_VOLUME;
//	wordSize = 59;
//	src = (uint8_t*) malloc(srcSize + 1);
//	ASSERT_NE((uint8_t*)NULL, src);
//	for (i = 0; i < srcSize; i++) {
//		src[i] = 0x0;
//	}
//	EXPECT_EQ(ERR_NO_ERR, generate_table(&header, &table, src, srcSize, wordSize));
//	free(src);
//
//	// Test 11: word size 16 + 1, large volume, padding
//	srcSize = HUFFMAN_TEST_LARGE_VOLUME;
//	wordSize = 17;
//	src = (uint8_t*) malloc(srcSize + 1);
//	ASSERT_NE((uint8_t*)NULL, src);
//	for (i = 0; i < srcSize; i++) {
//		src[i] = 0x0;
//	}
//	EXPECT_EQ(ERR_NO_ERR, generate_table(&header, &table, src, srcSize, wordSize));
//	free(src);
//
//	// Test 12: word size 60, large volume, padding
//	srcSize = HUFFMAN_TEST_LARGE_VOLUME;
//	wordSize = HUFFMAN_MAX_WORD_SIZE;
//	src = (uint8_t*) malloc(srcSize + 1);
//	ASSERT_NE((uint8_t*)NULL, src);
//	for (i = 0; i < srcSize; i++) {
//		src[i] = 0x0;
//	}
//	EXPECT_EQ(ERR_NO_ERR, generate_table(&header, &table, src, srcSize, wordSize));
//	free(src);
}
