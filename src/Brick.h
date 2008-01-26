/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2008
 * @author Daniel Bengtsson
 */

#ifndef BRICK_H
#define BRICK_H

#include <wSDL.h>

// Seems like Kyra has to be included before bbc
#include <wkyra.h> 

#include "bbc.h"
#include <stdlib.h>

using namespace grinliz;

/*! This class will handle the bricks (small icons) that
  the player should put in their correct place */
class Brick
{
public:

    // Con-/De- structors

    /**
     * Create a brick with a pixmap
     */
    Brick(char* file, int id);

    /**
     * Create a brick with a kyra sprite 
     */
    Brick(KrSprite* sprite, int id);

    Brick(const Brick& br);
    ~Brick();

    // Operators
    Brick& operator=(const Brick& br);

    SDL_Surface* getPixmap() const;
    KrSprite*    getSprite() const;

    unsigned int getWidth() const;
    unsigned int getHeight() const;

    void move( uint dx, uint dy );
    void setPos( uint x, uint y );
    
    void draw(SDL_Surface* screen);

    friend bool operator==(const Brick& b1, const Brick& b2);
    friend bool operator!=(const Brick& b1, const Brick& b2);

private:

    int m_brickID;
    SDL_Surface* m_pixmap;
    uint m_X, m_Y;
    KrSprite* m_sprite;
};

#endif
