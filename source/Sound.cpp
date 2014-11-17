/*
* Copyright (c) 2006, Ondrej Danek (www.ondrej-danek.net)
* All rights reserved.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of Ondrej Danek nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
* GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
* OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "SoundException.h"
#include "Type.h"
#include "Globals.h"

#define SND_TEST(x) if (x == -1) { SOUND_Error(); return; }

namespace Duel6
{
	namespace Sound
	{
		namespace
		{
			struct sndSound_s
			{
				bool inited;
				bool playing;
				Size channels;
				std::vector<Mix_Music*> modules;
				std::vector<Mix_Chunk*> samples;
			};

			// Inicializace struktury
			sndSound_s snd = { false };
		}

		/*
		==================================================
		Chyba zvukoveho systemu
		==================================================
		*/
		static void SOUND_Error()
		{
			snd.inited = false;
			snd.playing = false;
			Mix_CloseAudio();
			D6_THROW(SoundException, Format(D6_L("SDL_Mixer error: {0}")) << Mix_GetError());
		}

		/*
		==================================================
		Inicializace zvukoveho systemu
		==================================================
		*/
		void initialize(Size channels)
		{
			if (snd.inited)
			{
				return;
			}

			d6Console.print(D6_L("\n===Initialization of sound sub-system===\n"));
			d6Console.print(D6_L("...Starting SDL_mixer library\n"));

			// Init SDL audio sub sytem
			SND_TEST(SDL_InitSubSystem(SDL_INIT_AUDIO))

			// Init SDL_mixer library
			SND_TEST(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024))

			// Allocate channels
			channels = Mix_AllocateChannels(channels);

			d6Console.print(Format(D6_L("...Frequency: {0}\n...Channels: {0}\n")) << MIX_DEFAULT_FREQUENCY << channels);

			// Set up variables
			snd.channels = channels;
			snd.playing = false;
			snd.inited = true;
		}

		/*
		==================================================
		Nahrani modulu
		==================================================
		*/
		Size loadModule(const std::string& nm)
		{
			if (snd.inited)
			{
				Mix_Music *module = Mix_LoadMUS(nm.c_str());
				if (module == nullptr)
				{
					d6Console.print(Format(D6_L("SDL_mixer error: unable to load module {0}\n")) << nm);
					return -1;
				}
				snd.modules.push_back(module);
				d6Console.print(Format(D6_L("...Module loaded: {0}\n")) << nm);
				return snd.modules.size() - 1;
			}

			return Size(-1);
		}

		/*
		==================================================
		Nahrani zvukoveho samplu
		==================================================
		*/
		Size loadSample(const std::string& nm)
		{
			if (snd.inited)
			{
				Mix_Chunk* sample = Mix_LoadWAV(nm.c_str());					
				if (sample == nullptr)
				{
					d6Console.print(Format(D6_L("SDL_mixer error: unable to load sample {0}\n")) << nm);
					return -1;
				}
				snd.samples.push_back(sample);
				d6Console.print(Format(D6_L("...Sample loaded: {0}\n")) << nm);
				return snd.samples.size() - 1;
			}

			return Size(-1);
		}

		/*
		==================================================
		Zastaveni hudby
		==================================================
		*/
		void stopMusic()
		{
			if (snd.playing && snd.inited)
			{
				Mix_HaltMusic();
				snd.playing = false;
			}
		}

		/*
		==================================================
		Spusteni vybraneho modulu (hudby)
		==================================================
		*/
		void startMusic(Size i, bool loop)
		{
			if (i < snd.modules.size() && snd.inited)
			{
				stopMusic();
				SND_TEST(Mix_PlayMusic(snd.modules[i], loop ? -1 : 0))
				snd.playing = true;
			}
		}

		/*
		==================================================
		Zahrani samplu
		==================================================
		*/
		void playSample(Size i)
		{
			if (i < snd.samples.size() && snd.inited)
			{
				for (Size j = 0; j < snd.channels; j++)
				{
					if (!Mix_Playing(j) && !Mix_Paused(j))
					{
						SND_TEST(Mix_PlayChannel(j, snd.samples[i], 0))
						return;
					}
				}
			}
		}

		/*
		==================================================
		Nastaveni hlasitosti hudby a samplu
		==================================================
		*/
		void volume(int volume)
		{
			if (snd.inited)
			{
				d6Console.print(Format(D6_L("...Volume set to {0}\n")) << volume);
				Mix_VolumeMusic(volume);
				Mix_Volume(-1, volume);
			}
		}

		/*
		==================================================
		Deinicializace zvukoveho systemu
		==================================================
		*/
		void deInit()
		{
			if (snd.inited)
			{
				// Stop and free modules
				stopMusic();

				for (Mix_Music* module : snd.modules)
				{
					Mix_FreeMusic(module);
				}

				// Free all mixing channels and samples
				Mix_AllocateChannels(0);

				for (Mix_Chunk* sample : snd.samples)
				{
					Mix_FreeChunk(sample);
				}

				snd.modules.clear();
				snd.samples.clear();

				Mix_CloseAudio();

				snd.inited = false;
			}
		}
	}
}