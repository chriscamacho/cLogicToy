#include "editDialog.h"
#include "callbacks.h"
#include "wires.h"


GtkWidget* editDialog;
GtkWidget* edTypeDD;
GtkWidget* edOkButton;
GtkWidget* northIn;
GtkWidget* eastIn;
GtkWidget* southIn;
GtkWidget* westIn;
GtkWidget* northOut;
GtkWidget* eastOut;
GtkWidget* southOut;
GtkWidget* westOut;
GtkWidget* edDrawArea;
GtkWidget* errors;

cellStruct* edCell;
cellStruct* dest;
int noIOcb = 0;

//callbacks

void edTypeDD_changed_cb(GtkWidget* widget, gpointer data)
{
    (void)data;
    const gchar* sel = gtk_combo_box_get_active_id ((GtkComboBox*)widget);
    //printf("sel %s\n", sel);
    int i = atoi(sel);
    edCell->type = i;
    gtk_widget_queue_draw (edDrawArea);
    validateCell();
}

gboolean inOutChanged(GtkWidget* widget, gpointer data)
{
    (void)data;

    if (noIOcb == 0) {
        // TODO is there a more elegant way to do this ?
        if (widget == northIn && gtk_toggle_button_get_active ((GtkToggleButton*)northIn)) {
            gtk_toggle_button_set_active ((GtkToggleButton*)northOut, FALSE);
        }
        if (widget == eastIn && gtk_toggle_button_get_active ((GtkToggleButton*)eastIn)) {
            gtk_toggle_button_set_active ((GtkToggleButton*)eastOut, FALSE);
        }
        if (widget == southIn && gtk_toggle_button_get_active ((GtkToggleButton*)southIn)) {
            gtk_toggle_button_set_active ((GtkToggleButton*)southOut, FALSE);
        }
        if (widget == westIn && gtk_toggle_button_get_active ((GtkToggleButton*)westIn)) {
            gtk_toggle_button_set_active ((GtkToggleButton*)westOut, FALSE);
        }

        if (widget == northOut && gtk_toggle_button_get_active ((GtkToggleButton*)northOut)) {
            gtk_toggle_button_set_active ((GtkToggleButton*)northIn, FALSE);
        }
        if (widget == eastOut && gtk_toggle_button_get_active ((GtkToggleButton*)eastOut)) {
            gtk_toggle_button_set_active ((GtkToggleButton*)eastIn, FALSE);
        }
        if (widget == southOut && gtk_toggle_button_get_active ((GtkToggleButton*)southOut)) {
            gtk_toggle_button_set_active ((GtkToggleButton*)southIn, FALSE);
        }
        if (widget == westOut && gtk_toggle_button_get_active ((GtkToggleButton*)westOut)) {
            gtk_toggle_button_set_active ((GtkToggleButton*)westIn, FALSE);
        }

        edCell->inputs[0] = gtk_toggle_button_get_active ((GtkToggleButton*)northIn);
        edCell->inputs[1] = gtk_toggle_button_get_active ((GtkToggleButton*)eastIn);
        edCell->inputs[2] = gtk_toggle_button_get_active ((GtkToggleButton*)southIn);
        edCell->inputs[3] = gtk_toggle_button_get_active ((GtkToggleButton*)westIn);

        edCell->outputs[0] = gtk_toggle_button_get_active ((GtkToggleButton*)northOut);
        edCell->outputs[1] = gtk_toggle_button_get_active ((GtkToggleButton*)eastOut);
        edCell->outputs[2] = gtk_toggle_button_get_active ((GtkToggleButton*)southOut);
        edCell->outputs[3] = gtk_toggle_button_get_active ((GtkToggleButton*)westOut);

        int t = 0;
        char id[8];

        for(int d = 0; d < 4; d++) {
            t += edCell->outputs[d];
            t += edCell->inputs[d];
        }

        if (edCell->type == c_Empty && t > 0) {
            edCell->type = c_Wire;
            sprintf(id, "%i", edCell->type);
            gtk_combo_box_set_active_id((GtkComboBox*)edTypeDD, id);
        }

        if (edCell->type != c_Empty && t == 0) {
            edCell->type = c_Empty;
            sprintf(id, "%i", edCell->type);
            gtk_combo_box_set_active_id((GtkComboBox*)edTypeDD, id);
        }

        gtk_widget_queue_draw (edDrawArea);
        validateCell();
    }
    return FALSE;
}

