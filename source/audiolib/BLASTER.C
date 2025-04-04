/*
Copyright (C) 1994-1995 Apogee Software, Ltd.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
/**********************************************************************
   module: BLASTER.C

   author: James R. Dose
   date:   February 4, 1994

   Low level routines to support Sound Blaster, Sound Blaster Pro,
   Sound Blaster 16, and compatible sound cards.

   (c) Copyright 1994 James R. Dose.  All Rights Reserved.
**********************************************************************/

#include <dos.h>
#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "compat.h"
#include "sound.h"
#include "blaster.h"
#include "_blaster.h"

const int BLASTER_SampleSize[ BLASTER_MaxMixMode + 1 ] =
   {
   MONO_8BIT_SAMPLE_SIZE,  STEREO_8BIT_SAMPLE_SIZE,
   MONO_16BIT_SAMPLE_SIZE, STEREO_16BIT_SAMPLE_SIZE
   };

const CARD_CAPABILITY BLASTER_CardConfig[ BLASTER_MaxCardType + 1 ] =
   {
      { FALSE, INVALID,      INVALID, INVALID, INVALID }, // Unsupported
      {  TRUE,      NO,    MONO_8BIT,    4000,   23000 }, // SB 1.0
      {  TRUE,     YES,  STEREO_8BIT,    4000,   44100 }, // SBPro
      {  TRUE,      NO,    MONO_8BIT,    4000,   23000 }, // SB 2.xx
      {  TRUE,     YES,  STEREO_8BIT,    4000,   44100 }, // SBPro 2
      { FALSE, INVALID,      INVALID, INVALID, INVALID }, // Unsupported
      {  TRUE,     YES, STEREO_16BIT,    5000,   44100 }, // SB16
   };

CARD_CAPABILITY BLASTER_Card;

BLASTER_CONFIG BLASTER_Config =
   {
   UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED
   };

static int BLASTER_Installed = FALSE;

int BLASTER_Version;

static char   *BLASTER_DMABuffer;
static char   *BLASTER_DMABufferEnd;
static char   *BLASTER_CurrentDMABuffer;
static int     BLASTER_TotalDMABufferSize;

static int      BLASTER_TransferLength   = 0;
static int      BLASTER_MixMode          = BLASTER_DefaultMixMode;
static int      BLASTER_SamplePacketSize = MONO_16BIT_SAMPLE_SIZE;
static unsigned BLASTER_SampleRate       = BLASTER_DefaultSampleRate;
static int BLASTER_TC;

volatile int   BLASTER_SoundPlaying;
volatile int   BLASTER_SoundRecording;

void ( *BLASTER_CallBack )( void );

static int BLASTER_MixerType    = 0;

static int BLASTER_WaveBlasterState = 0x0F;

// This is defined because we can't create local variables in a
// function that switches stacks.
static int GlobalStatus;

int BLASTER_ErrorCode = BLASTER_Ok;

#define BLASTER_SetErrorCode( status ) \
   BLASTER_ErrorCode   = ( status );


/*---------------------------------------------------------------------
   Function: BLASTER_ErrorString

   Returns a pointer to the error message associated with an error
   number.  A -1 returns a pointer the current error.
---------------------------------------------------------------------*/

