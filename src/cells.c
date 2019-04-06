#include "cells.h"
#include "clist.h"

// suspend logic steps if 0
int doLogic = 0;

// images used to draw inside cells
GdkPixbuf* cellImages[c_total_number_images];

cellStruct *cells[gridWidth * gridHeight];

void freeTargets(cellStruct* c)
{
    if (!clistIsEmpty(c->targets)) {
        cnode_t *n = c->targets->head;

        while (n != NULL) {
            free(n->data);
            n = n->next;
        }

        clistEmptyList(c->targets);
    }
}

void findTwoInputs(cellStruct* c, int* v1, int* v2)
{
    int f1 = 0;

    for (int d = 0; d < 4; d++) {
        if (f1 == 0) {
            if (c->inputs[d] == 1) {
                f1 = 1;
                *v1 = c->inputStates[d];
            }
        } else {
            if(c->inputs[d] == 1) {
                *v2 = c->inputStates[d];
            }
        }
    }
}

void stepLogic()
{

    int rev[] = {2, 3, 0, 1};

    // update wire targets
    // NB a 6 tile length wire has the same delay as a 1 or 0 tile wire
    for (int y = 0; y < gridHeight; y++) {
        for (int x = 0; x < gridWidth; x++) {
            cellStruct* c = cells[x + (y * gridWidth)];

            if (!clistIsEmpty(c->targets)) {
                // this cell has targets
                cnode_t *node = c->targets->head;

                while (node != NULL) {
                    cellTarget* t = (cellTarget*)node->data;
                    // update inputState[] from target cell and from direction
                    t->cell->inputStates[rev[t->dirFrom]] = c->activated;
                    node = node->next;
                }

            }
        }
    }

    /*
    .    c_And = 0, 2 inputs 1 output
    .    c_Or = 2,  2 inputs 1 output
    .    c_Xor = 3, 2 inputs 1 output
    .    c_Not = 6, 1 input 1 output
    .    c_Out = 7, 1 input
    */
    // update all the logic gates from their wire inputs
    for (int y = 0; y < gridHeight; y++) {
        for (int x = 0; x < gridWidth; x++) {
            cellStruct* c = cells[x + (y * gridWidth)];
            int oldVal = c->activated;
            int v1, v2;

            // do logic AND
            if (c->type == c_And) {
                findTwoInputs(c, &v1, &v2);
                c->activated = v1 & v2;
            }

            // do logic XOR
            if (c->type == c_Xor) {
                findTwoInputs(c, &v1, &v2);
                c->activated = v1 ^ v2;
            }

            // do logic OR
            if (c->type == c_Or) {
                findTwoInputs(c, &v1, &v2);
                c->activated = v1 | v2;
            }

            // do logic NOT
            if (c->type == c_Not) {
                findTwoInputs(c, &v1, &v2); // will only find 1
                c->activated = !v1;
            }

            // do logic OUT
            if (c->type == c_Out) {
                findTwoInputs(c, &v1, &v2); // will only find 1
                c->activated = v1;
            }

            // redraw any that have changed
            if (c->activated != oldVal) {
                gtk_widget_queue_draw(c->drawArea);
            }
        }
    }
}

void clearGrid()
{
    for (int i = 0; i < gridHeight; i++) {
        for (int j = 0; j < gridWidth; j++) {
            cellStruct* cs = cells[j + (i * gridWidth)];
            cs->type = c_Empty;
            cs->inputs[0] = cs->inputs[1] = cs->inputs[2] = cs->inputs[3] = 0;
            cs->outputs[0] = cs->outputs[1] = cs->outputs[2] = cs->outputs[3] = 0;
            cs->inputStates[0] = cs->inputStates[1] = cs->inputStates[2] = cs->inputStates[3] = 0;
            cs->activated = 0;
            freeTargets(cs);
            gtk_widget_queue_draw (cs->drawArea);
        }
    }
}

void cellAsXml(cellStruct* c, char* cellStr)
{
    const char* tf[] = {"false", "true"};
    const char* lt[] = {"And", "In", "Or", "Xor", "Cross", "Not", "Out", "Wire"};
    char p1[32];
    char p2[32];
    char p3[160];
    char p4[160];
    sprintf(p1, "<tile>\n\t<pos x=\"%i\" y=\"%i\" />\n", c->x, c->y);
    sprintf(p2, "\t<type logic=\"%s\" />\n", lt[c->type]);
    sprintf(p3, "\t<outputs north=\"%s\" east=\"%s\" south=\"%s\" west=\"%s\" />\n",
            tf[c->outputs[0]], tf[c->outputs[1]], tf[c->outputs[2]], tf[c->outputs[3]]);
    sprintf(p4, "\t<inputs north=\"%s\" east=\"%s\" south=\"%s\" west=\"%s\" />\n</tile>\n\n",
            tf[c->inputs[0]], tf[c->inputs[1]], tf[c->inputs[2]], tf[c->inputs[3]]);
    sprintf(cellStr, "%s%s%s%s", p1, p2, p3, p4);
}
