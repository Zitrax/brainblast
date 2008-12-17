/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2008
 * @author Daniel Bengtsson
 */

#include "BrainSprite.h"
#include "Brainblast.h"

using namespace grinliz;
using namespace brain;

BrainSprite::~BrainSprite()
{
	if(bbc::debug)std::cout << "BrainSprite destroyed " << m_name << "\n"; 

#ifdef _DEBUG
	if( m_b )
		Brainblast::instance()->engine()->Tree()->DeleteNode(m_b);
#endif // _DEBUG	
}

void BrainSprite::move()
{
    if( m_static ) return;

    // 1. Find new position
    SetPos( static_cast<int>(X()+m_x_speed), 
            static_cast<int>(Y()+m_y_speed) );
//     SetPos( bbc::round(X()+m_x_speed), 
//             bbc::round(Y()+m_y_speed) );


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
// 		cout << "Y=" << Y() <<","<< VIDEOY <<","<< rect.max.y <<","<< m_y_speed << endl;
// 		cout << "Y=" << Y() + VIDEOY - rect.max.y - m_y_speed << endl;
        SetPos( X(), static_cast<int>(Y() + VIDEOY - rect.max.y - m_y_speed) );
        //stop();
        m_y_speed = -0.5*m_y_speed;
        
        if( fabs(m_y_speed) < 4.0 )
        {
            m_jumping = false;
            m_y_speed = 0;
            m_x_speed = 0;
        }
        else
            Brainblast::instance()->playSample(Brainblast::BOUNCE);
    }

//     if( m_name == "wizard" )
//         fprintf(stderr,"\rv=%f (%i)(%p)(%s)",m_y_speed,m_child!=0,this,m_name.c_str());
}

void BrainSprite::jump()
{
    if( !m_jumping ) 
    {
        m_jumping=true;
        m_y_speed = -20; 
    }
}

int BrainSprite::nextBounce()
{
	if( m_next_bounce_cache >= 0 )
	{
#ifdef _DEBUG
		if( m_b )
			m_b->SetPos(m_next_bounce_cache,VIDEOY-10);
#endif // _DEBUG
		
		return m_next_bounce_cache;
	}

	if( m_y_speed == 0 && m_x_speed == 0)
	{
		m_next_bounce_cache = X();
		return m_next_bounce_cache;
	}

	// Just iterate until bounce 
	//( could be made into a function if it turns out to be slow)

	int x  = X();
	int y  = Y();
	int ys = m_y_speed;

	Rectangle2I rect;
	QueryBoundingBox(&rect);
	int yd = rect.max.y - y;

	int i = 0;
	while(true)
	{
		x+=m_x_speed;
		y+=ys;
		
		if(x > VIDEOX) x = 0;
		if(x < 0     ) x = VIDEOX;

		ys+=m_acc;

		if( (y + yd + ys) > VIDEOY ) // bounce or stop
		{
			y = VIDEOY - yd - ys;
			ys = -0.5*ys;

#ifdef _DEBUG
			/*
			 * This debug code draws an 'X' where we expect to bounce
			 */
			if( !m_b )
			{
				m_b = new KrTextBox(Brainblast::instance()->m_score_font,10,10,1);
				m_b->SetTextChar("X",0);
				Brainblast::instance()->engine()->Tree()->AddNode(0,m_b);
				m_b->SetZDepth(20);
			}
			m_b->SetPos(x,VIDEOY-10);
#endif // _DEBUG
			if( fabs(ys) < 4.0 )
			{
				m_next_bounce_cache = x;
				return x;
			}
		}
		++i;
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

BrainSprite* BrainSprite::drop(KrImNode* new_parent)
{
    if( m_child ) {
        BrainSprite* clone = Brainblast::instance()->reparentSprite(m_child,new_parent);
        clone->SetPos(X()-30,Y()-30);
        clone->setAcc(m_acc);
        clone->setSpeed(m_x_speed,m_y_speed);
        m_child = 0;
        return clone;
    }
    return 0;
}

KrImNode* BrainSprite::Clone()
{
    BrainSprite* clone = new BrainSprite( SpriteResource(), "cloned" );
 	clone->SetAction( GetAction()->Id() );
 	clone->SetFrame( Frame() );

    // Local variables
    clone->m_x_speed       = this->m_x_speed;
    clone->m_y_speed       = this->m_y_speed;
    clone->m_acc           = this->m_acc;
    clone->m_jumping       = this->m_jumping;
    clone->m_child         = this->m_child;
    clone->m_creation_time = this->m_creation_time;
    clone->m_temporary     = this->m_temporary;

    clone->SetNodeId(this->NodeId());

	return clone;
}
