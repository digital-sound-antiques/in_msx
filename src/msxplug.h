#ifndef _MSXPLUG_H_
#define _MSXPLUG_H_
#include "kssplay.h"

void MSXPLUG_init(void) ;
void MSXPLUG_quit(void) ;
void MSXPLUG_config(HWND hParent) ;
void MSXPLUG_about(HWND hParent) ;
void MSXPLUG_getfileinfo(char *file, char *title, int *length_in_ms) ;
int MSXPLUG_infoDlg(char *file, HWND hParent) ;
int MSXPLUG_isourfile(char *fn) ;
	
int MSXPLUG_play(char *fn) ;
void MSXPLUG_pause() ;
void MSXPLUG_unpause() ;
int MSXPLUG_ispaused() ;
void MSXPLUG_stop() ;
const char *MSXPLUG_getMGStext() ;
int MSXPLUG_getDecodeTime();
int MSXPLUG_getOutputTime();
	
int MSXPLUG_getlength() ;
int MSXPLUG_getoutputtime() ;
void MSXPLUG_setoutputtime(int time_in_ms) ;

void MSXPLUG_setvolume(int volume) ;
void MSXPLUG_setpan(int pan) ;

void MSXPLUG_eq_set(int on, char data[10], int preamp) ;

void MSXPLUG_optdlg(HWND hwndParent) ;
void MSXPLUG_edit2413(HWND hwndParent) ;
int MSXPLUG_plsdlg(HWND hwndParent) ;
void MSXPLUG_maskdlg(HWND hwndParent);

void MSXPLUG_set_song(int pos) ;
int MSXPLUG_get_song(void) ;

void MSXPLUG_play_song(int pos);

void MSXPLUG_play2(int pos, int arg) ;

#endif