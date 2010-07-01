#ifndef SOUNDPLAYER_HPP
#define SOUNDPLAYER_HPP

#include <SDL/SDL_mixer.h>
#include <boost/shared_ptr.hpp>
#include "mpq.hpp"

class SoundPlayer
{
public:
	SoundPlayer();
	~SoundPlayer();

	void play(const char* filename);
	void stop();

private:
	Filesystem::File file;

	typedef boost::shared_ptr<Mix_Music> Music;
	Music music;
};

#endif /* SOUNDPLAYER_HPP */
