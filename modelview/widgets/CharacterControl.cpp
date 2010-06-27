#include "CharacterControl.hpp"
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include "../globalvars.h"

CharacterControl::CharacterControl(CharControl& old, QWidget* parent) :
	QWidget(parent), settings(old)
{
	QGridLayout* layout = new QGridLayout(this);

#define ADD_CONTROLS(row, member, name)                \
	member##_ = new QSpinBox(this);                    \
	layout->addWidget(new QLabel(name, this), row, 0); \
	layout->addWidget(member##_, row, 1);              \
	connect(member##_, SIGNAL(valueChanged(int)), this, SLOT(member(int)));

	ADD_CONTROLS(0, skin_color, "skin color");
	ADD_CONTROLS(1, face_type, "face type");
	ADD_CONTROLS(2, hair_style, "hair style");
	ADD_CONTROLS(3, hair_color, "hair color");
	ADD_CONTROLS(4, facial_feature, "facial feature");

#undef ADD_CONTROLS

	QPushButton* button = new QPushButton("Randomize", this);
	layout->addWidget(button, 5, 0, 1, 2);
	connect(button, SIGNAL(clicked()), this, SLOT(randomize()));

	setLayout(layout);
}

void CharacterControl::setVisible(bool visible)
{
	if (visible)
	{
#define UPDATE(member, value, maximum)                                \
		member->setSuffix(QString(" / %1").arg(settings.cd.maximum)); \
		member->setValue(settings.cd.value + 1);                      \
		member->setMinimum(1);                                        \
		member->setMaximum(settings.cd.maximum);

		UPDATE(skin_color_, skinColor, maxSkinColor);
		UPDATE(face_type_, faceType, maxFaceType);
		UPDATE(hair_style_, hairStyle, maxHairStyle);
		UPDATE(hair_color_, hairColor, maxHairColor);
		UPDATE(facial_feature_, facialHair, maxFacialHair);

#undef UPDATE
	}

	QWidget::setVisible(visible);
}

CharacterControl::~CharacterControl()
{
}

void CharacterControl::randomize()
{
	settings.cd.skinColor = rand() % settings.cd.maxSkinColor;
	settings.cd.faceType = rand() % settings.cd.maxFaceType;
	settings.cd.hairStyle = rand() % settings.cd.maxHairStyle;
	settings.cd.hairColor = rand() % settings.cd.maxHairColor;
	settings.cd.facialHair = rand() % settings.cd.maxFacialHair;

	if (g_canvas)
		settings.RefreshModel();
}

void CharacterControl::skin_color(int value)
{
	settings.cd.skinColor = value - 1;

	if (g_canvas)
		settings.RefreshModel();
}

void CharacterControl::face_type(int value)
{
	settings.cd.faceType = value - 1;

	if (g_canvas)
		settings.RefreshModel();
}

void CharacterControl::hair_style(int value)
{
	settings.cd.hairStyle = value - 1;

	if (g_canvas)
		settings.RefreshModel();
}

void CharacterControl::hair_color(int value)
{
	settings.cd.hairColor = value - 1;

	if (g_canvas)
		settings.RefreshModel();
}

void CharacterControl::facial_feature(int value)
{
	settings.cd.facialHair = value - 1;

	if (g_canvas)
		settings.RefreshModel();
}
