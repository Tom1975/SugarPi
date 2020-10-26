#pragma once

//
#include <circle/logger.h>
#include <circle/pwmsounddevice.h>
#include <circle/soundbasedevice.h>
#include <vc4/vchiq/vchiqdevice.h>
#include <vc4/sound/vchiqsoundbasedevice.h>
#include <circle/sched/scheduler.h>

#include "CPCCore/CPCCoreEmu/ISound.h"


#define QUEUE_SIZE 8

class SoundPi : public ISound
{
public:
#ifdef USE_VCHIQ_SOUND   
   SoundPi(CLogger* logger, CVCHIQDevice* vchiq_device, CScheduler* scheduler);
#else
   SoundPi(CLogger* logger, CInterruptSystem* interrupt, CScheduler* scheduler);
#endif
   virtual ~SoundPi();

   virtual void SetDefaultConfiguration() {}
   virtual void SaveConfiguration(const char* config_name, const char* ini_file) {}
   virtual bool LoadConfiguration(const char* config_name, const char* ini_file) { return true; }

   virtual bool Init(int sample_rate, int sample_bits, int nb_channels);
   virtual void Reinit();
   virtual unsigned int GetMaxValue() ;
   virtual unsigned int GetMinValue() ;
   virtual unsigned int GetSampleRate();
   virtual unsigned int GetBitDepth();
   virtual unsigned int GetNbChannels();
   virtual void CheckBuffersStatus();

   virtual IWaveHDR* GetFreeBuffer();
   virtual void AddBufferToPlay(IWaveHDR*);

   virtual void SyncWithSound() {};

   void Initialize();

   virtual unsigned GetChunk(s16 *pBuffer, unsigned nChunkSize);

protected:
   CLogger*          logger_;
   CScheduler*       scheduler_;

#ifdef USE_VCHIQ_SOUND   
   CVCHIQDevice*     vchiq_device_;
   CVCHIQSoundBaseDevice*sound_device_;
#else
   CPWMSoundBaseDevice * sound_device_;
   CInterruptSystem*	interrupt_;
#endif

   unsigned          queue_size_frames_;
   bool              started_;
   char*             buffer_;
   IWaveHDR          data_[QUEUE_SIZE];


   // Output buffer
   unsigned char*    chunk_buffer;
   unsigned int      index_write_;
   unsigned int      index_output_;
};