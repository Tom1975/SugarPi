/* GIMP RGBA C-Source image dump (coolspot.c) */

#include "SimpleBitmap.h"

#include "files.h"
#include <CPCCore/CPCCoreEmu/stdafx.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <stdexcept>
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

SimpleBitmap::SimpleBitmap(const char* file ) : width_(0), height_(0), pixel_data_(), first_byte_per_line(nullptr), last_byte_per_line(nullptr), loaded_(false)
{
   if ( file != nullptr)
   {
      file_ = file;
   }
}

SimpleBitmap::~SimpleBitmap()
{
   delete []first_byte_per_line;
   delete []last_byte_per_line;
}

void SimpleBitmap::Init()
{
    std::string str = file_.string();
   Load (str.c_str());
}

void SimpleBitmap::Load(const char* filename)
{
   CLogger::CLogger::Get ()->Write("SimpleBitmap", LogNotice, "Load : %s", filename);

   std::ifstream file(filename, std::ios::binary);
   if (!file) 
   {
      return;
   }

   // Read magic number
   BMPFileHeader fileHeader;
   file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));

   // Vérifier la signature "BM"
   bool not_loaded = true;
   if (fileHeader.fileType == 0x4D42)
   { // 'BM' en little-endian
      not_loaded = LoadBMP(fileHeader , file);
   }

   // Read BMP info header
   // Read width / height
   if (!not_loaded)
   {
      file.read(reinterpret_cast<char*>(&width_), sizeof(width_));
      file.read(reinterpret_cast<char*>(&height_), sizeof(height_));

      Lock();
      pixel_data_.resize(width_ * height_);
      Unlock();
      file.read(reinterpret_cast<char*>(&pixel_data_[0]), width_ * height_ * sizeof(int));

   }
   Lock();
   first_byte_per_line = new int[height_];
   last_byte_per_line = new int[height_];
   memset(first_byte_per_line, 0, height_ * sizeof(int));
   memset(last_byte_per_line, 0, height_ * sizeof(int));
   Unlock();

   for (unsigned int i = 0; i < height_; i++)
   {
      bool found = false;
      unsigned int j = 0;
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

// Fonction pour charger un fichier BMP dans un buffer
bool SimpleBitmap::LoadBMP(BMPFileHeader& fileHeader, std::ifstream& file)
{
   // Lire l'en-tête d'information BMP
   BMPInfoHeader infoHeader;
   file.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));

   // Vérifier que l'image est en 24 bits sans compression
   if (infoHeader.bitCount != 24 || infoHeader.compression != 0) {
      return false;
   }

   // Extraire la largeur et la hauteur
   width_ = infoHeader.width;
   height_ = infoHeader.height;

   // Se positionner à l'offset des données d'image
   file.seekg(fileHeader.offsetData, std::ios::beg);

   // Calculer la taille d'une ligne (chaque ligne doit être alignée sur 4 octets)
   int rowSize = (width_ * 3 + 3) & ~3; // Alignement sur 4 octets

   // Lire les données d'image (BGR format)
   std::vector<uint8_t> buffer(rowSize * height_);
   file.read(reinterpret_cast<char*>(buffer.data()), buffer.size());

   if (!file) {
      return false;
   }

   // Inverser l'image car BMP stocke les lignes de bas en haut
   pixel_data_.resize(width_ * height_ ); // Taille exacte des données RGB
   for (int y = 0; y < height_; ++y) {
      const uint8_t* srcRow = &buffer[(height_ - 1 - y) * rowSize];
      int* dstRow = &pixel_data_[y * width_ ];
      for (int x = 0; x < width_; ++x) 
      {
         dstRow[x] = srcRow[x] | 0xFF000000;
      }
   }

   return true;
}


void SimpleBitmap::GetSize(int& width, int& height)
{
   width = width_;
   height = height_;
}


void SimpleBitmap::DrawLogo(int line, int* buffer)
{
   Lock();
   if (pixel_data_.size() == 0)
   {
      Unlock();
      return;
   }
   Unlock();

   if (first_byte_per_line[line] == -1) return;

   int* begin = &pixel_data_[first_byte_per_line[line] + line * width_];
   int* end = &pixel_data_[last_byte_per_line[line] + line * width_];
   buffer += first_byte_per_line[line];
   for (; begin < end; ++begin)
   {
      if (*begin != 0)
         *buffer = *begin | 0xFF000000;
      buffer++;
   }
}
