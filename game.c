#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "lab4.h"

extern Node      *g_root;
extern EditStack  g_undo;
extern EditStack  g_redo;
extern Hash       g_index;

/* ----------------------------------------------------------------
 * TODO 31  run_diagnosis
 *
 * Walk the decision tree iteratively (no recursion) using a
 * FrameStack.  At each question node ask the user yes/no and push
 * the appropriate child.  At each solution leaf display the fix and
 * ask whether it solved the problem.
 *
 * If the fix did not help, enter the learning phase:
 *   - Ask the user what would actually fix the problem.
 *   - Ask for a yes/no question that distinguishes their problem
 *     from the solution just shown.
 *   - Ask which answer applies to their problem.
 *   - Create a new question node and a new solution node, wire them
 *     correctly, graft them into the tree, record an Edit for
 *     undo/redo, and index the new question with canonicalize/h_put.
 *
 * Edge case: if parent is NULL the root itself must be replaced.
 * ---------------------------------------------------------------- */
void run_diagnosis(void) {
    clear();
    attron(COLOR_PAIR(5) | A_BOLD);
    mvprintw(0, 0, "%-80s", " Tech Support Diagnosis");
    attroff(COLOR_PAIR(5) | A_BOLD);

    mvprintw(2, 2, "I'll help diagnose your tech problem.");
    mvprintw(3, 2, "Answer each question with y or n.");
    mvprintw(4, 2, "Press any key to start...");
    refresh();
    getch();

    FrameStack stack; // we made a new framestack
    fs_init(&stack);

    /* TODO: implement */
    fs_push(&stack, g_root, -1); // put the root in the stack

    while (!fs_empty(&stack)) { // gonna keep going till you reach the end

        //get the current node
        Frame f = fs_pop(&stack);
        Node* curr = f.node;

        //now gotta check if its question or answer

        if (curr->isQuestion == 1) { //question node
            clear();
            attron(COLOR_PAIR(5) | A_BOLD);
            mvprintw(0, 0, "%-80s", " Tech Support Diagnosis");
            attroff(COLOR_PAIR(5) | A_BOLD);


            int ans = get_yes_no(6, 2, curr->text); //use the function given to us (does asking + getting answer)

            fs_push(&stack, curr, ans); // now push the question back on the stack (this is like the parent)
            
            if (ans == 0) { // no
                fs_push(&stack, curr->no, -1);
            } else { //y yes
                fs_push(&stack, curr->yes, -1); //-1 bc we havent move farther (dont know answer)
            }
        
        } else {
            clear();
            attron(COLOR_PAIR(5) | A_BOLD);
            mvprintw(0, 0, "%-80s", " Tech Support Diagnosis");
            attroff(COLOR_PAIR(5) | A_BOLD);

            attron(COLOR_PAIR(3) | A_BOLD);
            mvprintw(6, 2, "Suggested fix: %s", curr->text);
            attroff(COLOR_PAIR(3) | A_BOLD);

            //can just use yes no again
            int solved = get_yes_no(8, 2, "Did this solve your problem? (y/n): ");
            
            if (solved == 1) { //did solve
                attron(COLOR_PAIR(3));
                mvprintw(10, 2, "Great! Glad I could help.");
                attroff(COLOR_PAIR(3));
                refresh();
                napms(1500); // found online (small pause)
                break; //nothing else to be done (no learning needed)
            }

            //NOW LEARNING PHASE IS NECESSARY (solution but didn't solve)

            // first make a new node

            Node* parent = NULL; // will stay null if it's the root (for the edit cases)
            int   parentAnswer = -1;
            if (!fs_empty(&stack)) { // if there is a parent (otherwise it was the root)
                Frame parentFrame = fs_pop(&stack); // need to make a node between parent and curr solution
                parent = parentFrame.node;
                parentAnswer = parentFrame.answeredYes;
            }

            char *tmp;
            char newSolution[800];
            char newQuestion[800];
            
            //get the solution
            tmp = get_input(10, 2, "What would fix this problem? ");
            strncpy(newSolution, tmp, sizeof(newSolution) - 1);
            newSolution[sizeof(newSolution) - 1] = '\0';

            //get question
            char prompt[800];
            snprintf(prompt, sizeof(prompt), "Give me a yes/no question that distinguishes your problem from \"%s\": ", curr->text); //online
            tmp = get_input(12, 2, prompt);
            strncpy(newQuestion, tmp, sizeof(newQuestion) - 1);
            newQuestion[sizeof(newQuestion) - 1] = '\0';

            //get connection (yes or no child)
            int userAns = get_yes_no(14, 2, "For your problem, is the answer yes or no? (y/n):");

            Node *newQ    = create_question_node(newQuestion);
            Node *newLeaf = create_solution_node(newSolution);
            Node *oldLeaf = curr;

            if (userAns == 1) { // it is the yes leaf
                newQ->yes = newLeaf;
                newQ->no  = oldLeaf;
            } else { // it is the no leaf
                newQ->yes = oldLeaf;
                newQ->no  = newLeaf;
            }

            
            if (parent == NULL) { //empty tree case
                g_root = newQ;
            } else if (parentAnswer == 1) { // add at parent yes leaf
                parent->yes = newQ;
            } else { // add at parent no leaf
                parent->no = newQ;
            }


            // for undo/redo
                        
            Edit e;
            e.type = EDIT_INSERT_SPLIT;
            e.parent = parent;
            e.wasYesChild = parentAnswer;
            e.oldLeaf = oldLeaf;
            e.newQuestion = newQ;
            e.newLeaf = newLeaf;
            es_push(&g_undo, e);
            while (!es_empty(&g_redo)) {
                Edit old = es_pop(&g_redo);
                //new Question and newLeaf are detached so they need to be freed
                free(old.newQuestion->text);
                free(old.newQuestion);
                free(old.newLeaf->text);
                free(old.newLeaf);
            }

            //hash table update

            char* canon = canonicalize(newQuestion);
            h_put(&g_index, canon, count_nodes(g_root));
            free(canon);

            attron(COLOR_PAIR(3));
            mvprintw(16, 2, "Thanks! I'll remember that.");
            attroff(COLOR_PAIR(3));
            refresh();
            napms(1500); // online
            break;

        }

    }
    fs_free(&stack);
}

/* ----------------------------------------------------------------
 * TODO 32  undo_last_edit
 * Return 1 on success, 0 if the undo stack is empty.
 * ---------------------------------------------------------------- */
int undo_last_edit(void) {
    if (es_empty(&g_undo)) {
        return 0;
    }

    Edit e = es_pop(&g_undo);

    if (e.parent == NULL) { // at the root
        g_root = e.oldLeaf; // update the root
    } else if (e.wasYesChild == 1) { // edited the right leaf
        e.parent->yes = e.oldLeaf;
    } else { // edited the left leaf
        e.parent->no = e.oldLeaf;
    }

    es_push(&g_redo, e); // add the undo to the redo stack (so we can redo the change)
    return 1;
}

/* ----------------------------------------------------------------
 * TODO 33  redo_last_edit
 * Return 1 on success, 0 if the redo stack is empty.
 * ---------------------------------------------------------------- */
int redo_last_edit(void) {
    if (es_empty(&g_redo)) {
        return 0;
    }
    
    Edit e = es_pop(&g_redo);

    if (e.parent == NULL) { 
        g_root = e.newQuestion; // put the new question at the root
    } else if (e.wasYesChild == 1) {
        e.parent->yes = e.newQuestion;
    } else {
        e.parent->no = e.newQuestion;
    }

    es_push(&g_undo, e);
    return 1;
}
