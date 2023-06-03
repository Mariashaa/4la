#include "lab03_sqlite.h"

#include <gtk/gtk.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

#define UI_FILE "lab03.glade"

typedef enum {
    STR2INT_SUCCESS,
    STR2INT_OVERFLOW,
    STR2INT_UNDERFLOW,
    STR2INT_INCONVERTIBLE
} str2int_errno;

str2int_errno str2int(int *out, const char *s, int base) {
    char *end;
    if (s[0] == '\0' || isspace(s[0]))
        return STR2INT_INCONVERTIBLE;
    errno = 0;
    long l = strtol(s, &end, base);
    if (l > INT_MAX || (errno == ERANGE && l == LONG_MAX))
        return STR2INT_OVERFLOW;
    if (l < INT_MIN || (errno == ERANGE && l == LONG_MIN))
        return STR2INT_UNDERFLOW;
    if (*end != '\0')
        return STR2INT_INCONVERTIBLE;
    *out = l;
    return STR2INT_SUCCESS;
}

enum
{
    KATHID = 0,
    KATHNAME,
    DISCID,
    DISCNAME,
    DISCLEC,
    DISCPRACT,
    DISCLAB,
    KONTRID,
    KONTRNAME
};

struct MainWindowObjects
{
    GtkWindow *main_window;
    GtkTreeView *treeview;
    GtkListStore *liststore;
    GtkAdjustment *adjustment;
    GtkTreeViewColumn *column_kath_id;
    GtkTreeViewColumn *column_kath_name;
    GtkTreeViewColumn *column_disc_id;
    GtkTreeViewColumn *column_disc_name;
    GtkTreeViewColumn *column_disc_lec;
    GtkTreeViewColumn *column_disc_pract;
    GtkTreeViewColumn *column_disc_lab;
    GtkTreeViewColumn *column_kontr_id;
    GtkTreeViewColumn *column_kontr_name;
    GtkComboBox *combobox;
    GtkToggleButton *checkbox;
    GtkListStore *kathedra_list_store;
} mainWindowObjects;

struct kathedra_dialog_objects
{
    GtkDialog *kathedra_dialog;
    GtkEntry *entry_name_kath;
    GtkButton *save_kathedra_btn;
    GtkButton *cancel_kathedra_btn;
} kathedra_dialog_objects;

struct kontrol_dialog_objects
{
    GtkDialog *kontrol_dialog;
    GtkEntry *entry_name_kontr;
    GtkButton *kontrol_save_btn;
    GtkButton *kontrol_cancel_btn;
} kontrol_dialog_objects;

struct discipline_dialog_objects
{
    GtkDialog *discipline_dialog;
    GtkLabel *lbl_name_kath;
    GtkLabel *lbl_name_disc;
    GtkLabel *lbl_lekc_disc;
    GtkLabel *lbl_pract_disc;
    GtkLabel *lbl_labs_disc;
    GtkLabel *lbl_name_kontr;
    GtkComboBoxText *kathedra_box;
    GtkComboBoxText *kontrol_box;
    GtkEntry *entry_name_disc;
    GtkEntry *entry_lec_disc;
    GtkEntry *entry_pract_disc;
    GtkEntry *entry_lab_disc;
    GtkButton *btninsert;
    GtkButton *discipline_cancel_btn;
} discipline_dialog_objects;

struct yes_dialog_objects
{
    GtkDialog *yes_dialog;
    GtkButton *yes_btn;
    GtkLabel *yes_label;
}yes_dialog_objects;

int callback(void *not_used, int argc, char **argv, char **col_names)
{
    GtkTreeIter iter;
    if (argc == 9)
    {
        gtk_list_store_append(GTK_LIST_STORE(mainWindowObjects.liststore), &iter);
        gtk_list_store_set(GTK_LIST_STORE(mainWindowObjects.liststore), &iter, KATHID,
                           atoi(argv[KATHID]), KATHNAME, argv[KATHNAME], 
                           DISCID, atoi(argv[DISCID]), DISCNAME, argv[DISCNAME],
                           DISCLEC, argv[DISCLEC], DISCPRACT, argv[DISCPRACT], DISCLAB, argv[DISCLAB],
                           KONTRID, atoi(argv[KONTRID]), KONTRNAME, argv[KONTRNAME], -1);
    }
    return 0;
}

