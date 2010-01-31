/*
Copyright (c) 2000-2003 Lee Thomason (www.grinninglizard.com)
Grinning Lizard Utilities.

This software is provided 'as-is', without any express or implied 
warranty. In no event will the authors be held liable for any 
damages arising from the use of this software.

Permission is granted to anyone to use this software for any 
purpose, including commercial applications, and to alter it and 
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must 
not claim that you wrote the original software. If you use this 
software in a product, an acknowledgment in the product documentation 
would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and 
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source 
distribution.
*/


#ifndef GRINLIZ_MATRIX_DEFINED
#define GRINLIZ_MATRIX_DEFINED

#include "gldebug.h"
#include "glvector.h"

namespace grinliz {

/** A 3D homogenous matrix. Although the bottom row is typically 0,0,0,1 this
	implementation uses the full 4x4 computation.

	It uses the (unfortutate) OpenGL layout:
	@verbatim
       (  m[0]   m[4]    m[8]   m[12] )    ( v[0])
        | m[1]   m[5]    m[9]   m[13] |    | v[1]|
 M(v) = | m[2]   m[6]   m[10]   m[14] |  x | v[2]|
       (  m[3]   m[7]   m[11]   m[15] )    ( v[3])
	@endverbatim
*/
class Matrix4
{
  public:
	inline static int INDEX( int row, int col ) { return col*4+row; }

	Matrix4()				{	SetIdentity();	}
	void SetIdentity()		{	x[0] = x[5] = x[10] = x[15] = 1.0f;
								x[1] = x[2] = x[3] = x[4] = x[6] = x[7] = x[8] = x[9] = x[11] = x[12] = x[13] = x[14] = 0.0f; 
							}
	void SetTranslation( float _x, float _y, float _z )
							{	x[12] = _x;	x[13] = _y;	x[14] = _z;	}
	void SetTranslation( const Vector3F& vec )
							{   x[12] = vec.x;	x[13] = vec.y;	x[14] = vec.z;	}

	void SetXRotation( float thetaDegree );
	void SetYRotation( float thetaDegree );
	void SetZRotation( float thetaDegree );

	void SetScale( float scale )	{ x[0] = x[5] = x[10] = scale; }

	void SetAxisAngle( const Vector3F& axis, float angle );

	void StripTrans()		{	x[12] = x[13] = x[14] = 0.0f;
								x[15] = 1.0f;
							}

	// Is this probably a rotation matrix?
	bool IsRotation() const;

	void Row( unsigned i, Vector3F *row ) const	{ row->x=x[INDEX(i,0)]; row->y=x[INDEX(i,1)]; row->z=x[INDEX(i,2)]; }
	void Col( unsigned i, Vector3F *col ) const	{ col->x=x[INDEX(0,i)]; col->y=x[INDEX(1,i)]; col->z=x[INDEX(2,i)]; }

	void Transpose( Matrix4* transpose ) const;
	float Determinant() const;
	void Adjoint( Matrix4* adjoint ) const;
	void Invert( Matrix4* inverse ) const;
	void Cofactor( Matrix4* cofactor ) const;
	float SubDeterminant(int excludeRow, int excludeCol) const;

	void ApplyScalar( float v ) {
		for( int i=0; i<16; ++i )
			x[i] *= v;
	}

	// Assumes this is a homogenous matrix.
	bool IsTranslationOnly() const {
		if ( x[0] == 1.0f && x[5] == 1.0f && x[10] == 1.0f ) {
			if (    x[4] == 0.0f && x[8] == 0.0f && x[9] == 0.0f 
			     && x[1] == 0.0f && x[2] == 0.0f && x[6] == 0.0f ) {
				return true;
			}
		}
		return false;
	}

	friend void MultMatrix4( const Matrix4& a, const Matrix4& b, Matrix4* c );
	friend void MultMatrix4( const Matrix4& a, const Vector3F& b, Vector3F* c );
	
	#ifdef DEBUG
	void Dump( const char* name ) const
	{
		GLOUTPUT(( "Mat%4s: %6.2f %6.2f %6.2f %6.2f\n"
				  "         %6.2f %6.2f %6.2f %6.2f\n"			
				  "         %6.2f %6.2f %6.2f %6.2f\n"			
				  "         %6.2f %6.2f %6.2f %6.2f\n",
				  name,
				  x[0], x[4], x[8], x[12], 
				  x[1], x[5], x[9], x[13], 			
				  x[2], x[6], x[10], x[14], 
				  x[3], x[7], x[11], x[15] ));
	}
	#endif
	
	// Row-Column notation is backwards from x,y regrettably. Very
	// confusing. Just uses array. Increment by one moves down to the next
	// row, so that the next columnt is at +4.
	union
	{
		float x[16];
		struct
		{
			// row-column
			float m11, m21, m31, m41, m12, m22, m32, m42, m13, m23, m33, m43, m14, m24, m34, m44;
		};
	};

	friend Matrix4 operator*( const Matrix4& a, const Matrix4& b )
	{	
		Matrix4 result;
		MultMatrix4( a, b, &result );
		return result;
	}
	friend Vector3F operator*( const Matrix4& a, const Vector3F& b )
	{
		Vector3F result;
		MultMatrix4( a, b, &result );
		return result;
	}

	friend bool Equal( const Matrix4& a, const Matrix4& b, float epsilon = 0.001f )
	{
		for( unsigned i=0; i<16; ++i )
			if ( !grinliz::Equal( a.x[i], b.x[i], epsilon ) )
				return false;
		return true;
	}
};


/** C = AB. Perform the operation in column-major form, meaning a vector as a columns.
	Note the target parameter is the last parameter, although it is more
	comfortable expressed: C = AB
*/
inline void MultMatrix4( const Matrix4& a, const Matrix4& b, Matrix4* c )
{
	// This does not support the target being one of the sources.
	GLASSERT( c != &a && c != &b && &a != &b );

	// The counters are rows and columns of 'c'
	for( int i=0; i<4; ++i ) 
	{
		for( int j=0; j<4; ++j ) 
		{
			// for c:
			//	j increments the row
			//	i increments the column
			*( c->x + i +4*j )	=   a.x[i+0]  * b.x[j*4+0] 
								  + a.x[i+4]  * b.x[j*4+1] 
								  + a.x[i+8]  * b.x[j*4+2] 
								  + a.x[i+12] * b.x[j*4+3];
		}
	}
}


/** w = Mv. Multiply a vector by a matrix. A vector is a column in column-major form.
*/
inline void MultMatrix4( const Matrix4& m, const Vector3F& v, Vector3F* w )
{
	GLASSERT( w != &v );
	for( int i=0; i<3; ++i )			// target row
	{
		*( &w->x + i )	=		m.x[i+0]  * v.x 
							  + m.x[i+4]  * v.y 
							  + m.x[i+8]  * v.z
							  + m.x[i+12];			// assume 1.0
	}
}

//
//// This form is at least odd to use and needs to be fixed FIXME
//inline void MultMatrix4Odd( const Vector3F& b, const Matrix4& a, Vector3F* c )
//{
//	GLASSERT( c != &b );
//
//	for( int i=0; i<3; ++i )			// target row
//	{
//		*( &c->x + i )	=		a.x[i*4+0]  * b.x 
//							  + a.x[i*4+1]  * b.y 
//							  + a.x[i*4+2]  * b.z
//							  + a.x[i*4+3];			// assume 1.0
//	}
//}
//
//

};	// namespace grinliz

#endif
