#ifndef MPQ_H
#define MPQ_H

#include <StormLib/StormLib.h>

// C++ files
#include <string>
#include <set>
#include <vector>
#include <algorithm>

class MPQFile
{
	bool eof;
	std::vector<unsigned char> buffer;
	std::size_t pointer;

	// disable copying
	MPQFile(const MPQFile &f);
	void operator=(const MPQFile &f);

public:
	MPQFile():eof(false),pointer(0) {}
	MPQFile(const char* filename);	// filenames are not case sensitive
	void openFile(const char* filename);
	~MPQFile();
	size_t read(void* dest, size_t bytes);
	size_t getSize();
	size_t getPos();
	unsigned char* getBuffer();
	unsigned char* getPointer();
	bool isEof();
	void seek(int offset);
	void seekRelative(int offset);
	void close();
	void save(const char* filename);

	static bool exists(const char* filename);
	static int getSize(const char* filename); // Used to do a quick check to see if a file is corrupted
};

inline void flipcc(char *fcc)
{
	char t;
	t=fcc[0];
	fcc[0]=fcc[3];
	fcc[3]=t;
	t=fcc[1];
	fcc[1]=fcc[2];
	fcc[2]=t;
}

class Filesystem
{
public:
	Filesystem()
	{
	}

	~Filesystem()
	{
		std::for_each(archives.begin(), archives.end(), SFileCloseArchive);
	}

	void add(const char* filename);

private:
	friend class MPQFile;
	std::vector<HANDLE> archives;
};

Filesystem& FS();

#endif
