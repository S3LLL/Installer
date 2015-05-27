/*****************************************************
 *          INSTALLATEUR FROGLINUX                   *
 *                                                   *
 *programmeur : POLLONGHINI Yohann                   *
 *date : 08/05/2008                                  *
 *                                                   *
 *                                                   *
 *****************************************************/

/*Ce fichier contient tout les evenement lier au bouton
de l'interface. Chaque bouton a son evenement propre.*/

#include "callbacks.h"
#include "menu.h"
#include "fonction.h"

void on_btnContinuerSupport_clicked(GtkButton *button, gpointer user_data) /* event sur le bouton continuer du choix du support */
{

    int choix;
	GtkWidget *choix_ddur,*choix_usb,*window,*fenetreSupport;

	/* initialisation des dossiers */
        remove("/var/lib/installFrogLinux/fichiers_exclus_utilisateurTmp.list");
        remove("/var/lib/installFrogLinux/fichiers_exclus_utilisateur.list");
        

    /*On cache la fenetre principal */
    fenetreSupport = (GtkWidget *)gtk_data_list_get_by_name(liste,"FenetreChoixSupport"); //On appel la fenetre du support dans la liste chainee
    gtk_widget_hide (fenetreSupport);

    /*Lancement de la prochain fenetre en fonciton du choix selectionne avec les buttons radio*/
	choix_ddur = (GtkWidget *)gtk_data_list_get_by_name(liste,"DDur");
	choix_usb = (GtkWidget *)gtk_data_list_get_by_name(liste,"USB");

	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(choix_ddur))){
		choix=1;

	}
	else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(choix_usb))){
		choix=2;

	}
	switch (choix){

	case 1 :
		ChoixDDurInstall(window);   //fenetre suivante pour disque rigide
		break;

	case 2 :
		InstallCleUsb(window); //fenetre suivante pour usb
		break;

	default :
		break;
	}
}

void on_btnContinuerDDurInst_cliked(GtkButton *button, gpointer user_data) /* event lors du choix de disque rigide */
{

    GtkWidget *dur,*window,*choixDur;
    choixDur = (GtkWidget *)gtk_data_list_get_by_name(liste,"FenetreChoixDur"); //recuperation de la fenetre du choix des disque dur
    gchar *intitule;


    ModeInstall(window); //lancement de la prochaine fenetre
	int i=1;
	do{


		char buffer[32];
		sprintf(buffer,"btnDur_%d",i);                                  //concatenation du nom de la variable afin de pouvoir lister tout les noms de disque exitant.
		dur = (GtkWidget *)gtk_data_list_get_by_name(liste,buffer);     //recuperation dans la liste chainee

		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(dur))){      //test si ce bouton est active

			intitule = gtk_button_get_label (GTK_BUTTON(dur));          //on recupere le nom du bouton
			printf(intitule);                                           //que l'on reinscrit dans le fichier
			FILE *pFile;                                                //qui est lue par le script shell afin
			pFile = fopen ("/var/lib/installFrogLinux/disque_cle.list","w");                       //de laisser une trace du choix
			fprintf(pFile,intitule);
			fclose(pFile);
			break;
		}else i++;

	}while(dur !=NULL);

        gtk_widget_hide (choixDur);                                     //on cache la fenetre actuel pour afficher la suivante


}

