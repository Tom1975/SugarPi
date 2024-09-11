
#include <windows.h>

#include "res/resource.h"

typedef enum {
   KEYB_FR,
   KEYB_EN,
   KEYB_ES,
   KEYB_DA
} tKeyboardType;

HBITMAP hBitmap = nullptr;
HBITMAP hOldBmp = nullptr;
HBITMAP hBmpMono = nullptr;
HBITMAP hMaskBmp = nullptr;
HBITMAP OldBmpMono = nullptr;
HBITMAP hOldMask = nullptr;
HBITMAP hBmpMasqueMonochrome = nullptr;
HBITMAP OldMasqueMono = nullptr;

HDC MemDC = nullptr;
HDC hDCMasqueMono = nullptr;
HDC hMaskDC = nullptr;
HDC hDCMono = nullptr;
HWND hWnd = nullptr;

unsigned int PixelClicked = 0;

void Draw();

////////////////////////////////////////////////////////////
// Windows stuff : Registration, initialisation

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   //	int wmId, wmEvent;
   PAINTSTRUCT ps;
   HDC hdc;

   // Get instance ID
   //CSugarMainWindow* pMainWindow = (CSugarMainWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
   //CSugarMainWindow* pMainWindow = sMainWindow;

   switch (message)
   {
   case WM_CREATE:
   {
      CREATESTRUCT* pCreateStr = (CREATESTRUCT*)lParam;
      SetWindowLongPtr(hWnd, GWLP_USERDATA,(LONG)pCreateStr->lpCreateParams);
      break;
   }

   case WM_LBUTTONDOWN:
      SetFocus(hWnd);
      break;
   case WM_LBUTTONUP:
   {
      break;
   }
   case WM_MOUSEMOVE:
   {
      break;
   }

   case WM_SYSKEYDOWN:
   case WM_KEYDOWN:

      break;
   case WM_SYSKEYUP:
   case WM_KEYUP:
      break;
   case WM_ERASEBKGND:
      return TRUE;
      break;
   case WM_CTLCOLORDLG:
      return TRUE;
      break;

   case WM_PAINT:
      hdc = BeginPaint(hWnd, &ps);

      // Display the current keyboard.
      Draw();

      EndPaint(hWnd, &ps);
      break;

   case WM_DESTROY:
      // Save configurartion
      PostQuitMessage(0);
      break;

   default:
      return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

////////////////////////////////////////////////////////////
// Registration of class

ATOM MyRegisterClass(HINSTANCE hInstance)
{
   WNDCLASSEX wcex;

   wcex.cbSize = sizeof(WNDCLASSEX);

   wcex.style = CS_HREDRAW | CS_VREDRAW;
   wcex.lpfnWndProc = WndProc;
   wcex.cbClsExtra = 0;
   wcex.cbWndExtra = 0;
   wcex.hInstance = hInstance;
   wcex.hIcon = nullptr;
   wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
   wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
   wcex.lpszMenuName = nullptr;
   wcex.lpszClassName = "KeyboardConfigurator";
   wcex.hIconSm = nullptr;

   return RegisterClassEx(&wcex);
}

////////////////////////////////////////////////////////////
// Main
int APIENTRY WinMain(HINSTANCE hInstance,
   HINSTANCE hPrevInstance,
   LPTSTR    lpCmdLine,
   int       nCmdShow)
{
   UNREFERENCED_PARAMETER(hPrevInstance);
   UNREFERENCED_PARAMETER(lpCmdLine);

   MyRegisterClass(hInstance);

   hWnd = CreateWindowEx(0, "KeyboardConfigurator", "Keyboard Configurator", WS_VISIBLE|WS_OVERLAPPEDWINDOW /*WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU |WS_BORDER*/ | WS_CLIPCHILDREN,
      CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, NULL, NULL, hInstance, 0); // NULL);
   
   HDC hDC = GetDC(hWnd);
   MemDC = CreateCompatibleDC(hDC);
   // Create mask
   hMaskDC = CreateCompatibleDC(hDC);
   hMaskBmp = reinterpret_cast<HBITMAP>(LoadImage(
      GetModuleHandle(NULL),
      MAKEINTRESOURCE(IDB_KEYB_MASK),
      IMAGE_BITMAP,
      0,
      0,
      0));

   hDCMono = CreateCompatibleDC(hDC);
   hBmpMono = CreateBitmap(640, 178, 1, 1, NULL);
   OldBmpMono = (HBITMAP)SelectObject(hDCMono, hBmpMono);

   hOldMask = (HBITMAP)SelectObject(hMaskDC, hMaskBmp);

   hDCMasqueMono = CreateCompatibleDC(hDC);
   hBmpMasqueMonochrome = CreateCompatibleBitmap(hDC, 640, 178);
   OldMasqueMono = (HBITMAP)SelectObject(hDCMasqueMono, hBmpMasqueMonochrome);

   ReleaseDC(hWnd, hDC);


   MSG msg;

   while (GetMessage(&msg, NULL, 0, 0) > 0)
   {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
   }
   return 0;
}


void SetKeyboardType(tKeyboardType keyboardType)
{
   // Free current bitmap
   if (hBitmap != NULL)
   {
      SelectObject(MemDC, hOldBmp);
      DeleteObject(hBitmap);
   }

   unsigned int res = IDB_KEYB;
   if (keyboardType == KEYB_FR)
   {
      res = IDB_KEYB_FR;
   }
   else if (keyboardType == KEYB_EN)
   {
      res = IDB_KEYB_EN;
   }
   else if (keyboardType == KEYB_ES)
   {
      res = IDB_KEYB_ES;
   }
   else if (keyboardType == KEYB_DA)
   {
      res = IDB_KEYB_DA;
   }
   
   PixelClicked = 0xFFFFFF;

   // Load bitmap
   hBitmap = reinterpret_cast<HBITMAP>(LoadImage(
      GetModuleHandle(NULL),
      MAKEINTRESOURCE(res),
      IMAGE_BITMAP,
      0,
      0,
      0));
   hOldBmp = (HBITMAP)SelectObject(MemDC, hBitmap);

   Draw();

}


void Draw()
{
   HDC hDC = GetDC(hWnd);

   BitBlt(hDC, 0, 0, 640, 178, MemDC, 0, 0, SRCCOPY);

   // Revert any key ?
   if (PixelClicked != 0xFFFFFF)
   {
      PatBlt(hDCMasqueMono, 0, 0, 640, 178, BLACKNESS);
      TransparentBlt(hDCMasqueMono, 0, 0, 640, 178, hMaskDC, 0, 0, 640, 178, PixelClicked);

      SetBkMode(hDCMasqueMono, OPAQUE);
      SetBkColor(hDCMasqueMono, 0);
      BitBlt(hDCMono, 0, 0, 640, 178, hDCMasqueMono, 0, 0, SRCCOPY);

      MaskBlt(hDC, 0, 0, 640, 178, MemDC, 0, 0, hBmpMono, 0, 0, MAKEROP4(DSTINVERT, SRCCOPY/*0x00AA0029*/));
   }


   ReleaseDC(hWnd, hDC);

}