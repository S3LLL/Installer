/*****************************************************
 *          INSTALLATEUR FROGLINUX                   *
 *                                                   *
 *programmeur : POLLONGHINI Yohann                   *
 *date : 08/05/2008                                  *
 *                                                   *
 *                                                   *
 *****************************************************/

#include "listeChainee.h"
#include "fonction.h"
#include "menu.h"
#include "callbacks.h"

GSList *liste=NULL;


void delete_event (GtkWidget *widget, GdkEvent *event, gpointer *data){  /* event pour la fermeture d'une fenetre */
    //   lancerScript("/var/lib/installFrogLinux/kill_install.sh","HD");
    lancerScript("/var/lib/installFrogLinux/kill_install.sh","USB");
    gtk_main_quit ();
}

void InstallCleUsb(GtkWidget *widget){	/* Fenetre d'installation sur cle USB */

	/* Declaration des variables */
	GtkWidget *FenetreInsUsb;
	GtkWidget *boxglob;
	GtkWidget *boxNord;
	GtkWidget *boxSud;
	GtkWidget *quitter;
	GtkWidget *precedent;
	GtkWidget *install;
	GtkWidget *btnUsb_1;
	GtkWidget *separator;
	GtkWidget *labelErreur;
    	GtkWidget *frog;
    	GtkWidget *frame;
    	GtkWidget *frame2;
    	GtkWidget *frame3;
    	GtkWidget *text;
    	GtkWidget *boxMid;

	/* Caracteristique de la fenetre */
	FenetreInsUsb = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_signal_connect (GTK_OBJECT (FenetreInsUsb), "delete_event",
				G_CALLBACK(delete_event),NULL);
        gtk_window_set_resizable(GTK_WINDOW (FenetreInsUsb), FALSE);
	gtk_window_set_title(GTK_WINDOW (FenetreInsUsb),"Installation de FrogLinux ");
	gtk_window_set_position(GTK_WINDOW (FenetreInsUsb),GTK_WIN_POS_CENTER);
	gtk_window_resize (GTK_WINDOW (FenetreInsUsb),500,100);
	gtk_container_border_width (GTK_CONTAINER (FenetreInsUsb), 10);

	/* Creation et remplissage boxglobal */
	boxglob = gtk_vbox_new (FALSE,0);

	/************ Creation et remplissage de la box nord *************/

	boxNord = gtk_vbox_new (FALSE,0);

        frog = gtk_image_new_from_file ("/var/lib/installFrogLinux/frog.jpg");
        gtk_box_pack_start (GTK_BOX(boxNord),frog,FALSE,FALSE,0);

    frame = gtk_frame_new ("Attention");
    text = gtk_label_new ("\t- Toutes les données et partitions sur votre disque rigide ou clé USB seront effacées !\n\t- 8Go d'espace disponible au minimun.  ");
    gtk_container_add (GTK_CONTAINER (frame), text);
    gtk_box_pack_start (GTK_BOX (boxNord),frame,FALSE,FALSE,5);

	/****** Detection start ******/
    boxMid = gtk_vbox_new (FALSE,0);
    frame3 = gtk_frame_new("Veuillez sélectionner votre périphérique");
	//lancement du script pour la detection ds cle
	//         lancerScript("/var/lib/installFrogLinux/detect_periph_usb.sh",NULL);
	lancerScript("/var/lib/installFrogLinux/detect_disque_cle.sh",NULL);
	//ouverture du fichier en read
	//         FILE *fp = fopen ("/var/lib/installFrogLinux/periph_usb.list", "r");
	FILE *fp = fopen ("/var/lib/installFrogLinux/disque_cle.list", "r");
	//test pour savoir si le fichier est bien ouvert
   	if (fp != NULL)
   	{
		//variable
		int i=2;
		char ligne[128]; // contient la ligne lue
		GtkWidget *btnUsb_1;
		gchar *text;//pointeur sur ligne lue

		// lecture de la premiere ligne
		text = fgets (ligne, sizeof ligne,fp);

		/* Initialisation du bouton radio1 obligatoire
		afin de pouvoir cree un groupe avec les suivants*/

		if ( text !=NULL ){//une cle est detecter
            		clean (text,fp);
			btnUsb_1 = gtk_radio_button_new_with_label (NULL,text);
			gtk_box_pack_start (GTK_BOX(boxMid), btnUsb_1,FALSE,FALSE,5);
			gtk_data_list_add_by_name(liste,btnUsb_1,"btnUsb_1");
			gtk_container_add (GTK_CONTAINER (frame3), boxMid);

		}else{//aucune cle detecter

			GtkWidget *DetecMiss;

			DetecMiss = gtk_label_new ("Aucun disque ou clé detecté ");
	                gtk_container_add (GTK_CONTAINER (frame3), DetecMiss);

		}
		//lecture de la ligne suivante
		text = fgets (ligne, sizeof ligne,fp);
		// On cree des boutons jusqu'a ce fgets ai lu un chaine vide
		while(text !=NULL) {
        	   clean (text,fp);
		   char chaine[32];//nom de variable pour la liste d'objet
		   sprintf(chaine,"btnUsb_%d",i);//concatenation
		   //creation de bouton radio
		   GtkWidget *radio;
		   radio = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON(btnUsb_1),text);
		   gtk_box_pack_start (GTK_BOX(boxMid), radio, FALSE, FALSE, 5);
		   liste = gtk_data_list_add_by_name(liste, radio, chaine);//ajout a la liste
		   i++;
		   text = fgets (ligne, sizeof ligne,fp);
		}

		fclose (fp);//cloture du fichier lu
        }
        else
        {
      		labelErreur = gtk_label_new ("Impossible de détecter un disque ou une clé.");
   	}




	/****** Detection done ******/

	/****** liste d'exclusion ***/

	GtkWidget *listeExclusion;
	GtkWidget *boxInter;
	GtkWidget *dossier;
    	GtkWidget *supprimer;
	boxInter = gtk_hbox_new (FALSE,0);


	listeExclusion = gtk_combo_box_entry_new_text();
    	gtk_entry_set_text (GTK_ENTRY (GTK_BIN (listeExclusion)->child),"dossiers exclus");
    	liste = gtk_data_list_add_by_name(liste,listeExclusion,"listeExclusion");
	dossier = gtk_button_new_from_stock ("gtk-open");
	supprimer = gtk_button_new_from_stock ("gtk-delete");

	gtk_box_pack_start (GTK_BOX (boxInter), listeExclusion, TRUE,TRUE,0);
	gtk_box_pack_start (GTK_BOX (boxInter), dossier, FALSE,TRUE,10);
    	gtk_box_pack_start (GTK_BOX (boxInter), supprimer, FALSE,FALSE,0);

    	frame2 = gtk_frame_new ("Exclusion de dossiers");
    	gtk_container_add (GTK_CONTAINER (frame2), boxInter);

	/****** fin liste d'exclusion ***/
	gtk_box_pack_start (GTK_BOX(boxglob), boxNord,FALSE,FALSE,5);
	gtk_box_pack_start (GTK_BOX(boxglob), frame3,FALSE,FALSE,5);
	gtk_box_pack_start (GTK_BOX (boxglob), frame2, FALSE,FALSE,5);

	/****** Separateur ********/
	separator = gtk_hseparator_new ();
	gtk_widget_set_usize (separator, 400, 1);
    	gtk_box_pack_start (GTK_BOX (boxglob), separator,FALSE, FALSE, 0);

	/*********** Creation et remplissage de la box Sud   *************/
    	boxSud =gtk_hbox_new (FALSE,0);

	quitter = gtk_button_new_from_stock ("gtk-quit");
	precedent = gtk_button_new_from_stock ("gtk-go-back");
	install = gtk_button_new_from_stock ("gtk-execute");

	gtk_box_pack_start (GTK_BOX(boxSud),quitter,FALSE,FALSE,0);
	gtk_box_pack_end   (GTK_BOX(boxSud),install,FALSE,FALSE,0);
	gtk_box_pack_end   (GTK_BOX(boxSud),precedent,FALSE,FALSE,0);
    	gtk_box_pack_start (GTK_BOX(boxglob),boxSud,FALSE,FALSE,5);

	/*********** Signal des differents boutons **********/
    	g_signal_connect ((gpointer) quitter, "clicked",
			   G_CALLBACK (on_btnQuitterUsb_cliked),
			   NULL);

    	g_signal_connect ((gpointer) precedent, "clicked",
			   G_CALLBACK (on_btnPrecedentInstallUsb_clicked),
			   NULL);

	g_signal_connect ((gpointer) install, "clicked",
			   G_CALLBACK (on_btnExecuterInsUsb_cliked),
			   NULL);

	g_signal_connect ((gpointer) dossier, "clicked",
			   G_CALLBACK (on_btnDossier_clicked),
			   NULL);

    	g_signal_connect ((gpointer) supprimer, "clicked",
			   G_CALLBACK (on_btnSupprimer_clicked),
			   NULL);

	gtk_container_add (GTK_CONTAINER (FenetreInsUsb), boxglob);


	/*** Ajout a la liste chainee ***/

		liste = gtk_data_list_add_by_name(liste,install,"install");
		liste = gtk_data_list_add_by_name(liste,FenetreInsUsb,"FenetreInsUsb");


	/********* Affichage *********/
	gtk_widget_show_all (FenetreInsUsb);
}

