#ifndef DRAWING_H
#define DRAWING_H

#include "lil_uefi/lil_uefi.h"
#include "memory.h"
#include "math.h"

typedef struct BITMAP {
    EFI_UINT32 width;
    EFI_UINT32 height;
    EFI_UINT32 stride;
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL * buffer;
} BITMAP;

EFI_GRAPHICS_OUTPUT_PROTOCOL * graphics;
BITMAP * screen;

BITMAP * createBitmap(EFI_UINT32 width, EFI_UINT32 height)
{
    BITMAP * sprite = (BITMAP*) malloc(sizeof(BITMAP) + sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * width * height);
    sprite->width = width;
    sprite->height = height;
    sprite->stride = width;
    sprite->buffer = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) (&(sprite->buffer) + sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL *));
    return sprite;
}

BITMAP * loadBitmap(EFI_UINT32 width, EFI_UINT32 height, EFI_UINT32 stride, const unsigned int * buffer)
{
    BITMAP * bitmap = malloc(sizeof(BITMAP));
    bitmap->width = width;
    bitmap->height = height;
    bitmap->stride = stride;
    bitmap->buffer = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL*) buffer;
    return bitmap;
}

void destroySprite(BITMAP * sprite)
{
    free(sprite);
}

void initialize_drawing(EFI_GRAPHICS_OUTPUT_PROTOCOL * graphics_in)
{
    graphics = graphics_in;
    screen = loadBitmap(graphics->Mode->info->HorizontalResolution, graphics->Mode->info->VerticalResolution, graphics->Mode->info->PixelsPerScanLine, (const unsigned int *) graphics->Mode->frame_buffer_base);
}

void fill(EFI_GRAPHICS_OUTPUT_BLT_PIXEL * buffer, EFI_UINT32 size, EFI_GRAPHICS_OUTPUT_BLT_PIXEL color)
{
    for (EFI_UINT64 idx = 0; idx < size; idx += 1)
    {
        buffer[idx] = color;
    }
}

EFI_GRAPHICS_OUTPUT_BLT_PIXEL color(EFI_UINT32 r, EFI_UINT32 g, EFI_UINT32 b)
{
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL p;
    p.Red = r;
    p.Green = g;
    p.Blue = b;
    return p;
}

void drawRectangleScreen(EFI_UINT32 dx, EFI_UINT32 dy, EFI_UINT32 width, EFI_UINT32 height, EFI_GRAPHICS_OUTPUT_BLT_PIXEL color)
{
    EFI_UINT32 * buffer = (EFI_UINT32 *) &color;
    graphics->Blt(graphics, buffer, EFI_GRAPHICS_OUTPUT_BLT_OPERATION_VideoFill, 0, 0, dx, dy, width, height, 0);
}

void clearScreen(EFI_GRAPHICS_OUTPUT_BLT_PIXEL color)
{
    drawRectangleScreen(0, 0, screen->width, screen->height, color);
}

void drawSpriteToScreen(EFI_UINT32 dx, EFI_UINT32 dy, BITMAP * sprite)
{
    graphics->Blt(graphics, (EFI_UINT32*)sprite->buffer, EFI_GRAPHICS_OUTPUT_BLT_OPERATION_BufferToVideo, 0, 0, dx, dy, sprite->width, sprite->height, sprite->stride * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
}

void grabScreenToSprite(EFI_UINT32 sx, EFI_UINT32 sy, BITMAP * sprite)
{
    graphics->Blt(graphics, (EFI_UINT32*)sprite->buffer, EFI_GRAPHICS_OUTPUT_BLT_OPERATION_VideoToBuffer, sx, sy, 0, 0, sprite->width, sprite->height, sprite->stride  * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
}

void drawSpriteTransparent(EFI_UINT32 dx, EFI_UINT32 dy, BITMAP * sprite, BITMAP * target)
{
    for (EFI_UINT32 y = 0; y < sprite->height; y++)
    {
        int py = y + dy;
        for (EFI_UINT32 x = 0; x < sprite->width; x++)
        {
            int px = x + dx;
            EFI_GRAPHICS_OUTPUT_BLT_PIXEL pixel = sprite->buffer[y * sprite->width + x];
            if (pixel.Reserved != 0) {
                target->buffer[py * target->stride + px] = pixel;
            }
        }
    }
}

void drawLine(int x0, int y0, int x1, int y1, BITMAP * target, EFI_GRAPHICS_OUTPUT_BLT_PIXEL color)
{
    int dx = x1 - x0;
    int dy = y1 - y0;
    EFI_UINT32 xSteps = abs(dx);
    EFI_UINT32 ySteps = abs(dy);
    EFI_UINT32 numSteps = MAX(xSteps, ySteps);
    float numStepsF = (float) numSteps;
    float xIncrement = dx / numStepsF;
    float yIncrement = dy / numStepsF;

    float xf = x0;
    float yf = y0;
    for (EFI_UINT32 step = 0; step <= numSteps; ++step)
    {
        int x = roundf(xf);
        int y = roundf(yf);
        EFI_UINT32 index = y * target->stride + x;
        if (index >= 0 && index < y * target->stride * target->height){
            target->buffer[y * target->stride + x] = color;
        }
        xf += xIncrement;
        yf += yIncrement;
    }
}

#endif