int callback2(void *not_used, int argc, char **argv, char **col_names)
{
    GtkTreeIter iter;
    if (argc == 2)
    {
        gtk_list_store_append(GTK_LIST_STORE(mainWindowObjects.liststore), &iter);
        gtk_list_store_set(GTK_LIST_STORE(mainWindowObjects.liststore), &iter, KATHID,
                           atoi(argv[KATHID]), KATHNAME, argv[KATHNAME], 
                           DISCID, 1, DISCNAME, " ",
                           DISCLEC, " ", DISCPRACT, " ", DISCLAB, " ",
                           KONTRID, 1, KONTRNAME, " ", -1);
    }
    return 0;
}

int callback4(void *not_used, int argc, char **argv, char **col_names)
{
    GtkTreeIter iter;
    if (argc == 2)
    {
        gtk_list_store_append(GTK_LIST_STORE(mainWindowObjects.liststore), &iter);
        gtk_list_store_set(GTK_LIST_STORE(mainWindowObjects.liststore), &iter, KATHID,
                           1, KATHNAME, " ", 
                           DISCID, 1, DISCNAME, " ",
                           DISCLEC, " ", DISCPRACT, " ", DISCLAB, " ",
                           KONTRID, atoi(argv[0]), KONTRNAME, argv[1], -1);
    }
    return 0;
}

int get_kathedra_callback(void *data, int argc, char **argv, char **col_names)
{
    GtkTreeIter iter;
    if (argc == 2)
    {
        for (int i = 0; i < argc; i++)
        {
            if (i % 2 == 1)
            {
                gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(discipline_dialog_objects.kathedra_box), argv[i]);
                
                printf("%s\n", argv[i]);
            }
        }
        return 0;
    }

    return 1;
}

int get_kontrol_callback(void *data, int argc, char **argv, char **col_names)
{

    if (argc > 0)
    {
        for (int i = 0; i < argc; i++)
        {
            if (i%2 == 1)
            {
                gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(discipline_dialog_objects.kontrol_box), argv[i]);
                printf("%s\n", argv[i]);
            }
        }
        return 0;
    }

    return 1;
}

void kath_id_cell_data_func(GtkTreeViewColumn *col, GtkCellRenderer *renderer, GtkTreeModel *model,
                          GtkTreeIter *iter, gpointer user_data)
{
    guint id;
    gchar buf[30];
    gtk_tree_model_get(model, iter, KATHID, &id, -1);
    g_snprintf(buf, sizeof(buf), "%d", id);
    g_object_set(renderer, "text", buf, NULL);
}

void kath_name_cell_data_func(GtkTreeViewColumn *col, GtkCellRenderer *renderer, GtkTreeModel *model,
                          GtkTreeIter *iter, gpointer user_data)
{
    gchar* name;
    gtk_tree_model_get(model, iter, KATHNAME, &name, -1);
    g_object_set(renderer, "text", g_locale_to_utf8(name, -1, NULL, NULL, NULL), NULL);
}

void disc_id_cell_data_func(GtkTreeViewColumn *col, GtkCellRenderer *renderer, GtkTreeModel *model,
                          GtkTreeIter *iter, gpointer user_data)
{
    guint id;
    gchar buf[30];
    gtk_tree_model_get(model, iter, DISCID, &id, -1);
    g_snprintf(buf, sizeof(buf), "%d", id);
    g_object_set(renderer, "text", buf, NULL);
}


void disc_name_cell_data_func(GtkTreeViewColumn *col, GtkCellRenderer *renderer, GtkTreeModel *model,
                          GtkTreeIter *iter, gpointer user_data)
{
    gchar* name;
    gtk_tree_model_get(model, iter, DISCNAME, &name, -1);
    g_object_set(renderer, "text", g_locale_to_utf8(name, -1, NULL, NULL, NULL), NULL);
}


void disc_lekc_cell_data_func(GtkTreeViewColumn *col, GtkCellRenderer *renderer, GtkTreeModel *model,
                          GtkTreeIter *iter, gpointer user_data)
{
    gchar* name;
    gtk_tree_model_get(model, iter, DISCLEC, &name, -1);
    g_object_set(renderer, "text", g_locale_to_utf8(name, -1, NULL, NULL, NULL), NULL);
}


void disc_prakt_cell_data_func(GtkTreeViewColumn *col, GtkCellRenderer *renderer, GtkTreeModel *model,
                          GtkTreeIter *iter, gpointer user_data)
{
    gchar* name;
    gtk_tree_model_get(model, iter, DISCPRACT, &name, -1);
    g_object_set(renderer, "text", g_locale_to_utf8(name, -1, NULL, NULL, NULL), NULL);
}


