#pragma once

//
#include <circle/logger.h>
#include <circle/pwmsounddevice.h>
#include <circle/soundbasedevice.h>

#include "CPCCore/CPCCoreEmu/ISound.h"


#define QUEUE_SIZE 8

class SoundPi : public ISound
{
public:
   SoundPi(CLogger* logger, CInterruptSystem	*interrupt);
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
   CSoundBaseDevice* sound_device_;

   unsigned          queue_size_frames_;
   bool              started_;
   char*             buffer_;
   IWaveHDR          data_[QUEUE_SIZE];


   // Output buffer
   unsigned char*    chunk_buffer;
   unsigned int      index_write_;
   unsigned int      index_output_;
};