void ProgressionInstallUsb(GtkWidget *widget){  /* Barre de progreesion USB */

   GtkWidget *pWindow;
   GtkWidget *pMainVBox;
   GtkWidget *pProgress;
   GtkWidget *pAnnuler;
   GtkWidget *message;
   GtkWidget *hbox1;



   pWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);

   gtk_window_set_title(GTK_WINDOW(pWindow), "Installation en cours");
   gtk_window_set_position(GTK_WINDOW (pWindow),GTK_WIN_POS_CENTER);
   gtk_window_set_default_size(GTK_WINDOW(pWindow), 320,100);
   gtk_container_set_border_width(GTK_CONTAINER(pWindow), 4);

   pMainVBox = gtk_vbox_new(TRUE, 0);
   gtk_container_add(GTK_CONTAINER(pWindow), pMainVBox);
   message = gtk_label_new ("L' installation peut prendre plusieurs minutes.");
   gtk_box_pack_start(GTK_BOX(pMainVBox), message, FALSE,FALSE,0);
   /* Creation de la barre de progression */
   pProgress = gtk_progress_bar_new();
   gtk_box_pack_start(GTK_BOX(pMainVBox), pProgress, TRUE, FALSE, 0);

   g_timeout_add(100, progressPuls2, NULL);

    /**************************/

    GtkWidget *separator = gtk_hseparator_new ();
    gtk_widget_set_usize (separator, 400, 5);
    gtk_box_pack_start (GTK_BOX (pMainVBox), separator,FALSE, FALSE, 0);

    /**************************/

   hbox1 = gtk_hbox_new(FALSE, 0);
   pAnnuler = gtk_button_new_from_stock ("gtk-cancel");
   gtk_box_pack_start(GTK_BOX(hbox1) , pAnnuler, FALSE, FALSE, 0);/** creation d'un bouton pAbout peut-etre **/

   gtk_box_pack_start(GTK_BOX(pMainVBox), hbox1, FALSE, FALSE, 0);

   gtk_widget_show_all(pWindow);

   gtk_signal_connect (GTK_OBJECT (pWindow), "delete_event",
                    G_CALLBACK(delete_event),NULL);

   g_signal_connect ((gpointer) pAnnuler, "clicked",
                    G_CALLBACK (on_btnAnnulerInstallUsb_clicked),
                    NULL);
   liste = gtk_data_list_add_by_name(liste,pWindow,"FenetreProgInsUsb");
   liste = gtk_data_list_add_by_name(liste,pProgress,"bar");

}