gboolean edOkButtonClicked(GtkWidget* widget, gpointer data)
{
    (void)data;
    (void)widget;
    for(int d = 0; d < 4; d++) {
        dest->inputs[d] = edCell->inputs[d];
        dest->outputs[d] = edCell->outputs[d];
    }
    dest->type = edCell->type;
    findWires();
    doLogic = 1;
    gtk_widget_hide(editDialog);

    return FALSE;
}

gboolean edCancelButton_clicked_cb(GtkWidget* widget, gpointer data)
{
    (void)widget;
    (void)data;
    //printf("cancel clicked\n");
    gtk_widget_hide(editDialog);
    doLogic = 1;
    return FALSE;
}

void setupEditDialog(GtkBuilder* builder)
{
    editDialog = GTK_WIDGET(gtk_builder_get_object(builder, "editDialog"));
    edTypeDD = GTK_WIDGET(gtk_builder_get_object(builder, "edTypeDD"));
    edOkButton = GTK_WIDGET(gtk_builder_get_object(builder, "edOkButton"));
    errors = GTK_WIDGET(gtk_builder_get_object(builder, "errors"));
    northIn = GTK_WIDGET(gtk_builder_get_object(builder, "northIn"));
    eastIn = GTK_WIDGET(gtk_builder_get_object(builder, "eastIn"));
    southIn = GTK_WIDGET(gtk_builder_get_object(builder, "southIn"));
    westIn = GTK_WIDGET(gtk_builder_get_object(builder, "westIn"));
    northOut = GTK_WIDGET(gtk_builder_get_object(builder, "northOut"));
    eastOut = GTK_WIDGET(gtk_builder_get_object(builder, "eastOut"));
    westOut = GTK_WIDGET(gtk_builder_get_object(builder, "westOut"));
    southOut = GTK_WIDGET(gtk_builder_get_object(builder, "southOut"));
    edDrawArea = GTK_WIDGET(gtk_builder_get_object(builder, "edDrawArea"));
    edCell = malloc(sizeof(cellStruct));
    edCell->drawArea = edDrawArea;


    // tell which column of the list model to render in the
    // edit type dropdown
    GtkCellRenderer * cell = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start( GTK_CELL_LAYOUT( edTypeDD ), cell, TRUE );
    gtk_cell_layout_set_attributes( GTK_CELL_LAYOUT( edTypeDD ), cell, "text", 0, NULL );
    gtk_combo_box_set_id_column((GtkComboBox*)edTypeDD, 1); // dont ask...!

    g_signal_connect (G_OBJECT (edDrawArea), "draw",
                      G_CALLBACK (draw_callback), edCell);
}

