#ifndef _PANDLG_H_
#define _PANDLG_H_

#include <windows.h>

#include "config/config.h"

#ifdef __cplusplus
extern "C" {
#endif

struct __tagPANDLG {
  HWND hDialog;
  HINSTANCE hInst;
  CONFIG *config;
  HWND hParam[14];
};

PANDLG *PANDLG_new(HINSTANCE hInstance, CONFIG *config);
void PANDLG_delete(PANDLG *);
void PANDLG_open(PANDLG *, HWND hParent);
void PANDLG_close(PANDLG *);
void PANDLG_update_config(PANDLG *, int);
void PANDLG_update_page(PANDLG *);

#ifdef __cplusplus
}
#endif

#endif