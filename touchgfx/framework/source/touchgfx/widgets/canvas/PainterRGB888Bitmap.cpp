/******************************************************************************
 * This file is part of the TouchGFX 4.9.3 distribution.
 * Copyright (C) 2017 Draupner Graphics A/S <http://www.touchgfx.com>.
 ******************************************************************************
 * This is licensed software. Any use hereof is restricted by and subject to 
 * the applicable license terms. For further information see "About/Legal
 * Notice" in TouchGFX Designer or in your TouchGFX installation directory.
 *****************************************************************************/

#include <touchgfx/widgets/canvas/PainterRGB888Bitmap.hpp>

namespace touchgfx
{

PainterRGB888Bitmap::PainterRGB888Bitmap(const Bitmap& bmp, uint8_t alpha) :
    AbstractPainterRGB888(), bitmapARGB8888Pointer(0), bitmapRGB888Pointer(0)
{
    setBitmap(bmp);
    setAlpha(alpha);
}

void PainterRGB888Bitmap::setBitmap(const Bitmap& bmp)
{
    bitmap = bmp;
    bitmapRectToFrameBuffer = bitmap.getRect();
    DisplayTransformation::transformDisplayToFrameBuffer(bitmapRectToFrameBuffer);
}

void PainterRGB888Bitmap::setAlpha(uint8_t alpha)
{
    painterAlpha = alpha;
}

uint8_t PainterRGB888Bitmap::getAlpha() const
{
    return painterAlpha;
}

void PainterRGB888Bitmap::render(uint8_t* ptr, int x, int xAdjust, int y, unsigned count, const uint8_t* covers)
{
    uint8_t* p = ptr + ((x + xAdjust) * 3);

    currentX = x + areaOffsetX;
    currentY = y + areaOffsetY;

    if (!renderInit())
    {
        return;
    }

    if (currentX + (int)count > bitmapRectToFrameBuffer.width)
    {
        count = bitmapRectToFrameBuffer.width - currentX;
    }

    uint8_t totalAlpha = (widgetAlpha * painterAlpha) / 255;
    if (bitmap.getFormat() == Bitmap::RGB888)
    {
        const uint8_t* src = bitmapRGB888Pointer;
        uint8_t pByte;
        if (totalAlpha == 255)
        {
            do
            {
                // Use alpha from covers directly
                uint32_t alpha = *covers++;
                if (alpha == 255)
                {
                    // Solid pixel
                    *p++ = *src++;
                    *p++ = *src++;
                    *p++ = *src++;
                }
                else
                {
                    pByte = *p;
                    *p++ = static_cast<uint8_t>((((*src++ - pByte) * alpha) >> 8) + pByte);
                    pByte = *p;
                    *p++ = static_cast<uint8_t>((((*src++ - pByte) * alpha) >> 8) + pByte);
                    pByte = *p;
                    *p++ = static_cast<uint8_t>((((*src++ - pByte) * alpha) >> 8) + pByte);
                }
            }
            while (--count != 0);
        }
        else
        {
            do
            {
                uint32_t alpha = (*covers) * totalAlpha;
                covers++;
                pByte = *p;
                *p++ = static_cast<uint8_t>((((*src++ - pByte) * alpha) >> 16) + pByte);
                pByte = *p;
                *p++ = static_cast<uint8_t>((((*src++ - pByte) * alpha) >> 16) + pByte);
                pByte = *p;
                *p++ = static_cast<uint8_t>((((*src++ - pByte) * alpha) >> 16) + pByte);
            }
            while (--count != 0);
        }
    }
    else if (bitmap.getFormat() == Bitmap::ARGB8888)
    {
        const uint32_t* src = bitmapARGB8888Pointer;
        uint8_t pByte;
        uint8_t cByte;
        if (totalAlpha == 255)
        {
            do
            {
                uint8_t srcAlpha = *src >> 24;
                uint32_t alpha = (*covers) * srcAlpha;
                covers++;
                if (alpha == 255u * 255u)
                {
                    // Solid pixel
                    *p++ = *src; // Blue
                    *p++ = *src >> 8; // Green
                    *p++ = *src >> 16; // Red
                }
                else
                {
                    // Non-Transparent pixel
                    pByte = *p;
                    cByte = *src;
                    *p++ = static_cast<uint8_t>((((cByte - pByte) * alpha) >> 16) + pByte);
                    pByte = *p;
                    cByte = *src >> 8;
                    *p++ = static_cast<uint8_t>((((cByte - pByte) * alpha) >> 16) + pByte);
                    pByte = *p;
                    cByte = *src >> 16;
                    *p++ = static_cast<uint8_t>((((cByte - pByte) * alpha) >> 16) + pByte);
                }
                src++;
            }
            while (--count != 0);
        }
        else
        {
            do
            {
                uint8_t srcAlpha = *src >> 24;
                uint32_t alpha = (*covers) * srcAlpha * totalAlpha;
                covers++;
                if (alpha)
                {
                    pByte = *p;
                    cByte = *src;
                    *p++ = static_cast<uint8_t>((((cByte - pByte) * alpha) >> 24) + pByte);
                    pByte = *p;
                    cByte = *src >> 8;
                    *p++ = static_cast<uint8_t>((((cByte - pByte) * alpha) >> 24) + pByte);
                    pByte = *p;
                    cByte = *src >> 16;
                    *p++ = static_cast<uint8_t>((((cByte - pByte) * alpha) >> 24) + pByte);
                }
                else
                {
                    p += 3;
                }
                src++;
            }
            while (--count != 0);
        }
    }
}

bool PainterRGB888Bitmap::renderInit()
{
    bitmapARGB8888Pointer = 0;
    bitmapRGB888Pointer = 0;

    if ((currentX >= bitmapRectToFrameBuffer.width) ||
            (currentY >= bitmapRectToFrameBuffer.height))
    {
        // Outside bitmap area, do not draw anything
        // Consider the following instead of "return" to get a tiled image:
        //   currentX %= bitmapRectToFrameBuffer.width
        //   currentY %= bitmapRectToFrameBuffer.height
        return false;
    }

    if (bitmap.getFormat() == Bitmap::ARGB8888)
    {
        bitmapARGB8888Pointer = (const uint32_t*)bitmap.getData();
        if (!bitmapARGB8888Pointer)
        {
            return false;
        }
        bitmapARGB8888Pointer += currentX + currentY * bitmapRectToFrameBuffer.width;
    }
    else
    {
        bitmapRGB888Pointer = bitmap.getData();
        if (!bitmapRGB888Pointer)
        {
            return false;
        }
        bitmapRGB888Pointer += (currentX + currentY * bitmapRectToFrameBuffer.width) * 3;
    }

    return true;
}

bool PainterRGB888Bitmap::renderNext(uint8_t& red, uint8_t& green, uint8_t& blue, uint8_t& alpha)
{
    if (currentX >= bitmapRectToFrameBuffer.width)
    {
        return false;
    }

    if (bitmapARGB8888Pointer != 0)
    {
        uint32_t argb8888 = *bitmapARGB8888Pointer++;
        alpha = (argb8888 >> 24) & 0xFF;
        red = (argb8888 >> 16) & 0xFF;
        green = (argb8888 >> 8) & 0xFF;
        blue = (argb8888) & 0xFF;
    }
    else if (bitmapRGB888Pointer != 0)
    {
        blue = *bitmapRGB888Pointer++;
        green = *bitmapRGB888Pointer++;
        red = *bitmapRGB888Pointer++;
        alpha = 255; // No alpha per pixel in the image, it is solid
    }
    if (painterAlpha < 255)
    {
        // Apply given alpha from setAlpha()
        alpha = (((uint16_t)alpha) * ((uint16_t)painterAlpha)) / 255;
    }
    return true;
}

} // namespace touchgfx
