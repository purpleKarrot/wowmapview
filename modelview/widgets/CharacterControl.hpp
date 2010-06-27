#ifndef CHARACTERCONTROL_HPP
#define CHARACTERCONTROL_HPP

#include <QWidget>
#include <QSpinBox>
#include "../charcontrol.h"

class CharacterControl: public QWidget
{
Q_OBJECT

public:
	CharacterControl(CharControl& old, QWidget* parent = 0);
	~CharacterControl();

	void setVisible(bool visible);

private slots:
	void skin_color(int value);
	void face_type(int value);
	void hair_style(int value);
	void hair_color(int value);
	void facial_feature(int value);
	void randomize();

private:
	CharControl& settings;

	QSpinBox* skin_color_;
	QSpinBox* face_type_;
	QSpinBox* hair_style_;
	QSpinBox* hair_color_;
	QSpinBox* facial_feature_;
	QSpinBox* facial_color_;
};

#endif /* CHARACTERCONTROL_HPP */
