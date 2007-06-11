
#include "BrainSprite.h"
#include "Brainblast.h"

using namespace grinliz;
using namespace brain;

void BrainSprite::move()
{
    if( m_x_speed!=0 && m_y_speed!=0 ) return;

    m_time++;

    // 1. Find new position
    SetPos( static_cast<int>(X()+m_x_speed), 
            static_cast<int>(Y()+m_y_speed) );

    if( X() > VIDEOX )
        SetPos(0,Y());
    else if( X() < 0 )
        SetPos( VIDEOX, Y() );

    // 2. Update velocity due to gravity
    m_y_speed+=m_acc;

    // Make sure that we did not collide, if so we move back to the edge
    Rectangle2I rect;
    QueryBoundingBox(&rect);
    if( (rect.max.y + m_y_speed) > VIDEOY )
    {
        SetPos( X(), static_cast<int>(Y() + VIDEOY - rect.max.y - m_y_speed) );
        //stop();
        m_y_speed = -0.5*m_y_speed;
        
        if( fabs(m_y_speed) < 4.0 )
	  {
	    m_jumping = false;
            m_y_speed = 0;
	  }
    }

    fprintf(stderr,"\rv=%f",m_y_speed);
}

void BrainSprite::jump()
{
  if( !m_jumping ) 
    {
      m_jumping=true;
      m_y_speed = -20; 
    }
}
 
