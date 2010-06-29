#include "dbcfile.h"
#include "mpq.hpp"

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
