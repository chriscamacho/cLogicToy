#ifndef __CLIST_H
#define __CLIST_H

/**
 * structure representing a node in a list
 */
typedef struct __cnode cnode_t;

struct __cnode {
    /*@{*/
    struct __cnode *prev;	/**< pointer to the previous node in the list */
    struct __cnode *next;	/**< pointer to the next node in the list */
    void *data;				/**< pointer to the data this node is tracking */
    /*@}*/
};


/**
 * structure representing a list of nodes (basically just a header)
 */
typedef struct __clist clist_t;

struct __clist {
    /*@{*/
    cnode_t *head;	/**< the first node in the list */
    cnode_t *tail; /**< the last node in the list */
    /*@}*/
};



clist_t *clistCreateList();
cnode_t *clistAddNode(clist_t *list, void *data);
cnode_t *clistInsertNode(clist_t *list, cnode_t *node, void *data);
cnode_t *clistFindNode(clist_t *list, void *ptr);
void clistDeleteNode(clist_t *list, cnode_t **node);
void clistDeleteNodeFromData(clist_t*, void*);
void clistEmptyList(clist_t *list);
void clistFreeList(clist_t **list);
void clistIterateForward(clist_t *list, void(nodeFunction)(cnode_t *node));
void clistIterateBackward(clist_t *list, void(nodeFunction)(cnode_t *node));
void clistSort(clist_t *list, int(cmpFunction)(cnode_t *node1, cnode_t *node2));
int clistTotal(clist_t *list);
int clistIsEmpty(clist_t *list);

#endif
