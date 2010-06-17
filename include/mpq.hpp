#ifndef MPQ_H
#define MPQ_H

// C++ files
#include <string>
#include <set>
#include <vector>
#include <algorithm>

class MPQFile
{
public:
	MPQFile() :
		eof(false), pointer(0)
	{
	}
	MPQFile(const char* filename); // filenames are not case sensitive
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

private:
	bool eof;
	std::vector<unsigned char> buffer;
	std::size_t pointer;

	// disable copying
	MPQFile(const MPQFile &f);
	void operator=(const MPQFile &f);
};

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

	void add(const std::string& filename);

	static bool defaultFilterFunc(std::string const&)
	{
		return true;
	}

	void getFileLists(std::set<FileTreeItem>& dest, //
		bool filterfunc(std::string const&) = defaultFilterFunc);

private:
	friend class MPQFile;
	std::vector<std::pair<std::string, void*> > archives;
};

Filesystem& FS();

#endif