void on_btnContinuerMode_cliked(GtkButton *button, gpointer user_data) /* event lors du choix de mode d'installation */
{
    int choix,pid_g,retour;

    GtkWidget *automatique,*guide,*manuel,*fenetreMode,*window,*bar,*prog,*pQuestion,*pAbout,*pWindow;
    automatique = (GtkWidget *)gtk_data_list_get_by_name(liste,"Auto");                     //recuperation de tous les boutons radio
    guide = (GtkWidget *)gtk_data_list_get_by_name(liste,"Guide");                          //  |
    manuel = (GtkWidget *)gtk_data_list_get_by_name(liste,"Manuel");                        //  |
    fenetreMode = (GtkWidget *)gtk_data_list_get_by_name(liste,"FenetreChoixMode");         //recuperation de la fenetre actuel

    pWindow=(GtkWidget *)gtk_data_list_get_by_name(liste,"Attention");

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(automatique))){                       //test d'activation des boutons radio lors de
          choix = 1;                                                                        //la pression du bouton suivant
    }else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(guide))){
          choix = 2;

    }else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(manuel))){
          choix = 3;

    }

    switch(choix){

        case 1 :                                                                            //Choix du mode automatique = Formatage + message de prevention

                /* creation de la boite de message */
                /* type : Question -> GTK_MESSAGE_QUESTION */
                /* boutons : 1 OUI, 1 NON -> GTK_BUTTONS_YES_NO */
                pQuestion = gtk_message_dialog_new (GTK_WINDOW(pWindow),
                                			GTK_DIALOG_MODAL,
                                                	GTK_MESSAGE_WARNING,
                					GTK_BUTTONS_YES_NO,
                                			"Toutes les données de ce disque seront supprimées\nVoulez-vous continuer?");

                /* affichage et attente d une reponse */
                switch(gtk_dialog_run(GTK_DIALOG(pQuestion))){

                    case GTK_RESPONSE_YES:
                            /* OUI -> demarre le script d'install */
                            gtk_widget_destroy(pQuestion);

                            GThread  *thread1;
                            GError *error = NULL;
                            if ((thread1=g_thread_create(lancerThreadDur,NULL,FALSE, &error))==NULL){       //lancement du script d'installation dans un thread
                            g_printerr ("Failed to create script thread: %s\n", error->message);
                            }
                            break;

                    case GTK_RESPONSE_NO:
                            /* NON -> on detruit la boite de message */
                            gtk_widget_destroy(pQuestion);
                            break;
                }

                break;

        case 2 :                                                                                            //Mode guidee = Lancement Gparted + fenetre d'aide partition
                gtk_widget_hide(fenetreMode);

                GThread  *thread1;
                GError *error = NULL;
                if ((thread1=g_thread_create(lancerThreadGParted,NULL,FALSE, &error))==NULL){
                g_printerr ("Failed to create script thread: %s\n", error->message);
                }
                break;

        case 3 :

                /* creation d un processus fils pouvant lancer une appli */                                 //Lancement d'un terminal , Mode ultra expert
                pid_g=fork();
                if(pid_g==0){
                /*lancement du prog*/
                retour=execlp("gnome-terminal","gnome-terminal",NULL,NULL);
                perror("erreur execlp");
                exit(2);

                }else if(pid_g <0 ){

                perror("erreur fork");
                exit(1);
                }

                gtk_main_quit ();
                break;

        default :
                break;
    }

}

void on_btnContinuerAidePart_cliked(GtkButton *button, gpointer user_data) /* event lors du clique sur executer dans l'aide  */
{
    GtkWidget *pAbout,*window,*warning;
    window = (GtkWidget *)gtk_data_list_get_by_name(liste,"FenetreAidePart");
    int retour;
    retour = lancerScriptInt("/var/lib/installFrogLinux/test_partition.sh",NULL);                     //lancement du script pour tester les partitions

    if (retour==2){                                                         //test si les prerequis on ete fait.

        pAbout = gtk_message_dialog_new (GTK_WINDOW(window),
                        GTK_DIALOG_MODAL,
                        GTK_MESSAGE_ERROR,
                        GTK_BUTTONS_OK,
                        "Les conditions ne sont pas remplies, \nVeuillez relire les indications.");

        gtk_dialog_run(GTK_DIALOG(pAbout));
        gtk_widget_destroy(pAbout);


    }else{    


	warning = gtk_message_dialog_new (GTK_WINDOW(window),
                                			GTK_DIALOG_MODAL,
                                                	GTK_MESSAGE_WARNING,
                					GTK_BUTTONS_YES_NO,
                                			"Merci toutes les conditions sont remplies,\nVoulez-vous continuer?");

                /* affichage et attente d une reponse */
                switch(gtk_dialog_run(GTK_DIALOG(warning))){

                    case GTK_RESPONSE_YES:
                            /* OUI -> demarre le script d'install */
                            gtk_widget_destroy(warning);

                            //lancement de l'installation si tout est OK
        		    lancerScript("/var/lib/installFrogLinux/num_partition.sh",NULL);                              //lancement du script de num partition qui donne les numero de partition restante
        		    GThread  *thread1;                                                  //lancement de l'installation si tout est OK
        		    GError *error = NULL;
        		    if ((thread1=g_thread_create(lancerThreadGuide,NULL,FALSE, &error))==NULL){
        		    g_printerr ("Failed to create script thread: %s\n", error->message);
        		    }
                      
                            break;

                    case GTK_RESPONSE_NO:
                            /* NON -> on detruit la boite de message */
                            gtk_widget_destroy(warning);
                            break;
                }
    }
}

