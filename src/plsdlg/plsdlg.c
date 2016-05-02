#include <windows.h>
#include <assert.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>
#if defined(_MSC_VER)
#include <crtdbg.h>
#if defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC 
#endif
#endif

#include "msxplug.h"
#include "config/config.h"
#include "rc/resource.h"
#include "plsdlg/plsdlg.h"
#include "winamp/frontend.h"

static UINT chks[4] = { IDC_PSGCHK, IDC_SCCCHK, IDC_OPLLCHK, IDC_OPLCHK } ;
static UINT lbldb[4] = { IDC_PSGDB, IDC_SCCDB, IDC_OPLLDB, IDC_OPLDB } ;
static UINT spins[4] = { IDC_PSGSPIN, IDC_SCCSPIN, IDC_OPLLSPIN, IDC_OPLSPIN } ;

static void set_time_text(HWND hWnd, int time)
{
  char buf[32] ;
  int h,m,s,ss = 0 ;

  if(time<0)
  {
    SetWindowText(hWnd,NULL) ;
    return ;
  }
  
  ss = (time%1000) / 10 ;
  time /= 1000 ;
  s = time%60 ;
  time /= 60 ;
  m = time%60 ;
  time /= 60 ;
  h = time ;
  
  if(h) sprintf(buf,"%02d:%02d:%02d",h,m,s) ;
  else sprintf(buf,"%02d:%02d",m,s) ;
  
  SetWindowText(hWnd, buf) ;

}

static void set_num_text(HWND hWnd, int val)
{
  char buf[16] ;

  sprintf(buf,"%d",val) ;
  SetWindowText(hWnd, buf) ;
}

static void set_db_text(HWND hWnd, int val)
{
  char buf[16] ;

  sprintf(buf,"%3.2fdB",(KSSPLAY_VOL_STEP*val)) ;
  SetWindowText(hWnd, buf) ;
}


static int clip(int min, int val, int max)
{
  if(max<val) return max ;
  else if(val<min) return min ;
  else return val ;
}

/* find item position from Winamp Playlist */
static int find_item(PLSDLG *plsdlg, char *text)
{
  int pos, max ;
  char *fn ;

  max = SendMessage(plsdlg->hWinamp,WM_WA_IPC,0,IPC_GETLISTLENGTH) - 1 ;
  for(pos=0;pos<=max;pos++)
  {
    fn = (char *)SendMessage(plsdlg->hWinamp,WM_WA_IPC,pos,IPC_GETPLAYLISTFILE) ;
    if(strcmp(text,fn)==0) break ;
  }
  if(pos>max) return 0 ;

  return pos ;
}

static void clean_pls(PLSDLG *plsdlg)
{
  int pos=SendMessage(plsdlg->hWinamp,WM_WA_IPC,0,IPC_GETLISTPOS) ; /* PUSH */
  SendMessage(plsdlg->hWinamp,WM_WA_IPC,plsdlg->pos,IPC_SETPLAYLISTPOS) ;
  SendMessage(plsdlg->hWinamp,WM_WA_IPC,(WPARAM)plsdlg->plsitem->filename,IPC_CHANGECURRENTFILE) ;
  SendMessage(plsdlg->hWinamp,WM_WA_IPC,pos,IPC_SETPLAYLISTPOS) ; /* POP */
}

static void update_item(PLSDLG *plsdlg, PLSITEM *item)
{
  char buf[_MAX_PATH] ;
  int i ;

  if(GetWindowText(GetDlgItem(plsdlg->hDialog, IDC_TITLE), buf, 256))
    PLSITEM_set_title(item,buf) ;
  else PLSITEM_set_title(item,NULL) ;

  if(GetWindowText(GetDlgItem(plsdlg->hDialog, IDC_SONG), buf, 8))
    item->song = atoi(buf) ; 
  else item->song = 0 ;
  
  /*
  GetWindowText(GetDlgItem(plsdlg->hDialog, IDC_FILENAME), buf, _MAX_PATH) ;
  PLSITEM_set_filename(item,buf) ;
  */

  if(GetWindowText(GetDlgItem(plsdlg->hDialog, IDC_PLAYTIME), buf, 16))
    item->time_in_ms = PPLS_get_time(buf,-1) ;
  else item->time_in_ms = -1 ;

  if(GetWindowText(GetDlgItem(plsdlg->hDialog, IDC_LOOPTIME), buf, 16))
    item->loop_in_ms = PPLS_get_time(buf,-1) ;
  else item->loop_in_ms = -1 ;

  if(GetWindowText(GetDlgItem(plsdlg->hDialog, IDC_FADETIME), buf, 16))
    item->fade_in_ms = PPLS_get_time(buf,-1) ;
  else item->fade_in_ms = -1 ;

  if(GetWindowText(GetDlgItem(plsdlg->hDialog, IDC_LOOP), buf, 8))
    item->loop_num = atoi(buf) ; 
  else item->loop_num = -1 ;

  for(i=0;i<4;i++)
  {
    if(IsDlgButtonChecked(plsdlg->hDialog, chks[i])==BST_CHECKED)
    {
      item->enable_vol[i] = 1 ;
      item->vol[i] = (char)SendMessage(GetDlgItem(plsdlg->hDialog,spins[i]), UDM_GETPOS, 0, 0) ;
    }
    else
    {
      item->enable_vol[i] = 0 ;
      item->vol[i] = 0 ;
    }
  }
}

