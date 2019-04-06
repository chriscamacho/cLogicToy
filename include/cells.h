#ifndef CELL_H
#define CELL_H

#include <gtk/gtk.h>
#include "clist.h"

#define gridWidth 32
#define gridHeight 32

extern int doLogic;

// NB if these change the list model in glade will need changing
// also check strings in cellAsXml()
// TODO look at manually creating the model from these enums...
enum cellType {
    c_And = 0,      // 2 inputs 1 output
    c_In,           // 1 output
    c_Or,           // 2 inputs 1 output
    c_Xor,          // 2 inputs 1 output
    c_Cross,        // 2 inputs 2 outputs
    c_Not,          // 1 input  1 output
    c_Out,          // 1 input
    c_Wire,         // 1 input  1-3 outputs

    c_BG,           // not an actual type, done for pixmap index
    c_inWire,       // not an actual type, done for pixmap index
    c_outWire,      // not an actual type, done for pixmap index
    c_On,           // not an actual type, done for pixmap index
    c_Lon,          // not an actual type, done for pixmap index
    c_total_number_images,
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


extern GdkPixbuf* cellImages[c_total_number_images];
extern cellStruct *cells[gridWidth * gridHeight];

void freeTargets(cellStruct* c);
void clearGrid();
void stepLogic();
void cellAsXml(cellStruct* c, char* cellStr);

#endif // CELL_H