void ProgressionInstallDur(GtkWidget *widget){  /* Barre de progression Dur */

   GtkWidget *pWindow;
   GtkWidget *pMainVBox;
   GtkWidget *pProgress;
   GtkWidget *pAnnuler;
   GtkWidget *message;
   GtkWidget *hbox1;



   pWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);

   gtk_window_set_title(GTK_WINDOW(pWindow), "Installation en cours");
   gtk_window_set_position(GTK_WINDOW (pWindow),GTK_WIN_POS_CENTER);
   gtk_window_set_default_size(GTK_WINDOW(pWindow), 320,100);
   gtk_container_set_border_width(GTK_CONTAINER(pWindow), 4);

   pMainVBox = gtk_vbox_new(TRUE, 0);
   gtk_container_add(GTK_CONTAINER(pWindow), pMainVBox);
   message = gtk_label_new ("L' installation peut prendre plusieurs minutes.");
   gtk_box_pack_start(GTK_BOX(pMainVBox), message, FALSE,FALSE,0);
   /* Creation de la barre de progression */
   pProgress = gtk_progress_bar_new();
   gtk_box_pack_start(GTK_BOX(pMainVBox), pProgress, TRUE, FALSE, 0);

   g_timeout_add(100, progressPuls, NULL);

    /**************************/

    GtkWidget *separator = gtk_hseparator_new ();
    gtk_widget_set_usize (separator, 400, 5);
    gtk_box_pack_start (GTK_BOX (pMainVBox), separator,FALSE, FALSE, 0);

    /**************************/

   hbox1 = gtk_hbox_new(FALSE, 0);
   pAnnuler = gtk_button_new_from_stock ("gtk-cancel");
   gtk_box_pack_start(GTK_BOX(hbox1) , pAnnuler, FALSE, FALSE, 0);/** creation d'un bouton pAbout peut-etre **/

   gtk_box_pack_start(GTK_BOX(pMainVBox), hbox1, FALSE, FALSE, 0);

   gtk_widget_show_all(pWindow);

   gtk_signal_connect (GTK_OBJECT (pWindow), "delete_event",
                    G_CALLBACK(delete_event),NULL);

   g_signal_connect ((gpointer) pAnnuler, "clicked",
                    G_CALLBACK (on_btnAnnulerInstallDur_clicked),
                    NULL);
   liste = gtk_data_list_add_by_name(liste,pWindow,"FenetreProgInsDur");
   liste = gtk_data_list_add_by_name(liste,pProgress,"bar2");

}