static void write_pls(PLSDLG *plsdlg)
{
  static char buf[PLSITEM_PRINT_SIZE] ;
  int pos ;

  update_item(plsdlg, plsdlg->plsitem) ;
  pos=SendMessage(plsdlg->hWinamp,WM_WA_IPC,0,IPC_GETLISTPOS) ; /* PUSH */
  SendMessage(plsdlg->hWinamp,WM_WA_IPC,plsdlg->pos,IPC_SETPLAYLISTPOS) ;
  PLSITEM_print(plsdlg->plsitem,buf,NULL) ;
  SendMessage(plsdlg->hWinamp,WM_WA_IPC,(WPARAM)buf,IPC_CHANGECURRENTFILE) ;
  SendMessage(plsdlg->hWinamp,WM_WA_IPC,pos,IPC_SETPLAYLISTPOS) ; /* POP */

  return ;
}

static int set_item(PLSDLG *plsdlg, char *text)
{
  PLSITEM *plsitem ;

  assert(plsdlg) ;
  plsitem = PLSITEM_new(text) ;
  assert(plsitem) ;

  if(plsdlg->plsitem) PLSITEM_delete(plsdlg->plsitem) ;
  plsdlg->plsitem = plsitem ;
  PLSITEM_set_title(plsdlg->plsitem,
      (char *)SendMessage(plsdlg->hWinamp,WM_WA_IPC,plsdlg->pos,IPC_GETPLAYLISTTITLE)) ;

  PLSDLG_update(plsdlg) ;
  return 0 ;
}

static void enable_volbox(PLSDLG *plsdlg, int i)
{
  int vol = plsdlg->plsitem->vol[i] ;

  EnableWindow(GetDlgItem(plsdlg->hDialog, lbldb[i]), TRUE) ;
  EnableWindow(GetDlgItem(plsdlg->hDialog, spins[i]), TRUE) ;
  if(vol<0) vol = 0;
  set_db_text(GetDlgItem(plsdlg->hDialog, lbldb[i]), vol) ;
  SendMessage(GetDlgItem(plsdlg->hDialog,spins[i]),UDM_SETPOS, 0, MAKELONG((short)vol, 0)) ;
}

static void disable_volbox(PLSDLG *plsdlg, int i)
{
  EnableWindow(GetDlgItem(plsdlg->hDialog, lbldb[i]), FALSE) ;
  EnableWindow(GetDlgItem(plsdlg->hDialog, spins[i]), FALSE) ;
  SetWindowText(GetDlgItem(plsdlg->hDialog, lbldb[i]), "") ;
}

static int get_filename(HWND hWnd, char *buf, int max)
{
  OPENFILENAME ofn ;

  memset(&ofn,0,sizeof(OPENFILENAME)) ;
  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = hWnd ;
  ofn.lpstrFilter = "Playlist Files(*.pls)\0*.pls\0All Files(*.*)\0*.*\0\0";
  ofn.Flags = OFN_OVERWRITEPROMPT ; 
  ofn.lpstrFile = buf ;
  ofn.nMaxFile = max ;

  return GetSaveFileName(&ofn) ;
}

