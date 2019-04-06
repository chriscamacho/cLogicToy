#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <gtk/gtk.h>

#define UNUSED(x) (void)(x)


gboolean timeOut(gpointer data);

gboolean mainClosed(GtkWidget *widget, gpointer data);

gboolean
draw_callback (GtkWidget *widget, cairo_t *cr, gpointer data);

gboolean
button_press_callback (GtkWidget *widget, GdkEvent  *event, gpointer data);

gboolean edCancelButton_clicked_cb(GtkWidget* widget, gpointer data);

void setupEditDialog(GtkBuilder* builder);

#endif /* CALLBACKS_H */
