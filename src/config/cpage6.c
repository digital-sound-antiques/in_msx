#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include "config.h"

static void update_page(HWND hDlg, CONFIG *config)
{
  CONFIG_wr_textbox(config, hDlg, "LOOP", IDC_LOOPNUM);
  CONFIG_wr_textbox(config, hDlg, "PLAYTIME", IDC_PLAYTIME);
  CONFIG_wr_textbox(config, hDlg, "FADETIME", IDC_FADETIME);
  CONFIG_wr_textbox(config, hDlg, "SILENT_LIMIT", IDC_SILENT);
  CONFIG_wr_checkbtn(config, hDlg, "SILENT_DETECT",IDC_SILENT_ENABLE);
  CONFIG_wr_radiobtn(config, hDlg, "TIME_DETECT_MODE", IDC_TDMODE0, IDC_TDMODE3,0);
  CONFIG_wr_radiobtn(config, hDlg, "TIME_DETECT_LEVEL", IDC_TDLEVEL0, IDC_TDLEVEL1,0);
} ;

static void update_config(HWND hDlg, CONFIG *config)
{
  CONFIG_rd_textbox(config, hDlg, "LOOP", IDC_LOOPNUM);
  CONFIG_rd_textbox(config, hDlg, "PLAYTIME", IDC_PLAYTIME);
  CONFIG_rd_textbox(config, hDlg, "FADETIME", IDC_FADETIME);
  CONFIG_rd_textbox(config, hDlg, "SILENT_LIMIT", IDC_SILENT);
  CONFIG_rd_checkbtn(config, hDlg, "SILENT_DETECT",IDC_SILENT_ENABLE);
  CONFIG_rd_radiobtn(config, hDlg, "TIME_DETECT_MODE", IDC_TDMODE0, IDC_TDMODE3,0);
  CONFIG_rd_radiobtn(config, hDlg, "TIME_DETECT_LEVEL", IDC_TDLEVEL0, IDC_TDLEVEL1,0);
}

static BOOL CALLBACK dlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  CONFIG *config ;

  if(uMsg == WM_INITDIALOG)
  {
    config = ((CONFIG *)((LPPROPSHEETPAGE)lParam)->lParam) ;
    SetProp(hDlg,"CONFIG",config) ;
    SendMessage(GetDlgItem(hDlg,IDC_LOOPSPIN), UDM_SETRANGE, 0, MAKELONG(255,0)) ;
    SendMessage(GetDlgItem(hDlg,IDC_SILENTSPIN), UDM_SETRANGE, 0, MAKELONG(60,1));
    update_page(hDlg, config) ;
    return TRUE ;
  }
  else config = (CONFIG *)GetProp(hDlg,"CONFIG") ;

  switch(uMsg)
  {
  case WM_COMMAND:
    if(config)
    {
      switch(LOWORD(wParam))
      {
      case IDC_PLAYTIME:
      case IDC_FADETIME:
      case IDC_SILENT:
      case IDC_LOOPNUM:
        if(HIWORD(wParam)==EN_CHANGE)
          PropSheet_Changed(config->dialog, hDlg) ;
        break;
      default:
        PropSheet_Changed(config->dialog, hDlg) ;
        break;
      }
    }
    return TRUE ;

  case WM_NOTIFY:
    switch(((LPNMHDR)lParam)->code)
    {
    case PSN_APPLY:
      update_config(hDlg,config) ;
      return TRUE ;
    case PSN_RESET:
      return TRUE ;
    }
    break ;

  case WM_DESTROY:
    RemoveProp(hDlg,"CONFIG") ;
    return TRUE ;

  default:
    break;
  }
  return FALSE ;
}

HPROPSHEETPAGE CreateConfigPage6(HINSTANCE hInst, CONFIG *config)
{
  PROPSHEETPAGE psp;

  psp.dwSize = sizeof(PROPSHEETPAGE);
  psp.dwFlags = PSP_DEFAULT ;
  psp.hInstance = hInst;
  psp.pszTemplate = MAKEINTRESOURCE(IDD_CFG_PAGE6);
  psp.pszIcon = NULL;
  psp.pfnDlgProc = dlgProc;
  psp.pszTitle = NULL;
  psp.lParam = (long)config ;
  
  return CreatePropertySheetPage(&psp) ;
}
