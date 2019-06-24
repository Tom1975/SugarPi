//

#include <memory.h>
#include <circle/util.h>
#include "SoundPi.h"

#define SOUND_CHANNELS		2
#define SOUND_BITS		   16
#define SOUND_RATE         44100

#define QUEUE_SIZE_MSECS 100
#define SAMPLE_RATE		44100
#define CHUNK_SIZE		4000
#define WRITE_CHANNELS	2

SoundPi::SoundPi(CLogger* logger, CInterruptSystem	*interrupt):
   logger_(logger),
//   CSoundBaseDevice (SoundFormatSigned16, 0, 44100),
   index_write_(0),
   index_output_(0),
   started_(false)
{
   sound_device_ = new CPWMSoundBaseDevice(interrupt, 44100, CHUNK_SIZE);
   // 1/100e second buffer
   for (int i = 0; i < QUEUE_SIZE; i++)
   {
      data_[i].buffer_length_ = SOUND_BITS / 8 * SOUND_CHANNELS * SOUND_RATE / 100;
      data_[i].data_ = new char[SOUND_BITS / 8 * SOUND_CHANNELS * SOUND_RATE / 100];
      data_[i].status_ = IWaveHDR::UNUSED;
   }
   chunk_buffer = new unsigned char[SOUND_BITS / 8 * SOUND_CHANNELS * SOUND_RATE / 10];
}

SoundPi::~SoundPi()
{
   for (int i = 0; i < QUEUE_SIZE; i++)
   {
      delete []data_[i].data_ ;
   }
   //delete m_PWMSoundDevice;
   delete chunk_buffer;
}

void SoundPi::Initialize()
{
   sound_device_->AllocateQueue(QUEUE_SIZE_MSECS);
   sound_device_->SetWriteFormat(SoundFormatSigned16, WRITE_CHANNELS);
   queue_size_frames_ = sound_device_->GetQueueSizeFrames();

   logger_->Write("Sound", LogNotice, "GetRangeMin : %i", sound_device_->GetRangeMin());
   logger_->Write("Sound", LogNotice, "GetRangeMax : %i", sound_device_->GetRangeMax());
   

   logger_->Write("Sound", LogNotice, "Init done");
}

unsigned int SoundPi::GetMaxValue()
{
   return sound_device_->GetRangeMax();
}

unsigned int SoundPi::GetMinValue()
{
   return sound_device_->GetRangeMin();
}


bool SoundPi::Init(int sample_rate, int sample_bits, int nb_channels)
{
   logger_->Write("Sound", LogNotice, "Init");
   return true;
}

void SoundPi::Reinit()
{
   logger_->Write("Sound", LogNotice, "Reinit");
}
unsigned int SoundPi::GetSampleRate()
{
   return SOUND_RATE;
}

unsigned int SoundPi::GetBitDepth()
{
   logger_->Write("Sound", LogNotice, "GetBitDepth");
   return SOUND_BITS;
}

unsigned int SoundPi::GetNbChannels()
{
   logger_->Write("Sound", LogNotice, "GetNbChannels");
   return SOUND_CHANNELS;
}

void SoundPi::CheckBuffersStatus()
{
}

IWaveHDR* SoundPi::GetFreeBuffer()
{
   for (int i = 0; i < QUEUE_SIZE; i++)
   {
      if (data_[i].status_ == IWaveHDR::UNUSED)
      {
         data_[i].status_ = IWaveHDR::USED;
         return &data_[i];
      }
   }
   return nullptr;
}

void SoundPi::AddBufferToPlay(IWaveHDR* wav)
{
   unsigned frame_available = sound_device_->GetQueueFramesAvail();
   short* buffer_short = (short*)(wav->data_);

   int nResult = sound_device_->Write(wav->data_, wav->buffer_length_);
   if (nResult != wav->buffer_length_ && frame_available != nResult)
   {
      //logger_->Write("Sound", LogNotice, "AddBufferToPlay : Available : %i, size %i, written : %i", frame_available, wav->buffer_length_, nResult);
   }
   else
   {
      if (!started_)
      {
         started_ = true;
         logger_->Write("Sound", LogNotice, "Start");
         if (!sound_device_->Start())
         {
            logger_->Write("Sound", LogPanic, "Cannot start sound device");
         }
      }
   }
   if (sound_device_->IsActive() == false)
   {
      logger_->Write("Sound", LogNotice, "AddBufferToPlay : Available : %i, size %i, written : %i", frame_available, wav->buffer_length_, nResult);
      logger_->Write("Sound", LogPanic, "Sound no longer active ?!");;
   }
      
   wav->status_ = IWaveHDR::UNUSED;

}

unsigned SoundPi::GetChunk(s16 *pBuffer, unsigned nChunkSize)
{
   logger_->Write("Sound", LogNotice, "GetChunk");
   memcpy(pBuffer, &chunk_buffer[index_output_], nChunkSize);
   index_output_ += nChunkSize;
}

