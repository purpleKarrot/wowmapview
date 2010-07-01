#include "modelviewer.h"
#include "video.h"
#include "mpq.hpp"

#include <wx/display.h>

#include "./GL/glew.h"

#ifdef _WINDOWS
	#include "./GL/wglew.h"
#elif _MAC // OSX
    #include <GL/glew.h>
#else // Linux
	#include <GL/glxew.h>

	void (*wglGetProcAddress(const char *function_name))(void)
	{
		return glXGetProcAddress((GLubyte*)function_name);
	}
#endif

extern ModelViewer *g_modelViewer;

VideoSettings video;
TextureManager texturemanager;

VideoSettings::VideoSettings()
{
#ifdef _WINDOWS
	hWnd = NULL;
	hRC = NULL;
	hDC = NULL;
#endif

	pixelFormat = 0;
	xRes = 0;
	yRes = 0;

	fov = 45.0f;

	init = false;
	render = false;

	//useAntiAlias = true;
	useEnvMapping = true;
	useShaders = true;
	useCompression = false;
	useVBO = false;
	usePBO = false;
#ifdef _WINDOWS
	useFBO = true;
#else
	useFBO = false;
#endif
}

VideoSettings::~VideoSettings()
{
}

bool VideoSettings::Init()
{
	int glewErr = glewInit();
    if (glewErr != GLEW_OK) {
        // problem: glewInit failed, something is seriously wrong
		wxLogMessage(_T("Error: GLEW failed to initialise.\n\tGLEW Error: %s\n"), glewGetErrorString(glewErr));
		return false;
	}  else {
		wxLogMessage(_T("Info: GLEW successfully initiated.\n"));
	}

	// Now get some specifics on the card
	// First up, the details
	if(glewIsSupported("GL_EXT_texture_filter_anisotropic"))
		glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, (GLint*)&AnisofilterLevel);
	else
		AnisofilterLevel = 0;

	glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, (GLint*)&numTextureUnits);
	vendor = ((char *)glGetString(GL_VENDOR));
	version = ((char *)glGetString(GL_VERSION));
	renderer = ((char *)glGetString(GL_RENDERER));

	//char *extens = (char *)glGetString(GL_EXTENSIONS);
	//wxLogMessage(extens);

	double num = atof((char *)glGetString(GL_VERSION));
	supportOGL20 = (num >= 2.0);
	if (supportOGL20)
		supportNPOT = true;
	else
		supportNPOT = glewIsSupported("GL_ARB_texture_non_power_of_two") == GL_TRUE ? true : false;

	supportFragProg = glewIsSupported("GL_ARB_fragment_program") == GL_TRUE ? true : false;
	supportVertexProg = glewIsSupported("GL_ARB_vertex_program") == GL_TRUE ? true : false;
	supportGLSL = glewIsSupported("GL_ARB_shading_language_100") == GL_TRUE ? true : false;
	supportShaders = (supportFragProg && supportVertexProg);

	supportDrawRangeElements = glewIsSupported("GL_EXT_draw_range_elements") == GL_TRUE ? true : false;
	supportMultiTex = glewIsSupported("GL_ARB_multitexture") == GL_TRUE ? true : false;
	supportVBO = glewIsSupported("GL_ARB_vertex_buffer_object") == GL_TRUE ? true : false;
	supportCompression = glewIsSupported("GL_ARB_texture_compression GL_ARB_texture_cube_map GL_EXT_texture_compression_s3tc") == GL_TRUE ? true : false;
	supportPointSprites = glewIsSupported("GL_ARB_point_sprite GL_ARB_point_parameters") == GL_TRUE ? true : false;
#ifdef _WINDOWS
	supportPBO = wglewIsSupported("WGL_ARB_pbuffer WGL_ARB_render_texture") == GL_TRUE ? true : false;
	supportAntiAlias = wglewIsSupported("WGL_ARB_multisample") == GL_TRUE ? true : false;
	supportWGLPixelFormat = wglewIsSupported("WGL_ARB_pixel_format") == GL_TRUE ? true : false;