void ProgressionInstallGuidee(GtkWidget *widget){ /* Barre de progreesion Guidee */

   GtkWidget *pWindow;
   GtkWidget *pMainVBox;
   GtkWidget *pProgress;
   GtkWidget *pAnnuler;
   GtkWidget *message;
   GtkWidget *hbox1;



   pWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);

   gtk_window_set_title(GTK_WINDOW(pWindow), "Installation en cours");
   gtk_window_set_position(GTK_WINDOW (pWindow),GTK_WIN_POS_CENTER);
   gtk_window_set_default_size(GTK_WINDOW(pWindow), 320,100);
   gtk_container_set_border_width(GTK_CONTAINER(pWindow), 4);

   pMainVBox = gtk_vbox_new(TRUE, 0);
   gtk_container_add(GTK_CONTAINER(pWindow), pMainVBox);
   message = gtk_label_new ("L' installation peut prendre quelques minutes.");
   gtk_box_pack_start(GTK_BOX(pMainVBox), message, FALSE,FALSE,0);
   /* Creation de la barre de progression */
   pProgress = gtk_progress_bar_new();
   gtk_box_pack_start(GTK_BOX(pMainVBox), pProgress, TRUE, FALSE, 0);

   g_timeout_add(100, progressPuls3, NULL);

    /**************************/

    GtkWidget *separator = gtk_hseparator_new ();
    gtk_widget_set_usize (separator, 400, 5);
    gtk_box_pack_start (GTK_BOX (pMainVBox), separator,FALSE, FALSE, 0);

    /**************************/

   hbox1 = gtk_hbox_new(FALSE, 0);
   pAnnuler = gtk_button_new_from_stock ("gtk-cancel");
   gtk_box_pack_start(GTK_BOX(hbox1) , pAnnuler, FALSE, FALSE, 0);/** creation d'un bouton pAbout peut-etre **/

   gtk_box_pack_start(GTK_BOX(pMainVBox), hbox1, FALSE, FALSE, 0);

   gtk_widget_show_all(pWindow);

   gtk_signal_connect (GTK_OBJECT (pWindow), "delete_event",
                    G_CALLBACK(delete_event),NULL);

   g_signal_connect ((gpointer) pAnnuler, "clicked",
                    G_CALLBACK (on_btnAnnulerInstallDur2_clicked),
                    NULL);

   liste = gtk_data_list_add_by_name(liste,pProgress,"bar3");

    liste = gtk_data_list_add_by_name(liste,pWindow,"FenetreProgInsGuide");
}

