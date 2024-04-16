#include <vector>

#include "DisplayPiDesktop.h"
#include "Shlwapi.h"
#include "dwmapi.h"

#define DISP_WIDTH    1024
#define DISP_HEIGHT   576

#define DISP_WINDOW_X   800
#define DISP_WINDOW_Y   600//576

#define SCR_PATH "\\SCR\\"

DisplayPiDesktop::DisplayPiDesktop(CLogger* logger) : DisplayPi(logger),
   m_pFSInt(NULL)
{
   pRT_ = nullptr;

   CoInitialize(NULL);
}

DisplayPiDesktop::~DisplayPiDesktop()
{
   if (pRT_)pRT_->Release();
   CoUninitialize();
}

void DisplayPiDesktop::ReleaseAll()
{
}

void DisplayPiDesktop::WindowsToTexture(int& x, int& y)
{

}

void DisplayPiDesktop::Init(HINSTANCE hInstance, HWND hWnd, IFullScreenInterface* pFSInt)
{
   ID2D1Factory* pD2DFactory = NULL;
   HRESULT hr = D2D1CreateFactory(
      D2D1_FACTORY_TYPE_SINGLE_THREADED,
      &pD2DFactory
   );

   m_hWnd = hWnd;
   
   // Obtain the size of the drawing area.
   RECT rc;
   GetClientRect(m_hWnd, &rc);

   // Create a Direct2D render target          
   
   hr = pD2DFactory->CreateHwndRenderTarget(
      D2D1::RenderTargetProperties(),
      D2D1::HwndRenderTargetProperties(
         m_hWnd,
         D2D1::SizeU(
            rc.right - rc.left,
            rc.bottom - rc.top)
         ,D2D1_PRESENT_OPTIONS_IMMEDIATELY
      ),
      &pRT_
   );
   pD2DFactory->Release();

   D2D1_SIZE_U size = { 0 };
   D2D1_BITMAP_PROPERTIES props;
   pRT_->GetDpi(&props.dpiX, &props.dpiY);
   D2D1_PIXEL_FORMAT pixelFormat = D2D1::PixelFormat(
      DXGI_FORMAT_B8G8R8A8_UNORM,
      D2D1_ALPHA_MODE_IGNORE
   );
   props.pixelFormat = pixelFormat;
   size.width = REAL_DISP_X;
   size.height = REAL_DISP_Y;

   hr = pRT_->CreateBitmap(size,
      display_buffer_[0],
      size.width * 4,
      props,
      &bitmap_);

   m_pFSInt = pFSInt;

   Initialization();

}

void DisplayPiDesktop::WaitVbl()
{
   // Wait for sound mixer
   int dbg = 1;
}

void DisplayPiDesktop::SetFrame(int frame_index)
{
   HRESULT hr;
   hr = bitmap_->CopyFromMemory(NULL,
      display_buffer_[frame_index], REAL_DISP_X * 4);

}

void DisplayPiDesktop::Draw()
{
   HRESULT hr;
   pRT_->BeginDraw();

   D2D1_RECT_F src_rect = {123,47/2,763, 527};
   pRT_->DrawBitmap(bitmap_, NULL, 1,
      D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &src_rect);
   hr = pRT_->EndDraw();
}