static void save_pls(PLSDLG *plsdlg)
{
  char plsfile[_MAX_PATH] = "" ;

  if(get_filename(plsdlg->hDialog, plsfile, _MAX_PATH))
  {
    char pls[PLSITEM_PRINT_SIZE] ;
    FILE *fp ;
    int i, max=SendMessage(plsdlg->hWinamp,WM_WA_IPC,0,IPC_GETLISTLENGTH) - 1 ;

    if((fp=fopen(plsfile,"w"))==NULL)
    {
      MessageBox(plsdlg->hDialog,"File can't open!","ERROR",MB_OK) ;
      return ;
    }

    fprintf(fp,"[playlist]\n") ;
    for(i=0;i<=max;i++)
    {
      PLSITEM *item ;
      item = PLSITEM_new((char *)SendMessage(plsdlg->hWinamp,WM_WA_IPC,i,IPC_GETPLAYLISTFILE)) ;
      PLSITEM_print(item,pls,plsfile) ;
      fprintf(fp,"File%d=%s\n",i+1,pls) ;
      PLSITEM_delete(item) ;
    }
    fprintf(fp,"NumberOfEntries=%d\n",i) ;
    fprintf(fp,"Version=2\n") ;
    fclose(fp) ;
  }
}

static void apply_all(PLSDLG *plsdlg)
{
  int i,sav, max ;

  i = MessageBox(plsdlg->hDialog,
    "New parameters will be applied to all items in playlists.\n\r"
    "(Empty parameters will be remained current value.)",
    "Notice",MB_OKCANCEL|MB_ICONWARNING) ;
  if(i==IDCANCEL) return ;

  update_item(plsdlg, plsdlg->plsitem) ;
  sav = SendMessage(plsdlg->hWinamp,WM_WA_IPC,0,IPC_GETLISTPOS) ;

  max=SendMessage(plsdlg->hWinamp,WM_WA_IPC,0,IPC_GETLISTLENGTH) - 1 ;

  for(i=0;i<=max;i++)
  {
    char buf[PLSITEM_PRINT_SIZE] ;
    int j ;
    PLSITEM *item ;
    item = PLSITEM_new((char *)SendMessage(plsdlg->hWinamp,WM_WA_IPC,i,IPC_GETPLAYLISTFILE)) ;

    /* Update available elements */
    if(plsdlg->plsitem->title)
      PLSITEM_set_title(item,plsdlg->plsitem->title);

    if(plsdlg->plsitem->time_in_ms>=0)
      item->time_in_ms = plsdlg->plsitem->time_in_ms ;

    if(plsdlg->plsitem->loop_in_ms>=0)
      item->loop_in_ms = plsdlg->plsitem->loop_in_ms;
    
    if(plsdlg->plsitem->fade_in_ms>=0)
      item->fade_in_ms = plsdlg->plsitem->fade_in_ms ;

    if(plsdlg->plsitem->loop_num>=0)
      item->loop_num = plsdlg->plsitem->loop_num ;

    for(j=0;j<4;j++)
    {
      if(plsdlg->plsitem->enable_vol[j])
      {
        item->enable_vol[j] = 1 ;
        item->vol[j] = plsdlg->plsitem->vol[j] ;
      }
    }

    /* Write playlist */
    SendMessage(plsdlg->hWinamp,WM_WA_IPC,i,IPC_SETPLAYLISTPOS) ;
    SendMessage(plsdlg->hWinamp,WM_WA_IPC,(WPARAM)PLSITEM_print(item,buf,NULL),IPC_CHANGECURRENTFILE) ;
    PLSITEM_delete(item) ;
  }

  SendMessage(plsdlg->hWinamp,WM_WA_IPC,sav,IPC_SETPLAYLISTPOS) ;
  PLSDLG_set_item(plsdlg,plsdlg->pos) ;
  SendMessage(plsdlg->hWinamp,WM_WA_IPC,0,IPC_REFRESHPLCACHE);

}


