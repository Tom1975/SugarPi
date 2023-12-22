#include "DisplayPiDesktop.h"
#include "Shlwapi.h"
#include <vector>
#include <DXGI.h>


#define DISP_WIDTH    1024
#define DISP_HEIGHT   576

#define DISP_WINDOW_X   800
#define DISP_WINDOW_Y   600//576

#define SCR_PATH "\\SCR\\"

DisplayPi::DisplayPi(CLogger* logger) : m_pFSInt(NULL), sync_on_frame_(true)
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
}

DisplayPi::~DisplayPi()
{
   delete font_;
}

void DisplayPi::ReleaseAll()
{
   // release
   ReleaseDC(m_hWnd, m_hwndDC);

   DeleteObject(m_iBitmap);
   delete m_BmpMem;
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

void DisplayPi::Init(HINSTANCE hInstance, HWND hWnd, IFullScreenInterface* pFSInt)
{
   m_hWnd = hWnd;
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
   m_iBitmap = CreateDIBSection(hDC, &bi24BitInfo, DIB_RGB_COLORS, (void**)&bBytes, 0, 0); // create a dib section for the dc

   m_BmpMem = new Bitmap(m_iBitmap, NULL);

   SelectObject(m_MemDC, m_iBitmap); // assign the dib section to the dc
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
   BitBlt(m_hwndDC, m_XIn, m_YIn, m_Width, m_Height, m_MemDC, m_X, m_Y, SRCCOPY);
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
