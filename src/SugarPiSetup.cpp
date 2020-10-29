//
#include "SugarPiSetup.h"


SugarPiSetup::SugarPiSetup() : display_(nullptr), sound_(nullptr)
{

}

SugarPiSetup::~SugarPiSetup()
{
   
}

void  SugarPiSetup::Init(DisplayPi* display, SoundPi* sound)
{
   display_ = display;
   sound_ = sound;
}

void SugarPiSetup::Load()
{
   
   // Syncronisation
   // Hardware configuration
   // Current cartridge
   
}

void SugarPiSetup::Save()
{
   // Syncronisation
   // Hardware configuration
   // Current cartridge

}