static void PLSDLG_set_multimode(PLSDLG *plsdlg, int mode)
{
  int i ;

  if(mode)
  {
    plsdlg->multi = 1 ;
    SetWindowText(GetDlgItem(plsdlg->hDialog,IDC_TITLE), "") ;
    SetWindowText(GetDlgItem(plsdlg->hDialog,IDC_PLAYTIME), "") ;
    SetWindowText(GetDlgItem(plsdlg->hDialog,IDC_LOOPTIME), "") ;
    SetWindowText(GetDlgItem(plsdlg->hDialog,IDC_FADETIME), "") ;
    SetWindowText(GetDlgItem(plsdlg->hDialog,IDC_LOOP), "") ;
    SetWindowText(GetDlgItem(plsdlg->hDialog,IDC_FILENAME), "") ;
    SetWindowText(GetDlgItem(plsdlg->hDialog,IDC_SONG),"**") ;
    SetWindowText(GetDlgItem(plsdlg->hDialog,IDC_POS),"ALL") ;
    EnableWindow(GetDlgItem(plsdlg->hDialog,IDC_FILENAME),FALSE) ;
    EnableWindow(GetDlgItem(plsdlg->hDialog,IDC_SONGSPIN),FALSE) ;
    EnableWindow(GetDlgItem(plsdlg->hDialog,IDC_PREV),FALSE) ;
    EnableWindow(GetDlgItem(plsdlg->hDialog,IDC_NEXT),FALSE) ;
    EnableWindow(GetDlgItem(plsdlg->hDialog,IDC_SYNC),FALSE) ;
    EnableWindow(GetDlgItem(plsdlg->hDialog,IDC_SEARCH),FALSE) ;
    EnableWindow(GetDlgItem(plsdlg->hDialog,IDC_RESET),FALSE) ;
    EnableWindow(GetDlgItem(plsdlg->hDialog,IDC_CLEAN),FALSE) ;
    for(i=0;i<4;i++)
    {
      disable_volbox(plsdlg,i) ;
      CheckDlgButton(plsdlg->hDialog,chks[i],BST_UNCHECKED) ;
    }
  }
  else
  {
    plsdlg->multi = 0 ;
    EnableWindow(GetDlgItem(plsdlg->hDialog,IDC_FILENAME),TRUE) ;
    EnableWindow(GetDlgItem(plsdlg->hDialog,IDC_SONGSPIN),TRUE) ;
    EnableWindow(GetDlgItem(plsdlg->hDialog,IDC_PREV),TRUE) ;
    EnableWindow(GetDlgItem(plsdlg->hDialog,IDC_NEXT),TRUE) ;
    EnableWindow(GetDlgItem(plsdlg->hDialog,IDC_SYNC),TRUE) ;
    EnableWindow(GetDlgItem(plsdlg->hDialog,IDC_SEARCH),TRUE) ;
    EnableWindow(GetDlgItem(plsdlg->hDialog,IDC_RESET),TRUE) ;
    EnableWindow(GetDlgItem(plsdlg->hDialog,IDC_CLEAN),TRUE) ;
  }
  PLSDLG_update(plsdlg) ;
}

