#ifndef KYRA_ISOGEN_EDGES_INCLUDED
#define KYRA_ISOGEN_EDGES_INCLUDED

class Edge
{
  public:
	virtual ~Edge()	{}

	int X() { return x; }
	int Y()	{ return y; }
	virtual bool Next() = 0;
	bool NoTouch()	{ return noTouch; }

	void SetNoTouch() { noTouch = true; }

  protected:
	Edge( int x, int y, int yMax )	{
		this->x = x;
		this->y = y;
		this->yMax = yMax;
		noTouch = false;
		
		GLASSERT( this->y <= this->yMax );
	}
	int x, y, yMax;

  private:
	bool noTouch;
};


class IsoEdgeNeg : public Edge
{
  public:
	IsoEdgeNeg( int x, int y, int yMax ) : Edge( x, y, yMax )	{}

	virtual bool Next() {
		y++;
		x -= 2;
		return ( y <= yMax );
	}
};


class IsoEdgePos : public Edge
{
  public:
	IsoEdgePos( int x, int y, int yMax ) : Edge( x, y, yMax )	
	{}

	virtual bool Next() {
		y++;
		x += 2;
		return ( y <= yMax );
	}
};


class VEdge : public Edge
{
  public:
	VEdge( int x, int y, int yMax ) : Edge( x, y, yMax ) {}

	virtual bool Next() {
		y++;
		return ( y <= yMax );
	}
};


// Implements half of a Bresenham line algorithm to render an arbitrary edge.
// Should only be used for internal edges, not the iso edges at seams.
class LineEdge : public Edge
{
  public:
	LineEdge( int x0, int y0, int x1, int y1 ) : Edge( x0, y0, y1 ) 
	{
		deltaX = abs( x1-x0 );
		deltaY = y1-y0;
		GLASSERT( deltaX <= deltaY );
		bias = ( x1 > x0 ) ? 1 : -1;

		error = deltaX*2 - deltaY;
	}

	virtual bool Next() {
		y++;

		// Check to advance the X:
		if ( error >= 0 ) 
		{
			x += bias;
			error += ( deltaX*2 - deltaY*2 );
		}
		else
		{
			error += deltaX*2;
		}
		return ( y <= yMax );
	}

	int deltaX, deltaY, error, bias;
};

#endif 
