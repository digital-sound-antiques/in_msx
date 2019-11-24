#include <stdio.h>
#include <windows.h>
#include <assert.h>
#include <commctrl.h>
#include <string.h>
#if defined(_DEBUG)
#include <crtdbg.h>
#define _CRTDBG_MAP_ALLOC 
#endif

#include "pls/ppls.h"
#include "pandlg/pandlg.h"
#include "config.h"

#define KSS_DESC "KSS\\KSS DATA (*.KSS)\\"
#define MGS_DESC "MGS\\MGSDRV (*.MGS)\\"
#define BGM_DESC "BGM\\MuSICA (*.BGM)\\BGR\\MuSICA (*.BGR)\\"
#define MBM_DESC "MBM\\MoonBlaster (*.MBM)\\"
#define MPK_DESC "MPK\\MPK (*.MPK)\\"
#define OPX_DESC "OPX\\OPLLDriver (*.MPK)\\"

In_Module * winampGetInModule2() ;

enum {STR_PARAM, INT_PARAM} ;
enum {FMT_NONE, FMT_DEC, FMT_DB, FMT_TIME, FMT_BOOL, FMT_PERC} ; 

static int str2num(char *p, int min, int max)
{
  int ret ;

  ret = atoi(p) ;
  if(ret<min) return min ;
  if(max<ret) return max ;
  return ret ;
}

static CONFIG_PARAM *find_param(CONFIG_PARAM **param_list, char *name)
{
  CONFIG_PARAM *p = param_list[HASH_KEY(name[0])];

  while(p)
  {
    if(strcmp(p->name, name)==0) break;
    p = p->next;
  }
  return p;
}

static CONFIG_PARAM *new_param(char *name, __int32 min, __int32 max)
{
  CONFIG_PARAM *p ;

  p = (CONFIG_PARAM *)malloc(sizeof(CONFIG_PARAM));
  if(p)
  {
    memset(p,0,sizeof(CONFIG_PARAM));
    if(!(p->name=(char *)malloc(strlen(name)+1))) goto Error_Exit;
    strcpy(p->name,name);
    p->max = max;
    p->min = min;
  }
  return p;

Error_Exit:
  free(p);
  return NULL;
}

static void set_string(CONFIG_PARAM *p, char *val)
{
  free(p->data);
  p->data = malloc(strlen(val)+1);
  if(p->data) strcpy((char *)p->data, val);
  else MessageBox(NULL,"OUTOFMEMORY","OUTOFMEMORY",MB_OK);
}

static void add_param(CONFIG *config, char *name, int type, int size, int format, __int32 min, __int32 max, void *def)
{
  CONFIG_PARAM *p;

  p = new_param(name, min, max);

  if(p)
  {
    p->type = type;
    p->format = format;
    p->size = size;
    if(size==0)
    {
      switch(p->type)
      {
      case STR_PARAM:
        set_string(p,(char *)def);
        break;

      case INT_PARAM:
        p->data = def;
        break;

      default:
        p->data = NULL;
        break;
      }
    }
    else /* Array */
    {
      switch(p->type)
      {
      case INT_PARAM:
        p->data = malloc(sizeof(__int32)*size);
        memcpy(p->data,def,sizeof(__int32)*size);
        break;
      default:
        p->data = NULL;
        break;
      }
    }
    p->next = config->param_list[HASH_KEY(name[0])];
    config->param_list[HASH_KEY(name[0])] = p;
  }

  return ;
}

static void delete_param(CONFIG_PARAM *p)
{
  if(p)
  {
    free(p->name);
    if((p->type==STR_PARAM)||(p->size)) free(p->data);
    free(p);
  }
}

void CONFIG_set_array_int(CONFIG *config, char *name, __int32 idx, __int32 val)
{
  CONFIG_PARAM *p;

  EnterCriticalSection(&config->cso);
  if(p=find_param(config->param_list, name))
  {
    if(idx<p->size) ((__int32 *)p->data)[idx] = val;
    else
    {
      MessageBox(NULL,name,"Error : Index overflow.", MB_OK);
    }
  }
  else
  {
    MessageBox(NULL,name,"Error : Can't find the parameter.", MB_OK);
  }
  LeaveCriticalSection(&config->cso);
}


__int32 CONFIG_get_array_int(CONFIG *config, char *name, __int32 idx)
{
  CONFIG_PARAM *p;

  if(p=find_param(config->param_list, name))
  {
    if(idx<p->size)
    {
      return ((__int32 *)p->data)[idx];
    }
    else
    {
      MessageBox(NULL,name,"Error : Index overflow.", MB_OK);
      return 0;
    }
  }
  else
  {
    MessageBox(NULL,name,"Error : Can't find the parameter.", MB_OK);
    return 0;
  }
}