void on_btnDossier_clicked(GtkButton *button, gpointer user_data)   /* event lors du clique sur le bouton Ouvrir */
{


    GtkWidget *pFileSelection;
    GtkWidget *pDialog;
    GtkWidget *pParent;
    GtkWidget *combobox;

    /* recuperation du widget combobox*/
    combobox= (GtkWidget *)gtk_data_list_get_by_name(liste,"listeExclusion");
    gchar *sChemin;

    pParent = GTK_WIDGET(user_data);

    /* Creation de la fenetre de selection */
    pFileSelection = gtk_file_chooser_dialog_new("Ouvrir...",
	GTK_WINDOW(pParent),
	GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
	GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
	GTK_STOCK_OPEN, GTK_RESPONSE_OK,
	NULL);

    /* On limite les actions a cette fenetre */
    gtk_window_set_modal(GTK_WINDOW(pFileSelection), TRUE);

    /* Affichage fenetre */
    switch(gtk_dialog_run(GTK_DIALOG(pFileSelection)))
	{
        case GTK_RESPONSE_OK:
            /* Recuperation du chemin */
            sChemin = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(pFileSelection));
	    /* on test si le chemin selectionne existe deja */
	    if(existeDejaDansListeExclusion(sChemin)){

		/* affichage d un popup d erreur */
		pDialog=gtk_message_dialog_new(GTK_WINDOW(pFileSelection),
						 GTK_DIALOG_MODAL,
						 GTK_MESSAGE_INFO,
						 GTK_BUTTONS_OK,
						 "Le dossier :%s est déjà dans la liste des Exclusions\n", sChemin);


	    }else{
		/* si le chemin n est pas un doublon
		   alors on l insert dans le combobox

		*/

		gtk_combo_box_append_text(combobox,sChemin);
		sauverListeExclusion(sChemin);

		/* affichage d un popup de confiramation */
		pDialog=gtk_message_dialog_new(GTK_WINDOW(pFileSelection),
						 GTK_DIALOG_MODAL,
						 GTK_MESSAGE_INFO,
						 GTK_BUTTONS_OK,
						 "Ajout du dossier :%s\n", sChemin);

	    }

	    /* liberation de la memoire utilisee par gtk */
	    gtk_dialog_run(GTK_DIALOG(pDialog));
            gtk_widget_destroy(pDialog);
	    g_free(sChemin);
            break;
        default:
            break;
	}
    /* liberation de la memoire utilisee par gtk */
    gtk_widget_destroy(pFileSelection);
}

void on_btnSupprimer_clicked(GtkButton *button, gpointer user_data)  /* event lors du clique sur le bouton Supprimer */
{
    GtkWidget *combobox;
    GtkWidget *pDialog;

    char *dossier;
    gint position;

    combobox= (GtkWidget *)gtk_data_list_get_by_name(liste,"listeExclusion");

    /* recuperation de la position sur la combobox */
    position=gtk_combo_box_get_active(combobox);


    /* si la postion n est pas correct */
    if(position <0){
	/* affichage d un popup d erreur*/
	pDialog = gtk_message_dialog_new(GTK_WINDOW(user_data),
					 GTK_DIALOG_MODAL,
					 GTK_MESSAGE_INFO,
					 GTK_BUTTONS_OK,
					 "Sélectionnez un dossier\n");

	gtk_dialog_run(GTK_DIALOG(pDialog));
	gtk_widget_destroy(pDialog);
    }else{
	/* sinon affiche un popup de confirmation */
	dossier=gtk_combo_box_get_active_text(combobox);
	pDialog = gtk_message_dialog_new (GTK_WINDOW(user_data),
					    GTK_DIALOG_MODAL,
					    GTK_MESSAGE_QUESTION,
					    GTK_BUTTONS_YES_NO,
					    "Voulez-vous vraiment supprimer :%s\n de la liste des exclusions ?",dossier);


	gtk_dialog_run(GTK_DIALOG(pDialog));
	gtk_widget_destroy(pDialog);


	/* supprime un element de la combobox */
	gtk_combo_box_remove_text(combobox,position);
	gtk_entry_set_text (GTK_ENTRY (GTK_BIN (combobox)->child),"fichiers exclus");
	/* supprime l element du fichier de sauvegarde */
	supprimerListeExclusion(dossier);
    }
}

