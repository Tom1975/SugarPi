#include <vector>

#include "DisplayPiDesktop.h"
#include "Shlwapi.h"
#include "dwmapi.h"

#define WAIT(x) std::this_thread::sleep_for(std::chrono::milliseconds(x));

#define DISP_WIDTH    1024
#define DISP_HEIGHT   576

#define DISP_WINDOW_X   800
#define DISP_WINDOW_Y   600//576

#define SCR_PATH "\\SCR\\"

DisplayPi::DisplayPi(CLogger* logger) : logger_(logger), m_pFSInt(NULL), sync_on_frame_(true), nb_frame_in_queue_(0)
{
   m_BlackScreen = false;
   m_pDiretories = NULL;
   m_X = m_Y = m_Width = m_Height = m_WidthWindow = m_HeightWindow = 0;
   m_XIn = 0;
   m_YIn = 0;
   SetSize(S_STANDARD);
   m_DnDType = 0;
   m_CurrentPart = 0;
   font_ = new CoolspotFont();

   pRT_ = nullptr;
   frame_buffer_ = nullptr;

   CoInitialize(NULL);

}

DisplayPi::~DisplayPi()
{
   if (pRT_)pRT_->Release();
   delete font_;
   delete frame_buffer_;
   CoUninitialize();
}

void DisplayPi::ReleaseAll()
{
   // release
   ReleaseDC(m_hWnd, m_hwndDC);

   //DeleteObject(m_iBitmap);
   //delete m_BmpMem;
}

void DisplayPi::WindowsToTexture(int& x, int& y)
{

}

void DisplayPi::SetSize(SizeEnum size)
{
   m_Size = size;
   switch (size)
   {
   case S_STANDARD:
      m_X = 159 - 16;
      m_Y = 47;
      //m_Y = 16;
      m_Width = 768;
      m_Height = 544;
      break;
   case S_BIG:
      m_X = 128;
      m_Y = 16;
      m_Width = DISP_WINDOW_X;
      m_Height = DISP_WINDOW_Y;
      break;
   case S_MAX:
      m_X = 0;
      m_Y = 0;
      m_Width = DISP_WIDTH;
      m_Height = DISP_HEIGHT;
      break;
   }
}

int DisplayPi::GetWidth()
{
   return m_Width;
}

int DisplayPi::GetHeight()
{
   return m_Height; //REAL_DISP_Y;
}

int* DisplayPi::GetVideoBuffer(int y) 
{
   return (int*) & frame_buffer_[(y) * REAL_DISP_X];
}

void DisplayPi::Reset() 
{
   memset(&frame_buffer_[REAL_DISP_X * REAL_DISP_Y* FRAME_BUFFER_SIZE], 0, REAL_DISP_X * REAL_DISP_Y * FRAME_BUFFER_SIZE);
}


