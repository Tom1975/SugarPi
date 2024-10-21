/* GIMP RGBA C-Source image dump (coolspot.c) */

#include "SugarboxLogo.h"

#include "files.h"
#include <CPCCore/CPCCoreEmu/stdafx.h>
#include <stdio.h>
#include <memory.h>


#ifdef  __circle__
//#include <circle/logger.h>
#include <circle/spinlock.h>
static CSpinLock   mutex_;
static void Lock() { mutex_.Acquire(); }
static void Unlock() { mutex_.Release(); }
#else
#include "CLogger.h"
#include <mutex>
static std::mutex mutex_;
static void Lock() { mutex_.lock(); }
static void Unlock() { mutex_.unlock(); }
#endif

//#include "res/logo.c"

/* GIMP RGBA C-Source image dump (SplashScreen.c) */

struct {
   unsigned int 	 width;
   unsigned int 	 height;
   unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */
} Sugarbox_logo = {
  753, 254, 4,
};

int* SugarboxLogo::pixel_data_ = nullptr;
int* SugarboxLogo::first_byte_per_line = nullptr;
int* SugarboxLogo::last_byte_per_line = nullptr;
bool SugarboxLogo::loaded_ = false;


SugarboxLogo::SugarboxLogo() 
{

   // Save to data

   /*if (fopen_s(&f, PATH_RES INTER_FILE "logo.bin", "w+b") == 0)
   {
      fwrite(gimp_image.pixel_data, sizeof(gimp_image.pixel_data), 1, f);
      fclose(f);
   }*/
}

SugarboxLogo::~SugarboxLogo()
{
}

void SugarboxLogo::Load()
{
   if (pixel_data_ == nullptr)
   {
      Lock();
      pixel_data_ = new int[Sugarbox_logo.width * Sugarbox_logo.height * Sugarbox_logo.bytes_per_pixel / sizeof(int)];
      first_byte_per_line = new int[Sugarbox_logo.height];
      last_byte_per_line = new int[Sugarbox_logo.height];
      memset(pixel_data_, 0, Sugarbox_logo.width * Sugarbox_logo.height * Sugarbox_logo.bytes_per_pixel);
      memset(first_byte_per_line, 0, Sugarbox_logo.height * sizeof(int));
      memset(last_byte_per_line, 0, Sugarbox_logo.height * sizeof(int));
      Unlock();
   }

   FILE* f;

   if (fopen_s(&f, PATH_RES INTER_FILE "logo.bin", "r+b") == 0)
   {
      int size_read = fread(pixel_data_, Sugarbox_logo.width * Sugarbox_logo.height * Sugarbox_logo.bytes_per_pixel, 1, f);
      //CLogger::Get()->Write("MenuItemWindows", LogNotice, "Bitmap read : %X", size_read);
      fclose(f);
   }

   // Compute first byte to display per line.
   //CLogger::Get()->Write("MenuItemWindows", LogNotice, " Compute first byte to display per line.");

   for (int i = 0; i < Sugarbox_logo.height; i++)
   {
      bool found = false;
      int j = 0;
      for (j = 0; j < Sugarbox_logo.width && !found; j++)
      {
         found = (pixel_data_[i * Sugarbox_logo.width + j] != 0);
      }
      first_byte_per_line[i] = (found) ? j : -1;

      if (first_byte_per_line[i] == -1)
      {
         last_byte_per_line[i] = -1;
      }
      else
      {
         found = false;
         j = 0;
         for (j = Sugarbox_logo.width - 1; j >= 0 && !found; j--)
         {
            found = (pixel_data_[i * Sugarbox_logo.width + j] != 0);
         }
         last_byte_per_line[i] = (found) ? j : -1;
      }
   }
   loaded_ = true;
}

void SugarboxLogo::DrawLogo(int line, int* buffer)
{
   Lock();
   if (pixel_data_ == nullptr)
   {
      Unlock();
      return;
   }
   Unlock();

   if (first_byte_per_line[line] == -1) return;

   int* begin = &pixel_data_[first_byte_per_line[line] + line * Sugarbox_logo.width];
   int* end = &pixel_data_[last_byte_per_line[line] + line * Sugarbox_logo.width];
   buffer += first_byte_per_line[line];
   for (; begin<end; ++begin)
   {
      if (*begin != 0)
         *buffer =*begin|0xFF000000;
      buffer++;
   }
}

void SugarboxLogo::GetSize(int& width, int& height)
{
   width = Sugarbox_logo.width;
   height = Sugarbox_logo.height;
}