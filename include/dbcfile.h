#ifndef DBCFILE_H
#define DBCFILE_H

#include <cassert>
#include <string>
#include <stdexcept>

class DBCFileBase
{
public:
	DBCFileBase() :
		data(0)
	{
	}

	~DBCFileBase()
	{
		delete[] data;
	}

	void load(const char* filename);

protected:
	void check_field_count(std::size_t expected);
	void check_record_size(std::size_t expected);

	std::size_t field_count() const
	{
		return fieldCount;
	}

	std::size_t record_size() const
	{
		return recordSize;
	}

	std::size_t string_size() const
	{
		return stringSize;
	}

	const char* string_table() const
	{
		return stringTable;
	}

private:
	std::size_t recordSize;
	std::size_t recordCount;
	std::size_t fieldCount;
	std::size_t stringSize;

protected:
	char* data;
	char* stringTable;
};

template<typename RecordT>
class DBCFileTemplate: public DBCFileBase
{
public:
	typedef RecordT Record;

	class Iterator
	{
	public:
		Iterator(const char* offset, const char* string,
			std::size_t record_size) :
			record(offset, string), record_size(record_size)
		{
		}

		/// Advance (prefix only)
		Iterator& operator++()
		{
			record.record += record_size;
			return *this;
		}

		Record const& operator*() const
		{
			return record;
		}

		const Record* operator->() const
		{
			return &record;
		}

		bool operator!=(const Iterator &b) const
		{
			return record.record != b.record.record;
		}

	private:
		Record record;
		std::size_t record_size;
	};

	// Get record by id
	Record getRecord(size_t id)
	{
		assert(data);
		return Record(&data[id * record_size()], string_table());
	}

	/// Get begin iterator over records
	Iterator begin()
	{
		assert(data);
		return Iterator(data, stringTable, record_size());
	}

	/// Get begin iterator over records
	Iterator end()
	{
		assert(data);
		return Iterator(stringTable, stringTable, record_size());
	}

	/// Trivial
	size_t getRecordCount() const
	{
		return recordCount;
	}

	size_t getFieldCount() const
	{
		return field_count();
	}
};

template<typename T> // TODO: enable if T is POD
struct dbc_field
{
	static T read(std::size_t offset, const char* record, const char* /*string*/)
	{
		return *reinterpret_cast<const T*> (&record[offset]);
	}
};

template<>
struct dbc_field<const char*>
{
	static const char* read(std::size_t offset, const char* record,
		const char* string)
	{
		offset = dbc_field<unsigned int>::read(offset, record, string);
		return &string[offset];
	}
};

class RecordBase
{
public:
	RecordBase(const char *record, const char* string) :
		record(record), string(string)
	{
	}

protected:
	template<typename T>
	T get(std::size_t offset) const
	{
		return dbc_field<T>::read(offset, record, string);
	}

public:
	const char* record;
	const char* string;
};

class Record: public RecordBase
{
public:
	Record(const char* offset, const char* string) :
		RecordBase(offset, string)
	{
	}

	template<typename T>
	T Get(std::size_t field) const
	{
		return get<T> (field * 4);
	}

	unsigned char getByte(size_t ofs) const
	{
		return get<unsigned char> (ofs);
	}

	const char* getString(size_t field) const
	{
		return get<const char*> (field * 4);
	}

	const char* getLocalizedString(size_t field, size_t locale = -1) const
	{
		if (locale == -1)
		{
			for (locale = 0; locale < 8; ++locale)
			{
				std::size_t offset = Get<unsigned int> (field + locale);
				if (offset)
					break;
			}
		}

		return get<const char*> ((field + locale) * 4);
	}
};

class DBCFile: public DBCFileTemplate<Record>
{
public:
	DBCFile(const std::string& filename) :
		filename(filename)
	{
	}

	void open()
	{
		load(filename.c_str());
	}

	struct NotFound: std::runtime_error
	{
		NotFound() :
			std::runtime_error("Key was not found")
		{
		}
	};

private:
	std::string filename;
};

template<typename DBC>
inline typename DBC::Record get_by_ID(DBC& file, unsigned int id)
{
	for (typename DBC::Iterator i = file.begin(); i != file.end(); ++i)
	{
		if (i->id() == id)
			return (*i);
	}

	throw DBCFile::NotFound();
}

inline DBCFile::Record getByID(DBCFile& file, unsigned int id, size_t field = 0)
{
	for (DBCFile::Iterator i = file.begin(); i != file.end(); ++i)
	{
		if (i->Get<unsigned int> (field) == id)
			return (*i);
	}

	throw DBCFile::NotFound();
}

#endif
