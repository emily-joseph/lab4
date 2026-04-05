#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "lab4.h"

extern Node *g_root;

#define MAGIC   0x54454348u   /* "TECH" */
#define VERSION 1u

typedef struct { Node *node; int id; } NodeMapping;

/* ----------------------------------------------------------------
 * TODO 27  save_tree
 *
 * Serialize the entire tree to a binary file using BFS order.
 *
 * File format:
 *   Header:  uint32 magic | uint32 version | uint32 nodeCount
 *   Per node (BFS order):
 *     uint8  isQuestion
 *     uint32 textLen          (bytes, no null terminator in file)
 *     char[] text             (exactly textLen bytes)
 *     int32  yesId            (-1 if NULL)
 *     int32  noId             (-1 if NULL)
 *
 * Return 1 on success, 0 on failure.
 * ---------------------------------------------------------------- */
int save_tree(const char *filename) {
    FILE *fp;
    uint32_t magic = MAGIC;
    uint32_t version = VERSION;
    uint32_t numNodes;

    if (filename == NULL) {
        return 0;
    }

    fp = fopen(filename, "wb"); //format from internet
    if (fp == NULL){
        fclose(fp);
        return 0;
    }

    numNodes = (uint32_t)count_nodes(g_root);

    //write magic number
    if (fwrite(&magic, sizeof(uint32_t), 1, fp) != 1) { //format from internet
        fclose(fp);
        return 0;
    }
    //write version
    if (fwrite(&version, sizeof(uint32_t), 1, fp) != 1) {
        fclose(fp);
        return 0;
    }
    //write numNodes
    if (fwrite(&numNodes, sizeof(uint32_t), 1, fp) != 1) {
        fclose(fp);
        return 0;
    }

    if (numNodes == 0) { //if there are no nodes (empty tree)
        fclose(fp);
        return 1;
    }

    int nextId = 0;

    NodeMapping *map = malloc(numNodes * sizeof(NodeMapping));
    if (map == NULL) {
        fclose(fp);
        return 0;
    }

    Queue *q = malloc(sizeof(Queue)); //make queue for BFS
    if (q == NULL) {
        fclose(fp);
        return 0;
    }
    q_init(q);
    q_enqueue(q, g_root, 0); //put root node


    //making the nodemap
    while(!q_empty(q)){
        Node* curr;
        int id;

        if(!q_dequeue(q, &curr, &id)) { //if the dequeue failed for some reason
            q_free(q);
            free(map); 
            fclose(fp);
            return 0;
        }

        //add this to the map
        map[id].node = curr; 
        map[id].id = id;

        //add both its children to the queue
        if (curr->yes != NULL) {
            nextId++;
            q_enqueue(q, curr->yes, nextId);
        }
        if (curr->no != NULL) {
            nextId++;
            q_enqueue(q, curr->no, nextId);
        }

    }

    for (uint32_t i = 0; i < numNodes; i++) {
        Node* curr = map[i].node;
        uint8_t isQuestion = curr->isQuestion;
        uint32_t textLen = (uint32_t)strlen(curr->text);
        char* text = curr->text;
        int32_t yesId = -1;
        int32_t noId = -1;

        if (curr->yes != NULL) {
            for (uint32_t j = 0; j < numNodes; j++) {
                if ((curr->yes) == (map[j].node)) {
                    yesId = map[j].id;
                }
            }
        }

        if (curr->no != NULL) {
            for (uint32_t j = 0; j < numNodes; j++) {
                if ((curr->no) == (map[j].node)) {
                    noId = map[j].id;
                }
            }
        }

        if (fwrite(&isQuestion, sizeof(uint8_t), 1, fp) != 1) {
            fclose(fp);
            return 0;
        } 

        if (fwrite(&textLen, sizeof(uint32_t), 1, fp) != 1) {
            fclose(fp);
            return 0;
        } 
        
        if (fwrite(text, sizeof(char), textLen, fp) != textLen) {
            fclose(fp);
            return 0;
        } 

        if (fwrite(&yesId, sizeof(int32_t), 1, fp) != 1) {
            fclose(fp);
            return 0;
        } 

        if (fwrite(&noId, sizeof(int32_t), 1, fp) != 1) {
            fclose(fp);
            return 0;
        } 

    }
    q_free(q);
    fclose(fp);
    free(map);
    return 1;
}

/* ----------------------------------------------------------------
 * TODO 28  load_tree
 *
 * Read a file written by save_tree and reconstruct the tree.
 * Validate the magic number.  Read all nodes into a flat array
 * first, then link children in a second pass.
 * Free any existing g_root before installing the new one.
 * Return 1 on success, 0 on any error (free partial allocations).
 * ---------------------------------------------------------------- */
int load_tree(const char *filename) {
    return 0;
}
