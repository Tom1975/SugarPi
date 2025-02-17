#include "Stripe.h"


Stripe::Stripe(DisplayPi* display) : Window(display)
{

}

Stripe::~Stripe()
{
}

void Stripe::Create(Window* parent, int x, int y, unsigned int width, unsigned int height)
{
   Window::Create(parent, x, y, width, height);

}

void Stripe::InitStripe(std::vector < GameDescriptor*>& game_list)
{
   game_list_ = game_list;
}


void Stripe::RedrawWindow()
{
   // Draw background
   // todo
   // Draw inner bitmap
   // todo

}