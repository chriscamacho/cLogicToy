#include "wires.h"
#include "cells.h"

clist_t* traversal = NULL;

cellTarget* createTarget(cellStruct* c, int dir)
{
    cellTarget* ct = malloc(sizeof(cellTarget));
    ct->cell = c;
    ct->dirFrom = dir;
    return ct;
}

// is the cell a logic cell that can take and input
// from a particular direction
int isSink(cellStruct* c, int dir)
{
    if (c->type == c_And ||
            c->type == c_Out ||
            c->type == c_Or ||
            c->type == c_Xor ||
            c->type == c_Not) {

        if (c->inputs[0] == 1 && dir == 2) { // north edge input incomming south
            return 1;
        }
        if (c->inputs[1] == 1 && dir == 3) { // east input incomming west
            return 1;
        }
        if (c->inputs[2] == 1 && dir == 0) { // south input, dir north
            return 1;
        }
        if (c->inputs[3] == 1 && dir == 1) { // west input, dir east
            return 1;
        }
    }

    return 0;

}

// run down a wire to find their destinations
//
// while not overly keen on recursion, in this case its a good fit
//
// NB wires are always 1 way!
//
void findTargets(cellStruct* root, cellStruct* cell)
{
    //printf("find target[%i,%i]\n",cell->x,cell->y);
    if (clistFindNode(traversal, cell) != NULL) {
        return;
    }
    // traversal guards against wire loops
    // which are creatable but invalid anyhow...
    clistAddNode(traversal, cell);

    int offset[] = {-gridWidth, 1, gridWidth, -1};

    for (int d = 0; d < 4; d++) {
        // guard against falling off the edges tho editor
        // shouldn't allow this...
        if (cell->y == 0 && d == 0) {
            continue;
        }
        if (cell->x == gridWidth - 1 && d == 1) {
            continue;
        }
        if (cell->y == gridHeight - 1 && d == 2) {
            continue;
        }
        if (cell->x == 0 && d == 3) {
            continue;
        }

        // is there an output going in direction d
        if(cell->outputs[d] != 0) {
            // find the cell next to us in direction d
            cellStruct* c = cells[offset[d] + cell->x + (cell->y * gridWidth)];

            // if its a bridge just keep going over/under them till there
            // are no more
            while(c->type == c_Cross && cell->outputs[d] != 0) {
                c = cells[offset[d] + c->x + (c->y * gridWidth)];
            }
            // if the new cell is a wire search it for possible targets
            if (c->type == c_Wire) {
                findTargets(root, c);
            }
            // if its a target add it to the roots list of targets
            if (isSink(c, d)) {
                //printf("target[%i,%i]\n", c->x, c->y);
                clistAddNode(root->targets, createTarget(c, d));
            }
        }
    }


}

// whenever the grid is changed find all the wire sources and targets
void findWires()
{
    if (traversal == NULL) {
        traversal = clistCreateList();
    } else {
        clistEmptyList(traversal);
    }
    for(int y = 0; y < gridHeight; y++) {
        for (int x = 0; x < gridWidth; x++) {
            cellStruct* c = cells[x + (y * gridWidth)];
            // if its a logic cell or input then
            // if can generate and output
            if (c->type == c_And ||
                    c->type == c_In ||
                    c->type == c_Or ||
                    c->type == c_Xor ||
                    c->type == c_Not) {
                //printf("------------------\n");
                //printf("wire source[%i,%i]\n", c->x, c->y);
                c->inputStates[0] = c->inputStates[1] = c->inputStates[2] = c->inputStates[3] = 0;
                freeTargets(c);
                findTargets(c, c);
            }
        }
    }
}
