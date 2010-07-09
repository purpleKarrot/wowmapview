#ifndef ANIMATED_H
#define ANIMATED_H

#include <cassert>
#include <utility>
#include <vector>

#include "modelheaders.h"
#include "mpq.hpp"
#include "vec3d.h"
#include "quaternion.h"

// interpolation functions
template<class T>
inline T interpolate(const float r, //
	const T &v1, const T &v2)
{
	return v1 * (1.f - r) + v2 * r;
}

template<>
inline Quaternion interpolate<Quaternion> (const float r, //
	const Quaternion &v1, const Quaternion &v2)
{
	return Quaternion::slerp(r, v1, v2);
}

template<class T>
inline T interpolateHermite(const float r, //
	const T &v1, const T &v2, const T &in, const T &out)
{
	float h1 = 2.f * r * r * r - 3.f * r * r + 1.f;
	float h2 = -2.f * r * r * r + 3.f * r * r;
	float h3 = r * r * r - 2.f * r * r + r;
	float h4 = r * r * r - r * r;

	return v1 * h1 + v2 * h2 + in * h3 + out * h4;
}

template<class T>
inline T interpolateBezier(const float r, //
	const T &v1, const T &v2, const T &in, const T &out)
{
	float h1 = (1.f - r) * (1.f - r) * (1.f - r);
	float h2 = 3.f * r * (1.f - r) * (1.f - r);
	float h3 = 3.f * r * r * (1.f - r);
	float h4 = r * r * r;

	return v1 * h1 + v2 * h2 + in * h3 + out * h4;
}

typedef std::pair<std::size_t, std::size_t> AnimRange;

// global time for global sequences
extern int globalTime;

enum Interpolations
{
	INTERPOLATION_NONE,
	INTERPOLATION_LINEAR,
	INTERPOLATION_HERMITE,
	INTERPOLATION_BEZIER
};

template<class T>
struct Identity
{
	static const T& conv(const T& t)
	{
		return t;
	}
};

// In WoW 2.0+ Blizzard are now storing rotation data in 16bit values instead of 32bit.
// I don't really understand why as its only a very minor saving in model sizes and adds extra overhead in
// processing the models.  Need this structure to read the data into.
struct PACK_QUATERNION
{
	int16_t x, y, z, w;
};

struct Quat16ToQuat32
{
	static const Quaternion conv(const PACK_QUATERNION t)
	{
		return Quaternion( //
			float(t.x < 0 ? t.x + 32768 : t.x - 32767) / 32767.f, //
			float(t.y < 0 ? t.y + 32768 : t.y - 32767) / 32767.f, //
			float(t.z < 0 ? t.z + 32768 : t.z - 32767) / 32767.f, //
			float(t.w < 0 ? t.w + 32768 : t.w - 32767) / 32767.f);
	}
};

// Convert opacity values stored as shorts to floating point
// I wonder why Blizzard decided to save 2 bytes by doing this
struct ShortToFloat
{
	static const float conv(const short t)
	{
		return t / 32767.f;
	}
};

#define	MAX_ANIMATED	500

/*
 Generic animated value class:

 T is the data type to animate
 D is the data type stored in the file (by default this is the same as T)
 Conv is a conversion object that defines T conv(D) to convert from D to T
 (by default this is an identity function)
 (there might be a nicer way to do this? meh meh)
 */
template<class T, class D = T, class Conv = Identity<T> >
class Animated
{
public:
	int type;
	int seq;
	uint32_t* globals;

	std::vector<std::size_t> times[MAX_ANIMATED];
	std::vector<T> data[MAX_ANIMATED];

	// for nonlinear interpolations:
	std::vector<T> in[MAX_ANIMATED];
	std::vector<T> out[MAX_ANIMATED];

	// for fix function
	std::size_t sizes;

	bool uses(std::size_t anim)
	{
		if (seq > -1)
			anim = 0;

		return !data[anim].empty();
	}

	T getValue(std::size_t anim, std::size_t time)
	{
		// obtain a time value and a data range
		if (seq > -1)
		{
			// TODO
			if (globals[seq] == 0)
				time = 0;
			else
				time = globalTime % globals[seq];
			anim = 0;
		}

		if (data[anim].size() > 1 && times[anim].size() > 1)
		{
			size_t pos = 0;
			int max_time = times[anim][times[anim].size() - 1];

			if (max_time > 0)
				time %= max_time; // I think this might not be necessary?

			for (size_t i = 0; i < times[anim].size() - 1; i++)
			{
				if (time >= times[anim][i] && time < times[anim][i + 1])
				{
					pos = i;
					break;
				}
			}

			std::size_t t1 = times[anim][pos];
			std::size_t t2 = times[anim][pos + 1];
			float r = (time - t1) / (float) (t2 - t1);

			if (type == INTERPOLATION_NONE)
				return data[anim][pos];

			if (type == INTERPOLATION_LINEAR)
				return interpolate(r, data[anim][pos], data[anim][pos + 1]);

			// INTERPOLATION_HERMITE is only used in cameras afaik?
			if (type == INTERPOLATION_HERMITE)
			{
				return interpolateHermite(r, data[anim][pos], //
					data[anim][pos + 1], in[anim][pos], out[anim][pos]);
			}

			//Is this used ingame or only by custom models?
			if (type == INTERPOLATION_BEZIER)
			{
				return interpolateBezier(r, data[anim][pos], //
					data[anim][pos + 1], in[anim][pos], out[anim][pos]);
			}

			//this shouldn't appear!
			return data[anim][pos];
		}

		// default value
		if (data[anim].empty())
			return T();

		return data[anim][0];
	}

