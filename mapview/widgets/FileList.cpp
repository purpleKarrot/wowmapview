#include "FileList.hpp"
#include <QVBoxLayout>
#include <QHeaderView>
#include <iostream>
#include "../wowmapview.h"

FileList::FileList()
{
	combo_box = new QComboBox(this);
	map_item_model = new MapItemModel(this);
	tree_view = new QTreeView(this);
	tree_view->header()->hide();

	tree_view->setModel(map_item_model);

	connect(tree_view, SIGNAL(doubleClicked(const QModelIndex&)), //
		this, SLOT(select(const QModelIndex&)));

	QVBoxLayout* layout = new QVBoxLayout;
	layout->addWidget(combo_box);
	layout->addWidget(tree_view);
	setLayout(layout);
}

FileList::~FileList()
{
}

void enterWorld(const MapEntry& entry);


void FileList::select(const QModelIndex& index)
{
	enterWorld(map_item_model->entry(index.row()));

//	if (gStates.empty())
//		return;
//	gStates[gStates.size() - 1]->enter(map_item_model->entry(index.row()));
}
