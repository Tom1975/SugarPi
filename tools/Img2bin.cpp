// ConsoleApplication1.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include <windows.h>
#include <wincodec.h>
#include <commdlg.h>
#include <d2d1.h>
#include <d2d1helper.h>

int main()
{
   HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
   
   IWICImagingFactory* m_pIWICFactory;

   if (SUCCEEDED(hr))
   {
      // Create WIC factory
      hr = CoCreateInstance(
         CLSID_WICImagingFactory,
         NULL,
         CLSCTX_INPROC_SERVER,
         IID_PPV_ARGS(&m_pIWICFactory)
      );



      CoUninitialize();
   }

   return 0;
}