char *BLASTER_ErrorString
   (
   int ErrorNumber
   )

   {
   char *ErrorString;

   switch( ErrorNumber )
      {
      case BLASTER_Warning :
      case BLASTER_Error :
         ErrorString = BLASTER_ErrorString( BLASTER_ErrorCode );
         break;

      case BLASTER_Ok :
         ErrorString = "Sound Blaster ok.";
         break;

      case BLASTER_EnvNotFound :
         ErrorString = "BLASTER environment variable not set.";
         break;

      case BLASTER_AddrNotSet :
         ErrorString = "Sound Blaster address not set.";
         break;

      case BLASTER_DMANotSet :
         ErrorString = "Sound Blaster 8-bit DMA channel not set.";
         break;

      case BLASTER_DMA16NotSet :
         ErrorString = "Sound Blaster 16-bit DMA channel not set.";
         break;

      case BLASTER_InvalidParameter :
         ErrorString = "Invalid parameter in BLASTER environment variable.";
         break;

      case BLASTER_CardNotReady :
         ErrorString = "Sound Blaster not responding on selected port.";
         break;

      case BLASTER_NoSoundPlaying :
         ErrorString = "No sound playing on Sound Blaster.";
         break;

      case BLASTER_InvalidIrq :
         ErrorString = "Invalid Sound Blaster Irq.";
         break;

      case BLASTER_UnableToSetIrq :
         ErrorString = "Unable to set Sound Blaster IRQ.  Try selecting an IRQ of 7 or below.";
         break;

#if 0
      case BLASTER_DmaError :
         ErrorString = DMA_ErrorString( DMA_Error );
         break;
#endif

      case BLASTER_NoMixer :
         ErrorString = "Mixer not available on selected Sound Blaster card.";
         break;

      case BLASTER_DPMI_Error :
         ErrorString = "DPMI Error in Blaster.";
         break;

      case BLASTER_OutOfMemory :
         ErrorString = "Out of conventional memory in Blaster.";
         break;

      default :
         ErrorString = "Unknown Sound Blaster error code.";
         break;
      }

   return( ErrorString );
   }


/*---------------------------------------------------------------------
   Function: BLASTER_ServiceInterrupt

   Handles interrupt generated by sound card at the end of a voice
   transfer.  Calls the user supplied callback function.
---------------------------------------------------------------------*/

void BLASTER_ServiceInterrupt
   (
   void
   )

   {
   // Keep track of current buffer
   BLASTER_CurrentDMABuffer += BLASTER_TransferLength;

   if ( BLASTER_CurrentDMABuffer >= BLASTER_DMABufferEnd )
      {
      BLASTER_CurrentDMABuffer = BLASTER_DMABuffer;
      }

   // Continue playback on cards without autoinit mode
   if ( BLASTER_Version < DSP_Version2xx )
      {
      if ( BLASTER_SoundPlaying )
         {
         Blaster_ContinueDma(BLASTER_TransferLength - 1);
         }

      if ( BLASTER_SoundRecording )
         {
         //BLASTER_DSP1xx_BeginRecord( BLASTER_TransferLength );
         }
      }

   // Call the caller's callback function
   if ( BLASTER_CallBack != NULL )
      {
      BLASTER_CallBack();
      }
   }

/*---------------------------------------------------------------------
   Function: BLASTER_GetDSPVersion

   Returns the version number of the sound card's DSP.
---------------------------------------------------------------------*/

int BLASTER_GetDSPVersion
   (
   void
   )

   {
   int version;

   if ( blaster_type == BLASTER_TYPE_16 )
      {
      BLASTER_Card.IsSupported     = TRUE;
      BLASTER_Card.HasMixer        = YES;
      BLASTER_Card.MaxMixMode      = STEREO_16BIT;
      BLASTER_Card.MinSamplingRate = 5000;
      BLASTER_Card.MaxSamplingRate = 44100;
      BLASTER_MixerType = SB16;
      version = DSP_Version4xx;
      }
   else if ( blaster_type == BLASTER_TYPE_PRO )
      {
      BLASTER_Card.IsSupported     = TRUE;
      BLASTER_Card.HasMixer        = YES;
      BLASTER_Card.MaxMixMode      = STEREO_8BIT;
      BLASTER_Card.MinSamplingRate = 4000;
      BLASTER_Card.MaxSamplingRate = 44100;
      BLASTER_MixerType = SBPro;
      version = DSP_Version3xx;
      }
   else if ( blaster_type == BLASTER_TYPE_2 )
      {
      BLASTER_Card.IsSupported     = TRUE;
      BLASTER_Card.HasMixer        = NO;
      BLASTER_Card.MaxMixMode      = MONO_8BIT;
      BLASTER_Card.MinSamplingRate = 4000;
      BLASTER_Card.MaxSamplingRate = 23000;
      BLASTER_MixerType = 0;
      version = DSP_Version2xx;
      }
   else
      {
      BLASTER_Card.IsSupported     = TRUE;
      BLASTER_Card.HasMixer        = NO;
      BLASTER_Card.MaxMixMode      = MONO_8BIT;
      BLASTER_Card.MinSamplingRate = 4000;
      BLASTER_Card.MaxSamplingRate = 23000;
      BLASTER_MixerType = 0;
      version = DSP_Version1xx;
      }

   return( version );
   }

