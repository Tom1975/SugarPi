#include <windows.h>

#include "DisplayPiDesktop.h"

#include "emulation.h"

////////////////////////////////////////////////////////////
bool end = false;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   switch (message)
   {
   case WM_CREATE:
   {
      CREATESTRUCT* pCreateStr = (CREATESTRUCT*)lParam;
      SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG)pCreateStr->lpCreateParams);
      break;
   }
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

void Run(Emulation * emulation)
{
   GdiplusStartupInput gdiplusStartupInput;
   ULONG_PTR           gdiplusToken;

   // Create the various objects that will interract with the core
   GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);


   emulation->Run(0);

   GdiplusShutdown(gdiplusToken);

}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
   // Create the core
   CLogger* log = new CLogger(LogDebug);
   DisplayPi* display = new DisplayPi(log);

   // Sound 
   SoundPi* sound = new SoundPi(log);

   // Keyboard
   KeyboardPi* keyboard = new KeyboardPi(log);

   MyRegisterClass(hInstance);
   Emulation emulation(log);

   HWND _hwnd = CreateWindowEx(0, "SugarPi", "SugarPi", WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_VISIBLE,
      CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, NULL, NULL, hInstance, &emulation); // NULL);

   display->Init(hInstance, _hwnd, 0);

   // Launch the message pump
   // emulation
   emulation.Initialize(display, sound, keyboard);	// must be initialized at last

   std::thread main_core(Run, &emulation);

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

   emulation.ForceStop();

   main_core.join();

   delete keyboard;
   delete display;
   delete log;

}
