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
    FILE *fp;
    uint32_t magic;
    uint32_t version;
    uint32_t nodeCount;

    if (filename == NULL) {
        return 0;
    }

    fp = fopen(filename, "rb");
    if (fp == NULL) {
        return 0;
    }

    //get magic numebr, version, and nodeCount
    if (fread(&magic, sizeof(uint32_t), 1, fp) != 1) {
        goto fail;
    }
    if (fread(&version, sizeof(uint32_t), 1, fp) != 1) {
        goto fail;
    }
    if (fread(&nodeCount, sizeof(uint32_t), 1, fp) != 1) {
        goto fail;
    }

    if (nodeCount == 0) { //if for some reason node count doesnt match g_root
        if (g_root != NULL) {
            free_tree(g_root);
        }
        g_root = NULL;
        fclose(fp);
        return 1;
    }

    Node **nodes = calloc(nodeCount, sizeof(Node *));
    int32_t *yesIds = malloc(nodeCount * sizeof(int32_t)); //this will hold addr into nodes of the yes
    int32_t *noIds  = malloc(nodeCount * sizeof(int32_t)); //same for no (so we can use this like nodes[noIds[blah]])
    if (nodes == NULL || yesIds == NULL || noIds == NULL) {
        goto fail;
    }

    for (uint32_t i = 0; i < nodeCount; i++) {
        uint8_t isQuestion;
        uint32_t textLen;
        char *text = NULL;
        Node *node = NULL;
        if (fread(&isQuestion, sizeof(uint8_t), 1, fp) != 1) {
            goto fail;
        }
        if (fread(&textLen, sizeof(uint32_t), 1, fp) != 1) {
            goto fail;
        }
        text = malloc((textLen+1) * sizeof(char)); // add space for null char
        if (text == NULL){
            goto fail;
        }
        
        if (textLen > 0) {
            if (fread(text, sizeof(char), textLen, fp) != textLen) {
                free(text);
                goto fail;
            }
        }
        text[textLen] = 0;


        if (fread(&yesIds[i], sizeof(int32_t), 1, fp) != 1) {
            free(text);
            goto fail;
        }
        if (fread(&noIds[i], sizeof(int32_t), 1, fp) != 1) {
            free(text);
            goto fail;
        }
        if (isQuestion == 1) {
            node = create_question_node(text); // only yes and no needs to be set up now
            if (node == NULL) {
                free(text);
                goto fail;
            }
        } else { // assume it's always 0 or 1
            node = create_solution_node(text);
            if (node == NULL) {
                free(text);
                goto fail;
            }
        }

        free(text); // not needed anymore

        //now we can put this new made node into our array
        nodes[i] = node;

    }

    //now let's acc rebuild the array
    for (uint32_t i = 0; i < nodeCount; i++) {
        if (yesIds[i] != -1) {
            nodes[i]->yes = nodes[yesIds[i]];
        } else {
            nodes[i]->yes = NULL;
        }
        if (noIds[i] != -1) {
            nodes[i]->no = nodes[noIds[i]];
        } else {
            nodes[i]->no = NULL;
        }
    }

    //takeout whatever was there before
    if (g_root != NULL) {
        free_tree(g_root);
    }

    g_root = nodes[0]; //root will always be the first node in array

    free(nodes);
    free(yesIds);
    free(noIds);
    fclose(fp);
    return 1;

    fail:
    if (nodes != NULL) {
        for (uint32_t i = 0; i < nodeCount; i++) {
            if (nodes[i] != NULL) {
                free(nodes[i]->text);
                free(nodes[i]);
            }
        }
    }
    free(nodes);
    free(yesIds);
    free(noIds);
    fclose(fp);
    return 0;
}
