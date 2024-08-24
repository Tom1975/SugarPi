#pragma once

#include "PiBitmap.h"

class SugarboxLogo : public PiBitmap
{
public:
   SugarboxLogo();
   virtual ~SugarboxLogo();

   void DrawLogo(int line, int* buffer);
   virtual void GetSize(int& width, int& height);

   static void Load();

private:

   static int *pixel_data_;
   static int *first_byte_per_line;
   static int *last_byte_per_line;
   static bool loaded_;
};