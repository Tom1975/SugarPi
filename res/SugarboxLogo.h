#pragma once

#include "PiBitmap.h"

class SugarboxLogo : public PiBitmap
{
public:
   SugarboxLogo();
   virtual ~SugarboxLogo();

   void DrawLogo(int line, int* buffer);
   virtual void GetSize(int& width, int& height);

private:
   int *pixel_data_;
   int *first_byte_per_line;
   int *last_byte_per_line;

};