void disc_labs_cell_data_func(GtkTreeViewColumn *col, GtkCellRenderer *renderer, GtkTreeModel *model,
                          GtkTreeIter *iter, gpointer user_data)
{
    gchar* name;
    gtk_tree_model_get(model, iter, DISCLAB, &name, -1);
    g_object_set(renderer, "text", g_locale_to_utf8(name, -1, NULL, NULL, NULL), NULL);
}


void kontr_id_cell_data_func(GtkTreeViewColumn *col, GtkCellRenderer *renderer, GtkTreeModel *model,
                          GtkTreeIter *iter, gpointer user_data)
{
    guint id;
    gchar buf[30];
    gtk_tree_model_get(model, iter, KONTRID, &id, -1);
    g_snprintf(buf, sizeof(buf), "%d", id);
    g_object_set(renderer, "text", buf, NULL);
}

void kontr_name_cell_data_func(GtkTreeViewColumn *col, GtkCellRenderer *renderer, GtkTreeModel *model,
                          GtkTreeIter *iter, gpointer user_data)
{
    gchar* name;
    gtk_tree_model_get(model, iter, KONTRNAME, &name, -1);
    g_object_set(renderer, "text", g_locale_to_utf8(name, -1, NULL, NULL, NULL), NULL);
}

int main(int argc, char **argv)
{
    GtkBuilder *builder;
    GError *error = NULL;
    gtk_init(&argc, &argv);

    builder = gtk_builder_new();

    if (!gtk_builder_add_from_file(builder, UI_FILE, &error))
    {
        g_warning("%s\n", error->message);
        g_free(error);
        return (1);
    }

    mainWindowObjects.main_window = GTK_WINDOW(gtk_builder_get_object(builder, "main_window"));
    mainWindowObjects.treeview =
        GTK_TREE_VIEW(gtk_builder_get_object(builder, "treeview_components"));
    mainWindowObjects.liststore =
        GTK_LIST_STORE(gtk_builder_get_object(builder, "liststore_components"));
    mainWindowObjects.adjustment =
        GTK_ADJUSTMENT(gtk_builder_get_object(builder, "adjustment_price"));
    mainWindowObjects.combobox =
        GTK_COMBO_BOX(gtk_builder_get_object(builder, "combo_select_table"));


    mainWindowObjects.checkbox =
        GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, "show_hidden"));  


    mainWindowObjects.column_kath_id =
        GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "cln_kath_id"));
    GtkCellRenderer *cell = GTK_CELL_RENDERER(gtk_builder_get_object(builder, "clnrender_kath_id"));
    gtk_tree_view_column_set_cell_data_func(mainWindowObjects.column_kath_id, cell,
                                                kath_id_cell_data_func, NULL, NULL);

    mainWindowObjects.column_kath_name =
        GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "cln_kath_name"));
    cell = GTK_CELL_RENDERER(gtk_builder_get_object(builder, "clnrender_kath_name"));
    gtk_tree_view_column_set_cell_data_func(mainWindowObjects.column_kath_name, cell,
                                            kath_name_cell_data_func, NULL, NULL);

    mainWindowObjects.column_disc_id =
        GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "cln_disc_id"));
    cell = GTK_CELL_RENDERER(gtk_builder_get_object(builder, "clnrender_disc_id"));
    gtk_tree_view_column_set_cell_data_func(mainWindowObjects.column_disc_id, cell,
                                            disc_id_cell_data_func, NULL, NULL);


    mainWindowObjects.column_disc_name =
        GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "cln_disc_name"));
    cell = GTK_CELL_RENDERER(gtk_builder_get_object(builder, "clnrender_disc_name"));
    gtk_tree_view_column_set_cell_data_func(mainWindowObjects.column_disc_name, cell,
                                            disc_name_cell_data_func, NULL, NULL);


    mainWindowObjects.column_disc_lec =
        GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "cln_disc_lec"));
    cell = GTK_CELL_RENDERER(gtk_builder_get_object(builder, "clnrender_disc_lec"));
    gtk_tree_view_column_set_cell_data_func(mainWindowObjects.column_disc_lec, cell,
                                            disc_lekc_cell_data_func, NULL, NULL);

    mainWindowObjects.column_disc_pract =
        GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "cln_disc_pract"));
    cell = GTK_CELL_RENDERER(gtk_builder_get_object(builder, "clnrender_disc_pract"));
    gtk_tree_view_column_set_cell_data_func(mainWindowObjects.column_disc_pract, cell,
                                            disc_prakt_cell_data_func, NULL, NULL);

    mainWindowObjects.column_disc_lab =
        GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "cln_disc_lab"));
    cell = GTK_CELL_RENDERER(gtk_builder_get_object(builder, "clnrender_disc_lab"));
    gtk_tree_view_column_set_cell_data_func(mainWindowObjects.column_disc_lab, cell,
                                            disc_labs_cell_data_func, NULL, NULL);


    mainWindowObjects.column_kontr_id =
        GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "cln_kontr_id"));
    cell = GTK_CELL_RENDERER(gtk_builder_get_object(builder, "clnrender_kontr_id"));
    gtk_tree_view_column_set_cell_data_func(mainWindowObjects.column_kontr_id, cell,
                                            kontr_id_cell_data_func, NULL, NULL);
