/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2008
 * @author Daniel Bengtsson
 */

#ifndef BRAINSPRITE_H
#define BRAINSPRITE_H

#include <wkyra.h>
#include <string>
#include <iostream>

class BrainSprite : public KrSprite
{
public:
BrainSprite(KrSpriteResource* res, std::string name) : 
        KrSprite(res), 
        m_name(name),
        m_x_speed(0.0),
        m_y_speed(1.0),
        m_time(0),
        m_acc(1.0),
        m_jumping(true),
        m_child(0){}
        virtual ~BrainSprite(){std::cout << "BrainSprite destroyed " << m_name << "\n"; }

    BrainSprite(const BrainSprite&);
    BrainSprite& operator=(const BrainSprite&);
    
    double speedX() const { return m_x_speed; }
    double speedY() const { return m_y_speed; }

	// Whats the point of double here when we cast to int in move() ?
    void setSpeed(double x, double y) { m_x_speed=x; m_y_speed=y; }
    void stop() { m_x_speed=0; m_y_speed=0; }
    
    int time() const { return m_time; }
    void setTime(int time) { m_time=time; }
    
    double acc() const { return m_acc; }
    void setAcc(double acc) { m_acc=acc; }
    
    void move();
    
    void left()  { m_x_speed = -5; }
    void right() { m_x_speed =  5; }

    void jump();

    /**
     * Note that bs must be a child node.
     * You can reparent it with \ref BrainBlast::reparent first.
     */
    void pickUp(BrainSprite* bs);
    void drop();
    bool isCarrying() {return m_child!=0;}

    KrImNode* Clone();

private:

    std::string m_name;

    double m_x_speed;
    double m_y_speed;

    int m_time;
    double m_acc;

    bool m_jumping;

    BrainSprite* m_child;

};

#endif
