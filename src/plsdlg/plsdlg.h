#ifndef _PLSDLG_H_
#define _PLSDLG_H_

#include "pls/ppls.h"

typedef struct tagPLSDLG
{
  HWND hWinamp ; /* Parent window = winamp main window */
  HWND hDialog ; /* Window handle to Information dialog */
  PLSITEM *plsitem ;
  int pos ; /* position */
  int sync ;
  int multi ;
  HICON hPrev, hNext, hPlay, hStop, hLogo ;
} PLSDLG ;

PLSDLG *PLSDLG_new(void) ;
void PLSDLG_delete(PLSDLG *plsdlg) ;
void PLSDLG_open(PLSDLG *plsdlg,HINSTANCE hInst, HWND hWinamp) ;
void PLSDLG_close(PLSDLG *plsdlg) ;
void PLSDLG_set_item(PLSDLG *plsdlg, int pos) ;
void PLSDLG_update(PLSDLG *plsdlg) ;
#endif

