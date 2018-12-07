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
 * Test for huffman coding implementation.
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
	uint64_t value = 0;

	// null checks
	EXPECT_EQ(ERR_NULL_PTR, put_bits(NULL, &start, 128, value, size));
	EXPECT_EQ(ERR_NULL_PTR, put_bits(&nullTest, &start, 128, value, size));
	EXPECT_EQ(ERR_NULL_PTR, put_bits(&testPtr, NULL, 128, value, size));

	// input validation
	EXPECT_EQ(ERR_INVALID_VALUE, put_bits(&testPtr, &start, 128, value, 0));
	EXPECT_EQ(ERR_INVALID_VALUE, put_bits(&testPtr, &start, 128, value, 65));
	start = 8;
	EXPECT_EQ(ERR_INVALID_VALUE, put_bits(&testPtr, &start, 128, value, size));

	// size validation
	start = 3;
	EXPECT_EQ(ERR_INSUFFICIENT_SPACE,
			put_bits(&testPtr, &start, 0, value, size));
	EXPECT_EQ(ERR_INSUFFICIENT_SPACE, put_bits(&testPtr, &start, 1, value, 6));
	start = 0;
	EXPECT_EQ(ERR_INSUFFICIENT_SPACE, put_bits(&testPtr, &start, 1, value, 9));
	start = 7;
	EXPECT_EQ(ERR_INSUFFICIENT_SPACE, put_bits(&testPtr, &start, 2, value, 10));
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

	// start 0
	testArr[0] = 0xA5;
	testPtr = testArr;
	start = 0;
	size = 5;
	value = 0x08;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, 2, value, size));
	EXPECT_EQ(0x40, testArr[0]);
	EXPECT_EQ(testArr, testPtr);
	EXPECT_EQ(5, start);

	// start 1, limited size
	testArr[0] = 0x11;
	testPtr = testArr;
	start = 1;
	size = 6;
	value = 0x35;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, 1, value, size));
	EXPECT_EQ(0x6A, testArr[0]);
	EXPECT_EQ(testArr, testPtr);
	EXPECT_EQ(7, start);

	// start 2
	testArr[0] = 0x61;
	testPtr = testArr;
	start = 2;
	size = 4;
	value = 0xD;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, 2, value, size));
	EXPECT_EQ(0x74, testArr[0]);
	EXPECT_EQ(testArr, testPtr);
	EXPECT_EQ(6, start);

	// start 3
	testArr[0] = 0xB7;
	testPtr = testArr;
	start = 3;
	size = 2;
	value = 0x1;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, 2, value, size));
	EXPECT_EQ(0xA8, testArr[0]);
	EXPECT_EQ(testArr, testPtr);
	EXPECT_EQ(5, start);

	// start 4
	testArr[0] = 0x10;
	testPtr = testArr;
	start = 4;
	size = 2;
	value = 0x2;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, 16, value, size));
	EXPECT_EQ(0x18, testArr[0]);
	EXPECT_EQ(testArr, testPtr);
	EXPECT_EQ(6, start);

	// start 5, clip
	testArr[0] = 0xA9;
	testPtr = testArr;
	start = 5;
	size = 2;
	value = 0x5;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, 16, value, size));
	EXPECT_EQ(0xAA, testArr[0]);
	EXPECT_EQ(testArr, testPtr);
	EXPECT_EQ(7, start);

	// start 6
	testArr[0] = 0xFF;
	testPtr = testArr;
	start = 6;
	size = 1;
	value = 0x0;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, 16, value, size));
	EXPECT_EQ(0xFC, testArr[0]);
	EXPECT_EQ(testArr, testPtr);
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

	// start 0
	testPtr = testArr;
	start = 0;
	size = 8;
	value = 0x96;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, 16, value, size));
	EXPECT_EQ(0x96, testArr[0]);
	EXPECT_EQ(testArr + 1, testPtr);
	EXPECT_EQ(0, start);

	// start 1, bit 0 = 0, clip value bits
	testArr[0] = 0x00;
	testPtr = testArr;
	start = 1;
	size = 7;
	value = 0xB5;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, 16, value, size));
	EXPECT_EQ(0x35, testArr[0]);
	EXPECT_EQ(testArr + 1, testPtr);
	EXPECT_EQ(0, start);

	// start 1, bit 0 = 1
	testArr[0] = 0xFF;
	testPtr = testArr;
	start = 1;
	size = 7;
	value = 0x35;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, 16, value, size));
	EXPECT_EQ(0xB5, testArr[0]);
	EXPECT_EQ(testArr + 1, testPtr);
	EXPECT_EQ(0, start);

	// start 2
	testArr[0] = 0x70;
	testPtr = testArr;
	start = 2;
	size = 6;
	value = 0x15;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, 16, value, size));
	EXPECT_EQ(0x55, testArr[0]);
	EXPECT_EQ(testArr + 1, testPtr);
	EXPECT_EQ(0, start);

	// start 3, clip value
	testArr[0] = 0x39;
	testPtr = testArr;
	start = 3;
	size = 5;
	value = 0xFED06;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, 16, value, size));
	EXPECT_EQ(0x26, testArr[0]);
	EXPECT_EQ(testArr + 1, testPtr);
	EXPECT_EQ(0, start);

	// start 4, overwrite
	testArr[0] = 0xC3;
	testPtr = testArr;
	start = 4;
	size = 4;
	value = 0x2;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, 16, value, size));
	EXPECT_EQ(0xC2, testArr[0]);
	EXPECT_EQ(testArr + 1, testPtr);
	EXPECT_EQ(0, start);

	// start 5, overwrite
	testArr[0] = 0x06;
	testPtr = testArr;
	start = 5;
	size = 3;
	value = 0xA;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, 16, value, size));
	EXPECT_EQ(0x02, testArr[0]);
	EXPECT_EQ(testArr + 1, testPtr);
	EXPECT_EQ(0, start);

	// start 6
	testArr[0] = 0xC9;
	testPtr = testArr;
	start = 6;
	size = 2;
	value = 0x2;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, 16, value, size));
	EXPECT_EQ(0xCA, testArr[0]);
	EXPECT_EQ(testArr + 1, testPtr);
	EXPECT_EQ(0, start);

	// start 7
	testArr[0] = 0xFF;
	testPtr = testArr;
	start = 7;
	size = 1;
	value = 0x0;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, 16, value, size));
	EXPECT_EQ(0xFE, testArr[0]);
	EXPECT_EQ(testArr + 1, testPtr);
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

	// start 0, 7 byte, overwrite
	memset(testArr, 0xFF, 16);
	testPtr = testArr;
	start = 0;
	size = 51;
	value = 0x314053627D8C9;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, 16, value, size));
	EXPECT_EQ(0x62, testArr[0]);
	EXPECT_EQ(0x80, testArr[1]);
	EXPECT_EQ(0xA6, testArr[2]);
	EXPECT_EQ(0xC4, testArr[3]);
	EXPECT_EQ(0xFB, testArr[4]);
	EXPECT_EQ(0x19, testArr[5]);
	EXPECT_EQ(0x20, testArr[6]);
	EXPECT_EQ(testArr + 6, testPtr);
	EXPECT_EQ(3, start);

	// start 1, 5 byte, even byte size
	testArr[0] = 0x86;
	testPtr = testArr;
	start = 1;
	size = 32;
	value = 0x50417263;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, 16, value, size));
	EXPECT_EQ(0xA8, testArr[0]);
	EXPECT_EQ(0x20, testArr[1]);
	EXPECT_EQ(0xB9, testArr[2]);
	EXPECT_EQ(0x31, testArr[3]);
	EXPECT_EQ(testArr + 4, testPtr);
	EXPECT_EQ(1, start);

	// start 2, 3 byte
	testArr[0] = 0x9B;
	testPtr = testArr;
	start = 2;
	size = 19;
	value = 0x58C9F;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, 16, value, size));
	EXPECT_EQ(0xAC, testArr[0]);
	EXPECT_EQ(0x64, testArr[1]);
	EXPECT_EQ(0xF8, testArr[2]);
	EXPECT_EQ(testArr + 2, testPtr);
	EXPECT_EQ(5, start);

	// start 2, max size
	testArr[0] = 0x40;
	testPtr = testArr;
	start = 2;
	size = 64;
	value = 0x1A2B3C4D5E6F7089;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, 16, value, size));
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
	EXPECT_EQ(2, start);

	// start 3, 4 byte
	testArr[0] = 0xA5;
	testArr[3] = 0xFF;
	testPtr = testArr;
	start = 3;
	size = 25;
	value = 0x1C5A691;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, 16, value, size));
	EXPECT_EQ(0xBC, testArr[0]);
	EXPECT_EQ(0x5A, testArr[1]);
	EXPECT_EQ(0x69, testArr[2]);
	EXPECT_EQ(0x10, testArr[3]);
	EXPECT_EQ(testArr + 3, testPtr);
	EXPECT_EQ(4, start);

	// start 4, 8 byte
	testArr[0] = 0xC3;
	testPtr = testArr;
	start = 4;
	size = 59;
	value = 0x35386B43F5941E3;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, 16, value, size));
	EXPECT_EQ(0xC6, testArr[0]);
	EXPECT_EQ(0xA7, testArr[1]);
	EXPECT_EQ(0x0D, testArr[2]);
	EXPECT_EQ(0x68, testArr[3]);
	EXPECT_EQ(0x7E, testArr[4]);
	EXPECT_EQ(0xB2, testArr[5]);
	EXPECT_EQ(0x83, testArr[6]);
	EXPECT_EQ(0xC6, testArr[7]);
	EXPECT_EQ(testArr + 7, testPtr);
	EXPECT_EQ(7, start);

	// start 5, 2 byte
	testArr[0] = 0x00;
	testPtr = testArr;
	start = 5;
	size = 9;
	value = 0x14A;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, 16, value, size));
	EXPECT_EQ(0x05, testArr[0]);
	EXPECT_EQ(0x28, testArr[1]);
	EXPECT_EQ(testArr + 1, testPtr);
	EXPECT_EQ(6, start);

	// start 5, max size
	testArr[0] = 0x92;
	testPtr = testArr;
	start = 5;
	size = 64;
	value = 0xF0E1D2C3B4A59687;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, 16, value, size));
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
	EXPECT_EQ(5, start);

	// start 6, 2 byte
	testArr[0] = 0x7B;
	testPtr = testArr;
	start = 6;
	size = 5;
	value = 0x06;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, 16, value, size));
	EXPECT_EQ(0x78, testArr[0]);
	EXPECT_EQ(0xC0, testArr[1]);
	EXPECT_EQ(testArr + 1, testPtr);
	EXPECT_EQ(3, start);

	// start 7, 6 byte
	testArr[0] = 0x56;
	testPtr = testArr;
	start = 7;
	size = 40;
	value = 0x84217BDE6A;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, 16, value, size));
	EXPECT_EQ(0x57, testArr[0]);
	EXPECT_EQ(0x08, testArr[1]);
	EXPECT_EQ(0x42, testArr[2]);
	EXPECT_EQ(0xF7, testArr[3]);
	EXPECT_EQ(0xBC, testArr[4]);
	EXPECT_EQ(0xD4, testArr[5]);
	EXPECT_EQ(testArr + 5, testPtr);
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

	// start 0, 6 bytes, overwrite
	memset(testArr, 0xFF, 16);
	testPtr = testArr;
	start = 0;
	size = 48;
	value = 0x1574689AEB6C;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, 16, value, size));
	EXPECT_EQ(0x15, testArr[0]);
	EXPECT_EQ(0x74, testArr[1]);
	EXPECT_EQ(0x68, testArr[2]);
	EXPECT_EQ(0x9A, testArr[3]);
	EXPECT_EQ(0xEB, testArr[4]);
	EXPECT_EQ(0x6C, testArr[5]);
	EXPECT_EQ(testArr + 6, testPtr);
	EXPECT_EQ(0, start);

	// start 0, full size, limited size
	testPtr = testArr;
	start = 0;
	size = 64;
	value = 0xFEDCBA9876543210;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, 8, value, size));
	EXPECT_EQ(0xFE, testArr[0]);
	EXPECT_EQ(0xDC, testArr[1]);
	EXPECT_EQ(0xBA, testArr[2]);
	EXPECT_EQ(0x98, testArr[3]);
	EXPECT_EQ(0x76, testArr[4]);
	EXPECT_EQ(0x54, testArr[5]);
	EXPECT_EQ(0x32, testArr[6]);
	EXPECT_EQ(0x10, testArr[7]);
	EXPECT_EQ(testArr + 8, testPtr);
	EXPECT_EQ(0, start);

	// start 1, 7 bytes
	testArr[0] = 0x46;
	testArr[6] = 0xF0;
	testPtr = testArr;
	start = 1;
	size = 55;
	value = 0xE2194DAF1E3508;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, 16, value, size));
	EXPECT_EQ(0x62, testArr[0]);
	EXPECT_EQ(0x19, testArr[1]);
	EXPECT_EQ(0x4D, testArr[2]);
	EXPECT_EQ(0xAF, testArr[3]);
	EXPECT_EQ(0x1E, testArr[4]);
	EXPECT_EQ(0x35, testArr[5]);
	EXPECT_EQ(0x08, testArr[6]);
	EXPECT_EQ(testArr + 7, testPtr);
	EXPECT_EQ(0, start);

	// start 2, 5 bytes
	testArr[0] = 0x81;
	testPtr = testArr;
	start = 2;
	size = 38;
	value = 0x29ECA7F031;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, 16, value, size));
	EXPECT_EQ(0xA9, testArr[0]);
	EXPECT_EQ(0xEC, testArr[1]);
	EXPECT_EQ(0xA7, testArr[2]);
	EXPECT_EQ(0xF0, testArr[3]);
	EXPECT_EQ(0x31, testArr[4]);
	EXPECT_EQ(testArr + 5, testPtr);
	EXPECT_EQ(0, start);


	// start 3, 2 bytes
	testArr[0] = 0xF3;
	testArr[1] = 0x01;
	testPtr = testArr;
	start = 3;
	size = 13;
	value = 0x0C52;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, 16, value, size));
	EXPECT_EQ(0xEC, testArr[0]);
	EXPECT_EQ(0x52, testArr[1]);
	EXPECT_EQ(testArr + 2, testPtr);
	EXPECT_EQ(0, start);

	// start 4, 4 bytes
	testArr[0] = 0x55;
	testPtr = testArr;
	start = 4;
	size = 28;
	value = 0x6B54D3C;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, 16, value, size));
	EXPECT_EQ(0x56, testArr[0]);
	EXPECT_EQ(0xB5, testArr[1]);
	EXPECT_EQ(0x4D, testArr[2]);
	EXPECT_EQ(0x3C, testArr[3]);
	EXPECT_EQ(testArr + 4, testPtr);
	EXPECT_EQ(0, start);

	// start 5, 3 bytes, start mid-array
	testArr[10] = 0x66;
	testPtr = testArr + 10;
	start = 5;
	size = 19;
	value = 0x194DEA;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, 16, value, size));
	EXPECT_EQ(0x61, testArr[10]);
	EXPECT_EQ(0x4D, testArr[11]);
	EXPECT_EQ(0xEA, testArr[12]);
	EXPECT_EQ(testArr + 13, testPtr);
	EXPECT_EQ(0, start);

	// start 6, 2 byte
	testArr[0] = 0xE6;
	testPtr = testArr;
	start = 6;
	size = 10;
	value = 0x12A;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, 16, value, size));
	EXPECT_EQ(0xE5, testArr[0]);
	EXPECT_EQ(0x2A, testArr[1]);
	EXPECT_EQ(testArr + 2, testPtr);
	EXPECT_EQ(0, start);

	// start 7, 5 bytes, clip, start mid-array
	testArr[3] = 0x38;
	testPtr = testArr + 3;
	start = 7;
	size = 33;
	value = 0x85EAC5F684;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, 13, value, size));
	EXPECT_EQ(0x39, testArr[3]);
	EXPECT_EQ(0xEA, testArr[4]);
	EXPECT_EQ(0xC5, testArr[5]);
	EXPECT_EQ(0xF6, testArr[6]);
	EXPECT_EQ(0x84, testArr[7]);
	EXPECT_EQ(testArr + 8, testPtr);
	EXPECT_EQ(0, start);

	// start 7, 8 bytes
	testArr[0] = 0xF3;
	testPtr = testArr;
	start = 7;
	size = 57;
	value = 0x03A5B9E8C1D7368;
	EXPECT_EQ(ERR_NO_ERR, put_bits(&testPtr, &start, 16, value, size));
	EXPECT_EQ(0xF2, testArr[0]);
	EXPECT_EQ(0x3A, testArr[1]);
	EXPECT_EQ(0x5B, testArr[2]);
	EXPECT_EQ(0x9E, testArr[3]);
	EXPECT_EQ(0x8C, testArr[4]);
	EXPECT_EQ(0x1D, testArr[5]);
	EXPECT_EQ(0x73, testArr[6]);
	EXPECT_EQ(0x68, testArr[7]);
	EXPECT_EQ(testArr + 8, testPtr);
	EXPECT_EQ(0, start);
}
