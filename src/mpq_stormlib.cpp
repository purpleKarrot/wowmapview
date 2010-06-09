#include "mpq_stormlib.h"
#include "wowmapview.h"

#include <vector>
#include <string>
#include <iostream>

typedef std::vector<HANDLE> ArchiveSet;

void
MPQFile::openFile(const char* filename)
{
	eof = false;
	pointer = 0;

	ArchiveSet& gOpenArchives = FS().archives;
	for(ArchiveSet::iterator i=gOpenArchives.begin(); i!=gOpenArchives.end(); ++i)
	{
		HANDLE mpq_a = *i;

		HANDLE fh;

		if( !SFileOpenFileEx( mpq_a, filename, 0, &fh ) )
			continue;

		// Found!
		DWORD filesize = SFileGetFileSize( fh );

		// HACK: in patch.mpq some files don't want to open and give 1 for filesize
		if (filesize<=1)
			return;

		buffer.resize(filesize);
		SFileReadFile( fh, &buffer[0], buffer.size() );
		SFileCloseFile( fh );

		return;
	}

	eof = true;
}

MPQFile::MPQFile(const char* filename):
	eof(false),
	pointer(0)
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
	for(ArchiveSet::iterator i=gOpenArchives.begin(); i!=gOpenArchives.end();++i)
	{
		HANDLE mpq_a = *i;

		if( SFileHasFile( mpq_a, filename ) )
			return true;
	}

	return false;
}

void MPQFile::save(const char* filename)
{
/*
	wxFile f;
	f.Open(wxString(filename, wxConvUTF8), wxFile::write);
	f.Write(buffer, size);
	f.Close();
*/
}

size_t MPQFile::read(void* dest, size_t bytes)
{
	if (eof)
		return 0;

	size_t rpos = pointer + bytes;
	if (rpos > buffer.size()) {
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
	for(ArchiveSet::iterator i=gOpenArchives.begin(); i!=gOpenArchives.end();++i)
	{
		HANDLE mpq_a = *i;
		HANDLE fh;
		
		if( !SFileOpenFileEx( mpq_a, filename, 0, &fh ) )
			continue;

		DWORD filesize = SFileGetFileSize( fh );
		SFileCloseFile( fh );
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

void Filesystem::add(const std::string& filename)
{
	HANDLE handle;

	if (!SFileOpenArchive(filename.c_str(), 0, 0, &handle))
		return;

	std::cout << "Added " << filename << " to file system." << std::endl;

	archives.push_back(handle);
}

Filesystem& FS()
{
	static Filesystem fs;
	return fs;
}
