
#ifndef FIELD_H
#define FIELD_H

class Field
{
public:

    Field(int width, int height) : 
        m_width(width),
        m_height(height){}
    virtual ~Field() {}

    int width() const { return m_width; }
    int height() const { return m_height; }
    
    KrImage* brickAtIdx(int idx);

protected:

    unsigned int m_width;
    unsigned int m_height;
};

#endif