void on_btnPrecedentInstallUsb_clicked(GtkButton *button, gpointer user_data) /* event precedent qui ramene sur le choix support (USB) */
{
    GtkWidget *fenetreSupport;
    GtkWidget *usb;
    GtkWidget *fenetreUsb;

    fenetreUsb = (GtkWidget *)gtk_data_list_get_by_name(liste,"FenetreInsUsb");
    fenetreSupport = (GtkWidget *)gtk_data_list_get_by_name(liste,"FenetreChoixSupport");


    //suppression de tout les elements cree et les element sauvegarder dans la liste chainee
    //afin d'eviter les doublon

    int i=1;
	do{

		char buffer[32];
		sprintf(buffer,"btnUsb_%d",i);
		usb = (GtkWidget *)gtk_data_list_get_by_name(liste,buffer);
        gtk_data_list_remove_by_name(liste, buffer);
		i++;

	}while(usb !=NULL);



    gtk_data_list_remove_by_name(liste, "listeExclusion");
    gtk_data_list_remove_by_name(liste, "install");
    gtk_data_list_remove_by_name(liste, "FenetreInsUsb");

    //destruction des fenetre cree
    gtk_widget_destroy (fenetreUsb);
    gtk_widget_show (fenetreSupport);


}

void on_btnPrecedentChoixMode_clicked(GtkButton *button, gpointer user_data)  /* event precedent qui ramene sur le choix des disque rigide */
{

    GtkWidget *fenetreDur;
    GtkWidget *fenetreMode;

    fenetreDur = (GtkWidget *)gtk_data_list_get_by_name(liste,"FenetreChoixDur");
	fenetreMode = (GtkWidget *)gtk_data_list_get_by_name(liste,"FenetreChoixMode");

    //suppression de tout les elements cree et les element sauvegarder dans la liste chainee
    //afin d'eviter les doublon

    gtk_data_list_remove_by_name(liste,"FenetreChoixMode");
    gtk_data_list_remove_by_name(liste,"Auto");
    gtk_data_list_remove_by_name(liste,"Guide");
    gtk_data_list_remove_by_name(liste,"Manuelle");
    gtk_data_list_remove_by_name(liste,"ValidationMode");


    //Destruction des fenetres cree
    gtk_widget_destroy (fenetreMode);
    gtk_widget_show (fenetreDur);


}

void on_btnPrecedentDDurInst_cliked(GtkButton *button, gpointer user_data)    /* event precedent qui ramene sur le choix du support (DDur) */
{
    GtkWidget *fenetreDur,*fenetreSupport,*dur;

    fenetreDur = (GtkWidget *)gtk_data_list_get_by_name(liste,"FenetreChoixDur");
    fenetreSupport = (GtkWidget *)gtk_data_list_get_by_name(liste,"FenetreChoixSupport");

    //suppression de tout les elements cree et les element sauvegarder dans la liste chainee
    //afin d'eviter les doublon

	int i=1;
	do{


		char buffer[32];
		sprintf(buffer,"btnDur_%d",i);
		dur = (GtkWidget *)gtk_data_list_get_by_name(liste,buffer);
        gtk_data_list_remove_by_name(liste,buffer);
        i++;

	}while(dur !=NULL);
	//Destruction des fenetres cree
    gtk_data_list_remove_by_name(liste,"FenetreChoixDur");
    gtk_widget_destroy (fenetreDur);
    gtk_widget_show (fenetreSupport);

}

