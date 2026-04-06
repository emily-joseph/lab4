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
                q_free(q);
                free(q);
                return 0;
            }
        } else { //answer
            if (curr->yes != NULL || curr->no != NULL) {
                q_free(q);
                free(q);
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
    q_free(q);
    free(q);
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
        int32_t pId; //will use -1 for root
        int32_t branch; //-1 root, 1 yes, 0 no
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
    int32_t found1 = -1; //-1 menas not found yet
    int32_t found2 = -1;
    uint32_t nextId = 0; //for putting nodes in queue

    //set up the root
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

    PathNode* a1 = calloc(nodeCount, sizeof(PathNode));
    if (a1 == NULL) {
        free(a1);
        free(arr);
        mvprintw(10, 2, "Error: Memory issue.");
        refresh();
        return;
    }
    PathNode* a2 = calloc(nodeCount, sizeof(PathNode));
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

    //save the lengths for traversal
    int32_t len1 = l1;
    int32_t len2 = l2;

    l1--; //addr of root node
    l2--;
    PathNode* lca = NULL;

    while (l1 > -1 && l2 >-1 && a1[l1].node == a2[l2].node) {
        lca = &a1[l1]; //goes first so lca doesn't need to be changed after
        l1--;
        l2--; //l2 points to the first unique node
    }

    //JUST GOTTA FIGURE OUT PRINTING NOW

    //clear and mvprintw from internet
    clear(); //from internet
    int row = 2;
    mvprintw(row, 2, "Printing solutions");
    row++;
    mvprintw(row, 4, "1) %s", sol1);
    row++;
    mvprintw(row, 4, "2) %s", sol2);
    row++;

    mvprintw(row, 2, "Shared path:");
    row++;

    //must start at end of a1 or a2 till lca

    //get index of lca within a1 and a2
    int32_t lcaIndex1 = -1;
    for (int32_t i = 0; i < len1; i++) {
        if (a1[i].node == lca->node) {
            lcaIndex1 = i;
            break;
        }
    }

    int32_t lcaIndex2 = -1;
    for (int32_t i = 0; i < len2; i++) {
        if (a2[i].node == lca->node) {
            lcaIndex2 = i;
            break;
        }
    }

    if (lcaIndex1 == -1 || lcaIndex2 == -1) {
        mvprintw(row++, 2, "Error: can't trace path.");
        refresh();
        free(a1);
        free(a2);
        free(arr);
        return;
    }

    //start at root not (index = len - 1) till the lca: SHARED PATH
    for (int32_t i = len1-1; i > lcaIndex1; i--) {
        if (a1[i].node->isQuestion == 1) {
            mvprintw(row, 4, "%s", a1[i].node->text);
            row++;
        }
    }


    row++;
    mvprintw(row, 2, "Divergence question:");
    row++;
    mvprintw(row, 4, "%s", a1[lcaIndex1].node->text);
    row++;

    PathNode* c1 = &a1[lcaIndex1-1]; //first unique node
    PathNode* c2 = &a2[lcaIndex2-1];

    char* yn = malloc(sizeof(char) * 4);
    if (c1 != NULL) {
        if (c1->branch == 1) {
                yn = "YES";
        } else {
            yn = "NO";
        }
        mvprintw(row, 4, "%s -> %s", yn, sol1); //how to print multiple variables from google
        row++;
    }

    if (c2 != NULL) {
        if (c2->branch == 1) {
                yn = "YES";
        } else {
            yn = "NO";
        }
        mvprintw(row, 4, "%s -> %s", yn, sol2); //how to print multiple variables from google
        row++;
    }

    refresh();
    free(a1);
    free(a2);
    free(arr);
}
