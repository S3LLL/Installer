/****************************************************
 *          INSTALLATEUR FROGLINUX                   *
 *                                                   *
 *programmeur : POLLONGHINI Yohann                   *
 *date : 08/05/2008                                  *
 *                                                   *
 *                                                   *
 ****************************************************/

#include "fonction.h"

int lancerScriptInt(char* script, char *args){   /* execution d'un script shell avec valeur de retour */

    int pid,retour,status,exit_code;
    /* creation d un processus fils pouvant lancer un script */

    pid=fork();
    if(pid==0){
	/*lancement du script*/
	retour=execlp("bash","bash",script,args,NULL);
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
	    exit_code = WEXITSTATUS (status);
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
    printf("script exit code :%d\n",exit_code);
    return exit_code;

}

void lancerScript(char* script, char *args){    /* execution d'un script shell sans valeur de retour */


    int pid,retour,status;

    /* creation d un processus fils pouvant lancer un script */
    pid=fork();
    if(pid==0){
	/*lancement du script*/
	retour=execlp("bash","bash",script,args,NULL);
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

gboolean existeDejaDansListeExclusion(char *var){  /* foncion qui test si un element existe deja dans la liste d'exclusion utilisateur */



    char *chemin="/var/lib/installFrogLinux/fichiers_exclus_utilisateur.list";
    char *dossier=strdup(var);
    FILE *in;
    char ligne[512];


    //printf("on verifie si %s exite dans le fichier\n",var);
    strcat(dossier,"\n");


    in = fopen(chemin, "r");  /* read */
    if(in!=NULL){

	while(fgets(ligne, sizeof ligne, in) != NULL){
	    //printf("ligne courante %s\n",ligne);

	    if(strcmp(ligne,dossier)==0){
		//printf("doublon trouve avec %s\n",ligne);
		return (1);
	    }
	}
    }

    //printf("pas de doublon trouve\n");
    return (0);

}

void supprimerListeExclusion(char *var){  /* fonction qui supprime un element de la liste d'exclusion */


    FILE *in,*out;
    char *chemin="/var/lib/installFrogLinux/fichiers_exclus_utilisateur.list";
    char *cheminTmp="/var/lib/installFrogLinux/fichiers_exclus_utilisateurTmp.list";
    char ligne[512];

    strcat(var,"\n");

    out = fopen(cheminTmp, "w");
    in = fopen(chemin, "r+");  /* read */

    if(in!=NULL){

	while(fgets(ligne, sizeof ligne, in) != NULL){
	    /* temps que la ligne courante et differente
	       de la ligne passee en parametre */

	    if(!(strcmp(ligne,var)==0)){
		/* on ecrit la ligne courante dans le
		   fichier temporaire */

		fprintf(out,"%s",ligne);

	    }
	}

	/* on supprime l ancien fichier d entree et
	   on renome le fichier temporaire */

	remove("/var/lib/installFrogLinux/fichiers_exclus_utilisateur.list");
	rename("/var/lib/installFrogLinux/fichiers_exclus_utilisateurTmp.list","fichiers_exclus_utilisateur.list");
	fclose(out);
	fclose(in);
    }else{
	fprintf(stderr,"pas de fichier de sauvegarde trouvé\n");

    }
}

void sauverListeExclusion(char *var){   /* fonction qui sauvegarde tous les element de la liste d'exclusion  */

    FILE *f;

    char *chemin="/var/lib/installFrogLinux/fichiers_exclus_utilisateur.list";

    //fprintf(stderr,"on sauve :||%s||\n",var);
    f = fopen(chemin, "a");
    fprintf(f,"%s\n",var);
    fclose(f);

}

void lancerProg(char* prog, char *args){  /* fonction qui lance une application */




    int pid_g,retour_g,status,retour;

    /* creation d un processus fils pouvant lancer une appli */
    pid_g=fork();
    if(pid_g==0){
	/*lancement du prog*/
	retour=execlp(prog,prog,args,NULL);
	perror("erreur execlp");
	exit(2);

    }else if(pid_g <0 ){

	perror("erreur fork");
	exit(1);
    }

    /* on attend le retour du fils */
    /* si il est de -1 il y a eu erreur
       avec waitpid
       si 2 erreur du prog
       sinon tout est bon
    */

    retour_g=waitpid(pid_g, &status, 0);
    if(retour_g!=-1){
	if (WIFEXITED (status)) {
	    int exit_code = WEXITSTATUS (status);
	    if(exit_code != 2) {
		printf("prog lancer\n");

	    }else{
		printf("prog mal lancer :%d\n",exit_code);

	    }
	}

    }else{
	perror("erreur wait ");
	exit(1);
    }

}

gboolean progressPuls(){    /* fonction permettent de faire bouger la barre de progression  */


    GtkWidget *bar=(GtkWidget *)gtk_data_list_get_by_name(liste,"bar");
    gtk_progress_bar_pulse (GTK_PROGRESS_BAR (bar));
    return (1);
}

gboolean progressPuls2(){   /* fonction permettent de faire bouger la barre de progression2 */


    GtkWidget *bar2=(GtkWidget *)gtk_data_list_get_by_name(liste,"bar2");
    gtk_progress_bar_pulse (GTK_PROGRESS_BAR (bar2));
    return (1);
}

gboolean progressPuls3(){   /* fonction permettent de faire bouger la barre de progression3 */


    GtkWidget *bar3=(GtkWidget *)gtk_data_list_get_by_name(liste,"bar3");
    gtk_progress_bar_pulse (GTK_PROGRESS_BAR (bar3));
    return (1);
}

void *lancerThreadUsb(void *args){      /* fonction qui lance le script d'installation USB  */

    int retour=0;
    GtkWidget *fenetreIns,*fenetreProg,*pAbout;
    GtkWidget *window = (GtkWidget *)gtk_data_list_get_by_name(liste,"FenetreInsUsb");
    fenetreIns = (GtkWidget *)gtk_data_list_get_by_name(liste,"FenetreInsUsb");
    fenetreProg = (GtkWidget *)gtk_data_list_get_by_name(liste,"FenetreProgInsUsb");

    gdk_threads_enter();
    gtk_widget_show(fenetreProg);
    gtk_widget_hide(fenetreIns);
    gdk_threads_leave();

    retour = lancerScriptInt("/var/lib/installFrogLinux/instal_periph_usb.sh",NULL);


    if (retour==0){

    gdk_threads_enter();
    gtk_widget_hide(fenetreProg);
    gdk_threads_leave();

    gdk_threads_enter();
    gchar *sSite = "http://www.froglinux.com";


        pAbout = gtk_message_dialog_new (GTK_WINDOW(window),
                        GTK_DIALOG_MODAL,
                        GTK_MESSAGE_INFO,
                        GTK_BUTTONS_OK,
                        "Fin de l'installation, \nmerci d'utiliser Froglinux, vous pouvez visiter :\n%s", sSite);

        gtk_dialog_run(GTK_DIALOG(pAbout));
        gtk_widget_destroy(pAbout);
    gdk_threads_leave();

     gdk_threads_enter();

     gtk_widget_show(fenetreIns);
     gdk_threads_leave();


    }else if (retour==2){
        gdk_threads_enter();
        pAbout = gtk_message_dialog_new (GTK_WINDOW(window),
                        GTK_DIALOG_MODAL,
                        GTK_MESSAGE_ERROR,
                        GTK_BUTTONS_OK,
                        "Une erreur est survenue lors de l'installation.\nVeuillez recommencer ou contacter support@froglinux.com");

        gtk_dialog_run(GTK_DIALOG(pAbout));
        gtk_widget_destroy(pAbout);
        gdk_threads_leave();


        gdk_threads_enter();
        lancerScript("/var/lib/installFrogLinux/kill_install.sh","USB");
        gtk_widget_hide(fenetreProg);
        gtk_widget_show(fenetreIns);
        gdk_threads_leave();
    }else{
        gdk_threads_enter();
        gtk_widget_hide(fenetreProg);
        gtk_widget_show(fenetreIns);
        gdk_threads_leave();
    }

}

void *lancerThreadDur(void *args){      /* fonction qui lance le script d'installation Disque rigide auto */

    int retour=1;
    GtkWidget *fenetreMode,*fenetreProg,*pAbout;

    fenetreMode = (GtkWidget *)gtk_data_list_get_by_name(liste,"FenetreChoixMode");
    fenetreProg = (GtkWidget *)gtk_data_list_get_by_name(liste,"FenetreProgInsDur");

    gdk_threads_enter();
    gtk_widget_show(fenetreProg);
    gtk_widget_hide(fenetreMode);
    gdk_threads_leave();

    retour= lancerScriptInt("/var/lib/installFrogLinux/install_HD.sh",NULL);  //NULL = pas de parametre pour le script donc compris automatique pour lui


    if (retour==0){

    gdk_threads_enter();
    gtk_widget_hide(fenetreProg);
    gdk_threads_leave();

    gdk_threads_enter();
    gchar *sSite = "http://www.froglinux.com";


        pAbout = gtk_message_dialog_new (GTK_WINDOW(fenetreMode),
                        GTK_DIALOG_MODAL,
                        GTK_MESSAGE_INFO,
                        GTK_BUTTONS_OK,
                        "Fin de l'installation, \nmerci d'utiliser froglinux, vous pouvez visiter :\n%s", sSite);

        gtk_dialog_run(GTK_DIALOG(pAbout));
        gtk_widget_destroy(pAbout);
        gdk_threads_leave();

     gdk_threads_enter();
     gtk_main_quit();
     gdk_threads_leave();


    }else if (retour == 2){
        gdk_threads_enter();
        pAbout = gtk_message_dialog_new (GTK_WINDOW(fenetreMode),
                        GTK_DIALOG_MODAL,
                        GTK_MESSAGE_ERROR,
                        GTK_BUTTONS_OK,
                        "Une erreur est survenue lors de l'installation.\nVeuillez recommencer ou contacter support@froglinux.com");

        gtk_dialog_run(GTK_DIALOG(pAbout));
        gtk_widget_destroy(pAbout);
        gdk_threads_leave();

        gdk_threads_enter();
        lancerScript("/var/lib/installFrogLinux/kill_install.sh","HD");
        gtk_widget_hide(fenetreProg);
        gtk_widget_show(fenetreMode);
        gdk_threads_leave();

    }else{

        gdk_threads_enter();
        gtk_widget_hide(fenetreProg);
        gtk_widget_show(fenetreMode);
        gdk_threads_leave();


    }

}

void *lancerThreadGParted(void *args){  /* fonction qui lance le script d'installation Disque rigide auto */

    GtkWidget *window;

    gdk_threads_enter();
    AidePartition(window);
    gdk_threads_leave();

    lancerProg("gparted",NULL);


}

void *lancerThreadGuide(void *args){    /* fonction qui lance le script d'installation Disque rigide guidee */

    int retour=1;
    GtkWidget *fenetreMode,*fenetreProg,*fenetreAide,*pAbout;

    fenetreMode = (GtkWidget *)gtk_data_list_get_by_name(liste,"FenetreChoixMode");
    fenetreProg = (GtkWidget *)gtk_data_list_get_by_name(liste,"FenetreProgInsGuide");
    fenetreAide = (GtkWidget *)gtk_data_list_get_by_name(liste,"FenetreAidePart");

    gdk_threads_enter();
    lancerScript("/var/lib/installFrogLinux/kill_gparted.sh",NULL);
    gtk_widget_show(fenetreProg);
    gtk_widget_hide(fenetreAide);
    gdk_threads_leave();

    retour= lancerScriptInt("/var/lib/installFrogLinux/install_HD.sh","guidee");


    if (retour==0){

    gdk_threads_enter();
    gtk_widget_hide(fenetreProg);
    gdk_threads_leave();

    gdk_threads_enter();
    gchar *sSite = "http://www.froglinux.com";


        pAbout = gtk_message_dialog_new (GTK_WINDOW(fenetreMode),
                        GTK_DIALOG_MODAL,
                        GTK_MESSAGE_INFO,
                        GTK_BUTTONS_OK,
                        "Fin de l'installation, \nmerci d'utiliser Froglinux, vous pouvez visiter :\n%s", sSite);

        gtk_dialog_run(GTK_DIALOG(pAbout));
        gtk_widget_destroy(pAbout);
    gdk_threads_leave();

     gdk_threads_enter();
     gtk_main_quit();
     gdk_threads_leave();


    }else if (retour ==2 ){
        gdk_threads_enter();
        pAbout = gtk_message_dialog_new (GTK_WINDOW(fenetreMode),
                        GTK_DIALOG_MODAL,
                        GTK_MESSAGE_ERROR,
                        GTK_BUTTONS_OK,
                        "Une erreur est survenue lors de l'installation.\nVeuillez recommencer ou contacter support@froglinux.com");

        gtk_dialog_run(GTK_DIALOG(pAbout));
        gtk_widget_destroy(pAbout);
        gdk_threads_leave();
        gdk_threads_enter();
        lancerScript("/var/lib/installFrogLinux/kill_install.sh","HD");
        gtk_widget_hide(fenetreProg);
        gtk_widget_show(fenetreMode);
        gdk_threads_leave();
    }else{

        gdk_threads_enter();
        gtk_widget_hide(fenetreProg);
        gtk_widget_show(fenetreMode);
        gdk_threads_leave();
    }
}

void clean(const char *buffer, FILE *fp){  /* retire les newlines des fgets */
       char *p = strchr(buffer,'\n');
       if (p != NULL)
              *p = 0;
       else
       {
              int c;
              while ((c = fgetc(fp)) != '\n' && c != EOF);
       }
}
