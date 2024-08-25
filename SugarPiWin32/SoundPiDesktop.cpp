#include "SoundPiDesktop.h"
#include <stdio.h>

SoundPi::SoundPi(CLogger* log) : buffer_in_queue_(0), mixer_(nullptr), reinit_order_(false), log_(log)
{
   m_bMute = false;
   for (int i = 0; i < NBBUFFERS; i++)
   {
      m_WaveHDR[i].status_ = IWaveHDR::UNUSED;
      m_WaveHDR[i].data_ = m_WaveHDR[i].WaveHDR.lpData = new char[BUFFERSIZE];
      m_WaveHDR[i].buffer_length_ = m_WaveHDR[i].WaveHDR.dwBufferLength = BUFFERSIZE;
      m_WaveHDR[i].WaveHDR.dwBytesRecorded = 0;
      m_WaveHDR[i].WaveHDR.dwUser = 0;
      m_WaveHDR[i].WaveHDR.dwFlags = 0;
      m_WaveHDR[i].WaveHDR.dwLoops = 0;

   }
   m_hWaveOut = NULL;

   Init(44100, 16, 2);
}


SoundPi::~SoundPi()
{
   FreeWav();

   for (int i = 0; i < NBBUFFERS; i++)
   {
      delete[]m_WaveHDR[i].WaveHDR.lpData;
   }
}

void SoundPi::FreeWav()
{
   if (m_hWaveOut != NULL)
   {
      MMRESULT result = waveOutReset(m_hWaveOut);
      if (result != MMSYSERR_NOERROR)
      {
         char fault[256];
         waveOutGetErrorTextA(result, fault, 256);
         MessageBoxA(NULL, fault, "waveOutReset : Error", MB_OK | MB_ICONEXCLAMATION);
      }

      // Quit properly the sound player
      IWaveHDR* pWave = NULL;
      for (int i = 0; i < NBBUFFERS; i++)
      {
         pWave = &m_WaveHDR[i];
         result = waveOutUnprepareHeader(m_hWaveOut, &m_WaveHDR[i].WaveHDR, sizeof(WAVEHDR));
         if (result != MMSYSERR_NOERROR)
         {
            char fault[256];
            waveOutGetErrorTextA(result, fault, 256);
            MessageBoxA(NULL, fault, "waveOutUnprepareHeader : Error", MB_OK | MB_ICONEXCLAMATION);
         }
      }

      result = waveOutClose(m_hWaveOut);
      if (result != MMSYSERR_NOERROR)
      {
         char fault[256];
         waveOutGetErrorTextA(result, fault, 256);
         MessageBoxA(NULL, fault, "waveOutClose : Error", MB_OK | MB_ICONEXCLAMATION);
      }

   }

}

void SoundPi::SetDefaultConfiguration()
{
}

void SoundPi::SaveConfiguration(const char* config_name, const char* ini_file)
{
#define MAX_SIZE_BUFFER 256
   TCHAR tmp_buffer[MAX_SIZE_BUFFER];

   sprintf_s(tmp_buffer, MAX_SIZE_BUFFER, "%i", m_SampleRate);
   WritePrivateProfileString(config_name, "Sound_SampleRate", tmp_buffer, ini_file);

   sprintf_s(tmp_buffer, MAX_SIZE_BUFFER, "%i", m_SampleBits);
   WritePrivateProfileString(config_name, "Sound_SampleBits", tmp_buffer, ini_file);

   sprintf_s(tmp_buffer, MAX_SIZE_BUFFER, "%i", m_NbChannels);
   WritePrivateProfileString(config_name, "Sound_NbChannels", tmp_buffer, ini_file);
}

bool SoundPi::LoadConfiguration(const char* config_name, const char* ini_file)
{
   TCHAR tmp_buffer[MAX_SIZE_BUFFER];
   GetPrivateProfileString(config_name, "Sound_SampleRate", "44100", tmp_buffer, MAX_SIZE_BUFFER, ini_file);
   sscanf_s(tmp_buffer, "%i", &m_SampleRate);

   GetPrivateProfileString(config_name, "Sound_SampleBits", "16", tmp_buffer, MAX_SIZE_BUFFER, ini_file);
   sscanf_s(tmp_buffer, "%i", &m_SampleBits);

   GetPrivateProfileString(config_name, "Sound_NbChannels", "2", tmp_buffer, MAX_SIZE_BUFFER, ini_file);
   sscanf_s(tmp_buffer, "%i", &m_NbChannels);

   reinit_order_ = true;
   return true;// Init();
}

bool SoundPi::Init(int sampleRate, int samplebits, int nbChannels)
{
   m_SampleRate = sampleRate;
   m_SampleBits = samplebits;
   m_NbChannels = nbChannels;
   return Init();
}