// GtkComboBox *lecturer_box = (GtkComboBox *)data;


    mainWindowObjects.column_kontr_name =
        GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "cln_kontr_name"));
    cell = GTK_CELL_RENDERER(gtk_builder_get_object(builder, "clnrender_kontr_name"));
    gtk_tree_view_column_set_cell_data_func(mainWindowObjects.column_kontr_name, cell,
                                            kontr_name_cell_data_func, NULL, NULL);


    gtk_builder_connect_signals(builder, &mainWindowObjects);

    g_object_unref(G_OBJECT(builder));
    gtk_widget_show_all(GTK_WIDGET(mainWindowObjects.main_window));

    sqlite_get_data();
    sqlite_get_kathedra_combo();
    sqlite_get_kontr_combo();



    gtk_main();
}

G_MODULE_EXPORT void on_btnsave_clicked(GtkWidget *button, gpointer data)
{
    GtkTreeIter iter;
    gboolean reader =
        gtk_tree_model_get_iter_first(GTK_TREE_MODEL(mainWindowObjects.liststore), &iter);
    gint active_item = gtk_combo_box_get_active(mainWindowObjects.combobox);
    while (reader)
    {
        gint kathid;
        gchar *kathname;
        gint discid;
        gchar *discname;
        gchar *disclect;
        gchar *discpract;
        gchar *disclab;
        gint kontrid;
        gchar *kontrname;
        gtk_tree_model_get(GTK_TREE_MODEL(mainWindowObjects.liststore), &iter, KATHID, &kathid,
                           KATHNAME, &kathname, DISCID, &discid, DISCNAME, &discname, 
                           DISCLEC, &disclect, DISCPRACT, &discpract, DISCLAB, &disclab,
                           KONTRID, &kontrid, KONTRNAME, &kontrname, -1);

        if(active_item == 0)
        {
            sqlite_kath_update(kathid, kathname);
        }
        else if(active_item == 1)
        {
            sqlite_kontr_update(kontrid, kontrname);
        }
        else if(active_item == 2)
        {// GtkComboBox *lecturer_box = (GtkComboBox *)data;

           sqlite_disc_update(discid, discname, disclect, discpract, disclab);
           sqlite_kath_update(kathid, kathname);
           sqlite_kontr_update(kontrid, kontrname);
        }
        reader = gtk_tree_model_iter_next(GTK_TREE_MODEL(mainWindowObjects.liststore), &iter);
    }
    gtk_list_store_clear(mainWindowObjects.liststore);
    if(active_item == 0)
    {
        sqlite_get_kathedra_data();
    }
    else if(active_item == 1)
    {
        sqlite_get_kontr_data();
    }// GtkComboBox *lecturer_box = (GtkComboBox *)data;

    else if(active_item == 2)
    {
       sqlite_get_data();
    }
}

G_MODULE_EXPORT void on_cellrenderertext_kathname_edited(GtkCellRendererText *renderer, gchar *path,
                                                         gchar *new_text, gpointer data)
{
    if (g_ascii_strcasecmp(new_text, "") != 0)
    {
        GtkTreeIter iter;
        GtkTreeModel *model;
        model = gtk_tree_view_get_model(mainWindowObjects.treeview);
        if (gtk_tree_model_get_iter_from_string(model, &iter, path))
            gtk_list_store_set(GTK_LIST_STORE(model), &iter, KATHNAME, new_text, -1);
    }
}

G_MODULE_EXPORT void on_cellrenderertext_discname_edited(GtkCellRendererText *renderer, gchar *path,
                                                         gchar *new_text, gpointer data)
{
    if (g_ascii_strcasecmp(new_text, "") != 0)
    {
        GtkTreeIter iter;
        GtkTreeModel *model;
        model = gtk_tree_view_get_model(mainWindowObjects.treeview);
        if (gtk_tree_model_get_iter_from_string(model, &iter, path))
            gtk_list_store_set(GTK_LIST_STORE(model), &iter, DISCNAME, new_text, -1);
    }
}

