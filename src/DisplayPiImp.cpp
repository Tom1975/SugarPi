//
#include "DisplayPiImp.h"

#include <memory.h>

#include <circle/types.h>
#include <circle/bcmpropertytags.h>
#include <circle/debug.h>

#include "res/button_1.h"
#include "res/coolspot.h"

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
   /*
   unsigned int screen_width = 640;
   unsigned int screen_height = 480;

   // Get display property, to compute best values
   CBcmPropertyTags Tags;
	TPropertyTagDisplayDimensions Dimensions;
	if (Tags.GetTag (PROPTAG_GET_DISPLAY_DIMENSIONS, &Dimensions, sizeof Dimensions))
	{
      screen_width  = Dimensions.nWidth;
      screen_height = Dimensions.nHeight;
   }

   // Now we have real width/height : compute best values for display, to have :
   // - good pixel ratio (4x3)
   
   float ratio = ((float)GetWidth())/((float)GetHeight());
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
      if ( NbPixelHeight % m_Height != 0)
      {
         int NbPixelHeightComputed = (NbPixelHeight / m_Height) * m_Height;
         if ( NbPixelHeight % m_Height > (m_Height/2) )
         {
            NbPixelHeightComputed += m_Height;
         }
         NbPixelHeight = NbPixelHeightComputed;
         NbPixelWidth = static_cast<long>(NbPixelHeight*ratio);
      }

      int fXmin, fXmax, fYmin, fYmax;
      fXmin = (findMode.w - NbPixelWidth) / 2;
      fXmax = (findMode.w - NbPixelWidth) / 2 + NbPixelWidth;
      fYmin = ((long)findMode.h - NbPixelHeight) / 2;
      fYmax = ((long)findMode.h - NbPixelHeight) / 2 + NbPixelHeight;


      m_DestRectFullScreen.x = fXmin;
      m_DestRectFullScreen.y = fYmin;
      m_DestRectFullScreen.w = NbPixelWidth;
      m_DestRectFullScreen.h = NbPixelHeight;*/   
   // - 


   if ( frame_buffer_ != nullptr)
   {
      delete frame_buffer_;
   }
   frame_buffer_ = new CBcmFrameBuffer(768, 277*2, 32, 1024, 1024* FRAME_BUFFER_SIZE);

   frame_buffer_->Initialize();
   frame_buffer_->SetVirtualOffset(143, 47/2);

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
		Tags.GetTag (PROPTAG_GET_EDID_BLOCK , &TagEDID, sizeof TagEDID, 4);

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
   return frame_buffer_->GetPitch();
}


int* DisplayPiImp::GetVideoBuffer(int y)
{
   if (!full_resolution_cached_)
   {
      y = y * 2 + added_line_;
   }
   

   y &= 0x3FF;
   y += buffer_used_ * 1024;

///// 
   //logger_->Write("Display", LogNotice, "GetVideoBuffer : y = %i; buffer_used_ : %i, ==>%i", y, buffer_used_, frame_buffer_->GetBuffer() + y * frame_buffer_->GetPitch());
   return reinterpret_cast<int*>(frame_buffer_->GetBuffer() + y * frame_buffer_->GetPitch());

//   return (int*)(frame_buffer_->GetBuffer() + (y * 2 /*+ added_line_*/)* frame_buffer_->GetPitch() );
   
      
}

void DisplayPiImp::SetFrame(int frame_index)
{
   frame_buffer_->SetVirtualOffset(143, 47 / 2 + frame_index * 1024);
}

void DisplayPiImp::Draw()
{
   frame_buffer_->WaitForVerticalSync();
}

void DisplayPiImp::ClearBuffer(int frame_index)
{

   unsigned char* line = reinterpret_cast<unsigned char*>(frame_buffer_->GetBuffer() + frame_index * 1024 * frame_buffer_->GetPitch());
   for (unsigned int count = 0; count < 1024; count++)
   {
      memset(line, 0x0, 1024 * 4);
      line += frame_buffer_->GetPitch();
   }

}