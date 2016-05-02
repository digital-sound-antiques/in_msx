#include <stdio.h>
#include <windows.h>
#include <assert.h>
#include "msxplug.h"
#include "kssplay.h"
#include "optdlg/optdlg.h"
#include "rc/resource.h"

static char *getLyrics(OPTDLG *__this)
{
  int i, now;

  strncpy(__this->lyrics_buf[__this->lyrics_pos], MSXPLUG_getMGStext(),80);
  __this->lyrics_buf_time[__this->lyrics_pos] = MSXPLUG_getDecodeTime();
  now = MSXPLUG_getOutputTime();

  for(i=__this->lyrics_pos;i!=(__this->lyrics_pos+1)%MAX_BUFFERED;i=(i+MAX_BUFFERED-1)%MAX_BUFFERED)
  {
    if(now+FRAME_PERIOD >= __this->lyrics_buf_time[i])
    break;
  }

  __this->lyrics_pos = (__this->lyrics_pos+1)%MAX_BUFFERED;
  return __this->lyrics_buf[i];
}

static void create_font(OPTDLG *__this, int nHeight, int nWidth)
{
  if(__this->hFont)
    DeleteObject(__this->hFont);
  __this->hFont = CreateFont(
  nHeight, nWidth, 0, 0, 
  0, 0, 0, 0, SHIFTJIS_CHARSET, 
  OUT_DEFAULT_PRECIS, 
  CLIP_DEFAULT_PRECIS, 
  DEFAULT_QUALITY, 
  FIXED_PITCH|FF_DONTCARE,
  NULL);
  SetTextAlign(__this->hDC, TA_TOP);
  SelectObject(__this->hDC,__this->hFont);
  SelectObject(__this->hDC,(HBRUSH)GetStockObject(BLACK_BRUSH));
}

static void restrict_sizing(WPARAM wParam, LPARAM lParam, int min_w, int min_h, int max_w, int max_h)
{
  LPRECT lpRect = (LPRECT)lParam;
  int width = lpRect->right - lpRect->left;
  int height = lpRect->bottom - lpRect->top;

  switch(wParam)
  {
  case WMSZ_BOTTOM:
    if(height>max_h)
      lpRect->bottom = lpRect->top + max_h;
    else if(height<min_h)
      lpRect->bottom = lpRect->top + min_h;
    break;

  case WMSZ_BOTTOMLEFT:
    if(height>max_h)
      lpRect->bottom = lpRect->top + max_h;
    else if(height<min_h)
      lpRect->bottom = lpRect->top + min_h;
    if(width>max_w)
      lpRect->left = lpRect->right - max_w;
    else if(width<min_w)
      lpRect->left = lpRect->right - min_w;
    break;

  case WMSZ_BOTTOMRIGHT:
    if(height>max_h)
      lpRect->bottom = lpRect->top + max_h;
    else if(height<min_h)
      lpRect->bottom = lpRect->top + min_h;
    if(width>max_w)
      lpRect->right = lpRect->left + max_w;
    else if(width<min_w)
      lpRect->right = lpRect->left + min_w;
    break;

  case WMSZ_LEFT:
    if(width>max_w)
      lpRect->left = lpRect->right - max_w;
    else if(width<min_w)
      lpRect->left = lpRect->right - min_w;
    break;

  case WMSZ_RIGHT:
    if(width>max_w)
      lpRect->right = lpRect->left + max_w;
    else if(width<min_w)
      lpRect->right = lpRect->left + min_w;
    break;

  case WMSZ_TOP:
    if(height>max_h)
      lpRect->top = lpRect->bottom - max_h;
    else if(height<min_h)
      lpRect->top = lpRect->bottom - min_h;
    break;

  case WMSZ_TOPLEFT:
    if(height>max_h)
      lpRect->top = lpRect->bottom - max_h;
    else if(height<min_h)
      lpRect->top = lpRect->bottom - min_h;
    if(width>max_w)
      lpRect->left = lpRect->right - max_w;
    else if(width<min_w)
      lpRect->left = lpRect->right - min_w;
    break;

  case WMSZ_TOPRIGHT:
    if(height>max_h)
      lpRect->top = lpRect->bottom - max_h;
    else if(height<min_h)
      lpRect->top = lpRect->bottom - min_h;
    if(width>max_w)
      lpRect->right = lpRect->left + max_w;
    else if(width<min_w)
      lpRect->right = lpRect->left + min_w;
    break;

  default:
    break;
  }
}