void on_btnQuitterSupport_cliked(GtkButton *button, gpointer user_data) /* event du bouton quitter support  */
{
    GtkWidget *pQuestion;
    GtkWidget *pWindow=
	(GtkWidget *)gtk_data_list_get_by_name(liste,"FenetreChoixSupport");


    /* creation de la boite de message */
    /* type : Question -> GTK_MESSAGE_QUESTION */
    /* boutons : 1 OUI, 1 NON -> GTK_BUTTONS_YES_NO */
    pQuestion = gtk_message_dialog_new (GTK_WINDOW(pWindow),
					GTK_DIALOG_MODAL,
					GTK_MESSAGE_QUESTION,
					GTK_BUTTONS_YES_NO,
					"Voulez-vous vraiment\nquitter ce programme?");

    /* affichage et attente d une reponse */
    switch(gtk_dialog_run(GTK_DIALOG(pQuestion))){
    case GTK_RESPONSE_YES:
	/* OUI -> on quitte l application */

	gtk_widget_destroy(pQuestion);
	gtk_main_quit();
	break;
    case GTK_RESPONSE_NO:
	/* NON -> on detruit la boite de message */
	gtk_widget_destroy(pQuestion);
	break;
    }
}

void on_btnQuitterMode_cliked(GtkButton *button, gpointer user_data)   /* event du bouton quitter Mode  */
{
    GtkWidget *pQuestion;
    GtkWidget *pWindow=
	(GtkWidget *)gtk_data_list_get_by_name(liste,"FenetreChoixMode");


    /* creation de la boite de message */
    /* type : Question -> GTK_MESSAGE_QUESTION */
    /* boutons : 1 OUI, 1 NON -> GTK_BUTTONS_YES_NO */
    pQuestion = gtk_message_dialog_new (GTK_WINDOW(pWindow),
					GTK_DIALOG_MODAL,
					GTK_MESSAGE_QUESTION,
					GTK_BUTTONS_YES_NO,
					"Voulez-vous vraiment\nquitter ce programme?");

    /* affichage et attente d une reponse */
    switch(gtk_dialog_run(GTK_DIALOG(pQuestion))){
    case GTK_RESPONSE_YES:
	/* OUI -> on quitte l application */

	gtk_widget_destroy(pQuestion);
	gtk_main_quit();
	break;
    case GTK_RESPONSE_NO:
	/* NON -> on detruit la boite de message */
	gtk_widget_destroy(pQuestion);
	break;
    }
}

void on_btnQuitterUsb_cliked(GtkButton *button, gpointer user_data)    /* event du bouton quitter Usb  */
{
    GtkWidget *pQuestion;
    GtkWidget *pWindow=
	(GtkWidget *)gtk_data_list_get_by_name(liste,"FenetreInsUsb");


    /* creation de la boite de message */
    /* type : Question -> GTK_MESSAGE_QUESTION */
    /* boutons : 1 OUI, 1 NON -> GTK_BUTTONS_YES_NO */
    pQuestion = gtk_message_dialog_new (GTK_WINDOW(pWindow),
					GTK_DIALOG_MODAL,
					GTK_MESSAGE_QUESTION,
					GTK_BUTTONS_YES_NO,
					"Voulez-vous vraiment\nquitter ce programme?");

    /* affichage et attente d une reponse */
    switch(gtk_dialog_run(GTK_DIALOG(pQuestion))){
    case GTK_RESPONSE_YES:
	/* OUI -> on quitte l application */

	gtk_widget_destroy(pQuestion);
	gtk_main_quit();
	break;
    case GTK_RESPONSE_NO:
	/* NON -> on detruit la boite de message */
	gtk_widget_destroy(pQuestion);
	break;
    }
}

void on_btnQuitterDDurInst_cliked(GtkButton *button, gpointer user_data) /* event du bouton quitter DDur  */
{
    GtkWidget *pQuestion;
    GtkWidget *pWindow=
	(GtkWidget *)gtk_data_list_get_by_name(liste,"FenetreChoixDur");


    /* creation de la boite de message */
    /* type : Question -> GTK_MESSAGE_QUESTION */
    /* boutons : 1 OUI, 1 NON -> GTK_BUTTONS_YES_NO */
    pQuestion = gtk_message_dialog_new (GTK_WINDOW(pWindow),
					GTK_DIALOG_MODAL,
					GTK_MESSAGE_QUESTION,
					GTK_BUTTONS_YES_NO,
					"Voulez-vous vraiment\nquitter ce programme?");

    /* affichage et attente d une reponse */
    switch(gtk_dialog_run(GTK_DIALOG(pQuestion))){
    case GTK_RESPONSE_YES:
	/* OUI -> on quitte l application */

	gtk_widget_destroy(pQuestion);
	gtk_main_quit();
	break;
    case GTK_RESPONSE_NO:
	/* NON -> on detruit la boite de message */
	gtk_widget_destroy(pQuestion);
	break;
    }
}

