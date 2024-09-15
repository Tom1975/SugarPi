
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
/// MAP color : each color is defined as :
/// 0-0-0 or FF-FF-FF : nothing
/// RGB : R = Line, G = bit. Ex : Esc = R:8, V:2
/// 
/// 
/// </summary>

unsigned char default_raw_map[10][8] =
{
   {0x52, 0x4F, 0x51, 0x61, 0x5E, 0x5B, 0x58, 0x63, },   // Cur_up Cur_right Cur_down F9 F6 F3 Enter F.
   {0x50, 0xE2, 0x5F, 0x60, 0x5D, 0x59, 0x5A, 0x62, },   // cur_left Copy f7 f8 f5 f1 f2 f0
   {0x4C, 0x30, 0x28, 0x32, 0x5C, 0xE5, 0x38, 0xE0, },   // Clr {[ Return }] F4 Shift `\ Ctrl
   {0x2E, 0x2D, 0x2F, 0x13, 0x34, 0x33, 0x2E, 0x37, },   // ^£ =- |@ P +; *: ?/ >,
   {0x27, 0x26, 0x12, 0x0C, 0x0F, 0x0E, 0x10, 0x36, },   // _0 )9 O I L K M <.
   {0x25, 0x24, 0x18, 0x1C, 0x0B, 0x0D, 0x11, 0x2C, },   // (8 '7 U Y H J N Space
   {0x23, 0x22, 0x15, 0x17, 0x0A, 0x09, 0x05, 0x19, },   // &,6,Joy1_Up %,5,Joy1_down, R,Joy1_Left T,Joy1_Right G,Joy1Fire2 F,Joy1Fire1 B V
   {0x21, 0x20, 0x08, 0x1A, 0x16, 0x07, 0x06, 0x1B, },   // $4 #3 E W S D C X
   {0x1E, 0x1F, 0x29, 0x14, 0x2B, 0x04, 0x39, 0x1D, },   // !1 "2 Esc Q Tab A CapsLock Z
   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2A, }    // Joy0up Joy0down Joy0left Joy0right Joy0F1 Joy0F2 unused Del
};

int selected_line = -1;
int selected_bit = -1;

unsigned char keyboard_lines_[10]; 
struct RawToCPC
{
   unsigned char* line_index;
   unsigned char bit;
};
RawToCPC raw_to_cpc_map_[0x100];


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
HWND hButtonLoad = nullptr;
HWND hButtonSave = nullptr;

HINSTANCE g_hInst = nullptr;
unsigned int PixelClicked = 0;

void ReDrawCharacter(HDC dc, int line, int bit, bool select);
void DrawCharacter(HDC dc, int x, int y, bool select = false);
void Draw(HDC dc);
void LoadConfig(std::filesystem::path path);
void SaveConfig(std::filesystem::path path);
void LoadConfig();
void SaveConfig();
void InitKeyboard(unsigned char key_map[10][8]);

#define KEYBOARD_WIDTH 640
#define KEYBOARD_HEIGHT 178

#define LINE_HEIGHT 20
#define COLUMN_WIDTH 30

#define POS_KEYBOARD_X  60
#define POS_KEYBOARD_Y  320

