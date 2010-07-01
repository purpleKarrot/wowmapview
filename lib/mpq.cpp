#include "mpq.hpp"

#include <StormLib/StormLib.h>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include <vector>
#include <string>
#include <iostream>

typedef std::vector<std::pair<std::string, void*> > ArchiveSet;

static void destrow_rwops(SDL_RWops* rwops)
{
	if (rwops)
	{
		delete[] rwops->hidden.mem.base;
		SDL_RWclose(rwops);
	}
}

Filesystem::File Filesystem::open(const char* filename)
{
	std::cout << "opening file: " << filename << std::endl;

	for (ArchiveSet::iterator i = archives.begin(); i != archives.end(); ++i)
	{
		HANDLE mpq_a = i->second;

		HANDLE fh;

		if (!SFileOpenFileEx(mpq_a, filename, 0, &fh))
			continue;

		// Found!
		DWORD filesize = SFileGetFileSize(fh);

		// in patch.mpq some files don't want to open and give 1 for filesize
		if (filesize <= 1)
		{
			SFileCloseFile(fh);
			continue;
		}

		unsigned char* buffer = new unsigned char[filesize];
		SFileReadFile(fh, buffer, filesize);
		SFileCloseFile(fh);

		return File(SDL_RWFromConstMem(buffer, filesize), destrow_rwops);
	}

	return File();
}

size_t MPQFile::read(void* dest, size_t bytes)
{
	if (!file)
		return 0;

	return SDL_RWread(file.get(), dest, 1, bytes);
}

bool MPQFile::isEof()
{
	return !file || file->hidden.mem.here == file->hidden.mem.stop;
}

void MPQFile::seek(int offset)
{
	if (file)
		SDL_RWseek(file.get(), offset, RW_SEEK_SET);
}

void MPQFile::seekRelative(int offset)
{
	if (file)
		SDL_RWseek(file.get(), offset, RW_SEEK_CUR);
}

void MPQFile::close()
{
	file.reset();
}

size_t MPQFile::getSize()
{
	if (!file)
		return 0;

	return file->hidden.mem.stop - file->hidden.mem.base;
}

size_t MPQFile::getPos()
{
	if (!file)
		return 0;

	return SDL_RWtell(file.get());
}

unsigned char* MPQFile::getBuffer()
{
	if (!file)
		return 0;

	return file->hidden.mem.base;
}

unsigned char* MPQFile::getPointer()
{
	if (!file)
		return 0;

	return file->hidden.mem.here;
}

Filesystem::Filesystem()
{
}

Filesystem::~Filesystem()
{
	ArchiveSet::iterator begin = archives.begin();
	ArchiveSet::iterator end = archives.end();

	for (ArchiveSet::iterator i = begin; i != end; ++i)
		SFileCloseArchive(i->second);
}

void Filesystem::add(const std::string& filename)
{
	HANDLE handle;

	if (!SFileOpenArchive(filename.c_str(), 0, 0, &handle))
		return;

	std::cout << "Added " << filename << " to file system." << std::endl;

	archives.push_back(std::pair<std::string, void*>(filename, handle));
}

bool Filesystem::exists(const char* filename)
{
	for (ArchiveSet::iterator i = archives.begin(); i != archives.end(); ++i)
	{
		HANDLE mpq_a = i->second;

		if (SFileHasFile(mpq_a, filename))
			return true;
	}

	return false;
}

int Filesystem::getSize(const char* filename)
{
	for (ArchiveSet::iterator i = archives.begin(); i != archives.end(); ++i)
	{
		HANDLE mpq_a = i->second;
		HANDLE fh;

		if (!SFileOpenFileEx(mpq_a, filename, 0, &fh))
			continue;

		DWORD filesize = SFileGetFileSize(fh);
		SFileCloseFile(fh);
		return filesize;
	}

	return 0;
}

void Filesystem::getFileLists(std::set<FileTreeItem> &dest, //
	bool filterfunc(std::string const&))
{
	for (ArchiveSet::iterator i = archives.begin(); i != archives.end(); ++i)
	{
		HANDLE mpq_a = i->second;

		HANDLE fh;
		if (SFileOpenFileEx(mpq_a, "(listfile)", 0, &fh))
		{
			DWORD filesize = SFileGetFileSize(fh);
			size_t size = filesize;

			std::string temp = i->first;
			int col = 0; // Black

			if (boost::algorithm::iends_with(temp, "patch.mpq"))
				col = 1; // Blue
			else if (boost::algorithm::iends_with(temp, "patch-2.mpq"))
				col = 2; // Red
			else if (boost::algorithm::iends_with(temp, "patch-3.mpq"))
				col = 3; // Green
			else if (boost::algorithm::iends_with(temp, "expansion.mpq"))
				col = 4; // Outlands Purple
			else if (boost::algorithm::iends_with(temp, "expansion2.mpq")
				|| boost::algorithm::iends_with(temp, "lichking.mpq"))
				col = 5; // Frozen Blue
			else if (boost::algorithm::iends_with(temp, "expansion3.mpq"))
				col = 6; // Destruction Orange

			std::vector<char> buffer;
			if (size > 0)
			{
				buffer.resize(size);
				SFileReadFile(fh, &buffer[0], buffer.size());
				char *p = &buffer[0], *end = &buffer[size];

				while (p <= end)
				{
					char *q = p;
					do
					{
						if (*q == 13)
							break;
					} while (q++ <= end);

					std::string line(p, q - p);
					if (line.empty())
						break;

					p = q + 2;

					boost::algorithm::to_lower(line);
					if (filterfunc(line))
						dest.insert(FileTreeItem(line, col));
				}
			}

			SFileCloseFile(fh);
		}
	}
}

Filesystem& FS()
{
	static Filesystem fs;
	return fs;
}
