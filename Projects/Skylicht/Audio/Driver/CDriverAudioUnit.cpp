/*
!@
MIT License

Copyright (c) 2012 - 2019 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#include "stdafx.h"
#include "CDriverAudioUnit.h"
#include "SkylichtAudio.h"

#ifdef USE_AUDIO_UNIT
namespace Skylicht
{
	namespace Audio
	{
		bool s_pauseEngine = false;
		int s_sampleRate = 0;
		float s_pauseTime = 0.0f;
		
		CDriverAudioUnit::CDriverAudioUnit()
		{
			init();
		}
		
		CDriverAudioUnit::~CDriverAudioUnit()
		{
			shutdown();
		}
		
		void CDriverAudioUnit::init()
		{
			printLog("[SkylichtAudio] Init CoreAudio RemoteIO");
			
			OSStatus status = noErr;
			
			// Describe audio component
			AudioComponentDescription desc;
			desc.componentType = kAudioUnitType_Output;
			
	#if defined(IOS)
			desc.componentSubType = kAudioUnitSubType_RemoteIO;
	#else
			// Use the DefaultOutputUnit for output when no device is specified
			// so we retain automatic output device switching when the default
			// changes.  Once we have complete support for device notifications
			// and switching, we can use the AUHAL for everything.
			desc.componentSubType = kAudioUnitSubType_DefaultOutput;
			// desc.componentSubType = kAudioUnitSubType_HALOutput;
	#endif
			
			desc.componentFlags = 0;
			desc.componentFlagsMask = 0;
			desc.componentManufacturer = kAudioUnitManufacturer_Apple;
			
			// Get component
			AudioComponent inputComponent = AudioComponentFindNext(NULL, &desc);
			if (inputComponent == nullptr)
				printLog("CDriverAudioUnit::init null component");
			
			// Init audio component
			status = AudioComponentInstanceNew(inputComponent, &m_audioUnit);
			if (status != noErr)
				printLog("CDriverAudioUnit::init error [0]");
			
	#define AU_OUTPUT_BUS   0
	#define AU_INPUT_BUS    1
			
	#if defined(IOS)
			// Enable IO for playback
			UInt32 flag = 1;
			
			status = AudioUnitSetProperty(m_audioUnit,
										  kAudioOutputUnitProperty_EnableIO,
										  kAudioUnitScope_Output,
										  AU_OUTPUT_BUS,
										  &flag,
										  sizeof(flag));
			if (status != noErr)
				printLog("CDriverAudioUnit::init error [1]\n");
	#endif
			
			AudioStreamBasicDescription audioFormat;
			
			audioFormat.mSampleRate = m_preferedRate;
			audioFormat.mFormatID = kAudioFormatLinearPCM;
			audioFormat.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
			audioFormat.mFramesPerPacket = 1;
			audioFormat.mChannelsPerFrame = 2;    //Final mix is always stereo
			audioFormat.mBitsPerChannel = 16;   //16 bit per sample per channel
			audioFormat.mBytesPerPacket = 4;
			audioFormat.mBytesPerFrame = 4;
			
			// Apply format
			status = AudioUnitSetProperty(m_audioUnit,
										  kAudioUnitProperty_StreamFormat,
										  kAudioUnitScope_Input,
										  AU_OUTPUT_BUS,
										  &audioFormat,
										  sizeof(audioFormat));
			if (status != noErr)
				printLog("CDriverAudioUnit::init error %d [3]\n", status);
			
			
			// Callback
			AURenderCallbackStruct callbackStruct;
			callbackStruct.inputProc = playbackCallback;
			callbackStruct.inputProcRefCon = this;
			status = AudioUnitSetProperty(m_audioUnit,
										  kAudioUnitProperty_SetRenderCallback,
										  kAudioUnitScope_Global,
										  AU_OUTPUT_BUS,
										  &callbackStruct,
										  sizeof(callbackStruct));
			if (status != noErr)
				printLog("CDriverAudioUnit::init error [2]\n");
			
			
			// init buffer length
			int minBufferSize = 2048;
			int bufferSizeIO = (static_cast<int>(m_bufferLength * m_preferedRate)) * 4;
			
			// force buffer size to the next multiple of <m_minBufferSize> samples
			bufferSizeIO += minBufferSize - (bufferSizeIO % minBufferSize);
			
			if (bufferSizeIO < minBufferSize)
				bufferSizeIO = minBufferSize;
			
			// recalc length
			// todo: very important!
			m_bufferLength = (bufferSizeIO / (float)m_preferedRate) / 4.0f;
			
			printf("CDriverAudioUnit:init recommend buffer: %d %f\n", bufferSizeIO, m_bufferLength);
			// need call change buffer duration but AudioSessionSetProperty Deprecated
			
			// init Audio
			status = AudioUnitInitialize(m_audioUnit);
			if (status != noErr)
				printLog("CDriverAudioUnit::init error [4]\n");
			
			// start Audio
			status = AudioOutputUnitStart(m_audioUnit);
			if (status != noErr)
				printLog("CDriverAudioUnit::init error [5]\n");
			
			s_sampleRate = m_preferedRate;
		}
		
		void CDriverAudioUnit::shutdown()
		{
			AudioOutputUnitStop(m_audioUnit);
			AudioUnitUninitialize(m_audioUnit);
			AudioComponentInstanceDispose(m_audioUnit);
		}
		
		void CDriverAudioUnit::suspend()
		{
			s_pauseEngine = true;
		}
		
		void CDriverAudioUnit::resume()
		{
			s_pauseEngine = false;
			
			// todo try start this audio
			AudioOutputUnitStart(m_audioUnit);
		}
		
		OSStatus CDriverAudioUnit::playbackCallback(void *inRefCon, AudioUnitRenderActionFlags *ioActionFlags, const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber, UInt32 inNumberFrames, AudioBufferList *ioData)
		{
			if (inRefCon)
			{
				CDriverAudioUnit *driver = (CDriverAudioUnit*)inRefCon;
				
				// write buffer
				for (int i = 0; i < ioData->mNumberBuffers; i++)
				{
					// todo update all emitter & decode stream
					unsigned short* data = (unsigned short*)ioData->mBuffers[i].mData;
					size_t dataSize = ioData->mBuffers[i].mDataByteSize;
					
					float duration = (dataSize / (float)s_sampleRate) / 4.0f;
					driver->changeDuration(duration);
					
					// printf("playbackCallback size %d & duration %f - %d\n", (int)dataSize, (float)duration, (int)s_sampleRate);
					
	#ifndef USE_MULTITHREAD_UPDATE
					CAudioEngine::getSoundEngine()->updateEmitter();
	#endif
					
					if (s_pauseEngine == true)
					{
						s_pauseTime = s_pauseTime + duration * 1000.0f;
						if (s_pauseTime > 1000.0f)
							s_pauseTime = 1000.0f;
					}
					else
					{
						s_pauseTime = s_pauseTime - duration * 1000.0f;
						if (s_pauseTime < 0.0f)
							s_pauseTime = 0.0f;
					}
					
					float masterGain = 1.0f - s_pauseTime / 1000.0f;
					if (masterGain < 0.0f)
						masterGain = 0.0f;
					else if (masterGain > 1.0f)
						masterGain = 1.0f;
					
					driver->setMasterGain(masterGain);
					driver->fillBuffer(data, dataSize / 4);
				}
			}
			
			return 0;
		}
	}
}

#endif
