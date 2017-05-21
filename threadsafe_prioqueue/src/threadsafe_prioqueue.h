/**
 * @file threadsafe_prioqueue.h
 *
 * The data structure defined in this macro holds only the most recent item of a
 * given type. Types are prioritized by order, but in order to not starve low
 * priority (high index) items, the pull functionality operates in a round robin
 * style.
 * 
 * @date    2017-05-20
 * @author  Noah Ansel
 */


#define THREADSAFE_PRIOQUEUE_USED_FLAG 0x1


#define THREADSAFE_PRIOQUEUE(NAME, TYPE, SIZE) \
\
enum NAME##_error {\
    /***/\
    NAME##_OUT_OF_BOUNDS\
}\
\
typedef struct NAME##_Node_struct {\
    /***/\
    TYPE data[1];\
    /***/\
    char controlFlag;\
} NAME##_Node;\
\
NAME##_Node NAME##_nodes[SIZE + 1];\
\
int NAME##_put_ptr(int idx, TYPE** ptr) {\
    if (idx >= SIZE) {\
    }\
    (*ptr) = NAME##_nodes[idx].data;\
    NAME##_nodes[idx].controlFlag |= THREADSAFE_PRIOQUEUE_USED_FLAG;\
}
