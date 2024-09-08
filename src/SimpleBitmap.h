#pragma once

#include "PiBitmap.h"

class SimpleBitmap : public PiBitmap
{
public:
   SimpleBitmap(const char* file = nullptr);
   virtual ~SimpleBitmap();

   void DrawLogo(int line, int* buffer);
   virtual void GetSize(int& width, int& height);

   void Load(const char* file);

private:
   unsigned int width_;
   unsigned int height_;
   int *pixel_data_;
   int *first_byte_per_line;
   int *last_byte_per_line;

   bool loaded_;
};
