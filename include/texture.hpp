#ifndef WOW_TEXTURE_HPP
#define WOW_TEXTURE_HPP

#include <GL/gl.h>
#include "resource.hpp"

namespace wow
{

class texture_instance
{
public:
	explicit texture_instance(std::string const& filename);
	~texture_instance();

	void bind() const;

private:
	texture_instance();
	texture_instance(texture_instance const&);
	void operator=(texture_instance const&);

private:
	GLuint name;

	// for debugging
	std::string path;
};

typedef resource<texture_instance>::type texture;

} // namespace wow

#endif /* WOW_TEXTURE_HPP */
