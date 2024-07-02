#include <vector>

#include "DisplayPiDesktop.h"
#include "Shlwapi.h"
#include "dwmapi.h"

#define DISP_WIDTH    1024
#define DISP_HEIGHT   576

#define DISP_WINDOW_X   800
#define DISP_WINDOW_Y   600//576

#define SCR_PATH "\\SCR\\"

ATOM s_FrameAtom = NULL;

DisplayPiDesktop::DisplayPiDesktop(CLogger* logger) : DisplayPi(logger),
m_pFSInt(NULL)
{
   CoInitialize(NULL);
}

DisplayPiDesktop::~DisplayPiDesktop()
{
   if (pRT_) pRT_->Release();

   CoUninitialize();
}

void DisplayPiDesktop::ReleaseAll()
{
}

void DisplayPiDesktop::WindowsToTexture(int& x, int& y)
{

}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   switch (message)
   {
   case WM_CREATE:
   {
      CREATESTRUCT* pCreateStr = (CREATESTRUCT*)lParam;
      SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pCreateStr->lpCreateParams);
      break;
   }
   case WM_QUIT:
      break;
   case WM_DESTROY:
      PostQuitMessage(0);
      break;
   case WM_PAINT:
   {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hWnd, &ps);



      EndPaint(hWnd, &ps);
      break;
   }
   default:
      return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}


HWND DisplayPiDesktop::CreateWindowFrame(BasicFrame* frame, int priority)
{
   return CreateWindowEx(0, "PiFrame", "PiFrame", WS_CHILD | WS_VISIBLE,
      frame->GetDisplayX(), frame->GetDisplayY(), frame->GetDisplayWidth(), frame->GetDisplayHeight(), m_hWnd, 0, hInstance_, frame); // NULL);
}

void DisplayPiDesktop::Init(HINSTANCE hInstance, HWND hWnd, IFullScreenInterface* pFSInt)
{
   hInstance_ = hInstance;
   if (s_FrameAtom == NULL)
   {
      WNDCLASSEX wcclient;
      wcclient.cbSize = sizeof(WNDCLASSEX);

      wcclient.style = 0;
      wcclient.lpfnWndProc = WndProc;
      wcclient.cbClsExtra = 0;
      wcclient.cbWndExtra = 0;
      wcclient.hInstance = hInstance;
      wcclient.hIcon = NULL,
         wcclient.hCursor = LoadCursor(NULL, IDC_ARROW);
      wcclient.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
      wcclient.lpszMenuName = NULL;
      wcclient.lpszClassName = "PiFrame";
      wcclient.hIconSm = 0;

      s_FrameAtom = RegisterClassEx(&wcclient);
   }

   ID2D1Factory* pD2DFactory = NULL;
   HRESULT hr = D2D1CreateFactory(
      D2D1_FACTORY_TYPE_SINGLE_THREADED,
      &pD2DFactory
   );

   m_hWnd = hWnd;

   // Obtain the size of the drawing area.
   RECT rc;
   GetClientRect(m_hWnd, &rc);

   DisplayPi::Initialization();

   // Create a Direct2D render target          
   hr = pD2DFactory->CreateHwndRenderTarget(
      D2D1::RenderTargetProperties(),
      D2D1::HwndRenderTargetProperties(
         m_hWnd,
         D2D1::SizeU(
            rc.right - rc.left,
            rc.bottom - rc.top)
         , D2D1_PRESENT_OPTIONS_IMMEDIATELY
      ),
      &pRT_
   );

   D2D1_SIZE_U size = { 0 };
   D2D1_BITMAP_PROPERTIES props;
   pRT_->GetDpi(&props.dpiX, &props.dpiY);
   D2D1_PIXEL_FORMAT pixelFormat = D2D1::PixelFormat(
      DXGI_FORMAT_B8G8R8A8_UNORM,
      D2D1_ALPHA_MODE_PREMULTIPLIED
   );
   props.pixelFormat = pixelFormat;
   size.width = REAL_DISP_X;
   size.height = REAL_DISP_Y;

   hr = pRT_->CreateBitmap(size,
      emu_frame_.GetBuffer(),
      emu_frame_.GetPitch(),
      props,
      &bitmap_);

   ///////////////////////////////////
   // Background
   Win32Frame frame_back;
   back_frame_.SetDisplay(0, 0);
   back_frame_.SetDisplaySize(640, 480);

   frame_back.basic_ = &back_frame_;
   hr = pRT_->CreateLayer(NULL, &frame_back.pLayer_);

   windows_list_.push_back(frame_back);

   ///////////////////////////////////
   // Menu
   Win32Frame frame_menu;
   menu_frame_.SetDisplay(0, 0);
   menu_frame_.SetDisplaySize(640, 480);

   frame_menu.basic_ = &menu_frame_;
   hr = pRT_->CreateLayer(NULL, &frame_menu.pLayer_);

   windows_list_.push_back(frame_menu);

   ///////////////////////////////////
   // Emulator screen
   Win32Frame frame_emu;
   emu_frame_.SetDisplay(0, 0);
   emu_frame_.SetDisplaySize(640, 480);

   frame_emu.basic_ = &emu_frame_;
   hr = pRT_->CreateLayer(NULL, &frame_emu.pLayer_);

   windows_list_.push_back(frame_emu);

   m_pFSInt = pFSInt;
   pD2DFactory->Release();


}

