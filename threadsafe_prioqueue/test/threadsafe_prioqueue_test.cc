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

    }   
};


TEST_F(ThreadsafePrioQueueTest, put_ptr) {
    TestStruct* data = NULL;
    int* res = NULL;

    TESTSTRUCT_put_ptr(0, &data);
    INTEGER_put_ptr(30, &res);

    EXPECT_FALSE(NULL == data);
    EXPECT_FALSE(NULL == res);
}