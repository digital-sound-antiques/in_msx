#ifndef _MASKDLG_H_
#define _MASKDLG_H_
#include <windows.h>
typedef struct tagMASKDLG
{
  CONFIG *cfg;
  HWND dialog ;
} MASKDLG ;

MASKDLG *MASKDLG_new(CONFIG *cfg) ;
void MASKDLG_open(MASKDLG *, HWND, HINSTANCE);
void MASKDLG_close(MASKDLG *) ;
void MASKDLG_delete(MASKDLG *) ;

#endif