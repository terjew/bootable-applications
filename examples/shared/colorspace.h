//https://stackoverflow.com/a/14733008/2087791

typedef struct Color_BGRA {
    unsigned char b;
    unsigned char g;
    unsigned char r;
    unsigned char a;
} Color_BGRA;

typedef struct Color_HSVA {
    unsigned char h;
    unsigned char s;
    unsigned char v;
    unsigned char a;
} Color_HSVA;

Color_BGRA HsvToRgb(Color_HSVA hsva)
{
    Color_BGRA bgra;
    unsigned char region, remainder, p, q, t;
    
    if (hsva.s == 0)
    {
        bgra.r = hsva.v;
        bgra.g = hsva.v;
        bgra.b = hsva.v;
        bgra.a = hsva.a;
        return bgra;
    }
    
    region = hsva.h / 43;
    remainder = (hsva.h - (region * 43)) * 6; 
    
    p = (hsva.v * (255 - hsva.s)) >> 8;
    q = (hsva.v * (255 - ((hsva.s * remainder) >> 8))) >> 8;
    t = (hsva.v * (255 - ((hsva.s * (255 - remainder)) >> 8))) >> 8;
    
    switch (region)
    {
        case 0:
            bgra.r = hsva.v; bgra.g = t; bgra.b = p;
            break;
        case 1:
            bgra.r = q; bgra.g = hsva.v; bgra.b = p;
            break;
        case 2:
            bgra.r = p; bgra.g = hsva.v; bgra.b = t;
            break;
        case 3:
            bgra.r = p; bgra.g = q; bgra.b = hsva.v;
            break;
        case 4:
            bgra.r = t; bgra.g = p; bgra.b = hsva.v;
            break;
        default:
            bgra.r = hsva.v; bgra.g = p; bgra.b = q;
            break;
    }
    
    return bgra;
}

Color_HSVA RgbToHsv(Color_BGRA bgra)
{
    Color_HSVA hsva;
    hsva.a = bgra.a;

    unsigned char rgbMin, rgbMax;

    rgbMin = bgra.r < bgra.g ? (bgra.r < bgra.b ? bgra.r : bgra.b) : (bgra.g < bgra.b ? bgra.g : bgra.b);
    rgbMax = bgra.r > bgra.g ? (bgra.r > bgra.b ? bgra.r : bgra.b) : (bgra.g > bgra.b ? bgra.g : bgra.b);
    
    hsva.v = rgbMax;
    if (hsva.v == 0)
    {
        hsva.h = 0;
        hsva.s = 0;
        return hsva;
    }

    hsva.s = 255 * (long)(rgbMax - rgbMin) / hsva.v;
    if (hsva.s == 0)
    {
        hsva.h = 0;
        return hsva;
    }

    if (rgbMax == bgra.r)
        hsva.h = 0 + 43 * (bgra.g - bgra.b) / (rgbMax - rgbMin);
    else if (rgbMax == bgra.g)
        hsva.h = 85 + 43 * (bgra.b - bgra.r) / (rgbMax - rgbMin);
    else
        hsva.h = 171 + 43 * (bgra.r - bgra.g) / (rgbMax - rgbMin);

    return hsva;
}
