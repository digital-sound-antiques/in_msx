#include <stdio.h>
#include <malloc.h>
#include <windows.h>
#include <commctrl.h>
#include <assert.h>
#include <crtdbg.h>

#include "controls/control.h"
#include "edit2413.h"

static char *LABEL[ENDPARAM] = { "TL","FB", "EG", "ML", "AR","DR","SL","RR","KR", "KL", "AM","PM","WF"} ;
static int PARAMAX[ENDPARAM] = { 63 , 7 , 1, 15, 15, 15, 15, 15, 1, 3, 1, 1, 1 } ;
static OPLL_PATCH dummy_patch ;

static int read_patch_param(OPLL_PATCH *patch, int param)
{
  switch(param)
  {
  case TL: return patch->TL ;
  case FB: return patch->FB ;
  case EG: return patch->EG ;
  case ML: return patch->ML ;
  case AR: return patch->AR ;
  case DR: return patch->DR ;
  case SL: return patch->SL ;
  case RR: return patch->RR ;
  case KR: return patch->KR ;
  case KL: return patch->KL ;
  case AM: return patch->AM ;
  case PM: return patch->PM ;
  case WS: return patch->WS ;
  default: return 0 ;
  }
}

static void write_patch_param(OPLL_PATCH *patch, int param, int value)
{
  switch(param)
  {
  case TL: patch->TL = value ; break ;
  case FB: patch->FB = value ; break ;
  case EG: patch->EG = value ; break ;
  case ML: patch->ML = value ; break ;
  case AR: patch->AR = value ; break ; 
  case DR: patch->DR = value ; break ;
  case SL: patch->SL = value ; break ;
  case RR: patch->RR = value ; break ;
  case KR: patch->KR = value ; break ;
  case KL: patch->KL = value ; break ;
  case AM: patch->AM = value ; break ;
  case PM: patch->PM = value ; break ;
  case WS: patch->WS = value ; break ;
  default: break ;
  }
}

static void update_page(EDIT2413 *edit2413)
{
  int i ;

  if(edit2413->cursel&1)
  {
    EnableWindow(edit2413->hParam[0], FALSE) ;
    EnableWindow(edit2413->hParam[1], FALSE) ;
  }
  else
  {
    EnableWindow(edit2413->hParam[0], TRUE) ;
    EnableWindow(edit2413->hParam[1], TRUE) ;
  }

  for(i=TL;i<ENDPARAM;i++)
  {
    if(edit2413->hParam[i]) 
      SendMessage(edit2413->hParam[i], WM_USER+1, read_patch_param(&edit2413->patch[edit2413->cursel],i), 0) ;
  }

  UpdateWindow(edit2413->hMain) ;
}

void EDIT2413_sync(EDIT2413 *edit2413)
{
  int i ;

  assert(edit2413) ;
  if(edit2413->opll)
    for(i=0;i<19*2;i++) memcpy(&edit2413->patch[i],&edit2413->opll->patch[i],sizeof(OPLL_PATCH)) ;
}

void EDIT2413_apply(EDIT2413 *edit2413)
{
  int i ;

  assert(edit2413) ;

  if(edit2413->opll)
    for(i=0;i<19*2;i++) memcpy(&edit2413->opll->patch[i],&edit2413->patch[i],sizeof(OPLL_PATCH)) ;

}


void EDIT2413_get_patch(EDIT2413 *edit2413, int num, OPLL_PATCH *patch)
{
  assert(edit2413&&patch) ;
  memcpy(patch, &edit2413->patch[num], sizeof(OPLL_PATCH)) ;
}

void EDIT2413_set_patch(EDIT2413 *edit2413, int num, OPLL_PATCH *patch)
{
  assert(edit2413) ;
  memcpy(&edit2413->patch[num], patch, sizeof(OPLL_PATCH)) ;
}

void EDIT2413_set_target(EDIT2413 *edit2413, OPLL *opll)
{
  assert(edit2413) ;

  edit2413->opll = opll ;
}

void EDIT2413_unset_target(EDIT2413 *edit2413)
{
  assert(edit2413) ; 
  edit2413->opll = NULL ;
}

static void reset_patch(EDIT2413 *edit2413)
{  
  int i ;
  for(i=0;i<19;i++) OPLL_getDefaultPatch(0,i,&edit2413->patch[i*2]) ;
}

