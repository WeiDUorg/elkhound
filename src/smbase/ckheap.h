// ckheap.h            see license.txt for copyright and terms of use
// Hooks called by the rest of the code, to allow plugging in a heap
// checker.

#ifndef CKHEAP_H
#define CKHEAP_H

#ifdef __cplusplus
extern "C" {
#endif


// Hook to check heap integrity, and fail an assertion if it's bad
void checkHeap();

// Hook to check that a given pointer is a valid allocated object;
// fail assertion if not
void checkHeapNode(void* node);

// Hook to print allocation statistics to stderr
void malloc_stats();

// count # of malloc/free calls in program
unsigned numMallocCalls();
unsigned numFreeCalls();


// actions the heap walk iterator might request
enum HeapWalkOpts {
  HW_GO   = 0,         // keep going
  HW_STOP = 1,         // stop iterating
  HW_FREE = 2,         // free the block I just examined
};

// function for walking the heap
//   block:   pointer to the malloc'd block of memory
//   size:    # of bytes in the block; possibly larger than
//            what was requested
//   returns: bitwise OR of HeapWalkOpts options
// NOTE: you cannot call malloc or free inside this function
// (you can cause 'block' to be freed by returning HW_FREE)
typedef enum HeapWalkOpts (*HeapWalkFn)(void *block, int size);

// Hook for a heap walk
void walkMallocHeap(HeapWalkFn func);

#ifndef ELK_USE_HEAP_CHECKER

inline void checkHeap() {}
inline void checkHeapNode(void* node) { (void)node; }
inline void malloc_stats() {}
inline unsigned numMallocCalls() { return 0;  }
inline unsigned numFreeCalls() { return 0; }
inline void walkMallocHeap(HeapWalkFn func) { (void)func; }

#endif // ELK_USE_HEAP_CHECKER

#ifdef __cplusplus
} // extern "C"
#endif

#endif // CKHEAP_H