/*---------------------------------------------------------------------
   Function: BLASTER_SetPlaybackRate

   Sets the rate at which the digitized sound will be played in
   hertz.
---------------------------------------------------------------------*/

void BLASTER_SetPlaybackRate
   (
   unsigned rate
   )

   {
   if ( BLASTER_Version < DSP_Version4xx )
      {
      int  timeconstant;
      int32_t ActualRate;

      // Send sampling rate as time constant for older Sound
      // Blaster compatible cards.

      ActualRate = rate * BLASTER_SamplePacketSize;
      if ( ActualRate < BLASTER_Card.MinSamplingRate )
         {
         rate = BLASTER_Card.MinSamplingRate / BLASTER_SamplePacketSize;
         }

      if ( ActualRate > BLASTER_Card.MaxSamplingRate )
         {
         rate = BLASTER_Card.MaxSamplingRate / BLASTER_SamplePacketSize;
         }

      timeconstant = ( int )CalcTimeConstant( rate, BLASTER_SamplePacketSize );

      // Keep track of what the actual rate is
      BLASTER_SampleRate  = ( unsigned )CalcSamplingRate( timeconstant );
      BLASTER_SampleRate /= BLASTER_SamplePacketSize;

      BLASTER_TC = timeconstant;
      }
   else
      {
      // Send literal sampling rate for cards with DSP version
      // 4.xx (Sound Blaster 16)

      BLASTER_SampleRate = rate;

      if ( BLASTER_SampleRate < BLASTER_Card.MinSamplingRate )
         {
         BLASTER_SampleRate = BLASTER_Card.MinSamplingRate;
         }

      if ( BLASTER_SampleRate > BLASTER_Card.MaxSamplingRate )
         {
         BLASTER_SampleRate = BLASTER_Card.MaxSamplingRate;
         }
      }
   }


/*---------------------------------------------------------------------
   Function: BLASTER_GetPlaybackRate

   Returns the rate at which the digitized sound will be played in
   hertz.
---------------------------------------------------------------------*/

unsigned BLASTER_GetPlaybackRate
   (
   void
   )

   {
   return( BLASTER_SampleRate );
   }


/*---------------------------------------------------------------------
   Function: BLASTER_SetMixMode

   Sets the sound card to play samples in mono or stereo.
---------------------------------------------------------------------*/

int BLASTER_SetMixMode
   (
   int mode
   )

   {
   int   CardType;

   CardType = BLASTER_Config.Type;

   mode &= BLASTER_MaxMixMode;

   if ( !( BLASTER_Card.MaxMixMode & STEREO ) )
      {
      mode &= ~STEREO;
      }

   if ( !( BLASTER_Card.MaxMixMode & SIXTEEN_BIT ) )
      {
      mode &= ~SIXTEEN_BIT;
      }

   BLASTER_MixMode = mode;
   BLASTER_SamplePacketSize = BLASTER_SampleSize[ mode ];

   return( mode );
   }


/*---------------------------------------------------------------------
   Function: BLASTER_StopPlayback

   Ends the DMA transfer of digitized sound to the sound card.
---------------------------------------------------------------------*/