static BOOL CALLBACK dlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  OPTDLG *__this = (OPTDLG *)GetProp(hDlg, "OPT") ;
  char tmp[81];
  RECT rect, crect;
  PAINTSTRUCT ps;
  HDC hDC;

  switch (uMsg)
	{
  case WM_TIMER:
	memset(tmp, '\0', sizeof(tmp));
    strncpy(tmp,getLyrics(__this),80);  
    GetClientRect(hDlg,&rect);
    FillRect(__this->hDC, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));
    TextOut(__this->hDC, 4, 4, getLyrics(__this), strlen(tmp));
    InvalidateRect(hDlg,NULL,FALSE);
    return TRUE;

  case WM_PAINT:
    hDC = BeginPaint(hDlg,&ps);
    GetClientRect(hDlg, &rect);
    BitBlt(hDC, 0, 0, rect.right, rect.bottom, __this->hDC, 0, 0, SRCCOPY);
    EndPaint(hDlg,&ps);
    return TRUE;

  case WM_INITDIALOG:
		return TRUE;

  case WM_COMMAND:
    /*
    if(LOWORD(wParam) == IDM_CHOOSEFONT)
    {
      LOGFONT lf;
      CHOOSEFONT cf;

      memset(&cf, 0, sizeof(CHOOSEFONT));
      cf.lStructSize = sizeof(CHOOSEFONT);
      cf.hwndOwner = hDlg;
      cf.lpLogFont = &lf;
      cf.Flags = CF_SCREENFONTS | CF_EFFECTS;
      cf.rgbColors = RGB(0, 0, 0);
      cf.nFontType = SCREEN_FONTTYPE;
      if(ChooseFont(&cf))
      {
        if(__this->hFont)
          DeleteObject((HGDIOBJ)__this->hFont);
        __this->hFont = CreateFontIndirect(&lf);
        SelectObject(__this->hDC,__this->hFont);
      }    
      return TRUE;
    }
    */
    
    if(HIWORD(wParam) == BN_CLICKED)
    {
      if(LOWORD(wParam) == IDOK)
      {
        OPTDLG_close(__this) ;
        return TRUE;
      }
      else if(LOWORD(wParam) == IDCANCEL)
      {
        OPTDLG_close(__this) ;
        return TRUE ;
      }
    }
    break ;

  case WM_SIZING:
    GetClientRect(hDlg,&crect);
    create_font(__this, crect.bottom-8, 0);
    return TRUE;

  case WM_SIZE:
    if(wParam == SIZE_RESTORED)
    {
      GetClientRect(hDlg,&crect);
      create_font(__this, crect.bottom-8, 0);
    }

  default:
    break; 
	}

	return FALSE;
}

void OPTDLG_close(OPTDLG *__this)
{
  if(__this->dialog)
  {
    KillTimer(__this->dialog, 1);
    RemoveProp(__this->dialog, "INFO") ;
    DestroyWindow(__this->dialog) ;
    __this->dialog = NULL ;
 
    DeleteDC(__this->hDC);
    DeleteObject((HGDIOBJ)__this->hFont);
    DeleteObject((HGDIOBJ)__this->hBitmap);
  }
}

static void setDefaultGuiFont(HWND hWnd)
{
  SendMessage(hWnd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0));    
}

void OPTDLG_open(OPTDLG *__this, HWND hParent, HINSTANCE hInst)
{
  if(!__this->dialog)
  {
    RECT rect;
    __this->dialog = CreateDialog(hInst, MAKEINTRESOURCE(IDD_OPTDLG), hParent, dlgProc ) ;
    assert(__this->dialog) ;
    SetProp(__this->dialog, "OPT", __this) ;
    SetTimer(__this->dialog, 1, FRAME_PERIOD, NULL) ;
    __this->hDC = CreateCompatibleDC(GetDC(__this->dialog));
    __this->hBitmap = CreateCompatibleBitmap(__this->hDC, 80*10, 128);
    SelectObject(__this->hDC, (HGDIOBJ)__this->hBitmap);
    GetClientRect(__this->dialog,&rect);
    create_font(__this, rect.bottom-8, 0);
    {
      TEXTMETRIC tmr;
      GetTextMetrics(__this->hDC,&tmr);
      GetWindowRect(__this->dialog,&rect);
      SetWindowPos(__this->dialog,0,0,0,tmr.tmMaxCharWidth*20,rect.bottom-rect.top,SWP_NOMOVE|SWP_NOZORDER);
    }
  }
  ShowWindow(__this->dialog, SW_SHOW) ;
}

OPTDLG *OPTDLG_new(void)
{
  OPTDLG *ret ;

  if(!(ret = (OPTDLG *)malloc(sizeof(OPTDLG)))) return NULL ;
  ret->dialog = NULL ;
  ret->delay_in_ms = 0;
  ret->lyrics_pos = 0;
  ret->hFont = NULL;
  ret->hBitmap = NULL;
  ret->hDC = NULL;
  return ret;
}

void OPTDLG_delete(OPTDLG *__this)
{
  OPTDLG_close(__this) ;
  free(__this) ;
}