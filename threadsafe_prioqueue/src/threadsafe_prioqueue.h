/**
 * @file threadsafe_prioqueue.h
 *
 * The data structure defined in this macro holds only the most recent item of a
 * given type. Types are prioritized by order, but in order to not starve low
 * priority (high index) items, the pull functionality operates in a round robin
 * style. This data structure supports any number of providers and one consumer.
 * 
 * @date    2017-05-20
 * @author  Noah Ansel
 */

#ifndef THREADSAFE_PRIOQUEUE_H_
#define THREADSAFE_PRIOQUEUE_H_

#define THREADSAFE_PRIOQUEUE_OCCUPIED_FLAG (char)0x1
#define THREADSAFE_PRIOQUEUE_LOCKED_FLAG (char)0x2

/**
 * @file
 * @enum ThreadsafePrioQueueError
 * Error codes for functions THREADSAFE_PRIOQUEUE()
 */
typedef enum ThreadsafePrioQueueError_enum {
    THREADSAFE_PRIOQUEUE_OK,
    THREADSAFE_PRIOQUEUE_OUT_OF_BOUNDS,
    THREADSAFE_PRIOQUEUE_LOCKED,
    THREADSAFE_PRIOQUEUE_NOT_LOCKED,
    THREADSAFE_PRIOQUEUE_EMPTY,
    THREADSAFE_PRIOQUEUE_ERROR
} ThreadsafePrioQueueError;

/**
 * Creates a new thread-safe priority queue named NAME that holds
 * SIZE elements of type TYPE. Defines several methods for accessing the data
 * structure. 
 */
