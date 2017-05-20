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
