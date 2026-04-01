#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lab4.h"

/* ----------------------------------------------------------------
 * ds.c  --  all data structures for the Tech Support Diagnosis Tool
 *
 * Implement every function marked TODO.  The only functions in this
 * entire lab permitted to use recursion are free_tree and count_nodes.
 * Everything else must be iterative.
 * ---------------------------------------------------------------- */


/* ====== Tree nodes ============================================== */

/* TODO 1 */
Node *create_question_node(const char *question) {
    Node *q = (Node*)malloc(sizeof(Node));
    q->text = strdup(question);
    q->isQuestion = 1;
    q->yes = NULL;
    q->no = NULL;
    return q;
}

/* TODO 2 */
Node *create_solution_node(const char *solution) {
    Node *s = (Node*)malloc(sizeof(Node));
    s->text = strdup(solution);
    s->isQuestion = 0;
    s->yes = NULL;
    s->no = NULL;
    return s;
}

/* TODO 3  (recursion allowed) */
void free_tree(Node *node) {
    if (node == NULL) return;
    free_tree(node->yes);
    free_tree(node->no);
    free(node->text);
    free(node);
}

/* TODO 4  (recursion allowed) */
int count_nodes(Node *root) {
    if (root == NULL) return 0;
    return 1 + count_nodes(root->yes) + count_nodes(root->no);
    
}


/* ====== FrameStack  (dynamic array, iterative traversal) ======== */

/* TODO 5 */
void fs_init(FrameStack *s) {
    s->size = 0;
    s->capacity = 1;
    Frame* temp = malloc(sizeof(Frame) * (s->capacity));
    if (temp == NULL) {
        return;
    }
    s->frames = temp;
}

/* TODO 6 */
void fs_push(FrameStack *s, Node *node, int answeredYes) {
    if (s->capacity <= s->size){
        s->capacity = (s->capacity)*2;
        Frame* temp = realloc(s->frames, sizeof(Frame)*(s->capacity));
        if (s->frames == NULL) {
            return;
        }
        s->frames = temp;
    }

    s->frames[s->size].node = node;
    s->frames[s->size].answeredYes = answeredYes;
    s->size = s->size + 1;
}

/* TODO 7 */
Frame fs_pop(FrameStack *s) {
    //if (s->size == 0) {
    //    return NULL;
    //}
    Frame dummy = s->frames[(s->size) - 1];
    s->size = s->size - 1;
    return dummy;
}

/* TODO 8 */
int fs_empty(FrameStack *s) {
    return (s->size == 0);
}

/* TODO 9 */
void fs_free(FrameStack *s) {
    free(s->frames);
    s->size = 0;
    s->capacity = 0;
    s->frames = NULL;
}


/* ====== EditStack  (dynamic array, undo/redo) =================== */

/* TODO 10 */
void es_init(EditStack *s) {
    s->size = 0;
    s->capacity = 1;
    Edit* temp = malloc(sizeof(Edit) * (s->capacity));
    if (temp == NULL) {
        return;
    }
    s->edits = temp;
}

/* TODO 11 */
void es_push(EditStack *s, Edit e) {
    if (s->capacity <= s->size){
        s->capacity = (s->capacity)*2;
        Frame* temp = realloc(s->edits, sizeof(Edit)*(s->capacity));
        if (s->edits == NULL) {
            return;
        }
        s->edits = temp;
    }

    s->edits[s->size] = e;
    s->size = s->size + 1;
}

/* TODO 12 */
Edit es_pop(EditStack *s) {
    Edit dummy = s->edits[s->size - 1];
    s->size = s->size - 1;
    return dummy;
}

/* TODO 13 */
int es_empty(EditStack *s) {
    return (s->size == 0);
}

/* TODO 14 */
void es_clear(EditStack *s) {
    s->size = 0;
}

/* provided -- do not modify */
void es_free(EditStack *s) {
    free(s->edits);
    s->edits    = NULL;
    s->size     = 0;
    s->capacity = 0;
}

void free_edit_stack(EditStack *s) { es_free(s); }


/* ====== Queue  (linked list, BFS) ============================== */

/* TODO 15 */
void q_init(Queue *q) {
    q->size = 0;
    q->front = NULL;
    q->rear = NULL;
}

/* TODO 16 */
void q_enqueue(Queue *q, Node *node, int id) {
    QueueNode* temp = malloc(sizeof(QueueNode));
    if(temp == NULL) {
        return;
    }
    temp->treeNode = node;
    temp->id = id;
    temp->next = NULL;

    if(q->front == NULL) {
        q->front = temp;
        q->rear = temp;
    } else {
        q->rear->next = temp;
        q->rear = temp;
    }
    q->size = q->size + 1;
}

/* TODO 17 */
int q_dequeue(Queue *q, Node **node, int *id) {
    if (q->size == 0) {
        return 0;
    }
    QueueNode* dummy = q->front;
    *node = dummy->treeNode;
    *id = dummy->id;
    q->front = dummy->next;
    if (q->front == NULL) {
        q->rear = NULL;
    }

    free(dummy);
    q->size = q->size - 1;
    return 1;
}

/* TODO 18 */
int q_empty(Queue *q) {
    return (q->size == 0);
}

/* TODO 19 */
void q_free(Queue *q) {
    QueueNode* curr = q->front;
    while (curr != NULL) {
        QueueNode* temp = curr->next;
        free(curr);
        curr = temp;
    }
    q->front = NULL;
    q->rear = NULL;
    q->size = 0;

}


/* ====== Hash table  (separate chaining) ======================== */

/* TODO 20
 * Convert a string to a canonical key:
 *   letters  -> lowercase
 *   spaces   -> underscore
 *   anything else -> drop
 * Caller owns the returned string and must free() it.
 */
char *canonicalize(const char *s) {
    if (s == NULL) return strdup("");
    return NULL;
}

/* TODO 21  (djb2: hash = hash*33 + c, seed 5381) */
unsigned h_hash(const char *s) {
    return 0;
}

/* TODO 22 */
void h_init(Hash *h, int nbuckets) {
}

/* TODO 23 */
int h_put(Hash *h, const char *key, int solutionId) {
    return 0;
}

/* TODO 24 */
int h_contains(const Hash *h, const char *key, int solutionId) {
    return 0;
}

/* TODO 25 */
int *h_get_ids(const Hash *h, const char *key, int *outCount) {
    *outCount = 0;
    return NULL;
}

/* TODO 26 */
void h_free(Hash *h) {
}
