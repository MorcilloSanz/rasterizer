/**
 * @file rasterizer.h
 * @brief Header-only software rasterizer for C/C++.
 *
 * A small, dependency-light 2D graphics library that draws directly into a
 * CPU-side frame buffer.
 *
 * Usage:
 *  Include this file from a single translation unit. The stb implementations
 *  (STB_IMAGE_IMPLEMENTATION, STB_TRUETYPE_IMPLEMENTATION) are defined here, so
 *  "vendor/stb_image.h" and "vendor/stb_truetype.h" must be available.
 *
 * Notes:
 *  - The coordinate origin (0, 0) is the top-left corner of the buffer.
 *  - Drawing outside the buffer is safe: out-of-bounds pixels are ignored.
 *  - Buffers and sprites own heap memory and must be released by the caller.
 * 
 * Author: Alberto Morcillo Sanz
 * Email: amorcillosanz@gmail.com
 * GitHub: MorcilloSanz
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "vendor/stb_image.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "vendor/stb_truetype.h"

#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifdef __cplusplus
extern "C" {
#endif

enum Channels {
    CHANNEL_GREY = 1,
    CHANNEL_RGB = 3,
    CHANNEL_RGBA = 4
};

typedef enum Channels Channels;

struct FrameBuffer {
    unsigned int width;
    unsigned int height;
    unsigned char* data;
    Channels channels;
};

typedef struct FrameBuffer FrameBuffer;
typedef struct FrameBuffer Sprite;

struct Color {
    float r;
    float g;
    float b;
    float a;
};

typedef struct Color Color;

/**
 * @brief Create a frame buffer object
 * 
 * @param width width of the frame buffer
 * @param height height of the frame buffer
 * @param channels channels of the frame buffer (CHANNEL_GREY, CHANNEL_RGB, CHANNEL_RGBA)
 * @return FrameBuffer 
 */
FrameBuffer create_frame_buffer(unsigned int width, unsigned int height, Channels channels) {

    FrameBuffer buffer;

    buffer.width = width;
    buffer.height = height;

    buffer.data = (unsigned char*)malloc(width * height * channels * sizeof(unsigned char));
    memset(buffer.data, 0, width * height * channels * sizeof(unsigned char));

    buffer.channels = channels;

    return buffer;
}

/**
 * @brief Free a frame buffer object
 * 
 * @param buffer pointer to the frame buffer object
 */
void free_frame_buffer(FrameBuffer buffer) {

    if (buffer.data != NULL) {
        free(buffer.data);
        buffer.data = NULL;
    }
}

/**
 * @brief Put a pixel in the frame buffer
 * 
 * @param buffer frame buffer object
 * @param x x coordinate of the pixel
 * @param y y coordinate of the pixel
 * @param color color of the pixel
 * @return int 1 if successful, -1 otherwise
 */
int put_pixel(FrameBuffer buffer, unsigned int x, unsigned int y, Color color) {

    if (x >= buffer.width || y >= buffer.height)
        return -1;

    unsigned char *px = &buffer.data[(y * buffer.width + x) * buffer.channels];
    float a = color.a;

    px[0] = (unsigned char)((color.r * a + (px[0] / 255.0f) * (1 - a)) * 255);
    px[1] = (unsigned char)((color.g * a + (px[1] / 255.0f) * (1 - a)) * 255);
    px[2] = (unsigned char)((color.b * a + (px[2] / 255.0f) * (1 - a)) * 255);

    if (buffer.channels == CHANNEL_RGBA)
        px[3] = (unsigned char)((a + (px[3] / 255.0f) * (1 - a)) * 255);

    return 1;
}

/**
 * @brief Plot a line with a low slope
 * 
 * @param buffer frame buffer object
 * @param x0 x coordinate of the first point
 * @param y0 y coordinate of the first point
 * @param x1 x coordinate of the second point
 * @param y1 y coordinate of the second point
 * @param color color of the line
 */