void DisplayPi::Init(HINSTANCE hInstance, HWND hWnd, IFullScreenInterface* pFSInt)
{

   ID2D1Factory* pD2DFactory = NULL;
   HRESULT hr = D2D1CreateFactory(
      D2D1_FACTORY_TYPE_SINGLE_THREADED,
      &pD2DFactory
   );


   m_hWnd = hWnd;
   
   // Obtain the size of the drawing area.
   RECT rc;
   GetClientRect(m_hWnd, &rc);

   // Create a Direct2D render target          
   
   hr = pD2DFactory->CreateHwndRenderTarget(
      D2D1::RenderTargetProperties(),
      D2D1::HwndRenderTargetProperties(
         m_hWnd,
         D2D1::SizeU(
            rc.right - rc.left,
            rc.bottom - rc.top)
      ),
      &pRT_
   );
   pD2DFactory->Release();

   frame_buffer_ = (unsigned int*)malloc(
      REAL_DISP_X * REAL_DISP_Y*4*2);

   D2D1_SIZE_U size = { 0 };
   D2D1_BITMAP_PROPERTIES props;
   pRT_->GetDpi(&props.dpiX, &props.dpiY);
   D2D1_PIXEL_FORMAT pixelFormat = D2D1::PixelFormat(
      DXGI_FORMAT_B8G8R8A8_UNORM,
      D2D1_ALPHA_MODE_IGNORE
   );
   props.pixelFormat = pixelFormat;
   size.width = REAL_DISP_X;
   size.height = REAL_DISP_Y;

   hr = pRT_->CreateBitmap(size,
      frame_buffer_,
      size.width * 4,
      props,
      &bitmap_);

   m_hwndDC = GetDC(m_hWnd);
   m_MemDC = CreateCompatibleDC(m_hwndDC);
   m_pFSInt = pFSInt;

   // DIB Section
   bi24BitInfo.bmiHeader.biBitCount = 32; // rgb 8 bytes for each component(3)
   bi24BitInfo.bmiHeader.biCompression = BI_RGB;// rgb = 3 components
   bi24BitInfo.bmiHeader.biPlanes = 1;
   bi24BitInfo.bmiHeader.biSize = sizeof(bi24BitInfo.bmiHeader); // size of this struct
   bi24BitInfo.bmiHeader.biWidth = REAL_DISP_X/*DISP_WIDTH*/; // width of window
   bi24BitInfo.bmiHeader.biHeight = REAL_DISP_Y/*DISP_HEIGHT*/; // height of window
   bi24BitInfo.bmiHeader.biSizeImage = 0;

   //bBytes = new BYTE[bi24BitInfo.bmiHeader.biWidth * bi24BitInfo.bmiHeader.biHeight * 4]; // create enough room. all pixels * each color component
   HDC hDC;
   hDC = CreateCompatibleDC(NULL);
   //m_iBitmap = CreateDIBSection(hDC, &bi24BitInfo, DIB_RGB_COLORS, (void**)&bBytes, 0, 0); // create a dib section for the dc

   //m_BmpMem = new Bitmap(m_iBitmap, NULL);

   //SelectObject(m_MemDC, m_iBitmap); // assign the dib section to the dc
   DeleteDC(hDC);
}


void DisplayPi::SetDefaultConfiguration()
{
}

void DisplayPi::SaveConfiguration(const char* config_name, const char* ini_file)
{
}

bool DisplayPi::LoadConfiguration(const char* config_name, const char* ini_file)
{
   return true;
}

HWND DisplayPi::CreateModelessConfigDlg(HWND hParent)
{
   // Create modeless dialog box for settings
   // TODO
   return NULL;
}

void DisplayPi::InitDlg()
{
}

void DisplayPi::Apply()
{
}

void DisplayPi::Cancel()
{
}


/*void DisplayPi::SetRasterPlot ( unsigned int x, unsigned int y, unsigned int col )
{
   if (x < REAL_DISP_X )
   {
      unsigned int count = (DISP_HEIGHT-2-((y * 2) % DISP_HEIGHT)) * REAL_DISP_X + x;
      bBytes[count] = col;
   }

}*/





void DisplayPi::SetBeam(int x, int y, unsigned int col)
{
   SetPixel(m_MemDC, x, y, col);
}

void DisplayPi::HSync()
{
}

void DisplayPi::StartSync()
{

}

