
#pragma once

#include "common.h"

#include "NXCanvas.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "stb_image.h"

#include "NXUnixPacketSocket.hpp"

struct KBDialog
{
    NXCanvas           *  canvas     = nullptr;
    NXUnixPacketSocket * _events     = nullptr;
private:
    NXRect      * screen_rect = nullptr;
    NXFontAtlas * font        = nullptr;
    NXFontAtlas * bold_font   = nullptr;

    U8 curr_choice;        
    U8 last_choice;        

    NXColor fg;
    NXColor bg;

public:
    KBDialog(NXCanvas * cnvs, NXUnixPacketSocket * evts)
    {
        canvas = cnvs;
        _events = evts;

        // https://en.wikipedia.org/wiki/Video_Graphics_Array#Color_palette
        bg = NXColor{   0,   0, 170, 255}; // Blue
        fg = NXColor{ 255, 255,  85, 255}; // Yellow
        canvas->state.bg = bg;
        canvas->state.fg = fg;
        canvas->state.mono_color_txform = true;

        screen_rect = &cnvs->bitmap.rect;

        // Load image with 1 byte per pixel
        int stb_width, stb_height, stb_bpp;
        //unsigned char* font = stbi_load( "5271font.png", &stb_width, &stb_height, &stb_bpp, 1 );

        // Wyse Font
        char font_path[] = "/boot/KeyBox/wy700font.png";
        unsigned char* font_stb_bmp = stbi_load(font_path, &stb_width, &stb_height, &stb_bpp, 1 );
        if (!font_stb_bmp) {
            fprintf(stderr, "missing font file %s\n", font_path);
            exit(1);
        }

        NXBitmap * font_bmp    = nullptr;
        int16_t width  = stb_width;
        int16_t height = stb_height;
        int8_t  chans  = stb_bpp;
        font_bmp =  new NXBitmap { (uint8_t *)font_stb_bmp, {0, 0, width, height}, NXColorChan::GREY1 };
        //printf("font: %d x %d\n", width, height);

        // Invert the Font Atlas
        {
            U8 * tmp_mem = (U8 *)malloc(width * height);
            memset(tmp_mem, 0xff, width * height);
            NXBitmap tmp_bmp =  { tmp_mem, {0, 0, width, height}, NXColorChan::GREY1 };

            NXCanvasROP tmp_rop = canvas->state.rop;
            canvas->state.rop = NXCanvasROP::XOR;

            NXBlit::blit(&tmp_bmp, &font_bmp->rect, // src
                         font_bmp, &font_bmp->rect, // dst
                         &canvas->state);

            canvas->state.rop = tmp_rop;
            free(tmp_mem);
        }

        font = new NXFontAtlas();
        font->atlas = font_bmp;
        font->rect  = { { 0, 128 }, { 512, 128 } };
        font->size  = { 32, 8 };
        font->init();

        bold_font = new NXFontAtlas();
        bold_font->atlas = font_bmp;
        bold_font->rect  = { { 0, 0 }, { 512, 128 } };
        bold_font->size  = { 32, 8 };
        bold_font->init();

        if (false)
            fprintf(stderr, "font char: %d x %d\n", bold_font->char_size.w, bold_font->char_size.h);
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
        canvas->fill_rect(screen_rect, canvas->state.bg);

        // Text Rect Grid
        canvas->draw_font_rect(font, NXRect{{0,0},{20,15}});

        NXPoint pt = screen_rect->origin;
        pt.y += (14) * font->char_size.h;

        pt.x += ( 2) * font->char_size.w;
        canvas->draw_font(font, pt, "no");

        pt.x += (14) * font->char_size.w;
        canvas->draw_font(font, pt, "yes");
    }

    void draw_message(const char * message)
    {
        U8 index = 0;

        NXPoint pt = {0, 0};

        pt.x = ( 2) * font->char_size.w;
        pt.y = ( 3) * font->char_size.h;

        canvas->draw_font(font, pt, message);
    }
};
