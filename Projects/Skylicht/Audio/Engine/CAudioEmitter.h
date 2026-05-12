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

#ifndef _SKYLICHTAUDIO_AUDIOEMITTER_
#define _SKYLICHTAUDIO_AUDIOEMITTER_

#include "Driver/ISoundSource.h"
#include "Driver/ISoundDriver.h"

#include "Decoder/IAudioDecoder.h"
#include "Thread/IMutex.h"

#define SKYLICHTAUDIO_MIN_PITCH	0.25f
#define SKYLICHTAUDIO_MAX_PITCH	4.0f

#define SKYLICHTAUDIO_MIN_GAIN	0.0f
#define SKYLICHTAUDIO_MAX_GAIN	4.0f

using namespace Skylicht::System;

namespace Skylicht
{
	namespace Audio
	{
		/**
		 * @brief Audio emitter for playing and managing sound sources.
		 * @ingroup Audio
		 * @code
		 * #include "SkylichtAudio.h"
		 * 
		 * // Initialize audio engine
		 * Audio::initSkylichtAudio();
		 * Audio::CAudioEngine* engine = Audio::CAudioEngine::getSoundEngine();
		 * 
		 * // Create and play sound
		 * Audio::CAudioEmitter* sound = engine->createAudioEmitter("SampleAudio/helicopter_loop.wav", true);
		 * if (sound) {
		 *     sound->setLoop(true);
		 *     sound->setGain(0.8f);
		 *     sound->setPosition(0.0f, 0.0f, 0.0f);
		 *     sound->play();
		 * }
		 * 
		 * // Stop with fade out
		 * sound->stopWithFade(1000.0f); // 1 second fade out
		 * @endcode
		 */
		class CAudioEmitter
		{
		protected:
			std::string m_name;
			std::string m_fileName;
			bool m_cache;

			IAudioDecoder* m_decoder;

			IMutex* m_mutex;
			IStream* m_stream;
			ISoundSource* m_source;
			ISoundDriver* m_driver;

			ISoundSource::ESourceState m_state;
			IAudioDecoder::EDecoderType m_decodeType;

			unsigned char** m_buffer;
			unsigned char* m_decodeBuffer;

			float m_bufferLengthTime;
			int m_currentBuffer;
			int m_numBuffer;
			int m_bufferSize;
			int m_allocBufferSize;

			bool m_init;
			int m_initState;

			float m_gain;
			bool m_loop;
			float m_pitch;
			bool m_forcePlay;

			bool m_is3DSound;

			SVector3 m_position;
			float m_rollOff;

			float m_fadeGain;
			float m_fadeout;
			float m_stopWithFade;
			float m_playWithFade;

			float m_currentTime;
		public:

			static IAudioDecoder::EDecoderType getDecode(const char* fileName);

		protected:

			EStatus initEmitter();

			void updateSourceBuffer();

		public:
			CAudioEmitter(IStream* stream, IAudioDecoder::EDecoderType type, ISoundDriver* driver);

			CAudioEmitter(const char* file, bool cache, ISoundDriver* driver);

			virtual ~CAudioEmitter();

			/**
			 * @brief Set name for this emitter
			 * @param name The name string
			 */
			void setName(const char* name)
			{
				m_name = name;
			}

			/**
			 * @brief Get name of this emitter
			 * @return The name string
			 */
			const char* getName()
			{
				return m_name.c_str();
			}

			/**
			 * @brief Get decoder type
			 * @return The EDecoderType
			 */
			IAudioDecoder::EDecoderType getDecoderType()
			{
				return m_decodeType;
			}

			/**
			 * @brief Get underlying stream
			 * @return Pointer to IStream
			 */
			IStream* getStream()
			{
				return m_stream;
			}

			/**
			 * @brief Get current state of the sound source
			 * @return The ESourceState (Playing, Stopped, Paused, etc.)
			 */
			ISoundSource::ESourceState getState()
			{
				return m_state;
			}

			/**
			 * @brief Initialize default values for the emitter
			 */
			void initDefaultValue();

			/**
			 * @brief Initialize the emitter (streaming and decoding)
			 * @return True if initialization is successful or in progress
			 */
			bool init();

			/**
			 * @brief Update the emitter state, handles decoding and streaming buffers.
			 * This is called automatically by CAudioEngine.
			 */
			virtual void update();

			/**
			 * @brief Start or resume playback
			 * @param fromBegin If true, seek to the beginning before playing
			 */
			virtual void play(bool fromBegin = true);

			/**
			 * @brief Stop playback
			 */
			virtual void stop();

			/**
			 * @brief Pause playback
			 */
			virtual void pause();

			/**
			 * @brief Reset emitter state to Stopped
			 */
			virtual void reset();

			/**
			 * @brief Stop streaming and decoding
			 */
			virtual void stopStream();

			/**
			 * @brief Seek to a specific time in the audio
			 * @param time Time in milliseconds
			 */
			virtual void seek(float time);

			/**
			 * @brief Stop playback with a fade-out effect
			 * @param time Duration of the fade-out in milliseconds
			 */
			void stopWithFade(float time);

			/**
			 * @brief Start playback with a fade-in effect
			 * @param gain Target gain after fade-in
			 * @param time Duration of the fade-in in milliseconds
			 */
			void playWithFade(float gain, float time);

			/**
			 * @brief Enable or disable looping
			 * @param loop True to enable looping
			 */
			virtual void setLoop(bool loop);

			/**
			 * @brief Check if looping is enabled
			 * @return True if looping is enabled
			 */
			virtual bool isLoop()
			{
				return m_loop;
			}

			/**
			 * @brief Set playback pitch
			 * @param p Pitch value (0.25f to 4.0f)
			 */
			virtual void setPitch(float p);

			/**
			 * @brief Get current playback pitch
			 * @return Pitch value
			 */
			virtual float getPitch()
			{
				return m_pitch;
			}

			/**
			 * @brief Set playback volume (gain)
			 * @param g Gain value (0.0f to 4.0f)
			 */
			virtual void setGain(float g);

			/**
			 * @brief Get current playback volume (gain)
			 * @return Gain value
			 */
			virtual float getGain()
			{
				return m_gain;
			}

			/**
			 * @brief Check if the sound is currently playing
			 * @return True if playing
			 */
			bool isPlaying();

			/**
			 * @brief Set 3D position of the sound source
			 * @param x X coordinate
			 * @param y Y coordinate
			 * @param z Z coordinate
			 */
			void setPosition(float x, float y, float z);

			/**
			 * @brief Set roll-off factor for 3D distance attenuation
			 * @param rollOff The distance at which the sound starts to attenuate
			 */
			void setRollOff(float rollOff);

			/**
			 * @brief Get current playback time in milliseconds
			 * @return Time in milliseconds
			 */
			float getCurrentTime()
			{
				return m_currentTime;
			}

			/**
			 * @brief Get underlying audio decoder
			 * @return Pointer to IAudioDecoder
			 */
			inline IAudioDecoder* getDecoder()
			{
				return m_decoder;
			}
		};
	}
}

#endif