static int get_illfilename(HWND hWnd, char *buf, int max, int mode)
{
  OPENFILENAME ofn ;

  memset(&ofn,0,sizeof(OPENFILENAME)) ;
  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = hWnd ;
  ofn.lpstrFilter = "ILL Files(*.ill)\0*.ill\0All Files(*.*)\0*.*\0\0";
  ofn.lpstrFile = buf ;
  ofn.nMaxFile = max ;

  if(mode)
  {
    ofn.Flags = OFN_FILEMUSTEXIST ;
    return GetOpenFileName(&ofn) ;
  }
  else
  {
    ofn.Flags = OFN_OVERWRITEPROMPT ;
    return GetSaveFileName(&ofn) ;
  }
}

enum { ITEM_DIR, ITEM_SLOT, ITEM_PATCH } ;

static BOOL CALLBACK dlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  int i ;
  int t, c ;
  char buf[_MAX_PATH]="", dump[16]="", text[256]="" ;

  EDIT2413 *edit2413 = (EDIT2413 *)GetProp(hDlg, "EDIT2413") ;

  switch(uMsg)
  {
  case WM_INITDIALOG:
    return TRUE ;

  case WM_CLOSE:
    EDIT2413_close(edit2413) ;
    return TRUE ;

  case WM_HSCROLL:
    for(i=TL;i<ENDPARAM;i++)
    {
      if(edit2413->hParam[i]==(HWND)lParam)
      {
        write_patch_param(&edit2413->patch[edit2413->cursel], i, wParam) ;
        if(edit2413->opll)
        {
          write_patch_param(&edit2413->opll->patch[edit2413->cursel], i, wParam) ;
          OPLL_forceRefresh(edit2413->opll) ;
        }
        return TRUE ;
      }
    }
    break ;

  case WM_COMMAND:
    if(HIWORD(wParam)==BN_CLICKED)
    {
      switch(LOWORD(wParam))
      {
      case IDC_RESET:
        reset_patch(edit2413) ;
        EDIT2413_apply(edit2413) ;
        update_page(edit2413) ;
        return TRUE ;

      case IDC_LOAD:
        if(get_illfilename(hDlg, buf, _MAX_PATH, 1))
        {
          EDIT2413_load(edit2413,buf) ;
          EDIT2413_apply(edit2413) ;
          update_page(edit2413) ;
        }
        return TRUE ;

      case IDC_SAVE:
        if(get_illfilename(hDlg, buf, _MAX_PATH, 0))
        {
          EDIT2413_save(edit2413,buf) ;
        }
        return TRUE ;
      default:
        break ;
      }
    }

    break ;

  case WM_TIMER:
    if((edit2413->cursel<2)&&(edit2413->opll))
    {
      memcpy(&edit2413->patch[edit2413->cursel],&edit2413->opll->patch[edit2413->cursel],sizeof(OPLL_PATCH)) ;
      update_page(edit2413) ;
    }
    return TRUE ;

  case WM_NOTIFY:
    switch(((LPNMHDR)lParam)->code)
    {
    case TVN_SELCHANGED:

      t = LOWORD((int)((LPNMTREEVIEW)lParam)->itemNew.lParam) ;
      c = HIWORD((int)((LPNMTREEVIEW)lParam)->itemNew.lParam) ;
      
      switch(t)
      {
      case ITEM_SLOT:
        edit2413->cursel = c ;
        update_page(edit2413) ;
        for(i=TL;i<ENDPARAM;i++) ShowWindow(edit2413->hParam[i], SW_SHOW) ;
        SetWindowText(GetDlgItem(hDlg, IDC_TONEINFO), NULL) ;
        break ;
     
      case ITEM_PATCH:
        for(i=TL;i<ENDPARAM;i++) ShowWindow(edit2413->hParam[i], SW_HIDE) ;
        OPLL_patch2dump(&edit2413->patch[c],(uint8_t *)dump) ;
        for(i=0;i<8;i++) sprintf(text+i*3, " %02X", (unsigned char)dump[i]) ;
        SetWindowText(GetDlgItem(hDlg, IDC_TONEINFO), text) ;
        break ;

      case ITEM_DIR:
        for(i=TL;i<ENDPARAM;i++) ShowWindow(edit2413->hParam[i], SW_HIDE) ;
        SetWindowText(GetDlgItem(hDlg, IDC_TONEINFO), NULL) ;
        break ;
      }
                
      break ;

    default:
      break ;
    }
    break ;

  case WM_DESTROY:
    return TRUE ;

  default:
    break;
  }
  return FALSE ;
}

