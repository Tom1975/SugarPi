//
#include <memory.h>
#include <math.h>
#include <stdlib.h>
#include "MenuButtonWindows.h"

#ifdef RASPPI
#include "BasicFrame.h"
#else
#include "DisplayPiDesktop.h"

#define PROFILE

#ifdef PROFILE
#include <profileapi.h>
#define START_CHRONO  QueryPerformanceFrequency((LARGE_INTEGER*)&freq);;QueryPerformanceCounter ((LARGE_INTEGER*)&s1);
#define STOP_CHRONO   QueryPerformanceCounter ((LARGE_INTEGER*)&s2);t=(DWORD)(((s2 - s1) * 1000000) / freq);
#define PROF_DISPLAY sprintf(s, "Duree displays Frame: %d us\n", t);OutputDebugString (s);
static __int64 s1, s2, freq;
static DWORD t;
static char s[1024];
#endif

#endif

#ifdef  __circle__
   #define PATH_FONT "SD:/FONTS/Facile_Sans.ttf"
#else
   #define PATH_FONT ".\\FONTS\\Facile_Sans.ttf" 
#endif


////////////////////////////////////////////////////////////////////////////////////
MenuButtonWindows::MenuButtonWindows(BasicFrame* display) : MenuItemWindows(display), button_(display)
{

}

MenuButtonWindows::~MenuButtonWindows()
{

}

void MenuButtonWindows::Create(SimpleBitmap* bmp, Window* parent, int x, int y, unsigned int width, unsigned int height)
{
   MenuItemWindows::Create("", parent, x, y, width, height);
   button_.Create(this, 0, 0, width, height);
   button_.InitButton(bmp, 0, 0);
}

void MenuButtonWindows::RedrawWindow()
{
   // Draw button
   button_.Redraw();
}

