//
#include "DisplayPiImp.h"

#include <memory.h>

#include <circle/types.h>
#include <circle/bcmpropertytags.h>
#include <circle/debug.h>
#include <circle/multicore.h>
#include "bcm_host.h"
#include <circle/addon/vc4/interface/vcinclude/common.h>


#include "res/button_1.h"
#include "res/coolspot.h"


#define WIDTH_SCREEN 640
#define HEIGHT_SCREEN 480

#define WIDTH_VIRTUAL_SCREEN 1024
#define HEIGHT_VIRTUAL_SCREEN (288*2)

typedef struct
{
    DISPMANX_DISPLAY_HANDLE_T   display;
    DISPMANX_MODEINFO_T         info;
    void                       *image;
    DISPMANX_UPDATE_HANDLE_T    update;
    DISPMANX_RESOURCE_HANDLE_T  resource;
    DISPMANX_ELEMENT_HANDLE_T   element;
    uint32_t                    vc_image_ptr;

} RECT_VARS_T;

DisplayPiImp::DisplayPiImp(CLogger* logger, CTimer* timer) :DisplayPi(logger),
   timer_(timer),
   mutex_(TASK_LEVEL)   
{
}

DisplayPiImp::~DisplayPiImp()
{
}

static void FillRect( void *image, int pitch, int aligned_height, int x, int y, int w, int h, int val )
{
    int         row;
    int         col;

   // pitch /4 because ARGB 32 bits !
    uint32_t *line = (uint32_t *)image + y * (pitch>>2) + x;

    for ( row = 0; row < h; row++ )
    {
        for ( col = 0; col < w; col++ )
        {
            line[col] = val;
        }
        line += (pitch>>2);
    }
}

#ifndef ALIGN_UP
#define ALIGN_UP(x,y)  ((x + (y)-1) & ~((y)-1))
#endif

