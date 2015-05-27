/****************************************************
 * fonction du programme autobackup                 *
 *                                                  *
 *                                                  *
 *                                                  *
 ****************************************************/
 
#include "listeChainee.h"



/* liste chainee pour les different widget */
GSList *gtk_data_list_add_by_name(GSList *list, gpointer data, gchar *name){
    /* D�claration d'une structure interne pour m�moriser le pointeur du widget
       ainsi que son nom.*/
    struct Struct_list_widget
    {
	gpointer Sdata;
	gchar *Sname;
    };
    struct Struct_list_widget *Slist_widget=NULL;
    
    Slist_widget=(struct Struct_list_widget*)
	g_malloc(sizeof(struct Struct_list_widget));
    /* Affection des valeurs*/
    Slist_widget->Sdata=data;
    Slist_widget->Sname=g_strdup(name);
    
    /* Ajout des nouvelles donn�es � la liste.*/
    list=g_slist_append(list, Slist_widget);
    return list;
}

GSList *gtk_data_list_remove_by_name(GSList *list, gchar *name){
	GSList *tmp_list=NULL;
	/* D�claration d'une structure interne pour m�moriser le pointeur du widget
	ainsi que son nom.*/
	struct Struct_list_widget
	{
		gpointer Sdata;
		gchar *Sname;
	};

	// Recherche de l'objet dans la liste
	tmp_list=list;
	while (tmp_list)
	{
		if (strcmp(((struct Struct_list_widget*)(tmp_list->data))->Sname,name)==0)
		{
			// Supression de l'objet
			g_free(((struct Struct_list_widget*)(tmp_list->data))->Sname);
			g_free((struct Struct_list_widget*)(tmp_list->data));
			list=g_slist_remove_link(list, tmp_list);
			g_slist_free_1(tmp_list);
			return list;
		}
		tmp_list=g_slist_next(tmp_list);
	}
	
	return NULL;
}

gpointer *gtk_data_list_get_by_name(GSList *list, gchar *name){
    GSList *tmp_list=NULL;
    /* D�claration d'une structure interne pour m�moriser le pointeur du widget
       ainsi que son nom.*/
    struct Struct_list_widget
    {
	gpointer Sdata;
	gchar *Sname;
    };
    
    // Recherche de l'objet dans la liste
    tmp_list=list;
    while (tmp_list)
	{
	    if (strcmp(((struct Struct_list_widget*)(tmp_list->data))->Sname,name)==0)
		return ((struct Struct_list_widget*)(tmp_list->data))->Sdata;
	    tmp_list=g_slist_next(tmp_list);
	}
    
    return NULL;
}

void gtk_data_list_remove_all(GSList *list){
    GSList *tmp_list=list;
    /* D�claration d'une structure interne pour m�moriser le pointeur du widget
       ainsi que son nom.*/
    struct Struct_list_widget
    {
	gpointer Sdata;
	gchar *Sname;
	};
    
    while (tmp_list)
	{
	    // Supression de l'objet
	    g_free(((struct Struct_list_widget*)(tmp_list->data))->Sname);
	    g_free((struct Struct_list_widget*)(tmp_list->data));
	    tmp_list=g_slist_next(tmp_list);
	}
    g_slist_free(list);
}


