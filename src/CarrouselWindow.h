#pragma once

#include <string>
#include <vector>
#include <filesystem>

#include "Window.h"
#include "BitmapWindows.h"
#include "MenuWindows.h"
#include "SugarboxLogo.h"

class CarrouselWindow : public Window
{
public:
   CarrouselWindow(BasicFrame* display);
   virtual ~CarrouselWindow();

   virtual void Create(Window* parent, int x, int y, unsigned int width, unsigned int height);

   virtual void Clear();

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

   BitmapWindows* logo_;
   SugarboxLogo* bitmap_;

};
