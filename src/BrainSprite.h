
#ifndef BRAINSPRITE_H
#define BRAINSPRITE_H

#include <wkyra.h>

class BrainSprite : public KrSprite
{
public:
    BrainSprite(KrSpriteResource* res) : 
        KrSprite(res), 
        m_x_speed(0.0),
        m_y_speed(1.0),
        m_time(0),
        m_acc(1.0),
        m_jumping(true),
        m_carrying(false){}
    virtual ~BrainSprite(){}
    
//     int speed() const { return m_speed; }
//     void setSpeed(int speed) { m_speed=speed;}
    void stop() { m_x_speed=0; m_y_speed=0; }
    
    int time() const { return m_time; }
    void setTime(int time) { m_time=time; }
    
    double acc() const { return m_acc; }
    void setAcc(double acc) { m_acc=acc; }
    
    void move();
    
    void left() { SetPos(X()-5,Y()); }
    void right() { SetPos(X()+5,Y()); }

    void jump();

    /**
     * Note that bs must be a child node.
     * You can reparent it with \ref BrainBlast::reparent first.
     */
    void pickUp(BrainSprite* bs);
    void drop();

private:

    double m_x_speed;
    double m_y_speed;

    int m_time;
    double m_acc;

    bool m_jumping;
    bool m_carrying;

};

#endif
