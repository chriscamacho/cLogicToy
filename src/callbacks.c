#include <gtk/gtk.h>

#include "callbacks.h"
#include "xmlLoader.h"
#include "cells.h"
#include "editDialog.h"

// callback for the main window
// see editDialog.c for the editor callbacks




gboolean timeOut(gpointer data)
{
    (void)data;

    if (doLogic) {
        stepLogic();
    }

    return TRUE;
}

gboolean on_fileSave_activate(GtkWidget *widget, gpointer data)
{
    (void)widget;
    (void)data;
    
    GtkWidget *dialog;
GtkFileChooser *chooser;
GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
gint res;

dialog = gtk_file_chooser_dialog_new ("Save File",
                                      (GtkWindow*)data,
                                      action,
                                      "_Cancel",
                                      GTK_RESPONSE_CANCEL,
                                      "_Save",
                                      GTK_RESPONSE_ACCEPT,
                                      NULL);
chooser = GTK_FILE_CHOOSER (dialog);

gtk_file_chooser_set_do_overwrite_confirmation (chooser, TRUE);


  gtk_file_chooser_set_current_name (chooser,
                                     "./examples/*");

res = gtk_dialog_run (GTK_DIALOG (dialog));
if (res == GTK_RESPONSE_ACCEPT)
  {
    char *filename;

    filename = gtk_file_chooser_get_filename (chooser);
    
       FILE * fp;

   fp = fopen (filename, "w");

   fprintf(fp,"<grid>\n\n");

    char str[1024];
    for(int y=0; y<gridHeight; y++) {
        for(int x=0; x<gridWidth; x++) {
            cellStruct* c = cells[x + (y * gridWidth)];
            if (c->type!=c_Empty) {
                cellAsXml(c, str);
                fprintf(fp,"%s",str);
            }
        }
    }
    fprintf(fp,"\n</grid>\n");
   fclose(fp);

    g_free (filename);
  }

gtk_widget_destroy (dialog);
    
    
    

    return FALSE;
}

gboolean fileNew_activate_cb(GtkWidget *widget, gpointer data)
{
    (void)widget;
    (void)data;
    clearGrid();
    return FALSE;
}

gboolean openFile(GtkWidget *widget, gpointer data)
{
    UNUSED(widget);
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;

    dialog = gtk_file_chooser_dialog_new ("Open File",
                                          (GtkWindow*)data,
                                          action,
                                          ("_Cancel"),
                                          GTK_RESPONSE_CANCEL,
                                          ("_Open"),
                                          GTK_RESPONSE_ACCEPT,
                                          NULL);
    gtk_file_chooser_set_filename ((GtkFileChooser *)dialog, "./examples/*");
    res = gtk_dialog_run (GTK_DIALOG (dialog));

    if (res == GTK_RESPONSE_ACCEPT) {
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
        filename = gtk_file_chooser_get_filename (chooser);
        loadGrid(filename);
        g_free (filename);
    }

    gtk_widget_destroy (dialog);

    // TODO do I actually need to do this?
    for (int i = 0; i < gridHeight; i++) {
        for (int j = 0; j < gridWidth; j++) {
            gtk_widget_queue_draw(cells[j + (i * gridWidth)]->drawArea);
        }
    }

    return FALSE;
}

gboolean mainClosed(GtkWidget *widget, gpointer data)
{
    UNUSED(widget);
    UNUSED(data);

    gtk_main_quit();
    return FALSE; // or code after gtk_main doesn't execute....
}



gboolean
button_press_callback (GtkWidget *widget, GdkEvent *event, gpointer data)
{
    (void)widget;
    guint button;

    cellStruct *cell = (cellStruct*)data;

    gdk_event_get_button (event, &button);

    if (button == GDK_BUTTON_PRIMARY) {
        editCell(cell);
    }

    if (button == GDK_BUTTON_SECONDARY) {
        if (cell->type == c_In) {
            if (cell->activated == 1) {
                cell->activated = 0;
            } else {
                cell->activated = 1;
            }

            gtk_widget_queue_draw (widget);
        }
    }

    return FALSE;
}

/*
 * the callback draws each cell in turn depending on the various
 * properties...
 */
gboolean
draw_callback (GtkWidget *widget, cairo_t *cr, gpointer data)
{
    cellStruct *cell = (cellStruct*)data;

    guint width, height;
    //GtkStyleContext *context;

    //context = gtk_widget_get_style_context (widget);

    width = gtk_widget_get_allocated_width (widget);
    height = gtk_widget_get_allocated_height (widget);

    //gtk_render_background (context, cr, 0, 0, width, height);

    cairo_translate(cr, width / 2.0, height / 2.0);
    cairo_matrix_t matrix;
    cairo_get_matrix (cr, &matrix);

    gdk_cairo_set_source_pixbuf (cr, cellImages[c_BG], -32, -32);
    cairo_paint(cr);

    char str[8];

    sprintf(&str[0], "%i,%i", cell->x, cell->y);
    cairo_move_to(cr, -26, 27);
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_font_size(cr, 8);
    cairo_show_text(cr, str);
    cairo_set_source_rgb(cr, 1, 1, 1);

    for (int d = 0; d < 4; d++) {
        if (cell->outputs[d]) {
            cairo_set_matrix(cr, &matrix);
            cairo_rotate (cr, 1.570796 * (d - 1));
            gdk_cairo_set_source_pixbuf (cr, cellImages[c_outWire], -32, -32);
            cairo_paint(cr);
        }
    }

    for (int d = 0; d < 4; d++) {
        if (cell->inputs[d]) {
            cairo_set_matrix(cr, &matrix);
            cairo_rotate (cr, 1.570796 * (d - 1));
            gdk_cairo_set_source_pixbuf (cr, cellImages[c_inWire], -32, -32);
            cairo_paint(cr);
        }
    }

    cairo_set_matrix(cr, &matrix);

    if (cell->type != c_Empty && cell->type != c_Wire) {
        gdk_cairo_set_source_pixbuf (cr, cellImages[cell->type], -32, -32);
        cairo_paint(cr);
    }

    // TODO offer these two as an option in settings ? (possibly logic active different colour)
    if (cell->activated == 1 && (cell->type == c_In || cell->type == c_Out)) {
        //if (cell->activated == 1) {
        gdk_cairo_set_source_pixbuf (cr, cellImages[c_On], -32, -32);
        cairo_paint(cr);
    }

    return FALSE;
}
