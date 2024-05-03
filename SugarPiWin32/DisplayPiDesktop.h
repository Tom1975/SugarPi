#pragma once

#include <windows.h>
#include <gdiplus.h>
#include <d2d1.h>
#include <mutex>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

#include "Screen.h"
#include "ddraw.h"

#include "IDirectories.h"
#include "CLogger.h"
#include "res/coolspot.h"
#include "DisplayPi.h"

#define REAL_DISP_X  1024 //832 //1024 // 768
#define REAL_DISP_Y  (288*2) //-16 //624 //576

class DisplayPiDesktop : public DisplayPi
{
public:

   DisplayPiDesktop(CLogger* logger);
   virtual ~DisplayPiDesktop(void);

   virtual void Lock() { mutex_.lock(); }
   virtual void Unlock() { mutex_.unlock(); }
   virtual void SetFrame(int frame_index);
   virtual void Draw();

   virtual void WindowsToTexture(int& x, int& y);
   virtual const char* GetInformations() { return "GDI"; };
   virtual void ReleaseAll();
   virtual void Init(HINSTANCE hInstance, HWND hWnd, IFullScreenInterface* pFSInt);
   virtual void WaitVbl();

protected:
   IFullScreenInterface* m_pFSInt;

   std::mutex mutex_;

   // DX
   HWND        m_hWnd;
   ID2D1HwndRenderTarget* pRT_;
   ID2D1Bitmap* bitmap_;
};
