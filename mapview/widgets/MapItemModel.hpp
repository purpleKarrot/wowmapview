#ifndef MAPITEMMODEL_HPP
#define MAPITEMMODEL_HPP

#include <QAbstractListModel>
#include <vector>
#include "../appstate.h"

class MapItemModel: public QAbstractListModel
{
public:
	MapItemModel(QObject* parent = 0);

	~MapItemModel();

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const
	{
		if (role == Qt::DisplayRole)
			return QString::fromStdString(maps.at(index.row()).name);

		return QVariant();
	}

	const MapEntry& entry(std::size_t idx) const
	{
		return maps[idx];
	}

private:
	int rowCount(const QModelIndex& parent = QModelIndex()) const
	{
		return maps.size();
	}

	std::vector<MapEntry> maps;
};

#endif /* MAPITEMMODEL_HPP */
