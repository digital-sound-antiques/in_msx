#include <stdio.h>
#include <windows.h>
#include <assert.h>
#include "msxplug.h"
#include "kssplay.h"
#include "config/config.h"
#include "maskdlg.h"
#include "rc/resource.h"

static void update_t(MASKDLG *dlg, UINT cid, UINT dev, UINT mask)
{
  if(IsDlgButtonChecked(dlg->dialog, cid) == BST_UNCHECKED)
    dlg->cfg->mask[dev] |= mask;
  else
    dlg->cfg->mask[dev] &= ~mask;
}
static void update_f(MASKDLG *dlg, UINT cid, UINT dev, UINT mask)
{
  CheckDlgButton(dlg->dialog,cid,dlg->cfg->mask[dev]&mask?BST_UNCHECKED:BST_CHECKED);
}

static void update(MASKDLG *dlg, BOOL b)
{
  if(b) {
    update_t(dlg,IDC_PSG1,EDSC_PSG,PSG_MASK_CH(0));
    update_t(dlg,IDC_PSG2,EDSC_PSG,PSG_MASK_CH(1));
    update_t(dlg,IDC_PSG3,EDSC_PSG,PSG_MASK_CH(2));

    update_t(dlg,IDC_SCC1,EDSC_SCC,SCC_MASK_CH(0));
    update_t(dlg,IDC_SCC2,EDSC_SCC,SCC_MASK_CH(1));
    update_t(dlg,IDC_SCC3,EDSC_SCC,SCC_MASK_CH(2));
    update_t(dlg,IDC_SCC4,EDSC_SCC,SCC_MASK_CH(3));
    update_t(dlg,IDC_SCC5,EDSC_SCC,SCC_MASK_CH(4));

    update_t(dlg,IDC_OPLL1,EDSC_OPLL,OPLL_MASK_CH(0));
    update_t(dlg,IDC_OPLL2,EDSC_OPLL,OPLL_MASK_CH(1));
    update_t(dlg,IDC_OPLL3,EDSC_OPLL,OPLL_MASK_CH(2));
    update_t(dlg,IDC_OPLL4,EDSC_OPLL,OPLL_MASK_CH(3));
    update_t(dlg,IDC_OPLL5,EDSC_OPLL,OPLL_MASK_CH(4));
    update_t(dlg,IDC_OPLL6,EDSC_OPLL,OPLL_MASK_CH(5));
    update_t(dlg,IDC_OPLL7,EDSC_OPLL,OPLL_MASK_CH(6));
    update_t(dlg,IDC_OPLL8,EDSC_OPLL,OPLL_MASK_CH(7));
    update_t(dlg,IDC_OPLL9,EDSC_OPLL,OPLL_MASK_CH(8));
    update_t(dlg,IDC_OPLL_BD,EDSC_OPLL,OPLL_MASK_BD);
    update_t(dlg,IDC_OPLL_SD,EDSC_OPLL,OPLL_MASK_SD);
    update_t(dlg,IDC_OPLL_HH,EDSC_OPLL,OPLL_MASK_HH);
    update_t(dlg,IDC_OPLL_TOM,EDSC_OPLL,OPLL_MASK_TOM);
    update_t(dlg,IDC_OPLL_CYM,EDSC_OPLL,OPLL_MASK_CYM);
    //update_t(dlg,(ID_OPLL_PCM,EDSC_OPLL,OPLL_MASK_PCM);

    update_t(dlg,IDC_OPL1,EDSC_OPL,OPL_MASK_CH(0));
    update_t(dlg,IDC_OPL2,EDSC_OPL,OPL_MASK_CH(1));
    update_t(dlg,IDC_OPL3,EDSC_OPL,OPL_MASK_CH(2));
    update_t(dlg,IDC_OPL4,EDSC_OPL,OPL_MASK_CH(3));
    update_t(dlg,IDC_OPL5,EDSC_OPL,OPL_MASK_CH(4));
    update_t(dlg,IDC_OPL6,EDSC_OPL,OPL_MASK_CH(5));
    update_t(dlg,IDC_OPL7,EDSC_OPL,OPL_MASK_CH(6));
    update_t(dlg,IDC_OPL8,EDSC_OPL,OPL_MASK_CH(7));
    update_t(dlg,IDC_OPL9,EDSC_OPL,OPL_MASK_CH(8));
    update_t(dlg,IDC_OPL_BD,EDSC_OPL,OPL_MASK_BD);
    update_t(dlg,IDC_OPL_SD,EDSC_OPL,OPL_MASK_SD);
    update_t(dlg,IDC_OPL_HH,EDSC_OPL,OPL_MASK_HH);
    update_t(dlg,IDC_OPL_TOM,EDSC_OPL,OPL_MASK_TOM);
    update_t(dlg,IDC_OPL_CYM,EDSC_OPL,OPL_MASK_CYM);
    update_t(dlg,IDC_OPL_PCM,EDSC_OPL,OPL_MASK_ADPCM);
    dlg->cfg->mask_update = 1;
  }
  else {
    update_f(dlg,IDC_PSG1,EDSC_PSG,PSG_MASK_CH(0));
    update_f(dlg,IDC_PSG2,EDSC_PSG,PSG_MASK_CH(1));
    update_f(dlg,IDC_PSG3,EDSC_PSG,PSG_MASK_CH(2));
    update_f(dlg,IDC_SCC1,EDSC_SCC,SCC_MASK_CH(0));
    update_f(dlg,IDC_SCC2,EDSC_SCC,SCC_MASK_CH(1));
    update_f(dlg,IDC_SCC3,EDSC_SCC,SCC_MASK_CH(2));
    update_f(dlg,IDC_SCC4,EDSC_SCC,SCC_MASK_CH(3));
    update_f(dlg,IDC_SCC5,EDSC_SCC,SCC_MASK_CH(4));
    update_f(dlg,IDC_OPLL1,EDSC_OPLL,OPLL_MASK_CH(0));
    update_f(dlg,IDC_OPLL2,EDSC_OPLL,OPLL_MASK_CH(1));
    update_f(dlg,IDC_OPLL3,EDSC_OPLL,OPLL_MASK_CH(2));
    update_f(dlg,IDC_OPLL4,EDSC_OPLL,OPLL_MASK_CH(3));
    update_f(dlg,IDC_OPLL5,EDSC_OPLL,OPLL_MASK_CH(4));
    update_f(dlg,IDC_OPLL6,EDSC_OPLL,OPLL_MASK_CH(5));
    update_f(dlg,IDC_OPLL7,EDSC_OPLL,OPLL_MASK_CH(6));
    update_f(dlg,IDC_OPLL8,EDSC_OPLL,OPLL_MASK_CH(7));
    update_f(dlg,IDC_OPLL9,EDSC_OPLL,OPLL_MASK_CH(8));
    update_f(dlg,IDC_OPLL_BD,EDSC_OPLL,OPLL_MASK_BD);
    update_f(dlg,IDC_OPLL_SD,EDSC_OPLL,OPLL_MASK_SD);
    update_f(dlg,IDC_OPLL_HH,EDSC_OPLL,OPLL_MASK_HH);
    update_f(dlg,IDC_OPLL_TOM,EDSC_OPLL,OPLL_MASK_TOM);
    update_f(dlg,IDC_OPLL_CYM,EDSC_OPLL,OPLL_MASK_CYM);
    /*update_f(dlg,(ID_OPLL_PCM,EDSC_OPLL,OPLL_MASK_PCM);*/
    update_f(dlg,IDC_OPL1,EDSC_OPL,OPL_MASK_CH(0));
    update_f(dlg,IDC_OPL2,EDSC_OPL,OPL_MASK_CH(1));
    update_f(dlg,IDC_OPL3,EDSC_OPL,OPL_MASK_CH(2));
    update_f(dlg,IDC_OPL4,EDSC_OPL,OPL_MASK_CH(3));
    update_f(dlg,IDC_OPL5,EDSC_OPL,OPL_MASK_CH(4));
    update_f(dlg,IDC_OPL6,EDSC_OPL,OPL_MASK_CH(5));
    update_f(dlg,IDC_OPL7,EDSC_OPL,OPL_MASK_CH(6));
    update_f(dlg,IDC_OPL8,EDSC_OPL,OPL_MASK_CH(7));
    update_f(dlg,IDC_OPL9,EDSC_OPL,OPL_MASK_CH(8));
    update_f(dlg,IDC_OPL_BD,EDSC_OPL,OPL_MASK_BD);
    update_f(dlg,IDC_OPL_SD,EDSC_OPL,OPL_MASK_SD);
    update_f(dlg,IDC_OPL_HH,EDSC_OPL,OPL_MASK_HH);
    update_f(dlg,IDC_OPL_TOM,EDSC_OPL,OPL_MASK_TOM);
    update_f(dlg,IDC_OPL_CYM,EDSC_OPL,OPL_MASK_CYM);
    update_f(dlg,IDC_OPL_PCM,EDSC_OPL,OPL_MASK_ADPCM);
  }
}

