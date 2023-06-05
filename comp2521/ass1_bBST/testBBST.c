// COMP2521 23T1 Assignment 1

// To make your own tests, make a function, and modify main.

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "bBST.h"

// Once again, don't do this - you should not access
// the internals of the ADT in your program
// This is bad practice.
typedef struct listnode {
    int value;
    struct listnode *next;
} *ListNode;
struct list {
    ListNode head;
    ListNode tail;
};

#define RED "\033[0;31m"
#define GRN "\033[0;32m"
#define YLW ""
#define BLU "\033[0;34m"
#define CLR "\033[0m"
// Don't use macros in your assignment, especially no variadic ones.
#define PERR(msg, ...) {fprintf(stderr, RED "-- " msg CLR, ## __VA_ARGS__); TreeFree(t); return false;}
#define PLERR(msg, ...) {fprintf(stderr, RED "-- " msg CLR, ## __VA_ARGS__); ListFree(l); TreeFree(t); return false;}

#define SIZE 11

static Tree makeTree() {
    Tree t = TreeNew();
    int vals[SIZE] = {10, 15, 25, 20, 23, 5, 1, 3, 22, 21, 17};
    for (int i = 0; i < SIZE; i++)
        TreeInsert(t, vals[i]);
    return t;
}

static bool testTreeInsertSearch(void) {
    printf(YLW "%s...\n" CLR, __func__);
    int vals[SIZE] = {10, 15, 25, 20, 23, 5, 1, 3, 22, 21, 17};
    int absent[3] = {2, 30, -2};
    Tree t = TreeNew();

    if (TreeSearch(t, vals[0]))
        PERR("Found value in empty tree.\n");
    for (int i = 0; i < SIZE; i++) {
        if (!TreeInsert(t, vals[i]))
            PERR("Failed to insert %d.\n", vals[i]);
        if (!TreeSearch(t, vals[i]))
            PERR("Failed to find %d after insertion.\n", vals[i]);
    }
    for (int i = 0; i < SIZE; i++)
        if (TreeInsert(t, vals[i]))
            PERR("Insert of %d succeeded twice.\n", vals[i]);
    for (int i = 0; i < SIZE; i++)
        if (!TreeSearch(t, vals[i]))
            PERR("Failed to find %d in final tree.\n", vals[i]);
    for (int i = 0; i < 3; i++)
        if (TreeSearch(t, absent[i]))
            PERR("Found %d in final tree.\n", absent[i]);
    
    TreeFree(t);
    return true;
}

static bool testTreeDelete(void) {
    printf(YLW "%s...\n" CLR, __func__);
    Tree t = makeTree();
    int vals[SIZE] = {15, 25, 22, 5, 23, 1, 17, 10, 20, 21, 3};

    for (int i = 0; i < SIZE; i++) {
        if (!TreeDelete(t, vals[i]))
            PERR("Failed to delete %d.\n", vals[i]);
        if (TreeSearch(t, vals[i]))
            PERR("Found %d after deletion.\n", vals[i]);
        for (int j = i + 1; j < SIZE; j++)
            if (!TreeSearch(t, vals[j]))
                PERR("Failed to find %d but not deleted (after deleting %d).\n", vals[j], vals[i]);
    }

    TreeFree(t);
    return true;
}

static bool testTreeToList(void) {
    printf(YLW "%s...\n" CLR, __func__);
    Tree t = makeTree();
    int exp[11] = {1, 3, 5, 10, 15, 17, 20, 21, 22, 23, 25};

    List l = TreeToList(t);
    printf(BLU "-- TreeList returned: [");
    ListNode curr = l->head;

    int index = -1;
    int indexVal = -1;
    int i = 0;
    while (curr && i < 11) {
        printf("%d", curr->value);
        if (index == -1 && curr->value != exp[i]) {
            index = i;
            indexVal = curr->value;
        }
        if (curr->next)
            printf(", ");
        curr = curr->next;
        i++;
    }

    if (curr != NULL)
        printf("...");
    printf("]\n" CLR);
    if (i != 11 || curr != NULL)
        PLERR("List sizes mismatched!\n");
    if (index != -1)
        PLERR("Expected %d not %d at index %d.\n", exp[index], indexVal, index);

    ListFree(l);
    TreeFree(t);
    return true;
}

static bool testTreeKthSmallest(void) {
    printf(YLW "%s...\n" CLR, __func__);
    Tree t = makeTree();
    int kVals[4] = {1, 11, 2, 5};
    int results[4] = {1, 25, 3, 15};

    for (int i = 0; i < 4; i++) {
        int val = TreeKthSmallest(t, kVals[i]);
        if (val != results[i])
            PERR("Expected %d but got %d for %d-th smallest.\n", results[i], val, kVals[i]);
    }
    
    TreeFree(t);
    return true;
}

