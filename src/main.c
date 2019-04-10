#include <gtk/gtk.h>
#include "callbacks.h"
#include "cells.h"
#include "clist.h"
#include "editDialog.h"


GdkPixbuf* loadCellImage(const char* fn)
{
    GError *gerror = NULL;
    //GdkPixbuf* pb = gdk_pixbuf_new_from_file (fn, &gerror);
    GdkPixbuf* pb = gdk_pixbuf_new_from_resource (fn, &gerror);

    if (!pb) {
        printf("error message: %s\n", gerror->message);
        g_error_free(gerror);
    }

    return pb;
}

int main(int argc, char *argv[])
{
    GtkBuilder      *builder;
    GtkWidget       *window;
    GtkWidget       *grid;

    gtk_init(&argc, &argv);

    builder = gtk_builder_new();
    #define PREFIX "/uk/co/bedroomcoders/cLogicToy/"
    //gtk_builder_add_from_file (builder, "mainUI.glade", NULL);
    gtk_builder_add_from_resource(builder, PREFIX"res/mainUI.glade", NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "mainWindow"));
    grid = GTK_WIDGET(gtk_builder_get_object(builder, "grid"));
    gtk_builder_connect_signals(builder, NULL);

    setupEditDialog(builder);

    g_object_unref(builder);

    cellImages[c_And] = loadCellImage(PREFIX"img/And.png");
    cellImages[c_In] = loadCellImage(PREFIX"img/In.png");
    cellImages[c_Or] = loadCellImage(PREFIX"img/Or.png");
    cellImages[c_Xor] = loadCellImage(PREFIX"img/Xor.png");
    cellImages[c_On] = loadCellImage(PREFIX"img/on.png");
    cellImages[c_Cross] = loadCellImage(PREFIX"img/Cross.png");
    cellImages[c_Not] = loadCellImage(PREFIX"img/Not.png");
    cellImages[c_Out] = loadCellImage(PREFIX"img/Out.png");
    cellImages[c_inWire] = loadCellImage(PREFIX"img/inwire.png");
    cellImages[c_outWire] = loadCellImage(PREFIX"img/outwire.png");
    cellImages[c_BG] = loadCellImage(PREFIX"img/bg.png");
    cellImages[c_Lon] = loadCellImage(PREFIX"img/lon.png");

    // for now the graphics icons are at a fixed 64x64 size
    gtk_widget_set_size_request (grid, 64 * gridWidth, 64 * gridHeight);

    // each cell in the grid has an event box to catch clicks
    // which contains a drawing area...
    for (int i = 0; i < gridHeight; i++) {
        for (int j = 0; j < gridWidth; j++) {

            cells[j + (i * gridWidth)] = malloc(sizeof(cellStruct));
            cells[j + (i * gridWidth)]->targets = clistCreateList();
            cells[j + (i * gridWidth)]->x = j;
            cells[j + (i * gridWidth)]->y = i;

            cells[j + (i * gridWidth)]->drawArea = gtk_drawing_area_new ();
            gtk_widget_set_size_request (cells[j + (i * gridWidth)]->drawArea, 32, 32);

            GtkWidget* event_box = gtk_event_box_new();
            gtk_container_add(GTK_CONTAINER (event_box),
                              cells[j + (i * gridWidth)]->drawArea);

            g_signal_connect(event_box, "button-press-event",
                             G_CALLBACK (button_press_callback),
                             cells[j + (i * gridWidth)]);

            gtk_grid_attach (GTK_GRID(grid), event_box, j, i, 1, 1);

            g_signal_connect (G_OBJECT (cells[j + (i * gridWidth)]->drawArea), "draw",
                              G_CALLBACK (draw_callback), cells[j + (i * gridWidth)]);
        }
    }




    clearGrid();

    doLogic = 0;
    g_timeout_add (250, timeOut, NULL);

    gtk_widget_show_all(window);

    gtk_main();

    printf("clean up\n");

    for (int i = 0; i < gridHeight; i++) {
        for (int j = 0; j < gridWidth; j++) {
            freeTargets(cells[j + (i * gridWidth)]);
            clistFreeList(&cells[j + (i * gridWidth)]->targets);
            free(cells[j + (i * gridWidth)]);
        }
    }
    freeEdCell();
    // TODO release loaded pixbuf's ?

    return 0;
}
