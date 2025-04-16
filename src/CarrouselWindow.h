#pragma once

#include <string>
#include <vector>
#include <filesystem>

#include "Window.h"
#include "BitmapWindows.h"
#include "SimpleBitmap.h"
#include "MenuWindows.h"
#include "SugarboxLogo.h"
#include "Stripe.h"

class CarrouselWindow : public Window
{
public:
   CarrouselWindow(DisplayPi* display);
   virtual ~CarrouselWindow();

   virtual void Create(Window* parent, int x, int y, unsigned int width, unsigned int height);

   virtual void RedrawWindow();
   virtual void Clear();

   virtual void ChangeGame(unsigned int index = 0);

   static void LoadCarrousel();


protected:
   class GameDescription
   {
   public:
      /////////////////////////
      // Mandatory
      // 
      // Game name
      std::string name_;

      // screenshot
      std::string screen_path_;

      // Media path
      std::vector<std::filesystem::path> media_list_;

      /////////////////////////
      // Optionnal
      // Game description
      std::string description_;

      // Startup command
      std::string startup_command_;

   };

   // Static data
   static std::vector<GameDescription> game_list_;
   static bool is_loaded_;

   // Window and display ressources
   BitmapWindows* logo_;
   SugarboxLogo* bitmap_;

   SimpleBitmap screenshot_bitmap_;
   Stripe stripe_;

   // Current game 
   GameDescription* current_game_;


};