void DisplayPiDesktop::WaitVbl()
{
   // Wait for sound mixer
   int dbg = 1;
}

void DisplayPiDesktop::CopyMemoryToRessources(ID2D1Bitmap* bitmap, BasicFrame* frame)
{
   HRESULT hr;

   hr = bitmap->CopyFromMemory(NULL,
      frame->GetBuffer(), frame->GetPitch());
}

void DisplayPiDesktop::Draw()
{
   pRT_->BeginDraw();
   for (auto it : windows_list_)
   {
      it.basic_->Refresh();
      CopyMemoryToRessources(bitmap_, it.basic_);
      it.basic_->FrameIsDisplayed();

      // To be use if problems occurs 
      int changed = it.basic_->AttributesHasChanged();
      //if (changed != 0)
      {
         D2D1_RECT_F src_rect = { it.basic_->GetOffsetX(), it.basic_->GetOffsetY(), it.basic_->GetWidth() + it.basic_->GetOffsetX(), it.basic_->GetHeight() + it.basic_->GetOffsetY() };
         D2D1_RECT_F dest_rect = { it.basic_->GetDisplayX(), it.basic_->GetDisplayY(), it.basic_->GetDisplayX() + it.basic_->GetDisplayWidth(), it.basic_->GetDisplayY() + it.basic_->GetDisplayHeight() };
         // Push the layer with the content bounds.
         D2D1_LAYER_PARAMETERS1 layerParameters = { 0 };

         layerParameters.contentBounds = D2D1::InfiniteRect();
         layerParameters.geometricMask = NULL;
         layerParameters.maskAntialiasMode = D2D1_ANTIALIAS_MODE_PER_PRIMITIVE;
         layerParameters.maskTransform = D2D1::IdentityMatrix();
         layerParameters.opacity = 1.0;
         layerParameters.opacityBrush = NULL;
         layerParameters.layerOptions = D2D1_LAYER_OPTIONS1_IGNORE_ALPHA;

         D2D1_LAYER_PARAMETERS* toto = (D2D1_LAYER_PARAMETERS*) & layerParameters;

         pRT_->PushLayer (
            toto,
            it.pLayer_
         );

         pRT_->DrawBitmap(bitmap_, &dest_rect, 1,
            D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &src_rect);

         pRT_->PopLayer();
      }

   }
   pRT_->EndDraw();
}