static BOOL bn_clicked_event(HWND hDlg, UINT uIdc, PLSDLG *plsdlg)
{
  int i, max, time_in_ms ;
  char *fn ;

  if(uIdc==IDCANCEL)
  {
    PLSDLG_close(plsdlg) ;
    return TRUE ;
  }

  max=SendMessage(plsdlg->hWinamp,WM_WA_IPC,0,IPC_GETLISTLENGTH) - 1 ;
  if(max<0) return FALSE ;

  switch(uIdc)
  {
  case IDC_SAVE:
    save_pls(plsdlg) ;
    return TRUE ;

  case IDC_SEARCH:
    if(plsdlg->pos<=max)
      MSXPLUG_play2(plsdlg->pos, 2) ;
    return TRUE ;

  case IDC_REFRESH:
    SendMessage(plsdlg->hWinamp,WM_WA_IPC,0,IPC_REFRESHPLCACHE);
    return TRUE ;

  case IDC_PLAY:
    if(plsdlg->pos>max) return TRUE ;
    SendMessage(plsdlg->hWinamp,WM_WA_IPC,plsdlg->pos,IPC_SETPLAYLISTPOS) ;
    PostMessage(plsdlg->hWinamp, WM_COMMAND, WINAMP_BUTTON2, 0) ;
    return TRUE ;

  case IDC_STOP:
    if(plsdlg->pos>max) return TRUE ;
    PostMessage(plsdlg->hWinamp, WM_COMMAND, WINAMP_BUTTON4, 0) ;
    return TRUE ;

  case IDC_UPDATE:
    if(plsdlg->multi) apply_all(plsdlg) ; else write_pls(plsdlg) ;
    return TRUE ;

  case IDC_RESET:
    plsdlg->pos = clip(0,plsdlg->pos,max) ;
    fn = (char *)SendMessage(plsdlg->hWinamp,WM_WA_IPC,plsdlg->pos,IPC_GETPLAYLISTFILE) ;
    set_item(plsdlg,fn) ;
    return TRUE ;

  case IDC_CLEAN:
    if(MessageBox(hDlg,"All information of the current item will be removed.","Notice",MB_OKCANCEL)==IDOK)
    {
      if(plsdlg->pos <= max) clean_pls(plsdlg) ; else plsdlg->pos = 0 ;
      PLSDLG_set_item(plsdlg,plsdlg->pos) ;
    }
    return TRUE ;
    
  case IDC_PREV:
  case IDC_NEXT:
    if(uIdc==IDC_PREV) plsdlg->pos--; else plsdlg->pos++ ;
    plsdlg->pos = clip(0,plsdlg->pos,max) ;
    fn = (char *)SendMessage(plsdlg->hWinamp,WM_WA_IPC,plsdlg->pos,IPC_GETPLAYLISTFILE) ;
    set_item(plsdlg,fn) ;
    return TRUE ;

  case IDC_PLAYTIME_NOW:
    time_in_ms = SendMessage(plsdlg->hWinamp,WM_WA_IPC,0,IPC_GETOUTPUTTIME);
    set_time_text(GetDlgItem(hDlg,IDC_PLAYTIME), time_in_ms) ;
    return TRUE ;

  case IDC_PSGCHK:
  case IDC_SCCCHK:
  case IDC_OPLLCHK:
  case IDC_OPLCHK:
    for(i=0;i<4;i++)
    {
      if(chks[i]==uIdc)
      {
        if(IsDlgButtonChecked(hDlg,chks[i])==BST_UNCHECKED) disable_volbox(plsdlg,i) ;
        else enable_volbox(plsdlg,i) ;
        return TRUE ;
      }
    }
    break ;

  case IDC_SYNC:
    if(IsDlgButtonChecked(hDlg, IDC_SYNC)==BST_CHECKED)
    {
      plsdlg->sync = 1 ;
      PLSDLG_set_item(plsdlg,-1) ;
    }
    else plsdlg->sync = 0 ;
    return TRUE ;

  case IDC_MULTI:
    if(IsDlgButtonChecked(hDlg, IDC_MULTI)==BST_CHECKED)
    {
      PLSDLG_set_multimode(plsdlg,1) ;
    }
    else
    {
      PLSDLG_set_multimode(plsdlg,0) ;
    }
    return TRUE ;
    
  default:
    break ;
  }

  return FALSE ;
}

static BOOL spin_event(HWND hDlg, HWND hSpin, PLSDLG *plsdlg)
{
  int pos ;
  
  pos = SendMessage(hSpin, UDM_GETPOS, 0, 0) ;

  if(hSpin == GetDlgItem(hDlg, IDC_PSGSPIN))
    set_db_text(GetDlgItem(hDlg, IDC_PSGDB), (char)pos) ;
  else if(hSpin == GetDlgItem(hDlg, IDC_SCCSPIN))
    set_db_text(GetDlgItem(hDlg, IDC_SCCDB), (char)pos) ;
  else if(hSpin == GetDlgItem(hDlg, IDC_OPLLSPIN))
    set_db_text(GetDlgItem(hDlg, IDC_OPLLDB), (char)pos) ;
  else if(hSpin == GetDlgItem(hDlg, IDC_OPLSPIN))
    set_db_text(GetDlgItem(hDlg, IDC_OPLDB), (char)pos) ;
  else return FALSE ;

  return TRUE ;
}

static BOOL CALLBACK dlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  PLSDLG *plsdlg ;
  int pos ;
  plsdlg = (PLSDLG *)GetProp(hDlg,"PLSDLG") ;

  switch(uMsg)
  {
  case WM_INITDIALOG:
    return TRUE ;

  case WM_DESTROY:
    return TRUE ;

  case WM_VSCROLL:
    if(spin_event(hDlg, (HWND)lParam, plsdlg)) return TRUE ;
    break ;
  
  case WM_COMMAND:
    if(HIWORD(wParam)==BN_CLICKED) return bn_clicked_event(hDlg,LOWORD(wParam),plsdlg) ;
    break ;

  case WM_TIMER:
    pos=SendMessage(plsdlg->hWinamp,WM_WA_IPC,0,IPC_GETLISTPOS) ;
    if((plsdlg->pos == pos)&&(!plsdlg->multi))
      EnableWindow(GetDlgItem(hDlg, IDC_PLAYTIME_NOW), TRUE) ;
    else
      EnableWindow(GetDlgItem(hDlg, IDC_PLAYTIME_NOW), FALSE) ;

    return TRUE ;

  default:
    break ;
  }

  return FALSE ;
}

