#include "FileListModel.hpp"
#include "../dbcfile.h"
#include <vector>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/find_iterator.hpp>
#include <QBrush>

FileListModel::FileListModel(QObject *parent) :
	QAbstractItemModel(parent)
{
}

FileListModel::~FileListModel()
{
}

QVariant FileListModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	TreeItem* item = static_cast<TreeItem*> (index.internalPointer());

	if (role == Qt::ForegroundRole)
	{
		switch (item->color())
		{
		case 1:
			return Qt::blue;
		case 2:
			return Qt::red;
		case 3:
			return Qt::green;
		case 4:
			return Qt::darkMagenta;
		case 5:
			return Qt::darkCyan;
		case 6:
			return Qt::darkYellow;
		}
		return Qt::black;
	}

	if (role == Qt::DisplayRole)
		return QString::fromStdString(item->name());

	return QVariant();
}

Qt::ItemFlags FileListModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return 0;

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QModelIndex FileListModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	const TreeItem* parentItem = !parent.isValid() ? &root
		: static_cast<TreeItem*> (parent.internalPointer());

	TreeItem* childItem = parentItem->child(row);

	return childItem ? createIndex(row, column, childItem) : QModelIndex();
}

QModelIndex FileListModel::parent(const QModelIndex& index) const
{
	if (!index.isValid())
		return QModelIndex();

	TreeItem* childItem = static_cast<TreeItem*> (index.internalPointer());
	TreeItem* parentItem = childItem->parent();

	if (!parentItem || parentItem == &root)
		return QModelIndex();

	return createIndex(parentItem->row(), 0, parentItem);
}

int FileListModel::rowCount(const QModelIndex& parent) const
{
	if (parent.column() > 0)
		return 0;

	const TreeItem* parentItem = !parent.isValid() ? &root
		: static_cast<TreeItem*> (parent.internalPointer());

	return parentItem->children();
}

void FileListModel::add_path(const std::string& path, int color)
{
	TreeItem* item = &root;

	typedef boost::algorithm::split_iterator<std::string::const_iterator> iter;
	for (iter it = boost::algorithm::make_split_iterator(path,
		boost::algorithm::first_finder("\\", boost::algorithm::is_iequal())); it
		!= iter(); ++it)
	{
		item = item->child(boost::copy_range<std::string>(*it));
		item->color(color);
	}

	item->path(path);
}
