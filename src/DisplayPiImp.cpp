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
   display_ = vc_dispmanx_display_open( screen );

   int ret = vc_dispmanx_display_get_info( display_, &info_);
   assert(ret == 0);

   uint32_t supported_formats = 0;
   ret = vc_dispmanx_query_image_formats(&supported_formats);
   logger_->Write("Display", LogNotice, "vc_dispmanx_query_image_formats : %X", supported_formats);

   DisplayPi::Initialization();

   // create various objects :
   //----------------------
   // background
   //back_frame_.Init(info_.width, info_.height, 1);
   back_wnd_.frame_ = &back_frame_;
   back_wnd_.type_of_image_ = VC_IMAGE_XRGB8888;
   back_wnd_.resource_ = vc_dispmanx_resource_create (back_wnd_.type_of_image_, back_wnd_.frame_->GetFullWidth(), back_wnd_.frame_->GetFullHeight(), &back_wnd_.ptr_);
   back_wnd_.element_ = 0;
   back_wnd_.priority_ = 10;
   back_wnd_.alpha_ = {     flags: DISPMANX_FLAGS_ALPHA_FIXED_ALL_PIXELS,
                              opacity: 0x000000FF,
                              mask: 0
                           };
   back_wnd_.frame_->SetDisplay(  0, 0 );
   back_wnd_.frame_->SetDisplaySize(  info_.width, info_.height );   
   windows_list_.push_back(back_wnd_);

   //----------------------
   // Main display for emulation
   //emu_frame_.Init(info_.width, info_.height, 3);
   emu_wnd_.frame_ = &emu_frame_;
   emu_wnd_.type_of_image_ = VC_IMAGE_XRGB8888;
   emu_wnd_.resource_ =  vc_dispmanx_resource_create (emu_wnd_.type_of_image_, emu_wnd_.frame_->GetFullWidth(), emu_wnd_.frame_->GetFullHeight(), &emu_wnd_.ptr_);
   emu_wnd_.element_ = 0;
   emu_wnd_.priority_ = 50;
   emu_wnd_.frame_->SetDisplay(  50, 50 );
   emu_wnd_.frame_->SetDisplaySize(  info_.width-100, info_.height-100 );

   emu_wnd_.alpha_ = {   flags: DISPMANX_FLAGS_ALPHA_FIXED_ALL_PIXELS,
                          opacity: 0x000000FF,
                          mask: 0
                       };

   windows_list_.push_back(emu_wnd_);

   //----------------------
   // Menu
   //menu_frame_.Init (info_.width, info_.height, 1);
   menu_wnd_.frame_ = &menu_frame_;
   menu_wnd_.type_of_image_ = VC_IMAGE_ARGB8888;
   menu_wnd_.resource_ = vc_dispmanx_resource_create (menu_wnd_.type_of_image_, menu_wnd_.frame_->GetFullWidth(), menu_wnd_.frame_->GetFullHeight(), &menu_wnd_.ptr_);
   menu_wnd_.element_ = 0;
   menu_wnd_.priority_ = 20;
   menu_wnd_.frame_->SetDisplay(  25, 25 );
   menu_wnd_.frame_->SetDisplaySize(  info_.width-50, info_.height-50 );

   menu_wnd_.alpha_ = {     flags: DISPMANX_FLAGS_ALPHA_FROM_SOURCE,
                              opacity: 0x000000FF,
                              mask: 0
                       };      
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
   logger_->Write("Display", LogNotice, " ####SCREEN W : %i; H : %i ", info_.width, info_.height);

   VC_RECT_T src_back_rect;
   vc_dispmanx_rect_set(&src_back_rect, 0,0 , (width) <<16, (height)<<16);

   VC_RECT_T src_rect;
   vc_dispmanx_rect_set(&src_rect, 147<<16, 47<<16, (768-147) <<16, (277-47)<<16);

   VC_RECT_T dst_rect;
   vc_dispmanx_rect_set(&dst_rect, 100, 100, info_.width-200, info_.height-200);

   logger_->Write("Display", LogNotice, " vc_dispmanx_element_add");
   back_wnd_.element_ = vc_dispmanx_element_add(update,
                              display_,
                              1000,
                              &dst_rect,
                              back_wnd_.resource_,
                              &src_back_rect,
                              DISPMANX_PROTECTION_NONE,
                              &alpha,
                              NULL,
                              DISPMANX_NO_ROTATE);
                                          
   logger_->Write("Display", LogNotice, " vc_dispmanx_element_add - back is done ");
   emu_wnd_.element_ = vc_dispmanx_element_add(update,
                              display_,
                              2000,
                              &dst_rect,
                              emu_wnd_.resource_,
                              &src_rect,
                              DISPMANX_PROTECTION_NONE,
                              &alpha,
                              NULL,
                              DISPMANX_NO_ROTATE);
   logger_->Write("Display", LogNotice, " vc_dispmanx_element_add - emu is done ");                              

   menu_wnd_.element_ = vc_dispmanx_element_add(update,
                              display_,
                              1500,
                              &dst_rect,
                              menu_wnd_.resource_,
                              &src_rect,
                              DISPMANX_PROTECTION_NONE,
                              &menu_wnd_.alpha_,
                              NULL,
                              DISPMANX_NO_ROTATE);

   logger_->Write("Display", LogNotice, " vc_dispmanx_element_add - menu is done ");

   vc_dispmanx_element_change_attributes (update, emu_wnd_.element_, ELEMENT_CHANGE_SRC_RECT, 0, 0, 0, &src_rect, 0, DISPMANX_NO_ROTATE);

   result = vc_dispmanx_update_submit_sync(update);
   if ( result != 0)
   {
      logger_->Write("Display", LogNotice, "  vc_dispmanx_update_submit_sync => result = %i ", result);
   }

   logger_->Write("Display", LogNotice, " End init.. Draw done.");

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
   return info_.width;
}