static BOOL CALLBACK dlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  int i;
  MASKDLG *dlg = (MASKDLG *)GetProp(hDlg, "INFO") ;

  switch (uMsg)
	{
  case WM_INITDIALOG:
		return TRUE;

  case WM_COMMAND:
    
    if(HIWORD(wParam) == BN_CLICKED)
    {
      if(LOWORD(wParam) == IDOK)
      {
        MASKDLG_close(dlg) ;
        return TRUE;
      }
      else if(LOWORD(wParam) == IDCANCEL)
      {
        MASKDLG_close(dlg) ;
        return TRUE ;
      }
      else if(LOWORD(wParam) == IDC_REVERSE)
      {
        for(i=IDC_PSG1;i<=IDC_OPLL_PCM;i++) 
        {
          if(IsDlgButtonChecked(hDlg,i)==BST_UNCHECKED)
            CheckDlgButton(hDlg,i,BST_CHECKED);
          else
            CheckDlgButton(hDlg,i,BST_UNCHECKED);
        }
        update(dlg,TRUE);
        return TRUE;
      }
      else if(LOWORD(wParam) == IDC_INIT)
      {
        for(i=IDC_PSG1;i<=IDC_OPLL_PCM;i++) 
          CheckDlgButton(hDlg,i,BST_CHECKED);
        update(dlg,TRUE);
      }
      else if(IDC_PSG1<=LOWORD(wParam)&&LOWORD(wParam)<=IDC_OPLL_PCM)
      {
        update(dlg,TRUE);
        return TRUE;
      }
    }
    break ;

  default:
    break; 
	}

	return FALSE;
}

void MASKDLG_close(MASKDLG *dlg)
{
  if(dlg->dialog)
  {
    RemoveProp(dlg->dialog, "INFO") ;
    DestroyWindow(dlg->dialog) ;
    dlg->dialog = NULL ;
  }
}

void MASKDLG_open(MASKDLG *dlg, HWND hParent, HINSTANCE hInst)
{
  if(!dlg->dialog)
  {
    dlg->dialog = CreateDialog(hInst, MAKEINTRESOURCE(IDD_MASKDLG), hParent, dlgProc ) ;
    assert(dlg->dialog) ;
    SetProp(dlg->dialog, "INFO", dlg) ;
    update(dlg,FALSE);
  }
  ShowWindow(dlg->dialog, SW_SHOW) ;
}

MASKDLG *MASKDLG_new(CONFIG *cfg)
{
  MASKDLG *dlg ;

  if(!(dlg = (MASKDLG *)malloc(sizeof(MASKDLG)))) return NULL ;
  dlg->dialog = NULL ;

  dlg->cfg = cfg;

  return dlg ;
}

void MASKDLG_delete(MASKDLG *dlg)
{
  MASKDLG_close(dlg) ;
  free(dlg) ;
}
