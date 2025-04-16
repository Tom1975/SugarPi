
#include "CarrouselWindow.h"

#include "files.h"

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace fs = std::filesystem;

bool CarrouselWindow::is_loaded_ = false;
std::vector<CarrouselWindow::GameDescription> CarrouselWindow::game_list_;

CarrouselWindow::CarrouselWindow(DisplayPi* display) :
   Window(display), stripe_(display)
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
   logo_->Create(this, (w - lw) / 2,!
                               0, bitmap_);

   current_game_ = nullptr;
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
   // Description window
   // Game selection stripe
   stripe_.Create(this, 0, width-200, width, 200);
}

void CarrouselWindow::Clear()
{
   Window::Clear();
}

void CarrouselWindow::RedrawWindow()
{
   // wait for game list to be loaded
   while (!is_loaded_)
   {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
   }

   Window::RedrawWindow();

   // Check that we have a game to display
   if (current_game_ == nullptr)
   {
      ChangeGame();
   }

   // Draw screenshot
   DrawBitmap(&screenshot_bitmap_, 10, 255);

   // Write description
   SelectColor(0xFF0000);
   WriteText(current_game_->description_.c_str(), 255, 255);

}

void CarrouselWindow::ChangeGame(unsigned int index )
{
   // Set current data
   current_game_ = &game_list_[index];

   // Force redraw
   std::filesystem::path scr_path = PATH_GAMES;
   scr_path /= current_game_->screen_path_;

   screenshot_bitmap_.Load(scr_path.string().c_str());

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
         std::ifstream f(p.path());
         json j = json::parse(f, nullptr, false);
         if (j.is_discarded())
         {
            // Parse error, log and do nothing
            CLogger::Get()->Write("CARROUSEL", LogNotice, "Parse error on file %s", p.path().u8string().c_str());
            continue;
         }
         CLogger::Get()->Write("CARROUSEL", LogNotice, "Parse file %s ok", p.path().u8string().c_str());

         // Load each values available.
         GameDescription description;
         description.name_ = j["Game_name"];
         description.description_ = j["Description"];
         description.screen_path_ = j["Screenshot"];

         // If everything is ok, add it to game carrousel.
         // otherwise, free resources
         game_list_.push_back(description);

      }
   }
   is_loaded_ = true;
}
