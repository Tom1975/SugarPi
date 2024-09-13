
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <filesystem>

#include "res/resource.h"

/// <summary>
/// 
/// ConfigKeyboard:
/// 
/// This tool permit to construct keyboard config file. This file will associate each Amstrad key (identified by it's link to a 
/// line / bit in the matrix) to a scan code. 
/// What can be done : 
/// - Choose the keyboard layout display (FR, ES, EN, DA) to ease the selection.
/// - Load a key config. Each key connected will be displayed in a tooltip window
/// - Assign the configuration : When you click on a key, a dialog box will offer different way to assign the scan code (by value, by pressing a key...)
/// - Save the config
/// 
/// </summary>


typedef enum {
   KEYB_FR,
   KEYB_EN,
   KEYB_ES,
   KEYB_DA
} tKeyboardType;

HBITMAP hbmp = nullptr;
HBITMAP hBitmap = nullptr;
HBITMAP hOldBmp = nullptr;
HBITMAP hBmpMono = nullptr;
HBITMAP hMaskBmp = nullptr;
HBITMAP OldBmpMono = nullptr;
HBITMAP hOldMask = nullptr;
HBITMAP hBmpMasqueMonochrome = nullptr;
HBITMAP OldMasqueMono = nullptr;

HBITMAP hCurrentLayoutBmp = nullptr;

HDC MemDC = nullptr;
HDC hDCMasqueMono = nullptr;
HDC hMaskDC = nullptr;
HDC hDCMono = nullptr;
HWND hWnd = nullptr;
HWND hComboLayout = nullptr;

HINSTANCE g_hInst = nullptr;
unsigned int PixelClicked = 0;

void Draw(HDC dc);
void LoadConfig(std::filesystem::path path);

#define POS_KEYBOARD_X  60
#define POS_KEYBOARD_Y  200

class KeyboardLayout
{
public:
   KeyboardLayout(const char* name, unsigned int resource) : _name(name), _resource (resource)
   {
      _hbmp = reinterpret_cast<HBITMAP>(LoadImage(
         GetModuleHandle(NULL),
         MAKEINTRESOURCE(_resource),
         IMAGE_BITMAP,
         0,
         0,
         0));

   }
   std::string _name;
   HBITMAP _hbmp;
   unsigned int _resource;
};

std::vector<KeyboardLayout> _list_layout = {
   KeyboardLayout("English",IDB_KEYB_EN),
   KeyboardLayout("French",IDB_KEYB_FR),
   KeyboardLayout("Spanish",IDB_KEYB_ES),
   KeyboardLayout("Danish",IDB_KEYB_DA),
};

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
      // Create child windows
      // - Combobox with layouts
      // - "open" "save" config files
      break;
   }
   case WM_COMMAND:
      if (HIWORD(wParam) == CBN_SELCHANGE)
         // If the user makes a selection from the list:
         //   Send CB_GETCURSEL message to get the index of the selected list item.
         //   Send CB_GETLBTEXT message to get the item.
         //   Display the item in a messagebox.
      {
         int ItemIndex = SendMessage((HWND)lParam, (UINT)CB_GETCURSEL,
            (WPARAM)0, (LPARAM)0);
         TCHAR  ListItem[256];
         hCurrentLayoutBmp = (HBITMAP)ComboBox_GetItemData((HWND)lParam, ItemIndex);
         InvalidateRect(hWnd, 0, TRUE);
         UpdateWindow(hWnd);
      }
      break;
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
      Draw(hdc);

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

   g_hInst = hInstance;
   MyRegisterClass(hInstance);

   hWnd = CreateWindowEx(0, "KeyboardConfigurator", "Keyboard Configurator", WS_VISIBLE|WS_OVERLAPPEDWINDOW /*WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU |WS_BORDER*/ | WS_CLIPCHILDREN,
      CW_USEDEFAULT, CW_USEDEFAULT, 800, 480, NULL, NULL, hInstance, 0); // NULL);
   

   hComboLayout = CreateWindowEx(WS_EX_STATICEDGE, "COMBOBOX", "MyCombo1",
      CBS_DROPDOWN | WS_CHILD | WS_VISIBLE,
      20, 20, 200, 20, hWnd, 0, hInstance, NULL); // 100 = ID of this control

   int i = 0;
   for (auto& it : _list_layout)
   {
      // Add to combobox
      SendMessage(hComboLayout, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)it._name.c_str());
      ComboBox_SetItemData(hComboLayout, i, it._hbmp);
      i++;
   }

   HDC hDC = GetDC(NULL);
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

}


void Draw(HDC dc)
{
   // Display selected keyboard.
   
   HDC memDC = CreateCompatibleDC(dc);
   HBITMAP oldBmp = (HBITMAP) SelectObject(memDC, hCurrentLayoutBmp);
   BitBlt(dc, POS_KEYBOARD_X, POS_KEYBOARD_Y, 640, 178, memDC, 0, 0, SRCCOPY);
   SelectObject(memDC, oldBmp);
   DeleteDC(memDC);

}

void LoadConfig(std::filesystem::path path)
{
   // Load config
   // Assign evey key
}

void AssignTooltip()
{
   HWND hwndTT = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL,
      WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
      CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
      hWnd, NULL, g_hInst, NULL);

   SetWindowPos(hwndTT, HWND_TOPMOST, 0, 0, 0, 0,
      SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

   TOOLINFO ti = { 0 };
   ti.cbSize = sizeof(TOOLINFO);
   ti.uFlags = TTF_SUBCLASS;
   ti.hwnd = hWnd;
   ti.hinst = g_hInst;
   ti.lpszText = TEXT("This is your tooltip string.");

   GetClientRect(hWnd, &ti.rect);

   // Associate the tooltip with the "tool" window.
   SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&ti);
}
