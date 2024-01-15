//
#include "DisplayPiImp.h"

#include <memory.h>

#include <circle/types.h>
#include <circle/bcmpropertytags.h>
#include <circle/debug.h>

#include "res/button_1.h"
#include "res/coolspot.h"

#define WIDTH_SCREEN 640
#define HEIGHT_SCREEN 480

#define WIDTH_VIRTUAL_SCREEN 1024
#define HEIGHT_VIRTUAL_SCREEN (288*2)


DisplayPiImp::DisplayPiImp(CLogger* logger, CTimer* timer) :DisplayPi(logger),
   timer_(timer),
   frame_buffer_(nullptr),
   mutex_(TASK_LEVEL)   
{
}

DisplayPiImp::~DisplayPiImp()
{
   if ( frame_buffer_ != nullptr)
   {
      delete frame_buffer_;
   }   
}

bool DisplayPiImp::Initialization()
{
   ListEDID();

   unsigned int screen_width = 640;
   unsigned int screen_height = 480;

   logger_->Write("Display", LogNotice, "Initialization...");
   // Get display property, to compute best values
   CBcmPropertyTags Tags;
	TPropertyTagDisplayDimensions Dimensions;
	if (Tags.GetTag (PROPTAG_GET_DISPLAY_DIMENSIONS, &Dimensions, sizeof Dimensions))
	{
      screen_width  = Dimensions.nWidth;
      screen_height = Dimensions.nHeight;
      logger_->Write("Display", LogNotice, "PROPTAG_GET_DISPLAY_DIMENSIONS : %i, %i.", screen_width, screen_height);
   }
   else
   {
      logger_->Write("Display", LogNotice, "PROPTAG_GET_DISPLAY_DIMENSIONS : ERROR !");
   }

   // Now we have real width/height : compute best values for display, to have :
   // - good pixel ratio (4x3)
   float ratio = ((float)GetWidth())/((float)GetHeight());
   int NbPixelWidth, NbPixelHeight;
   if ( (screen_width / screen_height ) < ratio )
   {
      //
      NbPixelWidth = static_cast<long>(screen_height * ratio);
      NbPixelHeight = screen_height ;
   }
   else
   {
      //
      NbPixelWidth = screen_width - 0;
      NbPixelHeight = static_cast<long>(screen_width / ratio);
   }   

   // Compute height to have a complete screen, without problem with scanlines
   int h = GetHeight();
   if ( NbPixelHeight % h != 0)
   {
      logger_->Write("Display", LogNotice, "NbPixelHeight % h : %i, %i.", NbPixelHeight, h);

      int NbPixelHeightComputed = (NbPixelHeight / h) * h;
      if ( NbPixelHeight % h > (h/2) )
      {
         NbPixelHeightComputed += h;
      }
      NbPixelHeight = NbPixelHeightComputed;
      NbPixelWidth = static_cast<long>(NbPixelHeight*ratio);
   }

   int fXmin, fXmax, fYmin, fYmax;
   fXmin = (screen_width - NbPixelWidth) / 2;
   fXmax = (screen_width - NbPixelWidth) / 2 + NbPixelWidth;
   fYmin = ((long)screen_height - NbPixelHeight) / 2;
   fYmax = ((long)screen_height - NbPixelHeight) / 2 + NbPixelHeight;

   logger_->Write("Display", LogNotice, "Computed width/height : %i, %i.", NbPixelWidth, NbPixelHeight);
   logger_->Write("Display", LogNotice, "fXmin = %i, fXmax = %i, fYmin = %i, fYmax  = %i.", fXmin, fXmax, fYmin, fYmax);
   // - 


   if ( frame_buffer_ != nullptr)
   {
      delete frame_buffer_;
   }
   frame_buffer_ = new CBcmFrameBuffer(WIDTH_SCREEN, HEIGHT_SCREEN, 32, WIDTH_VIRTUAL_SCREEN, HEIGHT_VIRTUAL_SCREEN * FRAME_BUFFER_SIZE);

   if (!frame_buffer_ || !frame_buffer_->Initialize())
   {
      logger_->Write("Display", LogNotice, "Error creating framebuffer...");
      return FALSE;
   }
      
   frame_buffer_->SetVirtualOffset(143, 47);

   DisplayPi::Initialization();

   return true;
}

