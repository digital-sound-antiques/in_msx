#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include "config.h"

static const int IDs_CUTOFF[EDSC_MAX] = {IDC_PSG_CUTOFF,IDC_SCC_CUTOFF,IDC_OPLL_CUTOFF,IDC_OPL_CUTOFF};

static void update_page(HWND hDlg, CONFIG *config)
{
  CONFIG_wr_textbox(config, hDlg, "PSG_CUTOFF", IDC_PSG_CUTOFF);
  CONFIG_wr_textbox(config, hDlg, "SCC_CUTOFF", IDC_SCC_CUTOFF);
  CONFIG_wr_textbox(config, hDlg, "OPLL_CUTOFF", IDC_OPLL_CUTOFF);
  CONFIG_wr_textbox(config, hDlg, "OPL_CUTOFF", IDC_OPL_CUTOFF);
  CONFIG_wr_textbox(config, hDlg, "R",IDC_R);
  CONFIG_wr_textbox(config, hDlg, "C",IDC_C);
  CONFIG_wr_checkbtn(config, hDlg, "ENABLE_RCF", IDC_ENABLE_RCF);
  CONFIG_wr_checkbtn(config, hDlg, "ENABLE_DCF", IDC_ENABLE_DCF);
}


static void update_config(HWND hDlg, CONFIG *config)
{
  CONFIG_rd_textbox(config, hDlg, "PSG_CUTOFF", IDC_PSG_CUTOFF);
  CONFIG_rd_textbox(config, hDlg, "SCC_CUTOFF", IDC_SCC_CUTOFF);
  CONFIG_rd_textbox(config, hDlg, "OPLL_CUTOFF", IDC_OPLL_CUTOFF);
  CONFIG_rd_textbox(config, hDlg, "OPL_CUTOFF", IDC_OPL_CUTOFF);
  CONFIG_rd_textbox(config, hDlg, "R",IDC_R);
  CONFIG_rd_textbox(config, hDlg, "C",IDC_C);
  CONFIG_rd_checkbtn(config,hDlg,  "ENABLE_RCF", IDC_ENABLE_RCF);
  CONFIG_rd_checkbtn(config, hDlg, "ENABLE_DCF", IDC_ENABLE_DCF);
  config->filter_update = 1;
}

static BOOL CALLBACK dlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  CONFIG *config ;
  int i,j;

  if(uMsg == WM_INITDIALOG)
  {
    config = ((CONFIG *)((LPPROPSHEETPAGE)lParam)->lParam) ;
    SetProp(hDlg,"CONFIG",config) ;
    for(i=0;i<EDSC_MAX;i++)
    {
      char *cutoffs[]=
      {
        "0","1000","2000","3000","4000","5000",
        "6000","7000","8000","9000","10000",
        "11000","12000","13000","14000","15000",
        "16000","17000","18000","19000","20000",NULL
      };
      char *windows[]={"Square","Hamming","Hannig","Bartlett",NULL};
      for(j=0;cutoffs[j];j++)
        SendMessage(GetDlgItem(hDlg,IDs_CUTOFF[i]),CB_ADDSTRING,0,(LPARAM)cutoffs[j]);
    }
    update_page(hDlg, config) ;
    return TRUE ;
  }
  else config = (CONFIG *)GetProp(hDlg,"CONFIG") ;

  switch(uMsg)
  {
  case WM_COMMAND:
    if((HWND)lParam==GetDlgItem(hDlg,IDC_R)||
       (HWND)lParam==GetDlgItem(hDlg,IDC_C)||
       (HWND)lParam==GetDlgItem(hDlg,IDC_ENABLE_DCF)||
       (HWND)lParam==GetDlgItem(hDlg,IDC_ENABLE_RCF))
    {
      PropSheet_Changed(config->dialog, hDlg);
      return TRUE;
    }
    for(i=0;i<EDSC_MAX;i++)
    {
      if((HWND)lParam==GetDlgItem(hDlg,IDs_CUTOFF[i]))
      {
        if(HIWORD(wParam)==CBN_EDITCHANGE||HIWORD(wParam)==CBN_SELCHANGE)
        {
          PropSheet_Changed(config->dialog, hDlg);
          return TRUE;
        }
      }
    }
    return FALSE ;

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

HPROPSHEETPAGE CreateConfigPage7(HINSTANCE hInst, CONFIG *config)
{
  PROPSHEETPAGE psp;

  psp.dwSize = sizeof(PROPSHEETPAGE);
  psp.dwFlags = PSP_DEFAULT ;
  psp.hInstance = hInst;
  psp.pszTemplate = MAKEINTRESOURCE(IDD_CFG_PAGE7);
  psp.pszIcon = NULL;
  psp.pfnDlgProc = dlgProc;
  psp.pszTitle = NULL;
  psp.lParam = (long)config ;
  
  return CreatePropertySheetPage(&psp) ;
}
