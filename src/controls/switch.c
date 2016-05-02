#include <windows.h>
#include <commctrl.h>
#include <malloc.h>
#include <crtdbg.h>
#include "control.h"

static BOOL CALLBACK dlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch(uMsg)
  {
  case WM_SETTEXT:
    return SendMessage(GetDlgItem(hDlg,IDC_SWITCH_TITLE), uMsg, wParam, lParam) ;

  case WM_COMMAND:
    if(HIWORD(wParam)==BN_CLICKED)
    {
      if(IsDlgButtonChecked(hDlg, IDC_SWITCH_ON)) SendMessage(GetParent(hDlg), WM_HSCROLL, 1, (LPARAM)hDlg) ;
      else SendMessage(GetParent(hDlg), WM_HSCROLL, 0, (LPARAM)hDlg) ;
      return TRUE ;
    }
    break ;

  case WM_ENABLE:
    EnableWindow(GetDlgItem(hDlg,IDC_SWITCH_ON), wParam) ;
    EnableWindow(GetDlgItem(hDlg,IDC_SWITCH_OFF), wParam) ;
    EnableWindow(GetDlgItem(hDlg,IDC_SWITCH_TITLE), wParam) ;
    return TRUE ;

  case WM_USER:

    return TRUE ;

  case WM_USER+1:
    if(wParam)
    {
      CheckRadioButton(hDlg, IDC_SWITCH_ON, IDC_SWITCH_OFF, IDC_SWITCH_ON) ;
    }
    else
    {
      CheckRadioButton(hDlg, IDC_SWITCH_ON, IDC_SWITCH_OFF, IDC_SWITCH_OFF) ;
    }
    UpdateWindow(hDlg) ;
    return TRUE ;

  case WM_DESTROY:
    break ;

  default:
    break ;
  }

  return FALSE ;
}

HWND CreateSwitchControl(HINSTANCE hInst, HWND hWndParent, char *title)
{
  HWND hwnd ;
  hwnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD_SWITCH), hWndParent, dlgProc) ;
  SetWindowText(hwnd, title) ; 
  return hwnd ; 
}

