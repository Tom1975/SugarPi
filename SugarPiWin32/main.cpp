#include <windows.h>

#include "DisplayPiDesktop.h"

#include "emulation.h"

////////////////////////////////////////////////////////////
bool end = false;


class EmualtionWin32
{
public:
   CLogger* log;
   DisplayPi* display;
   SoundPi* sound;
   KeyboardPi* keyboard;
   Emulation* emulation;
};



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

   EmualtionWin32* emu = reinterpret_cast<EmualtionWin32*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

   switch (message)
   {
   case WM_CREATE:
   {
      CREATESTRUCT* pCreateStr = (CREATESTRUCT*)lParam;
      SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pCreateStr->lpCreateParams);
      break;
   }
   case WM_KEYDOWN:
      emu->keyboard->Presskey(wParam);
      break;
   case WM_KEYUP:
      emu->keyboard->Unpresskey(wParam);
      break;
   case WM_QUIT:
      break;
   case WM_DESTROY:
      end = true;
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
   wcex.lpfnWndProc = WndProc;
   wcex.cbClsExtra = 0;
   wcex.cbWndExtra = 0;
   wcex.hInstance = hInstance;
   wcex.hIcon = LoadIcon(hInstance, NULL);
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


   emulation->emulation->Run(0);

   GdiplusShutdown(gdiplusToken);

}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
   // Create the core
   EmualtionWin32 emu;

   emu.log = new CLogger(LogDebug);
   emu.display = new DisplayPi(emu.log);

   // Sound 
   emu.sound = new SoundPi(emu.log);

   // Keyboard
   emu.keyboard = new KeyboardPi(emu.log);

   MyRegisterClass(hInstance);
   emu.emulation = new Emulation(emu.log);

   HWND _hwnd = CreateWindowEx(0, "SugarPi", "SugarPi", WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_VISIBLE,
      CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, NULL, NULL, hInstance, &emu); // NULL);

   emu.display->Init(hInstance, _hwnd, 0);

   // Launch the message pump
   // emulation
   emu.emulation->Initialize(emu.display, emu.sound, emu.keyboard);	// must be initialized at last

   std::thread main_core(Run, &emu);

   // Quit.
   MSG msg;
   while (!end)
   {
      while (PeekMessage(&msg, _hwnd, 0, 0, TRUE))
      {
         TranslateMessage(&msg);
         DispatchMessage(&msg);
      }
   }

   emu.emulation->ForceStop();

   main_core.join();

   delete emu.keyboard;
   delete emu.display;
   delete emu.log;

}