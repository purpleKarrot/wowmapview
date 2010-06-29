#include "dbcfile.h"
#include "mpq.hpp"
#include <iostream>

void DBCFileBase::load(const char* filename)
{
	MPQFile f(filename);
	assert(!f.isEof());

	char header[4];
	unsigned int na, nb, es, ss;

	f.read(header, 4); // Number of records
	assert(header[0]=='W' && header[1]=='D' && header[2]=='B' && header[3] == 'C');
	f.read(&na, 4); // Number of records
	f.read(&nb, 4); // Number of fields
	f.read(&es, 4); // Size of a record
	f.read(&ss, 4); // String size

	recordSize = es;
	recordCount = na;
	fieldCount = nb;
	stringSize = ss;
	//	assert(fieldCount*4 == recordSize);

	data = new char[recordSize * recordCount + stringSize];
	stringTable = data + recordSize * recordCount;
	f.read(data, recordSize * recordCount + stringSize);
	f.close();
}

void DBCFileBase::check_field_count(std::size_t expected)
{
	if (field_count() == expected)
		return;

	std::cout << "WARNING: field count does not match "
		"(is " << field_count() << ", expected " << expected << ").\n";
}

void DBCFileBase::check_record_size(std::size_t expected)
{
	if (record_size() == expected)
		return;

	std::cout << "ERROR: record size does not match "
		"(is " << record_size() << ", expected " << expected << ").\n";
}