bool DisplayPiImp::Initialization()
{
   logger_->Write("Display", LogNotice, "Initialization...");

   ListEDID();

   // TODO :Init dispmanx
   RECT_VARS_T    *vars;   
   static RECT_VARS_T gRectVars;
   uint32_t        screen = 0;
   vars = &gRectVars;

   bcm_host_init();

   printk("Open display[%i]...\n", screen );
   vars->display = vc_dispmanx_display_open( screen );

   int ret = vc_dispmanx_display_get_info( vars->display, &vars->info);
   assert(ret == 0);

   //int pitch = ALIGN_UP(vars->info.width*4, 32);
   int pitch = ALIGN_UP(WIDTH_VIRTUAL_SCREEN*4, 32);
   int aligned_height = ALIGN_UP(HEIGHT_VIRTUAL_SCREEN, 16);
   printk( "Display is %d x %d - pitch : %i\n", vars->info.width, vars->info.height, pitch );

   // Image : depending on the screen !
   vars->image = calloc( 1, vars->info.height * pitch);

    FillRect( display_buffer_[0], pitch, aligned_height,  0,  0, WIDTH_VIRTUAL_SCREEN,      HEIGHT_VIRTUAL_SCREEN,       0xFFFFFFFF );
    FillRect( display_buffer_[0], pitch, aligned_height, 20, 20, WIDTH_VIRTUAL_SCREEN - 40, HEIGHT_VIRTUAL_SCREEN -40,   0xFF00F800 );
    FillRect( display_buffer_[0], pitch, aligned_height, 40, 40, WIDTH_VIRTUAL_SCREEN - 80, HEIGHT_VIRTUAL_SCREEN - 800, 0xFF0007E0 );
    FillRect( display_buffer_[0], pitch, aligned_height, 60, 60, WIDTH_VIRTUAL_SCREEN - 120,HEIGHT_VIRTUAL_SCREEN - 120, 0xFF00001F );

   // create various objects :
   // Main display for emulation
   for (int i = 0; i < FRAME_BUFFER_SIZE; i++)
      main_resource_[i] = vc_dispmanx_resource_create (VC_IMAGE_ARGB8888, WIDTH_VIRTUAL_SCREEN, HEIGHT_VIRTUAL_SCREEN, &main_ptr_);
   //main_resource_[1] = vc_dispmanx_resource_create (VC_IMAGE_ARGB8888, WIDTH_VIRTUAL_SCREEN, HEIGHT_VIRTUAL_SCREEN, &back_ptr_);
   //menu_resource_ = vc_dispmanx_resource_create (VC_IMAGE_ARGB8888, WIDTH_VIRTUAL_SCREEN, HEIGHT_VIRTUAL_SCREEN, &menu_ptr_);

   /*if ( main_resource_ == 0)
   {
      logger_->Write("Display", LogNotice, "vc_dispmanx_resource_create result = 0...  ");
   }*/


   // Add main layer
   VC_RECT_T bmp_rect;
   vc_dispmanx_rect_set(&(bmp_rect),
                        0,
                        0,
                        WIDTH_VIRTUAL_SCREEN,
                        HEIGHT_VIRTUAL_SCREEN);

   int result = vc_dispmanx_resource_write_data(main_resource_[0],
                                          VC_IMAGE_ARGB8888,
                                          pitch,
                                          display_buffer_[0],
                                          &bmp_rect);
   if ( result != 0)
   {
      logger_->Write("Display", LogNotice, "vc_dispmanx_resource_write_data result = %i ", result);
   }


   // Create 
   DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(10);

   VC_DISPMANX_ALPHA_T alpha =
   {
      DISPMANX_FLAGS_ALPHA_FROM_SOURCE, 
      120,
      0
   };

   //---------------------------------------------------------------------
   logger_->Write("Display", LogNotice, " ####SCREEN W : %i; H : %i ", vars->info.width, vars->info.height);

   VC_RECT_T src_rect;
   //vc_dispmanx_rect_set(&src_rect, 147, 47, (768-147) <<16, (277-47)<<16);
   vc_dispmanx_rect_set(&src_rect, 0,0 , WIDTH_VIRTUAL_SCREEN<<16, HEIGHT_VIRTUAL_SCREEN<<16);

   VC_RECT_T dst_rect;
   //vc_dispmanx_rect_set(&dst_rect, 147, 47, (768-147), (277-47));
   vc_dispmanx_rect_set(&dst_rect, 20, 20, vars->info.width-10, vars->info.height-10);
   //vc_dispmanx_rect_set(&dst_rect, 0, 0, vars->info.width, vars->info.height);
   //vc_dispmanx_rect_set(&dst_rect, 0, 0, WIDTH_VIRTUAL_SCREEN, HEIGHT_VIRTUAL_SCREEN);

                                          
   element_ =
      vc_dispmanx_element_add(update,
                              vars->display,
                              2000,
                              &dst_rect,
                              main_resource_[0],
                              &src_rect,
                              DISPMANX_PROTECTION_NONE,
                              &alpha,
                              NULL,
                              DISPMANX_NO_ROTATE);


   result = vc_dispmanx_update_submit_sync(update);
   if ( result != 0)
   {
      logger_->Write("Display", LogNotice, "  vc_dispmanx_update_submit_sync => result = %i ", result);
   }

   logger_->Write("Display", LogNotice, " End init.. Draw done.");

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
   // todo
}

int DisplayPiImp::GetStride()
{
   // TODO : stride is for int* !!!
   return WIDTH_VIRTUAL_SCREEN;
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

   return &display_buffer_[current_buffer_][ y * 1024  ];
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
   //logger_->Write("Draw", LogNotice, " SetFrame : %i", frame_index);
   current_buffer_ = frame_index;
}

void DisplayPiImp::Draw()
{
   // Copy framebuffer
   VC_RECT_T bmp_rect;
   vc_dispmanx_rect_set(&(bmp_rect),
                        0,
                        0,
                        WIDTH_VIRTUAL_SCREEN,
                        HEIGHT_VIRTUAL_SCREEN);

   int pitch = ALIGN_UP(WIDTH_VIRTUAL_SCREEN*4, 32);


   int result = vc_dispmanx_resource_write_data(main_resource_[current_buffer_],
                                          VC_IMAGE_ARGB8888,
                                          pitch,
                                          display_buffer_[current_buffer_],
                                          &(bmp_rect));

   if ( result != 0)
   {
      logger_->Write("Display", LogNotice, "vc_dispmanx_resource_write_data result = %i ", result);
   }

   DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(0);

   vc_dispmanx_element_change_source (update, element_, main_resource_[current_buffer_]);
   
   result = vc_dispmanx_update_submit_sync(update);
   if ( result != 0)
   {
      logger_->Write("Display", LogNotice, "vc_dispmanx_update_submit_sync result = %i ", result);
   }
}

void DisplayPiImp::ClearBuffer(int frame_index)
{
   // todo
}