#define POS_MATRIX_X 60
#define POS_MATRIX_Y 60

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
      else
      {
         if (hButtonLoad == (HWND)lParam)
         {
            // Load 
            LoadConfig();
         }
         else if (hButtonSave == (HWND)lParam)
         {
            // Save
            SaveConfig();
         }
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
      int xPos = GET_X_LPARAM(lParam);
      int yPos = GET_Y_LPARAM(lParam);
      // are we over the keyboard ?
      if (xPos >= POS_KEYBOARD_X && xPos <= POS_KEYBOARD_X + KEYBOARD_WIDTH
         && yPos >= POS_KEYBOARD_Y && yPos <= POS_KEYBOARD_Y + KEYBOARD_HEIGHT)
      {
         xPos -= POS_KEYBOARD_X;
         yPos -= POS_KEYBOARD_Y;
         COLORREF col = GetPixel(hMaskDC, xPos, yPos);
         int bit = GetGValue(col);
         int line = GetRValue(col);


         HDC dc = GetDC(hWnd);
         
         if (bit < 8 && line < 10)
         {
            RECT rect = { 60, 295, 160, 315 };
            char coltxt[16];

            sprintf(coltxt, "%i-%i =%2.2X", bit, line, default_raw_map[line][bit]);
            DrawStatusTextA(dc, &rect, coltxt, SBT_POPOUT);

            if (bit == selected_bit && line == selected_line)
            {

            }
            else
            {
               if (selected_line != -1 && selected_bit != -1)
               {
                  ReDrawCharacter(dc, selected_line, selected_bit, false);
               }
               selected_bit = bit;
               selected_line = line;
               ReDrawCharacter(dc, selected_line, selected_bit, true);
            }
         }
         else
         {
            int bit = selected_bit;
            int line = selected_line;
            selected_bit = -1;
            selected_line = -1;
            ReDrawCharacter(dc, line, bit, false);
         }
         ReleaseDC(hWnd, dc);

         // - if it's different from previous one, redraw previous one correctly
         //   - redraw new one with color, also.
         // - if it's the same, do nothing
      }

      // Are we over the matrix ?
      // - if it's different from previous one, redraw previous one correctly
      //   - redraw new one with color, also.
      // - if it's the same, do nothing
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
      CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, 0); // NULL);
   

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
   ComboBox_SetCurSel(hComboLayout, 0);
   hCurrentLayoutBmp = (HBITMAP)ComboBox_GetItemData(hComboLayout, 0);

   hButtonLoad = CreateWindow("BUTTON",  // Predefined class; Unicode assumed 
      "Load Config",      // Button text 
      WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
      240,         // x position 
      20,         // y position 
      100,        // Button width
      30,        // Button height
      hWnd,     // Parent window
      NULL,       // No menu.
      hInstance,
      NULL);      // Pointer not needed.

   hButtonSave = CreateWindow("BUTTON",  // Predefined class; Unicode assumed 
      "Save Config",      // Button text 
      WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
      360,         // x position 
      20,         // y position 
      100,        // Button width
      30,        // Button height
      hWnd,     // Parent window
      NULL,       // No menu.
      hInstance,
      NULL);      // Pointer not needed.

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
   hBmpMono = CreateBitmap(KEYBOARD_WIDTH, KEYBOARD_HEIGHT, 1, 1, NULL);
   OldBmpMono = (HBITMAP)SelectObject(hDCMono, hBmpMono);

   hOldMask = (HBITMAP)SelectObject(hMaskDC, hMaskBmp);

   hDCMasqueMono = CreateCompatibleDC(hDC);
   hBmpMasqueMonochrome = CreateCompatibleBitmap(hDC, KEYBOARD_WIDTH, KEYBOARD_HEIGHT);
   OldMasqueMono = (HBITMAP)SelectObject(hDCMasqueMono, hBmpMasqueMonochrome);

   ReleaseDC(hWnd, hDC);
   MSG msg;

   InitKeyboard(default_raw_map);

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

void ReDrawCharacter(HDC dc, int line, int bit, bool select)
{
   DrawCharacter(dc, line + 1, 8 - bit, select);
}

/// <summary>
/// Draw a single character on the matrix
/// </summary>
/// <param name="dc"></param>
/// <param name="x"></param>
/// <param name="y"></param>
void DrawCharacter(HDC dc, int x, int y, bool select )
{
   RECT rect;
   rect.left = POS_MATRIX_X + x * COLUMN_WIDTH;
   rect.right = rect.left + COLUMN_WIDTH;
   rect.top = POS_MATRIX_Y + (1 + y) * LINE_HEIGHT;
   rect.bottom = rect.top + LINE_HEIGHT;

   int bit = 8 - y;
   int line = x - 1;
   char txt[16];
   if (x == 0 && y == 0)
   {
      strcpy(txt, "");
   }
   else if (x == 0)
   {
      sprintf(txt, "%i", bit);
   }
   else if (y == 0)
   {
      sprintf(txt, "%i", line);
   }
   else
   {
      bool sel = ( bit == selected_bit ) && (line == selected_line && select);
      sprintf(txt, "%2.2X", default_raw_map[line][bit]);
      SetBkColor(dc, sel ? 0xEE0000: 0x00FF00);
      SetTextColor(dc, sel ? 0xFFFFFF:0x0000FF);

   }
   DrawText(dc, txt, -1, &rect, DT_CENTER);
}

void Draw(HDC dc)
{

   // Display selected keyboard.
   HDC memDC = CreateCompatibleDC(dc);
   HBITMAP oldBmp = (HBITMAP) SelectObject(memDC, hCurrentLayoutBmp);
   BitBlt(dc, POS_KEYBOARD_X, POS_KEYBOARD_Y, KEYBOARD_WIDTH, KEYBOARD_HEIGHT, memDC, 0, 0, SRCCOPY);
   SelectObject(memDC, oldBmp);
   DeleteDC(memDC);

   // Draw bit/line matrix
   RECT rect = { POS_MATRIX_X , POS_MATRIX_Y, POS_MATRIX_X + 360, POS_MATRIX_Y + LINE_HEIGHT };
   DrawText(dc, "Line", -1, &rect, DT_CENTER);
   rect.right = POS_MATRIX_X + COLUMN_WIDTH;
   DrawText(dc, "Bit", -1, &rect, DT_CENTER);

   for (int x = 0; x < 11; x++)
   {
      for (int y = 0; y < 9; y++)
      {
         bool select = (selected_line == x-1 && selected_bit == y-1);
         DrawCharacter(dc, x, y, select);
      }
   }

}

