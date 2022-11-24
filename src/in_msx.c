#include <windows.h>
#include <commctrl.h>
#include "in_msx.h"
#include "msxplug.h"

In_Module mod = 
{
	IN_VER, // module type
	"MSXplug "PLUGIN_VERSION" ("PLUGIN_BUILD_DATE")", // description of module, with version string
	0,	/* hMainWindow (filled in by winamp after init() ) */
	0,  /* hDllInstance ( Also filled in by winamp) */
	"" // FileExtensions handled by this DLL
	,
	1,	/* is_seekable (is this stream seekable?) */
	1, /* uses output (does this plug-in use the output plug-ins?) */
	MSXPLUG_config, /* configuration dialog */
	MSXPLUG_about,  /* about dialog */
	MSXPLUG_init,   /* called at program init */
	MSXPLUG_quit,   /* called at program quit */
	MSXPLUG_getfileinfo, /* if file == NULL, current playing is used */
	MSXPLUG_infoDlg, 
	MSXPLUG_isourfile, /* called before extension checks, to allow detection of mms://, etc */
	
  /* PLAYBACK STUFF */
  MSXPLUG_play, /* return zero on success, -1 on file-not-found, some other value on other (stopping winamp) error */
	MSXPLUG_pause, /* pause stream */
	MSXPLUG_unpause, /* unpause stream */
	MSXPLUG_ispaused, /* ispaused? return 1 if pause, 0 if not */
	MSXPLUG_stop, /* stop (unload) stream */
	
  /* TIME STUFF */
	MSXPLUG_getlength, /* get length in ms */
	MSXPLUG_getoutputtime, /* returns current output time in ms. (usually returns outMod->GetOutputTime()) */
	MSXPLUG_setoutputtime, /* seeks to point in stream (in ms). Usually you signal yoru thread to seek, which seeks and calls outMod->Flush()..)

  /* VOLUME STUFF */
	MSXPLUG_setvolume, /* from 0 to 255.. usually just call outMod->SetVolume */
	MSXPLUG_setpan, /* from -127 to 127.. usually just call outMod->SetPan */

  /* VIS STUFF */
	0,0,0,0,0,0,0,0,0,

  /* DSP STUFF */
	0,0,

  /* EQ STUFF */
	MSXPLUG_eq_set,

  /* info setting (filled in by winamp) */
	NULL,

  /* Out_Module *outMod; (filled in by winamp, optionally used) */
	0 

};

/* the Dll initializer */
//BOOL WINAPI _DllMainCRTStartup(HANDLE hInst, ULONG ul_reason_for_call, LPVOID lpReserved)
//{
//	return TRUE;
//}

__declspec( dllexport ) In_Module * winampGetInModule2()
{
	return &mod;
}
