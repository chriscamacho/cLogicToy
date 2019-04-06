#ifndef CELL_H
#define CELL_H

#include <gtk/gtk.h>
#include "clist.h"

// {"And.png", "In.png", "Or.png", "Xor.png", "on.png",
//        "Cross.png", "Not.png", "Out.png", "inwire.png", "outwire.png"};

#define gridWidth 32
#define gridHeight 32

extern int doLogic;

enum cellType {
    c_And = 0,
    c_In = 1,
    c_Or = 2,
    c_Xor = 3,
    c_On = 4, // not an actual type, done for pixmap index
    c_Cross = 5,
    c_Not = 6,
    c_Out = 7,
    c_inWire = 8, // not an actual type, done for pixmap index
    c_outWire = 9, // not an actual type, done for pixmap index
    c_Wire = 10, // just a connection
    c_BG = 11,
    c_Empty = 255
};


typedef struct {
    int x, y;
    enum cellType type;
    int inputs[4];
    int inputStates[4];
    int outputs[4];
    clist_t* targets;
    int activated;
    GtkWidget *drawArea;
} cellStruct;


typedef struct {
    cellStruct* cell;
    int dirFrom;
} cellTarget;


extern GdkPixbuf* cellImages[10];
extern cellStruct *cells[gridWidth * gridHeight];

void freeTargets(cellStruct* c);
void clearGrid();
void stepLogic();
void cellAsXml(cellStruct* c, char* cellStr);

#endif // CELL_H
