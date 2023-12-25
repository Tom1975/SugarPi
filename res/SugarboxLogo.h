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
   static int pixel_data_[];

};