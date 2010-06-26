#include "mpq.hpp"

#include <StormLib/StormLib.h>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include <vector>
#include <string>
#include <iostream>

typedef std::vector<std::pair<std::string, void*> > ArchiveSet;

void MPQFile::openFile(const char* filename)
{
	std::cout << "opening file: " << filename << std::endl;

	eof = false;
	pointer = 0;

	ArchiveSet& gOpenArchives = FS().archives;
	for (ArchiveSet::iterator i = gOpenArchives.begin(); i
		!= gOpenArchives.end(); ++i)
	{
		HANDLE mpq_a = i->second;

		HANDLE fh;

		if (!SFileOpenFileEx(mpq_a, filename, 0, &fh))
			continue;

		// Found!
		DWORD filesize = SFileGetFileSize(fh);

		// HACK: in patch.mpq some files don't want to open and give 1 for filesize
		if (filesize <= 1)
			return;

		buffer.resize(filesize);
		SFileReadFile(fh, &buffer[0], buffer.size());
		SFileCloseFile(fh);

		return;
	}

	eof = true;
}

MPQFile::MPQFile(const char* filename) :
	eof(false), pointer(0)
{
	openFile(filename);
}

MPQFile::~MPQFile()
{
	close();
}

bool MPQFile::exists(const char* filename)
{
	ArchiveSet& gOpenArchives = FS().archives;
	for (ArchiveSet::iterator i = gOpenArchives.begin(); i
		!= gOpenArchives.end(); ++i)
	{
		HANDLE mpq_a = i->second;

		if (SFileHasFile(mpq_a, filename))
			return true;
	}

	return false;
}

void MPQFile::save(const char* filename)
{
}

size_t MPQFile::read(void* dest, size_t bytes)
{
	if (eof)
		return 0;

	size_t rpos = pointer + bytes;
	if (rpos > buffer.size())
	{
		bytes = buffer.size() - pointer;
		eof = true;
	}

	memcpy(dest, &(buffer[pointer]), bytes);

	pointer = rpos;

	return bytes;
}

bool MPQFile::isEof()
{
	return eof;
}

void MPQFile::seek(int offset)
{
	pointer = offset;
	eof = (pointer >= buffer.size());
}

void MPQFile::seekRelative(int offset)
{
	pointer += offset;
	eof = (pointer >= buffer.size());
}

void MPQFile::close()
{
	buffer.clear();
	eof = true;
}

size_t MPQFile::getSize()
{
	return buffer.size();
}

int MPQFile::getSize(const char* filename)
{
	ArchiveSet& gOpenArchives = FS().archives;
	for (ArchiveSet::iterator i = gOpenArchives.begin(); i
		!= gOpenArchives.end(); ++i)
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

size_t MPQFile::getPos()
{
	return pointer;
}

unsigned char* MPQFile::getBuffer()
{
	return &buffer[0];
}

unsigned char* MPQFile::getPointer()
{
	return &buffer[pointer];
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