G_MODULE_EXPORT void on_cellrenderertext_lekc_edited(GtkCellRendererText *renderer, gchar *path,
                                                         gchar *new_text, gpointer data)
{
    if (g_ascii_strcasecmp(new_text, "") != 0)
    {
        GtkTreeIter iter;
        GtkTreeModel *model;
        model = gtk_tree_view_get_model(mainWindowObjects.treeview);
        if (gtk_tree_model_get_iter_from_string(model, &iter, path))
            gtk_list_store_set(GTK_LIST_STORE(model), &iter, DISCLEC, new_text, -1);
    }
}

G_MODULE_EXPORT void on_cellrenderertext_prakct_edited(GtkCellRendererText *renderer, gchar *path,
                                                         gchar *new_text, gpointer data)
{
    if (g_ascii_strcasecmp(new_text, "") != 0)
    {
        GtkTreeIter iter;
        GtkTreeModel *model;
        model = gtk_tree_view_get_model(mainWindowObjects.treeview);
        if (gtk_tree_model_get_iter_from_string(model, &iter, path))
            gtk_list_store_set(GTK_LIST_STORE(model), &iter, DISCPRACT, new_text, -1);
    }
}

G_MODULE_EXPORT void on_cellrenderertext_lab_edited(GtkCellRendererText *renderer, gchar *path,
                                                         gchar *new_text, gpointer data)
{
    if (g_ascii_strcasecmp(new_text, "") != 0)
    {
        GtkTreeIter iter;
        GtkTreeModel *model;
        model = gtk_tree_view_get_model(mainWindowObjects.treeview);
        if (gtk_tree_model_get_iter_from_string(model, &iter, path))
            gtk_list_store_set(GTK_LIST_STORE(model), &iter, DISCLAB, new_text, -1);
    }
}

G_MODULE_EXPORT void on_cellrenderertext_kontrname_edited(GtkCellRendererText *renderer, gchar *path,
                                                         gchar *new_text, gpointer data)
{
    if (g_ascii_strcasecmp(new_text, "") != 0)
    {
        GtkTreeIter iter;
        GtkTreeModel *model;
        model = gtk_tree_view_get_model(mainWindowObjects.treeview);
        if (gtk_tree_model_get_iter_from_string(model, &iter, path))
            gtk_list_store_set(GTK_LIST_STORE(model), &iter, KONTRNAME, new_text, -1);
    }
}

G_MODULE_EXPORT void on_show_hidden_toggled(GtkToggleButton *button, gpointer data)
{
    gboolean visible = gtk_toggle_button_get_active(button);
    gint active_item = gtk_combo_box_get_active(mainWindowObjects.combobox);
    if(active_item == 0)
    {
        gtk_tree_view_column_set_visible(mainWindowObjects.column_kath_id, visible);
    }
    else if(active_item == 1)
    {
        gtk_tree_view_column_set_visible(mainWindowObjects.column_kontr_id, visible);
    }
    else if(active_item == 2)
    {
        gtk_tree_view_column_set_visible(mainWindowObjects.column_kath_id, visible); 
        gtk_tree_view_column_set_visible(mainWindowObjects.column_disc_id, visible);  
        gtk_tree_view_column_set_visible(mainWindowObjects.column_kontr_id, visible); 
    }
}

G_MODULE_EXPORT void on_btnabout_clicked(GtkButton *button, gpointer data)
{
    GtkWidget *dialog = gtk_dialog_new_with_buttons(
        "О программе", mainWindowObjects.main_window,
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, "_OK", GTK_RESPONSE_NONE, NULL);
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    gtk_container_set_border_width(GTK_CONTAINER(content_area), 15);
    GtkWidget *label = gtk_label_new("\nЛабораторная работа №3, реализовано добавление, изменение и удаление,\nкак для объединения таблиц, так и для каждой отдельной таблицы\n");
    gtk_container_add(GTK_CONTAINER(content_area), label);
    gtk_widget_show(label);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}
        gint a = 0;
        gint b = 0;
        gint c = 0;



