#pragma once
#include <filesystem>

#include "PiBitmap.h"

class SimpleBitmap : public PiBitmap
{
public:
   SimpleBitmap(const char* file = nullptr);
   virtual ~SimpleBitmap();

   void Init();

   void DrawLogo(int line, int* buffer);
   virtual void GetSize(int& width, int& height);

   void Load(const char* filename);

private:
   // Structure pour représenter un en-tête BMP
#pragma pack(push, 1) // Désactive l'alignement pour correspondre à la structure BMP
   struct BMPFileHeader {
      uint16_t fileType;      // Signature "BM"
      uint32_t fileSize;      // Taille totale du fichier
      uint16_t reserved1;     // Réservé, doit être 0
      uint16_t reserved2;     // Réservé, doit être 0
      uint32_t offsetData;    // Offset vers les données d'image
   };

   struct BMPInfoHeader {
      uint32_t size;          // Taille de cet en-tête (40 bytes)
      int32_t width;          // Largeur de l'image
      int32_t height;         // Hauteur de l'image
      uint16_t planes;        // Nombre de plans (toujours 1)
      uint16_t bitCount;      // Bits par pixel (24 pour RGB)
      uint32_t compression;   // Type de compression (0 = aucun)
      uint32_t imageSize;     // Taille des données d'image
      int32_t xPixelsPerMeter; // Résolution horizontale
      int32_t yPixelsPerMeter; // Résolution verticale
      uint32_t colorsUsed;    // Couleurs dans la palette (0 = toutes)
      uint32_t colorsImportant; // Couleurs importantes (0 = toutes)
   };
#pragma pack(pop)

   bool LoadBMP(BMPFileHeader& fileHeader, std::ifstream& file);

   std::filesystem::path file_;

   unsigned int width_;
   unsigned int height_;
   std::vector<int> pixel_data_;
   int *first_byte_per_line;
   int *last_byte_per_line;

   bool loaded_;
};
