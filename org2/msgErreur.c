#include "menu.h"
#include "fonction.h"

/* lancerScript : appelle depuis le c un script shell 
   tout en verifiant que la commande d appelle c est bien 
   passee 

   char* script : chemin du script 
   char* args : parametre du script
   
*/

void lancerScript(char* script, char *args){
    
    int pid,retour,status;  

    /* creation d un processus fils pouvant lancer un script */    
    pid=fork();
    if(pid==0){
	/*lancement du script*/ 
	retour=execlp("sh","sh",script,args,NULL);
	perror("erreur execlp");
	exit(2);
	
    }else if(pid <0 ){
	
	perror("erreur fork");
	exit(1);
    }
    
    /* on attend le retour du fils */
    /* si il est de -1 il y a eu erreur 
       avec waitpid 
       si 2 erreur du script 
       sinon tout est bon 
    */
    
    retour=waitpid(pid, &status, 0);
    if(retour!=-1){
	if (WIFEXITED (status)) {
	    int exit_code = WEXITSTATUS (status);
	    if(exit_code != 2) {
		printf("script termine\n");
		
	    }else{
		printf("script mal termine :%d\n",exit_code);
		
	    }
	}
	
    }else{
	perror("erreur wait ");
	exit(1);
    }   
}

int main(int argc, char **argv){

    GtkWidget *btnOK;
    GtkWidget *window;
    GtkWidget *vBox;
    GtkWidget *labelSupport;
    
    char * msg=argv[1];


    gtk_init(&argc, &argv);
 
    window=gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position( window,GTK_WIN_POS_CENTER);
    gtk_window_set_title (GTK_WINDOW (window), "Installation FrogLinux");
    gtk_window_set_default_size(GTK_WINDOW(window), 250, 100);

    vBox = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vBox);
    gtk_container_add (GTK_CONTAINER (window), vBox);

    labelSupport = gtk_label_new (msg);
    gtk_widget_show (labelSupport);
    gtk_box_pack_start (GTK_BOX (vBox), labelSupport, TRUE, FALSE, 0);
    gtk_label_set_justify (GTK_LABEL (labelSupport), GTK_JUSTIFY_CENTER);
    
     
    btnOK = gtk_button_new_from_stock ("gtk-ok");
    gtk_widget_show (btnOK);
    gtk_box_pack_start (GTK_BOX (vBox), btnOK, TRUE, FALSE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (btnOK), 4);

    g_signal_connect ((gpointer) btnOK, "clicked",
		      G_CALLBACK (gtk_main_quit),
		      NULL);
    
    g_signal_connect ((gpointer) window, "destroy",
		      G_CALLBACK (gtk_main_quit),
		      NULL);

 

    gtk_widget_show_all(window);


    gtk_main();

    
    return EXIT_SUCCESS;


}