void plot_line_low(FrameBuffer buffer, int x0, int y0, int x1, int y1, Color color) {

    int dx = x1 - x0;
    int dy = y1 - y0;
    int yi = 1;

    if(dy < 0) {
        yi = -1;
        dy = -dy;
    }

    int D = (2 * dy) - dx;
    int y = y0;

    for(int i = x0; i < x1; i ++) {

    put_pixel(buffer, i, y, color);

    if(D > 0) {
        y += yi;
        D += 2 * (dy - dx);
    }else
        D += 2 * dy;
    }
}

/**
 * @brief Plot a line with a high slope
 * 
 * @param buffer frame buffer object
 * @param x0 x coordinate of the first point
 * @param y0 y coordinate of the first point
 * @param x1 x coordinate of the second point
 * @param y1 y coordinate of the second point
 * @param color color of the line
 */
void plot_line_high(FrameBuffer buffer, int x0, int y0, int x1, int y1, Color color) {

    int dx = x1 - x0;
    int dy = y1 - y0;
    int xi = 1;

    if(dx < 0) {
        xi = -1;
        dx = -dx;
    }

    int D = (2  * dx) - dy;
    int x = x0;

    for(int j = y0; j < y1; j ++) {

        put_pixel(buffer, x, j, color);

        if(D > 0) {
            x += xi;
            D += 2 * (dx - dy);
        }else
            D += 2 * dx;
    }
}

/**
 * @brief Draw a line between two points using Bresenham's line algorithm
 * 
 * @param buffer frame buffer object
 * @param x0 x coordinate of the first point
 * @param y0 y coordinate of the first point
 * @param x1 x coordinate of the second point
 * @param y1 y coordinate of the second point
 * @param color color of the line
 */
void draw_line(FrameBuffer buffer,int x0, int y0, int x1, int y1, Color color) {

    if(abs(y1 - y0) < abs(x1 - x0)) {
        if(x0 > x1)
            plot_line_low(buffer, x1, y1, x0, y0, color);
        else
            plot_line_low(buffer, x0, y0, x1, y1, color);
    }else {
        if(y0 > y1)
            plot_line_high(buffer, x1, y1, x0, y0, color);
        else
            plot_line_high(buffer, x0, y0, x1, y1, color);
    }
}

/**
 * @brief Draw a triangle
 * 
 * @param buffer frame buffer object
 * @param x0 x coordinate of the first vertex
 * @param y0 y coordinate of the first vertex
 * @param x1 x coordinate of the second vertex
 * @param y1 y coordinate of the second vertex
 * @param x2 x coordinate of the third vertex
 * @param y2 y coordinate of the third vertex
 * @param color color of the triangle
 */
void draw_triangle(FrameBuffer buffer, int x0, int y0, int x1, int y1, int x2, int y2, Color color) {
    draw_line(buffer, x0, y0, x1, y1, color);
    draw_line(buffer, x1, y1, x2, y2, color);
    draw_line(buffer, x2, y2, x0, y0, color);
}

/**
 * @brief Fill a triangle using Barycentric coordinates
 * 
 * @param buffer frame buffer object
 * @param x0 x coordinate of the first vertex
 * @param y0 y coordinate of the first vertex
 * @param x1 x coordinate of the second vertex
 * @param y1 y coordinate of the second vertex
 * @param x2 x coordinate of the third vertex
 * @param y2 y coordinate of the third vertex
 * @param color color of the triangle
 */