G_MODULE_EXPORT void on_btndelete_clicked(GtkButton *button, gpointer data)
{
    GtkTreeSelection *selection;
    GtkTreeModel *model;
    GtkTreeIter iter;

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(mainWindowObjects.treeview));
    if (gtk_tree_selection_get_selected(selection, &model, &iter))
    {
        int kath_id, disc_id, kontr_id;

        gtk_tree_model_get (model, &iter, KATHID, &kath_id, DISCID, &disc_id, KONTRID, &kontr_id, -1);

        gint active_item = gtk_combo_box_get_active(mainWindowObjects.combobox);
        if(active_item == 0)
        {
            sqlite_kath_delete(kath_id);
            gtk_list_store_clear(mainWindowObjects.liststore);
            sqlite_get_kathedra_data();
        }
        else if(active_item == 1)
        {
            sqlite_kontr_delete(kontr_id);
            gtk_list_store_clear(mainWindowObjects.liststore);
            sqlite_get_kontr_data();
        }
        else if(active_item == 2)
        {
            sqlite_disc_delete(disc_id);
            gtk_list_store_clear(mainWindowObjects.liststore);
            sqlite_get_data();
        }
    }
}

G_MODULE_EXPORT void on_combo_changed(GtkComboBox *combo, gpointer data)
{
    gint active_item = gtk_combo_box_get_active(combo);
    if(active_item == 0)
    {
        gtk_tree_view_column_set_visible(mainWindowObjects.column_kath_name, 1);
        gtk_tree_view_column_set_visible(mainWindowObjects.column_disc_name, 0);
        gtk_tree_view_column_set_visible(mainWindowObjects.column_disc_lec, 0);
        gtk_tree_view_column_set_visible(mainWindowObjects.column_disc_pract, 0);
        gtk_tree_view_column_set_visible(mainWindowObjects.column_disc_lab, 0);
        gtk_tree_view_column_set_visible(mainWindowObjects.column_kontr_name, 0);

        gtk_list_store_clear(mainWindowObjects.liststore);
        sqlite_get_kathedra_data();
    }
    else if(active_item == 1)
    {
        gtk_tree_view_column_set_visible(mainWindowObjects.column_kath_name, 0);
        gtk_tree_view_column_set_visible(mainWindowObjects.column_disc_name, 0);
        gtk_tree_view_column_set_visible(mainWindowObjects.column_disc_lec, 0);
        gtk_tree_view_column_set_visible(mainWindowObjects.column_disc_pract, 0);
        gtk_tree_view_column_set_visible(mainWindowObjects.column_disc_lab, 0);
        gtk_tree_view_column_set_visible(mainWindowObjects.column_kontr_name, 1);


        gtk_list_store_clear(mainWindowObjects.liststore);
        sqlite_get_kontr_data();
    }
    else if(active_item == 2)
    {
        gtk_tree_view_column_set_visible(mainWindowObjects.column_kath_name, 1);
        gtk_tree_view_column_set_visible(mainWindowObjects.column_disc_name, 1);
        gtk_tree_view_column_set_visible(mainWindowObjects.column_disc_lec, 1);
        gtk_tree_view_column_set_visible(mainWindowObjects.column_disc_pract, 1);
        gtk_tree_view_column_set_visible(mainWindowObjects.column_disc_lab, 1);
        gtk_tree_view_column_set_visible(mainWindowObjects.column_kontr_name, 1);

        gtk_list_store_clear(mainWindowObjects.liststore);
        sqlite_get_data();
    }

    gtk_tree_view_column_set_visible(mainWindowObjects.column_kath_id, 0); 
    gtk_tree_view_column_set_visible(mainWindowObjects.column_disc_id, 0);  
    gtk_tree_view_column_set_visible(mainWindowObjects.column_kontr_id, 0); 

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(mainWindowObjects.checkbox), 0);
}

G_MODULE_EXPORT void on_window_destroy(GtkWidget *window, gpointer data)
{
    gtk_main_quit();
}

G_MODULE_EXPORT void on_btnexit_clicked(GtkWidget *window, gpointer data)
{
    gtk_main_quit();
}
G_MODULE_EXPORT void on_add_kathedra_btn_clicked(GtkWidget *window, gpointer data)
{
    GtkBuilder *builder;
    GError *error = NULL;

    builder = gtk_builder_new();

    if (!gtk_builder_add_from_file(builder, UI_FILE, &error))
    {
        g_warning("%s\n", error->message);
        g_free(error);
        return (1);
    }

    kathedra_dialog_objects.kathedra_dialog = GTK_DIALOG(gtk_builder_get_object(builder, "kathedra_dialog"));
    kathedra_dialog_objects.save_kathedra_btn = GTK_BUTTON(gtk_builder_get_object(builder, "save_kathedra_btn"));
    kathedra_dialog_objects.cancel_kathedra_btn = GTK_BUTTON(gtk_builder_get_object(builder, "cancel_kathedra_btn"));
    kathedra_dialog_objects.entry_name_kath = GTK_ENTRY(gtk_builder_get_object(builder, "entry_name_kath"));
    gtk_builder_connect_signals(builder, &kathedra_dialog_objects);
    g_object_unref(G_OBJECT(builder));
    gtk_widget_show_all(GTK_WIDGET(kathedra_dialog_objects.kathedra_dialog));
}