struct localTree {
  char *text ;
  struct localTree *next ;
  struct localTree *has;
  int param ;
} ;


static void add_item(HWND hTree, HTREEITEM hParent, struct localTree *item)
{
  HTREEITEM hMe ;
  TV_INSERTSTRUCT tv;

  if(!hTree||!item) return ;
  tv.hParent = hParent ;
  tv.hInsertAfter = TVI_LAST;
  tv.item.mask = TVIF_TEXT | TVIF_PARAM ;
  tv.item.pszText = item->text ;
  tv.item.lParam = item->param ;
  hMe = TreeView_InsertItem(hTree, &tv);
  add_item(hTree, hMe, item->has) ;
  add_item(hTree, hParent, item->next) ;

  return ;
}

static void init_tree(HWND hTree)
{
  
  int i ;

  struct localTree op[34] ; 

  struct localTree inst[16] =
  {
    {"@00 User",&inst[1],&op[0],MAKELPARAM(ITEM_PATCH,0)} ,
    {"@01 Violin",&inst[2],&op[2],MAKELPARAM(ITEM_PATCH,2)} , 
    {"@02 Guitar",&inst[3],&op[4],MAKELPARAM(ITEM_PATCH,4)} ,
    {"@03 Piano",&inst[4],&op[6],MAKELPARAM(ITEM_PATCH,6)} ,
    {"@04 Flute",&inst[5],&op[8],MAKELPARAM(ITEM_PATCH,8)} ,
    {"@05 Clarinet",&inst[6],&op[10],MAKELPARAM(ITEM_PATCH,10)} ,
    {"@06 Oboe",&inst[7],&op[12],MAKELPARAM(ITEM_PATCH,12)} ,
    {"@07 Trumpet",&inst[8],&op[14],MAKELPARAM(ITEM_PATCH,14)} ,
    {"@08 Organ",&inst[9],&op[16],MAKELPARAM(ITEM_PATCH,16)} ,
    {"@09 Horn",&inst[10],&op[18],MAKELPARAM(ITEM_PATCH,18)} ,
    {"@10 Synth",&inst[11],&op[20],MAKELPARAM(ITEM_PATCH,20)} ,
    {"@11 Harpsicode",&inst[12],&op[22],MAKELPARAM(ITEM_PATCH,22)} ,
    {"@12 Vibraphone",&inst[13],&op[24],MAKELPARAM(ITEM_PATCH,24)} ,
    {"@13 Synth Bass",&inst[14],&op[26],MAKELPARAM(ITEM_PATCH,26)} ,
    {"@14 Wood Bass",&inst[15],&op[28],MAKELPARAM(ITEM_PATCH,28)} ,
    {"@15 Elec Bass",NULL,&op[30],MAKELPARAM(ITEM_PATCH,30)}
  } ;

  struct localTree rythm[16] = 
  {
    {"Bass Drum",&rythm[1],&op[32],MAKELPARAM(ITEM_PATCH,32)},
    {"Hi-Hat&Snare",&rythm[2],&rythm[3],MAKELPARAM(ITEM_PATCH,34)},
    {"Tom-Tom&Cymbal",NULL,&rythm[5],MAKELPARAM(ITEM_PATCH,36)},
    {"Hi-Hat",&rythm[4],NULL,MAKELPARAM(ITEM_SLOT,34)},
    {"Snare Drum",NULL,NULL,MAKELPARAM(ITEM_SLOT,35)},
    {"Tom-Tom",&rythm[6],NULL,MAKELPARAM(ITEM_SLOT,36)},
    {"Cymbal",NULL,NULL,MAKELPARAM(ITEM_SLOT,37)}
  } ;

  struct localTree root[2] = 
  {
    {"Melodic Instrument",&root[1],&inst[0],MAKELPARAM(ITEM_DIR,0)},
    {"Percussive Instrument",NULL,&rythm[0],MAKELPARAM(ITEM_DIR,0)},
  } ;

  for(i=0;i<34;i++)
  {
    if(i&1)
    {
      op[i].text = "Carrior" ;
      op[i].next = NULL ;
    }
    else
    {
      op[i].text = "Modulator" ;
      op[i].next = &op[i+1] ;
    }
    op[i].param = MAKELPARAM(ITEM_SLOT,i) ;
    op[i].has = NULL ;
  }

  add_item(hTree, TVI_ROOT, &root[0]) ;

  return;
}