void DisplayPi::VSync(bool bDbg)
{
  //BitBlt(m_hwndDC, m_XIn, m_YIn, m_Width, m_Height, m_MemDC, m_X, m_Y, SRCCOPY);
   // Wait VbL ?
#ifndef USE_QEMU_SUGARPI
   bool clear_framebuffer = false;
   if (full_resolution_cached_ != full_resolution_)
   {
      clear_framebuffer = true;
      full_resolution_cached_ = full_resolution_;

   }

   if (sync_on_frame_) // To turn on : Use the display core !
   {
      HRESULT hr;
      hr = bitmap_->CopyFromMemory(NULL,
         &frame_buffer_[0 * REAL_DISP_X * REAL_DISP_Y], REAL_DISP_X * 4);

      pRT_->BeginDraw();
      D2D1_RECT_F rect = D2D1::RectF(0, 0, 640, 480);
      D2D1_RECT_F rect_src = D2D1::RectF(143, 47, 783, 527);
      pRT_->DrawBitmap(bitmap_, &rect, 1,
         D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &rect_src);
      hr = pRT_->EndDraw();

      if (clear_framebuffer)
      {
         // TODO
      }

   }
   else
   {
      mutex_.lock();
      // get a new one (or wait for one to be free)
      bool found = false;

      for (int i = 0; i < FRAME_BUFFER_SIZE && !found; i++)
      {
         if (frame_used_[i] == FR_FREE)
         {
            frame_queue_[nb_frame_in_queue_++] = buffer_used_;
            frame_used_[buffer_used_] = FR_READY;

            frame_used_[i] = FR_USED;
            buffer_used_ = i;
            found = true;
            break;
         }
      }
      if (!found)
         logger_->Write("DIS", LogNotice, "All buffers are used");
      mutex_.unlock();
   }
#else
   frame_buffer_->SetVirtualOffset(143, 47 / 2 * 1024);
   frame_buffer_->WaitForVerticalSync();

#endif
   //added_line_ ^= 1;
   //buffer_num_ ^= 1;
   //added_line_ = 1;

   //   static unsigned int count = 0;
   //   static unsigned int max_tick = 0;
   //   static unsigned int nb_long_frame = 0;

      // Frame is ready

      // wait for a new frame to be available




      // If last frame is more than 20ms, just don't do it

      /*frame_buffer_->WaitForVerticalSync();
      unsigned int new_tick = timer_->GetClockTicks();

      if (new_tick - last_tick_frame_ > max_tick)
         max_tick = new_tick - last_tick_frame_;

      if (new_tick - last_tick_frame_ > 20500)
      {
         nb_long_frame++;
      }

      if (++count == 500)
      {
         logger_->Write("DIS", LogNotice, "500frame : max_frame : %i; Nb frames > 20ms : %i", max_tick, nb_long_frame);
         max_tick = 0;
         count = 0;
         nb_long_frame = 0;
      }
      last_tick_frame_ = new_tick;
      */

#define PROPTAG_BLANK_SCREEN	0x00040002
      /*CBcmPropertyTags Tags;
      TBlankScreen blankScreen;
      blankScreen.blank = 0;
      if (Tags.GetTag(PROPTAG_BLANK_SCREEN, &blankScreen, sizeof blankScreen, 4))
      {
      }
      else
      {
         logger_->Write("Display", LogNotice, "PROPTAG_BLANK_SCREEN - KO...");
      }

      */
      //logger_->Write("Display", LogNotice, "Vsync : added_line_=%i", added_line_);
}
  

void DisplayPi::DisplayDebug(int x, int y)
{
   // todo
}

void DisplayPi::DisplayDrop(int nbSections, tDropText* pLabels)
{
   SolidBrush linGrBrush(Color(200, 255, 255, 255));  // blue
   SolidBrush linGrBrushSel(Color(100, 255, 255, 255));  // blue
   Graphics gfx(m_MemDC);

   // Initialize arguments.
   Font myFont(L"Arial", 96.0f / (REAL)nbSections);
   PointF origin(0.0f, 0.0f);
   SolidBrush blackBrush(Color(200, 180, 180, 180));
   SolidBrush blackBrushSel(Color(200, 0, 0, 0));

   for (int i = 0; i < nbSections; i++)
   {
      RectF rectStr;
      rectStr.X = m_X + i * (static_cast<float>(m_Width) / static_cast<float>(nbSections));
      rectStr.Y = static_cast<float>(m_Y);
      rectStr.Width = static_cast<float>(m_Width) / static_cast<float>(nbSections);
      rectStr.Height = static_cast<float>(m_Height);

      gfx.FillRectangle((m_CurrentPart == i) ? &linGrBrushSel : &linGrBrush, rectStr.X, rectStr.Y, rectStr.Width, rectStr.Height);

      StringFormat  format;
      format.SetAlignment(StringAlignmentCenter);
      format.SetLineAlignment(StringAlignmentCenter);

      size_t size_of_string = strlen(pLabels[i]);
      wchar_t* stringw = new wchar_t[size_of_string + 1];
      MultiByteToWideChar(CP_ACP, 0, pLabels[i], size_of_string, stringw, size_of_string + 1);

      // Draw string.
      gfx.DrawString(
         stringw,
         size_of_string,
         &myFont,
         rectStr,
         &format,
         (m_CurrentPart == i) ? &blackBrushSel : &blackBrush);
      delete[]stringw;
   }
}

