/**
 * @file swarm_test.cc
 *
 * Unit tests to verify functionality of {@link swarm.c}.
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
	uint8_t testArr[128];
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
	uint8_t testArr[128];
	uint8_t* testPtr;
	uint8_t size;
	uint8_t start;
	uint64_t result = 0;

	for (uint8_t i = 0; i < 128; i++) {
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
	uint8_t testArr[128];
	uint8_t* testPtr;
	uint8_t size;
	uint8_t start;
	uint64_t result = 0;

	for (uint8_t i = 0; i < 128; i++) {
		testArr[i] = (((0x55 ^ i) & 0xF) << 4) | (i & 0xF);
	}
}

/**
 * Validates output of {@link extract_bits} for case 3 (multi-byte, non-even end).
 */
TEST_F(HuffmanTest, extract_bits_case3) {
	uint8_t testArr[128];
	uint8_t* testPtr;
	uint8_t size;
	uint8_t start;
	uint64_t result = 0;

	for (uint8_t i = 0; i < 128; i++) {
		testArr[i] = (((0x55 ^ i) & 0xF) << 4) | (i & 0xF);
	}
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
	uint8_t testArr[128];
	uint8_t* testPtr = testArr;
	uint8_t* nullTest = NULL;
	uint8_t size = 3;
	uint8_t start = 3;
	uint64_t value = 0;
}

/**
 * Validates output for {@link put_bits} for case 2 (single byte, even end).
 */
TEST_F(HuffmanTest, put_bits_case2) {
	uint8_t testArr[128];
	uint8_t* testPtr = testArr;
	uint8_t* nullTest = NULL;
	uint8_t size = 3;
	uint8_t start = 3;
	uint64_t value = 0;
}

/**
 * Validates output for {@link put_bits} for case 3 (multi-byte, non-even end).
 */
TEST_F(HuffmanTest, put_bits_case3) {
	uint8_t testArr[128];
	uint8_t* testPtr = testArr;
	uint8_t* nullTest = NULL;
	uint8_t size = 3;
	uint8_t start = 3;
	uint64_t value = 0;
}

/**
 * Validates output for {@link put_bits} for case 4 (multi-byte, even end).
 */
TEST_F(HuffmanTest, put_bits_case4) {
	uint8_t testArr[128];
	uint8_t* testPtr = testArr;
	uint8_t* nullTest = NULL;
	uint8_t size = 3;
	uint8_t start = 3;
	uint64_t value = 0;
}