G_MODULE_EXPORT void on_cancel_kathedra_btn_clicked(GtkWidget *window, gpointer data)
{
    gtk_widget_destroy(kathedra_dialog_objects.kathedra_dialog);
}

G_MODULE_EXPORT void on_save_kathedra_btn_clicked(GtkWidget *window, gpointer data)
{
    const gchar *kath_name = gtk_entry_get_text(GTK_ENTRY(kathedra_dialog_objects.entry_name_kath));
    if (g_ascii_strcasecmp(kath_name, "") != 0)
    {
        sqlite_kath_insert(kath_name);
        yes_dialog_init("Успех");
        gtk_widget_destroy(kathedra_dialog_objects.kathedra_dialog);
        gint active_item = gtk_combo_box_get_active(mainWindowObjects.combobox);
        sqlite_get_kathedra_combo();
        if (active_item == 0)
        {
            gtk_list_store_clear(mainWindowObjects.liststore);
            sqlite_get_kathedra_data();
        }
        
    }
    else
    {
        yes_dialog_init((stderr, " Error : % s\n ", " Empty kathedra name "));
    
    }
}

void yes_dialog_init(char *text_secondary)
{
    GtkBuilder *builder;
    GError *error = NULL;

    builder = gtk_builder_new();

    if (!gtk_builder_add_from_file(builder, UI_FILE, &error))
    {
        g_warning("%s\n", error->message);
        g_free(error);
        return (1);
    }
    yes_dialog_objects.yes_dialog = GTK_DIALOG(gtk_builder_get_object(builder, "yes_dialog"));
    yes_dialog_objects.yes_btn = GTK_BUTTON(gtk_builder_get_object(builder, "yes_btn"));
    yes_dialog_objects.yes_label = GTK_LABEL(gtk_builder_get_object(builder, "yes_label"));
    gtk_builder_connect_signals(builder, &yes_dialog_objects);
    g_object_unref(G_OBJECT(builder));
    gtk_widget_show_all(GTK_WIDGET(yes_dialog_objects.yes_dialog));
    gtk_label_set_text(yes_dialog_objects.yes_label, text_secondary);
}

G_MODULE_EXPORT void on_add_kontrol_bnt_clicked(GtkWidget *window, gpointer data)
{
    GtkBuilder *builder;
    GError *error = NULL;

    builder = gtk_builder_new();

    if (!gtk_builder_add_from_file(builder, UI_FILE, &error))
    {
        g_warning("%s\n", error->message);
        g_free(error);
        return (1);
    }

    kontrol_dialog_objects.kontrol_dialog = GTK_DIALOG(gtk_builder_get_object(builder, "kontrol_dialog"));
    kontrol_dialog_objects.kontrol_save_btn = GTK_BUTTON(gtk_builder_get_object(builder, "kontrol_save_btn"));
    kontrol_dialog_objects.kontrol_cancel_btn = GTK_BUTTON(gtk_builder_get_object(builder, "kontrol_cancel_btn"));
    kontrol_dialog_objects.entry_name_kontr = GTK_ENTRY(gtk_builder_get_object(builder, "entry_name_kontr"));
    gtk_builder_connect_signals(builder, &kontrol_dialog_objects);
    g_object_unref(G_OBJECT(builder));
    gtk_widget_show_all(GTK_WIDGET(kontrol_dialog_objects.kontrol_dialog));
}

G_MODULE_EXPORT void kontrol_save_btn_clicked(GtkWidget *window, gpointer data)
{
    const gchar *kontr_name = gtk_entry_get_text(GTK_ENTRY(kontrol_dialog_objects.entry_name_kontr));

    if (g_ascii_strcasecmp(kontr_name, "") != 0)
    {
        sqlite_kontr_insert(kontr_name);
        gtk_widget_destroy(kontrol_dialog_objects.kontrol_dialog);
        yes_dialog_init("Контроль добавлен");
        gint active_item = gtk_combo_box_get_active(mainWindowObjects.combobox);
        sqlite_get_kontr_combo();
        if (active_item == 1)
        {
            gtk_list_store_clear(mainWindowObjects.liststore);
            sqlite_get_kontr_data();
        }
        
    }
    else
    {
        yes_dialog_init((stderr, " Error : % s\n ", " Empty kathedra name "));
    }
}

