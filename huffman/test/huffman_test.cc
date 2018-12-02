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
class HuffmanTest : public ::testing::Test {
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
 * Validates output of {@link extract_bits}.
 */
TEST_F(HuffmanTest, extract_bits) {
    uint8_t testArr[128];
    uint8_t* testPtr = testArr;
    uint8_t size = 3;
    uint8_t start = 3;
    uint64_t result = 0;

    // Case I:
    // todo finish

    // Case II:
    // todo finish

    // Case III:
    // todo finish

    // Case IV:
    // todo finish
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
	EXPECT_EQ(ERR_INSUFFICIENT_SPACE, put_bits(&testPtr, &start, 0, value, size));
	EXPECT_EQ(ERR_INSUFFICIENT_SPACE, put_bits(&testPtr, &start, 1, value, 6));
	start = 0;
	EXPECT_EQ(ERR_INSUFFICIENT_SPACE, put_bits(&testPtr, &start, 1, value, 9));
	start = 7;
	EXPECT_EQ(ERR_INSUFFICIENT_SPACE, put_bits(&testPtr, &start, 2, value, 10));
}

/**
 * Validates output for {@link put_bits}.
 */
TEST_F(HuffmanTest, put_bits) {
	uint8_t testArr[128];
    uint8_t* testPtr = testArr;
	uint8_t* nullTest = NULL;
	uint8_t size = 3;
	uint8_t start = 3;
	uint64_t value = 0;

	// Case I:
	// todo finish

	// Case II:
	// todo finish

	// Case III:
	// todo finish

	// Case IV:
	// todo finish
}