void on_btnExecuterInsUsb_cliked(GtkButton *button, gpointer user_data)  /* event du lancer du de l'install usb */
{
    GtkWidget *pQuestion,*pAbout;
    GtkWidget *pWindow=(GtkWidget *)gtk_data_list_get_by_name(liste,"Attention");
    GtkWidget *usb;
    gchar *intitule;

	int i=1;
	do{

		char buffer[32];
		sprintf(buffer,"btnUsb_%d",i);                                      //concatenation du nom de la variable afin de pouvoir lister tout les noms de disque exitant.
		usb = (GtkWidget *)gtk_data_list_get_by_name(liste,buffer);         //recuperation dans la liste chainee

		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(usb))){          //test si ce bouton est active

			intitule = gtk_button_get_label (GTK_BUTTON(usb));              //on recupere le nom du bouton
			printf(intitule);                                               //que l'on reinscrit dans le fichier
			FILE *pFile;                                                    //qui est lue par le script shell afin
			pFile = fopen ("/var/lib/installFrogLinux/disque_cle.list","w");                          //de laisser une trace du choix
			fprintf(pFile,intitule);
			fclose(pFile);
			break;
		}else i++;

	}while(usb !=NULL);

    /* creation de la boite de message */
    /* type : Question -> GTK_MESSAGE_QUESTION */
    /* boutons : 1 OUI, 1 NON -> GTK_BUTTONS_YES_NO */
    pQuestion = gtk_message_dialog_new (GTK_WINDOW(pWindow),
					GTK_DIALOG_MODAL,
					GTK_MESSAGE_WARNING,
					GTK_BUTTONS_YES_NO,
					"Toutes les données de ce disque seront supprimées\nVoulez-vous continuer?");

    /* affichage et attente d une reponse */
    switch(gtk_dialog_run(GTK_DIALOG(pQuestion))){

    case GTK_RESPONSE_YES:
        /* OUI -> demarre le script d'install */
        gtk_widget_destroy(pQuestion);

        GThread  *thread1;
        GError *error = NULL;
        if ((thread1=g_thread_create(lancerThreadUsb,NULL,FALSE, &error))==NULL){
        g_printerr ("Failed to create script thread: %s\n", error->message);
        }
	break;

    case GTK_RESPONSE_NO:
        /* NON -> on detruit la boite de message */
        gtk_widget_destroy(pQuestion);
        break;
    }
}

void on_btnAnnulerInstallUsb_clicked(GtkButton *button, gpointer user_data) /* event lors de l'annulation install usb */
{
    GtkWidget *pQuestion,*fenetreUsb,*fenetreProgIns,*bar,*about;
    fenetreUsb = (GtkWidget *)gtk_data_list_get_by_name(liste,"FenetreInsUsb");
    fenetreProgIns = (GtkWidget *)gtk_data_list_get_by_name(liste,"FenetreProgInsUsb");
    bar = (GtkWidget *)gtk_data_list_get_by_name(liste,"bar");
    about = (GtkWidget *)gtk_data_list_get_by_name(liste,"About");
    /* creation de la boite de message */
    /* type : Question -> GTK_MESSAGE_QUESTION */
    /* boutons : 1 OUI, 1 NON -> GTK_BUTTONS_YES_NO */

    pQuestion = gtk_message_dialog_new (GTK_WINDOW(user_data),
					GTK_DIALOG_MODAL,
					GTK_MESSAGE_QUESTION,
					GTK_BUTTONS_YES_NO,
					"Voulez-vous vraiment\n annuler les actions en cours ?");

    /* affichage et attente d une reponse */
    switch(gtk_dialog_run(GTK_DIALOG(pQuestion))){
    case GTK_RESPONSE_YES:
	/* OUI -> on quitte l application */

	lancerScript("/var/lib/installFrogLinux/kill_install.sh","USB");
	gtk_widget_destroy(pQuestion);
	gtk_widget_show(fenetreUsb);
	gtk_widget_hide(fenetreProgIns);


	break;
    case GTK_RESPONSE_NO:
	/* NON -> on detruit la boite de message */
	gtk_widget_destroy(pQuestion);
	break;
    }

}

