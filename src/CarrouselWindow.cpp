
#include "CarrouselWindow.h"

#include "files.h"

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace fs = std::filesystem;



CarrouselWindow::CarrouselWindow(BasicFrame* display) :
   Window(display)
{
   // Compute window size : Depending on display
   unsigned int w = display->GetWidth();
   unsigned int h = display->GetHeight();
   double ratio = (double)w / (double)h;

   Create(0, 10, 0, w-20, h);

   // Create Title bitmap 
   // Keep first quarter for it
   bitmap_ = new SugarboxLogo();
   logo_ = new BitmapWindows(display);

   //logo_->Create(this, 240, 70, bitmap_);
   int lw, lh;
   bitmap_->GetSize(lw, lh);
   logo_->Create(this, (w - lw) / 2,
                               (h/4 - lh) / 2, bitmap_);
}

CarrouselWindow::~CarrouselWindow()
{
   delete logo_;
   delete bitmap_;
}

void CarrouselWindow::Create(Window* parent, int x, int y, unsigned int width, unsigned int height)
{
   Window::Create(parent, x, y, width, height);

   // Add internal windows : 
   // Screenshot window
   // Description window
   // Game selection stripe
}

void CarrouselWindow::LoadCarrousel()
{
   // Fill inner game structure with "Carrousel" folder :
   std::string path = PATH_GAMES;

   // For each game in folder : 
   for (const auto& p : fs::directory_iterator(path))
   {
      if (p.path().extension() == ".car")
      {
         // Read the JSON file
         std::ifstream f("example.json");
         json data = json::parse(f);

         // Load each values available.
         // If everything is ok, add it to game carrousel.
         // otherwise, free resources

      }
   }

}

void CarrouselWindow::Clear()
{
   Window::Clear();
}