void fill_triangle(FrameBuffer buffer, int x0, int y0, int x1, int y1, int x2, int y2, Color color) {

    int v1[2] = {x0, y0};
    int v2[2] = {x1, y1};
    int v3[2] = {x2, y2};

    int maxX = max(v1[0], max(v2[0], v3[0]));
    int maxY = max(v1[1], max(v2[1], v3[1]));
    int minX = min(v1[0], min(v2[0], v3[0]));
    int minY = min(v1[1], min(v2[1], v3[1]));

    int vs1[2] = {v2[0] - v1[0], v2[1] - v1[1]};
    int vs2[2] = {v3[0] - v1[0], v3[1] - v1[1]};

    for(int x = minX; x <= maxX; x ++) {
        for(int y = minY; y <= maxY; y ++) {

            int q[2] = {x - v1[0], y - v1[1]};

            float s = (q[0] * vs2[1] - q[1] * vs2[0]) / (float)(vs1[0] * vs2[1] - vs1[1] * vs2[0]);
            float t = (vs1[0] * q[1] - vs1[1] * q[0]) / (float)(vs1[0] * vs2[1] - vs1[1] * vs2[0]);

            if((s >= 0) && (t >= 0) && (s + t <= 1))
                put_pixel(buffer, x, y, color);
        }
    }
}

/**
 * @brief Fill a rectangle
 * 
 * @param buffer frame buffer object
 * @param x x coordinate of the top-left corner
 * @param y y coordinate of the top-left corner
 * @param width width of the rectangle
 * @param height height of the rectangle
 * @param color color of the rectangle
 */
void fill_rect(FrameBuffer buffer, int x, int y, unsigned int width, unsigned int height, Color color) {
    
    for(int i = x; i < x + width; i ++) {
        for(int j = y; j < y + height; j ++)
            put_pixel(buffer, i, j, color);
    }
}

/**
 * @brief Draw a rectangle
 * 
 * @param buffer frame buffer object
 * @param x x coordinate of the top-left corner
 * @param y y coordinate of the top-left corner
 * @param width width of the rectangle
 * @param height height of the rectangle
 * @param color color of the rectangle
 */
void draw_rect(FrameBuffer buffer, int x, int y, unsigned int width, unsigned int height, Color color) {

    for(int i = x; i < x + width; i ++) {
        put_pixel(buffer, i, y, color);
        put_pixel(buffer, i, y + height - 1, color);
    }

    for(int j = y; j < y + height; j ++) {
        put_pixel(buffer, x, j, color);
        put_pixel(buffer, x + width - 1, j, color);
    }
}

/**
 * @brief Draw a circle outline using the midpoint circle algorithm
 *
 * @param buffer frame buffer object
 * @param cx x coordinate of the center
 * @param cy y coordinate of the center
 * @param radius radius of the circle in pixels
 * @param color color of the circle
 */
void draw_circle(FrameBuffer buffer, int cx, int cy, unsigned int radius, Color color) {

    int x = (int)radius;
    int y = 0;
    int err = 1 - x;

    while (x >= y) {

        put_pixel(buffer, cx + x, cy + y, color);
        put_pixel(buffer, cx - x, cy + y, color);
        put_pixel(buffer, cx + x, cy - y, color);
        put_pixel(buffer, cx - x, cy - y, color);
        put_pixel(buffer, cx + y, cy + x, color);
        put_pixel(buffer, cx - y, cy + x, color);
        put_pixel(buffer, cx + y, cy - x, color);
        put_pixel(buffer, cx - y, cy - x, color);

        y++;
        if (err < 0) {
            err += 2 * y + 1;
        } else {
            x--;
            err += 2 * (y - x) + 1;
        }
    }
}

/**
 * @brief Fill a circle using horizontal scanlines
 *
 * @param buffer frame buffer object
 * @param cx x coordinate of the center
 * @param cy y coordinate of the center
 * @param radius radius of the circle in pixels
 * @param color color of the circle
 */
void fill_circle(FrameBuffer buffer, int cx, int cy, unsigned int radius, Color color) {

    int r = (int)radius;

    for (int dy = -r; dy <= r; dy++) {

        int dx = (int)(sqrt((double)(r * r - dy * dy)) + 0.5);

        for (int x = cx - dx; x <= cx + dx; x++)
            put_pixel(buffer, x, cy + dy, color);
    }
}

