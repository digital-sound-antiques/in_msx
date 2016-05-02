#include <stdio.h>
#include <malloc.h>
#include <windows.h>
#include <commctrl.h>
#include <assert.h>
#include <crtdbg.h>
#include "pandlg.h"
#include "controls/control.h"

PANDLG *PANDLG_new(HINSTANCE hInstance, CONFIG *config)
{
  PANDLG *dlg = malloc(sizeof(PANDLG));
  dlg->hInst = hInstance;
  dlg->config = config; 
  dlg->hDialog = NULL;
  return dlg;
}

void PANDLG_update_config(PANDLG *dlg, int val)
{
  CONFIG_set_int(dlg->config,"OPLL_CH_PAN",val);
  dlg->config->ch_pan_update = 1;
}

void PANDLG_update_page(PANDLG *dlg)
{
  static int P2S[] = { 1, 2, 0, 1 };
  int pan = CONFIG_get_int(dlg->config,"OPLL_CH_PAN");
  for(int i=0;i<14;i++)
  {
    if((pan>>(i*2))&3)
    {
      SendMessage(dlg->hParam[i],WM_USER+1,P2S[(pan>>(i*2))&3],0);
      CheckDlgButton(dlg->hParam[i],IDC_TITLE,BST_UNCHECKED);
    }
    else
    {
      SendMessage(dlg->hParam[i],WM_USER+1,1,0);
      CheckDlgButton(dlg->hParam[i],IDC_TITLE,BST_CHECKED);
    }
  }
}

static BOOL CALLBACK PANDLG_dlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  static int S2P[] = { 2, 3, 1 };
  PANDLG *ctx = (PANDLG *)GetProp(hDlg,"PanDialog") ;
  int i,val = 0;

  switch(uMsg)
  {
  case WM_INITDIALOG:
    return FALSE ;

  case WM_HSCROLL:
    for(i=0;i<14;i++)
    {
      if(IsDlgButtonChecked(ctx->hParam[i],IDC_TITLE)==BST_UNCHECKED)
        val |= S2P[SendMessage(GetDlgItem(ctx->hParam[i],IDC_SLIDER),WM_USER,0,0)&3]<<(i*2);
    }
    PANDLG_update_config(ctx,val);
    return TRUE;
    break ;

  case WM_COMMAND:
    if(HIWORD(wParam)==BN_CLICKED)
    {
      switch(LOWORD(wParam))
      {
      case IDOK:
        PANDLG_close(ctx);
        return TRUE ;

      case IDC_RESET:
        PANDLG_update_config(ctx,0x7EDE79E);
        PANDLG_update_page(ctx);
        return TRUE;

      default:
        break ;
      }
    }

    break ;

  case WM_CLOSE:
    PANDLG_close(ctx);
    return TRUE;

  case WM_DESTROY:
    return TRUE;

  default:
    break;
  }
  return FALSE ;
}

void PANDLG_open(PANDLG *dlg, HWND hParent)
{
  int x = 4, y = 4, pan;
  char *LABEL[] = {"Ch 1","Ch 2","Ch 3","Ch 4","Ch 5","Ch 6","Ch 7","Ch 8","Ch 9","BD","HH","SD","TOM","CYM"};

  if(!dlg->hDialog)
  {
    dlg->hDialog = CreateDialog(dlg->hInst, MAKEINTRESOURCE(IDD_PANDLG), hParent, PANDLG_dlgProc);
    pan = CONFIG_get_int(dlg->config,"OPLL_CH_PAN");
    for(int i=0;i<14;i++)
    {
      RECT rect;

      dlg->hParam[i] = CreatePanControl(dlg->hInst, dlg->hDialog, LABEL[i], 0,2) ;
      SetWindowPos(dlg->hParam[i],NULL,x,y,0,0,SWP_NOSIZE|SWP_NOZORDER) ;
      GetWindowRect(dlg->hParam[i], &rect);
      y += rect.bottom - rect.top ;
      ShowWindow(dlg->hParam[i], SW_SHOW);
    }
    SetProp(dlg->hDialog,"PanDialog",dlg);
  }
  PANDLG_update_page(dlg);
  ShowWindow(dlg->hDialog, SW_SHOW);
}

void PANDLG_close(PANDLG *dlg)
{
  DestroyWindow(dlg->hDialog);
  RemoveProp(dlg->hDialog,"PanDialog");
  dlg->hDialog=NULL;
}

void PANDLG_delete(PANDLG *dlg)
{
  free(dlg);
}