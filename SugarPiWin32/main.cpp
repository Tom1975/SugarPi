#include <windows.h>

#include "DisplayPiDesktop.h"
#include "KeyboardHardwareImplemetationWin.h"
#include "emulation.h"

////////////////////////////////////////////////////////////
bool end = false;


class EmualtionWin32
{
public:
   CLogger* log;
   DisplayPiDesktop* display;
   SoundPi* sound;
   KeyboardPi* keyboard;
   KeyboardHardwareImplemetationWin* keyboardImp;
   Emulation* emulation;
   int nCore;
};


HHOOK _k_hook = nullptr;
EmualtionWin32* emu_hook = nullptr;
LRESULT __stdcall k_Callback1(int nCode, WPARAM wParam, LPARAM lParam)
{
   PKBDLLHOOKSTRUCT key = (PKBDLLHOOKSTRUCT)lParam;
   //a key was pressed

   switch (wParam)
   {
      case WM_KEYDOWN:
      case WM_SYSKEYDOWN:
         if (key->scanCode == VK_LWIN)
         {
            emu_hook->keyboardImp->CodeActionSpecial(VK_LWIN, true);
            return 1;
         }
            
         emu_hook->keyboardImp->Presskey(key->scanCode);

         break;
      case WM_SYSKEYUP:
      case WM_KEYUP:
         if (key->scanCode == VK_LWIN)
         {
            emu_hook->keyboardImp->CodeActionSpecial(VK_LWIN, false);
            return 1;
         }
         emu_hook->keyboardImp->Unpresskey(key->scanCode);
         break;
   }

   return CallNextHookEx(NULL, nCode, wParam, lParam);
}



LRESULT CALLBACK WndProcFrame(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

   EmualtionWin32* emu = reinterpret_cast<EmualtionWin32*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

   switch (message)
   {
   case WM_CREATE:
   {
      CREATESTRUCT* pCreateStr = (CREATESTRUCT*)lParam;
      SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pCreateStr->lpCreateParams);
      emu_hook = reinterpret_cast<EmualtionWin32*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
      _k_hook = SetWindowsHookEx(WH_KEYBOARD_LL, k_Callback1, NULL, 0);
      break;
   }
   case WM_KEYDOWN:
      //Check just the key for joypad
      if (emu->emulation->IsInMenu() || (wParam == VK_LWIN))
      {
         emu->keyboardImp->CodeActionSpecial(wParam, true);
      }
      break;
   case WM_KEYUP:
      //Check just the key for joypad
      if (emu->emulation->IsInMenu()|| (wParam == VK_LWIN))
      {
         emu->keyboardImp->CodeActionSpecial(wParam, false);
      }
      break;
   case WM_SETFOCUS:
      if (_k_hook == nullptr)
         _k_hook = SetWindowsHookEx(WH_KEYBOARD_LL, k_Callback1, NULL, 0);
      return DefWindowProc(hWnd, message, wParam, lParam);
   case WM_KILLFOCUS:
      if (_k_hook != nullptr)
         UnhookWindowsHookEx(_k_hook);
      _k_hook = nullptr;
      return DefWindowProc(hWnd, message, wParam, lParam);
   case WM_QUIT:
      break;
   case WM_DESTROY:
      end = true;
      if (_k_hook !=nullptr)
         UnhookWindowsHookEx(_k_hook);
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

// Windows stuff : Registration, initialisation
ATOM MyRegisterClass(HINSTANCE hInstance)
{
   WNDCLASSEX wcex;

   wcex.cbSize = sizeof(WNDCLASSEX);

   wcex.style = CS_HREDRAW | CS_VREDRAW;
   wcex.lpfnWndProc = WndProcFrame;
   wcex.cbClsExtra = 0;
   wcex.cbWndExtra = 0;
   wcex.hInstance = hInstance;
   wcex.hIcon = NULL;
   wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
   wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
   wcex.lpszMenuName = NULL;
   wcex.lpszClassName = "SugarPi";
   wcex.hIconSm = LoadIcon(wcex.hInstance, NULL);

   return RegisterClassEx(&wcex);
}

void Run(EmualtionWin32* emulation)
{
   GdiplusStartupInput gdiplusStartupInput;
   ULONG_PTR           gdiplusToken;

   // Create the various objects that will interract with the core
   GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);


   emulation->emulation->Run(emulation->nCore);

   GdiplusShutdown(gdiplusToken);

}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
   // Create the core
   EmualtionWin32 emu;

   emu.log = new CLogger(LogDebug);
   emu.display = new DisplayPiDesktop (emu.log);

   // Sound 
   emu.sound = new SoundPi(emu.log);

   // Keyboard
   emu.keyboard = new KeyboardPi(emu.log);
   emu.keyboardImp = new KeyboardHardwareImplemetationWin(emu.keyboard);
   emu.keyboard->SetHard(emu.keyboardImp);

   MyRegisterClass(hInstance);
   emu.emulation = new Emulation(emu.log);

   HWND _hwnd = CreateWindowEx(0, "SugarPi", "SugarPi", WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_VISIBLE,
      CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, &emu); // NULL);

   emu.display->Init(hInstance, _hwnd, 0);

   // Launch the message pump
   // emulation
   emu.emulation->Initialize(emu.display, emu.sound, emu.keyboard);	// must be initialized at last


   // 
   emu.nCore = 1;
   EmualtionWin32 emu_sound = emu;
   emu_sound.nCore = 0;
   EmualtionWin32 emu_disp = emu;
   emu_disp.nCore = 2;
   EmualtionWin32 delayed_init = emu;
   delayed_init.nCore = 3;

   std::thread sound_core(Run, &emu_sound); //Main thread
   std::thread disp_core(Run, &emu_disp); //Main thread
   std::thread delayed_core(Run, &delayed_init); //Main thread
   std::thread main_core(Run, &emu); //Main thread

   // Quit.
   MSG msg;
   while (GetMessage(&msg, _hwnd, 0, 0) && !end)
   {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
   }

   emu.emulation->ForceStop();

   main_core.join();
   delayed_core.join();
   sound_core.join();
   disp_core.join();

   delete emu.keyboard;
   delete emu.display;
   delete emu.log;

}
