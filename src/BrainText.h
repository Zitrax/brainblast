/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2009
 * @author Daniel Bengtsson
 */

#ifndef BRAINTEXT_H
#define BRAINTEXT_H

#include <string>
#include <wkyra.h>

/**
 * This class should handle all writing of
 * of text to the screen such as menus and
 * scores and other information.
 */
class BrainText
{
public:
    BrainText();
    ~BrainText();

    enum Box
    {
		UNKNOWN,
        TOP_LEFT,
        TOP_RIGHT,
        TOP_CENTER,
        CENTER,
        HIGH_SCORE
    };

	// Must be called before use
	// to set up the boxes and load the
	// needed fonts.
	bool init(KrEngine& engine, int width, int height);

	/**
	 * Load a font from a bitmap
	 */
	KrFontResource* loadFont(const char* file, int glyphs);

    void write(enum Box box, const std::string& text, int line);
	void read(enum Box box, std::string* text, int line);
    void clear(enum Box box);
    void clearAll();

#ifdef _DEBUG
	KrFontResource* getScoreFont() const { return m_score_font; }
#endif // _DEBUG

private:

    BrainText(const BrainText& bb);
    BrainText& operator=(const BrainText& bb);

	KrTextBox* getBox(enum Box box) const;

	KrFontResource* m_font;
	KrFontResource* m_score_font;
	KrFontResource* m_title_font;

	KrTextBox* m_left_score_text_box;
	KrTextBox* m_right_score_text_box;
	KrTextBox* m_center_text_box;
	KrTextBox* m_top_center_text_box;
	KrTextBox* m_high_score_text_box;
};

#endif // BRAINTEXT_H