/**
 * @brief Draw a rounded rectangle outline (1 px) using the midpoint circle algorithm for corners 
 * and straight lines for edges
 *
 * @param buffer frame buffer object
 * @param x x coordinate of the top-left corner
 * @param y y coordinate of the top-left corner
 * @param width width of the rectangle
 * @param height height of the rectangle
 * @param radius corner radius in pixels (clamped to half of the smallest side)
 * @param color color of the outline
 */
void draw_rounded_rect(FrameBuffer buffer, int x, int y, unsigned int width, unsigned int height, unsigned int radius, Color color) {

    int w = (int)width;
    int h = (int)height;

    if (w <= 0 || h <= 0)
        return;

    int r = (int)radius;
    r = min(r, w / 2);
    r = min(r, h / 2);

    if (r == 0) {
        draw_rect(buffer, x, y, width, height, color);
        return;
    }

    // Centers of the four corner arcs
    int cx1 = x + r,         cy1 = y + r;
    int cx2 = x + w - 1 - r, cy2 = y + h - 1 - r;

    // Straight edges (the arcs already cover their endpoints)
    for (int i = cx1 + 1; i < cx2; i++) {
        put_pixel(buffer, i, y, color);
        put_pixel(buffer, i, y + h - 1, color);
    }

    for (int j = cy1 + 1; j < cy2; j++) {
        put_pixel(buffer, x, j, color);
        put_pixel(buffer, x + w - 1, j, color);
    }

    // Corner arcs (midpoint circle algorithm, one octant mirrored into 4 corners)
    int px = r;
    int py = 0;
    int err = 1 - px;

    while (px >= py) {

        // Bottom-right
        put_pixel(buffer, cx2 + px, cy2 + py, color);
        // Top-right
        put_pixel(buffer, cx2 + px, cy1 - py, color);
        // Top-left
        put_pixel(buffer, cx1 - px, cy1 - py, color);
        // Bottom-left
        put_pixel(buffer, cx1 - px, cy2 + py, color);

        if (px != py) {
            put_pixel(buffer, cx2 + py, cy2 + px, color);
            put_pixel(buffer, cx2 + py, cy1 - px, color);
            put_pixel(buffer, cx1 - py, cy1 - px, color);
            put_pixel(buffer, cx1 - py, cy2 + px, color);
        }

        py++;
        if (err < 0) {
            err += 2 * py + 1;
        } else {
            px--;
            err += 2 * (py - px) + 1;
        }
    }
}

/**
 * @brief Fill a rounded rectangle using the midpoint circle algorithm for corners 
 * and straight lines for edges
 *
 * @param buffer frame buffer object
 * @param x x coordinate of the top-left corner
 * @param y y coordinate of the top-left corner
 * @param width width of the rectangle
 * @param height height of the rectangle
 * @param radius corner radius in pixels (clamped to half of the smallest side)
 * @param color fill color
 */
void fill_rounded_rect(FrameBuffer buffer, int x, int y, unsigned int width, unsigned int height, unsigned int radius, Color color) {

    int w = (int)width;
    int h = (int)height;

    if (w <= 0 || h <= 0)
        return;

    int r = (int)radius;
    r = min(r, w / 2);
    r = min(r, h / 2);

    int cx1 = x + r,         cy1 = y + r;
    int cx2 = x + w - 1 - r, cy2 = y + h - 1 - r;

    // Middle band: full-width rows
    for (int j = cy1 + 1; j < cy2; j++)
        for (int i = x; i < x + w; i++)
            put_pixel(buffer, i, j, color);

    // Top and bottom caps: rows shrink following the circle equation
    for (int dy = 0; dy <= r; dy++) {

        int dx = (int)(sqrt((double)(r * r - dy * dy)) + 0.5);

        int x_start = cx1 - dx;
        int x_end   = cx2 + dx;

        int top_row    = cy1 - dy;
        int bottom_row = cy2 + dy;

        for (int i = x_start; i <= x_end; i++)
            put_pixel(buffer, i, top_row, color);

        // Avoid painting the same row twice (matters with alpha) when cy1 == cy2
        if (bottom_row != top_row) {
            for (int i = x_start; i <= x_end; i++)
                put_pixel(buffer, i, bottom_row, color);
        }
    }
}

