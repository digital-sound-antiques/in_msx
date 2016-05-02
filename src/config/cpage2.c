#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include "config.h"

static void print_time(char *buf, int time)
{
  int h,m,s,ss = 0 ;

  ss = (time%1000) / 10 ;
  time /= 1000 ;
  s = time%60 ;
  time /= 60 ;
  m = time%60 ;
  time /= 60 ;
  h = time ;
  
  sprintf(buf,"%02d:%02d:%02d",h,m,s) ;
}

static void update_page(HWND hDlg, CONFIG *config)
{
  int i,rate_list[] = {96000,49716,48000,44100,32000,22050,11025} ;
  int rate = CONFIG_get_int(config, "RATE");

  for(i=0;i<7;i++)
  {
    if(rate==rate_list[i])
    {
      CheckRadioButton(hDlg, IDC_RADIO96K, IDC_RADIO11K, IDC_RADIO96K+i) ;
      break;
    }
  }

  CONFIG_wr_radiobtn(config,hDlg,"CPUSPEED",IDC_CLK_AUTO,IDC_CLK_FASTEST,0);
  CONFIG_wr_radiobtn(config,hDlg,"STEREO",IDC_STEREO_AUTO,IDC_STEREO_ON,0);
  CONFIG_wr_checkbtn(config,hDlg,"PSG_HQ",IDC_PSG_HQ);
  CONFIG_wr_checkbtn(config,hDlg,"SCC_HQ",IDC_SCC_HQ);
  CONFIG_wr_checkbtn(config,hDlg,"OPLL_HQ",IDC_OPLL_HQ);
  CONFIG_wr_textbox(config,hDlg,"BUFSIZE",IDC_BUFFER);
}

static void update_config(HWND hDlg, CONFIG *config)
{
  int i,rate_list[] = {96000,49716,48000,44100,32000,22050,11025} ;

  for(i=0;i<7;i++)
    if(IsDlgButtonChecked(hDlg, IDC_RADIO96K+i)==BST_CHECKED)
      CONFIG_set_int(config,"RATE",rate_list[i]);

  CONFIG_rd_radiobtn(config,hDlg,"CPUSPEED",IDC_CLK_AUTO,IDC_CLK_FASTEST,0);
  CONFIG_rd_radiobtn(config,hDlg,"STEREO",IDC_STEREO_AUTO,IDC_STEREO_ON,0);
  CONFIG_rd_checkbtn(config,hDlg,"PSG_HQ",IDC_PSG_HQ);
  CONFIG_rd_checkbtn(config,hDlg,"SCC_HQ",IDC_SCC_HQ);
  CONFIG_rd_checkbtn(config,hDlg,"OPLL_HQ",IDC_OPLL_HQ);
  CONFIG_rd_textbox(config,hDlg,"BUFSIZE",IDC_BUFFER);
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
    if(config)
    {
      if(HIWORD(wParam)==BN_CLICKED)
      {
        if(LOWORD(wParam)==IDC_PSG_HQ)
        {
          CONFIG_rd_checkbtn(config, hDlg, "PSG_HQ", IDC_PSG_HQ);
          config->quality_update = 1 ;
        }
        else if(LOWORD(wParam)==IDC_SCC_HQ)
        {
          CONFIG_rd_checkbtn(config, hDlg, "SCC_HQ", IDC_SCC_HQ);
          config->quality_update = 1 ;
        }
        else if(LOWORD(wParam)==IDC_OPLL_HQ)
        {
          CONFIG_rd_checkbtn(config, hDlg, "OPLL_HQ", IDC_OPLL_HQ);
          config->quality_update = 1 ;
        }
        else PropSheet_Changed(config->dialog, hDlg) ;
      }
      else PropSheet_Changed(config->dialog, hDlg) ;
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

HPROPSHEETPAGE CreateConfigPage2(HINSTANCE hInst, CONFIG *config)
{
  PROPSHEETPAGE psp;

  psp.dwSize = sizeof(PROPSHEETPAGE);
  psp.dwFlags = PSP_DEFAULT ;
  psp.hInstance = hInst;
  psp.pszTemplate = MAKEINTRESOURCE(IDD_CFG_PAGE2);
  psp.pszIcon = NULL;
  psp.pfnDlgProc = dlgProc;
  psp.pszTitle = NULL;
  psp.lParam = (long)config ;
  
  return CreatePropertySheetPage(&psp) ;
}
