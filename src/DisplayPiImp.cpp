//
#include "DisplayPiImp.h"

#include <memory.h>

#include <circle/types.h>
#include <circle/bcmpropertytags.h>
#include <circle/debug.h>

#include "res/button_1.h"
#include "res/coolspot.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
#define WIDTH_SCREEN 640
#define HEIGHT_SCREEN 480

#define WIDTH_VIRTUAL_SCREEN 1024
#define HEIGHT_VIRTUAL_SCREEN (288)

//////////////////////////////////////////////////////////////////////
// Address for HVS
//////////////////////////////////////////////////////////////////////

#if RASPPI == 3
   #define BCM_PERIPH_BASE_VIRT    (0x3F000000U)   
#elif RASPPI == 4
   // ARM Base address (Pi4) - We start in the "Low Peripherals mode" by default. DON'T ADD arm_peri_high=1 in config.txt !!!
   #define BCM_PERIPH_BASE_VIRT    (0xFE000000U)   
#else
   #error "Raspberry target not supported"
#endif

// Addresses
#define SCALER_BASE (BCM_PERIPH_BASE_VIRT + 0x400000)

#define SCALER_DISPCTRL     (SCALER_BASE + 0x00)
#define SCALER_DISPSTAT     (SCALER_BASE + 0x04)
#define SCALER_DISPID       (SCALER_BASE + 0x08)
#define SCALER_DISPEOLN     (SCALER_BASE + 0x18)
#define SCALER_DISPLIST0    (SCALER_BASE + 0x20)
#define SCALER_DISPLIST1    (SCALER_BASE + 0x24)
#define SCALER_DISPLIST2    (SCALER_BASE + 0x28)
#define SCALER_DISPCTRL0    (SCALER_BASE + 0x40)


#if RASPPI == 3
   #define SCALER_LIST_MEMORY  (SCALER_BASE + 0x2000)
#elif RASPPI == 4
   #define SCALER_LIST_MEMORY  (SCALER_BASE + 0x4000) //Pi4 = 4
#endif

// values
#define SCALER_DISPCTRL_ENABLE  (1<<31)
#define SCALER_CTL0_END                         1U << 31
#define SCALER_CTL0_VALID                       1U << 30

#if RASPPI == 3
   #define SCALER_CTL0_UNITY                    1U << 4
#elif RASPPI == 4
   #define SCALER5_CTL0_UNITY			            1U << 15
#endif

#define SCALER_CTL0_RGBA_EXPAND_ZERO            0
#define SCALER_CTL0_RGBA_EXPAND_LSB             1
#define SCALER_CTL0_RGBA_EXPAND_MSB             2
#define SCALER_CTL0_RGBA_EXPAND_ROUND           3

#define SCALER_CTL0_HFLIP                       1U << 16
#define SCALER_CTL0_VFLIP                       1U << 15

#define SCALER_DISPCTRLX_ENABLE                 (1<<31)
#define SCALER_DISPCTRLX_RESET                  (1<<30)
#define SCALER_DISPCTRL_W(n)                    ((n & 0xfff) << 12)
#define SCALER_DISPCTRL_H(n)                    (n & 0xfff)
#define SCALER_DISPBKGND_AUTOHS                 (1<<31)
#define SCALER_DISPBKGND_INTERLACE              (1<<30)
#define SCALER_DISPBKGND_GAMMA                  (1<<29)
#define SCALER_DISPBKGND_FILL                   (1<<24)

#define BASE_BASE(n)                            (n & 0xffff)
#define BASE_TOP(n)                             ((n & 0xffff) << 16)

#define CONTROL_END                             (1<<31)

#define REG32(addr) ((volatile unsigned int *)(unsigned long long)(addr))

#define WRITE_WORD(word) (dlist_memory[(*offset)++] = word)