void BLASTER_StopPlayback
   (
   void
   )

   {
   int DmaChannel;

   Blaster_StopDma();
   Blaster_Shutdown();

   BLASTER_SoundPlaying = FALSE;
   BLASTER_SoundRecording = FALSE;

   BLASTER_DMABuffer = NULL;
   }

/*---------------------------------------------------------------------
   Function: BLASTER_GetCurrentPos

   Returns the offset within the current sound being played.
---------------------------------------------------------------------*/

int BLASTER_GetCurrentPos
   (
   void
   )

   {
   int   offset = Blaster_GetDmaCount();

   if ( BLASTER_MixMode & STEREO )
      {
      offset >>= 1;
      }

   return( offset );
   }

/*---------------------------------------------------------------------
   Function: BLASTER_BeginBufferedPlayback

   Begins multibuffered playback of digitized sound on the sound card.
---------------------------------------------------------------------*/

int BLASTER_BeginBufferedPlayback
   (
   char    *BufferStart,
   int      BufferSize,
   int      NumDivisions,
   unsigned SampleRate,
   int      MixMode,
   void  ( *CallBackFunc )( void )
   )

   {
   int TransferLength;

//JIM
//   if ( BLASTER_SoundPlaying || BLASTER_SoundRecording )
      {
      BLASTER_StopPlayback();
      }

   BLASTER_SetMixMode( MixMode );

   BLASTER_DMABuffer          = BufferStart;
   BLASTER_CurrentDMABuffer   = BufferStart;
   BLASTER_TotalDMABufferSize = BufferSize;
   BLASTER_DMABufferEnd       = BufferStart + BufferSize;

   BLASTER_SetPlaybackRate( SampleRate );

   BLASTER_SetCallBack( CallBackFunc );

   Blaster_Init(BLASTER_TC, BLASTER_SampleRate, ( BLASTER_MixMode & STEREO ) ? 2 : 1, (BLASTER_MixMode & SIXTEEN_BIT) != 0 );

   TransferLength = BufferSize / NumDivisions;
   BLASTER_TransferLength = TransferLength;

   int dma_len, len;
   if ( (BLASTER_MixMode & SIXTEEN_BIT) != 0 && BLASTER_Version < DSP_Version4xx)
      {
      dma_len = (BufferSize / 2) - 1;
      len = (TransferLength / 2) - 1;
      }
   else
      {
      dma_len = BufferSize - 1;
      len = TransferLength - 1;
      }

   Blaster_StartDma(0, dma_len, len, BLASTER_Version >= DSP_Version2xx );

   return( BLASTER_Ok );
   }

/*---------------------------------------------------------------------
   Function: BLASTER_BeginBufferedRecord

   Begins multibuffered recording of digitized sound on the sound card.
---------------------------------------------------------------------*/

int BLASTER_BeginBufferedRecord
   (
   char    *BufferStart,
   int      BufferSize,
   int      NumDivisions,
   unsigned SampleRate,
   int      MixMode,
   void  ( *CallBackFunc )( void )
   )

   {

   return( BLASTER_Error );
   }


/*---------------------------------------------------------------------
   Function: BLASTER_GetVoiceVolume

   Reads the average volume of the digitized sound channel from the
   Sound Blaster's mixer chip.
---------------------------------------------------------------------*/

int BLASTER_GetVoiceVolume
   (
   void
   )

   {
   int volume;

   switch( BLASTER_MixerType )
      {
      case SBPro :
      case SBPro2 :
      case SB16 :
         volume = Blaster_GetVolume();
         break;

      default :
         BLASTER_SetErrorCode( BLASTER_NoMixer );
         volume = BLASTER_Error;
      }

   return( volume );
   }


/*---------------------------------------------------------------------
   Function: BLASTER_SetVoiceVolume

   Sets the volume of the digitized sound channel on the Sound
   Blaster's mixer chip.
---------------------------------------------------------------------*/

