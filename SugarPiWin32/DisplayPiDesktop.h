#pragma once

#include <windows.h>
#include <gdiplus.h>
#include <d2d1.h>
#include <mutex>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

#include "Screen.h"
#include "ICfg.h"
#include "ddraw.h"

#include "IDirectories.h"
#include "CLogger.h"
#include "res/coolspot.h"

#define REAL_DISP_X  1024 //832 //1024 // 768
#define REAL_DISP_Y  624 //-16 //624 //576

#define FRAME_BUFFER_SIZE 2

class DisplayPi : public IDisplay, public ICfg
{
public:

   DisplayPi(CLogger* logger);
   virtual ~DisplayPi(void);

   void SyncWithFrame(bool set) { sync_on_frame_ = set; }
   bool IsSyncOnFrame() { return sync_on_frame_; }

   void SetFullResolution(bool set) { full_resolution_ = set; };
   void Loop();

   virtual void WindowsToTexture(int& x, int& y);
   virtual void SetScanlines(int scan) { m_TypeScanlines = scan; };
   virtual void Display() {};
   virtual void Refresh() {};
   virtual bool AFrameIsReady();
   virtual void Config() {};
   virtual void GetWindowSize(int* w, int* h) { *w = m_WidthWindow; *h = m_HeightWindow; };
   virtual const char* GetInformations() { return "GDI"; };
   virtual void FullScreenToggle() {};
   virtual void ForceFullScreen(bool bSetFullScreen) {}
   virtual void ReleaseAll();
   virtual int GetWidth();
   virtual int GetHeight();
   virtual void Init(HINSTANCE hInstance, HWND hWnd, IFullScreenInterface* pFSInt);
   virtual void SetBeam(int x, int y, unsigned int col);
   virtual void SetDirectories(IDirectories* pDiretories) { m_pDiretories = pDiretories; };
   virtual void HSync();
   virtual void VSync(bool bDbg = false);
   virtual void StartSync();
   virtual void WaitVbl();
   virtual int* GetVideoBuffer(int y);
   virtual void Reset();
   virtual bool SetSyncWithVbl(int speed) { return false; };
   virtual bool IsWaitHandled() { return false; };
   virtual bool GetBlackScreenInterval() { return m_BlackScreen; };
   virtual void SetBlackScreenInterval(bool bBS) { m_BlackScreen = bBS; };

   virtual void SetDefaultConfiguration();
   virtual void SaveConfiguration(const char* config_name, const char* ini_file);
   virtual bool LoadConfiguration(const char* config_name, const char* ini_file);
   virtual HWND CreateModelessConfigDlg(HWND hParent);
   virtual void InitDlg();
   virtual void Apply();
   virtual void Cancel();
   // Set the window position : 
   // x, y : Coord of the window
   // xIn, yIn : Coord of the display, in pixel, in the window (So toolbar and status bar can take place)
   // width, height : size of the window
   // Inner display still use the display content width and height)
   virtual void WindowChanged(int xIn, int yIn, int wndWidth, int wndHeight)
   {
      m_WidthWindow = wndWidth;
      m_HeightWindow = wndHeight;
      m_XIn = xIn;
      m_YIn = yIn;
   };

   virtual void Screenshot();
   virtual void Screenshot(const char* scr_path);
   virtual void ScreenshotEveryFrame(int bSetOn) { m_DoScreenshotSerie = bSetOn; };
   virtual bool IsEveryFrameScreened() {
      return m_DoScreenshotSerie
         ;
   };
   virtual bool IsDisplayed() { return true; };

   virtual void SetSize(SizeEnum size);
   virtual SizeEnum  GetSize() { return m_Size; };

   virtual void ResetLoadingMedia();
   virtual void SetLoadingMedia();

   virtual void ResetDragnDropDisplay();
   virtual void SetDragnDropDisplay(int type);
   virtual void SetCurrentPart(int x, int y);
   virtual int GetDnDPart() { return m_CurrentPart; };

   // Services
   void DisplayText(const char* txt, int x, int y, bool selected = false);

protected:
   // Scanlines
   int m_TypeScanlines;       // 1 : scanlines / 0 : copylines
   // Info on display...

   typedef TCHAR tDropText[16];
   virtual void DisplayDrop(int nbSections, tDropText* pLabels);
   virtual void DisplayDebug(int x, int y);

   int m_DnDType;
   int m_CurrentPart;
   bool m_BlackScreen;

   // Progress bar for media loading
   bool full_resolution_;
   bool full_resolution_cached_;

   int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
   bool GetNewScreenshotFile(TCHAR* buffer_P, unsigned int size_P);

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

   bool sync_on_frame_;
   CoolspotFont* font_;
   CLogger* logger_;

   std::mutex mutex_;

   // Frame buffer availability
   typedef enum
   {
      FR_FREE,
      FR_USED,
      FR_READY
   } FrameState;
   FrameState frame_used_[FRAME_BUFFER_SIZE];
   unsigned int buffer_used_;

   unsigned int frame_queue_[FRAME_BUFFER_SIZE];
   unsigned int nb_frame_in_queue_;

   // DX
   ID2D1HwndRenderTarget* pRT_;
   unsigned int* frame_buffer_;
   ID2D1Bitmap* bitmap_;
};
