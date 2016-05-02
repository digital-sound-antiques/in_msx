#include <windows.h>
#include "config.h"
#include "pandlg/pandlg.h"

static void update_page(HWND hDlg, CONFIG *config)
{
  CONFIG_wr_radiobtn(config, hDlg, "PSG_MODE", IDC_PSG_AUTO, IDC_PSG_YM,0);
  CONFIG_wr_radiobtn(config, hDlg, "SCC_MODE", IDC_SCC_AUTO, IDC_SCC_PLUS,0);
  CONFIG_wr_radiobtn(config, hDlg, "OPLL_MODE",IDC_OPLL_2413, IDC_OPLL_281B,0);
  CONFIG_wr_checkbtn(config, hDlg, "OPLL_STEREO",IDC_OPLL_STEREO);
}

static void update_config(HWND hDlg, CONFIG *config)
{
  CONFIG_rd_radiobtn(config, hDlg, "PSG_MODE", IDC_PSG_AUTO, IDC_PSG_YM,0);
  CONFIG_rd_radiobtn(config, hDlg, "SCC_MODE", IDC_SCC_AUTO, IDC_SCC_PLUS,0);
  CONFIG_rd_radiobtn(config, hDlg, "OPLL_MODE",IDC_OPLL_2413, IDC_OPLL_281B,0);
  CONFIG_rd_checkbtn(config, hDlg, "OPLL_STEREO",IDC_OPLL_STEREO);
}

static BOOL CALLBACK dlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  CONFIG *config ;

  if(uMsg == WM_INITDIALOG)
  {
    config = ((CONFIG *)((LPPROPSHEETPAGE)lParam)->lParam) ;
    SetProp(hDlg,"CONFIG",config) ;
    update_page(hDlg, config) ;
    return TRUE ;
  }
  else config = (CONFIG *)GetProp(hDlg,"CONFIG") ;

  switch(uMsg)
  {
  case WM_COMMAND:
    if(HIWORD(wParam)==BN_CLICKED)
    {
      switch(LOWORD(wParam))
      {
      case IDC_PANEDIT:
        PANDLG_open(config->pandlg, hDlg);
        break;
      case IDC_OPLL_STEREO:
      case IDC_OPLL_2413: case IDC_OPLL_VRC7: case IDC_OPLL_281B:
      case IDC_SCC_AUTO: case IDC_SCC_NORMAL: case IDC_SCC_PLUS:
      case IDC_PSG_AUTO: case IDC_PSG_AY: case IDC_PSG_YM:
        PropSheet_Changed(config->dialog,hDlg);
        break;
      default:
        break;
      }
    }
    return TRUE ;

  case WM_NOTIFY:
    switch(((LPNMHDR)lParam)->code)
    {
    case PSN_APPLY:
      update_config(hDlg,config) ;
      config->mode_update = 1 ;
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

HPROPSHEETPAGE CreateConfigPage3(HINSTANCE hInst, CONFIG *config)
{
  PROPSHEETPAGE psp;

  psp.dwSize = sizeof(PROPSHEETPAGE);
  psp.dwFlags = PSP_DEFAULT ;
  psp.hInstance = hInst;
  psp.pszTemplate = MAKEINTRESOURCE(IDD_CFG_PAGE3);
  psp.pszIcon = NULL;
  psp.pfnDlgProc = dlgProc;
  psp.pszTitle = NULL;
  psp.lParam = (long)config ;
  
  return CreatePropertySheetPage(&psp) ;
}