EDIT2413 *EDIT2413_new(void)
{
  EDIT2413 *edit2413 ;

  if((edit2413=(EDIT2413 *)malloc(sizeof(EDIT2413)))==NULL) return NULL ;
  edit2413->opll = NULL ;
  edit2413->hMain = NULL ;
  reset_patch(edit2413) ;

  return edit2413 ;
}

void EDIT2413_open(EDIT2413 *edit2413, HWND hwndParent, HINSTANCE hInst)
{
  RECT rect ;
  RECT crect ;
  int i, x = 0 ,y = 0 ;
  
  if(edit2413)
  {
    if(edit2413->hMain==NULL)
    {
      edit2413->hMain = CreateDialog(hInst, MAKEINTRESOURCE(IDD_EDIT2413), hwndParent, dlgProc) ;
      SetWindowPos(GetDlgItem(edit2413->hMain,IDC_LOGO),NULL,0,0,57*2,23*2,SWP_NOMOVE) ;
      edit2413->cursel = 0 ;

      SetProp(edit2413->hMain, "EDIT2413", edit2413) ;

      GetClientRect(GetDlgItem(edit2413->hMain,IDC_VOICETREE), &rect) ;
      x = rect.right + 8 ;

      for(i=TL;i<=WS;i++)
      {
        edit2413->hParam[i] = CreateMixerControl(hInst, edit2413->hMain, LABEL[i], 0,PARAMAX[i]) ;
        SetWindowPos(edit2413->hParam[i],NULL,x,y,0,0,SWP_NOSIZE|SWP_NOZORDER) ; 
        GetWindowRect(edit2413->hParam[i], &rect) ;
        y += rect.bottom - rect.top - 1 ;
      }

      x += rect.right - rect.left - 1 ;
      GetWindowRect(edit2413->hMain, &rect) ;
      GetClientRect(edit2413->hMain, &crect) ;

      x += (rect.right - rect.left) - crect.right ;
      y += (rect.bottom - rect.top) - crect.bottom ;

      SetWindowPos(edit2413->hMain,NULL,0,0,x,y,SWP_NOZORDER|SWP_NOMOVE|SWP_NOOWNERZORDER) ;

      init_tree(GetDlgItem(edit2413->hMain, IDC_VOICETREE)) ;
      SetTimer(edit2413->hMain,1,100,NULL) ;
    }

    ShowWindow(edit2413->hMain, SW_SHOW) ;
  }
}

void EDIT2413_close(EDIT2413 *edit2413)
{
  if(edit2413)
  {
    KillTimer(edit2413->hMain,1) ;
    RemoveProp(edit2413->hMain, "EDIT2413") ;
    DestroyWindow(edit2413->hMain) ;
    edit2413->hMain = NULL;
  }
}

void EDIT2413_delete(EDIT2413 *edit2413)
{
  if(edit2413)
  {
    if(edit2413->hMain) EDIT2413_close(edit2413);
    free(edit2413) ;
  }
}

void EDIT2413_save(EDIT2413 *edit2413, char *filename)
{
  char illdata[16*19] = {'I','L','L','2',0,0,0,0, 0,0,0,0,0,0,0,0 } ;
  FILE *fp ;
  int i ;

  for(i=1;i<19;i++)
    OPLL_patch2dump(&edit2413->patch[i*2], (uint8_t *)illdata + 16*i) ;

  if((fp=fopen(filename,"wb"))==NULL) return ;
  fwrite(illdata, sizeof(char), 16*19, fp) ;
  fclose(fp) ;
}

void EDIT2413_load(EDIT2413 *edit2413, char *filename)
{
  char illdata[16*19] ;
  FILE *fp ;
  int i ;

  if((fp=fopen(filename,"rb"))==NULL) return ;
  fread(illdata, sizeof(char), 16*19, fp) ;
  fclose(fp) ;

  for(i=1;i<19;i++)
    OPLL_dump2patch((uint8_t *)illdata + 16*i, &edit2413->patch[i*2]) ;
}