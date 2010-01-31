/*
Copyright (c) 2000-2003 Lee Thomason (www.grinninglizard.com)

Grinning Lizard Utilities. Note that software that uses the 
utility package (including Lilith3D and Kyra) have more restrictive
licences which applies to code outside of the utility package.


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

#ifndef GlFixed_MATH_INCLUDED
#define GlFixed_MATH_INCLUDED

#include <limits.h>
#include "../../grinliz/gltypes.h"
#include "../../grinliz/gldebug.h"

#define GlFixed_0 		( 0 )		// Help the compliler - use 1<<16 instead of a const.
#define GlFixed_1 		( 1<<16 )	// Help the compliler - use 1<<16 instead of a const.

inline S32		GlIntToFixed( int x )			{ return x << 16; }
inline S32		GlDoubleToFixed( double x )		{ return S32( x * 65536.0 + 0.5); }
inline double	GlFixedToDouble( S32 f )		{ return double( f ) / 65536.0; }
inline float	GlFixedToFloat( S32 f )			{ return float( f ) / 65536.0f; }
inline int      GlFixedToInt( S32 f )			{ return f >> 16; }
inline int		GlFixedToIntRound( S32 f )		{ return ( f + 0x0800 ) >> 16; }
inline int		GlFixedToIntRoundUp( S32 f )	{ return ( f + 0xffff ) >> 16; }


inline S32 GlFixedMult( S32 _x, S32 _y )
{
	#ifdef VALIDATE_DEBUG
		S64 x = _x;
		S64 y = _y;
		S64 ret = ( ( x * y ) >> 16 );
		GLASSERT( ret <= INT_MAX && ret >= INT_MIN );
		return S32( ret );
	#else
		return (S32) ( ( S64( _x ) * S64( _y ) ) >> 16 );
	#endif
}


inline S32 GlFixedDiv( S32 _x, S32 _y )
{
	#ifdef VALIDATE_DEBUG
		S64 x = ( S64( _x ) << 16 );
		S64 y = _y;

		S64 ret =  x / y;
		GLASSERT( ret <= INT_MAX && ret >= INT_MIN );
		return S32( ret );
	#else
		return S32( ( S64( _x ) << 16 ) / S64( _y ) );
	#endif
}


/**	A fixed math class. Uses 16.16 fixed point numbers.
*/
class GlFixed
{
  public:
	GlFixed()									{}

	GlFixed( const GlFixed &x )					{ v = x.v; }
	GlFixed( int x )							{ v = GlIntToFixed( x ); }
	GlFixed( const double x )					{ v = GlDoubleToFixed( x ); }

	#ifdef DEBUG
	void DebugDump()		{ GLOUTPUT(( "fixed=%.2f\n", GlFixedToDouble( v ) )); }
	#endif


	// This could be done with overloaded operators, but I dislike overloaded
	// operators for casting because I think they are confusing when they
	// are used. -lee

	int    ToInt()	const					{ return GlFixedToInt( v ); }
	int    ToIntRoundUp()	const			{ return GlFixedToIntRoundUp( v ); }
	int    ToIntRound() const				{ return GlFixedToIntRound( v ); }
	double ToDouble() const					{ return GlFixedToDouble( v ); }
	float  ToFloat() const					{ return GlFixedToFloat( v ); }

	GlFixed Abs()							{ GlFixed t; ( v < 0 ) ? t.v = -v : t.v = v; return t; }

	GlFixed& operator = (const GlFixed &x)		{ v = x.v;          		return *this; }
	GlFixed& operator = (const int x)			{ v = GlIntToFixed( x );    return *this; }
	GlFixed& operator = (const double x)		{ v = GlDoubleToFixed( x ); return *this; }

	GlFixed& operator +=  (const GlFixed x)		{ v += x.v;          return *this; }
	GlFixed& operator +=  (const int x)			{ v += GlIntToFixed(x);    return *this; }
	GlFixed& operator +=  (const double x)		{ v += GlDoubleToFixed(x);    return *this; }

	GlFixed& operator -=  (const GlFixed x)		{ v -= x.v;          return *this; }
	GlFixed& operator -=  (const int x)			{ v -= GlIntToFixed(x);    return *this; }
	GlFixed& operator -=  (const double x)		{ v -= GlDoubleToFixed(x);    return *this; }

	GlFixed& operator *=  (const GlFixed x)		{ v = GlFixedMult(v, x.v);       return *this; }
	GlFixed& operator *=  (const int x)			{ v *= x;                     return *this; }
	GlFixed& operator *=  (const double x)		{ v = GlDoubleToFixed(GlFixedToDouble(v) * x);  return *this; }

	GlFixed& operator /=  (const GlFixed x)		{ v = GlFixedDiv(v, x.v);						return *this; }
	GlFixed& operator /=  (const int x)			{ v /= x;										return *this; }
	GlFixed& operator /=  (const double x)		{ v = GlFixedDiv( v, GlDoubleToFixed(x) );			return *this; }

	GlFixed& operator <<= (const int x)			{ v <<= x;           return *this; }
	GlFixed& operator >>= (const int x)			{ v >>= x;           return *this; }

	GlFixed& operator ++ ()						{ v += GlFixed_1;    return *this; }
	GlFixed& operator -- ()						{ v -= GlFixed_1;    return *this; }