void CONFIG_set_int(CONFIG *config, char *name, __int32 val)
{
  CONFIG_PARAM *p;

  EnterCriticalSection(&config->cso);
  if(p=find_param(config->param_list, name))
  {
    p->data = (void *)val;
  }
  LeaveCriticalSection(&config->cso);
}

void CONFIG_set_str(CONFIG *config, char *name, char *val)
{
  CONFIG_PARAM *p;

  EnterCriticalSection(&config->cso);
  if(p=find_param(config->param_list, name))
    set_string(p,val);
  LeaveCriticalSection(&config->cso);
}

char *CONFIG_get_str(CONFIG *config, char *name)
{
  CONFIG_PARAM *p;
  char *ret = NULL;

  EnterCriticalSection(&config->cso);
  if(p = find_param(config->param_list, name))
  {
    ret = (char *)p->data;
  }
  else
  {
    MessageBox(NULL,name,"Error : Can't find the parameter.", MB_OK);
  }
  LeaveCriticalSection(&config->cso);

  return ret;
}


__int32 CONFIG_get_int(CONFIG *config, char *name)
{
  CONFIG_PARAM *p;
  __int32 ret = 0;

  EnterCriticalSection(&config->cso);
  if(p = find_param(config->param_list, name))
  {
    ret = (__int32)p->data;
  }
  else
  {
    MessageBox(NULL,name,"Error : Can't find the parameter.", MB_OK);
  }
  LeaveCriticalSection(&config->cso);
  return ret;
}


