#pragma once

//
#include <circle/logger.h>
#include <circle/pwmsounddevice.h>

#include "CPCCore/CPCCoreEmu/ISound.h"


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
   virtual unsigned int GetSampleRate();
   virtual unsigned int GetBitDepth();
   virtual unsigned int GetNbChannels();
   virtual void CheckBuffersStatus();

   virtual IWaveHDR* GetFreeBuffer();
   virtual void AddBufferToPlay(IWaveHDR*);

   virtual void SyncWithSound() {};

protected:
   CLogger*          logger_;
   CPWMSoundDevice*	m_PWMSoundDevice;
   char*             buffer_;
   IWaveHDR          data_;
};