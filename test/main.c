#include <stdio.h>

#include <rasterizer/rasterizer.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <rasterizer/vendor/stb_image_write.h>

int main() {

    Color red = { .r = 1.f, .g = 0.f, .b = 0.f, .a = 1.f };
    Color blue = { .r = 0.f, .g = 0.f, .b = 1.f, .a = 1.f };
    Color green = { .r = 0.f, .g = 1.f, .b = 0.f, .a = 1.f };
    Color white = { .r = 1.f, .g = 1.f, .b = 1.f, .a = 1.f };

    FrameBuffer buffer = create_frame_buffer(800, 600, CHANNEL_RGBA);

    // Put pixel
    put_pixel(buffer, 400, 300, red);

    // Draw lines
    draw_line(buffer, 100, 100, 700, 500, red);
    draw_line(buffer, 100, 500, 700, 100, red);

    // Draw triangles
    fill_triangle(buffer, 200, 200, 600, 200, 400, 400, blue);
    draw_triangle(buffer, 200, 200, 600, 200, 400, 400, green);

    // Draw rectangles
    fill_rect(buffer, 50, 50, 100, 100, green);
    draw_rect(buffer, 50, 50, 100, 100, blue);

    fill_rounded_rect(buffer, 600, 400, 150, 100, 20, red);
    draw_rounded_rect(buffer, 600, 400, 150, 100, 20, white);

    // Draw circles
    draw_circle(buffer, 100, 300, 50, red);
    fill_circle(buffer, 700, 300, 50, blue);

    // Load font and draw text
    stbtt_fontinfo* font = load_font("Roboto-Light.ttf");

    Sprite sprite_text = text_sprite("2D CPU Rasterizer", font, 64, white);
    draw_sprite(buffer, 250, 10, &sprite_text);
    free_frame_buffer(sprite_text);
    
    free(font);

    // Load and draw sprite image
    Sprite sprite_image = image_sprite("sprite.png");
    draw_sprite(buffer, 300, 400, &sprite_image);
    
    Sprite sprite_image_resized = resize_sprite(sprite_image, 100, 100);
    draw_sprite(buffer, 450, 450, &sprite_image_resized);
    free_frame_buffer(sprite_image_resized);

    free_frame_buffer(sprite_image);

    // Save the frame buffer to a PNG file
    stbi_write_png("output.png", buffer.width, buffer.height, buffer.channels, buffer.data, buffer.width * buffer.channels);
    free_frame_buffer(buffer);

    return 0;
}