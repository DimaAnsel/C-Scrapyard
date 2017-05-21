/**
 * @file threadsafe_prioqueue_test.cc
 *
 * Unit tests to verify functionality of {@link threadsafe_prioqueue.h}.
 * 
 * @date    2017-05-20
 * @author  Noah Ansel
 */

#include "threadsafe_prioqueue.h"
#include "gtest/gtest.h"

/**
 *
 */
typedef struct Test_struct {
    int id;
    char data[3600];
} TestStruct;

#define NUM_ELEMENTS 32

// initializes a primitive test queue
THREADSAFE_PRIOQUEUE(INTEGER, int, NUM_ELEMENTS)


// initializes a struct test queue
THREADSAFE_PRIOQUEUE(TESTSTRUCT, TestStruct, NUM_ELEMENTS)



class ThreadsafePrioQueueTest : public ::testing::Test {
protected:
    virtual void SetUp() {
        TESTSTRUCT_init();
        INTEGER_init();
    }   
};


TEST_F(ThreadsafePrioQueueTest, put_ptr) {
    TestStruct* data = NULL;
    int* res = NULL;

    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, TESTSTRUCT_put_ptr(0, &data));
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, INTEGER_put_ptr(30, &res));

    EXPECT_FALSE(NULL == data);
    EXPECT_FALSE(NULL == res);

    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OUT_OF_BOUNDS, TESTSTRUCT_put_ptr(-1, &data));
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OUT_OF_BOUNDS, INTEGER_put_ptr(35, &res));
}

TEST_F(ThreadsafePrioQueueTest, put_unlock) {
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_NOT_LOCKED, TESTSTRUCT_put_unlock(2, false));

    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OUT_OF_BOUNDS, TESTSTRUCT_put_unlock(35, false));
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OUT_OF_BOUNDS, INTEGER_put_unlock(-1, true));

    TestStruct* data = NULL;
    int* res = NULL;

    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, TESTSTRUCT_put_ptr(0, &data));
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, INTEGER_put_ptr(30, &res));

    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, TESTSTRUCT_put_unlock(0, true));
    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OK, INTEGER_put_unlock(30, false));

    EXPECT_EQ(THREADSAFE_PRIOQUEUE_OCCUPIED_FLAG, TESTSTRUCT_nodes[0].controlFlag);
    EXPECT_EQ(0, INTEGER_nodes[30].controlFlag);
}
