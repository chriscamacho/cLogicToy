#include <gtk/gtk.h>

#include "xmlLoader.h"
#include "cells.h"
#include "wires.h"

#include <stdio.h>
#include <strings.h>


#define BUFFSIZE        8192
char Buff[BUFFSIZE];

cellStruct newCell;

// as each xml tag is first encountered the callback is triggered
static void XMLCALL
start(void *data, const XML_Char *el, const XML_Char **attr)
{
    int i;
    (void)data;

    if (strcasecmp("tile", el) == 0) {
        memset(&newCell, 0, sizeof(cellStruct));
        newCell.type = c_Empty;
    }

    for (i = 0; attr[i]; i += 2) {
        if (strcasecmp("pos", el) == 0) {
            if (strcasecmp("x", attr[i]) == 0) {
                newCell.x = atoi(attr[i + 1]);
            }

            if (strcasecmp("y", attr[i]) == 0) {
                newCell.y = atoi(attr[i + 1]);
            }
        }

        if (strcasecmp("type", el) == 0) {
            if (strcasecmp("logic", attr[i]) == 0) {
                if (strcasecmp("and", attr[i + 1]) == 0) {
                    newCell.type = c_And;
                }

                if (strcasecmp("in", attr[i + 1]) == 0) {
                    newCell.type = c_In;
                }

                if (strcasecmp("or", attr[i + 1]) == 0) {
                    newCell.type = c_Or;
                }

                if (strcasecmp("xor", attr[i + 1]) == 0) {
                    newCell.type = c_Xor;
                }

                if (strcasecmp("cross", attr[i + 1]) == 0) {
                    newCell.type = c_Cross;
                }

                if (strcasecmp("not", attr[i + 1]) == 0) {
                    newCell.type = c_Not;
                }

                if (strcasecmp("out", attr[i + 1]) == 0) {
                    newCell.type = c_Out;
                }

                if (strcasecmp("wire", attr[i + 1]) == 0) {
                    newCell.type = c_Wire;
                }
            }

        }

        if (strcasecmp("inputs", el) == 0) {
            int o = 0;

            if (strcasecmp("north", attr[i]) == 0) {
                o = 0;
            }

            if (strcasecmp("east", attr[i]) == 0) {
                o = 1;
            }

            if (strcasecmp("south", attr[i]) == 0) {
                o = 2;
            }

            if (strcasecmp("west", attr[i]) == 0) {
                o = 3;
            }

            if (strcasecmp("true", attr[i + 1]) == 0) {
                newCell.inputs[o] = 1;
            } else {
                newCell.inputs[o] = 0;
            }
        }

        if (strcasecmp("outputs", el) == 0) {
            int o = 0;

            if (strcasecmp("north", attr[i]) == 0) {
                o = 0;
            }

            if (strcasecmp("east", attr[i]) == 0) {
                o = 1;
            }

            if (strcasecmp("south", attr[i]) == 0) {
                o = 2;
            }

            if (strcasecmp("west", attr[i]) == 0) {
                o = 3;
            }

            if (strcasecmp("true", attr[i + 1]) == 0) {
                newCell.outputs[o] = 1;
            } else {
                newCell.outputs[o] = 0;
            }

        }
    }

}

// when the end of the xml tag is encountered this is called
// the temporary cell is transfered to the grid, but the existing
// draw area and target lists are preserved
static void XMLCALL
end(void *data, const XML_Char *el)
{
    (void)data;
    (void)el;
    cellStruct* cs = cells[newCell.x + (newCell.y * gridWidth)];
    GtkWidget* da = cs->drawArea;
    clist_t* l;
    l = cs->targets;
    memcpy(cs, &newCell, sizeof(cellStruct));
    cs->drawArea = da;
    cs->targets = l;
}

void loadGrid(const char* fileName)
{
    clearGrid();

    XML_Parser p = XML_ParserCreate(NULL);

    if (! p) {
        fprintf(stderr, "Couldn't allocate memory for parser\n");
        exit(-1);
    }

    FILE *fp;
    fp = fopen(fileName, "r");

    XML_SetElementHandler(p, start, end);

    for (;;) {
        int done;
        int len;

        len = (int)fread(Buff, 1, BUFFSIZE, fp);

        if (ferror(stdin)) {
            fprintf(stderr, "Read error\n");
            exit(-1);
        }

        done = feof(fp);

        if (XML_Parse(p, Buff, len, done) == XML_STATUS_ERROR) {
            fprintf(stderr,
                    "Parse error at line %lu:\n%s\n",
                    XML_GetCurrentLineNumber(p),
                    XML_ErrorString(XML_GetErrorCode(p)));
            exit(-1);
        }

        if (done) {
            break;
        }
    }

    XML_ParserFree(p);
    findWires();

    doLogic = 1;
}