bool DisplayPiImp::ListEDID()
{
   //  Display all resolution supported
	CBcmPropertyTags Tags;
	TPropertyTagEDIDBlock TagEDID;
	TagEDID.nBlockNumber = 0;
   bool tag_send = Tags.GetTag (PROPTAG_GET_EDID_BLOCK , &TagEDID, sizeof TagEDID, 4);
	if ( tag_send && TagEDID.nStatus == 0)
	{
      logger_->Write("Display", LogNotice, "EDID message : ");
		// Decodage :
      // check id
      unsigned char header[]={0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00 };
      if ( memcmp( &TagEDID.Block[0], header, sizeof(header)) == 0)
      {
         // Manufacturer ID, product, serial number
         // week/year
         // EDID version
         logger_->Write("Display", LogNotice, "EDID version : %i.%i", TagEDID.Block[18], TagEDID.Block[19]);
         debug_hexdump(TagEDID.Block, 128, "EDID");
      }
      else
      {
         logger_->Write("Display", LogNotice, "EDID Wrong header");
         debug_hexdump (TagEDID.Block, 128, "EDID");
      }
	}   
   return true;
}


void DisplayPiImp::SetScanlines(int scan)
{
   
}

const char* DisplayPiImp::GetInformations()
{
   logger_->Write("Display", LogNotice, "Get Information ");
   return "Display for Raspberry PI - Bare metal";
}

// Wait VBL
void DisplayPiImp::WaitVbl()
{
   frame_buffer_->WaitForVerticalSync();
}

int DisplayPiImp::GetStride()
{
   // TODO : stride is for int* !!!
   return frame_buffer_->GetPitch() / sizeof(int);
}

int DisplayPiImp::GetWidth()
{
   return WIDTH_VIRTUAL_SCREEN;
}

int DisplayPiImp::GetHeight()
{
   return HEIGHT_VIRTUAL_SCREEN;
}

int* DisplayPiImp::GetVideoBuffer(int y)
{
   if (!full_resolution_)
   {
      y = y * 2 + added_line_;
   }

   if ( y > HEIGHT_VIRTUAL_SCREEN) y = HEIGHT_VIRTUAL_SCREEN-1;
   y += buffer_used_ * HEIGHT_VIRTUAL_SCREEN;

   return reinterpret_cast<int*>(frame_buffer_->GetBuffer() + y * frame_buffer_->GetPitch());
}

void DisplayPiImp::SyncWithFrame (bool set)
{
   if (set)
   {
      sync_on_frame_ = set;
   }
   else
   {
      sync_on_frame_ = true;//set;
   }
}

void DisplayPiImp::SetFrame(int frame_index)
{
   //logger_->Write("Display", LogNotice, "SetFrame : 143, %i", 47 + frame_index * HEIGHT_VIRTUAL_SCREEN);
   frame_buffer_->SetVirtualOffset(143, 47 + frame_index * HEIGHT_VIRTUAL_SCREEN);
}

void DisplayPiImp::Draw()
{
   //logger_->Write("Display", LogNotice, "Draw");
   frame_buffer_->WaitForVerticalSync();
}

void DisplayPiImp::ClearBuffer(int frame_index)
{
   //logger_->Write("Display", LogNotice, "ClearBuffer : frame_index = %i", frame_index);
   unsigned char* line = reinterpret_cast<unsigned char*>(frame_buffer_->GetBuffer() + frame_index * HEIGHT_VIRTUAL_SCREEN * frame_buffer_->GetPitch());
   for (unsigned int count = 0; count < HEIGHT_VIRTUAL_SCREEN; count++)
   {
      memset(line, 0x0, WIDTH_VIRTUAL_SCREEN * 4);
      line += frame_buffer_->GetPitch();
   }
   //logger_->Write("Display", LogNotice, "End clear");
}