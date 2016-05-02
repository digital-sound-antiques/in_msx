#include <stdio.h>
#include <windows.h>
#include <commctrl.h>
#include <assert.h>
#include "msxplug.h"
#include "kssplay.h"
#include "kssdlg.h"
#include "rc/resource.h"

static BOOL CALLBACK dlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  KSSDLG *kssdlg = (KSSDLG *)GetProp(hDlg, "INFO") ;

  switch (uMsg)
	{
  case WM_INITDIALOG:
    SendMessage(GetDlgItem(hDlg,IDC_SLIDER), TBM_SETRANGE, TRUE, MAKELONG(0,255)) ;
    SendMessage(GetDlgItem(hDlg,IDC_SLIDER), TBM_SETPOS, TRUE, 0) ;
    SetWindowText(GetDlgItem(hDlg,IDC_SONG),"0");
		return TRUE;

  case WM_HSCROLL:
    if((HWND)(lParam) == GetDlgItem(hDlg,IDC_SLIDER))
    {
      char buf[16];
      int pos = SendMessage((HWND)(lParam), TBM_GETPOS, 0, 0);
      SetWindowText(GetDlgItem(hDlg,IDC_SONG),itoa(pos,buf,16));
      if(wParam == SB_ENDSCROLL) MSXPLUG_play_song(pos);
    }
    break;

  case WM_COMMAND:
    
    if(LOWORD(wParam) == IDM_OPTDLG)
    {
      MSXPLUG_optdlg(NULL);
      break;
    }
    else if(LOWORD(wParam) == IDM_CONFIG)
    {
      MSXPLUG_config(NULL);
      break;
    }
    else if(LOWORD(wParam) == IDM_VEDIT)
    {
      MSXPLUG_edit2413(NULL);
      break;
    }
    else if(LOWORD(wParam) == IDM_MASK)
    {
      MSXPLUG_maskdlg(NULL);
      break;
    }

    if(HIWORD(wParam) == BN_CLICKED)
    {
      if(LOWORD(wParam) == IDC_PREV)
      {
        SendMessage(GetDlgItem(hDlg,IDC_SLIDER),WM_KEYDOWN,VK_LEFT,1);
        SendMessage(GetDlgItem(hDlg,IDC_SLIDER),WM_KEYUP,VK_LEFT,0);
        return TRUE;
      }
      if(LOWORD(wParam) == IDC_NEXT)
      {
        SendMessage(GetDlgItem(hDlg,IDC_SLIDER),WM_KEYDOWN,VK_RIGHT,1);
        SendMessage(GetDlgItem(hDlg,IDC_SLIDER),WM_KEYUP,VK_RIGHT,0);
        return TRUE;
      }
      if(LOWORD(wParam) == IDOK)
      {
        KSSDLG_close(kssdlg) ;
        return TRUE;
      }
      else if(LOWORD(wParam) == IDCANCEL)
      {
        KSSDLG_close(kssdlg) ;
        return TRUE ;
      }
      else if(LOWORD(wParam) == IDC_SYNC)
      {
        if(IsDlgButtonChecked(hDlg, IDC_SYNC) == BST_CHECKED)
          kssdlg->sync = 1 ;
        else
          kssdlg->sync = 0 ;
        return TRUE ;
      }
    }
    break ;

  default:
    break; 
	}

	return FALSE;
}

static const char *type2str(int type)
{
  switch(type)
  {
  case KSSDATA:
    return "KSS" ;
  case MGSDATA:
    return "MGSDRV" ;
  case MPK106DATA:
  case MPK103DATA:
    return "MPK" ;
  case OPXDATA:
    return "OPLLDriver" ;
  case BGMDATA:
    return "MuSICA" ;
  case MBMDATA:
    return "MoonBlaster" ;
  default:
    return "UNKNOWN" ;
  }
}