static inline void put32 (uintptr nAddress, u32 nValue)
{
	*(u32 volatile *) nAddress = nValue;
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

//static unsigned int offset = 0;
static volatile unsigned int* dlist_memory = (unsigned int*) SCALER_LIST_MEMORY; 

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

/* We'll use a simple "double buffering" scheme to avoid writing out a new display list while
   one is still in-flight. */
static const unsigned short dlist_buffer_count = 1;
static const unsigned short dlist_offsets[] = { 0, 128 };
static unsigned short next_dlist_buffer = 0;


DisplayPiImp::DisplayPiImp(CLogger* logger, CTimer* timer) :DisplayPi(logger),
   timer_(timer),
   mutex_(TASK_LEVEL),
   current_structure_(nullptr)   
{
   for (int i = 0; i < NB_BUFFERS; i++)
   {
      cpc_buffers_ [i] = new unsigned char[WIDTH_VIRTUAL_SCREEN * HEIGHT_VIRTUAL_SCREEN * sizeof(unsigned int)];
      logger_->Write("Display", LogNotice, "cpc_buffers_ %i  =%8.8X ", i, cpc_buffers_ [i]);

      int* ptr = (int*)cpc_buffers_ [i];
      for (int x = 0; x < WIDTH_VIRTUAL_SCREEN * HEIGHT_VIRTUAL_SCREEN; x++)
      {
         ptr[x] = 0x80FF8080; // mostly red ?
      }
   }
   windows_structures_[Test][0].buffer_ = cpc_buffers_;
}

DisplayPiImp::~DisplayPiImp()
{
   for (int i = 0; i < NB_BUFFERS; i++)
   {
      delete []cpc_buffers_[i];
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

   DisplayPi::Initialization();

   return true;
}

void DisplayPiImp::InterruptionHandler()
{
   // Sync interrupt
   logger_->Write("DisplayPiImp", LogNotice, "InterruptionHandler");
}

void DisplayPiImp::InterruptStub (void *pParam)
{
   DisplayPiImp* obj = static_cast<DisplayPiImp*>(pParam);
   obj->logger_->Write("DisplayPiImp", LogNotice, "InterruptStub");
   obj->InterruptionHandler();
}

bool DisplayPiImp::InitInterrupt(CInterruptSystem* interrupt)
{
   logger_->Write("DisplayPiImp", LogNotice, "InitInterrupt");
   // 3f : sync frame ??
   //interrupt->ConnectIRQ (142, &InterruptStub, this);
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
   //frame_buffer_->WaitForVerticalSync();
   // TODO !
}

int DisplayPiImp::GetStride()
{
   // TODO : stride is for int* !!!
   return WIDTH_VIRTUAL_SCREEN * sizeof(unsigned int);
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
   if ( y > HEIGHT_VIRTUAL_SCREEN) y = HEIGHT_VIRTUAL_SCREEN-1;

   return reinterpret_cast<int*>( cpc_buffers_[0] +  y * WIDTH_VIRTUAL_SCREEN * sizeof(unsigned int));
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
   // test with a single buffer
   /*if (current_structure_->nb_buffers_ == 0 )
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
   */
   //logger_->Write("Display", LogNotice, "SetFrame : 143, %i", 47 + frame_index * HEIGHT_VIRTUAL_SCREEN);
   //frame_buffer_->SetVirtualOffset(143, 47 + frame_index * HEIGHT_VIRTUAL_SCREEN);

}

void DisplayPiImp::Draw()
{
   //logger_->Write("Display", LogNotice, "Draw");
   //frame_buffer_->WaitForVerticalSync(); // TODO
}

void DisplayPiImp::ClearBuffer(int frame_index)
{
   //logger_->Write("Display", LogNotice, "ClearBuffer : frame_index = %i", frame_index);
   //memset( cpc_buffers_[frame_index], 0x0, WIDTH_VIRTUAL_SCREEN * HEIGHT_VIRTUAL_SCREEN * sizeof(unsigned int));
   //logger_->Write("Display", LogNotice, "End clear");
}

void DisplayPiImp::SetSetup(WindowsType setup)
{
   // Definition des fenetres (HVS)
   SetWindowsConfiguration(windows_structures_[setup], window_structures_size_[setup]);
}

void DisplayPiImp::GetFrameBuffer(int w, int h, int p, int color)
{

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
   }
   plane_ = new hvs_plane[nb_windows_];
   for (int i = 0; i < nb_windows_; i++)
   {
      plane_[i].format = window_structure->format_;
      plane_[i].pixel_order = window_structure->order_;
      plane_[i].start_x = window_structure->x_;
      plane_[i].start_y = window_structure->y_;
      plane_[i].width = window_structure->w_;
      plane_[i].height = window_structure->h_;
      plane_[i].pitch = window_structure->pitch_;
      plane_[i].framebuffer = window_structure->buffer_[0];
   }
   write_display_list(plane_, nb_windows_);
}

 
void DisplayPiImp::write_plane(unsigned short* offset, hvs_plane plane)
{
    /* Write out the words for this plane. Each word conveys some information to the HVS on how it
       should interpret this plane. */

    /* Control Word */
    /*unsigned int control_word = SCALER_CTL0_VALID               |        // denotes the start of a plane
                            SCALER5_CTL0_UNITY               |        // indicates no scaling
                            plane.pixel_order       << 13  |        // pixel order
                            number_of_words         << 24  |        // number of words in this plane
                            plane.format;                           // pixel format
                            */
   unsigned int control_word = SCALER_CTL0_VALID | SCALER5_CTL0_UNITY | dlist_memory[(*offset)];
   WRITE_WORD(control_word);

    /* Position Word 0 */
    unsigned int position_word_0 = plane.start_x    << 0   |
                               plane.start_y        << 16;  // 16 for y
   position_word_0 = SCALER_CTL0_VALID | SCALER5_CTL0_UNITY | dlist_memory[(*(offset))];
    WRITE_WORD(position_word_0);

    /* Position Word 1: scaling, only if non-unity */

    /* Position Word 2 */
    unsigned int position_word_2 = plane.width         << 0    |
                               plane.height        << 16;
   position_word_2 = SCALER_CTL0_VALID | SCALER5_CTL0_UNITY | dlist_memory[(*(offset+1))];                               
    WRITE_WORD(position_word_2);

    
    /* Position Word 3: used by HVS */
    WRITE_WORD(0xDEADBEEF);

    /* Pointer Word */
    /* This cast is okay, because the framebuffer pointer can always be held in 4 bytes
       even though we're on a 64 bit architecture. */
    unsigned int framebuffer = (unsigned int) (intptr)( plane.framebuffer);

   logger_->Write("SetSetup", LogNotice, "Preparing FB  ");  

    WRITE_WORD( 0x80000000 | framebuffer);

    /* Pointer Context: used by HVS */
    WRITE_WORD(0xDEADBEEF);

    /* Pitch Word */
    unsigned int pitch_word = plane.pitch;
    WRITE_WORD(pitch_word);

   logger_->Write("SetSetup", LogNotice, "All done");    

}

void DisplayPiImp::write_display_list(hvs_plane planes[], unsigned char count)
{
    unsigned short offset = dlist_offsets[next_dlist_buffer];
    offset = 0;

    /* Write out each plane. */
    //for (unsigned char p = 0; p < count; p++) {
        write_plane(&offset, planes[0]);
    //}

    /* End Word */
    dlist_memory[offset] = SCALER_CTL0_END;

    /* Tell the HVS where the display list is by writing to the SCALER_DISPLIST1 register. */
    put32(SCALER_DISPLIST0, 0);/*
    put32(SCALER_DISPLIST1, 0);
    put32(SCALER_DISPLIST2, 0);*/

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

void DisplayPiImp::DumpDisplayList()
{
   for (int i = 0; i < 32; i += 1)
   {
      logger_->Write("Scaller registers", LogNotice, "Offset : %i; value = %8.8X",  i, *REG32(BCM_PERIPH_BASE_VIRT + 0x400000 + i*4 ));
   }

   logger_->Write("Display list", LogNotice, "SCALER_DISPLIST0 : value = %8.8X", *REG32(SCALER_DISPLIST0) );
   logger_->Write("Display list", LogNotice, "SCALER_DISPLIST1 : value = %8.8X", *REG32(SCALER_DISPLIST1) );
   logger_->Write("Display list", LogNotice, "SCALER_DISPLIST2 : value = %8.8X", *REG32(SCALER_DISPLIST2) );
   
   for (int i = 0; i < 32; i += 1)
   {
      logger_->Write("Display list", LogNotice, "Offset : %i; value = %8.8X",  i, dlist_memory[i]);
   }

   for (int i = 0; i < 32; i += 1)
   {
      logger_->Write("Display list #1", LogNotice, "Offset : %i; value = %8.8X",  (*REG32(SCALER_DISPLIST0))+i, dlist_memory[*REG32(SCALER_DISPLIST0)+i]);
   }
}

void hvs_initialize(CLogger* logger);

void DisplayPiImp::Loop()
{
   loop_run = true;
   logger_->Write("DISPLAY TEST HVS", LogNotice, "Testing HVS");
   
   logger_->Write("DISPLAY TEST HVS", LogNotice, "Initialisation of HVS");
   //hvs_initialize(logger_);
   logger_->Write("DISPLAY TEST HVS", LogNotice, "Initialisation done");
   DumpDisplayList();

   logger_->Write("DISPLAY TEST HVS", LogNotice, "hvs_initialize done");
   SetSetup (Test);
   logger_->Write("DISPLAY TEST HVS", LogNotice, "Setup done");

   // Dump display list content
   DumpDisplayList();

   logger_->Write("Loop", LogNotice, "Start loop");
   /*while (loop_run)
   {
      static unsigned int col = 0;

      unsigned int* pixels = (unsigned int*) test_buffer_;
      for (int i = 0; i < 1080*1920; ++i) {
           cpc_buffers_[0][i] =  0x84; // random color
      }
      // wait
      CTimer::Get ()->MsDelay (20);
   }*/
}



struct hvs_channel {
  volatile unsigned int dispctrl;
  volatile unsigned int dispbkgnd;
  volatile unsigned int dispstat;
  // 31:30  mode
  // 29     full
  // 28     empty
  // 17:12  frame count
  // 11:0   line
  volatile unsigned int dispbase;
};

//volatile unsigned int* dlist_memory;
volatile struct hvs_channel *hvs_channels = (volatile struct hvs_channel*)REG32(SCALER_DISPCTRL0);
int display_slot = 0;

void hvs_wipe_displaylist(void) {
  for (int i=0; i<1024; i++) {
    dlist_memory[i] = CONTROL_END;
  }
  display_slot = 0;
}

void hvs_initialize(CLogger* logger) {


   // Read SCALER_DISPCTRL
   dlist_memory = REG32(SCALER_LIST_MEMORY);
   unsigned int previous_value = *REG32(SCALER_DISPCTRL);
   unsigned int state = *REG32(SCALER_DISPSTAT);
   logger->Write("hvs_initialize", LogNotice, "init value read = %8.8X; State = %8.8X", previous_value, state);

  //timer_initialize(&ddr2_monitor);
  //timer_set_periodic(&ddr2_monitor, 500, ddr2_checker, NULL);
  *REG32(SCALER_DISPCTRL) &= ~SCALER_DISPCTRL_ENABLE; // disable HVS
  *REG32(SCALER_DISPCTRL) = SCALER_DISPCTRL_ENABLE | 0x9a0dddff; // re-enable HVS

   unsigned int dispid = *REG32(SCALER_DISPID);
   logger->Write("hvs_initialize", LogNotice, "SCALER_DISPID : %8.8X => %8.8X", SCALER_DISPID, dispid);

   // Read again SCALER_DISPCTRL
   previous_value = *REG32(SCALER_DISPCTRL);
   state = *REG32(SCALER_DISPSTAT);
   logger->Write("hvs_initialize", LogNotice, "After disable /enable, init value read = %8.8X; State = %8.8X", previous_value, state);

  /*for (int i=0; i<3; i++) {
    hvs_channels[i].dispctrl = SCALER_DISPCTRLX_RESET;
    hvs_channels[i].dispctrl = 0;
    hvs_channels[i].dispbkgnd = 0x1020202; // bit 24
  }

   logger->Write("hvs_initialize", LogNotice, "set dispbase");
  hvs_channels[2].dispbase = BASE_BASE(0)      | BASE_TOP(0x7f0);
  hvs_channels[1].dispbase = BASE_BASE(0xf10)  | BASE_TOP(0x50f0);
  hvs_channels[0].dispbase = BASE_BASE(0x800) | BASE_TOP(0xf00);
*/
   logger->Write("hvs_initialize", LogNotice, "set dispbase done !");
   logger->Write("hvs_initialize", LogNotice, "set hvs_wipe_displaylist...");
   
  //hvs_wipe_displaylist();

  logger->Write("hvs_initialize", LogNotice, "hvs_wipe_displaylist done !");

  //*REG32(SCALER_DISPEOLN) = 0x40000000;
logger->Write("hvs_initialize", LogNotice, "SCALER_DISPEOLN set !");

 logger->Write("hvs_initialize", LogNotice, "TEST ENDED !");  
}