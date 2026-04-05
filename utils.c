#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "lab4.h"

extern Node *g_root;

/* ----------------------------------------------------------------
 * TODO 29  check_integrity
 *
 * Use BFS to verify:
 *   - Every question node has both yes and no children (non-NULL).
 *   - Every solution node has both children NULL.
 * Return 1 if valid, 0 if any violation is found.
 * ---------------------------------------------------------------- */
int check_integrity(void) {
    Queue* q = malloc(sizeof(Queue));
    if (q == NULL) {
        return 0;
    }
    q_init(q);
    q_enqueue(q, g_root, 0);

    uint32_t currId = 0;
    while (!q_empty(q)) {
        Node* curr;
        int id;

        if (q_dequeue(q, &curr, &id) == 0) {
            q_free(q);
            return 0;
            //fail
        }
        if (curr->isQuestion == 1) { //question
            if (curr->yes == NULL || curr->no == NULL) {
                return 0;
            }
        } else { //answer
            if (curr->yes != NULL || curr->no != NULL) {
                return 0;
            }
        }
        if (curr->yes != NULL) {
            currId++;
            q_enqueue(q, curr->yes, currId);
        }
        if (curr->no != NULL) {
            currId++;
            q_enqueue(q, curr->no, currId);
        }
    }
}

/* ----------------------------------------------------------------
 * TODO 30  find_shortest_path
 *
 * Given the exact text of two solution leaves, display the
 * questions that distinguish them.  Use BFS with a parent-tracking
 * PathNode array to find both leaves, build ancestor arrays for
 * each, find the Lowest Common Ancestor (LCA), then print:
 *   - The shared path of questions both solutions pass through.
 *   - The divergence question (LCA) and which branch leads where.
 *
 * Display results with mvprintw.  Print an error if either
 * solution is not found.  Free all allocations before returning.
 * ---------------------------------------------------------------- */
void find_shortest_path(const char *sol1, const char *sol2) {
    if (g_root == NULL) {
        mvprintw(10, 2, "Error: knowledge base is empty.");
        refresh();
        return;
    }
    mvprintw(10, 2, "find_shortest_path not yet implemented.");
    refresh();
}
