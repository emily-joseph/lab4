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
            free(q);
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
    return 1;
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

    if (sol1 == NULL || sol2 == NULL) {
        mvprintw(10, 2, "Error: invalid solution text.");
        refresh();
        return;
    }
    
    int nodeCount = count_nodes(g_root);
    if (nodeCount <= 0) {
        mvprintw(10, 2, "Error: empty knowledge base.");
        refresh();
        return;
    }

    typedef struct {
        Node *node;
        int32_t pId;          //will use -1 for root
        int32_t branch;  //-1 root, 1 yes, 0 no
    } PathNode;

    PathNode *arr = calloc(nodeCount, sizeof(PathNode));
    if (arr == NULL) {
        mvprintw(10, 2, "Error: Memory issue.");
        refresh();
        return;
    }

    Queue* q = malloc(sizeof(Queue));
    if (q == NULL) {
        free(arr);
        mvprintw(10, 2, "Error: Memory issue.");
        refresh();
        return;
    }
    q_init(q);
    int32_t found1 = -1;
    int32_t found2 = -1;
    uint32_t nextId = 0;

    arr[0].node = g_root;
    arr[0].pId = -1;
    arr[0].branch = -1;
    q_enqueue(q, g_root, 0);

    while (!q_empty(q) && (found1==-1 || found2==-1)) {
        Node* curr;
        int id;

        if (!q_dequeue(q, &curr, &id)) {
            q_free(q);
            free(q);
            free(arr);
            mvprintw(10, 2, "Error: BFS failed.");
            refresh();
            return;
        }

        //trying to find the two solutions
        if (curr->isQuestion == 0) { //solution!
            if (strcmp(curr->text, sol1) == 0 && found1 == -1) {
                found1 = id;
            } 
            if (strcmp(curr->text, sol2) == 0 && found2 == -1) {
                found2 = id;
            } 
        }

        //update queue to continue search
        if (curr->yes != NULL) {
            nextId++;
            arr[nextId].node = curr->yes;
            arr[nextId].pId = id;
            arr[nextId].branch = 1;
            q_enqueue(q, curr->yes, nextId);
        }
        if (curr->no != NULL) {
            nextId++;
            arr[nextId].node = curr->no;
            arr[nextId].pId = id;
            arr[nextId].branch = 0;
            q_enqueue(q, curr->no, nextId);
        }
    }
    
    q_free(q); //dont need q anymore (BFS done)
    free(q);

    if (found1 == -1 || found2 == -1) {
        free(arr);
        mvprintw(10, 2, "Error: No such solutions.");
        refresh();
        return;
    }

    //now lets build ancestor array

    PathNode* a1 = malloc(nodeCount * sizeof(PathNode));
    if (a1 == NULL) {
        free(a1);
        free(arr);
        mvprintw(10, 2, "Error: Memory issue.");
        refresh();
        return;
    }
    PathNode* a2 = malloc(nodeCount * sizeof(PathNode));
    if (a2 == NULL) {
        free(a1);
        free(a2);
        free(arr);
        mvprintw(10, 2, "Error: Memory issue.");
        refresh();
        return;
    }

    int32_t l1 = 0;
    int32_t l2 = 0;

    int32_t currId = found1;
    while (currId != -1) { //reached root
        a1[l1] = arr[currId];
        l1++;
        currId = arr[currId].pId;
    }

    currId = found2;
    while (currId != -1) { //reached root
        a2[l2] = arr[currId];
        l2++;
        currId = arr[currId].pId;
    }

    l1--; //addr of root node
    l2--;
    PathNode* lca = NULL;

    while (l1 >= 0 && l2 >= 0 && a1[l1].node == a2[l2].node) {
        lca = &a1[l1]; //goes first so lca doesn't need to be changed after
        l1--;
        l2--;
    }

    //JUST GOTTA FIGURE OUT PRINTING NOW

    //mvprintw(10, 2, "find_shortest_path not yet implemented.");
    //refresh();
}