void ModeInstall (GtkWidget *widget){	/* Fenetre du choix du mode d'installation */

    /* Variable et style de texte */
	GtkWidget *FenetreChoixMode;
	GtkWidget *quitter;
	GtkWidget *boxglob;
	GtkWidget *boxNord;
	GtkWidget *boxMid;
	GtkWidget *boxSud;
	GtkWidget *welcome;
	GtkWidget *Auto;
	GtkWidget *Guide;
	GtkWidget *Manuel;
	GtkWidget *separator;
	GtkWidget *ValidationMode;
	GtkWidget *precedent;
    GtkWidget *frog;
    GtkWidget *Executer;
    GtkWidget *frame;


	/* Caracteristique de la fenetre */
	FenetreChoixMode = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_signal_connect (GTK_OBJECT (FenetreChoixMode), "delete_event",
				G_CALLBACK(delete_event),NULL);
        gtk_window_set_resizable(GTK_WINDOW (FenetreChoixMode), FALSE);
	gtk_window_set_title(GTK_WINDOW (FenetreChoixMode),"Installation de Froglinux ");
	gtk_window_set_position(GTK_WINDOW (FenetreChoixMode),GTK_WIN_POS_CENTER);
	gtk_window_resize (GTK_WINDOW (FenetreChoixMode),500,100);
	gtk_container_border_width (GTK_CONTAINER (FenetreChoixMode), 10);

	/* Creation et remplissage boxglobal */

	boxglob = gtk_vbox_new (FALSE,0);

	/************ Creation et remplissage de la box nord *************/

	boxNord = gtk_vbox_new (FALSE,0);
    frog = gtk_image_new_from_file ("/var/lib/installFrogLinux/frog.jpg");

	gtk_box_pack_start (GTK_BOX (boxNord),frog,FALSE,FALSE,0);
    gtk_box_pack_start (GTK_BOX (boxglob),boxNord,FALSE,FALSE,10);

    boxMid = gtk_vbox_new (FALSE,0);
    frame = gtk_frame_new("Choix du mode d' installation");

	Auto = gtk_radio_button_new_with_label (NULL," Automatique        (Débutant) /!\\ Attention toute les données seront perdues ");
	Guide = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (Auto)," Guidé                   (Intermédiaire) ");
	Manuel = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON (Auto), " Manuel                 (Expert)  ");



	gtk_box_pack_start (GTK_BOX (boxMid),Auto,FALSE, FALSE, 5);
	gtk_box_pack_start (GTK_BOX (boxMid),Guide, FALSE, FALSE, 5);
	gtk_box_pack_start (GTK_BOX (boxMid),Manuel, FALSE, FALSE, 5);


    gtk_container_add (GTK_CONTAINER (frame), boxMid);
	gtk_box_pack_start (GTK_BOX (boxglob),frame,FALSE,FALSE,5);

	/************************ Separateur ******************************/

	separator = gtk_hseparator_new ();
	gtk_widget_set_usize (separator, 400, 5);

	gtk_box_pack_start (GTK_BOX (boxglob), separator, FALSE, FALSE, 0);


	/**************** Creation et remplissage de la box Sud ***********/

	boxSud = gtk_hbox_new (FALSE,0);

	quitter = gtk_button_new_from_stock ("gtk-quit");
	ValidationMode = gtk_button_new_from_stock ("gtk-go-forward");

        precedent = gtk_button_new_from_stock ("gtk-go-back");
        gtk_box_pack_start (GTK_BOX (boxSud),quitter,FALSE,FALSE,0);
        gtk_box_pack_end (GTK_BOX (boxSud),ValidationMode,FALSE,FALSE,0);
	gtk_box_pack_end (GTK_BOX (boxSud),precedent,FALSE,FALSE,0);



    /* Evenement sur bouton */

    g_signal_connect ((gpointer) quitter, "clicked",
			   G_CALLBACK (on_btnQuitterMode_cliked),
			   NULL);

    g_signal_connect ((gpointer) precedent, "clicked",
			   G_CALLBACK (on_btnPrecedentChoixMode_clicked),
			   NULL);

    g_signal_connect ((gpointer) ValidationMode, "clicked",
			   G_CALLBACK (on_btnContinuerMode_cliked),
			   NULL);

    /* AJout box Sud a la box global */
	gtk_box_pack_start (GTK_BOX (boxglob),boxSud,TRUE,FALSE,10);

    /* Ajout de la box glob a la fenetre */
	gtk_container_add (GTK_CONTAINER (FenetreChoixMode), boxglob);

	/* Remplissage de la liste chainee */
		liste = gtk_data_list_add_by_name(liste,FenetreChoixMode,"FenetreChoixMode");
		liste = gtk_data_list_add_by_name(liste,Auto,"Auto");
		liste = gtk_data_list_add_by_name(liste,Guide,"Guide");
		liste = gtk_data_list_add_by_name(liste,Manuel,"Manuel");
		liste = gtk_data_list_add_by_name(liste,ValidationMode,"ValidationMode");

	/* Affichage */
	gtk_widget_show_all (FenetreChoixMode);


}

void AidePartition (GtkWidget *widget){  /* Fenetre aide pour Gparted */

    GtkWidget *FenetreAidePart;
    GtkWidget *boxglob;
    GtkWidget *boxtext;
    GtkWidget *boxquit;
    GtkWidget *annuler;
    GtkWidget *execute;
    GtkWidget *frame;
    GtkWidget *text;
    GtkWidget *separator;

    FenetreAidePart = gtk_window_new (GTK_WINDOW_TOPLEVEL);
           gtk_signal_connect (GTK_OBJECT (FenetreAidePart), "delete_event",
				G_CALLBACK(on_btnAnnulerAidePart_clicked),NULL);
    gtk_window_set_resizable(GTK_WINDOW (FenetreAidePart), FALSE);
    gtk_window_set_title(GTK_WINDOW (FenetreAidePart)," Aide Partition ");
    gtk_window_set_position(GTK_WINDOW (FenetreAidePart),GTK_WIN_POS_CENTER);

    gtk_window_set_keep_above(GTK_WINDOW (FenetreAidePart),TRUE);
    gtk_container_border_width (GTK_CONTAINER (FenetreAidePart), 10);

    boxglob = gtk_vbox_new (FALSE,0);

    boxtext = gtk_hbox_new (FALSE,0);
    gtk_box_pack_start (GTK_BOX (boxglob), boxtext, FALSE, FALSE, 0);

    frame = gtk_frame_new ("Conditions requises");
    text = gtk_label_new ("\n\t- Le disque doit contenir au maximun 2 partitions.  \n\t- 8Go et plus d'espace disponible. \n\t- Vous ne devez pas créer de partitions dans l'espace libre. ");
    gtk_container_add (GTK_CONTAINER (frame), text);
    gtk_box_pack_start (GTK_BOX (boxtext),frame,FALSE,FALSE,0);

    /* separateur */
    separator = gtk_hseparator_new ();
    gtk_widget_set_usize (separator, 200,10);
    gtk_box_pack_start (GTK_BOX (boxglob), separator, FALSE, FALSE, 0);

    boxquit = gtk_hbox_new (FALSE,0);
    gtk_box_pack_start (GTK_BOX (boxglob), boxquit, FALSE, FALSE, 0);

    annuler = gtk_button_new_from_stock ("gtk-cancel");
    execute = gtk_button_new_from_stock ("gtk-execute");

    gtk_box_pack_start (GTK_BOX (boxquit),annuler,FALSE,FALSE,0);
    gtk_box_pack_end (GTK_BOX (boxquit),execute,FALSE,FALSE,0);

    gtk_container_add (GTK_CONTAINER (FenetreAidePart), boxglob);

    gtk_widget_show_all (FenetreAidePart);

    g_signal_connect ((gpointer) annuler, "clicked",
			   G_CALLBACK (on_btnAnnulerAidePart_clicked),
			   NULL);

    g_signal_connect ((gpointer) execute, "clicked",
			   G_CALLBACK (on_btnContinuerAidePart_cliked),
			   NULL);

    liste = gtk_data_list_add_by_name(liste,FenetreAidePart,"FenetreAidePart");
}

