#pragma once

#include <windows.h>
#include <gdiplus.h>
#include <d2d1_1.h>

#include <mutex>
#include <vector>
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
   //virtual void Draw();

   virtual void BeginDraw();
   virtual void EndDraw();
   virtual bool ChangeNeeded(int change);
   //virtual void Draw();

   virtual void WindowsToTexture(int& x, int& y);
   virtual const char* GetInformations() { return "GDI"; };
   virtual void ReleaseAll();
   virtual void Init(HINSTANCE hInstance, HWND hWnd, IFullScreenInterface* pFSInt);
   virtual void WaitVbl();

protected:
   virtual void CopyMemoryToRessources(DisplayPi::Frame* frame_);
   virtual void ChangeAttribute(Frame*, int src_x, int src_y, int src_w, int src_h,
      int dest_x, int dest_y, int dest_w, int dest_h);

   virtual void CopyMemoryToRessources(ID2D1Bitmap* bitmap, BasicFrame* frame);

   HWND CreateWindowFrame(BasicFrame* frame, int priority);

   IFullScreenInterface* m_pFSInt;

   std::mutex mutex_;

   // DX
   HWND        m_hWnd;
   HWND        m_hEmuWnd;
   HWND        m_hBackWnd;
   HWND        m_hMenuWnd;

   BasicFrame* frame_;
   HWND wnd_;
   ID2D1HwndRenderTarget* pRT_;
   ID2D1Bitmap* bitmap_;

   class Win32Frame : public Frame
   {
   public:
      ID2D1Layer* pLayer_;
   };

   //std::vector<Win32Frame> windows_list_;

   HINSTANCE hInstance_;
};
