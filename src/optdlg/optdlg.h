#ifndef _OPTDLG_H_
#define _OPTDLG_H_
#include <windows.h>

#define MAX_BUFFERED 100 /* frames */
#define FRAME_PERIOD 100

typedef struct tagOPTDLG
{
  HFONT hFont;
  HDC hDC;
  HBITMAP hBitmap;
  HWND dialog ;
  char lyrics_buf[MAX_BUFFERED][81];
  int lyrics_buf_time[MAX_BUFFERED];
  int lyrics_pos;
  int delay_in_ms;
} OPTDLG ;

OPTDLG *OPTDLG_new() ;
void OPTDLG_open(OPTDLG *, HWND, HINSTANCE);
void OPTDLG_close(OPTDLG *) ;
void OPTDLG_delete(OPTDLG *) ;

#endif