PLSDLG *PLSDLG_new(void)
{
  PLSDLG *plsdlg ;
  
  if((plsdlg=(PLSDLG *)malloc(sizeof(PLSDLG)))==NULL) return NULL ;
  plsdlg->hWinamp = NULL ;
  plsdlg->hDialog = NULL ;
  plsdlg->plsitem = NULL ;
  plsdlg->sync = 1 ;
  plsdlg->multi = 0 ;
  return plsdlg ;
}


void PLSDLG_set_item(PLSDLG *plsdlg, int pos)
{
  char *text ;
  int max = SendMessage(plsdlg->hWinamp,WM_WA_IPC,0,IPC_GETLISTLENGTH) - 1 ;

  if(max<0) return ;
  if(pos<0) pos=SendMessage(plsdlg->hWinamp,WM_WA_IPC,0,IPC_GETLISTPOS) ;

  text = (char *)SendMessage(plsdlg->hWinamp,WM_WA_IPC,pos,IPC_GETPLAYLISTFILE) ;
  plsdlg->pos = pos ;
  (void)set_item(plsdlg, text) ;
}

void PLSDLG_update(PLSDLG *plsdlg)
{
  int i ;

  assert(plsdlg) ;
  if(plsdlg->hDialog&&plsdlg->plsitem&&!plsdlg->multi)
  {
    SetWindowText(GetDlgItem(plsdlg->hDialog,IDC_TITLE), plsdlg->plsitem->title) ;
    SetWindowText(GetDlgItem(plsdlg->hDialog,IDC_FILENAME), plsdlg->plsitem->filename) ;
    set_num_text(GetDlgItem(plsdlg->hDialog,IDC_SONG), plsdlg->plsitem->song) ;
    set_num_text(GetDlgItem(plsdlg->hDialog,IDC_POS), plsdlg->pos+1) ;
    if(plsdlg->plsitem->loop_num>=0)
    {
      set_num_text(GetDlgItem(plsdlg->hDialog,IDC_LOOP), plsdlg->plsitem->loop_num) ;
      SendMessage(GetDlgItem(plsdlg->hDialog,IDC_LOOPSPIN),UDM_SETPOS, 0, MAKELONG((short)plsdlg->plsitem->loop_num, 0)) ;
    }
    else
    {
      SetWindowText(GetDlgItem(plsdlg->hDialog,IDC_LOOP), "") ;
      SendMessage(GetDlgItem(plsdlg->hDialog,IDC_LOOPSPIN),UDM_SETPOS, 0, MAKELONG((short)0, 0)) ;
    }

    set_time_text(GetDlgItem(plsdlg->hDialog,IDC_PLAYTIME), plsdlg->plsitem->time_in_ms) ;
    set_time_text(GetDlgItem(plsdlg->hDialog,IDC_LOOPTIME), plsdlg->plsitem->loop_in_ms) ;
    set_time_text(GetDlgItem(plsdlg->hDialog,IDC_FADETIME), plsdlg->plsitem->fade_in_ms) ;
    
    for(i=0;i<4;i++)
    {
      SendMessage(GetDlgItem(plsdlg->hDialog,spins[i]), UDM_SETRANGE, TRUE, MAKELONG(255,-255)) ;
      if(plsdlg->plsitem->enable_vol[i])
      {
        CheckDlgButton(plsdlg->hDialog,chks[i],BST_CHECKED) ;
        enable_volbox(plsdlg,i) ;
      }
      else
      {
        CheckDlgButton(plsdlg->hDialog,chks[i],BST_UNCHECKED) ;
        disable_volbox(plsdlg,i) ;
      }
    }

    if(plsdlg->sync)
      CheckDlgButton(plsdlg->hDialog, IDC_SYNC, BST_CHECKED) ;
    else
      CheckDlgButton(plsdlg->hDialog, IDC_SYNC, BST_UNCHECKED) ;

    if(plsdlg->multi)
      CheckDlgButton(plsdlg->hDialog, IDC_MULTI, BST_CHECKED) ;
    else
      CheckDlgButton(plsdlg->hDialog, IDC_MULTI, BST_UNCHECKED) ;
  }
}

static void setDefaultGuiFont(HWND hWnd)
{
  SendMessage(hWnd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0));    
}

