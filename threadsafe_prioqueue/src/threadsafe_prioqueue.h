/**
 * @file threadsafe_prioqueue.h
 *
 * The data structure defined in this macro holds only one item of a
 * given type. Types are prioritized by order, but in order to not starve low
 * priority (high index) items, the pull functionality operates in a round robin
 * style until either a pull is made on an empty queue or the pull index is
 * manually set via set_pullIdx. A pull on an empty queue resets the next pull
 * to begin at index 0 (highest priority), while set_pullIdx specifies an the
 * index from which the next pull will begin looking.
 *
 * This data structure supports any number of providers and one consumer.
 * 
 * @date    2017-05-20
 * @author  Noah Ansel
 */

#ifndef THREADSAFE_PRIOQUEUE_H_
#define THREADSAFE_PRIOQUEUE_H_

/**Bit flag used to mark an occupied data slot.*/
#define THREADSAFE_PRIOQUEUE_OCCUPIED_FLAG (char)0x1
/**Bit flag used to mark a locked data slot.*/
#define THREADSAFE_PRIOQUEUE_LOCKED_FLAG (char)0x2

/**
 * @file
 * @enum ThreadsafePrioQueueError
 * Error codes for functions defined in {@link THREADSAFE_PRIOQUEUE}.
 */
typedef enum ThreadsafePrioQueueError_enum {
    /**Routine successfully executed.*/
    THREADSAFE_PRIOQUEUE_OK,
    /**Provided index was out of bounds.*/
    THREADSAFE_PRIOQUEUE_OUT_OF_BOUNDS,
    /**Tried to access locked data.*/
    THREADSAFE_PRIOQUEUE_LOCKED,
    /**Tried to unlock non-locked data.*/
    THREADSAFE_PRIOQUEUE_NOT_LOCKED,
    /**Tried to pull from an empty queue.*/
    THREADSAFE_PRIOQUEUE_EMPTY,
    /**Unknown error occurred.*/
    THREADSAFE_PRIOQUEUE_ERROR
} ThreadsafePrioQueueError;

/**
 * Creates a new thread-safe priority queue named NAME that holds
 * SIZE elements of type TYPE. Defines several methods for accessing the data
 * structure:
 *      * NAME_init(): Initializes the queue.
 *      * NAME_put_ptr(idx, ptr): Gives provider access to specified index.
 *      * NAME_put_unlock(idx, occupied): Unlocks a specified index and updates the occupied flag.
 *      * NAME_pull_ptr(ptr): Gives consumer access to the next set of data.
 *      * NAME_pull_unlock(): Unlocks the pull data.
 *      * NAME_set_pullIdx(): Sets the next pull to begin from specified index.
 */
#define THREADSAFE_PRIOQUEUE(NAME, TYPE, SIZE, RESET_ON_FAILED_PULL) \
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
    /**Number of occupied elements of the array.*/\
	int numOccupied;\
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
    NAME.numOccupied = 0;\
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
    /*Lock & update ptr, change numOccupied if needed.*/\
    NAME.nodes[idx].controlFlag |= THREADSAFE_PRIOQUEUE_LOCKED_FLAG;\
    (*ptr) = NAME.nodes[idx].data;\
    if ((NAME.nodes[idx].controlFlag & THREADSAFE_PRIOQUEUE_OCCUPIED_FLAG) != 0) {\
    	NAME.numOccupied--;\
    }\
    return THREADSAFE_PRIOQUEUE_OK;\
}\
\
/*Determine if a given element contains data.*/\
ThreadsafePrioQueueError NAME##_idx_occupied(int idx, char* ret) {\
	if (idx < 0 || idx >= SIZE) {\
	    /*Out of bounds, return error*/\
	    return THREADSAFE_PRIOQUEUE_OUT_OF_BOUNDS;\
	} else if ((NAME.nodes[idx].controlFlag & THREADSAFE_PRIOQUEUE_OCCUPIED_FLAG) != 0) {\
	    /*idx is occupied*/\
	    (*ret) = 1;\
	} else {\
		/*idx is not occupied*/\
		(*ret) = 0;\
	}\
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
        NAME.numOccupied++;\
    } else {\
        NAME.nodes[idx].controlFlag &= ~THREADSAFE_PRIOQUEUE_OCCUPIED_FLAG;\
    }\
    /*Unlock & return*/\
    NAME.nodes[idx].controlFlag &= ~THREADSAFE_PRIOQUEUE_LOCKED_FLAG;\
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
            NAME.nodes[i].controlFlag &= ~(THREADSAFE_PRIOQUEUE_LOCKED_FLAG | \
                THREADSAFE_PRIOQUEUE_OCCUPIED_FLAG);\
            NAME.numOccupied--;\
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
            if (RESET_ON_FAILED_PULL) {\
                NAME.pullIdx = 0;\
            }\
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
    NAME.nodes[SIZE].controlFlag &= ~THREADSAFE_PRIOQUEUE_LOCKED_FLAG;\
    return THREADSAFE_PRIOQUEUE_OK;\
}\
\
/*Sets pullIdx to set idx. Future pulls will begin from that index.*/\
ThreadsafePrioQueueError NAME##_set_pullIdx(int idx) {\
    NAME.pullIdx = idx;\
    return THREADSAFE_PRIOQUEUE_OK;\
}

#endif // THREADSAFE_PRIOQUEUE_H_