static bool testTreeKthLargest(void) {
    printf(YLW "%s...\n" CLR, __func__);
    Tree t = makeTree();
    int kVals[4] = {1, 11, 2, 5};
    int results[4] = {25, 1, 23, 20};

    for (int i = 0; i < 4; i++) {
        int val = TreeKthLargest(t, kVals[i]);
        if (val != results[i])
            PERR("Expected %d but got %d for %d-th largest.\n", results[i], val, kVals[i]);
    }

    TreeFree(t);
    return true;
}

static bool testTreeLCA(void) {
    printf(YLW "%s...\n" CLR, __func__);
    Tree t = makeTree();
    int aVals[6] = {10, 23, 20, 15, 3, -2};
    int bVals[6] = {20, 20, 7, 22, 5, 30};
    int results[6] = {15, 21, UNDEFINED, 15, 5, UNDEFINED};
    
    for (int i = 0; i < 6; i++) {
        int result = TreeLCA(t, aVals[i], bVals[i]);
        if (result != results[i])
            PERR("Expected %d but got %d for LCA(%d, %d).\n", results[i], result, aVals[i], bVals[i]);
    }

    TreeFree(t);
    return true;
}

static bool testTreeFloor(void) {
    printf(YLW "%s...\n" CLR, __func__);
    Tree t = makeTree();
    int vals[7] = {10, 2, 15, 24, -2, 16, 30};
    int results[7] = {10, 1, 15, 23, UNDEFINED, 15, 25};

    for (int i = 0; i < 7; i++) {
        int result = TreeFloor(t, vals[i]);
        if (result != results[i])
            PERR("Expected %d but got %d as floor of %d.\n", results[i], result, vals[i]);
    }

    TreeFree(t);
    return true;
}

static bool testTreeCeiling(void) {
    printf(YLW "%s...\n" CLR, __func__);
    Tree t = makeTree();
    int vals[7] = {10, 2, 15, 18, -2, 16, 30};
    int results[7] = {10, 3, 15, 20, 1, 17, UNDEFINED};

    for (int i = 0; i < 7; i++) {
        int result = TreeCeiling(t, vals[i]);
        if (result != results[i])
            PERR("Expected %d but got %d as ceiling of %d.\n", results[i], result, vals[i]);
    }

    TreeFree(t);
    return true;
}

static bool runTreeSearchBetween(Tree t, int lower, int upper, int *exp, int len) {
    List l = TreeSearchBetween(t, lower, upper);
    printf(BLU "-- TreeSearchBetween(%d, %d) returned: [", lower, upper);
    ListNode curr = l->head;

    int index = -1;
    int indexVal = -1;
    int i = 0;
    while (curr && i < len) {
        printf("%d", curr->value);
        if (index == -1 && curr->value != exp[i]) {
            index = i;
            indexVal = curr->value;
        }
        if (curr->next)
            printf(", ");
        curr = curr->next;
        i++;
    }

    if (curr != NULL)
        printf("...");
    printf("]\n" CLR);
    if (i != len || curr != NULL)
        PLERR("List sizes mismatched!\n");
    if (index != -1)
        PLERR("Expected %d not %d at index %d.\n", exp[index], indexVal, index);

    ListFree(l);
    return true;
}

static bool testTreeSearchBetween(void) {
    printf(YLW "%s...\n" CLR, __func__);
    Tree t = makeTree();
    int lowers[5] = {1, 17, 10, -2, 11};
    int uppers[5] = {25, 17, 30, 16, 14};
    int exp[5][11] = {
        {1, 3, 5, 10, 15, 17, 20, 21, 22, 23, 25},
        {17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {10, 15, 17, 20, 21, 22, 23, 25, 0, 0, 0},
        {1, 3, 5, 10, 15, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };
    int lens[5] = {11, 1, 8, 5, 0};

    for (int i = 0; i < 5; i++)
        if (!runTreeSearchBetween(t, lowers[i], uppers[i], exp[i], lens[i]))
            return false;

    TreeFree(t);
    return true;
}

int main(int argc, char **argv) {
    bool (*tests[])(void) = {
        testTreeInsertSearch,
        testTreeDelete,
        testTreeToList,
        testTreeKthSmallest,
        testTreeKthLargest,
        testTreeLCA,
        testTreeFloor,
        testTreeCeiling,
        testTreeSearchBetween,
        NULL
    };
    int numTests = 9;
    int success = 0;
    int total = 0;

    if (argc == 1) {
        for (int i = 0; tests[i]; i++) {
            bool result = (tests[i])();
            if (result)
                printf(GRN "Test passed.\n\n" CLR);
            else
                printf(RED "Test failed.\n\n" CLR);
            success += result;
            total++;
        }
    } else {
        for (int i = 1; i < argc; i++) {
            int testNo = atoi(argv[i]) - 1;
            if (testNo < 0 || testNo >= numTests)
                continue;

            bool result = (tests[testNo])();
            if (result)
                printf(GRN "Test passed.\n\n" CLR);
            else
                printf(RED "Test failed.\n\n" CLR);
            success += result;
            total++;
        }
    }

    if (total == success)
        printf(GRN "All %d tests passed!\n" CLR, total);
    else
        printf(RED "%d of %d tests passed.\n" CLR, success, total);
    return 0;
}