int BLASTER_SetVoiceVolume
   (
   int volume
   )

   {
   int status;

   volume = min( 255, volume );
   volume = max( 0, volume );

   status = BLASTER_Ok;
   switch( BLASTER_MixerType )
      {
      case SBPro :
      case SBPro2 :
      case SB16 :
         Blaster_SetVolume(volume);
         break;

      default :
         BLASTER_SetErrorCode( BLASTER_NoMixer );
         status = BLASTER_Error;
      }

   return( status );
   }


/*---------------------------------------------------------------------
   Function: BLASTER_GetMidiVolume

   Reads the average volume of the Midi sound channel from the
   Sound Blaster's mixer chip.
---------------------------------------------------------------------*/

int BLASTER_GetMidiVolume
   (
   void
   )

   {
   int volume;

   switch( BLASTER_MixerType )
      {
      case SBPro :
      case SBPro2 :
      case SB16 :
         volume = Music_GetVolume();
         break;

      default :
         BLASTER_SetErrorCode( BLASTER_NoMixer );
         volume = BLASTER_Error;
      }

   return( volume );
   }


/*---------------------------------------------------------------------
   Function: BLASTER_SetMidiVolume

   Sets the volume of the Midi sound channel on the Sound
   Blaster's mixer chip.
---------------------------------------------------------------------*/

int BLASTER_SetMidiVolume
   (
   int volume
   )

   {
   int status;

   volume = min( 255, volume );
   volume = max( 0, volume );

   status = BLASTER_Ok;
   switch( BLASTER_MixerType )
      {
      case SBPro :
      case SBPro2 :
      case SB16 :
         Music_SetVolume(volume);
         break;

      default :
         BLASTER_SetErrorCode( BLASTER_NoMixer );
         status = BLASTER_Error;
      }

   return( status );
   }

/*---------------------------------------------------------------------
   Function: BLASTER_CardHasMixer

   Checks if the selected Sound Blaster card has a mixer.
---------------------------------------------------------------------*/

int BLASTER_CardHasMixer
   (
   void
   )

   {
   return( BLASTER_Card.HasMixer );
   }


/*---------------------------------------------------------------------
   Function: BLASTER_GetEnv

   Retrieves the BLASTER environment settings and returns them to
   the caller.
---------------------------------------------------------------------*/

int BLASTER_GetEnv
   (
   BLASTER_CONFIG *Config
   )

   {
   Config->Address   = 0x220;
   Config->Type      = 6;
   Config->Interrupt = 5;
   Config->Dma8      = 1;
   Config->Dma16     = 3;
   Config->Midi      = 0x330;
   Config->Emu       = UNDEFINED;

   return( BLASTER_Ok );
   }


/*---------------------------------------------------------------------
   Function: BLASTER_SetCardSettings

   Sets up the sound card's parameters.
---------------------------------------------------------------------*/

int BLASTER_SetCardSettings
   (
   BLASTER_CONFIG Config
   )

   {
   if ( BLASTER_Installed )
      {
      BLASTER_Shutdown();
      }

   BLASTER_Config.Address   = Config.Address;
   BLASTER_Config.Type      = Config.Type;
   BLASTER_Config.Interrupt = Config.Interrupt;
   BLASTER_Config.Dma8      = Config.Dma8;
   BLASTER_Config.Dma16     = Config.Dma16;
   BLASTER_Config.Midi      = Config.Midi;
   BLASTER_Config.Emu       = Config.Emu;
   BLASTER_MixerType        = Config.Type;

   if ( BLASTER_Config.Emu == UNDEFINED )
      {
      BLASTER_Config.Emu = BLASTER_Config.Address + 0x400;
      }

   return( BLASTER_Ok );
   }


/*---------------------------------------------------------------------
   Function: BLASTER_GetCardSettings

   Sets up the sound card's parameters.
---------------------------------------------------------------------*/

