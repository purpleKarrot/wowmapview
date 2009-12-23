#ifndef MATRIX_H
#define MATRIX_H

#include <cmath>
#include "vec3d.h"
#include "quaternion.h"

class Matrix {
public:
	float m[4][4];

	Matrix()
	{
	}

	Matrix(const Matrix& p)
	{
        for (size_t j=0; j<4; j++) {
        	for (size_t i=0; i<4; i++) {
        		m[j][i] = p.m[j][i];
			}
		}
	}

	Matrix& operator= (const Matrix& p)
	{
        for (size_t j=0; j<4; j++) {
        	for (size_t i=0; i<4; i++) {
        		m[j][i] = p.m[j][i];
			}
		}
		return *this;
	}


	void zero()
	{
        for (size_t j=0; j<4; j++) {
        	for (size_t i=0; i<4; i++) {
        		m[j][i] = 0;
			}
		}
	}

	void unit()
	{
		zero();
        m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0f;
	}

	void translation(const Vec3D& tr)
	{
		/*
			100#
			010#
			001#
			0001
		*/
		unit();
		m[0][3]=tr.x;
		m[1][3]=tr.y;
		m[2][3]=tr.z;
	}

	static const Matrix newTranslation(const Vec3D& tr)
	{
		Matrix t;
		t.translation(tr);
		return t;
	}

	void scale(const Vec3D& sc)
	{
		/*
			#000
			0#00
			00#0
			0001
		*/
		zero();
		m[0][0]=sc.x;
		m[1][1]=sc.y;
		m[2][2]=sc.z;
		m[3][3]=1.0f;
	}

	static const Matrix newScale(const Vec3D& sc)
	{
		Matrix t;
		t.scale(sc);
		return t;
	}

	void quaternionRotate(const Quaternion& q)
	{
		/*
			###0
			###0
			###0
			0001
		*/
		m[0][0] = 1.0f - 2.0f * q.y * q.y - 2.0f * q.z * q.z;
		m[0][1] = 2.0f * q.x * q.y + 2.0f * q.w * q.z;
		m[0][2] = 2.0f * q.x * q.z - 2.0f * q.w * q.y;
		m[1][0] = 2.0f * q.x * q.y - 2.0f * q.w * q.z;
		m[1][1] = 1.0f - 2.0f * q.x * q.x - 2.0f * q.z * q.z;
		m[1][2] = 2.0f * q.y * q.z + 2.0f * q.w * q.x;
		m[2][0] = 2.0f * q.x * q.z + 2.0f * q.w * q.y;
		m[2][1] = 2.0f * q.y * q.z - 2.0f * q.w * q.x;
		m[2][2] = 1.0f - 2.0f * q.x * q.x - 2.0f * q.y * q.y;
		m[0][3] = m[1][3] = m[2][3] = m[3][0] = m[3][1] = m[3][2] = 0;
		m[3][3] = 1.0f;
	}

	static const Matrix newQuatRotate(const Quaternion& qr)
	{
		Matrix t;
		t.quaternionRotate(qr);
		return t;
	}

	Vec3D operator* (const Vec3D& v) const
	{
		Vec3D o;
		o.x = m[0][0]*v.x + m[0][1]*v.y + m[0][2]*v.z + m[0][3];
		o.y = m[1][0]*v.x + m[1][1]*v.y + m[1][2]*v.z + m[1][3];
		o.z = m[2][0]*v.x + m[2][1]*v.y + m[2][2]*v.z + m[2][3];
        return o;
	}

