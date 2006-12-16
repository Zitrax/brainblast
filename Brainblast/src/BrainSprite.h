
#ifndef BRAINSPRITE_H
#define BRAINSPRITE_H

#include "../../kyra/engine/kyra.h" 

class BrainSprite : public KrSprite
{
public:
    BrainSprite(KrSpriteResource* res) : 
        KrSprite(res), 
        m_speed(1),
        m_time(0),
        m_acc(1.01){}
    virtual ~BrainSprite(){}

    int speed() const { return m_speed; }
    void setSpeed(int speed) { m_speed=speed;}
    void stop() { m_speed=0;}

    int time() const { return m_time; }
    void setTime(int time) { m_time=time; }

    double acc() const { return m_acc; }
    void setAcc(double acc) { m_acc=acc; }

    void move();

private:

    int m_speed;
    int m_time;
    double m_acc;
};

#endif
