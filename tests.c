#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

struct list_t {
    int key;
    struct list_t *next;
};

enum input_type {
    SORTED,
    UNSORTED
};

typedef enum input_type input_type;
typedef struct list_t list_t;

typedef struct {
    list_t *head;
    list_t *tail;
} mergeable_heap;


// Declare the functions and types used from 10_2_solution.c
extern mergeable_heap* make_heap();
extern void insert(mergeable_heap* heap, int key, input_type type);
extern int minimum(mergeable_heap* heap, input_type type);
extern int sorted_extract_min(mergeable_heap* heap);

extern void prepend(mergeable_heap* heap, int key);
extern int extract_min(mergeable_heap* heap, input_type type);
extern mergeable_heap* union_heaps(mergeable_heap* heap1, mergeable_heap* heap2, input_type type);
extern void print_list(list_t* node);

void test_sorted_heap(mergeable_heap *heap) {
    insert(heap, 3, SORTED);
    insert(heap, 1, SORTED);
    insert(heap, 4, SORTED);
    printf("Minimum: %d\n", minimum(heap, SORTED));
    printf("Extract Min: %d\n", extract_min(heap, SORTED));
    printf("New Minimum after extraction: %d\n", minimum(heap, SORTED));

    mergeable_heap *heapA = make_heap();
    insert(heapA, 2, SORTED);
    insert(heapA, 5, SORTED);
    mergeable_heap *mergedHeapA = union_heaps(heap, heapA, SORTED);
    printf("Minimum of merged heap: %d\n", minimum(mergedHeapA, SORTED));
    printf("Extract Min of merged heap: %d\n", extract_min(mergedHeapA, SORTED));
//    Should result the text of: Testing Sorted List Heap
//    Minimum: 1
//    Extract Min: 1
//    New Minimum after extraction: 3
//    Minimum of merged heap: 2
//    Extract Min of merged heap: 2
}

void test_unsorted_heap(mergeable_heap *heapB) {
    printf("\nTesting Unsorted List Heap\n");
//    mergeable_heap *heapB = make_heap();
    prepend(heapB, 3);
    prepend(heapB, 1);
    prepend(heapB, 4);
    printf("Minimum: %d\n", minimum(heapB, UNSORTED));
    printf("Extract Min: %d\n", extract_min(heapB, UNSORTED));
    printf("New Minimum after extraction: %d\n", minimum(heapB, UNSORTED));

    mergeable_heap *heapC = make_heap();
    prepend(heapC, 2);
    prepend(heapC, 5);
    mergeable_heap *mergedHeapB = union_heaps(heapB, heapC, UNSORTED);
    printf("Minimum of merged heap: %d\n", minimum(mergedHeapB, UNSORTED));
    printf("Extract Min of merged heap: %d\n", extract_min(mergedHeapB, UNSORTED));
}

//void handle_sigsegv(int sig) {
//    printf("----------------Caught segmentation fault!---------------\n");
//    printf("--------------------------ERROR!-------------------------\n");
//    exit(EXIT_FAILURE);
//}

//int main() {
//    signal(SIGSEGV, handle_sigsegv);
//
//    printf("Testing Sorted List Heap\n");
//    mergeable_heap *heap = make_heap();
//    mergeable_heap *unsorted_heap = make_heap();
//
//    test_sorted_heap(heap);
//    test_unsorted_heap(unsorted_heap);
//
//    // Merge sort example usage
//    mergeable_heap *mergeSortHeap = make_heap();
//    prepend(mergeSortHeap, 3);
//    prepend(mergeSortHeap, 1);
//    prepend(mergeSortHeap, 4);
//    prepend(mergeSortHeap, 2);
//
//
//    printf("Original List: ");
//    print_list(mergeSortHeap->head);
//    sort(mergeSortHeap);
//
//    printf("Sorted List: ");
//    print_list(mergeSortHeap->head);
//
//    // Once done, make sure to free the allocated heap
////    destroy_heap(heap);
//    return 0;
//}