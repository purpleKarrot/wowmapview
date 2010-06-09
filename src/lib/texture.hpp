#ifndef WOW_TEXTURE_HPP
#define WOW_TEXTURE_HPP

#include <GL/gl.h>
#include <boost/flyweight.hpp>
#include <boost/flyweight/key_value.hpp>

namespace wow
{

class TextureI
{
public:
	explicit TextureI(std::string const& filename);
	~TextureI();

	void bind() const;

private:
	TextureI();
	TextureI(TextureI const&);
	void operator=(TextureI const&);

private:
	GLuint name;

	// for debugging
	std::string path;
};

typedef boost::flyweights::flyweight< //
	boost::flyweights::key_value<std::string, TextureI> > Texture;

} // namespace wow

#endif /* WOW_TEXTURE_HPP */