void on_btnAnnulerInstallDur_clicked(GtkButton *button, gpointer user_data) /* event lors de l'annulation install dur */
{
    GtkWidget *pQuestion,*fenetreMode,*fenetreProgIns,*bar;
    fenetreMode = (GtkWidget *)gtk_data_list_get_by_name(liste,"FenetreChoixMode");
    fenetreProgIns = (GtkWidget *)gtk_data_list_get_by_name(liste,"FenetreProgInsDur");
    bar = (GtkWidget *)gtk_data_list_get_by_name(liste,"bar");
    /* creation de la boite de message */
    /* type : Question -> GTK_MESSAGE_QUESTION */
    /* boutons : 1 OUI, 1 NON -> GTK_BUTTONS_YES_NO */

    pQuestion = gtk_message_dialog_new (GTK_WINDOW(user_data),
					GTK_DIALOG_MODAL,
					GTK_MESSAGE_QUESTION,
					GTK_BUTTONS_YES_NO,
					"Voulez-vous vraiment\n annuler les actions en cours ?");

    /* affichage et attente d une reponse */
    switch(gtk_dialog_run(GTK_DIALOG(pQuestion))){
    case GTK_RESPONSE_YES:
	/* OUI -> on quitte l application */
	lancerScript("/var/lib/installFrogLinux/kill_install.sh","HD");
	gtk_widget_destroy(pQuestion);
	gtk_widget_show(fenetreMode);
	gtk_widget_hide(fenetreProgIns);

	break;
    case GTK_RESPONSE_NO:
	/* NON -> on detruit la boite de message */
	gtk_widget_destroy(pQuestion);
	break;
    }

}
void on_btnAnnulerInstallDur2_clicked(GtkButton *button, gpointer user_data) /* event lors de l'annulation install dur */
{
    GtkWidget *pQuestion,*fenetreMode,*fenetreProgIns,*bar,*fenetreAide;
    fenetreMode = (GtkWidget *)gtk_data_list_get_by_name(liste,"FenetreChoixMode");
    fenetreProgIns = (GtkWidget *)gtk_data_list_get_by_name(liste,"FenetreProgInsDur");
    fenetreAide = (GtkWidget *)gtk_data_list_get_by_name(liste,"FenetreAidePart");
    bar = (GtkWidget *)gtk_data_list_get_by_name(liste,"bar");
    /* creation de la boite de message */
    /* type : Question -> GTK_MESSAGE_QUESTION */
    /* boutons : 1 OUI, 1 NON -> GTK_BUTTONS_YES_NO */

    pQuestion = gtk_message_dialog_new (GTK_WINDOW(user_data),
					GTK_DIALOG_MODAL,
					GTK_MESSAGE_QUESTION,
					GTK_BUTTONS_YES_NO,
					"Voulez-vous vraiment\n annuler les actions en cours ?");

    /* affichage et attente d une reponse */
    switch(gtk_dialog_run(GTK_DIALOG(pQuestion))){
    case GTK_RESPONSE_YES:
	/* OUI -> on quitte l application */
	lancerScript("/var/lib/installFrogLinux/kill_install.sh","HD");

	gtk_data_list_remove_by_name(liste,"FenetreAidePart");
	gtk_widget_destroy (fenetreAide);
	gtk_widget_destroy(pQuestion);
	gtk_widget_show(fenetreMode);
	gtk_widget_hide(fenetreProgIns);

	break;
    case GTK_RESPONSE_NO:
	/* NON -> on detruit la boite de message */
	gtk_widget_destroy(pQuestion);
	break;
    }

}
void on_btnAnnulerAidePart_clicked(GtkButton *button, gpointer user_data) /* event lors de l'annulation de l'aide partition */
{
    GtkWidget *fenetreMode,*fenetreAide;
    fenetreMode = (GtkWidget *)gtk_data_list_get_by_name(liste,"FenetreChoixMode");
    fenetreAide = (GtkWidget *)gtk_data_list_get_by_name(liste,"FenetreAidePart");


    gtk_data_list_remove_by_name(liste,"FenetreAidePart");
    gtk_widget_destroy (fenetreAide);
    gtk_widget_show (fenetreMode);
    lancerScript("/var/lib/installFrogLinux/kill_gparted.sh",NULL);
}
