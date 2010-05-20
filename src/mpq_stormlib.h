#ifndef MPQ_H
#define MPQ_H

#include "stormlib/src/StormLib.h"

// C++ files
#include <string>
#include <set>
#include <vector>

struct FileTreeItem {
	std::string fn;
	int col;

	/// Comparison
	bool operator<(const FileTreeItem &i) const {
		return fn < i.fn;
	}

	bool operator>(const FileTreeItem &i) const {
		return fn < i.fn;
	}
};


class MPQArchive
{
	//MPQHANDLE handle;
	HANDLE mpq_a;
public:
	MPQArchive(const char* filename);
	~MPQArchive();

	void close();
};


class MPQFile
{
	//MPQHANDLE handle;
	bool eof;
	unsigned char *buffer;
	size_t pointer, size;

	// disable copying
	MPQFile(const MPQFile &f) {}
	void operator=(const MPQFile &f) {}

public:
	MPQFile():eof(false),size(0),buffer(0),pointer(0) {}
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

inline bool defaultFilterFunc(std::string) { return true; }

#endif

