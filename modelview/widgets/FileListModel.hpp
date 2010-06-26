#ifndef MAPITEMMODEL_HPP
#define MAPITEMMODEL_HPP

#include <QAbstractItemModel>
#include "TreeItem.hpp"

class FileListModel: public QAbstractItemModel
{
public:
	FileListModel(QObject *parent = 0);

	~FileListModel();

	void add_path(const std::string& path, int color);

private:
	QVariant data(const QModelIndex &index, int role) const;

	Qt::ItemFlags flags(const QModelIndex &index) const;

	QModelIndex index(int row, int column, const QModelIndex &parent) const;

	QModelIndex parent(const QModelIndex &index) const;

	int rowCount(const QModelIndex &parent) const;

	int columnCount(const QModelIndex &parent) const
	{
		return 1;
	}

private:
	TreeItem root;
};

#endif /* MAPITEMMODEL_HPP */