void DisplayPi::ResetLoadingMedia()
{

}

void DisplayPi::SetLoadingMedia()
{

}

void DisplayPi::ResetDragnDropDisplay()
{
   m_DnDType = 0;
}

void DisplayPi::SetDragnDropDisplay(int type)
{
   m_DnDType = type;
}

void DisplayPi::SetCurrentPart(int x, int y)
{
   switch (m_DnDType)
   {
      // Test : Is it SNA?
   case 1:
      m_CurrentPart = 0;
      break;

      // Is it ROM  ?
   case 2:
      break;

      // Is it DSK ?
   case 3:
      m_CurrentPart = (x < (m_WidthWindow) / 2) ? 0 : 1;
      break;

      // TAPE : TODO
   case 4:
      m_CurrentPart = 0;
      break;
   case 5:
      m_CurrentPart = 0;
      break;
   case 6:
      m_CurrentPart = 0;
      break;
   case 8:
      m_CurrentPart = 0;
      break;
   }
}

int DisplayPi::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
   UINT  num = 0;          // number of image encoders
   UINT  size = 0;         // size of the image encoder array in bytes

   ImageCodecInfo* pImageCodecInfo = NULL;

   GetImageEncodersSize(&num, &size);
   if (size == 0)
      return -1;  // Failure

   pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
   if (pImageCodecInfo == NULL)
      return -1;  // Failure

   GetImageEncoders(num, size, pImageCodecInfo);

   for (UINT j = 0; j < num; ++j)
   {
      if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
      {
         *pClsid = pImageCodecInfo[j].Clsid;
         free(pImageCodecInfo);
         return j;  // Success
      }
   }

   free(pImageCodecInfo);
   return -1;  // Failure
}

void DisplayPi::Screenshot()
{
   // Create new file
   TCHAR pathFile[MAX_PATH];

   if (GetNewScreenshotFile(pathFile, MAX_PATH))
   {
      Screenshot(pathFile);
   }
   else
   {
      // Error !
   }
}

void DisplayPi::Screenshot(const char* scr_path)
{
   // Create BMP of the right size
   HBITMAP hTmpBmp = CreateCompatibleBitmap(m_hwndDC, m_Width, m_Height);
   HDC hDC = CreateCompatibleDC(m_hwndDC);
   HBITMAP oldBmp = (HBITMAP)SelectObject(hDC, hTmpBmp);

   // Copy MemDC into it
   BitBlt(hDC, 0, 0, m_Width, m_Height, m_hwndDC, m_XIn, m_YIn, SRCCOPY);
   // Save this bitmap to the file
   Bitmap bmp(hTmpBmp, NULL);

   // Save the altered image.
   CLSID pngClsid;
   GetEncoderClsid(L"image/png", &pngClsid);

   size_t size_of_string = strlen(scr_path);
   wchar_t* stringw = new wchar_t[size_of_string + 1];
   MultiByteToWideChar(CP_ACP, 0, scr_path, size_of_string, stringw, size_of_string + 1);

   bmp.Save(stringw, &pngClsid, NULL);

   SelectObject(hDC, oldBmp);
   DeleteObject(oldBmp);
   DeleteDC(hDC);
}

