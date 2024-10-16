/* GIMP RGBA C-Source image dump (coolspot.c) */

#include "SimpleBitmap.h"

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

SimpleBitmap::SimpleBitmap(const char* file ) : width_(0), height_(0), pixel_data_(nullptr), first_byte_per_line(nullptr), last_byte_per_line(nullptr), loaded_(false)
{
   if (file != nullptr)
   {
      Load(file);
   }
}

SimpleBitmap::~SimpleBitmap()
{
}

void SimpleBitmap::Load(const char* file)
{
   FILE* f;

   if (fopen_s(&f, file, "r+b") == 0)
   {
      // Read width / height
      if (fread(&width_, sizeof(unsigned int), 1, f) != 1)
      {
         // error
         width_ = 0;
         fclose(f);
         return;
      }

      if (fread(&height_, sizeof(unsigned int), 1, f) != 1)
      {
         // error
         width_ = 0;
         height_ = 0;
         fclose(f);
         return;
      }

      Lock();
      pixel_data_ = new int[width_ * height_ ];
      first_byte_per_line = new int[height_];
      last_byte_per_line = new int[height_];
      memset(pixel_data_, 0, width_ * height_ * sizeof( int));
      memset(first_byte_per_line, 0, height_ * sizeof(int));
      memset(last_byte_per_line, 0, height_ * sizeof(int));
      Unlock();

      int size_read = fread(pixel_data_, width_ * height_ * sizeof(int), 1, f);

      fclose(f);
   }

   for (int i = 0; i < height_; i++)
   {
      bool found = false;
      int j = 0;
      for (j = 0; j < width_ && !found; j++)
      {
         found = (pixel_data_[i * width_ + j] != 0);
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
         for (j = width_ - 1; j >= 0 && !found; j--)
         {
            found = (pixel_data_[i * width_ + j] != 0);
         }
         last_byte_per_line[i] = (found) ? j : -1;
      }
   }
   loaded_ = true;
}

void SimpleBitmap::DrawLogo(int line, int* buffer)
{
   Lock();
   if (pixel_data_ == nullptr)
   {
      Unlock();
      return;
   }
   Unlock();

   if (first_byte_per_line[line] == -1) return;

   int* begin = &pixel_data_[first_byte_per_line[line] + line * width_];
   int* end = &pixel_data_[last_byte_per_line[line] + line * width_];
   buffer += first_byte_per_line[line];
   for (; begin<end; ++begin)
   {
      if (*begin != 0)
         *buffer =*begin|0xFF000000;
      buffer++;
   }
}

void SimpleBitmap::GetSize(int& width, int& height)
{
   width = width_;
   height = height_;
}