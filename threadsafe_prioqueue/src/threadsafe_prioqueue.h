/**
 * @file threadsafe_prioqueue.h
 *
 * The data structure defined in this macro holds only the most recent item of a
 * given type. Types are prioritized by order, but in order to not starve low
 * priority (high index) items, the pull functionality operates in a round robin
 * style. This data structure supports one provider and one consumer.
 * 
 * @date    2017-05-20
 * @author  Noah Ansel
 */


#define THREADSAFE_PRIOQUEUE_OCCUPIED_FLAG (char)0x1
#define THREADSAFE_PRIOQUEUE_LOCKED_FLAG (char)0x2

typedef enum ThreadsafePrioQueueError_enum {
    THREADSAFE_PRIOQUEUE_OK,
    THREADSAFE_PRIOQUEUE_OUT_OF_BOUNDS,
    THREADSAFE_PRIOQUEUE_NOT_LOCKED
} ThreadsafePrioQueueError;

#define THREADSAFE_PRIOQUEUE(NAME, TYPE, SIZE) \
\
typedef struct NAME##_Node_struct {\
    TYPE* data;\
    char controlFlag;\
} NAME##_Node;\
\
TYPE NAME##_data[SIZE + 1];\
NAME##_Node NAME##_nodes[SIZE + 1];\
\
int NAME##_init() {\
    int i;\
    for (i = 0; i < SIZE + 1; i++) {\
        NAME##_nodes[i].data = &(NAME##_data[i]);\
        NAME##_nodes[i].controlFlag = 0;\
    }\
}\
\
int NAME##_put_ptr(int idx, TYPE** ptr) {\
    if (idx < 0 || idx >= SIZE) {\
        return THREADSAFE_PRIOQUEUE_OUT_OF_BOUNDS;\
    }\
    (*ptr) = NAME##_nodes[idx].data;\
    NAME##_nodes[idx].controlFlag |= THREADSAFE_PRIOQUEUE_LOCKED_FLAG;\
    return THREADSAFE_PRIOQUEUE_OK;\
}\
\
int NAME##_put_unlock(int idx, char occupied) {\
    if (idx < 0 || idx >= SIZE) {\
        return THREADSAFE_PRIOQUEUE_OUT_OF_BOUNDS;\
    } else if ((NAME##_nodes[idx].controlFlag & THREADSAFE_PRIOQUEUE_LOCKED_FLAG) == 0) {\
        return THREADSAFE_PRIOQUEUE_NOT_LOCKED;\
    }\
    if (occupied) {\
        NAME##_nodes[idx].controlFlag |= THREADSAFE_PRIOQUEUE_OCCUPIED_FLAG;\
    }\
    NAME##_nodes[idx].controlFlag ^= THREADSAFE_PRIOQUEUE_LOCKED_FLAG;\
    return THREADSAFE_PRIOQUEUE_OK;\
}
