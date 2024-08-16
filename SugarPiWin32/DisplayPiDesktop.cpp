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

int DisplayPiDesktop::GetWidth()
{
   return rc_.right - rc_.left;
}

int DisplayPiDesktop::GetHeight()
{
   return rc_.bottom - rc_.top;
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
   GetClientRect(m_hWnd, &rc_);

   DisplayPi::Initialization();

   // Create a Direct2D render target          
   hr = pD2DFactory->CreateHwndRenderTarget(
      D2D1::RenderTargetProperties(),
      D2D1::HwndRenderTargetProperties(
         m_hWnd,
         D2D1::SizeU(
            rc_.right - rc_.left,
            rc_.bottom - rc_.top)
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

   size.height = 1080;
   size.width = 1920;
   hr = pRT_->CreateBitmap(size,
      menu_frame_.GetBuffer(),
      menu_frame_.GetPitch(),
      props,
      &menu_bitmap_);

   ///////////////////////////////////
   // Background
   Win32Frame *frame_back = new Win32Frame;
   back_frame_.SetDisplay(0, 0);
   back_frame_.SetDisplaySize(800, 600);

   frame_back->frame_= &back_frame_;
   frame_back->bitmap_ = bitmap_;
   hr = pRT_->CreateLayer(NULL, &frame_back->pLayer_);

   windows_list_.push_back(frame_back);

   ///////////////////////////////////
   // Menu
   Win32Frame *frame_menu = new Win32Frame;
   menu_frame_.SetDisplay(0, 0);
   menu_frame_.SetDisplaySize(800, 600);

   frame_menu->frame_ = &menu_frame_;
   frame_menu->bitmap_ = menu_bitmap_;
   hr = pRT_->CreateLayer(NULL, &frame_menu->pLayer_);

   windows_list_.push_back(frame_menu);

   ///////////////////////////////////
   // Emulator screen
   Win32Frame *frame_emu = new Win32Frame;;
   emu_frame_.SetDisplay(0, 0);
   emu_frame_.SetDisplaySize(800, 600);

   frame_emu->frame_ = &emu_frame_;
   frame_emu->bitmap_ = bitmap_;
   hr = pRT_->CreateLayer(NULL, &frame_emu->pLayer_);

   windows_list_.push_back(frame_emu);

   m_pFSInt = pFSInt;
   pD2DFactory->Release();


}

void DisplayPiDesktop::WaitVbl()
{
   // Wait for sound mixer
   int dbg = 1;
}

void DisplayPiDesktop::CopyMemoryToRessources(DisplayPi::Frame* frame)
{
   frame->frame_->Refresh();

   CopyMemoryToRessources(((Win32Frame*)frame)->bitmap_, frame->frame_);

   frame->frame_->FrameIsDisplayed();
}

void DisplayPiDesktop::CopyMemoryToRessources(ID2D1Bitmap* bitmap, BasicFrame* frame)
{
   HRESULT hr;

   hr = bitmap->CopyFromMemory(NULL,
      frame->GetReadyBuffer(), frame->GetPitch());
}

void DisplayPiDesktop::ChangeAttribute(Frame* frame, int src_x, int src_y, int src_w, int src_h,
   int dest_x, int dest_y, int dest_w, int dest_h)
{
   D2D1_RECT_F src_rect = { src_x, src_y, src_x + src_w, src_y + src_h };
   D2D1_RECT_F dest_rect = { dest_x, dest_y, dest_x + dest_w, dest_y + dest_h };
   // Push the layer with the content bounds.
   D2D1_LAYER_PARAMETERS1 layerParameters = { 0 };

   layerParameters.contentBounds = D2D1::InfiniteRect();
   layerParameters.geometricMask = NULL;
   layerParameters.maskAntialiasMode = D2D1_ANTIALIAS_MODE_PER_PRIMITIVE;
   layerParameters.maskTransform = D2D1::IdentityMatrix();
   layerParameters.opacity = 1.0;
   layerParameters.opacityBrush = NULL;
   layerParameters.layerOptions = D2D1_LAYER_OPTIONS1_INITIALIZE_FROM_BACKGROUND;

   D2D1_LAYER_PARAMETERS* toto = (D2D1_LAYER_PARAMETERS*)&layerParameters;

   Win32Frame* win_frame = (Win32Frame*)frame;

   pRT_->PushLayer(
      toto,
      win_frame->pLayer_
   );

   pRT_->DrawBitmap(((Win32Frame*)frame)->bitmap_, &dest_rect, 1,
      D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &src_rect);

   pRT_->PopLayer();
}

void DisplayPiDesktop::BeginDraw()
{
   pRT_->BeginDraw();
}

void DisplayPiDesktop::EndDraw()
{
   pRT_->EndDraw();
}

bool DisplayPiDesktop::ChangeNeeded(int change)
{
   // ALWAYS redraw !
   return true;
}