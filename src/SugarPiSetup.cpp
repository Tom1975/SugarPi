//
#include "SugarPiSetup.h"

#define DRIVE		"SD:"

SugarPiSetup::SugarPiSetup( CLogger* log) : log_(log), display_(nullptr), sound_(nullptr), config_(log)
{

}

SugarPiSetup::~SugarPiSetup()
{
   
}

void  SugarPiSetup::Init(DisplayPi* display, SoundMixer* sound)
{
   display_ = display;
   sound_ = sound;
}

void SugarPiSetup::Load()
{
   config_.OpenFile(DRIVE "/Config/config");

   // Syncronisation
   #define SIZE_OF_BUFFER 32
   char sync_buffer[SIZE_OF_BUFFER];
   if (config_.GetConfiguration ("SETUP", "sync", "sound", sync_buffer, SIZE_OF_BUFFER ))
   {
      if (strcmp ( sync_buffer, "sound") == 0)
      {
         SetSync(SYNC_SOUND);
      }
      else if (strcmp ( sync_buffer, "frame") == 0)
      {
         SetSync(SYNC_FRAME);
      }
   }

   // Hardware configuration
   // Current cartridge
   
}

void SugarPiSetup::Save()
{
   // Syncronisation
   // Hardware configuration
   // Current cartridge

   config_.CloseFile();
}

void  SugarPiSetup::SetSync (SYNC_TYPE sync)
{
   sync_ = sync;
   if (sync_==SYNC_SOUND)
   {
      display_->SyncWithFrame(false);
      sound_->SyncOnSound(true);      
   }
   else
   {
      display_->SyncWithFrame(true);
      sound_->SyncOnSound(false);
   }
}

 SugarPiSetup::SYNC_TYPE  SugarPiSetup::GetSync ()
{
   return sync_;
}

