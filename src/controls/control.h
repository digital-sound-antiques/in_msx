#include <windows.h>
#include "rc/resource.h"
HWND CreateMixerControl(HINSTANCE hInst, HWND hWndParent, char *title, int min, int max) ;
HWND CreateSwitchControl(HINSTANCE hInst, HWND hWndParent, char *title) ;
HWND CreatePanControl(HINSTANCE hInst, HWND hWndParent, char *title, int min, int max) ;
