#ifndef EDITDIALOG_H
#define EDITDIALOG_H

#include "cells.h"

void setupEditDialog(GtkBuilder* builder);
void cancelEdit();
void editCell(cellStruct* cell);
void freeEdCell();
void validateCell();

#endif // EDITDIALOG_H
