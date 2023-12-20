#include <windows.h>

#include "DisplayPiDesktop.h"

#include "emulation.h"

////////////////////////////////////////////////////////////

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   return DefWindowProc(hWnd, message, wParam, lParam);
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

   HWND _hwnd = CreateWindowEx(0, "SugarPi", "SugarPi", WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_VISIBLE,
      CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, NULL, NULL, hInstance, 0); // NULL);

   display->Init(hInstance, _hwnd, 0);

   // Launch the message pump
   // emulation
   Emulation emulation(log);
   emulation.Initialize(display, sound, keyboard);	// must be initialized at last

   std::thread main_core(Run, &emulation);

   // Quit.
   MSG msg;
   bool end = false;
   while (!end)
   {
      while (PeekMessage(&msg, NULL, 0, 0, TRUE))
      {
         TranslateMessage(&msg);
         DispatchMessage(&msg);
      }
   }

   delete keyboard;
   delete display;
   delete log;

}