	GlFixed operator ++ (int)						{ GlFixed t;  t.v = v;   v += GlFixed_1;  return t; }
	GlFixed operator -- (int)						{ GlFixed t;  t.v = v;   v -= GlFixed_1;  return t; }

	// Negation.
	GlFixed operator - () const					{ GlFixed t;  t.v = -v;  return t; }

	inline friend GlFixed operator +  (const GlFixed x, const GlFixed y)    { GlFixed t;  t.v = x.v + y.v; return t; }
	inline friend GlFixed operator +  (const int x,	   const GlFixed y)		{ GlFixed t;  t.v = GlIntToFixed(x) + y.v; return t; }
	inline friend GlFixed operator +  (const GlFixed x, const int y      )  { GlFixed t;  t.v = x.v + GlIntToFixed(y); return t; }

	inline friend GlFixed operator -  (const GlFixed x, const GlFixed y)    { GlFixed t;  t.v = x.v - y.v;        return t; }
	inline friend GlFixed operator -  (const int       x, const GlFixed y)    { GlFixed t;  t.v = GlIntToFixed(x) - y.v;        return t; }
	inline friend GlFixed operator -  (const GlFixed x, const int       y)    { GlFixed t;  t.v = x.v - GlIntToFixed(y);        return t; }
	inline friend GlFixed operator -  (const double x,    const GlFixed y)    { GlFixed t;  t.v = GlDoubleToFixed(x) - y.v;     return t; }
	inline friend GlFixed operator -  (const GlFixed x, const double    y)    { GlFixed t;  t.v = x.v - GlDoubleToFixed(y);     return t; }

	inline friend GlFixed operator *  (const GlFixed x, const GlFixed y)    { GlFixed t;  t.v = GlFixedMult(x.v, y.v);        return t; }
	inline friend GlFixed operator *  (const int x,       const GlFixed y)    { GlFixed t;  t.v = GlFixedMult(GlIntToFixed(x), y.v);        return t; }
	inline friend GlFixed operator *  (const GlFixed x, const int y)			 { GlFixed t;  t.v = GlFixedMult(x.v, GlIntToFixed(y));		return t; }
	inline friend GlFixed operator *  (const double x,    const GlFixed y)    { GlFixed t;  t.v = GlFixedMult(GlDoubleToFixed(x), y.v);        return t; }
	inline friend GlFixed operator *  (const GlFixed x, const double y)		 { GlFixed t;  t.v = GlFixedMult(x.v, GlDoubleToFixed(y));		return t; }

	inline friend GlFixed operator /  (const GlFixed x, const GlFixed y)    { GlFixed t;  t.v = GlFixedDiv(x.v, y.v);           return t; }
	inline friend GlFixed operator /  (const int       x, const GlFixed y)    { GlFixed t;  t.v = GlFixedDiv(GlIntToFixed(x), y.v);           return t; }
	inline friend GlFixed operator /  (const GlFixed x, const int       y)    { GlFixed t;  t.v = GlFixedDiv(x.v, GlIntToFixed(y));           return t; }

	inline friend GlFixed operator << (const GlFixed x, const int y)    { GlFixed t;  t.v = x.v << y;    return t; }
	inline friend GlFixed operator >> (const GlFixed x, const int y)    { GlFixed t;  t.v = x.v >> y;    return t; }

	inline friend bool operator == (const GlFixed x, const GlFixed y)    { return (x.v == y.v);       }
	inline friend bool operator == (const int x,       const GlFixed y)    { return (GlIntToFixed(x) == y.v);       }
	inline friend bool operator == (const GlFixed x, const int y)			{ return (x.v == GlIntToFixed(y));       }

	inline friend bool operator != (const GlFixed x, const GlFixed y)    { return (x.v != y.v);       }
	inline friend bool operator != (const int x,		  const GlFixed y)    { return (GlIntToFixed(x) != y.v);       }
	inline friend bool operator != (const GlFixed x, const int y)			{ return (x.v !=GlIntToFixed(y));       }

	inline friend bool operator <  (const GlFixed x, const GlFixed y)   { return (x.v < y.v);        }
	inline friend bool operator <  (const int x,	 const GlFixed y)   { return (GlIntToFixed(x) < y.v);        }
	inline friend bool operator <  (const GlFixed x, const int y)		{ return (x.v < GlIntToFixed(y) );        }

	inline friend bool operator >  (const GlFixed x, const GlFixed y)   { return (x.v > y.v);        }
	inline friend bool operator >  (const int x,	 const GlFixed y)   { return (GlIntToFixed(x) > y.v);        }
	inline friend bool operator >  (const GlFixed x, const int y)		{ return (x.v > GlIntToFixed(y));        }

	inline friend bool operator <= (const GlFixed x, const GlFixed y)    { return (x.v <= y.v);       }
	inline friend bool operator <= (const int x,		  const GlFixed y)    { return (GlIntToFixed(x) <= y.v);       }
	inline friend bool operator <= (const GlFixed x, const int y)			{ return (x.v <= GlIntToFixed(y));       }

	inline friend bool operator >= (const GlFixed x, const GlFixed y)    { return (x.v >= y.v);       }
	inline friend bool operator >= (const int x,		  const GlFixed y)    { return (GlIntToFixed(x) >= y.v);       }
	inline friend bool operator >= (const GlFixed x, const int y)			{ return (x.v >= GlIntToFixed(y));       }

    S32 v;
};


#endif
