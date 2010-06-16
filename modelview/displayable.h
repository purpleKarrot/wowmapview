#ifndef DISPLAYABLE_H
#define DISPLAYABLE_H

#pragma warning( disable : 4100 )

class Displayable
{
public:
	virtual ~Displayable() {};

	virtual void setupAtt(int id) {};
	virtual void setupAtt2(int id) {};
	virtual void draw() {};
	virtual void reset() {};
	virtual void update(int) {};
};

#endif