G_MODULE_EXPORT void kontrol_cancel_btn_clicked(GtkWidget *window, gpointer data)
{
    gtk_widget_destroy(kontrol_dialog_objects.kontrol_dialog);
}

G_MODULE_EXPORT void on_yes_bnt_clicked (GObject *object, gpointer data)
{
    gtk_widget_destroy(yes_dialog_objects.yes_dialog);
}

G_MODULE_EXPORT void on_add_discipline_btn_clicked(GtkWidget *window, gpointer data)
{
    GtkBuilder *builder;
    GError *error = NULL;

    builder = gtk_builder_new();

    if (!gtk_builder_add_from_file(builder, UI_FILE, &error))
    {
        g_warning("%s\n", error->message);
        g_free(error);
        return (1);
    }

    discipline_dialog_objects.discipline_dialog = GTK_DIALOG(gtk_builder_get_object(builder, "discipline_dialog"));
    discipline_dialog_objects.btninsert = GTK_BUTTON(gtk_builder_get_object(builder, "btninsert"));
    discipline_dialog_objects.discipline_cancel_btn = GTK_BUTTON(gtk_builder_get_object(builder, "discipline_cancel_btn"));
    discipline_dialog_objects.entry_name_disc = GTK_ENTRY(gtk_builder_get_object(builder, "entry_name_disc"));
    discipline_dialog_objects.entry_lec_disc = GTK_ENTRY(gtk_builder_get_object(builder, "entry_lec_disc"));
    discipline_dialog_objects.entry_pract_disc = GTK_ENTRY(gtk_builder_get_object(builder, "entry_pract_disc"));
    discipline_dialog_objects.entry_lab_disc = GTK_ENTRY(gtk_builder_get_object(builder, "entry_lab_disc"));
    discipline_dialog_objects.kathedra_box = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder, "kathedra_box"));
    discipline_dialog_objects.kontrol_box = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder, "kontrol_box"));
    gtk_builder_connect_signals(builder, &discipline_dialog_objects);
    g_object_unref(G_OBJECT(builder));
    gtk_widget_show_all(GTK_WIDGET(discipline_dialog_objects.discipline_dialog));
    gtk_combo_box_text_remove_all(discipline_dialog_objects.kontrol_box);
    sqlite_get_kontr_combo();
    gtk_combo_box_text_remove_all(discipline_dialog_objects.kathedra_box);
    sqlite_get_kathedra_combo();
}

G_MODULE_EXPORT void on_btninsert_clicked(GtkButton *button, gpointer data)
{
    const gchar *kath_name = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(discipline_dialog_objects.kathedra_box));
    const gchar *kontr_name = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(discipline_dialog_objects.kontrol_box));
    const gchar *disc_name = gtk_entry_get_text(GTK_ENTRY(discipline_dialog_objects.entry_name_disc));
    const gchar *disc_leck = gtk_entry_get_text(GTK_ENTRY(discipline_dialog_objects.entry_lec_disc));
    const gchar *disk_pract = gtk_entry_get_text(GTK_ENTRY(discipline_dialog_objects.entry_pract_disc));
    const gchar *disc_lab  = gtk_entry_get_text(GTK_ENTRY(discipline_dialog_objects.entry_lab_disc));

    sqlite_disc_insert(disc_name, disc_leck, disk_pract, disc_lab, kath_name, kontr_name);
    gtk_entry_set_text(discipline_dialog_objects.entry_pract_disc, "");
    gtk_entry_set_text(discipline_dialog_objects.entry_lab_disc, "");
    gtk_entry_set_text(discipline_dialog_objects.entry_lec_disc, "");
    gtk_list_store_clear(mainWindowObjects.liststore);
    sqlite_get_data();
    gtk_widget_destroy(discipline_dialog_objects.discipline_dialog);
    yes_dialog_init("Дисциплина добавлена");                             
}

G_MODULE_EXPORT void on_discipline_dialog_close(GtkWidget *window, gpointer data)
{
    gtk_widget_destroy(discipline_dialog_objects.discipline_dialog);
}