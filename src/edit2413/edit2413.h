#ifndef _EDIT2413_H_
#define _EDIT2413_H_
#include <windows.h>
#include "emu2413/emu2413.h"

enum { TL=0, FB, EG, ML, AR, DR, SL, RR, KR, KL, AM, PM, WF, ENDPARAM } ;

typedef struct tagEDIT2413
{
  HWND hMain ;
  HWND hParam[16] ;
  HICON hLogo ;

  int cursel ;
  int maiden ;

  OPLL *opll ;
  OPLL_PATCH patch[19*2] ;

} EDIT2413 ;

EDIT2413 *EDIT2413_new(void) ;
void EDIT2413_open(EDIT2413 *edit2413, HWND hwndParent, HINSTANCE hInst) ;
void EDIT2413_close(EDIT2413 *edit2413) ;
void EDIT2413_delete(EDIT2413 *edit2413) ;
void EDIT2413_set_target(EDIT2413 *edit2413, OPLL *opll) ;
void EDIT2413_unset_target(EDIT2413 *edit2413) ;

void EDIT2413_set_patch(EDIT2413 *edit2413, int num, OPLL_PATCH *patch) ;
void EDIT2413_get_patch(EDIT2413 *edit2413, int num, OPLL_PATCH *patch) ;

void EDIT2413_apply(EDIT2413 *edit2413) ;
void EDIT2413_sync(EDIT2413 *edit2413) ;

void EDIT2413_load(EDIT2413 *edit2413, char *filename) ;
void EDIT2413_save(EDIT2413 *edit2413, char *filename) ;
#endif