// set up the edit dialog to reflect the state of the cell
// that being edited
void editCell(cellStruct* c)
{
    doLogic = 0;
    noIOcb = 1; // disable io callback so it doesn't change stuff as we set things up
    char id[8];

    dest = c;
    edCell->type = c->type;
    edCell->x = c->x;
    edCell->y = c->y;
    for(int d = 0; d < 4; d++) {
        edCell->inputs[d] = c->inputs[d];
        edCell->outputs[d] = c->outputs[d];
    }
    gtk_toggle_button_set_active ((GtkToggleButton*)northIn, edCell->inputs[0]);
    gtk_toggle_button_set_active ((GtkToggleButton*)eastIn, edCell->inputs[1]);
    gtk_toggle_button_set_active ((GtkToggleButton*)southIn, edCell->inputs[2]);
    gtk_toggle_button_set_active ((GtkToggleButton*)westIn, edCell->inputs[3]);

    gtk_toggle_button_set_active ((GtkToggleButton*)northOut, edCell->outputs[0]);
    gtk_toggle_button_set_active ((GtkToggleButton*)eastOut, edCell->outputs[1]);
    gtk_toggle_button_set_active ((GtkToggleButton*)southOut, edCell->outputs[2]);
    gtk_toggle_button_set_active ((GtkToggleButton*)westOut, edCell->outputs[3]);


    sprintf(id, "%i", edCell->type);
    gtk_combo_box_set_active_id((GtkComboBox*)edTypeDD, id);

    // validator makes this sensitive (or not!)
    //gtk_widget_set_sensitive (edOkButton, FALSE);

    gtk_widget_show(editDialog);
    noIOcb = 0;
}

void validateCell()
{
    char errStr[1024];

    int i = 0;
    int o = 0;
    for (int d = 0; d < 4; d++) {
        if (edCell->inputs[d] == 1) {
            i++;
        }
        if (edCell->outputs[d] == 1) {
            o++;
        }
    }
    int valid = 1;
    if (edCell->type == c_And ||
            edCell->type == c_Or ||
            edCell->type == c_Xor) {
        if (i != 2) {
            valid = 0;
            strcpy(errStr, "exactly 2 inputs are needed\0");
        }
        if (o != 1) {
            valid = 0;
            strcpy(errStr, "exactly 1 output is needed\0");
        }
    }

    if (edCell->type == c_In) {
        if (i != 0) {
            valid = 0;
            strcpy(errStr, "exactly 0 inputs are needed\0");
        }
        if (o != 1) {
            valid = 0;
            strcpy(errStr, "exactly 1 output is needed\0");
        }
    }

    if (edCell->type == c_Not) {
        if (i != 1) {
            valid = 0;
            strcpy(errStr, "exactly 1 input is needed\0");
        }
        if (o != 1) {
            valid = 0;
            strcpy(errStr, "exactly 1 output is needed\0");
        }
    }

    if (edCell->type == c_Out) {
        if (i != 1) {
            valid = 0;
            strcpy(errStr, "exactly 1 input is needed\0");
        }
        if (o != 0) {
            valid = 0;
            strcpy(errStr, "exactly 0 outputs are needed\0");
        }
    }

    if (edCell->type == c_Cross) {
        if (i != 2) {
            valid = 0;
            strcpy(errStr, "exactly 2 inputs are needed\0");
        }
        if (o != 2) {
            valid = 0;
            strcpy(errStr, "exactly 2 outputs are needed\0");
        }
        int vi = 0;
        int vo = 0;
        vi = edCell->inputs[0] + edCell->inputs[2];
        vo = edCell->outputs[0] + edCell->outputs[2];
        if (vi != 1 && vo != 1) {
            valid = 0;
            strcpy(errStr, "vertical wire needs exactly 1 input and output\0");
        }
        int hi = 0;
        int ho = 0;
        hi = edCell->inputs[3] + edCell->inputs[1];
        ho = edCell->outputs[1] + edCell->outputs[3];
        if (hi != 1 && ho != 1) {
            valid = 0;
            strcpy(errStr, "horizontal wire needs exactly 1 input and output\0");
        }
    }

    if (edCell->type == c_Wire) {
        if (i != 1) {
            valid = 0;
            strcpy(errStr, "exactly 1 input is needed\0");
        }
        if (o == 0) {
            valid = 0;
            strcpy(errStr, "more than 1 output is needed\0");
        }
    }

    if (valid) {
        gtk_widget_set_sensitive (edOkButton, TRUE);
        gtk_label_set_text((GtkLabel*)errors, "");
    } else {
        gtk_widget_set_sensitive (edOkButton, FALSE);
        gtk_label_set_text((GtkLabel*)errors, errStr);
    }

}

void freeEdCell()
{
    free(edCell);
}
