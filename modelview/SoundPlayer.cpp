#include "SoundPlayer.hpp"

SoundPlayer::SoundPlayer()
{
}

SoundPlayer::~SoundPlayer()
{
	stop();
}

void SoundPlayer::play(const char* file)
{
	stop();

//	if (is_mp3)
//	{
//		Mix_Music* music = Mix_LoadMUS_RW(SDL_RWFromConstMem(mem, size));
//		Mix_PlayMusic(music, 1);
//		mp3 = boost::shared_ptr<Mix_Music>(music, Mix_FreeMusic);
//	}
//	else if (is_wav)
//	{
//		Mix_Chunk* chunk = Mix_LoadWAV_RW(SDL_RWFromConstMem(mem, size));
//		Mix_PlayChannelTimed(-1, chunk, 1, -1);
//		wav = boost::shared_ptr<Mix_Chunk>(chunk, Mix_FreeChunk);
//	}
//	else
	{
		assert(!"file must be either mp3 or wav!");
	}
}

void SoundPlayer::stop()
{
//	Mix_HaltChannel(-1);
//	Mix_HaltMusic();
}