void SupportInstall (GtkWidget *widget){	/* Fenetre choix du support d'installation */


	GtkWidget *FenetreChoixSupport;
	GtkWidget *BoxGlob;
	GtkWidget *BoxNord;
	GtkWidget *BoxMid;
	GtkWidget *BoxQuit;
	GtkWidget *separator;
	GtkWidget *DDur;
	GtkWidget *USB;
	GtkWidget *quitter;
	GtkWidget *welcome;
	GtkWidget *ValidationSupport;
    GtkWidget *frog;
    GtkWidget *frame;


	/***************** Creation de la fenetre avec ces caracteristique *****************/
	FenetreChoixSupport = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	gtk_signal_connect (GTK_OBJECT (FenetreChoixSupport), "delete_event",
				G_CALLBACK(delete_event),NULL);

        gtk_window_set_resizable(GTK_WINDOW (FenetreChoixSupport), FALSE);
	gtk_window_set_title(GTK_WINDOW (FenetreChoixSupport),"Installation de Froglinux ");
	gtk_window_resize(GTK_WINDOW (FenetreChoixSupport),500,100);
	gtk_window_set_position(GTK_WINDOW (FenetreChoixSupport),GTK_WIN_POS_CENTER);
	gtk_container_border_width (GTK_CONTAINER (FenetreChoixSupport), 10);

	/****************** creation d'une box Vertical *******************************************/

	BoxGlob = gtk_vbox_new(FALSE,0);

	/* Creation Box Nord */

	BoxNord = gtk_vbox_new(FALSE,0);
	gtk_box_pack_start (GTK_BOX (BoxGlob),BoxNord,TRUE,FALSE,0);


    /*image */

    frog = gtk_image_new_from_file ("/var/lib/installFrogLinux/frog.jpg");
    gtk_box_pack_start(GTK_BOX(BoxNord), frog,FALSE,FALSE,0);

	/* Label de bienvenue */
	welcome = gtk_label_new("Merci d'utiliser l'installateur de FrogLinux. Vous êtes sur le point d'installer FrogLinux\nun système d'exploitation simple et flexible, conçu et adapté pour vous.");

	gtk_box_pack_start (GTK_BOX (BoxNord),welcome,FALSE,FALSE,10);


    	frame = gtk_frame_new ("Veuillez sélectionner le support");
    	BoxMid = gtk_vbox_new(FALSE,0);
	/* Creation BoutonRadio1 Disque Dur */	/* Choix par Default */
	DDur = gtk_radio_button_new_with_label (NULL," Disque rigide interne ");
    	gtk_box_pack_start (GTK_BOX (BoxMid),DDur,TRUE,FALSE,10);
	/* Creation BoutonRadio2 USB */
	USB = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (DDur)," Disque ou clé USB ");
    	gtk_box_pack_start (GTK_BOX (BoxMid),USB,TRUE,FALSE,10);

    	gtk_container_add (GTK_CONTAINER (frame), BoxMid);
	gtk_box_pack_start (GTK_BOX (BoxGlob), frame, FALSE, FALSE, 5);
	/* separator */
	separator = gtk_hseparator_new ();
	gtk_widget_set_usize (separator, 400, 5);

	gtk_box_pack_start (GTK_BOX (BoxGlob), separator, FALSE, FALSE, 5);

	/************** Creation BoxQuit ***************************/
	BoxQuit=gtk_hbox_new(FALSE,0);
	gtk_box_pack_start (GTK_BOX (BoxGlob),BoxQuit, TRUE, FALSE,5);

	/* Creation button*/

	quitter = gtk_button_new_from_stock ("gtk-quit");
	gtk_box_pack_start (GTK_BOX (BoxQuit),quitter,FALSE,FALSE,0);


	/* Creation button validation */
	ValidationSupport = gtk_button_new_from_stock ("gtk-go-forward");
	gtk_box_pack_end (GTK_BOX (BoxQuit),ValidationSupport,FALSE,FALSE,0);

	/* Creation des signaux */

	g_signal_connect ((gpointer) quitter, "clicked",
			   G_CALLBACK (on_btnQuitterSupport_cliked),
			   NULL);


	g_signal_connect ((gpointer) ValidationSupport, "clicked",
			   G_CALLBACK (on_btnContinuerSupport_clicked),
			   NULL);


	/* All object dans la liste chainee */

		liste = gtk_data_list_add_by_name(liste,FenetreChoixSupport,"FenetreChoixSupport");
		liste = gtk_data_list_add_by_name(liste,BoxGlob,"BoxGlob");
		liste = gtk_data_list_add_by_name(liste,BoxNord,"BoxNord");
		liste = gtk_data_list_add_by_name(liste,BoxQuit,"BoxQuit");
		liste = gtk_data_list_add_by_name(liste,separator,"separator");
		liste = gtk_data_list_add_by_name(liste,DDur,"DDur");
		liste = gtk_data_list_add_by_name(liste,USB,"USB");
		liste = gtk_data_list_add_by_name(liste,quitter,"quitter");
		liste = gtk_data_list_add_by_name(liste,welcome,"welcome");
		liste = gtk_data_list_add_by_name(liste,ValidationSupport,"ValidationSupport");
        	liste = gtk_data_list_add_by_name(liste,frog,"frog");



	gtk_container_add (GTK_CONTAINER (FenetreChoixSupport), BoxGlob);

	/* affichage */

	gtk_widget_show_all (FenetreChoixSupport);

}

