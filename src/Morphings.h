#pragma once

#include "Morphings.h"
#include "BasicFrame.h"

class Morphings : public Morphing
{
public:
   Morphings(BasicFrame* frame, int x, int y, int w, int h, int nbframe);
   virtual ~Morphings();


   bool Refresh();

protected:
   BasicFrame* frame_;
   int base_x;
   int base_y;
   int base_width;
   int base_height;

   int final_x;
   int final_y;
   int final_width;
   int final_height;

   float count_;
   float max_count_;
};