	void init(AnimationBlock& b, MPQFile& f, uint32_t* gs)
	{
		globals = gs;
		type = b.type;
		seq = b.seq;
		if (seq != -1)
			assert(gs);

		// times
		assert(b.nTimes == b.nKeys);

		sizes = b.nTimes;
		if (b.nTimes == 0)
			return;

		for (size_t j = 0; j < b.nTimes; j++)
		{
			AnimationBlockHeader* pHeadTimes =
				(AnimationBlockHeader*) (f.getBuffer() + b.ofsTimes + j
					* sizeof(AnimationBlockHeader));

			unsigned int *ptimes = (unsigned int*) (f.getBuffer()
				+ pHeadTimes->ofsEntrys);

			for (size_t i = 0; i < pHeadTimes->nEntrys; i++)
				times[j].push_back(ptimes[i]);
		}

		// keyframes
		for (size_t j = 0; j < b.nKeys; j++)
		{
			AnimationBlockHeader* pHeadKeys =
				(AnimationBlockHeader*) (f.getBuffer() + b.ofsKeys + j
					* sizeof(AnimationBlockHeader));

			D *keys = (D*) (f.getBuffer() + pHeadKeys->ofsEntrys);
			switch (type)
			{
			case INTERPOLATION_NONE:
			case INTERPOLATION_LINEAR:
				for (size_t i = 0; i < pHeadKeys->nEntrys; i++)
					data[j].push_back(Conv::conv(keys[i]));
				break;
			case INTERPOLATION_HERMITE:
			case INTERPOLATION_BEZIER:
				for (size_t i = 0; i < pHeadKeys->nEntrys; i++)
				{
					data[j].push_back(Conv::conv(keys[i * 3]));
					in[j].push_back(Conv::conv(keys[i * 3 + 1]));
					out[j].push_back(Conv::conv(keys[i * 3 + 2]));
				}
				break;
			}
		}
	}

	void init(AnimationBlock& b, MPQFile& f, uint32_t* gs, MPQFile* animfiles)
	{
		globals = gs;
		type = b.type;
		seq = b.seq;
		if (seq != -1)
		{
			assert(gs);
		}

		// times
		assert(b.nTimes == b.nKeys);
		sizes = b.nTimes;
		if (b.nTimes == 0)
			return;

		for (size_t j = 0; j < b.nTimes; j++)
		{
			AnimationBlockHeader* pHeadTimes =
				(AnimationBlockHeader*) (f.getBuffer() + b.ofsTimes + j
					* sizeof(AnimationBlockHeader));

			uint32_t *ptimes;
			if (animfiles[j].getSize() > pHeadTimes->ofsEntrys)
			{
				ptimes = (uint32_t*) (animfiles[j].getBuffer()
					+ pHeadTimes->ofsEntrys);
			}
			else if (f.getSize() > pHeadTimes->ofsEntrys)
			{
				ptimes = (uint32_t*) (f.getBuffer() + pHeadTimes->ofsEntrys);
			}
			else
			{
				continue;
			}

			for (size_t i = 0; i < pHeadTimes->nEntrys; i++)
				times[j].push_back(ptimes[i]);
		}

		// keyframes
		for (size_t j = 0; j < b.nKeys; j++)
		{
			AnimationBlockHeader* pHeadKeys =
				(AnimationBlockHeader*) (f.getBuffer() + b.ofsKeys + j
					* sizeof(AnimationBlockHeader));

			assert((D*)(f.getBuffer() + pHeadKeys->ofsEntrys));

			D *keys;
			if (animfiles[j].getSize() > pHeadKeys->ofsEntrys)
			{
				keys = (D*) (animfiles[j].getBuffer() + pHeadKeys->ofsEntrys);
			}
			else if (f.getSize() > pHeadKeys->ofsEntrys)
			{
				keys = (D*) (f.getBuffer() + pHeadKeys->ofsEntrys);
			}
			else
			{
				continue;
			}

			switch (type)
			{
			case INTERPOLATION_NONE:
			case INTERPOLATION_LINEAR:
				for (size_t i = 0; i < pHeadKeys->nEntrys; i++)
					data[j].push_back(Conv::conv(keys[i]));
				break;
			case INTERPOLATION_HERMITE:
			case INTERPOLATION_BEZIER:
				for (size_t i = 0; i < pHeadKeys->nEntrys; i++)
				{
					data[j].push_back(Conv::conv(keys[i * 3]));
					in[j].push_back(Conv::conv(keys[i * 3 + 1]));
					out[j].push_back(Conv::conv(keys[i * 3 + 2]));
				}
				break;
			}
		}
	}

	void fix(T fixfunc(const T))
	{
		switch (type)
		{
		case INTERPOLATION_NONE:
		case INTERPOLATION_LINEAR:
			for (std::size_t i = 0; i < sizes; ++i)
			{
				for (std::size_t j = 0; j < data[i].size(); ++j)
				{
					data[i][j] = fixfunc(data[i][j]);
				}
			}
			break;
		case INTERPOLATION_HERMITE:
		case INTERPOLATION_BEZIER:
			for (std::size_t i = 0; i < sizes; ++i)
			{
				for (std::size_t j = 0; j < data[i].size(); ++j)
				{
					data[i][j] = fixfunc(data[i][j]);
					in[i][j] = fixfunc(in[i][j]);
					out[i][j] = fixfunc(out[i][j]);
				}
			}
			break;
		}
	}
};

typedef Animated<float, short, ShortToFloat> AnimatedShort;

#endif