CONFIG *CONFIG_new(char *path, char *section)
{
  CONFIG *config ;

  if(strlen(path)>=512) return NULL ;
  if(strlen(section)>=32) return NULL ;
  
  if((config = (CONFIG *)malloc(sizeof(CONFIG)))==NULL) return NULL ;

  memset(config,0,sizeof(CONFIG)) ;

  InitializeCriticalSection(&config->cso);
  /* Add parameters */
#define _add_param_int(name,format,min,max,def) add_param(config,name,INT_PARAM,0,format,min,max,(void *)def)
#define _add_param_str(name,min,max,def) add_param(config,name,STR_PARAM,0,FMT_NONE,min,max,(void *)def)
#define _add_array_int(name,format,min,max,def,size) add_param(config,name,INT_PARAM,size,format,min,max,(void *def));

  /*              NAME           FORMAT    MIN   MAX  DEFAULT */
  _add_param_int("OPENSETUP",  FMT_BOOL, 0,    1,    1);
  _add_param_int("RATE",       FMT_DEC, 11025,96000,44100) ;
  _add_param_int("STEREO",     FMT_DEC,  0,    2,    0) ;
  _add_param_int("BUFSIZE",    FMT_DEC, 576,  16384,2048) ;
  _add_param_int("LOOP",       FMT_DEC, 0,    512,  1) ;
  _add_param_int("PLAYTIME",   FMT_TIME, 0,    1<<24,300000) ;
  _add_param_int("FADETIME",   FMT_TIME, 0,    1<<24,5000) ;
  _add_param_int("CPUSPEED",   FMT_DEC, 0,    16,    0) ;
  _add_param_int("BPS",        FMT_DEC, 16,   16,   16);

  _add_param_int("ENABLE_KSS", FMT_BOOL, 0,    1,    1);
  _add_param_int("ENABLE_MGS", FMT_BOOL, 0,    1,    1);
  _add_param_int("ENABLE_BGM", FMT_BOOL, 0,    1,    1);
  _add_param_int("ENABLE_MPK", FMT_BOOL, 0,    1,    1);
  _add_param_int("ENABLE_OPX", FMT_BOOL, 0,    1,    1);
  _add_param_int("ENABLE_MBM", FMT_BOOL, 0,    1,    1);

  _add_param_int("MASTER_VOL", FMT_DB, KSSPLAY_VOL_MIN,KSSPLAY_VOL_MAX,0);
  _add_param_int("PSG_VOL",    FMT_DB, KSSPLAY_VOL_MIN,KSSPLAY_VOL_MAX,0) ;
  _add_param_int("SCC_VOL",    FMT_DB, KSSPLAY_VOL_MIN,KSSPLAY_VOL_MAX,0) ;
  _add_param_int("OPL_VOL",    FMT_DB, KSSPLAY_VOL_MIN,KSSPLAY_VOL_MAX,0) ;
  _add_param_int("OPLL_VOL",   FMT_DB, KSSPLAY_VOL_MIN,KSSPLAY_VOL_MAX,0) ;

  _add_param_int("PSG_PAN",   FMT_PERC,-256,255,0) ;
  _add_param_int("SCC_PAN",   FMT_PERC,-256,255,0) ;
  _add_param_int("OPL_PAN",   FMT_PERC,-256,255,0) ;
  _add_param_int("OPLL_PAN",  FMT_PERC,-256,255,0) ;
  _add_param_int("PSG_PAN_MIXER",   FMT_PERC,-256,255,0) ;
  _add_param_int("SCC_PAN_MIXER",   FMT_PERC,-256,255,0) ;
  _add_param_int("OPL_PAN_MIXER",   FMT_PERC,-256,255,0) ;
  _add_param_int("OPLL_PAN_MIXER",  FMT_PERC,-256,255,0) ;

  _add_param_int("PSG_MASK",  FMT_DEC, 0, (1<<24), 0);
  _add_param_int("SCC_MASK",  FMT_DEC, 0, (1<<24), 0);
  _add_param_int("OPL_MASK",  FMT_DEC, 0, (1<<24), 0);
  _add_param_int("OPLL_MASK",  FMT_DEC, 0, (1<<24), 0);

  _add_param_int("OVERWRITE_VOLUME", FMT_BOOL, 0,1,0) ;
  _add_param_int("AUTOSAVE_VOLUME",  FMT_BOOL, 0,1,1);

  _add_param_int("PSG_MODE",   FMT_DEC, 0,3,1);
  _add_param_int("SCC_MODE",   FMT_DEC, 0,3,1);
  _add_param_int("OPLL_MODE",  FMT_DEC, 0,2,0);

  _add_param_int("R", FMT_DEC, 0, 1<<24, 4700);
  _add_param_int("C", FMT_DEC, 0, 1<<24, 15);
  _add_param_int("ENABLE_RCF", FMT_DEC, 0, 1, 1);
  _add_param_int("ENABLE_DCF", FMT_DEC, 0, 1, 1);

  _add_param_int("PSG_HQ",     FMT_BOOL, 0,1,1);
  _add_param_int("SCC_HQ",     FMT_BOOL, 0,1,0);
  _add_param_int("OPLL_HQ",    FMT_BOOL, 0,1,0);

  _add_param_int("OPLL_STEREO",  FMT_BOOL,0,1,0);

  _add_param_int("MBM_MODE", FMT_DEC, 0, 2, 2);
  _add_param_int("MBM_ENABLE_STEREO", FMT_BOOL, 0, 1, 0);
  _add_param_int("MBM_SYNC", FMT_DEC, 0, 1, 1);
  _add_param_str("MBK_PATH",0,0,"");
  _add_param_str("MBK_DUMMY",0,0,"__DUMMY.MBK");

  _add_param_str("MGSDRV",0,0,"C:\\Program Files\\Winamp\\Plugins\\MGSDRV.COM");
  _add_param_str("KINROU",0,0,"C:\\Program Files\\Winamp\\Plugins\\KINROU5.DRV");
  _add_param_str("MPK103",0,0,"C:\\Program Files\\Winamp\\Plugins\\MPK103.BIN");
  _add_param_str("MPK106",0,0,"C:\\Program Files\\Winamp\\Plugins\\MPK.BIN");
  _add_param_str("OPXDRV",0,0,"C:\\Program Files\\Winamp\\Plugins\\OPX4KSS.BIN");
  _add_param_str("FMBIOS",0,0,"C:\\Program Files\\Winamp\\Plugins\\FMPAC.ROM");
  _add_param_str("MBMDRV",0,0,"C:\\Program Files\\Winamp\\Plugins\\MBR143.001");

  _add_param_int("TIME_DETECT_MODE",  FMT_DEC, 0,3,1);
  _add_param_int("TIME_DETECT_LEVEL", FMT_DEC, 0,1,0);
  _add_param_int("SILENT_LIMIT", FMT_DEC, 1,60,5);
  _add_param_int("SILENT_DETECT", FMT_DEC, 0,1,1);

  _add_param_int("PSG_CUTOFF",  FMT_DEC, 0,20000,0);
  _add_param_int("SCC_CUTOFF",  FMT_DEC, 0,20000,0);
  _add_param_int("OPL_CUTOFF",  FMT_DEC, 0,20000,0);
  _add_param_int("OPLL_CUTOFF", FMT_DEC, 0,20000,0);

  _add_param_int("OPLL_CH_PAN", FMT_DEC, 0,1<<28,0x7EDE79E);
  _add_param_int("EXPAND_INFO", FMT_BOOL, 0,1,0);

  strcpy(config->file_name, path);
  strcat(config->file_name, PLUGIN_NAME".ini");
  strcpy(config->ill_path, path);
  strcat(config->ill_path, PLUGIN_NAME".ill");
  strcpy(config->section_name, section);

  return config ;
}