void KSSDLG_update(KSSDLG *kssdlg, char *filename, int playlist_mode)
{
  KSS *kss ;
  char buf[16];

  assert(kssdlg) ;

  if(!kssdlg->dialog) return ;

  SetWindowText(GetDlgItem(kssdlg->dialog, IDC_FILENAME),filename ) ;
  kss = KSS_load_file(filename) ;
  if(kss)
  {
    SetWindowText(GetDlgItem(kssdlg->dialog, IDC_TYPE), type2str(kss->type)) ;
    SetWindowText(GetDlgItem(kssdlg->dialog, IDC_ID), (char *)kss->idstr) ;
    SetWindowText(GetDlgItem(kssdlg->dialog, IDC_TITLE), (char *)kss->title) ;
    SetWindowText(GetDlgItem(kssdlg->dialog, IDC_EXTRA), (char *)kss->extra) ;
    SendMessage(GetDlgItem(kssdlg->dialog,IDC_SLIDER), TBM_SETRANGE, TRUE, MAKELONG(kss->trk_min,kss->trk_max)) ;
    SendMessage(GetDlgItem(kssdlg->dialog,IDC_SLIDER), TBM_SETPOS, TRUE, MSXPLUG_get_song()) ;
    if(kss->trk_max==kss->trk_min)
    {
      EnableWindow(GetDlgItem(kssdlg->dialog,IDC_SLIDER),FALSE);
      EnableWindow(GetDlgItem(kssdlg->dialog,IDC_PREV),FALSE);
      EnableWindow(GetDlgItem(kssdlg->dialog,IDC_NEXT),FALSE);
      SendMessage(GetDlgItem(kssdlg->dialog,IDC_SLIDER), TBM_SETTICFREQ, 1,0) ;
      SendMessage(GetDlgItem(kssdlg->dialog,IDC_SLIDER), TBM_SETPAGESIZE, 0,1);
      SetWindowText(GetDlgItem(kssdlg->dialog,IDC_SONG),"N/A");
    }
    else
    {
      EnableWindow(GetDlgItem(kssdlg->dialog,IDC_SLIDER),playlist_mode?FALSE:TRUE);
      EnableWindow(GetDlgItem(kssdlg->dialog,IDC_PREV),playlist_mode?FALSE:TRUE);
      EnableWindow(GetDlgItem(kssdlg->dialog,IDC_NEXT),playlist_mode?FALSE:TRUE);
      SendMessage(GetDlgItem(kssdlg->dialog,IDC_SLIDER), TBM_SETTICFREQ, (kss->trk_max-kss->trk_min+1)/16,0) ;
      SendMessage(GetDlgItem(kssdlg->dialog,IDC_SLIDER), TBM_SETPAGESIZE, 0, (kss->trk_max-kss->trk_min+1)/16) ;
      SendMessage(GetDlgItem(kssdlg->dialog,IDC_SLIDER), TBM_SETLINESIZE, 0, 1);
      SetWindowText(GetDlgItem(kssdlg->dialog,IDC_SONG),itoa(MSXPLUG_get_song(),buf,16));
    }
    KSS_delete(kss) ;
  }
  else
  {
    SetWindowText(GetDlgItem(kssdlg->dialog, IDC_TYPE), NULL) ;
    SetWindowText(GetDlgItem(kssdlg->dialog, IDC_ID), NULL) ;
    SetWindowText(GetDlgItem(kssdlg->dialog, IDC_TITLE), NULL) ;
    SetWindowText(GetDlgItem(kssdlg->dialog, IDC_EXTRA), NULL) ;
  }

  if(kssdlg->sync)
    CheckDlgButton(kssdlg->dialog, IDC_SYNC, BST_CHECKED) ;
  else
    CheckDlgButton(kssdlg->dialog, IDC_SYNC, BST_UNCHECKED) ;

}

void KSSDLG_close(KSSDLG *kssdlg)
{
  if(kssdlg->dialog)
  {
    RemoveProp(kssdlg->dialog, "INFO") ;
    DestroyWindow(kssdlg->dialog) ;
    kssdlg->dialog = NULL ;
  }
}

static void setDefaultGuiFont(HWND hWnd)
{
  SendMessage(hWnd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0));    
}

void KSSDLG_open(KSSDLG *kssdlg, HINSTANCE hInst, HWND hParent)
{
  if(!kssdlg->dialog)
  {
    kssdlg->dialog = CreateDialog(hInst, MAKEINTRESOURCE(IDD_KSSDLG), hParent, dlgProc ) ;
    assert(kssdlg->dialog) ;
    SetProp(kssdlg->dialog, "INFO", kssdlg) ;
    setDefaultGuiFont(GetDlgItem(kssdlg->dialog, IDC_TITLE));
    setDefaultGuiFont(GetDlgItem(kssdlg->dialog, IDC_FILENAME));
    setDefaultGuiFont(GetDlgItem(kssdlg->dialog, IDC_EXTRA));
  }
  ShowWindow(kssdlg->dialog, SW_SHOW) ;
}

KSSDLG *KSSDLG_new(void)
{
  KSSDLG *kssdlg ;

  if(!(kssdlg = (KSSDLG *)malloc(sizeof(KSSDLG)))) return NULL ;
  kssdlg->dialog = NULL ;
  kssdlg->sync = 1 ;

  return kssdlg ;
}

void KSSDLG_delete(KSSDLG *kssdlg)
{
  KSSDLG_close(kssdlg) ;
  free(kssdlg) ;
}