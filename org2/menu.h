#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <gtk/gtk.h>
#include <string.h>

extern GSList *liste;

void delete_event (GtkWidget *widget, GdkEvent *event, gpointer *data);
void InstallCleUsb(GtkWidget *widget);
void ModeInstall (GtkWidget *widget);
void SupportInstall (GtkWidget *widget);
void ProgressionInstallUsb(GtkWidget *widget);
void ProgressionInstallDur(GtkWidget *widget);
void ChoixDDurInstall (GtkWidget *widget);
void ProgressionRecherchePartition(GtkWidget *widget);
void AidePartition(GtkWidget *widget);
void clean(const char *buffer, FILE *fp);
