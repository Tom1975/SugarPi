#pragma once

#include <windows.h>
#include <mmsystem.h>

#include "CPCCore/CPCCoreEmu/ISound.h"
#include "CLogger.h"



#define NBBUFFERS  25
// Buffer size : 2 channel, 16 bits, 1000 sample ( 1 / 44,1 of a second)
#define BUFFERSIZE 2*2*882


class SoundPi : public ISound // , public ICfg
{
public:
   SoundPi(CLogger* log);
   virtual ~SoundPi(void);

   void InitObject(HINSTANCE hInstance);
   void InitMixer(ISoundMixer* mixer) {
      mixer_ = mixer;
   };

   virtual unsigned int GetMaxValue() { return (1 << (GetBitDepth())) - 1; }
   virtual unsigned int GetMinValue() { return 0; }

   virtual bool Init(int sampleRate, int samplebits, int nbChannels);
   virtual void Reinit();
   virtual unsigned int GetSampleRate();
   virtual unsigned int GetBitDepth();
   virtual unsigned int GetNbChannels() { return m_NbChannels; };
   virtual IWaveHDR* GetFreeBuffer();
   virtual void AddBufferToPlay(IWaveHDR* pWave);

   virtual void SetDefaultConfiguration();
   virtual void SaveConfiguration(const char* config_name, const char* ini_file);
   virtual bool LoadConfiguration(const char* config_name, const char* ini_file);

   virtual void SetVolume(WORD volume);
   virtual WORD GetVolume();
   virtual void Mute(bool bMute) { m_bMute = bMute; };
   virtual bool IsMuted() { return m_bMute; };

   void FreeWav();
   virtual void SyncWithSound();

   static void CALLBACK CallbackSound(HWAVEOUT m_hWO, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);
   int buffer_in_queue_;

protected:
   bool Init();
   void CheckBuffersStatus();

   bool reinit_order_;
   ISoundMixer* mixer_;
   int m_SampleRate;
   int m_SampleBits;
   int m_NbChannels;

   bool m_bMute;

   HWAVEOUT m_hWaveOut;

   class WaveHDRImp : public IWaveHDR
   {
   public:
      WAVEHDR WaveHDR;
   };
   WaveHDRImp m_WaveHDR[NBBUFFERS];
   CLogger* log_;

};
