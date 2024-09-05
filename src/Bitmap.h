#pragma once

#include "PiBitmap.h"

class Bitmap : public PiBitmap
{
public:
   Bitmap();
   virtual ~Bitmap();

   void DrawLogo(int line, int* buffer);
   virtual void GetSize(int& width, int& height);

   void Load();

private:
   unsigned int width_;
   unsigned int height_;
   int *pixel_data_;
   int *first_byte_per_line;
   int *last_byte_per_line;

   bool loaded_;
};