#endif
	supportFBO = glewIsSupported("GL_EXT_framebuffer_object") == GL_TRUE ? true : false;
	supportTexRects = glewIsSupported("GL_ARB_texture_rectangle") == GL_TRUE ? true : false;

	// Now output and log the info
	wxLogMessage(_T("Video Renderer: %s"), renderer);
	wxLogMessage(_T("Video Vendor: %s"), vendor);
	wxLogMessage(_T("Driver Version: %s"), version);

	
	if (wxString(renderer, wxConvUTF8).IsSameAs(_T("GDI Generic"), false)) {
		wxLogMessage(_T("\tWarning: Running in software mode, this is not enough. Please try updating your video drivers."));
		// bloody oath - wtb a graphics card
		hasHardware = false;
	} else {
		hasHardware = true;
	}
	
	/*
	// Display Device info
	wxLogMessage(_T("Display Device Count: %i"), wxDisplay::GetCount());
	for (size_t i=0; i<wxDisplay::GetCount(); i++) {
		wxDisplay disp(i);
		wxRect bounds = disp.GetGeometry();
		wxString name = disp.GetName();
		wxLogMessage(_T("Device Name: %s\n Primary Device: %i\n Bit-Depth: %ibits\n Window Bounds: %i,%i,%i,%i\n"), name.c_str(), disp.IsPrimary(), disp.GetCurrentMode().GetDepth(), bounds.x, bounds.y, bounds.width, bounds.height);
	}

	if (wxDisplay::GetCount() > 1) {
		// TODO: If they have more than one device we should ask which they want to use.
		
	}
	*/

	wxLogMessage(_T("Support wglPixelFormat: %s"), supportWGLPixelFormat ? _T("true") : _T("false"));
	wxLogMessage(_T("Support Texture Compression: %s"), supportCompression ? _T("true") : _T("false"));
	wxLogMessage(_T("Support Multi-Textures: %s"), supportMultiTex ? _T("true") : _T("false"));
	wxLogMessage(_T("Support Draw Range Elements: %s"), supportDrawRangeElements ? _T("true") : _T("false"));
	wxLogMessage(_T("Support Vertex Buffer Objects: %s"), supportVBO ? _T("true") : _T("false"));
	wxLogMessage(_T("Support Point Sprites: %s"), supportPointSprites ? _T("true") : _T("false"));
	wxLogMessage(_T("Support Pixel Shaders: %s"), supportFragProg ? _T("true") : _T("false"));
	wxLogMessage(_T("Support Vertex Shaders: %s"), supportVertexProg ? _T("true") : _T("false"));
	wxLogMessage(_T("Support GLSL: %s"), supportGLSL ? _T("true") : _T("false"));
	wxLogMessage(_T("Support Anti-Aliasing: %s"), supportAntiAlias ? _T("true") : _T("false"));
	wxLogMessage(_T("Support Pixel Buffer Objects: %s"), supportPBO ? _T("true") : _T("false"));
	wxLogMessage(_T("Support Frame Buffer Objects: %s"), supportFBO ? _T("true") : _T("false"));
	wxLogMessage(_T("Support Non-Power-of-Two: %s"), supportNPOT ? _T("true") : _T("false"));
	wxLogMessage(_T("Support Rectangle Textures: %s"), supportTexRects ? _T("true") : _T("false"));
	wxLogMessage(_T("Support OpenGL 2.0: %s"), supportOGL20 ? _T("true") : _T("false"));

	// Max texture sizes
	GLint texSize; 
	// Rectangle
	if (glewIsSupported("GL_ARB_texture_rectangle")) {
		glGetIntegerv(GL_MAX_RECTANGLE_TEXTURE_SIZE_ARB, &texSize); 
		wxLogMessage(_T("Max Rectangle Texture Size Supported: %i"), texSize);
	}
	// Square
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texSize); 
	wxLogMessage(_T("Max Texture Size Supported: %i\n"), texSize);
	

	
	// Debug:
	//supportPointSprites = false; // Always set to false,  for now.
	//supportVBO = false;
	//supportFBO = false;
	//supportNPOT = false;
	//supportPBO = false;
	//supportCompression = false;
	//supportTexRects = false;

	init = true;

	return true;
}

