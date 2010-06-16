#include "mpq_stormlib.h"

#include <StormLib/StormLib.h>
#include <wx/log.h>
#include <wx/file.h>

#include <vector>
#include <string>
#include "util.h"

using namespace std;

typedef vector< pair< string, HANDLE* > > ArchiveSet;
ArchiveSet gOpenArchives;

MPQArchive::MPQArchive(const char* filename)
{
	bool result = !!SFileOpenArchive( filename, 0, 0, &mpq_a );

	wxLogMessage(_T("Opening %s"), filename);
	if(!result) {
		wxLogMessage(_T("Error opening archive %s"), filename);
		return;
	}
	gOpenArchives.push_back( make_pair( filename, &mpq_a ) );
}

MPQArchive::~MPQArchive()
{
	/*
	for(ArchiveSet::iterator i=gOpenArchives.begin(); i!=gOpenArchives.end();++i)
	{
		mpq_archive &mpq_a = **i;
		
		free(mpq_a.header);
	}
	*/
	//gOpenArchives.erase(gOpenArchives.begin(), gOpenArchives.end());
}

void MPQArchive::close()
{
	SFileCloseArchive(mpq_a);
	for(ArchiveSet::iterator it=gOpenArchives.begin(); it!=gOpenArchives.end();++it)
	{
		HANDLE &mpq_b = *it->second;
		if (&mpq_b == &mpq_a) {
			gOpenArchives.erase(it);
			//delete (*it);
			return;
		}
	}
	
}

void
MPQFile::openFile(const char* filename)
{
	eof = false;
	buffer = 0;
	pointer = 0;
	size = 0;
	if( useLocalFiles ) {
		wxString fn = gamePath;
		fn.Append(wxString(filename, wxConvUTF8));

		if (wxFile::Exists(fn)) {
			// success
			wxFile file;
			// if successfully opened
			if (file.Open(fn, wxFile::read)) {
				size = file.Length();
				if (size > 0) {
					buffer = new unsigned char[size];
					// if successfully read data
					if (file.Read(buffer, size) > 0) {
						eof = false;
						file.Close();
						return;
					} else {
						wxDELETEA(buffer);
						eof = true;
						size = 0;
					}
				}
				file.Close();
			}
		}
	}

	for(ArchiveSet::iterator i=gOpenArchives.begin(); i!=gOpenArchives.end(); ++i)
	{
		HANDLE &mpq_a = *i->second;

		HANDLE fh;

		if( !SFileOpenFileEx( mpq_a, filename, 0, &fh ) )
			continue;

		// Found!
		DWORD filesize = SFileGetFileSize( fh );
		size = filesize;

		// HACK: in patch.mpq some files don't want to open and give 1 for filesize
		if (size<=1) {
			eof = true;
			buffer = 0;
			return;
		}

		buffer = new unsigned char[size];
		SFileReadFile( fh, buffer, (DWORD)size );
		SFileCloseFile( fh );

		return;
	}

	eof = true;
	buffer = 0;
}

MPQFile::MPQFile(const char* filename):
	eof(false),
	buffer(0),
	pointer(0),
	size(0)
{
	openFile(filename);
}

MPQFile::~MPQFile()
{
	close();
}

bool MPQFile::exists(const char* filename)
{
	if( useLocalFiles ) {
		wxString fn = gamePath;
		fn.Append(wxString(filename, wxConvUTF8));
		if (wxFile::Exists(fn))
			return true;
	}

	for(ArchiveSet::iterator i=gOpenArchives.begin(); i!=gOpenArchives.end();++i)
	{
		HANDLE &mpq_a = *i->second;

		if( SFileHasFile( mpq_a, filename ) )
			return true;
	}

	return false;
}

void MPQFile::save(const char* filename)
{
	wxFile f;
	f.Open(wxString(filename, wxConvUTF8), wxFile::write);
	f.Write(buffer, size);
	f.Close();
}

size_t MPQFile::read(void* dest, size_t bytes)
{
	if (eof) 
		return 0;

	size_t rpos = pointer + bytes;
	if (rpos > size) {
		bytes = size - pointer;
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
	eof = (pointer >= size);
}

void MPQFile::seekRelative(int offset)
{
	pointer += offset;
	eof = (pointer >= size);
}

void MPQFile::close()
{
	wxDELETEA(buffer);
	eof = true;
}

size_t MPQFile::getSize()
{
	return size;
}

int MPQFile::getSize(const char* filename)
{
	if( useLocalFiles ) {
		wxString fn = gamePath;
		fn.Append(wxString(filename, wxConvUTF8));
		if (wxFile::Exists(fn)) {
			wxFile file(fn);
			return file.Length();
		}
	}

	for(ArchiveSet::iterator i=gOpenArchives.begin(); i!=gOpenArchives.end();++i)
	{
		HANDLE &mpq_a = *i->second;
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
	return buffer;
}

unsigned char* MPQFile::getPointer()
{
	return buffer + pointer;
}


#include <wx/tokenzr.h>

void getFileLists(std::set<FileTreeItem> &dest, bool filterfunc(std::string))
{
	for(ArchiveSet::iterator i=gOpenArchives.begin(); i!=gOpenArchives.end();++i)
	{
		HANDLE &mpq_a = *i->second;

		HANDLE fh;
		if( SFileOpenFileEx( mpq_a, "(listfile)", 0, &fh ) )
		{
			// Found!
			DWORD filesize = SFileGetFileSize( fh );
			size_t size = filesize;

			wxString temp(i->first.c_str(), wxConvUTF8);
			temp.MakeLower();
			int col = 0; // Black

			if (temp.Find(_T("patch.mpq")) > -1)
				col = 1; // Blue
			else if (temp.Find(_T("patch-2.mpq")) > -1)
				col = 2; // Red
			else if (temp.Find(_T("patch-3.mpq")) > -1)
				col = 3; // Green
			else if (temp.Find(_T("expansion.mpq")) > -1)
				col = 4; // Outlands Purple
			else if (temp.Find(_T("expansion2.mpq")) > -1 || temp.Find(_T("lichking.mpq")) > -1)
				col = 5; // Frozen Blue
			else if (temp.Find(_T("expansion3.mpq")) > -1)
				col = 6; // Destruction Orange

			if (size > 0 ) {
				unsigned char *buffer = new unsigned char[size];
				SFileReadFile( fh, buffer, (DWORD)size );
				unsigned char *p = buffer, *end = buffer + size;

				while (p <= end) {
					unsigned char *q=p;
					do {
						if (*q==13) 
							break;
					} while (q++<=end);

					wxString line(reinterpret_cast<char *>(p), wxConvUTF8, q-p);
					if (line.Length()==0) 
						break;
					//p += line.length();
					p = q + 2;
					//line.erase(line.length()-2, 2); // delete \r\n

					if (filterfunc(std::string(line.mb_str()))) {
						
						// This is just to help cleanup Duplicates
						// Ideally I should tokenise the string and clean it up automatically
						line.MakeLower();
						line[0] = char(line.GetChar(0) - 32);
						int ret = line.Find('\\');
						if (ret>-1)
							line[ret+1] = char(line.GetChar(ret+1) - 32);

						FileTreeItem tmp;
						tmp.fn = line.mb_str();
						tmp.col = col;
						dest.insert(tmp);
					}
				}
				
				wxDELETEA(buffer);
				p = NULL;
				end = NULL;
			}

			SFileCloseFile( fh );
		}
	}
}

