
#pragma once

#include "common.h"

#include "KBScreen.hpp"
#include "KBTextBox.hpp"
#include "KBListBox.hpp"
#include "NXConstStringList.hpp"

#include "NXUnixPacketSocket.hpp"

struct KBDialog
{
    KBScreen           * _screen     = nullptr;
    NXCanvas           * _canvas     = nullptr;
    NXUnixPacketSocket * _events     = nullptr;
private:

    U8 curr_choice;        
    U8 last_choice;        

    NXColor fg;
    NXColor bg;

public:
    KBDialog(KBScreen * screen, NXUnixPacketSocket * evts)
    {
        _screen = screen;
        _canvas = screen->canvas();
        _events = evts;
    }

    ~KBDialog()
    {
        // TODO: fix leaks
    }

    int confirm(const char * message)
    {
        U8 timer = 0;

        draw_bkgnd();

        _events->send_msg("sysbeep");
        _events->send_msg("delaysleep");

        NXRect rc_left  = _screen->text_rect.inset(1);
        NXRect rc_right = rc_left;

        // Need a split rect routine
        rc_left.size.w -= 5;  // Shrink left

        // Then take remainder for rc_right
        rc_right.size.w = 5;
        rc_right.origin.x = rc_left.origin.x + rc_left.size.w;

        KBTextBox text_box(_screen, message);
        text_box._text_rect = rc_left;
        text_box.draw();

        KBListBox list_box(_screen, _events);
        list_box._text_rect = rc_right;

        NXConstStringList choices;
        const char * menu_strs[] = {
            " no  ",
            " yes ",
            NULL
        };
        choices.set_list(menu_strs);

        int choice = list_box.go(&choices, false, 30);

        return choice;
    }

    void draw_bkgnd()
    {
        _canvas->clear();

        // Text Rect Grid
        _canvas->draw_font_rect(&_screen->font, _screen->text_rect);

        NXPoint pt = _screen->screen_rect.origin;
        pt.y += (_screen->text_rect.size.h) * _screen->font.char_size.h;

        pt.x += ( 2) * _screen->font.char_size.w;
        _canvas->draw_font(&_screen->font, pt, "no");

        pt.x += (14) * _screen->font.char_size.w;
        _canvas->draw_font(&_screen->font, pt, "yes");
    }

};
