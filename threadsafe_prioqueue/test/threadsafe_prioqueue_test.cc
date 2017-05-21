/**
 * @file threadsafe_prioqueue_test.cc
 *
 * Unit tests to verify functionality of {@link threadsafe_prioqueue.h}.
 * 
 * @date    2017-05-20
 * @author  Noah Ansel
 */

#include <stdio.h>
#include <string.h>
#include "threadsafe_prioqueue.h"
#include "gtest/gtest.h"

/**
 *
 */
typedef struct Test_struct {
    int id;
    char data[2048];
} TestStruct;

#define NUM_ELEMENTS 32

// initializes a primitive test queue
THREADSAFE_PRIOQUEUE(INTEGER, int, NUM_ELEMENTS)


// initializes a struct test queue
THREADSAFE_PRIOQUEUE(TESTSTRUCT, TestStruct, NUM_ELEMENTS)


/**
 * Test fixture for {@link THREADSAFE_PRIOQUEUE} tests.
 */
class ThreadsafePrioQueueTest : public ::testing::Test {
protected:
    /**
     * Initializes queues.
     */
    virtual void SetUp() {
        TESTSTRUCT_init();
        INTEGER_init();
    }   
};

/**
 * Tests NAME_put_ptr functionality.
 */
TEST_F(ThreadsafePrioQueueTest, put_ptr) {
    TestStruct* data = NULL;
    int* res = NULL;
    int* res2 = NULL;

    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, TESTSTRUCT_put_ptr(0, &data));
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, INTEGER_put_ptr(30, &res));
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_LOCKED, INTEGER_put_ptr(30, &res2));
    EXPECT_EQ(TESTSTRUCT.nodes[0].data, data);
    EXPECT_EQ(INTEGER.nodes[30].data, res);

    // EXPECT_FALSE(NULL == data);
    // EXPECT_FALSE(NULL == res);

    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OUT_OF_BOUNDS, TESTSTRUCT_put_ptr(-1, &data));
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OUT_OF_BOUNDS, INTEGER_put_ptr(35, &res));
}

/**
 * Tests NAME_put_unlock functionality.
 */
TEST_F(ThreadsafePrioQueueTest, put_unlock) {
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_NOT_LOCKED, TESTSTRUCT_put_unlock(2, false));

    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OUT_OF_BOUNDS, TESTSTRUCT_put_unlock(35, false));
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OUT_OF_BOUNDS, INTEGER_put_unlock(-1, true));

    TestStruct* data = NULL;
    int* res = NULL;
    int* res2 = NULL;

    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, TESTSTRUCT_put_ptr(0, &data));
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, INTEGER_put_ptr(30, &res));
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_LOCKED, INTEGER_put_ptr(30, &res2));

    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, TESTSTRUCT_put_unlock(0, true));
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, INTEGER_put_unlock(30, false));

    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OCCUPIED_FLAG, TESTSTRUCT.nodes[0].controlFlag);
    EXPECT_EQ(0, INTEGER.nodes[30].controlFlag);

    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, INTEGER_put_ptr(30, &res2));
}

/**
 * Tests NAME_pull_ptr functionality.
 */