void VideoSettings::InitGL()
{
	if (!init)
		return;

	GLenum err = 0;
	err = glGetError();
	if (err)
		wxLogMessage(_T("OpenGL Error: %s : %s :[0x%x] : An error occured on line %i"), __FUNCTION__, __FILE__, (unsigned int)err, __LINE__);

	if (supportDrawRangeElements && supportVBO) {
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	} else {
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	
	if(supportAntiAlias && curCap.aaSamples>0)
		glEnable(GL_MULTISAMPLE_ARB);

	// Colour and materials
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT);
	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	glColorMaterial(GL_FRONT_AND_BACK, GL_EMISSION);
	glColorMaterial(GL_FRONT_AND_BACK, GL_SPECULAR);
	
	// For environmental mapped meshes
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 18.0f);


	// TODO: Implement a scene graph, or scene manager to optimise OpenGL?
	// Default texture settings.
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);
	/*
	glTexEnvf(GL_TEXTURE_ENV,GL_RGB_SCALE, 1.000000);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB, GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE2_RGB, GL_CONSTANT);
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB, GL_MODULATE);
	glTexEnvf(GL_TEXTURE_ENV,GL_ALPHA_SCALE, 1.000000);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA, GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_ALPHA, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE2_ALPHA, GL_CONSTANT);
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA, GL_REPLACE);
	*/
	
	/*
	// Alpha blending texture settings.
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);
	glTexEnvf(GL_TEXTURE_ENV,GL_RGB_SCALE, 1.000000);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB, GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE2_RGB, GL_CONSTANT);
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB, GL_MODULATE);
	glTexEnvf(GL_TEXTURE_ENV,GL_ALPHA_SCALE, 1.000000);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA, GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_ALPHA, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE2_ALPHA, GL_CONSTANT);
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA, GL_MODULATE);
	*/

	glAlphaFunc(GL_GEQUAL, 0.9f);
	glDepthFunc(GL_LEQUAL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_SWAP_BYTES, false);
	glPixelStorei(GL_PACK_LSB_FIRST, false);

	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	err = glGetError();
	if (err)
		wxLogMessage(_T("OpenGL Error: %s : %s :[0x%x] : An error occured on line %i"), __FUNCTION__, __FILE__, (unsigned int)err, __LINE__);
}


void VideoSettings::EnumDisplayModes()
{
}

// This function basically just uses any available display mode
bool VideoSettings::GetAvailableMode()
{
	return true;
}

bool VideoSettings::GetCompatibleWinMode(VideoCaps caps)
{
	return false;
}

GLuint TextureManager::add(std::string name)
{
	GLuint id = 0;

	// if the item already exists, return the existing ID
	if (names.find(name) != names.end()) {
		id = names[name];
		items[id]->addref();
		return id;
	}

	// Else, create the texture

	Texture *tex = new Texture(name);
	if (tex) {
		// clear old texture memory from vid card
		glDeleteTextures(1, &id);
		// create new texture and put it in memory
		glGenTextures(1, &id);

		tex->id = id;
		LoadBLP(id, tex);

		do_add(name, id, tex);
		return id;
	}

	return 0;
}
//#define SAVE_BLP

