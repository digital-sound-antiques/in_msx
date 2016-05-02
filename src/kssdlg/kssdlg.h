#ifndef _KSSDLG_H_
#define _KSSDLG_H_
#include <windows.h>
typedef struct tagKSSDLG
{
  int playlist_pos ;
  int filename ;
  int title ;
  int playtime ;
  int looptime ;
  int fadetime ;
  int loop_num ;
  int sync ;

  HWND dialog ;
} KSSDLG ;

KSSDLG *KSSDLG_new() ;
void KSSDLG_open(KSSDLG *KSSDLG, HINSTANCE hInst, HWND hParent) ;
void KSSDLG_close(KSSDLG *KSSDLG) ;
void KSSDLG_delete(KSSDLG *KSSDLG) ;
void KSSDLG_update(KSSDLG *KSSDLG, char *filename,int mode) ;

#endif