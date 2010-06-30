#ifndef MPQ_H
#define MPQ_H

#include <SDL/SDL_rwops.h>
#include <boost/shared_ptr.hpp>

// C++ files
#include <string>
#include <set>
#include <vector>
#include <algorithm>

inline void flipcc(char *fcc)
{
	char t;
	t = fcc[0];
	fcc[0] = fcc[3];
	fcc[3] = t;
	t = fcc[1];
	fcc[1] = fcc[2];
	fcc[2] = t;
}

struct FileTreeItem
{
	FileTreeItem(std::string const& name, int color) :
		file_name(name), color(color)
	{
	}

	std::string file_name;
	int color;

	bool operator<(const FileTreeItem &i) const
	{
		return file_name < i.file_name;
	}
};

class Filesystem
{
public:
	Filesystem();
	~Filesystem();

	typedef boost::shared_ptr<SDL_RWops> File;

	File open(const char* filename);

	void add(const std::string& filename);

	bool exists(const char* filename);
	int getSize(const char* filename);

	static bool defaultFilterFunc(std::string const&)
	{
		return true;
	}

	void getFileLists(std::set<FileTreeItem>& dest, //
		bool filterfunc(std::string const&) = defaultFilterFunc);

private:
	friend class MPQFile;
	typedef std::vector<std::pair<std::string, void*> > ArchiveSet;
	ArchiveSet archives;
};

Filesystem& FS();

class MPQFile
{
public:
	MPQFile() :
		eof(false), pointer(0)
	{
	}

	MPQFile(const char* filename) :
		eof(false), pointer(0)
	{
		openFile(filename);
	}

	~MPQFile()
	{
		close();
	}

	void openFile(const char* filename);

	size_t read(void* dest, size_t bytes);
	size_t getSize();
	size_t getPos();
	unsigned char* getBuffer();
	unsigned char* getPointer();
	bool isEof();
	void seek(int offset);
	void seekRelative(int offset);
	void close();

private:
	Filesystem::File file;

	bool eof;
	std::vector<unsigned char> buffer;
	std::size_t pointer;

	// disable copying
	MPQFile(const MPQFile &f);
	void operator=(const MPQFile &f);
};

#endif