#define THREADSAFE_PRIOQUEUE(NAME, TYPE, SIZE) \
\
/*Container for data & metadata.*/\
typedef struct NAME##_Node_struct {\
    /*Pointer to data.*/\
    TYPE* data;\
    /*Bit flags used to mark occupied & locks.*/\
    char controlFlag;\
} NAME##_Node;\
\
/*Data structure to hold all data for queue.*/\
typedef struct NAME##_buffer_struct {\
    /**Queue data.*/\
    TYPE data[SIZE + 1];\
    /**Metadata for queue data.*/\
    NAME##_Node nodes[SIZE + 1];\
    /**Index of next pull.*/\
    int pullIdx;\
} NAME##_buffer;\
\
/*Variable storing all data for queue.*/\
NAME##_buffer NAME;\
\
/*Initializes the queue.*/\
ThreadsafePrioQueueError NAME##_init() {\
    int i;\
    /*Map all nodes to data*/\
    for (i = 0; i < SIZE + 1; i++) {\
        NAME.nodes[i].data = &(NAME.data[i]);\
        NAME.nodes[i].controlFlag = 0;\
    }\
    NAME.pullIdx = 0;\
    return THREADSAFE_PRIOQUEUE_OK;\
}\
\
/*If idx is not locked, updates ptr to location to put data.*/\
ThreadsafePrioQueueError NAME##_put_ptr(int idx, TYPE** ptr) {\
    if (idx < 0 || idx >= SIZE) {\
        /*Out of bounds, return error*/\
        return THREADSAFE_PRIOQUEUE_OUT_OF_BOUNDS;\
    } else if ((NAME.nodes[idx].controlFlag & THREADSAFE_PRIOQUEUE_LOCKED_FLAG) != 0) {\
        /*idx locked, return error*/\
        return THREADSAFE_PRIOQUEUE_LOCKED;\
    }\
    /*Lock & update ptr, return*/\
    NAME.nodes[idx].controlFlag |= THREADSAFE_PRIOQUEUE_LOCKED_FLAG;\
    (*ptr) = NAME.nodes[idx].data;\
    return THREADSAFE_PRIOQUEUE_OK;\
}\
\
/*Unlocks the specified idx and updates the occupied flag.*/\
ThreadsafePrioQueueError NAME##_put_unlock(int idx, char occupied) {\
    if (idx < 0 || idx >= SIZE) {\
        /*Out of bounds, return error*/\
        return THREADSAFE_PRIOQUEUE_OUT_OF_BOUNDS;\
    } else if ((NAME.nodes[idx].controlFlag & THREADSAFE_PRIOQUEUE_LOCKED_FLAG) == 0) {\
        /*Not locked, return error*/\
        return THREADSAFE_PRIOQUEUE_NOT_LOCKED;\
    }\
    /*Update occupied flag*/\
    if (occupied) {\
        NAME.nodes[idx].controlFlag |= THREADSAFE_PRIOQUEUE_OCCUPIED_FLAG;\
    }\
    /*Unlock & return*/\
    NAME.nodes[idx].controlFlag ^= THREADSAFE_PRIOQUEUE_LOCKED_FLAG;\
    return THREADSAFE_PRIOQUEUE_OK;\
}\
\
/*If pull is not locked, updates ptr to location to pull data from.*/\
ThreadsafePrioQueueError NAME##_pull_ptr(TYPE** ptr) {\
    if ((NAME.nodes[SIZE].controlFlag & THREADSAFE_PRIOQUEUE_LOCKED_FLAG) != 0) {\
        /*Pull locked, return error*/\
        return THREADSAFE_PRIOQUEUE_LOCKED;\
    }\
    /*Get pullIdx now for thread safety.*/\
    int i, firstIdx = NAME.pullIdx;\
    TYPE* temp;\
    /*Begin searching from pullIdx.*/\
    for (i = firstIdx;;) {\
        if (((NAME.nodes[i].controlFlag & THREADSAFE_PRIOQUEUE_OCCUPIED_FLAG) != 0) &&\
                ((NAME.nodes[i].controlFlag & THREADSAFE_PRIOQUEUE_LOCKED_FLAG) == 0)) {\
            /*Found an unlocked occupied data slot, atomically swap data and mark as unoccupied*/\
            /*Lock*/\
            NAME.nodes[i].controlFlag |= THREADSAFE_PRIOQUEUE_LOCKED_FLAG;\
            NAME.nodes[SIZE].controlFlag |= THREADSAFE_PRIOQUEUE_LOCKED_FLAG;\
            /*Swap*/\
            temp = NAME.nodes[SIZE].data;\
            NAME.nodes[SIZE].data = NAME.nodes[i].data;\
            NAME.nodes[i].data = temp;\
            /*Unlock, free*/\
            NAME.nodes[i].controlFlag ^= (THREADSAFE_PRIOQUEUE_LOCKED_FLAG | \
                THREADSAFE_PRIOQUEUE_OCCUPIED_FLAG);\
            /*Update ptr & pullIdx, return OK*/\
            NAME.pullIdx = (i + 1) % SIZE;\
            (*ptr) = NAME.nodes[SIZE].data;\
            return THREADSAFE_PRIOQUEUE_OK;\
        }\
        /*Increment, wrap, check if completed cycle.*/\
        i++;\
        if (i == SIZE) {\
            i = 0;\
        }\
        if (i == firstIdx) {\
            /*Reset pullIdx, return error*/\
            NAME.pullIdx = 0;\
            return THREADSAFE_PRIOQUEUE_EMPTY;\
        }\
    }\
    /*Unknown error occurred*/\
    return THREADSAFE_PRIOQUEUE_ERROR;\
}\
\
/*Unlocks the pull data.*/\
ThreadsafePrioQueueError NAME##_pull_unlock() {\
    if ((NAME.nodes[SIZE].controlFlag & THREADSAFE_PRIOQUEUE_LOCKED_FLAG) == 0) {\
        /*Pull not locked, return error*/\
        return THREADSAFE_PRIOQUEUE_NOT_LOCKED;\
    }\
    /*Unlock & return*/\
    NAME.nodes[SIZE].controlFlag ^= THREADSAFE_PRIOQUEUE_LOCKED_FLAG;\
    return THREADSAFE_PRIOQUEUE_OK;\
}\
\
/*Resets pullIdx to 0. Future pulls will begin from 0 (highest priority).*/\
ThreadsafePrioQueueError NAME##_reset_pullIdx() {\
    NAME.pullIdx = 0;\
}

#endif // THREADSAFE_PRIOQUEUE_H_