int DisplayPiImp::GetWidth()
{
   return info_.width;
}

int DisplayPiImp::GetHeight()
{
   return info_.height;
}

int* DisplayPiImp::GetVideoBuffer(int y)
{
   if ( y > emu_frame_.GetHeight()) y = emu_frame_.GetHeight()-1;

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
   int result = 0;
   
   CopyMemoryToRessources();

   Lock();
   emu_frame_.FrameIsDisplayed();
   Unlock();

   static float value = 0;

   VC_RECT_T src_rect, dst_rect, back_src_rect, back_dst_rect;
   vc_dispmanx_rect_set(&src_rect, 147<<16, 47<<16, (768-147) <<16, (277-47)<<16);
   vc_dispmanx_rect_set(&dst_rect, fabs(sinf(value)*200.f), fabs(sinf(value)*200.f), info_.width - 2*fabs(sinf(value)*200.f), info_.height-2*fabs(sinf(value)*200.f));

   int back_x = back_wnd_.frame_->GetOffsetX();
   int back_y = back_wnd_.frame_->GetOffsetY();

   vc_dispmanx_rect_set(&back_src_rect, back_x<<16, back_y<<16, info_.width<<16, info_.height<<16);
   vc_dispmanx_rect_set(&back_dst_rect, 0, 0, info_.width, info_.height);
   value += 0.01;
   
   DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(0);
   
   // keep for testing
   vc_dispmanx_element_change_attributes (update, emu_wnd_.element_, ELEMENT_CHANGE_SRC_RECT|ELEMENT_CHANGE_DEST_RECT, 0, 0, &dst_rect, &src_rect, 0, DISPMANX_NO_ROTATE);
   vc_dispmanx_element_change_attributes (update, back_wnd_.element_, ELEMENT_CHANGE_SRC_RECT, 0, 0, &back_dst_rect, &back_src_rect, 0, DISPMANX_NO_ROTATE);
   
   for (auto it : windows_list_)
   //auto it = windows_list_[0];
   {
      if ( it.frame_->AttributesHasChanged() )
      {
         int back_x = it.frame_->GetOffsetX();
         int back_y = it.frame_->GetOffsetY();

         vc_dispmanx_rect_set(&back_src_rect, back_x<<16, back_y<<16, info_.width<<16, info_.height<<16);
         vc_dispmanx_rect_set(&back_dst_rect, 0, 0, info_.width, info_.height);

         logger_->Write("Display", LogNotice, "vc_dispmanx_element_change_attributes result = %X : %i %i %i %i ", 
         &it, back_x, back_y, info_.width, info_.height);

         vc_dispmanx_element_change_attributes (update, 
            it.element_, ELEMENT_CHANGE_SRC_RECT|ELEMENT_CHANGE_DEST_RECT, 0, 0,
             &back_dst_rect, &back_src_rect,
              0, DISPMANX_NO_ROTATE);
      }

   }
   

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
      it.frame_->Refresh();
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