
#pragma once

#include "common.h"

#include "KBScreen.hpp"

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

        draw_message(message);

        _screen->flush();

        _events->send_msg("sysbeep");
        _events->send_msg("delaysleep");

        // event loop
        while (true)
        {
            _events->send_msg("wait");
            auto msg = _events->recv_msg();

            if (false)
                fprintf(stderr, "msg %s\n", msg._str);

            if (msg == "b0")
            {
                break;
            }
            else
            if (msg == "b1")
            {
                break;
            }
            else
            if (msg == "b2")
            {
                break;
            }
            else
            if (msg == "b3")
            {
                return 1;
            }
            else
            if (msg == "wake")
            {
            }
            else
            if (msg == "tick")
            {
                timer++;
                if (timer > 30)
                    break;
            }
            else
            {
                fprintf(stderr, "KBDialog unhandled msg\n");
            }
        } // event loop
        
        // Return cancel
        return 0;
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

    void draw_message(const char * message)
    {
        U8 index = 0;

        NXPoint pt = {0, 0};

        pt.x = ( 2) * _screen->font.char_size.w;
        pt.y = ( 2) * _screen->font.char_size.h;

        _canvas->draw_font(&_screen->font, pt, message);
    }
};