void CONFIG_delete(CONFIG *config)
{
  if(config)
  {
    int i;
    CONFIG_PARAM *p,*q;

    for(i=0;i<HASH_KEY_NUM;i++)
    {
      p = config->param_list[i];
      while(p)
      {
        q = p;
        p = p->next ;
        delete_param(q);
      }
    }

    CONFIG_dialog_end(config) ;
    if(config->pandlg) PANDLG_delete(config->pandlg);
    DeleteCriticalSection(&config->cso);
    free(config) ;
  }
}

void CONFIG_load(CONFIG *config)
{
  int i ;
  CONFIG_PARAM *p;
  char buf[128];


  for(i=0;i<HASH_KEY_NUM;i++)  
  {
    p = config->param_list[i];

    while(p)
    {
      switch(p->type)
      {
      case INT_PARAM:
        GetPrivateProfileString(config->section_name, p->name, "", buf, 128, config->file_name);
        if(buf[0]) p->data = (void *)str2num(buf, p->min, p->max) ;
        break;

      case STR_PARAM:
        GetPrivateProfileString(config->section_name, p->name, "", buf, 128, config->file_name);
        if(buf[0]) set_string(p,buf);
        break;

      default:
        break;
      }
      p=p->next ;
    }
  }

  config->vol[EDSC_PSG] = CONFIG_get_int(config,"PSG_VOL");
  config->vol[EDSC_SCC] = CONFIG_get_int(config,"SCC_VOL");
  config->vol[EDSC_OPLL] = CONFIG_get_int(config,"OPLL_VOL");
  config->vol[EDSC_OPL] = CONFIG_get_int(config,"OPL_VOL");

  if(CONFIG_get_int(config,"ENABLE_KSS")) strcpy(config->extensions, KSS_DESC) ;
  if(CONFIG_get_int(config,"ENABLE_MGS")) strcat(config->extensions, MGS_DESC) ;
  if(CONFIG_get_int(config,"ENABLE_BGM")) strcat(config->extensions, BGM_DESC) ;
  if(CONFIG_get_int(config,"ENABLE_MPK")) strcat(config->extensions, MPK_DESC) ;
  if(CONFIG_get_int(config,"ENABLE_OPX")) strcat(config->extensions, OPX_DESC) ;
  if(CONFIG_get_int(config,"ENABLE_MBM")) strcat(config->extensions, MBM_DESC) ;
  for(i=0;config->extensions[i]!='\0';i++)
    if(config->extensions[i]=='\\') config->extensions[i] = '\0';
}

void CONFIG_save(CONFIG *config)
{
  CONFIG_PARAM *p;
  char buf[16];
  int i;

  CONFIG_set_int(config,"PSG_VOL",config->vol[EDSC_PSG]);
  CONFIG_set_int(config,"SCC_VOL",config->vol[EDSC_SCC]);
  CONFIG_set_int(config,"OPLL_VOL",config->vol[EDSC_OPLL]);
  CONFIG_set_int(config,"OPL_VOL",config->vol[EDSC_OPL]); 

  for(i=0;i<HASH_KEY_NUM;i++)
  {
    p = config->param_list[i];

    while(p)
    {
      switch(p->type)
      {
      case STR_PARAM:
        WritePrivateProfileString(config->section_name, p->name, (char *)p->data, config->file_name);
        break;

      case INT_PARAM:
        WritePrivateProfileString(config->section_name, p->name, itoa((__int32)p->data,buf,10), config->file_name);
        break;

      default:
        break;
      }
      p = p-> next ;
    }
  }
}

/* Config --> CheckBox */
void CONFIG_wr_checkbtn(CONFIG *config, HWND hDlg, char *name, int idc)
{
  if(CONFIG_get_int(config,name))
    CheckDlgButton(hDlg, idc, BST_CHECKED) ;
  else
    CheckDlgButton(hDlg, idc, BST_UNCHECKED) ;
}

