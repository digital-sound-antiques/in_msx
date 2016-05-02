#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include "config.h"

/* Dialog Items */
static UINT SLIDER_ID[EDSC_MAX] = { IDC_PSGVOL, IDC_SCCVOL, IDC_OPLLVOL, IDC_OPLVOL } ;
static UINT DBTEXT_ID[EDSC_MAX] = { IDC_PSGDB, IDC_SCCDB, IDC_OPLLDB, IDC_OPLDB } ;
static UINT MUTEBTN_ID[EDSC_MAX] = { IDC_PSGMUTE, IDC_SCCMUTE, IDC_OPLLMUTE, IDC_OPLMUTE } ;

static void set_db_text(HWND hWnd, int val)
{
  char buf[16] ;
  sprintf(buf,"%3.2fdB",(KSSPLAY_VOL_STEP*val));
  SetWindowText(hWnd, buf) ;
}

static __int32 adjust_func1(__int32 value)
{
  return 64 - value ;
}

static __int32 adjust_pan_wr(__int32 value)
{
  return 128-value;
}

static __int32 adjust_pan_rd(__int32 value)
{
  return 128-value;
}

static struct 
{
  int id;
  char *name;
}
PAN_SLIDER[] = 
{
  {IDC_PSG_PAN,"PSG_PAN_MIXER"},
  {IDC_SCC_PAN,"SCC_PAN_MIXER"},
  {IDC_OPL_PAN,"OPL_PAN_MIXER"},
  {IDC_OPLL_PAN,"OPLL_PAN_MIXER"}
};

static void create_pan_slider(HWND hDlg,CONFIG *config)
{
}

static void update_page(HWND hDlg, CONFIG *config)
{
  HWND hWnd ;
  int i, master_volume = CONFIG_get_int(config,"MASTER_VOL");
 
  hWnd = GetDlgItem(hDlg, IDC_MASTERVOL);
  SendMessage(hWnd, TBM_SETRANGE, TRUE, MAKELONG(0,128));
  SendMessage(hWnd, TBM_SETTICFREQ, 16, 0) ;
  CONFIG_wr_slider(config, hDlg, "MASTER_VOL", IDC_MASTERVOL, adjust_func1);
  CONFIG_wr_textbox(config, hDlg, "MASTER_VOL", IDC_MASTERDB);

  for(i=0;i<EDSC_MAX;i++)
  {
    hWnd = GetDlgItem(hDlg, PAN_SLIDER[i].id);
    SendMessage(hWnd, TBM_SETRANGE, TRUE, MAKELONG(0,256));
    SendMessage(hWnd, TBM_SETPAGESIZE, 0, (long)16);
    SendMessage(hWnd, TBM_SETTIC, 0, (long)128);
    CONFIG_wr_slider(config, hDlg, PAN_SLIDER[i].name, PAN_SLIDER[i].id, adjust_pan_wr);
  }

  for(i=0;i<EDSC_MAX;i++)
  {
    hWnd = GetDlgItem(hDlg, SLIDER_ID[i]) ;
    SendMessage(hWnd, TBM_SETRANGE, TRUE, MAKELONG(0,256)) ;
    SendMessage(hWnd, TBM_SETTICFREQ, 32, 0) ;
    SendMessage(hWnd, TBM_SETPOS, TRUE, (long)128-config->curvol[i]) ;
    set_db_text(GetDlgItem(hDlg,DBTEXT_ID[i]),config->curvol[i]) ;

    if(config->mute[i])
      CheckDlgButton(hDlg, MUTEBTN_ID[i], BST_CHECKED);
    else
      CheckDlgButton(hDlg, MUTEBTN_ID[i], BST_UNCHECKED);
  }

  CONFIG_wr_checkbtn(config, hDlg, "OVERWRITE_VOLUME", IDC_OVERWRITE);
  CONFIG_wr_checkbtn(config, hDlg, "AUTOSAVE_VOLUME", IDC_AUTO_SAVE);

  if(CONFIG_get_int(config,"AUTOSAVE_VOLUME"))
  {
    EnableWindow(GetDlgItem(hDlg, IDC_SAVE), FALSE);
    EnableWindow(GetDlgItem(hDlg, IDC_RESTORE), FALSE);
  }
  else
  {
    EnableWindow(GetDlgItem(hDlg, IDC_SAVE), TRUE);
    EnableWindow(GetDlgItem(hDlg, IDC_RESTORE), TRUE);
  }

}

