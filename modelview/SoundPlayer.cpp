#include "SoundPlayer.hpp"
#include <stdexcept>

SoundPlayer::SoundPlayer()
{
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 0, 1024) < 0)
		throw std::runtime_error(Mix_GetError());
}

SoundPlayer::~SoundPlayer()
{
	stop();
	Mix_CloseAudio();
}

void SoundPlayer::play(const char* filename)
{
	stop();

	file = FS().open(filename);

	music = Music(Mix_LoadMUS_RW(file.get()), Mix_FreeMusic);
	Mix_PlayMusic(music.get(), 1);
}

void SoundPlayer::stop()
{
	Mix_HaltMusic();
}
