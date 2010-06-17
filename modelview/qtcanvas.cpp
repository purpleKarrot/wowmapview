#include <GL/glew.h>
#include "qtcanvas.hpp"
#include "modelcanvas.h"

void qtcanvas::paintGL()
{
	if (video.render)
	{
		if (old->wmo)
			old->RenderWMO();
		else if (old->model)
			old->Render();
		else if (old->adt)
			old->RenderADT();
	}
}