bool DisplayPi::GetNewScreenshotFile(char* buffer_P, unsigned int size_P)
{
   if (m_pDiretories == NULL)
      return false;

   char exePath[MAX_PATH];
   strcpy_s(exePath, MAX_PATH, m_pDiretories->GetBaseDirectory());
   ::PathAppend(exePath, SCR_PATH);

   // Create new sound file
   bool bNameOk = false;
   unsigned int inc = 0;

   while (!bNameOk && inc <= 9999)
   {
      sprintf_s(buffer_P, size_P, "%sSCR%4.4i.PNG", exePath, inc);

      // Exists ?  inc and retry
      WIN32_FIND_DATA FindFileData;
      HANDLE handle = FindFirstFile(buffer_P, &FindFileData);

      if (handle != INVALID_HANDLE_VALUE)
      {
         inc++;
      }
      else
      {
         bNameOk = true;
      }
   }
   return bNameOk;
}

void DisplayPi::WaitVbl()
{
   // Wait for sound mixer
   int dbg = 1;
}

bool DisplayPi::AFrameIsReady()
{
   return false;
}

void DisplayPi::DisplayText(const char* txt, int x, int y, bool selected)
{
   // Display text
   int i = 0;

   char buff[16];
   memset(buff, 0, sizeof buff);
   strncpy(buff, txt, 15);

   unsigned int x_offset_output = 0;

   while (txt[i] != '\0')
   {

      // Display character
      unsigned char c = txt[i];

      if (c == ' ' || c >= 0x80)
      {
         x_offset_output += 10;
      }
      else
      {
         // Look for proper bitmap position (on first line only)
         for (int display_y = 0; display_y < font_->GetLetterHeight(c); display_y++)
         {
            int* line = GetVideoBuffer(display_y + y);
            font_->CopyLetter(c, display_y, &line[x + x_offset_output]);
         }
         x_offset_output += font_->GetLetterLength(c);
         
      }
      i++;

   }
}

void DisplayPi::Loop()
{
   logger_->Write("DIS", LogNotice, "Starting loop");
   // Waiting for a new frame to display
   //int old_frame_index = -1;
   while (1)
   {
      // Display available frame
      int frame_index = -1;
      mutex_.lock();
      if (nb_frame_in_queue_ > 0)
      {
         /*if ( old_frame_index != -1)
         {
            frame_used_[old_frame_index] = FR_FREE;
         }*/
         frame_index = frame_queue_[0];
         nb_frame_in_queue_--;
         memmove(frame_queue_, &frame_queue_[1], nb_frame_in_queue_ * sizeof(unsigned int));

         //if (frame_index != -1)
         {
            //logger_->Write("DIS", LogNotice, "Loop : display %i - nb_frame_in_queue_ : %i", frame_index, nb_frame_in_queue_);
            mutex_.unlock();

            //frame_buffer_->SetVirtualOffset(143, 47 / 2 + frame_index * 1024);

            HRESULT hr;
            hr = bitmap_->CopyFromMemory(NULL,
               &frame_buffer_[frame_index * REAL_DISP_X * REAL_DISP_Y], REAL_DISP_X * 4);

            pRT_->BeginDraw();
            pRT_->DrawBitmap(bitmap_, NULL, 1,
                D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, NULL);
            hr = pRT_->EndDraw();


            // Set it as available
            frame_used_[frame_index] = FR_FREE;
            //old_frame_index = frame_index;
         }
         /*else
         {
            mutex_.unlock();
            logger_->Write("DIS", LogNotice, "No buffer to display");
            CTimer::Get()->MsDelay(1);
         }*/
      }
      else
      {
         mutex_.unlock();
         WAIT(1);
      }
      // sleep ?

   }
}