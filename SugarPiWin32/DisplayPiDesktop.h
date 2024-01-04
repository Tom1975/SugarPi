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
#define REAL_DISP_Y  624 //-16 //624 //576

#define FRAME_BUFFER_SIZE 2

class DisplayPiDesktop : public DisplayPi
{
public:

   DisplayPiDesktop(CLogger* logger);
   virtual ~DisplayPiDesktop(void);

   virtual void Lock() { mutex_.lock(); }
   virtual void Unlock() { mutex_.unlock(); }
   virtual int* GetVideoBuffer(int y);
   virtual void SetFrame(int frame_index);
   virtual void Draw();
   virtual void ClearBuffer(int frame_index);

   virtual void WindowsToTexture(int& x, int& y);
   virtual void GetWindowSize(int* w, int* h) { *w = m_WidthWindow; *h = m_HeightWindow; };
   virtual const char* GetInformations() { return "GDI"; };
   virtual void ReleaseAll();
   virtual int GetWidth();
   virtual int GetHeight();
   virtual void Init(HINSTANCE hInstance, HWND hWnd, IFullScreenInterface* pFSInt);
   virtual void SetDirectories(IDirectories* pDiretories) { m_pDiretories = pDiretories; };
   virtual void WaitVbl();
   virtual void Reset();

protected:
   SizeEnum m_Size;

   // Displayed window : 
   int m_X, m_Y;
   int m_XIn, m_YIn;
   int m_Width;
   int m_Height;

   int m_WidthWindow;
   int m_HeightWindow;

   int m_DoScreenshotSerie;


   IDirectories* m_pDiretories;
   IFullScreenInterface* m_pFSInt;

   HWND        m_hWnd;
   HDC         m_hwndDC;
   HDC         m_MemDC;
   HBITMAP     m_Bitmap;
   HBITMAP     m_iBitmap;
   Bitmap* m_BmpMem;
   BITMAPINFO  bi24BitInfo; // We set this up to grab what we want

   bool m_bDeviceLost;

   CoolspotFont* font_;

   std::mutex mutex_;

   // DX
   ID2D1HwndRenderTarget* pRT_;
   unsigned int* frame_buffer_;
   ID2D1Bitmap* bitmap_;
};