static BOOL bn_clicked_event(HWND hDlg, UINT uIdc, CONFIG *config)
{
  int i ;

  if(uIdc==IDC_SAVE)
  {
    for(i=0;i<EDSC_MAX;i++) config->vol[i] = config->curvol[i] ;
    config->refresh_mixer = 1 ;
    return TRUE ;
  }

  if(uIdc==IDC_RESTORE)
  {
    for(i=0;i<EDSC_MAX;i++) config->curvol[i] = config->vol[i] ;
    config->refresh_mixer = 1 ;
    config->volume_update = 1 ;
    return TRUE ;
  }

  if(uIdc==IDC_AUTO_SAVE)
  {
    CONFIG_rd_checkbtn(config,hDlg,"AUTOSAVE_VOLUME",IDC_AUTO_SAVE);
    update_page(hDlg,config) ;
    return TRUE ;
  }
  if(uIdc==IDC_OVERWRITE)
  {
    CONFIG_rd_checkbtn(config,hDlg,"OVERWRITE_VOLUME",IDC_OVERWRITE);
    if(IsDlgButtonChecked(hDlg,uIdc)==BST_CHECKED)
    {
      for(i=0;i<EDSC_MAX;i++) config->curvol[i] = config->vol[i] ;
    }
    else
    {
      for(i=0;i<EDSC_MAX;i++) config->curvol[i] = config->plsvol[i] ;
    }
    config->refresh_mixer = 1 ;
    config->volume_update = 1 ;
    return TRUE ;
  }

  for(i=0;i<EDSC_MAX;i++)
    if(MUTEBTN_ID[i]==uIdc) break ;

  if(i<EDSC_MAX)
  {
    if(IsDlgButtonChecked(hDlg,uIdc)==BST_CHECKED)
      config->mute[i] = 1 ; 
    else
      config->mute[i] = 0 ;
    config->volume_update = 1 ;
    return TRUE ;
  }

  return FALSE ;

}

static BOOL CALLBACK dlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  CONFIG *config ;
  int dwPos, i ;

  if(uMsg == WM_INITDIALOG)
  {
    config = ((CONFIG *)((LPPROPSHEETPAGE)lParam)->lParam) ;
    SetProp(hDlg,"CONFIG",config) ;
    update_page(hDlg, config) ;
    SetTimer(hDlg,1,200,NULL) ;
    return TRUE ;
  }
  else config = (CONFIG *)GetProp(hDlg,"CONFIG") ;

  switch(uMsg)
  {
  case WM_NOTIFY:
    switch(((LPNMHDR)lParam)->code)
    {
    case PSN_APPLY:
      return TRUE ;
    case PSN_RESET:
      return TRUE ;
    }
    break ;

  case WM_HSCROLL:
    for(i=0;i<EDSC_MAX;i++)
    {
      if((HWND)lParam == GetDlgItem(hDlg,PAN_SLIDER[i].id))
      {
        dwPos = 128 - SendMessage((HWND)lParam, TBM_GETPOS, 0, 0);
        if(-16<dwPos&&dwPos<16)
          SendMessage((HWND)lParam, TBM_SETPOS, TRUE, (long)128) ;
        
        CONFIG_rd_slider(config, hDlg, PAN_SLIDER[i].name, PAN_SLIDER[i].id, adjust_pan_rd);
        config->pan_update = 1 ;
        return TRUE;
      }
    }
    return FALSE ;

  case WM_VSCROLL:
    if((HWND)lParam == GetDlgItem(hDlg,IDC_MASTERVOL))
    {
      CONFIG_rd_slider(config,hDlg,"MASTER_VOL",IDC_MASTERVOL,adjust_func1);
      CONFIG_wr_textbox(config,hDlg,"MASTER_VOL",IDC_MASTERDB);
      config->volume_update = 1 ;
      return TRUE;
    }
    for(i=0;i<EDSC_MAX;i++)
    {
      dwPos = 128-SendMessage((HWND)lParam, TBM_GETPOS, 0, 0);
      if((HWND)lParam == GetDlgItem(hDlg,SLIDER_ID[i]))
      {
        config->curvol[i] = dwPos;
        set_db_text(GetDlgItem(hDlg,DBTEXT_ID[i]),config->curvol[i]) ;
        if(CONFIG_get_int(config,"AUTOSAVE_VOLUME"))
          config->vol[i] = config->curvol[i];
        config->volume_update = 1 ;
        return TRUE ;
      }

    }
    return FALSE ;

  case WM_COMMAND:
    if(HIWORD(wParam)==BN_CLICKED) return bn_clicked_event(hDlg,LOWORD(wParam),config) ;
    return TRUE;

  case WM_TIMER:
    if(config->refresh_mixer)
    {
      update_page(hDlg, config) ;
      config->refresh_mixer = 0 ;
    }
    return TRUE ;

  case WM_DESTROY:
    KillTimer(hDlg,1) ;
    RemoveProp(hDlg,"CONFIG") ;
    return TRUE ;

  default:
    break;
  }
  return FALSE ;
}

HPROPSHEETPAGE CreateConfigPage5(HINSTANCE hInst, CONFIG *config)
{
  PROPSHEETPAGE psp;

  psp.dwSize = sizeof(PROPSHEETPAGE);
  psp.dwFlags = PSP_DEFAULT ;
  psp.hInstance = hInst;
  psp.pszTemplate = MAKEINTRESOURCE(IDD_CFG_PAGE5);
  psp.pszIcon = NULL;
  psp.pfnDlgProc = dlgProc;
  psp.pszTitle = NULL;
  psp.lParam = (long)config ;

  return CreatePropertySheetPage(&psp) ;
}
