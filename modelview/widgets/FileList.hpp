#ifndef FILELIST_HPP
#define FILELIST_HPP

#include <QWidget>
#include <QComboBox>
#include <QTreeView>
#include "FileListModel.hpp"

class FileList: public QWidget
{
Q_OBJECT

public:
	FileList();
	~FileList();

private slots:
void filter(const QString& text);
void select(const QModelIndex& index);

private:
	QComboBox* combo_box;
	QTreeView* tree_view;
	FileListModel* file_list_model;
};

#endif /* FILELIST_HPP */
