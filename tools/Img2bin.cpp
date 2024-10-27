// ConsoleApplication1.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include <windows.h>
#include <wincodec.h>
#include <commdlg.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <iostream>
#include <filesystem>

int wmain(int argc, wchar_t* argv[])
{
   // decode input name
   if (argc < 2)
   {
      std::cout << "Need an input file !" << std::endl;
   }

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

      // Create a decoder
      IWICBitmapDecoder* pDecoder = NULL;

      hr = m_pIWICFactory->CreateDecoderFromFilename(
         argv[1],                      // Image to be decoded
         NULL,                            // Do not prefer a particular vendor
         GENERIC_READ,                    // Desired read access to the file
         WICDecodeMetadataCacheOnDemand,  // Cache metadata when needed
         &pDecoder                        // Pointer to the decoder
      );

      // Retrieve the first frame of the image from the decoder
      IWICBitmapFrameDecode* pFrame = NULL;

      if (SUCCEEDED(hr))
      {
         hr = pDecoder->GetFrame(0, &pFrame);
      }


      // Get the Pixel Format
      WICPixelFormatGUID pPixelFormat;
      
      pFrame->GetPixelFormat(&pPixelFormat);

      // Now get a POINTER to an instance of the Pixel Format    
      IWICComponentInfo* pIComponentInfo = NULL;
      if (SUCCEEDED(hr))
      {
         hr = m_pIWICFactory->CreateComponentInfo(pPixelFormat, &pIComponentInfo);
      }

      // Get IWICPixelFormatInfo from IWICComponentInfo
      IWICPixelFormatInfo* pIPixelFormatInfo;

      hr = pIComponentInfo->QueryInterface(__uuidof(IWICPixelFormatInfo), reinterpret_cast<void**>(&pIPixelFormatInfo));

      // Now get the Bits Per Pixel
      UINT bitsPerPixel;
      if (SUCCEEDED(hr))
      {
         hr = pIPixelFormatInfo->GetBitsPerPixel(&bitsPerPixel);
      }

      UINT width;
      UINT height;
      if (SUCCEEDED(hr))
      {
         hr = pFrame->GetSize(&width, &height);
      }

      IWICBitmap* pIBitmap = NULL;
      IWICBitmapLock* pILock = NULL;


      WICRect rcLock = { 0, 0, width, height }; // from GetSize earlier

      // Create the bitmap from the image frame.
      if (SUCCEEDED(hr))
      {
         hr = m_pIWICFactory->CreateBitmapFromSource(
            pFrame,          // Create a bitmap from the image frame
            WICBitmapCacheOnDemand,  // Cache metadata when needed
            &pIBitmap);              // Pointer to the bitmap
      }

      if (SUCCEEDED(hr))
      {
         hr = pIBitmap->Lock(&rcLock, WICBitmapLockRead, &pILock);
      }

      // Now get the stride from the lock.
      UINT stride;
      pILock->GetStride(& stride);
      UINT size;
      BYTE* bmp_ptr;
      pILock->GetDataPointer(&size, &bmp_ptr);

      std::wstring out_file(argv[1]);
      out_file += L".bin";

      const wchar_t* ptr_w = out_file.c_str();

      std::mbstate_t state = std::mbstate_t();
      std::size_t len = 1 + std::wcsrtombs(nullptr, &ptr_w, 0, &state);
      std::vector<char> mbstr(len);
      std::wcsrtombs(&mbstr[0], &ptr_w, mbstr.size(), &state);

      FILE* f = fopen(&mbstr[0], "w+b");
      if (f != nullptr)
      {
         fwrite(&width, sizeof (width), 1, f);
         fwrite(&height, sizeof(height), 1, f);
         fwrite(bmp_ptr, size, 1, f);
         fclose(f);
      }
      

      CoUninitialize();
   }

   return 0;
}