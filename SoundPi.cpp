//
#include "SoundPi.h"

#define SOUND_CHANNELS		2
#define SOUND_BITS		   16
#define SOUND_RATE         44100

SoundPi::SoundPi(CLogger* logger, CInterruptSystem	*interrupt):
   logger_(logger)
{
   m_PWMSoundDevice = new CPWMSoundDevice(interrupt);
   // 1/100e second buffer
   buffer_ = new char[SOUND_BITS/8 * SOUND_CHANNELS * SOUND_RATE / 100];
}

SoundPi::~SoundPi()
{
   delete m_PWMSoundDevice;
}

bool SoundPi::Init(int sample_rate, int sample_bits, int nb_channels)
{
   data_.buffer_length_ = 441;
   data_.data_ = buffer_;
   data_.status_ = IWaveHDR::UNUSED;
   return true;
}

void SoundPi::Reinit()
{

}
unsigned int SoundPi::GetSampleRate()
{
   return SOUND_RATE;
}

unsigned int SoundPi::GetBitDepth()
{
   return SOUND_BITS;
}

unsigned int SoundPi::GetNbChannels()
{
   return SOUND_CHANNELS;
}

void SoundPi::CheckBuffersStatus()
{

}

IWaveHDR* SoundPi::GetFreeBuffer()
{
   return &data_;
}

void SoundPi::AddBufferToPlay(IWaveHDR* wav)
{
   // wait for sample to finish
   while (!m_PWMSoundDevice->PlaybackActive());

   m_PWMSoundDevice->Playback(buffer_, 441, 2, 16);
}
