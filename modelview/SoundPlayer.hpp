#ifndef SOUNDPLAYER_HPP
#define SOUNDPLAYER_HPP

#include <SDL/SDL_mixer.h>
#include <boost/shared_ptr.hpp>

class SoundPlayer
{
public:
	SoundPlayer();
	~SoundPlayer();

	void play(const char* file);
	void stop();

private:
	boost::shared_ptr<Mix_Chunk> wav;
	boost::shared_ptr<Mix_Music> mp3;
};

#endif /* SOUNDPLAYER_HPP */
