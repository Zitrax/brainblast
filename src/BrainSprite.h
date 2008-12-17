/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2008
 * @author Daniel Bengtsson
 */

#ifndef BRAINSPRITE_H
#define BRAINSPRITE_H

#include "bbc.h"
#include <wkyra.h>
#include <string>
#include <iostream>

class BrainSprite : public KrSprite
{
public:
	/**
	 * A temporary sprite will be deleted after a delay
	 */
	BrainSprite(KrSpriteResource* res, std::string name, bool temporary=false) : 
        KrSprite(res), 
        m_name(name),
        m_x_speed(0.0),
        m_y_speed(1.0),
        m_acc(1.0),
        m_jumping(true),
        m_child(0),
		m_creation_time(time(0)),
		m_temporary(temporary)
		,m_static(false)
		,m_next_bounce_cache(-1)
#ifdef _DEBUG
		,m_b(0)
#endif // _DEBUG
		{}

	virtual ~BrainSprite();

    BrainSprite(const BrainSprite&);
    BrainSprite& operator=(const BrainSprite&);
    
    double speedX() const { return m_x_speed; }
    double speedY() const { return m_y_speed; }

	// Whats the point of double here when we cast to int in move() ?
    void setSpeed(double x, double y) { m_x_speed=x; m_y_speed=y; }
    void stop() { m_x_speed=0; m_y_speed=0; }
    
    double acc() const { return m_acc; }
    void setAcc(double acc) { m_acc=acc; }
    
    virtual void move();
    
    virtual void left()  { m_x_speed = -5; }
    virtual void right() { m_x_speed =  5; }

    void jump();

	/** 
	 * Will give the X coordinate of the next bounce 
	 * Can't be const due to Kyras X() and Y() functions.
	 **/
	int nextBounce();

	time_t creationTime() const { return m_creation_time; }
	bool temporary() const { return m_temporary; }
	void setTemporary(bool tmp) { m_temporary=tmp; }
	
	/**
	 * If static this sprite will not move in the move
	 * function.
	 */
	void setStatic(bool stat) { m_static = stat; }

    /**
     * Note that bs must be a child node.
     * You can reparent it with \ref BrainBlast::reparent first.
     */
    void pickUp(BrainSprite* bs);
	/**
	 * If something was dropped, it's returned.
	 */
    BrainSprite* drop(KrImNode* new_parent);
    bool isCarrying() const {return m_child!=0;}
	BrainSprite* carry() const { return m_child; }

    KrImNode* Clone();

private:

    std::string m_name;

    double m_x_speed;
    double m_y_speed;

    double m_acc;

    bool m_jumping;

    BrainSprite* m_child;

	/// Used to measure for how long this sprite has been alive
	time_t m_creation_time;
	/// If true we can delete this sprite after a delay
	bool m_temporary; 

	bool m_static;

	int m_next_bounce_cache;

#ifdef _DEBUG
	KrTextBox* m_b; 
#endif
};

#endif