void PLSDLG_open(PLSDLG *plsdlg,HINSTANCE hInst, HWND hWinamp)
{
  if(!plsdlg->hDialog)
  {
    plsdlg->hWinamp = hWinamp ;
    plsdlg->hDialog = CreateDialog(hInst,MAKEINTRESOURCE(IDD_PLSDLG), hWinamp, dlgProc) ;
    assert(plsdlg->hDialog) ;
    SetProp(plsdlg->hDialog,"PLSDLG",plsdlg) ;
    plsdlg->hPrev=(HICON)LoadImage(hInst,MAKEINTRESOURCE(IDI_PREV),IMAGE_ICON,16,16,LR_DEFAULTCOLOR);
    plsdlg->hPlay=(HICON)LoadImage(hInst,MAKEINTRESOURCE(IDI_PLAY),IMAGE_ICON,16,16,LR_DEFAULTCOLOR);
    plsdlg->hStop=(HICON)LoadImage(hInst,MAKEINTRESOURCE(IDI_STOP),IMAGE_ICON,16,16,LR_DEFAULTCOLOR);
    plsdlg->hNext=(HICON)LoadImage(hInst,MAKEINTRESOURCE(IDI_NEXT),IMAGE_ICON,16,16,LR_DEFAULTCOLOR);
    plsdlg->hLogo=(HICON)LoadImage(hInst,MAKEINTRESOURCE(IDI_MSXPLUG),IMAGE_ICON,77,18,LR_DEFAULTCOLOR);
    SendDlgItemMessage(plsdlg->hDialog,IDC_PREV,BM_SETIMAGE,IMAGE_ICON,(LPARAM)plsdlg->hPrev) ;
    SendDlgItemMessage(plsdlg->hDialog,IDC_PLAY,BM_SETIMAGE,IMAGE_ICON,(LPARAM)plsdlg->hPlay) ;
    SendDlgItemMessage(plsdlg->hDialog,IDC_STOP,BM_SETIMAGE,IMAGE_ICON,(LPARAM)plsdlg->hStop) ;
    SendDlgItemMessage(plsdlg->hDialog,IDC_NEXT,BM_SETIMAGE,IMAGE_ICON,(LPARAM)plsdlg->hNext) ;
    SendDlgItemMessage(plsdlg->hDialog,IDC_LOGO,STM_SETIMAGE,IMAGE_ICON,(LPARAM)plsdlg->hLogo) ;
    SendMessage(GetDlgItem(plsdlg->hDialog,IDC_LOOPSPIN), UDM_SETRANGE, TRUE, MAKELONG(255,0)) ;
    SendMessage(GetDlgItem(plsdlg->hDialog,IDC_SONGSPIN), UDM_SETRANGE, TRUE, MAKELONG(255,0)) ;
    SetTimer(plsdlg->hDialog, 1, 200, NULL) ;
    plsdlg->multi = 0 ;
    PLSDLG_set_multimode(plsdlg,plsdlg->multi) ;
    setDefaultGuiFont(GetDlgItem(plsdlg->hDialog,IDC_TITLE)) ;
    setDefaultGuiFont(GetDlgItem(plsdlg->hDialog,IDC_FILENAME)) ;
    setDefaultGuiFont(GetDlgItem(plsdlg->hDialog,IDC_MGSTEXT)) ;
  }
  ShowWindow(plsdlg->hDialog, SW_SHOW) ;
}

void PLSDLG_close(PLSDLG *plsdlg)
{
  assert(plsdlg) ;
  if(plsdlg->hDialog)
  {
    KillTimer(plsdlg->hDialog, 1) ;
    RemoveProp(plsdlg->hDialog,"PLSDLG") ; 
    DestroyWindow(plsdlg->hDialog) ;
    DestroyIcon(plsdlg->hPrev) ;
    DestroyIcon(plsdlg->hPlay) ;
    DestroyIcon(plsdlg->hStop) ;
    DestroyIcon(plsdlg->hNext) ;
    DestroyIcon(plsdlg->hLogo) ;
    plsdlg->hDialog = NULL ;
  }
}

void PLSDLG_delete(PLSDLG *plsdlg)
{
  if(plsdlg)
  {
    PLSDLG_close(plsdlg) ;
    if(plsdlg->plsitem) PLSITEM_delete(plsdlg->plsitem) ;
    free(plsdlg) ;
  }
}
