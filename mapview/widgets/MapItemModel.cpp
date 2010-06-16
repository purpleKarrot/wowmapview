#include "MapItemModel.hpp"
#include "../dbcfile.h"

MapItemModel::MapItemModel(QObject* parent) :
	QAbstractListModel(parent)
{
	DBCFile f("DBFilesClient\\Map.dbc");
	f.open();

	for (DBCFile::Iterator i = f.begin(); i != f.end(); ++i)
	{
		MapEntry e;
		e.id = i->getInt(0);
		e.name = i->getString(1);
		e.description = i->getString(3);
		if (e.description == "")
			e.description = e.name;

		if (e.name.find("Transport") != std::string::npos //
			|| e.name.find("Test") != std::string::npos //
			|| e.name.find("test") != std::string::npos //
			|| e.name.find("development") != std::string::npos //
			|| e.name == "QA_DVD" || e.name == "unused" || e.name == "Collin")
			continue;

		maps.push_back(e);
	}
}

MapItemModel::~MapItemModel()
{
}
