#include "Brick.h"

Brick::Brick(char* file, int id) : m_brickID(id), 
                                   m_pixmap(SDL_LoadBMP(file)),
                                   m_X(0), m_Y(0)
{
    if(bbc::debug) std::cerr << "Brick::Brick()\n";

    if ( m_pixmap == NULL ) {
        fprintf(stderr, "Couldn't load %s: %s\n", file, SDL_GetError());
        exit(0); // Abort program
    }

    SDL_SetColorKey(m_pixmap, SDL_SRCCOLORKEY, SDL_MapRGB(m_pixmap->format, 0xff, 0xff, 0xff));


    if(bbc::debug) std::cerr << "Brick constructor finished\n";
}

// Copy constructor (Meyers)
#warning "Copy and assignment makes bricks with same ID !! (bug/feature)?"
Brick::Brick(const Brick& br) : m_brickID(br.m_brickID), 
                                m_pixmap( new SDL_Surface(*br.m_pixmap) ),
                                m_X(br.m_X), m_Y(br.m_Y)
{
    if(bbc::debug) std::cerr << "Brick::Brick(const Brick& br)\n";

    assert(m_pixmap);
    assert( *this == br );
}

// Assignment operator (Meyers)
Brick&
Brick::operator=(const Brick& br)
{
    if(bbc::debug) std::cerr << "Brick::operator=(const Brick& br)\n";
  
    m_brickID = br.m_brickID;

    delete m_pixmap;
    m_pixmap = new SDL_Surface(*br.m_pixmap);
    assert(m_pixmap);

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

unsigned int
Brick::getWidth() const { return m_pixmap->w; }

unsigned int
Brick::getHeight() const { return m_pixmap->h; }

void Brick::setPos( uint x, uint y )
{
    m_X = x;
    m_Y = y;
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