	Matrix operator* (const Matrix& p) const
	{
		Matrix o;
		o.m[0][0] = m[0][0]*p.m[0][0] + m[0][1]*p.m[1][0] + m[0][2]*p.m[2][0] + m[0][3]*p.m[3][0];
		o.m[0][1] = m[0][0]*p.m[0][1] + m[0][1]*p.m[1][1] + m[0][2]*p.m[2][1] + m[0][3]*p.m[3][1];
		o.m[0][2] = m[0][0]*p.m[0][2] + m[0][1]*p.m[1][2] + m[0][2]*p.m[2][2] + m[0][3]*p.m[3][2];
		o.m[0][3] = m[0][0]*p.m[0][3] + m[0][1]*p.m[1][3] + m[0][2]*p.m[2][3] + m[0][3]*p.m[3][3];

		o.m[1][0] = m[1][0]*p.m[0][0] + m[1][1]*p.m[1][0] + m[1][2]*p.m[2][0] + m[1][3]*p.m[3][0];
		o.m[1][1] = m[1][0]*p.m[0][1] + m[1][1]*p.m[1][1] + m[1][2]*p.m[2][1] + m[1][3]*p.m[3][1];
		o.m[1][2] = m[1][0]*p.m[0][2] + m[1][1]*p.m[1][2] + m[1][2]*p.m[2][2] + m[1][3]*p.m[3][2];
		o.m[1][3] = m[1][0]*p.m[0][3] + m[1][1]*p.m[1][3] + m[1][2]*p.m[2][3] + m[1][3]*p.m[3][3];

		o.m[2][0] = m[2][0]*p.m[0][0] + m[2][1]*p.m[1][0] + m[2][2]*p.m[2][0] + m[2][3]*p.m[3][0];
		o.m[2][1] = m[2][0]*p.m[0][1] + m[2][1]*p.m[1][1] + m[2][2]*p.m[2][1] + m[2][3]*p.m[3][1];
		o.m[2][2] = m[2][0]*p.m[0][2] + m[2][1]*p.m[1][2] + m[2][2]*p.m[2][2] + m[2][3]*p.m[3][2];
		o.m[2][3] = m[2][0]*p.m[0][3] + m[2][1]*p.m[1][3] + m[2][2]*p.m[2][3] + m[2][3]*p.m[3][3];

		o.m[3][0] = m[3][0]*p.m[0][0] + m[3][1]*p.m[1][0] + m[3][2]*p.m[2][0] + m[3][3]*p.m[3][0];
		o.m[3][1] = m[3][0]*p.m[0][1] + m[3][1]*p.m[1][1] + m[3][2]*p.m[2][1] + m[3][3]*p.m[3][1];
		o.m[3][2] = m[3][0]*p.m[0][2] + m[3][1]*p.m[1][2] + m[3][2]*p.m[2][2] + m[3][3]*p.m[3][2];
		o.m[3][3] = m[3][0]*p.m[0][3] + m[3][1]*p.m[1][3] + m[3][2]*p.m[2][3] + m[3][3]*p.m[3][3];

		return o;
	}

	float determinant() const
	{
		#define SUB(a,b) (m[2][a]*m[3][b] - m[3][a]*m[2][b])
		return
			 m[0][0] * (m[1][1]*SUB(2,3) - m[1][2]*SUB(1,3) + m[1][3]*SUB(1,2))
			-m[0][1] * (m[1][0]*SUB(2,3) - m[1][2]*SUB(0,3) + m[1][3]*SUB(0,2))
			+m[0][2] * (m[1][0]*SUB(1,3) - m[1][1]*SUB(0,3) + m[1][3]*SUB(0,1))
			-m[0][3] * (m[1][0]*SUB(1,2) - m[1][1]*SUB(0,2) + m[1][2]*SUB(0,1));
		#undef SUB
	}

	const float minor(size_t x, size_t y) const
	{
		float s[3][3];
		for (size_t j=0, v=0; j<4; j++) {
			if (j==y) continue;
			for (size_t i=0, u=0; i<4; i++) {
				if (i!=x) {
					s[v][u++] = m[j][i];
				}
			}
			v++;
		}
		#define SUB(a,b) (s[1][a]*s[2][b] - s[2][a]*s[1][b])
		return s[0][0] * SUB(1,2) - s[0][1] * SUB(0,2) + s[0][2] * SUB(0,1);
		#undef SUB
	}
	
	const Matrix adjoint() const
	{
		Matrix a;
		for (size_t j=0; j<4; j++) {
			for (size_t i=0; i<4; i++) {
				a.m[i][j] = (((i+j)&1)?-1.0f:1.0f) * minor(i,j);
			}
		}
		return a;
	}
	
	void invert()
	{
		Matrix adj = this->adjoint();
		float invdet = 1.0f / this->determinant();
        for (size_t j=0; j<4; j++) {
        	for (size_t i=0; i<4; i++) {
				m[j][i] = adj.m[j][i] * invdet;
			}
		}
	}

	void transpose()
	{
        for (size_t j=1; j<4; j++) {
        	for (size_t i=0; i<j; i++) {
				float f = m[j][i];
				m[j][i] = m[i][j];
				m[i][j] = f;
			}
		}
	}

	Matrix& operator*= (const Matrix& p)
	{
		return *this = this->operator*(p);
	}

	operator float*()
	{
		return (float*)this;
	}

};


#endif