/**
 * @brief Load a font from a file
 *
 * @param font_path path to the font file
 * @param font      output: loaded font information
 * @return 1 on success, 0 on failure
 */
int load_font(const char* font_path, stbtt_fontinfo* font) {

    FILE* fontFile = fopen(font_path, "rb");
    if (!fontFile) {
        fprintf(stderr, "No se pudo abrir la fuente: %s\n", font_path);
        return 0;
    }

    // Determine the size of the font file
    fseek(fontFile, 0, SEEK_END);
    long size = ftell(fontFile);
    fseek(fontFile, 0, SEEK_SET);

    unsigned char* fontBuffer = (unsigned char*)malloc(size);
    if (!fontBuffer) {
        fclose(fontFile);
        return 0;
    }

    if (fread(fontBuffer, 1, size, fontFile) != (size_t)size) {
        fprintf(stderr, "Error leyendo la fuente: %s\n", font_path);
        free(fontBuffer);
        fclose(fontFile);
        return 0;
    }

    fclose(fontFile);

    if (!stbtt_InitFont(font, fontBuffer, stbtt_GetFontOffsetForIndex(fontBuffer, 0))) {
        fprintf(stderr, "stbtt_InitFont falló para: %s\n", font_path);
        free(fontBuffer);
        return 0;
    }

    return 1;
}

/**
 * @brief Convert a float color value to an unsigned char
 * @param v color value (0.0 to 1.0)
 * @return converted value (0 to 255)
 */
unsigned char color_to_byte(float v) {
    if (v < 0.0f) v = 0.0f;
    if (v > 1.0f) v = 1.0f;
    return (unsigned char)(v * 255.0f + 0.5f);
}

/**
 * @brief Create a sprite from text
 * @param text the text to render
 * @param font the font to use
 * @param fontSize the font size
 * @param color the text color
 * @return the created sprite
 */
Sprite text_sprite(const char* text, stbtt_fontinfo* font, float fontSize, Color color) {

    Sprite sprite = {0};
    float scale = stbtt_ScaleForPixelHeight(font, fontSize);

    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(font, &ascent, &descent, &lineGap);
    ascent  = (int)(ascent * scale + 0.5f);
    descent = (int)(descent * scale - 0.5f);

    // Compute width of the text
    float xpos = 0;
    for (int i = 0; text[i]; i++) {
        int adv, lsb;
        stbtt_GetCodepointHMetrics(font, text[i], &adv, &lsb);
        xpos += adv * scale;
        if (text[i + 1])
            xpos += scale * stbtt_GetCodepointKernAdvance(font, text[i], text[i + 1]);
    }

    int width  = (int)(xpos + 0.5f) + 1;
    int height = ascent - descent;

    // Mask for the text
    unsigned char* mask = (unsigned char*)calloc((size_t)width * height, 1);
    if (!mask) 
        return sprite;

    xpos = 0;
    for (int i = 0; text[i]; i++) {
        int adv, lsb, x0, y0, x1, y1;
        stbtt_GetCodepointHMetrics(font, text[i], &adv, &lsb);
        stbtt_GetCodepointBitmapBox(font, text[i], scale, scale, &x0, &y0, &x1, &y1);

        int x = (int)(xpos + lsb * scale + 0.5f);
        int y = ascent + y0;
        if (x < 0) x = 0;
        if (y < 0) y = 0;

        int gw = x1 - x0;
        if (x + gw <= width && y + (y1 - y0) <= height) {
            stbtt_MakeCodepointBitmap(font, mask + y * width + x,
                                      gw, y1 - y0, width, scale, scale, text[i]);
        }

        xpos += adv * scale;
        if (text[i + 1])
            xpos += scale * stbtt_GetCodepointKernAdvance(font, text[i], text[i + 1]);
    }

    // Apply color and alpha to the mask to create the final sprite
    unsigned char* data = (unsigned char*)malloc((size_t)width * height * 4);

    if (!data) {
        free(mask);
        return sprite;
    }

    unsigned char r = color_to_byte(color.r);
    unsigned char g = color_to_byte(color.g);
    unsigned char b = color_to_byte(color.b);
    float alpha = color.a < 0.0f ? 0.0f : (color.a > 1.0f ? 1.0f : color.a);

    for (int i = 0; i < width * height; i++) {
        data[i * 4 + 0] = r;
        data[i * 4 + 1] = g;
        data[i * 4 + 2] = b;
        data[i * 4 + 3] = (unsigned char)(mask[i] * alpha + 0.5f);
    }

    free(mask);

    sprite.data = data;
    sprite.width = width;
    sprite.height = height;
    sprite.channels = CHANNEL_RGBA;

    return sprite;
}

