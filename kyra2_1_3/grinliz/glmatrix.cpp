#include "glmatrix.h"
#include "glgeometry.h"

using namespace grinliz;

void grinliz::SinCosDegree( float degreeTheta, float* sinTheta, float* cosTheta )
{
	degreeTheta = NormalizeAngleDegrees( degreeTheta );

	if ( degreeTheta == 0.0f ) {
		*sinTheta = 0.0f;
		*cosTheta = 1.0f;
	}
	else if ( degreeTheta == 90.0f ) {
		*sinTheta = 1.0f;
		*cosTheta = 0.0f;
	}
	else if ( degreeTheta == 180.0f ) {
		*sinTheta = 0.0f;
		*cosTheta = -1.0f;
	}
	else if ( degreeTheta == 270.0f ) {
		*sinTheta = -1.0f;
		*cosTheta = 0.0f;
	}
	else {
		float radian = ToRadian( degreeTheta );
		*sinTheta = sinf( radian );
		*cosTheta = cosf( radian );	
	}
	
}


void Matrix4::SetXRotation( float theta )
{
	float cosTheta, sinTheta;
	SinCosDegree( theta, &sinTheta, &cosTheta );

	// COLUMN 1
	x[0] = 1.0f;
	x[1] = 0.0f;
	x[2] = 0.0f;
	
	// COLUMN 2
	x[4] = 0.0f;
	x[5] = cosTheta;
	x[6] = sinTheta;

	// COLUMN 3
	x[8] = 0.0f;
	x[9] = -sinTheta;
	x[10] = cosTheta;

	// COLUMN 4
	x[12] = 0.0f;
	x[13] = 0.0f;
	x[14] = 0.0f;
}


void Matrix4::SetYRotation( float theta )
{
	float cosTheta, sinTheta;
	SinCosDegree( theta, &sinTheta, &cosTheta );

	// COLUMN 1
	x[0] = cosTheta;
	x[1] = 0.0f;
	x[2] = -sinTheta;
	
	// COLUMN 2
	x[4] = 0.0f;
	x[5] = 1.0f;
	x[6] = 0.0f;

	// COLUMN 3
	x[8] = sinTheta;
	x[9] = 0;
	x[10] = cosTheta;

	// COLUMN 4
	x[12] = 0.0f;
	x[13] = 0.0f;
	x[14] = 0.0f;
}

void Matrix4::SetZRotation( float theta )
{
	float cosTheta, sinTheta;
	SinCosDegree( theta, &sinTheta, &cosTheta );

	// COLUMN 1
	x[0] = cosTheta;
	x[1] = sinTheta;
	x[2] = 0.0f;
	
	// COLUMN 2
	x[4] = -sinTheta;
	x[5] = cosTheta;
	x[6] = 0.0f;

	// COLUMN 3
	x[8] = 0.0f;
	x[9] = 0.0f;
	x[10] = 1.0f;

	// COLUMN 4
	x[12] = 0.0f;
	x[13] = 0.0f;
	x[14] = 0.0f;
}


void Matrix4::SetAxisAngle( const Vector3F& axis, float angle )
{
	angle *= DEG_TO_RAD;
	float c = (float) cos( angle );
	float s = (float) sin( angle );
	float t = 1.0f - c;

	x[0]  = t*axis.x*axis.x + c;
	x[1]  = t*axis.x*axis.y - s*axis.z;
	x[2]  = t*axis.x*axis.z + s*axis.y;
	x[3]  = 0.0f;

	x[4]  = t*axis.x*axis.y + s*axis.z;
	x[5]  = t*axis.y*axis.y + c;
	x[6]  = t*axis.y*axis.z - s*axis.x;
	x[7]  = 0.0f;

	x[8]  = t*axis.x*axis.z - s*axis.y;
	x[9]  = t*axis.y*axis.z + s*axis.x;
	x[10] = t*axis.z*axis.z + c;
	x[11] = 0.0f;

	x[12] = x[13] = x[14] = 0.0f;
	x[15] = 1.0f;
}


bool Matrix4::IsRotation() const
{
	// Check the rows and columns:

	for( int i=0; i<4; ++i )
	{
		float row = 0.0f;
		float col = 0.0f;
		for( int j=0; j<4; ++j )
		{
			row += x[i+j*4]*x[i+j*4];
			col += x[i*4+j]*x[i*4+j];
		}
		if ( !Equal( row, 1.0f, 0.0001f ) )
			return false;
		if ( !Equal( row, 1.0f, 0.0001f ) )
			return false;
	}
	// Should also really check orthogonality.
	return true;
}


void Matrix4::Transpose( Matrix4* transpose ) const
{
  for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
			transpose->x[INDEX(c,r)] = x[INDEX(r,c)];
        }
    }
}

float Matrix4::Determinant() const
{
    // The determinant is the dot product of:
	// the first row and the first row of cofactors
	// which is the first col of the adjoint matrix
	Matrix4 cofactor;
	Cofactor( &cofactor );
	Vector3F rowOfCofactor, rowOfThis;
	cofactor.Row( 0, &rowOfCofactor );
	Row( 0, &rowOfThis );
	return DotProduct( rowOfCofactor, rowOfThis );
}


void Matrix4::Adjoint( Matrix4* adjoint ) const
{
	Matrix4 cofactor;
	Cofactor( &cofactor );
	cofactor.Transpose( adjoint );
}


void Matrix4::Invert( Matrix4* inverse ) const
{
	// The inverse is the adjoint / determinant of adjoint
	Adjoint( inverse );
	float d = inverse->Determinant();
	inverse->ApplyScalar( 1.0f / d );

	#ifdef DEBUG
	Matrix4 result;
	Matrix4 identity;
	MultMatrix4( *this, *inverse, &result );
	GLASSERT( Equal( identity, result, 0.0001f ) );
	#endif
}


void Matrix4::Cofactor( Matrix4* cofactor ) const
{
    int i = 1;

    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            float det = SubDeterminant(r, c);
            cofactor->x[INDEX(r,c)] = i * det;	// i flip flops between 1 and -1
            i = -i;
        }
        i = -i;
    }
}


float Matrix4::SubDeterminant(int excludeRow, int excludeCol) const 
{
    // Compute non-excluded row and column indices
    int row[3];
    int col[3];

    for (int i = 0; i < 3; ++i) {
        row[i] = i;
        col[i] = i;

        if (i >= excludeRow) {
            ++row[i];
        }
        if (i >= excludeCol) {
            ++col[i];
        }
    }

    // Compute the first row of cofactors 
    float cofactor00 = 
      x[INDEX(row[1],col[1])] * x[INDEX(row[2],col[2])] -
      x[INDEX(row[1],col[2])] * x[INDEX(row[2],col[1])];

    float cofactor10 = 
      x[INDEX(row[1],col[2])] * x[INDEX(row[2],col[0])] -
      x[INDEX(row[1],col[0])] * x[INDEX(row[2],col[2])];

    float cofactor20 = 
      x[INDEX(row[1],col[0])] * x[INDEX(row[2],col[1])] -
      x[INDEX(row[1],col[1])] * x[INDEX(row[2],col[0])];

    // Product of the first row and the cofactors along the first row
    return      x[INDEX(row[0],col[0])] * cofactor00 +
				x[INDEX(row[0],col[1])] * cofactor10 +
				x[INDEX(row[0],col[2])] * cofactor20;
}