void SoundPi::Reinit()
{
   Init();
}
bool SoundPi::Init()
{
   MMRESULT result;

   //if (mixer_) mixer_->StopMixer();

   WAVEFORMATEX pFormat;
   pFormat.wFormatTag = WAVE_FORMAT_PCM;
   pFormat.nChannels = m_NbChannels;
   pFormat.nSamplesPerSec = m_SampleRate;
   pFormat.nAvgBytesPerSec = (m_SampleBits / 8) * pFormat.nChannels * m_SampleRate;
   pFormat.nBlockAlign = (m_SampleBits / 8) * pFormat.nChannels;
   pFormat.wBitsPerSample = m_SampleBits;
   pFormat.cbSize = 0;

   FreeWav();

   result = waveOutOpen(&m_hWaveOut, WAVE_MAPPER, &pFormat, (DWORD_PTR)SoundPi::CallbackSound, (DWORD_PTR)this, CALLBACK_FUNCTION | WAVE_FORMAT_DIRECT);
   if (result != MMSYSERR_NOERROR)
   {

      char fault[256];
      waveOutGetErrorTextA(result, fault, 256);
      MessageBoxA(NULL, fault, "waveOutOpen : ERROR .", MB_OK | MB_ICONEXCLAMATION);
      return false;
   }

   for (int i = 0; i < NBBUFFERS; i++)
   {
      IWaveHDR* pWave = NULL;
      pWave = &m_WaveHDR[i];
      m_WaveHDR[i].WaveHDR.dwFlags = 0;
      m_WaveHDR[i].WaveHDR.dwLoops = 0;
      result = waveOutPrepareHeader(m_hWaveOut, &m_WaveHDR[i].WaveHDR, sizeof(WAVEHDR));
      if (result != MMSYSERR_NOERROR)
      {
         char fault[256];
         waveOutGetErrorTextA(result, fault, 256);
         MessageBoxA(NULL, fault, "waveOutPrepareHeader : ERROR .", MB_OK | MB_ICONEXCLAMATION);
      }
   }
   //if (mixer_) mixer_->StartMixer();
   return true;
}
void CALLBACK SoundPi::CallbackSound(HWAVEOUT m_hWO, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
   /*if (uMsg == MM_WOM_DONE)
   {
      SoundPi* player = (SoundPi*)dwInstance;
      player->buffer_in_queue_--;

   }*/
}

unsigned int SoundPi::GetSampleRate()
{
   return m_SampleRate;
}

unsigned int SoundPi::GetBitDepth()
{
   return m_SampleBits;
}


void SoundPi::CheckBuffersStatus()
{
   // Verify the status of the current buffers
   for (int i = 0; i < NBBUFFERS; i++)
   {
      if ((m_WaveHDR[i].status_ == IWaveHDR::INQUEUE)
         && ((m_WaveHDR[i].WaveHDR.dwFlags & WHDR_DONE) == WHDR_DONE)
         )
      {

         m_WaveHDR[i].status_ = IWaveHDR::UNUSED;
         m_WaveHDR[i].WaveHDR.dwFlags = 0;
         m_WaveHDR[i].WaveHDR.dwLoops = 0;
         HRESULT result = waveOutPrepareHeader(m_hWaveOut, &m_WaveHDR[i].WaveHDR, sizeof(WAVEHDR));

         buffer_in_queue_--;
      }
   }
}

IWaveHDR* SoundPi::GetFreeBuffer()
{
   IWaveHDR* pWave = NULL;

   CheckBuffersStatus();
   for (int i = 0; i < NBBUFFERS; i++)
   {
      if (m_WaveHDR[i].status_ == IWaveHDR::UNUSED)
      {
         pWave = &m_WaveHDR[i];
         pWave->status_ = IWaveHDR::USED;
         break;
      }
   }

   if (pWave == NULL)
   {
      return NULL;
   }

   return pWave;
}

void SoundPi::SyncWithSound()
{
   // Wait until onyl one buffer is left
   while (buffer_in_queue_ > 3)
   {
      Sleep(1);
   }
}

void SoundPi::AddBufferToPlay(IWaveHDR* pWave)
{
   if (!m_bMute)
   {
      WaveHDRImp* wave = reinterpret_cast<WaveHDRImp*> (pWave);
      MMRESULT result = waveOutWrite(m_hWaveOut, &wave->WaveHDR, sizeof(wave->WaveHDR));
      if (result != MMSYSERR_NOERROR)
      {
         char fault[256];
         waveOutGetErrorTextA(result, fault, 256);
         MessageBoxA(NULL, fault, "Failed to write.", MB_OK | MB_ICONEXCLAMATION);
         return;
      }

      buffer_in_queue_++;
      pWave->status_ = IWaveHDR::INQUEUE;
   }
   else
   {
      pWave->status_ = IWaveHDR::UNUSED;
   }
}

WORD SoundPi::GetVolume()
{
   DWORD vol;
   waveOutGetVolume(m_hWaveOut, &vol);

   return (WORD)vol;
}

void SoundPi::SetVolume(WORD volume)
{
   if (m_hWaveOut)
   {
      waveOutSetVolume(m_hWaveOut, ((volume << 16) | volume));
   }
}