/**
 * @brief Creates a sprite from an image file
 * 
 * @param image_path path to the image file
 * @return Sprite created from the image
 */
Sprite image_sprite(const char* image_path) {

    Sprite sprite;

    int width, height, channels;
    unsigned char* data = stbi_load(image_path, &width, &height, &channels, 0);

    if (!data) {
        fprintf(stderr, "Error loading image: %s\n", image_path);
        return sprite;
    }

    sprite.data = data;
    sprite.width = (unsigned int)width;
    sprite.height = (unsigned int)height;
    sprite.channels = (Channels)channels;

    return sprite;
}

/**
 * @brief Resizes a sprite to a new width and height using nearest-neighbor interpolation
 * 
 * @param sprite sprite to be resized
 * @param new_width new width of the sprite
 * @param new_height new height of the sprite
 * @return resized sprite
 */
Sprite resize_sprite(Sprite sprite, unsigned int new_width, unsigned int new_height) {

    Sprite resized_sprite;
    resized_sprite.width = new_width;
    resized_sprite.height = new_height;
    resized_sprite.channels = sprite.channels;

    resized_sprite.data = (unsigned char*)malloc(new_width * new_height * sprite.channels);
    if (!resized_sprite.data) {
        fprintf(stderr, "Error allocating memory for resized sprite\n");
        return resized_sprite;
    }

    for (unsigned int j = 0; j < new_height; j++) {
        for (unsigned int i = 0; i < new_width; i++) {

            unsigned int src_x = (i * sprite.width) / new_width;
            unsigned int src_y = (j * sprite.height) / new_height;

            for (unsigned int c = 0; c < sprite.channels; c++) {
                resized_sprite.data[(j * new_width + i) * sprite.channels + c] =
                    sprite.data[(src_y * sprite.width + src_x) * sprite.channels + c];
            }
        }
    }

    return resized_sprite;
}

/**
 * @brief Draws a sprite on the frame buffer
 * 
 * @param buffer buffer where the sprite will be drawn
 * @param x x coordinate of the top-left corner where the sprite will be drawn
 * @param y y coordinate of the top-left corner where the sprite will be drawn
 * @param sprite sprite to be drawn
 */
void draw_sprite(FrameBuffer buffer, int x, int y, Sprite* sprite) {

    if (!sprite->data)
        return;

    int ch = sprite->channels;

    for (unsigned int j = 0; j < sprite->height; j++) {

        int dy = y + (int)j;
        if (dy < 0 || dy >= (int)buffer.height)
            continue;

        for (unsigned int i = 0; i < sprite->width; i++) {

            int dx = x + (int)i;
            if (dx < 0 || dx >= (int)buffer.width)
                continue;

            const unsigned char* src = &sprite->data[(j * sprite->width + i) * ch];

            Color color = {
                .r = src[0] / 255.0f,
                .g = src[1] / 255.0f,
                .b = src[2] / 255.0f,
                .a = (ch == CHANNEL_RGBA ? src[3] : 255) / 255.0f
            };

            if (color.a <= 0.0f)
                continue;

            put_pixel(buffer, (unsigned int)dx, (unsigned int)dy, color);
        }
    }
}

#ifdef __cplusplus
}
#endif