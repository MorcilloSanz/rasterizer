/**
 * @file rasterizer.h
 * @brief A small, dependency-light 2D graphics library that draws directly into a
 * CPU-side frame buffer.
 *
 * Notes:
 *  - The coordinate origin (0, 0) is the top-left corner of the buffer.
 *  - Drawing outside the buffer is safe: out-of-bounds pixels are ignored.
 *  - Buffers and sprites own heap memory and must be released by the caller.
 * 
 * @author Alberto Morcillo Sanz
 * 
 * email amorcillosanz@gmail.com
 * GitHub: MorcilloSanz
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enum for the number of channels in the frame buffer
 * 
 * CHANNEL_GREY: 1 channel (grayscale)
 * CHANNEL_RGB: 3 channels (red, green, blue)
 * CHANNEL_RGBA: 4 channels (red, green, blue, alpha)
 */
enum Channels {
    CHANNEL_GREY = 1,
    CHANNEL_RGB = 3,
    CHANNEL_RGBA = 4
};

typedef enum Channels Channels;

/**
 * @brief Structure representing a frame buffer.
 * 
 * @param width Width of the frame buffer in pixels
 * @param height Height of the frame buffer in pixels
 * @param data Pointer to the pixel data (allocated on the heap)
 * @param channels Number of channels in the pixel data (1 for grey, 3 for RGB, 4 for RGBA)
 * 
 * Note: The pixel data is stored in row-major order, with each pixel represented by
 * a sequence of bytes corresponding to its channels. For example, an RGBA pixel
 * is represented by four bytes in the order R, G, B, A.
 * 
 * The frame buffer owns the memory for the pixel data, and it must be freed by the caller 
 * using the free_frame_buffer function.
 */
struct FrameBuffer {
    unsigned int width;
    unsigned int height;
    unsigned char* data;
    Channels channels;
};

typedef struct FrameBuffer FrameBuffer;
typedef struct FrameBuffer Sprite;

/**
 * @brief Structure representing a color
 * 
 * @param r red component (0.0 to 1.0)
 * @param g green component (0.0 to 1.0)
 * @param b blue component (0.0 to 1.0)
 * @param a alpha component (0.0 to 1.0)
 */
struct Color {
    float r;
    float g;
    float b;
    float a;
};

typedef struct Color Color;

/**
 * @brief Structure representing a font info object
 * 
 */
typedef struct stbtt_fontinfo stbtt_fontinfo;

/**
 * @brief Create a frame buffer object
 * 
 * @param width width of the frame buffer
 * @param height height of the frame buffer
 * @param channels channels of the frame buffer (CHANNEL_GREY, CHANNEL_RGB, CHANNEL_RGBA)
 * @return FrameBuffer 
 */
FrameBuffer create_frame_buffer(unsigned int width, unsigned int height, Channels channels);

/**
 * @brief Free a frame buffer object
 * 
 * @param buffer pointer to the frame buffer object
 */
void free_frame_buffer(FrameBuffer buffer);

/**
 * @brief Put a pixel in the frame buffer
 * 
 * @param buffer frame buffer object
 * @param x x coordinate of the pixel
 * @param y y coordinate of the pixel
 * @param color color of the pixel
 * @return int 1 if successful, -1 otherwise
 */
int put_pixel(FrameBuffer buffer, unsigned int x, unsigned int y, Color color);


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
void draw_line(FrameBuffer buffer,int x0, int y0, int x1, int y1, Color color);

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
void draw_triangle(FrameBuffer buffer, int x0, int y0, int x1, int y1, int x2, int y2, Color color);

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
void fill_triangle(FrameBuffer buffer, int x0, int y0, int x1, int y1, int x2, int y2, Color color);

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
void draw_rect(FrameBuffer buffer, int x, int y, unsigned int width, unsigned int height, Color color);

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
void fill_rect(FrameBuffer buffer, int x, int y, unsigned int width, unsigned int height, Color color);

/**
 * @brief Draw a circle outline using the midpoint circle algorithm
 *
 * @param buffer frame buffer object
 * @param cx x coordinate of the center
 * @param cy y coordinate of the center
 * @param radius radius of the circle in pixels
 * @param color color of the circle
 */
void draw_circle(FrameBuffer buffer, int cx, int cy, unsigned int radius, Color color);

/**
 * @brief Fill a circle using horizontal scanlines
 *
 * @param buffer frame buffer object
 * @param cx x coordinate of the center
 * @param cy y coordinate of the center
 * @param radius radius of the circle in pixels
 * @param color color of the circle
 */
void fill_circle(FrameBuffer buffer, int cx, int cy, unsigned int radius, Color color);

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
void draw_rounded_rect(FrameBuffer buffer, int x, int y, unsigned int width, unsigned int height, unsigned int radius, Color color);

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
void fill_rounded_rect(FrameBuffer buffer, int x, int y, unsigned int width, unsigned int height, unsigned int radius, Color color);

/**
 * @brief 
 * 
 * @param font_path 
 * @return stbtt_fontinfo* 
 */
stbtt_fontinfo* load_font(const char* font_path);

/**
 * @brief Create a sprite from text
 * @param text the text to render
 * @param font the font to use
 * @param fontSize the font size
 * @param color the text color
 * @return the created sprite
 */
Sprite text_sprite(const char* text, stbtt_fontinfo* font, float fontSize, Color color);

/**
 * @brief Creates a sprite from an image file
 * 
 * @param image_path path to the image file
 * @return Sprite created from the image
 */
Sprite image_sprite(const char* image_path);

/**
 * @brief Resizes a sprite to a new width and height using nearest-neighbor interpolation
 * 
 * @param sprite sprite to be resized
 * @param new_width new width of the sprite
 * @param new_height new height of the sprite
 * @return resized sprite
 */
Sprite resize_sprite(Sprite sprite, unsigned int new_width, unsigned int new_height);

/**
 * @brief Draws a sprite on the frame buffer
 * 
 * @param buffer buffer where the sprite will be drawn
 * @param x x coordinate of the top-left corner where the sprite will be drawn
 * @param y y coordinate of the top-left corner where the sprite will be drawn
 * @param sprite sprite to be drawn
 */
void draw_sprite(FrameBuffer buffer, int x, int y, Sprite* sprite);

#ifdef __cplusplus
}
#endif