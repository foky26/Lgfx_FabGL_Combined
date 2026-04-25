// GIFDraw is called by AnimatedGIF library for each frame line

#define BUFFER_SIZE 400   // Optimum is >= GIF width or integral division of width

// La paleta del GIF viene en big-endian; hacemos swap para que
// LovyanGFX reciba el RGB565 en little-endian correcto.
inline uint16_t gifColor(uint16_t c) {
    return (c >> 8) | (c << 8);
}

void GIFDraw(GIFDRAW *pDraw)
{
    uint8_t *s;
    uint16_t *d, *usPalette, usTemp[320];
    int x, y;
    int marginx = x_offset;
    int marginy = y_offset;

    usPalette = pDraw->pPalette;
    y = pDraw->iY + pDraw->y; // current line

    s = pDraw->pPixels;
    if (pDraw->ucDisposalMethod == 2) // restore to background color
    {
        for (x = 0; x < pDraw->iWidth; x++)
        {
            if (s[x] == pDraw->ucTransparent)
                s[x] = pDraw->ucBackground;
        }
        pDraw->ucHasTransparency = 0;
    }

    // Apply the new pixels to the main image
    if (pDraw->ucHasTransparency) // if transparency used
    {
        uint8_t *pEnd, c, ucTransparent = pDraw->ucTransparent;
        int x, iCount;
        pEnd = s + pDraw->iWidth;
        x = 0;
        iCount = 0;
        while (x < pDraw->iWidth)
        {
            c = ucTransparent - 1;
            d = usTemp;
            while (c != ucTransparent && s < pEnd)
            {
                c = *s++;
                if (c == ucTransparent) // done, stop
                {
                    s--; // back up to treat it like transparent
                }
                else // opaque
                {
                    *d++ = gifColor(usPalette[c]);
                    iCount++;
                }
            } // while looking for opaque pixels
            if (iCount) // any opaque pixels?
            {
                for (int xOffset = 0; xOffset < iCount; xOffset++) {
                    display.drawPixel(pDraw->iX + x + xOffset + marginx,
                                      marginy + y, usTemp[xOffset]);
                }
                x += iCount;
                iCount = 0;
            }
            // no, look for a run of transparent pixels
            c = ucTransparent;
            while (c == ucTransparent && s < pEnd)
            {
                c = *s++;
                if (c == ucTransparent)
                    iCount++;
                else
                    s--;
            }
            if (iCount)
            {
                x += iCount; // skip these
                iCount = 0;
            }
        }
    }
    else
    {
        s = pDraw->pPixels;
        // Translate the 8-bit pixels through the RGB565 palette
        for (x = 0; x < pDraw->iWidth; x++)
        {
            display.drawPixel(x + marginx, marginy + y, gifColor(usPalette[*s++]));
        }
    }
} /* GIFDraw() */


