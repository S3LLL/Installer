
 
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>


GSList *gtk_data_list_add_by_name(GSList *list, gpointer data, gchar *name);
GSList *gtk_data_list_remove_by_name(GSList *list, gchar *name);
gpointer *gtk_data_list_get_by_name(GSList *list, gchar *name);
void gtk_data_list_remove_all(GSList *list);


