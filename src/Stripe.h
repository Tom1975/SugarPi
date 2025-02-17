#pragma once

#include "Window.h"
#include "SimpleBitmap.h"


class GameDescriptor
{
public:

};

class Stripe : public Window
{
public:
   Stripe(DisplayPi* display);
   virtual ~Stripe();

   virtual void Create(Window* parent, int x, int y, unsigned int width, unsigned int height);

   virtual void InitStripe(std::vector < GameDescriptor*>& game_list);
   virtual void RedrawWindow();


private:
   std::vector < GameDescriptor*> game_list_;
};
