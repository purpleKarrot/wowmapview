#include "frustum.h"
#include "video.h"


void Plane::normalize()
{
	float len;
	len = sqrtf(a*a + b*b + c*c);
	a /= len;
	b /= len;
	c /= len;
	d /= len;
}


void Frustum::retrieve()
{
	float mat[16];
	
	glGetFloatv(GL_MODELVIEW_MATRIX, mat);
	glMatrixMode(GL_PROJECTION);
	
	glPushMatrix();
	
	glMultMatrixf(mat);
	glGetFloatv(GL_PROJECTION_MATRIX, mat);
	
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	
	planes[RIGHT].a = mat[ 3] - mat[ 0];
	planes[RIGHT].b = mat[ 7] - mat[ 4];
	planes[RIGHT].c = mat[11] - mat[ 8];
	planes[RIGHT].d = mat[15] - mat[12];
	planes[RIGHT].normalize();

	planes[LEFT].a = mat[ 3] + mat[ 0];
	planes[LEFT].b = mat[ 7] + mat[ 4];
	planes[LEFT].c = mat[11] + mat[ 8];
	planes[LEFT].d = mat[15] + mat[12];
	planes[LEFT].normalize();

	planes[BOTTOM].a = mat[ 3] + mat[ 1];
	planes[BOTTOM].b = mat[ 7] + mat[ 5];
	planes[BOTTOM].c = mat[11] + mat[ 9];
	planes[BOTTOM].d = mat[15] + mat[13];
	planes[BOTTOM].normalize();

	planes[TOP].a = mat[ 3] - mat[ 1];
	planes[TOP].b = mat[ 7] - mat[ 5];
	planes[TOP].c = mat[11] - mat[ 9];
	planes[TOP].d = mat[15] - mat[13];
	planes[TOP].normalize();

	planes[BACK].a = mat[ 3] - mat[ 2];
	planes[BACK].b = mat[ 7] - mat[ 6];
	planes[BACK].c = mat[11] - mat[10];
	planes[BACK].d = mat[15] - mat[14];
	planes[BACK].normalize();

	planes[FRONT].a = mat[ 3] + mat[ 2];
	planes[FRONT].b = mat[ 7] + mat[ 6];
	planes[FRONT].c = mat[11] + mat[10];
	planes[FRONT].d = mat[15] + mat[14];
	planes[FRONT].normalize();

}

bool Frustum::contains(const Vec3D &v) const
{
	for (int i=0; i<6; i++) {
		if ((planes[i].a*v.x + planes[i].b*v.y + planes[i].c*v.z + planes[i].d) <= 0) {
            return false;
		}
	}
    return true;
}

bool Frustum::intersects(const Vec3D &v1, const Vec3D &v2) const
{
	Vec3D points[8];
	points[0] = Vec3D(v1.x,v1.y,v1.z);
	points[1] = Vec3D(v1.x,v1.y,v2.z);
	points[2] = Vec3D(v1.x,v2.y,v1.z);
	points[3] = Vec3D(v1.x,v2.y,v2.z);
	points[4] = Vec3D(v2.x,v1.y,v1.z);
	points[5] = Vec3D(v2.x,v1.y,v2.z);
	points[6] = Vec3D(v2.x,v2.y,v1.z);
	points[7] = Vec3D(v2.x,v2.y,v2.z);

 	for (int i=0; i<6; i++) {
		int numIn = 0;

		for (int k=0; k<8; k++) {
			if ((planes[i].a*points[k].x + planes[i].b*points[k].y + planes[i].c*points[k].z + planes[i].d) > 0)
			{
				numIn++;
			}
		}

		if (numIn == 0) return false;
	}

	return true;
}


bool Frustum::intersectsSphere(const Vec3D& v, const float rad) const
{
	for(int i = 0; i < 6; ++i) {
		float distance = (planes[i].a*v.x + planes[i].b*v.y + planes[i].c*v.z + planes[i].d);
		if (distance < -rad) return false;
		if (fabs(distance) < rad) return true;
	}
	return true;
}

