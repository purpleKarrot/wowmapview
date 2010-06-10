#ifndef FILELIST_HPP
#define FILELIST_HPP

#include <QWidget>
#include <QComboBox>
#include <QTreeView>
#include "MapItemModel.hpp"

class FileList: public QWidget
{
Q_OBJECT

public:
	FileList();
	~FileList();

private slots:
	void select(const QModelIndex& index);

private:
	QComboBox* combo_box;
	QTreeView* tree_view;
	MapItemModel* map_item_model;
};

#endif /* FILELIST_HPP */