void LoadConfig()
{
   // Select file
   OPENFILENAME ofn;
   TCHAR szFile[260];       // buffer for file name

   // Initialize OPENFILENAME
   ZeroMemory(&ofn, sizeof(ofn));
   ofn.lStructSize = sizeof(ofn);
   ofn.hwndOwner = hWnd;
   ofn.lpstrFile = szFile;
   // Set lpstrFile[0] to '\0' so that GetOpenFileName does not
   // use the contents of szFile to initialize itself.
   ofn.lpstrFile[0] = '\0';
   ofn.nMaxFile = sizeof(szFile);
   ofn.lpstrFilter = "Config file\0.bin\0";
   ofn.nFilterIndex = 1;
   ofn.lpstrFileTitle = NULL;
   ofn.nMaxFileTitle = 0;
   ofn.lpstrInitialDir = NULL;
   ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

   if (GetOpenFileName(&ofn) == TRUE)
   {
      LoadConfig(ofn.lpstrFile);
   }

   // load
}

void SaveConfig()
{
   // select file
      // Select file
   OPENFILENAME ofn;
   TCHAR szFile[260];       // buffer for file name

   // Initialize OPENFILENAME
   ZeroMemory(&ofn, sizeof(ofn));
   ofn.lStructSize = sizeof(ofn);
   ofn.hwndOwner = hWnd;
   ofn.lpstrFile = szFile;
   // Set lpstrFile[0] to '\0' so that GetOpenFileName does not
   // use the contents of szFile to initialize itself.
   ofn.lpstrFile[0] = '\0';
   ofn.nMaxFile = sizeof(szFile);
   ofn.lpstrFilter = "Config file\0.bin\0";
   ofn.nFilterIndex = 1;
   ofn.lpstrFileTitle = NULL;
   ofn.nMaxFileTitle = 0;
   ofn.lpstrInitialDir = NULL;
   ofn.Flags = OFN_OVERWRITEPROMPT;

   if (GetOpenFileName(&ofn) == TRUE)
   {
      SaveConfig(ofn.lpstrFile);
   }
   // 
   // save
}

unsigned int getline(const char* buffer, int size, std::string& out)
{
   if (size == 0)
   {
      return 0;
   }

   // looking for /n
   int offset = 0;
   while (buffer[offset] != 0x0A && buffer[offset] != 0x0D && offset < size)
   {
      offset++;
   }

   char* line = new char[offset + 1];
   memcpy(line, buffer, offset);
   line[offset] = '\0';
   out = std::string(line);
   delete[]line;
   return (offset == size) ? offset : offset + 1;
}

void LoadConfig(std::filesystem::path path)
{
   // Open file
   FILE* f;
   f= fopen(path.string().c_str(), "r+b");
   if (f != nullptr)
   {
      MessageBox( hWnd, "File read error !", "Error", MB_OK);
      return;
   }

   // Load every known gamepad to internal structure
   fseek(f, 0, SEEK_END);
   unsigned int buffer_size = ftell(f);
   rewind(f);
   unsigned char* buff = new unsigned char[buffer_size];
   unsigned nBytesRead;

   if (fread(buff, buffer_size, 1, f) != nBytesRead)
   {
      // ERROR
      fclose(f);
      MessageBox(hWnd, "KeyboardPi", "Error reading keyboard layout file", MB_OK);
      return;
   }

   // get next line
   const char* ptr_buffer = (char*)buff;
   unsigned int offset = 0;
   unsigned int end_line;
   std::string s;
   int line_index = 0;
   while ((end_line = getline(&ptr_buffer[offset], nBytesRead, s)) > 0 && line_index < 8)
   {
      nBytesRead -= end_line;

      // Do not use emty lines, and comment lines
      if (s.size() == 0 || s[0] == '#')
      {
         continue;
      }

      // Decode line to buffer
      for (unsigned int raw_key = 0; raw_key < 8 && (2 + raw_key * 3) < end_line; raw_key++)
      {
         char number[3];
         memcpy(number, &ptr_buffer[offset + raw_key * 3], 2);
         number[2] = '\0';
         unsigned char value = strtoul(number, NULL, 16);
         default_raw_map[line_index][raw_key] = value;
      }
      offset += end_line;
      line_index++;
   }
   delete[]buff;
   fclose(f);

   InitKeyboard(default_raw_map);

}

void SaveConfig(std::filesystem::path path)
{
   // Save config 
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

void InitKeyboard(unsigned char key_map[10][8])
{
   memset(raw_to_cpc_map_, 0, sizeof raw_to_cpc_map_);

   for (int line = 0; line < 10; line++)
   {
      for (int bit = 0; bit < 8; bit++)
      {
         unsigned char raw_key = key_map[line][bit];
         raw_to_cpc_map_[raw_key].line_index = &keyboard_lines_[line];
         raw_to_cpc_map_[raw_key].bit = 1 << bit;
      }
   }

   // Now, adjust tooltips from values of the keyboard


}