TEST_F(ThreadsafePrioQueueTest, pull_ptr) {
    TestStruct* dataIn = NULL;
    TestStruct* dataOut = NULL;
    int* intIn = NULL;
    int* intOut = NULL;

    // pull when empty
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_EMPTY, TESTSTRUCT_pull_ptr(&dataOut));

    // pull when written but empty
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, TESTSTRUCT_put_ptr(10, &dataIn));
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, TESTSTRUCT_put_unlock(10, false));
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_EMPTY, TESTSTRUCT_pull_ptr(&dataOut));

    // pull when non-empty but locked
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, TESTSTRUCT_put_ptr(4, &dataIn));
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_EMPTY, TESTSTRUCT_pull_ptr(&dataOut));

    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, TESTSTRUCT_put_unlock(4, true));

    // pull when non-empty & not locked
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, TESTSTRUCT_pull_ptr(&dataOut));
    EXPECT_FALSE(dataOut == NULL);
    EXPECT_EQ(dataIn, dataOut);

    // pull when pull locked
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_LOCKED, TESTSTRUCT_pull_ptr(&dataOut));
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, TESTSTRUCT_pull_unlock());

    // pull with need to wrap
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, TESTSTRUCT_put_ptr(1, &dataIn));
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, TESTSTRUCT_put_unlock(1, true));
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, TESTSTRUCT_put_ptr(4, &dataOut));
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, TESTSTRUCT_put_unlock(4, true));
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, TESTSTRUCT_pull_ptr(&dataOut));
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, TESTSTRUCT_pull_unlock());
    EXPECT_FALSE(dataOut == NULL);
    EXPECT_EQ(dataIn, dataOut);

    // pull when empty resets pullIdx
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, INTEGER_put_ptr(3, &intIn));
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, INTEGER_put_unlock(3, true));
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, INTEGER_put_ptr(4, &intIn));
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, INTEGER_pull_ptr(&intOut));
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, INTEGER_pull_unlock());
    EXPECT_EQ(4, INTEGER.pullIdx);
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_EMPTY, INTEGER_pull_ptr(&intOut));
    EXPECT_EQ(0, INTEGER.pullIdx);

}

/**
 * Tests NAME_pull_unlock functionality.
 */
TEST_F(ThreadsafePrioQueueTest, pull_unlock) {
    TestStruct* dataIn = NULL;
    TestStruct* dataOut = NULL;

    // unlock non-locked
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_NOT_LOCKED, TESTSTRUCT_pull_unlock());

    // put data
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, TESTSTRUCT_put_ptr(1, &dataIn));
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, TESTSTRUCT_put_unlock(1, true));

    // pull
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, TESTSTRUCT_pull_ptr(&dataOut));

    // check lock flag
    EXPECT_NE(0, TESTSTRUCT.nodes[32].controlFlag & THREADSAFE_PRIOQUEUE_LOCKED_FLAG);

    // unlock
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, TESTSTRUCT_pull_unlock());

    // check lock flag
    EXPECT_EQ(0, TESTSTRUCT.nodes[32].controlFlag & THREADSAFE_PRIOQUEUE_LOCKED_FLAG);
}

/**
 * Tests NAME_reset_pullIdx functionality.
 */
TEST_F(ThreadsafePrioQueueTest, reset_pullIdx) {
    int* dataIn = NULL;
    int* dataOut = NULL;

    // put data & pull
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, INTEGER_put_ptr(3, &dataIn));
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, INTEGER_put_unlock(3, true));
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, INTEGER_pull_ptr(&dataOut));
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, INTEGER_pull_unlock());
    
    // test pullIdx
    EXPECT_EQ(4, INTEGER.pullIdx);
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, INTEGER_reset_pullIdx());
    EXPECT_EQ(0, INTEGER.pullIdx);
}

/**
 * Verifies data integrity through input and output.
 */
TEST_F(ThreadsafePrioQueueTest, dataIntegrity) {
    int value;
    int* intIn;
    int* intOut;
    TestStruct data;
    TestStruct* dataIn;
    TestStruct* dataOut;
    data.id = 1380;
    snprintf(data.data, 2048, "This is a test. It can be large because this is a very big data structure.");


    // test for integer
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, INTEGER_put_ptr(31, &intIn));
    (*intIn) = 52469;
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, INTEGER_put_unlock(31, true));
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, INTEGER_pull_ptr(&intOut));
    value = (*intOut);
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, INTEGER_pull_unlock());
    EXPECT_EQ(52469, value);

    // test for struct
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, TESTSTRUCT_put_ptr(31, &dataIn));
    memcpy(dataIn, &data, sizeof(TestStruct));
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, TESTSTRUCT_put_unlock(31, true));
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, TESTSTRUCT_pull_ptr(&dataOut));
    EXPECT_EQ(0, memcmp(&data, dataOut, sizeof(TestStruct)));
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, TESTSTRUCT_pull_unlock());
}
