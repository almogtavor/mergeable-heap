#include <stdlib.h>
#include <stdarg.h>
#include <signal.h>
#include <stdio.h>
#include <stdbool.h>

/////////////////////////////////////////////////////////////////////////////
// List operations
/////////////////////////////////////////////////////////////////////////////

struct list_t {
    int key;
    struct list_t *next;
    struct list_t *prev;
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

mergeable_heap *make_heap() {
    mergeable_heap *heap = malloc(sizeof(mergeable_heap));
    heap->head = NULL;
    heap->tail = NULL;
    return heap;
}

/**
 * Searching for the minimum element of an unsorted heap. Complexity time O(n).
 */
int find_min_in_unsorted_heap(list_t *current) {
    int min = current->key;
    current = current->next;

    while (current) {
        if (current->key < min) {
            min = current->key;
        }
        current = current->next;
    }

    return min;
}

//list_t *link_together(list_t *a, list_t *b) {
//    list_t *result;
//
//    if (!a) {
//        result = b;
//    } else {
//        result = a;
//        while (a->next) {
//            a = a->next;
//        }
//        a->next = b;
//    }
//
//    return result;
//}

list_t *merge_sorted(list_t *a, list_t *b) {
    list_t merged_heap;
    list_t *current = &merged_heap;

    while (a && b) {
        if (a->key < b->key) {
            current->next = a;
            a = a->next;
        } else {
            current->next = b;
            b = b->next;
        }
        current = current->next;
    }

    if (a) {
        current->next = a;
    } else {
        current->next = b;
    }

    return merged_heap.next;
}

/////////////////////////////////////////////////////////////////////////////
// 1. Mergreable heaps with sorted list
/////////////////////////////////////////////////////////////////////////////

/**
 * We receive the address of tail since we want to be able to update it.
 *
 * @param list
 * @param key
 * @return
 */
list_t *insert_sorted(list_t *list, int key, list_t **tail) {
    list_t *new_node = malloc(sizeof(list_t));
    new_node->key = key;
    new_node->next = NULL;

    // In case the key should get inserted to the root of the heap
    if (!list || key < list->key) {
        new_node->next = list;
        if (!list) {  // If the list was empty, update the tail to the new node
            *tail = new_node;
        }
        return new_node;
    }

    // Scanning the list until finding an instance of an element which is strictly larger than key
    list_t *current = list;
    while (current->next && current->next->key < key) {
        current = current->next;
    }

    if (current->next==NULL) {
        // Insert at the end of the list
        current->next = new_node;
        *tail = new_node;
    } else {
        // We take all elements from current that are greater than key, and put them in new_node after its root - key
        // Then we take new_node and put it after all elements of current that are smaller than key
        new_node->next = current->next;
        current->next = new_node;
    }
    return list;
}

void insert1(mergeable_heap *heap, int key) {
    heap->head = insert_sorted(heap->head, key, &heap->tail);
}

int minimum(mergeable_heap *heap, input_type input_t) {
    if (input_t == SORTED) {
        return heap->head->key;
    } else if (input_t == UNSORTED) {
        return find_min_in_unsorted_heap(heap->head);
    }
}

/**
 * To extract the minimum element we'll do the following:
 * 1. CalL minimum()
 * 2. # TODO:
 * @param heap
 * @return
 */
int sorted_extract_min(mergeable_heap *heap) {
    list_t *old_head = heap->head;
    int min = minimum(heap, SORTED);
    heap->head = heap->head->next;
    free(old_head); // We can now free head after its deletion
    return min;
}


mergeable_heap *union1(mergeable_heap *heap_a, mergeable_heap *heap_b) {
    mergeable_heap *merged_heap = malloc(sizeof(mergeable_heap));
    merged_heap->head = merge_sorted(heap_a->head, heap_b->head);

    free(heap_a);
    free(heap_b);

    return merged_heap;
}

/////////////////////////////////////////////////////////////////////////////
// 2. Mergreable heaps with unsorted lists
/////////////////////////////////////////////////////////////////////////////


/**
 * Inserts a new node with the specified key into the heap. This operation prepends the node
 * to the linked list that represents the heap, updating the heap's head, and its tail (in case of a new list).
 * Relevant for unsorted heaps. Complexity time O(1).
 *
 * @param heap The heap to insert into.
 * @param key The key of the new node to insert.
 */
void prepend(mergeable_heap *heap, int key) {
    list_t *new_node = malloc(sizeof(list_t));

    new_node->key = key;
    new_node->next = heap->head;

    if (heap->head==NULL){
        heap->tail = new_node;
    }
    heap->head = new_node;
}

/**
 * Deletes all nodes with the specified key from the list and updates the tail if necessary.
 * Relevant for unsorted heaps. Complexity time O(n).
 *
 * @param list The head of the list from which nodes will be deleted.
 * @param key The key of nodes to delete.
 * @param tail Pointer to the tail of the list.
 * @return The possibly new head of the list.
 */
list_t *delete_key(list_t *list, int key, list_t **tail) {
    list_t *current = list, *prev = NULL;

    while (current) {
        if (current->key == key) {
            list_t *toDelete = current;
            if (*tail == current) {  // Update tail if tail is being deleted
                *tail = prev;
            }
            if (prev != NULL) {
                prev->next = current->next;  // Skip the node being deleted
            } else {
                list = current->next;  // Update head if the head node is being deleted
            }
            current = current->next;
            free(toDelete);
        } else {
            prev = current;
            current = current->next;
        }
    }

    // If all nodes were deleted, set tail to NULL
    if (prev == NULL) *tail = NULL;

    return list;
}

int unsorted_extract_min(mergeable_heap *heap) {
    int min = minimum(heap, UNSORTED);
    heap->head = delete_key(heap->head, min, &heap->tail);
    return min;
}


/**
 * Merges two heaps into one in constant time, for unsorted heaps cases.
 */
mergeable_heap *unsorted_union(mergeable_heap *heap_a, mergeable_heap *heap_b) {
    mergeable_heap *merged_heap = make_heap();
    if (!heap_a->head) {
        free(heap_a);
        return heap_b;
    }
    if (!heap_b->head) {
        free(heap_b);
        return heap_a;
    }
    // Connect the tail of heap_a to the head of heap_b
    heap_a->tail->next = heap_b->head;

    // Set the head of the new merged heap to the head of heap_a & tail to tail of heap_b
    merged_heap->head = heap_a->head;
    merged_heap->tail = heap_b->tail;

    free(heap_a);
    free(heap_b);
    return merged_heap;
}
/**
 * Merges two heaps into one in constant time, for unsorted heaps cases.
 */


// Function to split the nodes of the given list into half
list_t* split(list_t* head) {
    list_t *fast = head, *slow = head;
    list_t *prev = NULL;
    while (fast && fast->next) {
        prev = slow;
        slow = slow->next;
        fast = fast->next->next;
    }
    if (prev) prev->next = NULL;  // Split the list into two halves
    return slow;
}

// Function to merge two sorted lists
list_t* merge(list_t* a, list_t* b) {
    list_t dummy, *tail = &dummy;
    dummy.next = NULL;
    while (a && b) {
        if (a->key < b->key) { tail->next = a; a = a->next; }
        else { tail->next = b; b = b->next; }
        tail = tail->next;
    }
    tail->next = a ? a : b;
    return dummy.next;
}

// Recursive merge sort function
list_t* mergeSort(list_t* head) {
    if (!head || !head->next) return head;
    list_t *second = split(head);
    head = mergeSort(head);
    second = mergeSort(second);
    return merge(head, second);
}



void destroy_heap(mergeable_heap *heap) {
    free(heap);
}

////


void test_sorted_heap(mergeable_heap *heap) {
    insert1(heap, 3);
    insert1(heap, 1);
    insert1(heap, 4);
    printf("Minimum: %d\n", minimum(heap, SORTED));
    printf("Extract Min: %d\n", sorted_extract_min(heap));
    printf("New Minimum after extraction: %d\n", minimum(heap, SORTED));

    mergeable_heap *heapA = make_heap();
    insert1(heapA, 2);
    insert1(heapA, 5);
    mergeable_heap *mergedHeapA = union1(heap, heapA);
    printf("Minimum of merged heap: %d\n", minimum(mergedHeapA, SORTED));
    printf("Extract Min of merged heap: %d\n", sorted_extract_min(mergedHeapA));
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
    printf("Extract Min: %d\n", unsorted_extract_min(heapB));
    printf("New Minimum after extraction: %d\n", minimum(heapB, UNSORTED));

    mergeable_heap *heapC = make_heap();
    prepend(heapC, 2);
    prepend(heapC, 5);
    mergeable_heap *mergedHeapB = unsorted_union(heapB, heapC);
    printf("Minimum of merged heap: %d\n", minimum(mergedHeapB, UNSORTED));
    printf("Extract Min of merged heap: %d\n", unsorted_extract_min(mergedHeapB));
}

// Helper function to print list
void printList(list_t *node) {
    while (node != NULL) {
        printf("%d -> ", node->key);
        node = node->next;
    }
    printf("NULL\n");
}

void handle_sigsegv(int sig) {
    printf("----------------Caught segmentation fault!---------------\n");
    printf("--------------------------ERROR!-------------------------\n");
    exit(EXIT_FAILURE);
}

int main() {
    signal(SIGSEGV, handle_sigsegv);

    mergeable_heap *A = make_heap();
    mergeable_heap *B = make_heap();
    char selectedChar;
    while (selectedChar != 'E') {
        input_type inputType;
        printf("How would you like to implement the mergeable heap? E for exit\n");
        printf("  1) Using sorted linked lists.\n");
        printf("  2) Using unsorted linked lists.\n");
        scanf("%c", &selectedChar);
        if (selectedChar != 'E') break;
        else if (selectedChar != '1') inputType = SORTED;
        else if (selectedChar != '2') inputType = UNSORTED;
        else exit(1);
        printf("Please choose an operation from the menu: \n");
        printf("  1) Make Heap \n");
        printf("  2) Insert \n");
        printf("  3) Union \n");
        printf("  4) Insert \n");
        scanf("%c", &selectedChar);
        if (selectedChar != 'E') break;
        else if (selectedChar != '1') inputType = SORTED;
        else if (selectedChar != '2') inputType = UNSORTED;
        else exit(1);
        printf("Press any key to continue. E to exit: \n");
        scanf("%c", &selectedChar);
    }

    printf("Testing Sorted List Heap\n");
    mergeable_heap *heap = make_heap();
    mergeable_heap *unsorted_heap = make_heap();

    test_sorted_heap(heap);
    test_unsorted_heap(unsorted_heap);

    // Merge sort example usage
    mergeable_heap *mergeSortHeap = make_heap();
    prepend(mergeSortHeap, 3);
    prepend(mergeSortHeap, 1);
    prepend(mergeSortHeap, 4);


    printf("Original List: ");
    printList(mergeSortHeap->head);
    // TODO: update the tail
    mergeSortHeap->head= mergeSort(mergeSortHeap->head);

    printf("Sorted List: ");
    printList(mergeSortHeap->head);

    // Once done, make sure to free the allocated heap
    destroy_heap(heap);
    return 0;
}