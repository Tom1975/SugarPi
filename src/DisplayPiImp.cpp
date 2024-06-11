//
#include "DisplayPiImp.h"

#include <memory.h>

#include <circle/types.h>
#include <circle/bcmpropertytags.h>
#include <circle/debug.h>
#include <circle/multicore.h>
#include "bcm_host.h"
#include <circle/addon/vc4/interface/vcinclude/common.h>

#include <math.h> 

#include "res/button_1.h"
#include "res/coolspot.h"
#include "res/SugarboxLogo.h"


#define WIDTH_SCREEN 640
#define HEIGHT_SCREEN 480

#define WIDTH_VIRTUAL_SCREEN 1024
#define HEIGHT_VIRTUAL_SCREEN (288*2)


#define ELEMENT_CHANGE_LAYER          (1<<0)
#define ELEMENT_CHANGE_OPACITY        (1<<1)
#define ELEMENT_CHANGE_DEST_RECT      (1<<2)
#define ELEMENT_CHANGE_SRC_RECT       (1<<3)
#define ELEMENT_CHANGE_MASK_RESOURCE  (1<<4)
#define ELEMENT_CHANGE_TRANSFORM      (1<<5)

DisplayPiImp::DisplayPiImp(CLogger* logger, CTimer* timer) :DisplayPi(logger),
   timer_(timer),
   mutex_(TASK_LEVEL)   
{
}

DisplayPiImp::~DisplayPiImp()
{
}

#ifndef ALIGN_UP
#define ALIGN_UP(x,y)  ((x + (y)-1) & ~((y)-1))
#endif

