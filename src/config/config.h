#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <windows.h>
#include <crtdbg.h>
#include "rc/resource.h"
#include "in_msx.h"
#include "kssplay.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {PSG_DISABLE,PSG_AUTO,PSG_AY,PSG_YM} ;
enum {SCC_DISABLE,SCC_AUTO,SCC_NORMAL,SCC_PLUS} ;

#define HASH_KEY_NUM 16
#define HASH_KEY(x) ((x)&(HASH_KEY_NUM-1))

typedef struct __tagCONFIG_PARAM
{
  __int32 type;
  __int32 size; /* 0: non-array, !0: array */
  __int32 format;
  __int32 max;
  __int32 min;
  char *name;
  void *data;
  struct __tagCONFIG_PARAM *next;

} CONFIG_PARAM;

typedef struct __tagPANDLG PANDLG;

typedef struct
{
  HWND hWinamp ;
  HWND dialog ;
  HWND status ;
  DLGPROC dlgProc ;
  HICON hYM2413 ;
  
  PANDLG *pandlg;

  CONFIG_PARAM *param_list[HASH_KEY_NUM];

  char file_name[_MAX_PATH+16];
  char section_name[32] ;
  char extensions[256] ;
  char ill_path[_MAX_PATH] ;

  int mgsdrv_loaded ;
  int mpk106_loaded ;
  int mpk103_loaded ;
  int opxdrv_loaded ;
  int kinrou_loaded ;
  int mbmdrv_loaded ;
  int fmbios_loaded ;

  int vol[EDSC_MAX];     /* volume saved */
  int pan[EDSC_MAX];     /* pan saved */
  int curpan[EDSC_MAX] ; /* current pan */
  int plspan[EDSC_MAX] ; /* current playlist pan */
  int curvol[EDSC_MAX] ; /* current volume */
  int plsvol[EDSC_MAX] ; /* current playlist volume */
  int mute[EDSC_MAX] ;
  int mask[EDSC_MAX] ;

  int volume_update ;
  int mask_update;
  int filter_update ;
  int pan_update;
  int ch_pan_update;
  int mode_update ;
  int quality_update ;
  int refresh_mixer ;
  int need_to_restart ;
  int iswinamp;

  CRITICAL_SECTION cso;

} CONFIG ;

CONFIG *CONFIG_new(char *path, char *section) ;
void CONFIG_load(CONFIG *config) ;
void CONFIG_save(CONFIG *config) ;
void CONFIG_dialog_show(CONFIG *config, HWND hwndParent, HINSTANCE hDllInstance, int page) ;
void CONFIG_dialog_show2(CONFIG *config, HWND hwndParent, HINSTANCE hDllInstance, int page) ;
void CONFIG_dialog_end(CONFIG *config) ;
void CONFIG_delete(CONFIG *config) ;
__int32 CONFIG_get_int(CONFIG *config, char *name);
char *CONFIG_get_str(CONFIG *config, char *name);
void CONFIG_set_int(CONFIG *config, char *name, __int32 v);
void CONFIG_set_str(CONFIG *config, char *name, char *v);

void CONFIG_apply_update(CONFIG *cfg, KSSPLAY *kssplay);
void CONFIG_force_update(CONFIG *cfg, KSSPLAY *kssplay);

/* GUI MAPPER */
void CONFIG_wr_checkbtn(CONFIG *config, HWND hDlg, char *name, int idc);
void CONFIG_rd_checkbtn(CONFIG *config, HWND hDlg, char *name, int idc);
void CONFIG_wr_textbox(CONFIG *config, HWND hDlg, char *name, int idc);
void CONFIG_rd_textbox(CONFIG *config, HWND hDlg, char *name, int idc);
void CONFIG_wr_radiobtn(CONFIG *config, HWND hDlg, char *name,int first_idc, int last_idc, __int32 offset);
void CONFIG_rd_radiobtn(CONFIG *config, HWND hDlg, char *name, int first_idc, int last_idc, __int32 offset);
void CONFIG_wr_slider(CONFIG *config, HWND hDlg, char *name, int idc, __int32 (*adjust_func)(__int32 value));
void CONFIG_rd_slider(CONFIG *config, HWND hDlg, char *name, int idc, __int32 (*adjust_func)(__int32 value));

HPROPSHEETPAGE CreateConfigPage1(HINSTANCE hInst, CONFIG *config) ;
HPROPSHEETPAGE CreateConfigPage2(HINSTANCE hInst, CONFIG *config) ;
HPROPSHEETPAGE CreateConfigPage3(HINSTANCE hInst, CONFIG *config) ;
HPROPSHEETPAGE CreateConfigPage4(HINSTANCE hInst, CONFIG *config) ;
HPROPSHEETPAGE CreateConfigPage5(HINSTANCE hInst, CONFIG *config) ;
HPROPSHEETPAGE CreateConfigPage6(HINSTANCE hInst, CONFIG *config) ;
HPROPSHEETPAGE CreateConfigPage7(HINSTANCE hInst, CONFIG *config) ;
HPROPSHEETPAGE CreateConfigPage8(HINSTANCE hInst, CONFIG *config) ;
HPROPSHEETPAGE CreateConfigPageA(HINSTANCE hInst, CONFIG *config) ;

#ifdef __cplusplus
}
#endif

#endif