
#include "BrainSprite.h"
#include "Brainblast.h"

using namespace grinliz;
using namespace brain;

void BrainSprite::move()
{
    if( !m_speed ) return;

    SetPos( X(), Y()+static_cast<int>(m_speed*m_acc*m_time++) );
    
    int next_delta = static_cast<int>(m_speed*m_acc*(m_time+1));

    // Make sure that we did not collide, if so we move back to the edge
    Rectangle2I rect;
    QueryBoundingBox(&rect);
    if( (rect.max.y + next_delta) > VIDEOY )
    {
        SetPos( X(), Y() + VIDEOY - rect.max.y -next_delta );
        stop();
    }
}