int BLASTER_GetCardSettings
   (
   BLASTER_CONFIG *Config
   )

   {
   if ( BLASTER_Config.Address == UNDEFINED )
      {
      return( BLASTER_Warning );
      }
   else
      {
      Config->Address   = BLASTER_Config.Address;
      Config->Type      = BLASTER_Config.Type;
      Config->Interrupt = BLASTER_Config.Interrupt;
      Config->Dma8      = BLASTER_Config.Dma8;
      Config->Dma16     = BLASTER_Config.Dma16;
      Config->Midi      = BLASTER_Config.Midi;
      Config->Emu       = BLASTER_Config.Emu;
      }

   return( BLASTER_Ok );
   }


/*---------------------------------------------------------------------
   Function: BLASTER_GetCardInfo

   Returns the maximum number of bits that can represent a sample
   (8 or 16) and the number of channels (1 for mono, 2 for stereo).
---------------------------------------------------------------------*/

int BLASTER_GetCardInfo
   (
   int *MaxSampleBits,
   int *MaxChannels
   )

   {
   if ( BLASTER_Card.MaxMixMode & STEREO )
      {
      *MaxChannels = 2;
      }
   else
      {
      *MaxChannels = 1;
      }

   if ( BLASTER_Card.MaxMixMode & SIXTEEN_BIT )
      {
      *MaxSampleBits = 16;
      }
   else
      {
      *MaxSampleBits = 8;
      }

   return( BLASTER_Ok );
   }


/*---------------------------------------------------------------------
   Function: BLASTER_SetCallBack

   Specifies the user function to call at the end of a sound transfer.
---------------------------------------------------------------------*/

void BLASTER_SetCallBack
   (
   void ( *func )( void )
   )

   {
   BLASTER_CallBack = func;
   }

/*---------------------------------------------------------------------
   Function: BLASTER_SetupWaveBlaster

   Allows the WaveBlaster to play music while the Sound Blaster 16
   plays digital sound.
---------------------------------------------------------------------*/

void BLASTER_SetupWaveBlaster
   (
   void
   )

   {
   }


/*---------------------------------------------------------------------
   Function: BLASTER_ShutdownWaveBlaster

   Restores WaveBlaster mixer to original state.
---------------------------------------------------------------------*/

void BLASTER_ShutdownWaveBlaster
   (
   void
   )

   {
   }


/*---------------------------------------------------------------------
   Function: BLASTER_Init

   Initializes the sound card and prepares the module to play
   digitized sounds.
---------------------------------------------------------------------*/

int BLASTER_Init
   (
   void
   )

   {
   int status;

   if ( BLASTER_Installed )
      {
      BLASTER_Shutdown();
      }

   if ( BLASTER_Config.Address == UNDEFINED )
      {
      BLASTER_SetErrorCode( BLASTER_AddrNotSet );
      return( BLASTER_Error );
      }

   BLASTER_SoundPlaying = FALSE;

   BLASTER_SetCallBack( NULL );

   BLASTER_DMABuffer = NULL;

   BLASTER_Version = BLASTER_GetDSPVersion();

   BLASTER_SetPlaybackRate( BLASTER_DefaultSampleRate );
   BLASTER_SetMixMode( BLASTER_DefaultMixMode );

   // Install our interrupt handler
   Blaster_SetIrqHandler( BLASTER_ServiceInterrupt );

   BLASTER_Installed = TRUE;
   status = BLASTER_Ok;

   BLASTER_SetErrorCode( status );
   return( status );
   }


/*---------------------------------------------------------------------
   Function: BLASTER_Shutdown

   Ends transfer of sound data to the sound card and restores the
   system resources used by the card.
---------------------------------------------------------------------*/

void BLASTER_Shutdown
   (
   void
   )

   {
   int Irq;
   int Interrupt;

   // Halt the DMA transfer
   BLASTER_StopPlayback();

   Blaster_SetIrqHandler(NULL);

   BLASTER_SoundPlaying = FALSE;

   BLASTER_DMABuffer = NULL;

   BLASTER_SetCallBack( NULL );

   BLASTER_Installed = FALSE;
   }