void ChoixDDurInstall (GtkWidget *widget){   /* Fenetre choix du disque dur d'installation */

    GtkWidget *FenetreChoixDur;
    GtkWidget *welcome;
    GtkWidget *btnDur_1;
    GtkWidget *boxglob;
    GtkWidget *boxnord;
    GtkWidget *boxmid;
    GtkWidget *boxsud;
    GtkWidget *frog;
    GtkWidget *quitter;
    GtkWidget *precedent;
    GtkWidget *suivant;
    GtkWidget *frame;

    /***************** Creation de la fenetre avec ces caracteristique *****************/
	FenetreChoixDur = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	gtk_signal_connect (GTK_OBJECT (FenetreChoixDur), "delete_event",
				G_CALLBACK(delete_event),NULL);

        gtk_window_set_resizable(GTK_WINDOW (FenetreChoixDur), FALSE);
	gtk_window_set_title(GTK_WINDOW (FenetreChoixDur),"Installation de Froglinux");
	gtk_window_resize(GTK_WINDOW (FenetreChoixDur),500,100);
	gtk_window_set_position(GTK_WINDOW (FenetreChoixDur),GTK_WIN_POS_CENTER);
	gtk_container_border_width (GTK_CONTAINER (FenetreChoixDur), 10);

	/***************** Remplissage de la box global *************************************/

    boxglob = gtk_vbox_new (FALSE,0);

    /** Box Nord **/

    boxnord = gtk_vbox_new (FALSE,0);
    gtk_box_pack_start (GTK_BOX (boxglob), boxnord, TRUE , FALSE, 0);

    frog = gtk_image_new_from_file ("/var/lib/installFrogLinux/frog.jpg");
    gtk_box_pack_start (GTK_BOX (boxnord), frog, FALSE,FALSE,0);

    boxmid = gtk_vbox_new (FALSE,0);
    frame = gtk_frame_new ("Veuillez sélectionner votre disque ");

    gtk_box_pack_start (GTK_BOX (boxnord), frame, FALSE, FALSE,10);

        /** Detection des partitions **/

	lancerScript("/var/lib/installFrogLinux/detect_disque_cle.sh",NULL);

   //ouverture du fichier en read
	FILE *fp = fopen ("/var/lib/installFrogLinux/disque_cle.list", "r");
	//test pour savoir si le fichier est bien ouvert
   	if (fp != NULL)
   	{
		//variable
		int i=2;
		char ligne[128]; // contient la ligne lue

		gchar *text;//pointeur sur ligne lue

		// lecture de la premiere ligne
		text = fgets (ligne, sizeof ligne,fp);

		/* Initialisation du bouton radio1 obligatoire
		afin de pouvoir cree un groupe avec les suivants*/

		if ( text !=NULL ){//une cle est detecter
            clean (text,fp);
			btnDur_1 = gtk_radio_button_new_with_label (NULL,text);

			gtk_box_pack_start (GTK_BOX(boxmid), btnDur_1,FALSE,FALSE,10);
			gtk_data_list_add_by_name(liste,btnDur_1,"btnDur_1");

		}else{//aucune cle detecter

			GtkWidget *DetecMiss;

			DetecMiss = gtk_label_new ("Auncun disque rigide interne disponible ");
			gtk_box_pack_start (GTK_BOX(boxmid),DetecMiss,FALSE,FALSE,10);
            gtk_container_add (GTK_CONTAINER (frame), DetecMiss);


		}
		//lecture de la ligne suivante
		text = fgets (ligne, sizeof ligne,fp);

		// On cree des boutons jusqu'a ce fgets ai lu un chaine vide
		while(text !=NULL) {
        clean (text,fp);
		char chaine[32];//nom de variable pour la liste d'objet
		sprintf(chaine,"btnDur_%d",i);//concatenation
		//creation de bouton radio
		GtkWidget *radio;
		radio = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON(btnDur_1),text);
		gtk_box_pack_start (GTK_BOX(boxmid), radio, FALSE, FALSE, 10);
		liste = gtk_data_list_add_by_name(liste, radio, chaine);//ajout a la liste
		i++;
		text = fgets (ligne, sizeof ligne,fp);

		}

		fclose (fp);//cloture du fichier lu

   	}
    gtk_container_add (GTK_CONTAINER (frame), boxmid);
        /** Detection done **/
    //barre de separation
    GtkWidget *separator;
    separator = gtk_hseparator_new ();
	gtk_widget_set_usize (separator, 400, 5);

	gtk_box_pack_start (GTK_BOX (boxglob), separator, FALSE, FALSE, 0);

        /** Box Sud **/
    boxsud = gtk_hbox_new (FALSE,0);
    gtk_box_pack_start (GTK_BOX(boxglob), boxsud, TRUE, FALSE,0);

    quitter = gtk_button_new_from_stock ("gtk-quit");
    precedent = gtk_button_new_from_stock ("gtk-go-back");
    suivant = gtk_button_new_from_stock ("gtk-go-forward");

    gtk_box_pack_start (GTK_BOX(boxsud),quitter,FALSE,FALSE,0);
    gtk_box_pack_end   (GTK_BOX(boxsud),suivant,FALSE,FALSE,0);
    gtk_box_pack_end   (GTK_BOX(boxsud),precedent,FALSE,FALSE,0);

    /* signal */

    g_signal_connect ((gpointer) quitter, "clicked",
			   G_CALLBACK (on_btnQuitterDDurInst_cliked),
			   NULL);


    g_signal_connect ((gpointer) suivant, "clicked",
			   G_CALLBACK (on_btnContinuerDDurInst_cliked),
			   NULL);

    g_signal_connect ((gpointer) precedent, "clicked",
			   G_CALLBACK (on_btnPrecedentDDurInst_cliked),
			   NULL);

    liste = gtk_data_list_add_by_name(liste,FenetreChoixDur,"FenetreChoixDur");

    gtk_container_add (GTK_CONTAINER (FenetreChoixDur), boxglob);

    gtk_widget_show_all (FenetreChoixDur);
}

int main (int argc, char *argv[]){        /* Main */


	/* init */
	gtk_init (&argc, &argv);
	if (!g_thread_supported())
    	g_thread_init (NULL);
        //g_thread_init(NULL);
        gdk_threads_init();
	/* Declaration variable */
	GtkWidget *window,*window2,*window3,*window4,*windowTmp2,*windowTmp,*windowTmp3;

	/* Debut du programme */
	ProgressionInstallUsb(window2);
	windowTmp = (GtkWidget *)gtk_data_list_get_by_name(liste,"FenetreProgInsUsb");
	gtk_widget_hide(windowTmp);

	ProgressionInstallDur(window3);
	windowTmp2 = (GtkWidget *)gtk_data_list_get_by_name(liste,"FenetreProgInsDur");
	gtk_widget_hide(windowTmp2);

        ProgressionInstallGuidee(window4);
	windowTmp3 = (GtkWidget *)gtk_data_list_get_by_name(liste,"FenetreProgInsGuide");
	gtk_widget_hide(windowTmp3);

	SupportInstall(window);

        gdk_threads_enter();
	gtk_main();
        gdk_threads_leave();
	return EXIT_SUCCESS;
}
