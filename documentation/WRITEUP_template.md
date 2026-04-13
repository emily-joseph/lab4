# ECE 312 Lab 4 Write-Up: Tech Support Diagnosis Tool

**Name:** Emily Joseph
**EID:** ej7887
**Date:** 4/12/2025

---

## What This Document Is

1–2 pages of honest reflection on the decisions you made and the problems you hit. Not a summary of the lab spec.

Full credit requires:
- Two specific design choices with a stated reason and a named alternative
- Four Big-O analyses with reasoning shown
- Two concrete bugs (symptom → cause → fix → rule)
- Reflection on the knowledge base you grew through sessions
- A note on `find_shortest_path`

Vague entries ("I had a leak and fixed it") earn no credit.

---

## Section 1 — Design Choices (two required, ~100 words each)

For each: what did you choose, what was the alternative, and why?

Candidate topics:
- Array-backed stack vs. linked-list stack
- Two-pass design in `load_tree` — why not link during the read phase?
- Dynamic `PathNode` array in `find_shortest_path` vs. fixed-size stack array
- Ownership model for nodes in undo/redo — why not free on undo?
- Iterative diagnosis loop — what state did you have to track explicitly?

### 1.A — [Two-pass design in `load_tree`]

*What I chose:* I chose to read all the nodes into a nodes array, with two other integer arrays holding the yesIds and noIds. Then in the second pass I linked the children by ID.

*What I considered instead:* I considered linking the children in the first pass itself.

*Why:* Since we saved the tree is BFS order, the children of a node won't appear before the parent. This means once we are loading a specific node, in order to link its right and left child we would have to continue to traverse the tree nodes. This meant that using a two-pass design solved that problem because after the first pass, all the nodes in the tree had been stored in the nodes array. In the second pass we could safely link the children of each node by using nodes[i]->yes = nodes[yesIds[i]] (and similarly for no) and will know if a yes or no child exists.

---

### 1.B — [Ownership model for nodes in undo/redo]

*What I chose:* I chose to not free the newQuestion and newLeaf on undo and instead putting them on the redo stack with the children still linked.

*What I considered instead:* Freeing the nodes on undo and building them again on redo.

*Why:* If I had freed the newQuestion and newLeaf on undo, redo wouldn't be able restore it since it doesn't know what the new question or solution was and would have to ask the user for the input again (defeating the whole purpose). By keeping the nodes alive on the redo stack, to perform a redo all I have to do is swap the pointers so that the parent is point to the new question. This does mean that the detatched nodes need to be specifically freed when the redo stack is cleared in a new learning session or when the program exits.

---

## Section 2 — Complexity Analysis (all four required)

Show the reasoning, not just the answer.

### 2.1 — Amortized cost of a single FrameStack push
When the size is less than capacity, all that has to happen is putting the new node at the right position, updating its answered yes, and incrementing the size which is O(1). When the size is not less than the capacity, then you need to resize the entire FrameStack. In this case the size is multiplied by 2 and realloc copies all existing elements which is O(n). As pushes occur capacity grows like 1 to 2 to 4 to 8... so if you do n pushes, resizing will happen about log base 2 (n) times. Spreading the O(n) copy cost across the n pushes gives O(n)/n = O(1) amortized per push.

### 2.2 — Hash table average-case lookup
The first thing that happens for a lookup is the hash is computed which iterates over every character in the key. If k is the key length then computing the hash is O(k). Then we find the bucket using bucket = hash % h->nbuckets which is just O(1). Next we iterate through all the entries in the current bucket. This really depends on how many keys end up in the bucket which depends, but if we account for the fact that the number of buckets is fixed at 31 and based on the lab requirements we have around 25 nodes, we can estimate that this averages O(1) which is found by dividing the number of keys by the number of buckets. This means the average-case lookup is around O(k) which is about O(1).

### 2.3 — Diagnosis traversal (best, worst, average)
The tree starts with a single question and two solution leaves. The diagnose traversal goes through every node from root to leaf (the solution). The best case is that the tree has only the initial question and the two solution leaves, so the path is always exactly two nodes. The worst case is that if a tree has a height h, the diagnosis traversal has to go through the longest path from root to leaf which is O(h). If there are n nodes, then the maximum possible height (h) would be n-1 where every new problem was appended to the same branch. Now the average case would be in the case of a balanced tree where each question splits the candidates roughly in half. This would be O(log n).


### 2.4 — `find_shortest_path` time and space
First off find_shortest_path needs to build the PathNode array by going through every node in the tree till it finds both solutions. If we assume that you must go through every node to find both solutions this is O(n). Then it builds the two ancestor list arrays that walk from root to end which is O(h) for each h (height). Then to find the LCA, we traverse from the root to the end which is also O(h). This makes the total time O(n). As for space, the first thing we built is the PathNodes array which I set allocated enough space to have a PathNode for every node in the tree. Then each of the ancestor arrays holds a node in each level from the root to the leaf. This gives us O(n) overall for space.
---

## Section 3 — Bugs (two required)

### 3.A — [Short name]

*Symptom:*

*Cause:* [quote the wrong line or describe the wrong logic]

*Fix:*

*Rule that would have prevented it:*

---

### 3.B — [Short name]

*Symptom:*

*Cause:*

*Fix:*

*Rule that would have prevented it:*

---

## Section 4 — Knowledge Base Reflection (~100 words)

1. How many nodes does your submitted `techsupport.dat` contain?

2. What categories of problems did you teach the program? Give one example question/solution pair for each category.

3. Look at the tree with `[V]`.  Are the questions you taught it good distinguishing questions — do they split the remaining candidates roughly in half?  Name one question you would improve and describe what you would replace it with.

4. Describe one `[F]ind Path` result.  What were the two solutions, what was the shared path, and did the output match your expectation?

---

## Section 5 — Reflection (3–5 sentences)

Answer at least two:

- What was the hardest part and why?
- What did the iterative diagnosis loop teach you about recursion?
- What would you do differently if starting over?
- Was there a moment something clicked? What was it?

---

## Section 6 — Time Log

| Date | Hours | What you worked on |
|------|-------|--------------------|
| | | |
| | | |
| | | |
| | | |
| | | |
| | | |
| | | |
| | | |
| | | |
| | | |

**Total hours:** ___