/* Config <-- CheckBox */
void CONFIG_rd_checkbtn(CONFIG *config, HWND hDlg, char *name, int idc)
{
  if(IsDlgButtonChecked(hDlg, idc)==BST_CHECKED)
    CONFIG_set_int(config,name,1);
  else
    CONFIG_set_int(config,name,0);
}

static void itot(char *buf, int time)
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

char *CONFIG_param2text(CONFIG *config, char *name)
{
  static char buf[16];
  CONFIG_PARAM *p;

  if(p=find_param(config->param_list, name))
  {
    switch(p->type)
    {
    /* String */
    case STR_PARAM:
      return (char *)p->data;

    /* Integer */
    case INT_PARAM:
      switch(p->format)
      {
      case FMT_DEC:
      case FMT_NONE:
        itoa((__int32)p->data,buf,10);
        return buf;

      case FMT_BOOL:
        if((__int32)p->data)
          return "YES";
        else
          return "NO";

      case FMT_TIME:
        itot(buf,(__int32)p->data);
        return buf;

      case FMT_DB:
        sprintf(buf,"%3.2fdB",((__int32)p->data*KSSPLAY_VOL_STEP));
        return buf;
     
      case FMT_PERC:
        if((__int32)p->data<0)
          sprintf(buf,"%3.2f%%",((double)-((__int32)p->data))/p->min);
        else
          sprintf(buf,"%3.2f%%",((double)((__int32)p->data))/p->max);
        return buf;

      default:
        return p->name;
      }

    /* Others(Error) */
    default:
      return p->name;
    }
  }
  else return "?";
}

void CONFIG_text2param(CONFIG *config, char *buf, char *name)
{
  CONFIG_PARAM *p;

  if(p=find_param(config->param_list, name))
  {
    switch(p->type)
    {
    case STR_PARAM:
      set_string(p,buf);
      break;

    case INT_PARAM:
      if(buf==NULL)
      {
        p->data = 0;
        return ;
      }
      switch(p->format)
      {
      case FMT_DEC:
        p->data = (void *)atoi(buf);
        break;

      case FMT_DB:
        break;
        
      case FMT_BOOL:
        if(strcmp(buf,"1")==0||strcmp(buf,"YES")==0||strcmp(buf,"TRUE"))
          p->data = (void *)1;
        else
          p->data = (void *)0;
        break;

      case FMT_TIME:
        p->data = (void *)PPLS_get_time(buf,5*60000);
        break;

      default:
        break;
      }

    default:
      break;
    }
  }
}

void CONFIG_wr_textbox(CONFIG *config, HWND hDlg, char *name, int idc)
{
  SetWindowText(GetDlgItem(hDlg,idc),CONFIG_param2text(config, name));
}

void CONFIG_rd_textbox(CONFIG *config, HWND hDlg, char *name, int idc)
{
  char buf[512];

  GetWindowText(GetDlgItem(hDlg,idc),buf,512);
  CONFIG_text2param(config,buf,name);
}


void CONFIG_wr_radiobtn(CONFIG *config, HWND hDlg, char *name, int first_idc, int last_idc, __int32 offset)
{
  CheckRadioButton(hDlg, first_idc, last_idc, first_idc + CONFIG_get_int(config,name) - offset) ;
}

void CONFIG_rd_radiobtn(CONFIG *config, HWND hDlg, char *name, int first_idc, int last_idc,__int32 offset)
{
  int i;

  for(i=first_idc;i<=last_idc;i++)
  {
    if(IsDlgButtonChecked(hDlg, i) == BST_CHECKED)
      CONFIG_set_int(config,name,i-first_idc+offset);
  }
}

void CONFIG_wr_slider(CONFIG *config, HWND hDlg, char *name, int idc,__int32 (*adjust_func)(__int32 value))
{
  if(adjust_func)
    SendMessage(GetDlgItem(hDlg, idc),TBM_SETPOS,TRUE,adjust_func(CONFIG_get_int(config,name))) ;
  else
    SendMessage(GetDlgItem(hDlg, idc),TBM_SETPOS,TRUE,CONFIG_get_int(config,name)) ;
}

void CONFIG_rd_slider(CONFIG *config, HWND hDlg, char *name, int idc, __int32 (*adjust_func)(__int32 value))
{
  if(adjust_func)
    CONFIG_set_int(config,name,adjust_func(SendMessage(GetDlgItem(hDlg,idc),TBM_GETPOS,0,0))) ;
  else
    CONFIG_set_int(config,name,SendMessage(GetDlgItem(hDlg,idc),TBM_GETPOS,0,0)) ;
}