bool DisplayPiImp::Initialization()
{
   logger_->Write("Display", LogNotice, "Initialization...");

   ListEDID();

   // TODO :Init dispmanx
   uint32_t        screen = 0;

   bcm_host_init();

   printk("Open display[%i]...\n", screen );
   vars_.display = vc_dispmanx_display_open( screen );

   int ret = vc_dispmanx_display_get_info( vars_.display, &vars_.info);
   assert(ret == 0);

   //int pitch = ALIGN_UP(vars_.info.width*4, 32);
   int pitch = ALIGN_UP(WIDTH_VIRTUAL_SCREEN*4, 32);
   int aligned_height = ALIGN_UP(HEIGHT_VIRTUAL_SCREEN, 16);
   printk( "Display is %d x %d - pitch : %i\n", vars_.info.width, vars_.info.height, pitch );

   // create various objects :
   // background
   back_frame_.Init(vars_.info.width, vars_.info.height, 1);
   back_wnd_.frame_ = &back_frame_;
   back_wnd_.type_of_image_ = VC_IMAGE_XRGB8888;
   back_wnd_.resource_ = vc_dispmanx_resource_create (back_wnd_.type_of_image_, back_wnd_.frame_->GetFullWidth(), back_wnd_.frame_->GetFullHeight(), &back_wnd_.ptr_);
   windows_list_.push_back(back_wnd_);

   emu_frame_.Init(vars_.info.width, vars_.info.height, 3);
   emu_wnd_.frame_ = &emu_frame_;
   emu_wnd_.type_of_image_ = VC_IMAGE_XRGB8888;
   emu_wnd_.resource_ =  vc_dispmanx_resource_create (emu_wnd_.type_of_image_, emu_wnd_.frame_->GetFullWidth(), emu_wnd_.frame_->GetFullHeight(), &emu_wnd_.ptr_);
   windows_list_.push_back(emu_wnd_);

   // Menu
   menu_frame_.Init (vars_.info.width, vars_.info.height, 1);
   menu_wnd_.frame_ = &menu_frame_;
   menu_wnd_.type_of_image_ = VC_IMAGE_ARGB8888;
   menu_wnd_.element_ = vc_dispmanx_resource_create (menu_wnd_.type_of_image_, menu_wnd_.frame_->GetFullWidth(), menu_wnd_.frame_->GetFullHeight(), &menu_wnd_.ptr_);
   windows_list_.push_back(menu_wnd_);

   // Write background
   int width = back_wnd_.frame_->GetFullWidth();
   int height = back_wnd_.frame_->GetFullHeight();
   
   VC_RECT_T back_rect;
   vc_dispmanx_rect_set(&(back_rect),
                        0,
                        0,
                        back_wnd_.frame_->GetFullWidth(),
                        back_wnd_.frame_->GetFullHeight()
                        );   

   /*int back_pitch = ALIGN_UP(width*4, 32);

   background_buffer_ = new int [back_pitch*height];
   logger_->Write("Display", LogNotice, "background_buffer_ allocated - pitch = %i", back_pitch);
   for (int i = 0; i < height; i++)
   {
      for (int j = 0; j < width; j++)
      {
         if ( (( (j & 0x3F) < 0x20) && ((i&0x3F) < 0x20)  )
         ||(( (j & 0x3F) >= 0x20) && ((i&0x3F) >= 0x20)) )
         {
            background_buffer_[i*width + j] = 0xFFCCCCCC;
         }
         else
         {
            background_buffer_[i*width + j] = 0xFFDDDDDD;
         }
      }
   }
   logger_->Write("Display", LogNotice, "background_buffer_ generated");
   */
   printk( "vc_dispmanx_resource_write_data back_wnd_ : Pitch = %i.w = %i, h = %i\n",
    back_wnd_.frame_->GetPitch(),
    back_rect.width,
    back_rect.height);

   int result = vc_dispmanx_resource_write_data(back_wnd_.resource_,
                                          VC_IMAGE_XRGB8888,
                                          back_wnd_.frame_->GetPitch(),
                                          back_wnd_.frame_->GetBuffer(),
                                          &back_rect);
   logger_->Write("Display", LogNotice, "background_buffer_ written");

   if ( result != 0)
   {
      logger_->Write("Display", LogNotice, "vc_dispmanx_resource_write_data result = %i ", result);
   }


   // Create 
   DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(10);

   VC_DISPMANX_ALPHA_T alpha = 
   {
      flags: DISPMANX_FLAGS_ALPHA_FIXED_ALL_PIXELS,
      opacity: 0x000000FF,
      mask: 0
   };
   //---------------------------------------------------------------------
   logger_->Write("Display", LogNotice, " ####SCREEN W : %i; H : %i ", vars_.info.width, vars_.info.height);

   VC_RECT_T src_back_rect;
   vc_dispmanx_rect_set(&src_back_rect, 0,0 , (width) <<16, (height)<<16);

   VC_RECT_T src_rect;
   //vc_dispmanx_rect_set(&src_rect, 147, 47, (768-147) <<16, (277-47)<<16);
   vc_dispmanx_rect_set(&src_rect, 147<<16, 47<<16, (768-147) <<16, (277-47)<<16);
   //vc_dispmanx_rect_set(&src_rect, 0,0 , WIDTH_VIRTUAL_SCREEN<<16, HEIGHT_VIRTUAL_SCREEN<<16);

   VC_RECT_T dst_rect;
   //vc_dispmanx_rect_set(&dst_rect, 0, 0, vars_.info.width, vars_.info.height);
   vc_dispmanx_rect_set(&dst_rect, 100, 100, vars_.info.width-200, vars_.info.height-200);

   back_wnd_.element_ = vc_dispmanx_element_add(update,
                              vars_.display,
                              1000,
                              &dst_rect,
                              back_wnd_.resource_,
                              &src_back_rect,
                              DISPMANX_PROTECTION_NONE,
                              &alpha,
                              NULL,
                              DISPMANX_NO_ROTATE);
                                          
   emu_wnd_.element_ = vc_dispmanx_element_add(update,
                              vars_.display,
                              2000,
                              &dst_rect,
                              emu_wnd_.resource_,
                              &src_rect,
                              DISPMANX_PROTECTION_NONE,
                              &alpha,
                              NULL,
                              DISPMANX_NO_ROTATE);


   vc_dispmanx_element_change_attributes (update, emu_wnd_.element_, ELEMENT_CHANGE_SRC_RECT, 0, 0, 0, &src_rect, 0, DISPMANX_NO_ROTATE);

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

   return  (int*)(&emu_frame_.GetBuffer()[y * emu_frame_.GetPitch()]);
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

   back_wnd_.frame_->Refresh();
   // Copy framebuffer
   VC_RECT_T bmp_rect;
   vc_dispmanx_rect_set(&(bmp_rect),
                        0,
                        0,
                        WIDTH_VIRTUAL_SCREEN,
                        HEIGHT_VIRTUAL_SCREEN);

   int pitch = ALIGN_UP(WIDTH_VIRTUAL_SCREEN*4, 32);

   static float value = 0;



#define BACK_MOVE 0x10
   int back_x = (BACK_MOVE + sinf(value*4)*BACK_MOVE);
   int back_y = (BACK_MOVE + cos(value*4)*BACK_MOVE);
   logger_->Write("Display", LogNotice, "sin back: x = %i; y = %i ", back_x, back_y);
   value += 0.01;
   
      VC_RECT_T src_rect, dst_rect, back_src_rect, back_dst_rect;
   vc_dispmanx_rect_set(&src_rect, 147<<16, 47<<16, (768-147) <<16, (277-47)<<16);
   vc_dispmanx_rect_set(&dst_rect, fabs(sinf(value)*200.f), fabs(sinf(value)*200.f), vars_.info.width - 2*fabs(sinf(value)*200.f), vars_.info.height-2*fabs(sinf(value)*200.f));

   //vc_dispmanx_rect_set(&back_src_rect, back_wnd_.frame_->GetDisplayX()<<16, back_wnd_.frame_->GetDisplayY()<<16, back_wnd_.frame_->GetDisplayWidth()<<16, back_wnd_.frame_->GetDisplayHeight()<<16);
   back_x = back_wnd_.frame_->GetOffsetX();
   back_y = back_wnd_.frame_->GetOffsetY();
   logger_->Write("Display", LogNotice, "sin back - 2: x = %i; y = %i ", back_x, back_y);
   vc_dispmanx_rect_set(&back_src_rect, back_x<<16, back_y<<16, vars_.info.width<<16, vars_.info.height<<16);


   vc_dispmanx_rect_set(&back_dst_rect, 0, 0, vars_.info.width, vars_.info.height);
   value += 0.01;

   int result = vc_dispmanx_resource_write_data(emu_wnd_.resource_,
                                          VC_IMAGE_XRGB8888,
                                          emu_wnd_.frame_->GetPitch(),
                                          emu_wnd_.frame_->GetBuffer(),
                                          &(bmp_rect));

   if ( result != 0)
   {
      logger_->Write("Display", LogNotice, "vc_dispmanx_resource_write_data result = %i ", result);
   }

   DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(0);

   //vc_dispmanx_element_change_source (update, emu_wnd_.element_, main_resource_[current_buffer_]);
   vc_dispmanx_element_change_attributes (update, emu_wnd_.element_, ELEMENT_CHANGE_DEST_RECT, 0, 0, &dst_rect, &src_rect, 0, DISPMANX_NO_ROTATE);
   vc_dispmanx_element_change_attributes (update, back_wnd_.element_, ELEMENT_CHANGE_SRC_RECT, 0, 0, &back_dst_rect, &back_src_rect, 0, DISPMANX_NO_ROTATE);
   

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

void DisplayPiImp::CopyMemoryToRessources()
{
   for (auto it : windows_list_)
   {
      if ( it.frame_->HasFrameChanged())
      {
         VC_RECT_T bmp_rect;
         vc_dispmanx_rect_set(&(bmp_rect),
                              0,
                              0,
                              it.frame_->GetFullWidth(),
                              it.frame_->GetFullHeight());

         vc_dispmanx_resource_write_data(it.resource_,
                                                it.type_of_image_,
                                                it.frame_->GetPitch(),
                                                it.frame_->GetBuffer(),
                                                &bmp_rect);
      }
   }
}