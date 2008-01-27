
#include "BrainSprite.h"
#include "Brainblast.h"

using namespace grinliz;
using namespace brain;

void BrainSprite::move()
{
//    if( m_x_speed!=0 && m_y_speed!=0 ) return;

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
            m_x_speed = 0;
        }
    }

    if( m_name == "paprice" )
        fprintf(stderr,"\rv=%f (%i)(%p)(%s)",m_y_speed,m_child!=0,this,m_name.c_str());
}

void BrainSprite::jump()
{
    if( !m_jumping ) 
    {
        m_jumping=true;
        m_y_speed = -20; 
    }
}


void BrainSprite::pickUp(BrainSprite* bs)
{
    if( !m_child )
    {
        bs->DeltaPos(-30,-30);
        m_child = bs;
    }
}

void BrainSprite::drop()
{
    if( m_child ) {
        BrainSprite* clone = Brainblast::instance()->reparentSprite(m_child,0);
        clone->SetPos(X()-30,Y()-30);
        clone->setAcc(m_acc);
        clone->setSpeed(m_x_speed,m_y_speed);
        m_child = 0;
    }
}

KrImNode* BrainSprite::Clone()
{
    BrainSprite* clone = new BrainSprite( SpriteResource(), "cloned" );
 	clone->SetAction( GetAction()->Id() );
 	clone->SetFrame( Frame() );

    // Local variables
    clone->m_x_speed  = this->m_x_speed;
    clone->m_y_speed  = this->m_y_speed;
    clone->m_time     = this->m_time;
    clone->m_acc      = this->m_acc;
    clone->m_jumping  = this->m_jumping;
    clone->m_child    = this->m_child;

	return clone;
}
