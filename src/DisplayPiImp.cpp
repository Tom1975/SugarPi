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

#define PBASE 0x3F000000

#define SCALER_DISPLIST0                        (PBASE+0x00400020)
#define SCALER_DISPLIST1                        (PBASE+0x00400024)
#define SCALER_DISPLIST2                        (PBASE+0x00400028)

#define SCALER_CTL0_END                         1U << 31
#define SCALER_CTL0_VALID                       1U << 30
#define SCALER_CTL0_UNITY                       1U << 4

#define SCALER_CTL0_RGBA_EXPAND_ZERO            0
#define SCALER_CTL0_RGBA_EXPAND_LSB             1
#define SCALER_CTL0_RGBA_EXPAND_MSB             2
#define SCALER_CTL0_RGBA_EXPAND_ROUND           3

#define SCALER_CTL0_HFLIP                       1U << 16
#define SCALER_CTL0_VFLIP                       1U << 15

static inline void put32 (uintptr nAddress, u32 nValue)
{
	*(u32 volatile *) nAddress = nValue;
}

static volatile unsigned int* dlist_memory = (unsigned int*) 0x3F402000;

/* We'll use a simple "double buffering" scheme to avoid writing out a new display list while
   one is still in-flight. */
static const unsigned short dlist_buffer_count = 2;
static const unsigned short dlist_offsets[] = { 0, 128 };
static unsigned short next_dlist_buffer = 0;

#define WRITE_WORD(word) (dlist_memory[(*offset)++] = word)


DisplayPiImp::DisplayPiImp(CLogger* logger, CTimer* timer) :DisplayPi(logger),
   timer_(timer),
   frame_buffer_(nullptr),
   mutex_(TASK_LEVEL),
   current_structure_(nullptr)   
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

void DisplayPiImp::InterruptionHandler()
{
   // Sync interrupt
}

void DisplayPiImp::InterruptStub (void *pParam)
{
   static_cast<DisplayPiImp*>(pParam)->InterruptionHandler();
}

bool DisplayPiImp::InitInterrupt(CInterruptSystem* interrupt)
{
   // 3f : sync frame ??
   interrupt->ConnectIRQ (142, &InterruptStub, this);
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
   if (current_structure_->nb_buffers_ == 0 )
   {
      // Nothing to do
   }
   else
   {
      current_buffer_ ++;
      if (current_structure_->nb_buffers_ <= current_buffer_)
      {
         current_buffer_ = 0;
      }
      // Update structure
      UpdateWindowsConfiguration();
   }

   //logger_->Write("Display", LogNotice, "SetFrame : 143, %i", 47 + frame_index * HEIGHT_VIRTUAL_SCREEN);
   //frame_buffer_->SetVirtualOffset(143, 47 + frame_index * HEIGHT_VIRTUAL_SCREEN);

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


void DisplayPiImp::SetWindowsConfiguration(WindowStructure* window_structure, int nb_win)
{
   // Create plane list
   nb_windows_ = nb_win;
   current_structure_ = window_structure;
   // Reset current buffer
   current_buffer_ = 0;
   animation_step_ = 0;

   if ( plane_)
   {
      delete []plane_ ;
      plane_ = new hvs_plane[nb_windows_];
   }
}

void DisplayPiImp::write_plane(unsigned short* offset, hvs_plane plane)
{
    /* Write out the words for this plane. Each word conveys some information to the HVS on how it
       should interpret this plane. */

    /* Control Word */
    const unsigned char number_of_words = 7;
    unsigned int control_word = SCALER_CTL0_VALID              |        // denotes the start of a plane
                            SCALER_CTL0_UNITY              |        // indicates no scaling
                            plane.pixel_order       << 13  |        // pixel order
                            number_of_words         << 24  |        // number of words in this plane
                            plane.format;                           // pixel format
    WRITE_WORD(control_word);

    /* Position Word 0 */
    unsigned int position_word_0 = plane.start_x        << 0   |
                               plane.start_y        << 12;
    WRITE_WORD(position_word_0);

    /* Position Word 1: scaling, only if non-unity */

    /* Position Word 2 */
    unsigned int position_word_2 = plane.width         << 0    |
                               plane.height        << 16;
    WRITE_WORD(position_word_2);

    /* Position Word 3: used by HVS */
    WRITE_WORD(0xDEADBEEF);

    /* Pointer Word */
    /* This cast is okay, because the framebuffer pointer can always be held in 4 bytes
       even though we're on a 64 bit architecture. */
    unsigned int framebuffer = (unsigned int) (int ptr) plane.framebuffer;
    WRITE_WORD(0x80000000 | framebuffer);

    /* Pointer Context: used by HVS */
    WRITE_WORD(0xDEADBEEF);

    /* Pitch Word */
    unsigned int pitch_word = plane.pitch;
    WRITE_WORD(pitch_word);
}

void DisplayPiImp::write_display_list(hvs_plane planes[], unsigned char count)
{
    unsigned short offset = dlist_offsets[next_dlist_buffer];
    const unsigned short start = offset;

    /* Write out each plane. */
    for (unsigned char p = 0; p < count; p++) {
        write_plane(&offset, planes[p]);
    }

    /* End Word */
    dlist_memory[offset] = SCALER_CTL0_END;

    /* Tell the HVS where the display list is by writing to the SCALER_DISPLIST1 register. */
    put32(SCALER_DISPLIST1, start);

    next_dlist_buffer = (next_dlist_buffer + 1) % dlist_buffer_count;
}

void DisplayPiImp::UpdateWindowsConfiguration()
{
   if (current_structure_ != nullptr)
   {
      for (int i = 0; i < nb_windows_; i++)
      {
         plane_[i].format = current_structure_->format_;
         plane_[i].pixel_order = current_structure_->order_;
         plane_[i].start_x = current_structure_->x_;
         plane_[i].start_y = current_structure_->y_;
         plane_[i].width = current_structure_->w_;
         plane_[i].height = current_structure_->h_;
         plane_[i].pitch = current_structure_->pitch_;
         plane_[i].framebuffer = current_structure_->buffer_[current_buffer_];
      }

      write_display_list(plane_, nb_windows_);
   }
}

