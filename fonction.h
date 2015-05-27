#include "menu.h"
 
int lancerScriptInt(char* script, char *args);
void lancerScript(char* script, char *args);
void lancerProg(char* prog, char *args);
gboolean progressPuls();
gboolean progressPuls2();
gboolean progressPuls3();
gboolean existeDejaDansListeExclusion(char *var);
void *lancerThreadUsb(void *args);
void *lancerThreadDur(void *args);
void *lancerThreadGParted(void *args);
void *lancerThreadGuide(void *args);