void TextureManager::LoadBLP(GLuint id, Texture *tex)
{
	// Vars
	int offsets[16], sizes[16], w=0, h=0, type=0;
	GLint format = 0;
	char attr[4];

	// bind the texture
	glBindTexture(GL_TEXTURE_2D, id);
	
	MPQFile f(tex->name.c_str());
	if (g_modelViewer) {
		g_modelViewer->modelOpened->Add(wxString(tex->name.c_str(), wxConvUTF8));
	}
	if (f.isEof()) {
		tex->id = 0;
		wxLogMessage(_T("Error: Could not load the texture '%s'"), wxString(tex->name.c_str(), wxConvUTF8).c_str());
		f.close();
		return;
	} else {
		//tex->id = id; // I don't see the id being set anywhere,  should I set it now?
		wxLogMessage(_T("Loading texture: %s"), wxString(tex->name.c_str(), wxConvUTF8).c_str());
	}

	f.seek(4);
	f.read(&type,4);
	f.read(attr,4);
	f.read(&w,4);
	f.read(&h,4);
	f.read(offsets,4*16);
	f.read(sizes,4*16);

	tex->w = w;
	tex->h = h;

	bool hasmipmaps = (attr[3]>0);
	int mipmax = hasmipmaps ? 16 : 1;

	/*
	reference: http://en.wikipedia.org/wiki/.BLP
	*/
	//wxLogMessage(_T("[BLP]: type: %d, encoding: %d, alphadepth: %d, alphaencoding: %d, mipmap: %d, %d*%d"), type, attr[0], attr[1], attr[2], attr[3], w, h);
	if (type == 0) { // JPEG compression
		std::cerr << "ERROR: " << tex->name << " has type " << type
				<< std::endl;
	} else if (type == 1) {
		if (attr[0] == 2) {
			/*
			Type 1 Encoding 2 AlphaDepth 0 (DXT1 no alpha)
			The image data is formatted using DXT1 compression with no alpha channel.

			Type 1 Encoding 2 AlphaDepth 1 (DXT1 one bit alpha)
			The image data is formatted using DXT1 compression with a one-bit alpha channel.

			Type 1 Encoding 2 AlphaDepth 8 (DXT3)
			The image data is formatted using DXT3 compression.

			Type 1 Encoding 2 AlphaDepth 8 AlphaEncoding 7 (DXT5)
			The image data are formatted using DXT5 compression.
			*/
			// encoding 2, directx compressed
			unsigned char *ucbuf = NULL;
			if (!video.supportCompression) 
				ucbuf = new unsigned char[w*h*4];
		
			format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			int blocksize = 8;
			
			// guesswork here :(
			// new alpha bit depth == 4 for DXT3, alfred 2008/10/11
			if (attr[1]==8 || attr[1]==4) {
				format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
				blocksize = 16;
			}

			// Fix to the BLP2 format required in WoW 2.0 thanks to Linghuye (creator of MyWarCraftStudio)
			if (attr[1]==8 && attr[2]==7) {
				format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
				blocksize = 16;
			}

			tex->compressed = true;

			unsigned char *buf = new unsigned char[sizes[0]];

			// do every mipmap level
			for (int i=0; i<mipmax; i++) {
				if (w==0) w = 1;
				if (h==0) h = 1;
				if (offsets[i] && sizes[i]) {
					f.seek(offsets[i]);
					f.read(buf,sizes[i]);

					int size = ((w+3)/4) * ((h+3)/4) * blocksize;

					if (video.supportCompression) {
						glCompressedTexImage2DARB(GL_TEXTURE_2D, i, format, w, h, 0, size, buf);
					} else {
						decompressDXTC(format, w, h, size, buf, ucbuf);					
						glTexImage2D(GL_TEXTURE_2D, i, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, ucbuf);
					}
					
				} else break;
				w >>= 1;
				h >>= 1;
			}

			wxDELETEA(buf);
			if (!video.supportCompression) 
				wxDELETEA(ucbuf);

		} else if (attr[0]==1) {
			/*
			Type 1 Encoding 0 AlphaDepth 0 (uncompressed paletted image with no alpha)
			Each by of the image data is an index into Palette which contains the actual RGB value for the pixel. Although the palette entries are 32-bits, the alpha value of each Palette entry may contain garbage and should be discarded.

			Type 1 Encoding 1 AlphaDepth 1 (uncompressed paletted image with 1-bit alpha)
			This is the same as Type 1 Encoding 1 AlphaDepth 0 except that immediately following the index array is a second image array containing 1-bit alpha values for each pixel. The first byte of the array is for pixels 0 through 7, the second byte for pixels 8 through 15 and so on. Bit 0 of each byte corresponds to the first pixel (leftmost) in the group, bit 7 to the rightmost. A set bit indicates the pixel is opaque while a zero bit indicates a transparent pixel.

			Type 1 Encoding 1 AlphaDepth 8(uncompressed paletted image with 8-bit alpha)
			This is the same as Type 1 Encoding 1 AlphaDepth 0 except that immediately following the index array is a second image array containing the actual 8-bit alpha values for each pixel. This second array starts at BLP2Header.Offset[0] + BLP2Header.Width * BLP2Header.Height.
			*/

			// encoding 1, uncompressed
			unsigned int pal[256];
			f.read(pal, 1024);

			unsigned char *buf = new unsigned char[sizes[0]];
			unsigned int *buf2 = new unsigned int[w*h];
			unsigned int *p = NULL;
			unsigned char *c = NULL, *a = NULL;

			int alphabits = attr[1];
			bool hasalpha = (alphabits!=0);

			tex->compressed = false;

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
									//alpha = (*a & (128 >> cnt++)) ? 0xff : 0;
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

			wxDELETEA(buf2);
			wxDELETEA(buf);
		} else {
			wxLogMessage(_T("Error: %s:%s#%d type=%d, attr[0]=%d"), __FILE__, __FUNCTION__, __LINE__, type, attr[0]);
		}
	}

	f.close();

	/*
	// TODO: Add proper support for mipmaps
	if (hasmipmaps) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	} else {
	*/
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//}
}

void TextureManager::doDelete(GLuint id)
{
	if (glIsTexture(id)) {
		glDeleteTextures(1, &id);
	}
}


void Texture::getPixels(unsigned char* buf, unsigned int format)
{
	glBindTexture(GL_TEXTURE_2D, id);
	glGetTexImage(GL_TEXTURE_2D, 0, format, GL_UNSIGNED_BYTE, buf);
}

void decompressDXTC(GLint format, int w, int h, size_t size, unsigned char *src, unsigned char *dest)
{	
	// DXT1 Textures, currently being handles by our routine below
	if (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) {
		DDSDecompressDXT1(src, w, h, dest);
		return;
	}
	
	// DXT3 Textures
	if (format == GL_COMPRESSED_RGBA_S3TC_DXT3_EXT) {
		DDSDecompressDXT3(src, w, h, dest);
		return;
	}

	// DXT5 Textures
	if (format == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT)	{
		//DXT5UnpackAlphaValues(src, w, h, dest);
		DDSDecompressDXT5(src, w, h, dest);
		return;
	}
}
