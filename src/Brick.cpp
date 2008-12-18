/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2008
 * @author danielb
 */
#include "Brick.h"

Brick::Brick(char* file, int id) : 
    m_brickID(id), 
    m_pixmap(SDL_LoadBMP(file)),
    m_X(0), m_Y(0),
    m_sprite(0)
{
    if(bbc::debug) std::cerr << "Brick::Brick(" << file << "," << id << ")\n";
    
    if( m_pixmap == 0 ) 
    {
        fprintf(stderr, "Couldn't load %s: %s\n", file, SDL_GetError());
    }
    
    SDL_SetColorKey(m_pixmap, SDL_SRCCOLORKEY, SDL_MapRGB(m_pixmap->format, 0xff, 0xff, 0xff));
}

Brick::Brick(KrSprite* sprite, int id) :
    m_brickID(id),
    m_pixmap(0),
    m_X(0), m_Y(0),
    m_sprite(sprite)
{
    if(bbc::debug) std::cerr << "Brick::Brick(" << sprite << "," << id << ")\n";
}

// Copy constructor (Meyers)
// warning "Copy and assignment makes bricks with same ID !! (bug/feature)?"
Brick::Brick(const Brick& br) : m_brickID(br.m_brickID), 
                                m_pixmap( new SDL_Surface(*br.m_pixmap) ),
                                m_X(br.m_X), m_Y(br.m_Y),
                                m_sprite(br.m_sprite)
{
    if(bbc::debug) std::cerr << "Brick::Brick(const Brick& br)\n";
    
    assert( *this == br );
}

// Assignment operator (Meyers)
Brick&
Brick::operator=(const Brick& br)
{
    if(bbc::debug) std::cerr << "Brick::operator=(const Brick& br)\n";
  
    m_brickID = br.m_brickID;

    zap(m_pixmap);
    if( br.m_pixmap )
        m_pixmap = new SDL_Surface(*br.m_pixmap);

    zap(m_sprite);
    if( br.m_sprite )
        m_sprite = new KrSprite(*br.m_sprite);
    
    assert( *this == br );

    return *this;
}

Brick::~Brick()
{
    if(bbc::debug) std::cerr << "Brick::~Brick()\n";

    SDL_FreeSurface(m_pixmap);
}

bool operator==(const Brick& b1, const Brick& b2)
{
    return ( b1.m_brickID == b2.m_brickID );
}

bool operator!=(const Brick& b1, const Brick& b2)
{
    return ( b1.m_brickID != b2.m_brickID );
}

SDL_Surface* 
Brick::getPixmap() const { return m_pixmap; }

KrSprite*
Brick::getSprite() const { return m_sprite; }

unsigned int
Brick::getWidth() const 
{ 
    if(m_pixmap) return m_pixmap->w;
    else if(m_sprite) 
    {
        Rectangle2I rect;
        m_sprite->QueryBoundingBox(&rect);
        return rect.max.x - rect.min.x;
    }
    else return 0;
}

unsigned int
Brick::getHeight() const 
{ 
    if( m_pixmap )
        return m_pixmap->h; 
    else if(m_sprite)
    {
        Rectangle2I rect;
        m_sprite->QueryBoundingBox(&rect);
        return rect.max.y - rect.min.y;
    }
    else return 0;
}

void Brick::setPos( uint x, uint y )
{
    m_X = x;
    m_Y = y;
	if( m_sprite ) { m_sprite->SetPos(x,y); }
}

void Brick::move( uint x, uint y )
{
    m_X += x;
    m_Y += y;
}

void Brick::draw(SDL_Surface* screen)
{
    assert(screen);
    if( !screen )
        return;

    if( m_pixmap )
    {
        SDL_Rect pos; pos.x = m_X; pos.y = m_Y;
        SDL_BlitSurface( m_pixmap, NULL, screen, &pos );
        SDL_UpdateRects( screen, 1, &pos);
    }
}
