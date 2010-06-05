#include "texture.hpp"
#include "../mpq.h"
#include <iostream>

namespace wow
{

static void LoadBLP(GLuint id, const char* filename)
{
	glBindTexture(GL_TEXTURE_2D, id);

	int offsets[16],sizes[16],w,h, type=0;
	GLint format;

	char attr[4];

	MPQFile f(filename);
	if (f.isEof()) {
		std::cerr << "Could not load texture from " << filename << std::endl;
		return;
	}

	f.seek(4);
	f.read(&type,4);
	f.read(attr,4);
	f.read(&w,4);
	f.read(&h,4);
	f.read(offsets,4*16);
	f.read(sizes,4*16);

	bool hasmipmaps = (attr[3]>0);
	int mipmax = hasmipmaps ? 16 : 1;

	if (type != 1) {
		std::cerr << filename << " has invalid type: " << type << std::endl;
		return;
	}

	if (attr[0] == 2) {
		// compressed

		unsigned char *ucbuf = NULL;

		format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		int blocksize = 8;

		// guesswork here :(
		if (attr[1]==8 || attr[1]==4) {
			format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			blocksize = 16;
		}

		if (attr[1]==8 && attr[2]==7) {
			format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			blocksize = 16;
		}

		unsigned char *buf = new unsigned char[sizes[0]];

		// do every mipmap level
		for (int i=0; i<mipmax; i++) {
			if (w==0) w = 1;
			if (h==0) h = 1;
			if (offsets[i] && sizes[i]) {
				f.seek(offsets[i]);
				f.read(buf,sizes[i]);

				int size = ((w+3)/4) * ((h+3)/4) * blocksize;

				glCompressedTexImage2D(GL_TEXTURE_2D, i, format, w, h, 0, size, buf);

			} else break;
			w >>= 1;
			h >>= 1;
		}

		delete[] buf;
	}
	else if (attr[0]==1) {
		// uncompressed
		unsigned int pal[256];
		f.read(pal, 1024);

		unsigned char *buf = new unsigned char[sizes[0]];
		unsigned int *buf2 = new unsigned int[w*h];
		unsigned int *p = NULL;
		unsigned char *c = NULL, *a = NULL;

		int alphabits = attr[1];
		bool hasalpha = (alphabits!=0);

		for (int i=0; i<mipmax; i++) {
			if (w==0) w = 1;
			if (h==0) h = 1;
			if (offsets[i] && sizes[i]) {
				f.seek(offsets[i]);
				f.read(buf,sizes[i]);

				int cnt = 0;
				int alpha = 0;

				p = buf2;
				c = buf;
				a = buf + w*h;
				for (int y=0; y<h; y++) {
					for (int x=0; x<w; x++) {
						unsigned int k = pal[*c++];
						k = ((k&0x00FF0000)>>16) | ((k&0x0000FF00)) | ((k& 0x000000FF)<<16);

						if (hasalpha) {
							if (alphabits == 8) {
								alpha = (*a++);
							} else if (alphabits == 4) {
								alpha = (*a & (0xf << cnt++)) * 0x11;
								if (cnt == 2) {
									cnt = 0;
									a++;
								}
							} else if (alphabits == 1) {
								alpha = (*a & (1 << cnt++)) ? 0xff : 0;
								if (cnt == 8) {
									cnt = 0;
									a++;
								}
							}
						} else alpha = 0xff;

						k |= alpha << 24;
						*p++ = k;
					}
				}

				glTexImage2D(GL_TEXTURE_2D, i, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, buf2);

			} else break;
			w >>= 1;
			h >>= 1;
		}

		delete[] buf2;
		delete[] buf;
	}

	f.close();

	/*
	// TODO: Add proper support for mipmaps
	if (hasmipmaps) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	} else {
	*/
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	//}
}

TextureI::TextureI(std::string const& filename) :
	path(filename)
{
	glGenTextures(1, &name);
	LoadBLP(name, filename.c_str());

	std::cout << "Loading " << path << std::endl;
}

TextureI::~TextureI()
{
	glDeleteTextures(1, &name);
	std::cout << "Deleting " << path << std::endl;
}

void TextureI::bind() const
{
	glBindTexture(GL_TEXTURE_2D, name);
}

} // namespace wow
