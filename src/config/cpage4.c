#include <windows.h>
#include "config.h"

static void update_page(HWND hDlg, CONFIG *config)
{
  CONFIG_wr_textbox(config, hDlg, "MGSDRV", IDC_MGSDRV_PATH);
  CONFIG_wr_textbox(config, hDlg, "KINROU", IDC_KINROU_PATH);
  CONFIG_wr_textbox(config, hDlg, "MPK103", IDC_MPK103_PATH);
  CONFIG_wr_textbox(config, hDlg, "MPK106", IDC_MPK106_PATH);
  CONFIG_wr_textbox(config, hDlg, "OPXDRV", IDC_OPXDRV_PATH);
  CONFIG_wr_textbox(config, hDlg, "FMBIOS", IDC_FMBIOS_PATH);
  CONFIG_wr_textbox(config, hDlg, "MBMDRV", IDC_MBMDRV_PATH);
}

static void update_config(HWND hDlg, CONFIG *config)
{
  CONFIG_rd_textbox(config, hDlg, "MGSDRV", IDC_MGSDRV_PATH);
  CONFIG_rd_textbox(config, hDlg, "KINROU", IDC_KINROU_PATH);
  CONFIG_rd_textbox(config, hDlg, "MPK103", IDC_MPK103_PATH);
  CONFIG_rd_textbox(config, hDlg, "MPK106", IDC_MPK106_PATH);
  CONFIG_rd_textbox(config, hDlg, "OPXDRV", IDC_OPXDRV_PATH);
  CONFIG_rd_textbox(config, hDlg, "FMBIOS", IDC_FMBIOS_PATH);
  CONFIG_rd_textbox(config, hDlg, "MBMDRV", IDC_MBMDRV_PATH);
}

static int get_driver_filename(HWND hWnd, char *buf, int max)
{
  OPENFILENAME ofn ;

  memset(&ofn,0,sizeof(OPENFILENAME)) ;
  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = hWnd ;
  ofn.lpstrFilter = "All Files(*.*)\0*.*\0\0";
  ofn.Flags = OFN_FILEMUSTEXIST ; 
  ofn.lpstrFile = buf ;
  ofn.nMaxFile = max ;

  return GetOpenFileName(&ofn) ;
}

static int set_driver_filename(HWND hDlg, UINT uIdc,  char *drvpath)
{
  char buf[_MAX_PATH] ;

  strncpy(buf, drvpath, _MAX_PATH) ;
  buf[_MAX_PATH-1]='\0' ;
  if(get_driver_filename(hDlg, buf, _MAX_PATH))
  {
    HWND hWnd = GetDlgItem(hDlg, uIdc) ;
    SetWindowText(hWnd,buf);
    strncpy(drvpath, buf,_MAX_PATH) ;
    drvpath[_MAX_PATH-1]='\0' ;
    return 1 ;
  }
  else return 0 ;
}

static BOOL bn_clicked_event(HWND hDlg, UINT uIdc, CONFIG *config)
{
  char buf[_MAX_PATH] = "\0" ;

  switch(uIdc)
  {
  case  IDC_SET_MBMDRV:
    if(set_driver_filename(hDlg, IDC_MBMDRV_PATH, buf))
    {
      CONFIG_set_str(config,"MBMDRV",buf);
      config->mbmdrv_loaded = 0;
    }
    return TRUE;

  case  IDC_SET_MGSDRV:
    if(set_driver_filename(hDlg, IDC_MGSDRV_PATH, buf))
    {
      CONFIG_set_str(config,"MGSDRV",buf);
      config->mgsdrv_loaded = 0 ;
    }
    return TRUE;
  
  case IDC_SET_KINROU:
    if(set_driver_filename(hDlg, IDC_KINROU_PATH, buf))
    {
      CONFIG_set_str(config,"KINROU",buf);
      config->kinrou_loaded = 0 ;
    }
    return TRUE;

  case IDC_SET_MPK106:
    if(set_driver_filename(hDlg, IDC_MPK106_PATH, buf))
    {
      CONFIG_set_str(config,"MPK106",buf);
      config->mpk106_loaded = 0 ;
    }
    return TRUE;

  case IDC_SET_MPK103:
    if(set_driver_filename(hDlg, IDC_MPK103_PATH, buf))
    {
      CONFIG_set_str(config,"MPK103",buf);
      config->mpk103_loaded = 0 ;
    }
    return TRUE;

  case IDC_SET_OPXDRV:
    if(set_driver_filename(hDlg, IDC_OPXDRV_PATH, buf))
    {
      CONFIG_set_str(config,"OPXDRV",buf);
      config->opxdrv_loaded = 0 ;
    }
    return TRUE;

  case IDC_SET_FMBIOS:
    if(set_driver_filename(hDlg, IDC_FMBIOS_PATH, buf))
    {
      CONFIG_set_str(config,"FMBIOS",buf);
      config->fmbios_loaded = 0 ;
    }
    return TRUE;

  default:
    return FALSE;
  }
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
  case WM_DESTROY:
    RemoveProp(hDlg,"CONFIG") ;
    return TRUE ;

  case WM_COMMAND:
    if(HIWORD(wParam)==BN_CLICKED)
      return bn_clicked_event(hDlg,LOWORD(wParam),config) ;
    break ;

  default:
    break;
  }
  return FALSE ;
}

HPROPSHEETPAGE CreateConfigPage4(HINSTANCE hInst, CONFIG *config)
{
  PROPSHEETPAGE psp;

  psp.dwSize = sizeof(PROPSHEETPAGE);
  psp.dwFlags = PSP_DEFAULT ;
  psp.hInstance = hInst;
  psp.pszTemplate = MAKEINTRESOURCE(IDD_CFG_PAGE4);
  psp.pszIcon = NULL;
  psp.pfnDlgProc = dlgProc;
  psp.pszTitle = NULL;
  psp.lParam = (long)config ;
  
  return CreatePropertySheetPage(&psp) ;
}
