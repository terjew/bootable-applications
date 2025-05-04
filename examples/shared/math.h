#ifndef MATH_H
#define MATH_H

#define M_PI 3.14159265358979323846264338327950288
#define M_PI_2 1.57079632679489661923
#define M_PI_M_2 6.283185307179586476925286766559

int _fltused = 0;

double cos(double x)
{
    asm("fldl %0;"
        "fcos;"
        "fstpl %0"
        : "+m"(x)
    );
    return x;
}

double sin(double x)
{
    asm("fldl %0;"
        "fsin;"
        "fstpl %0"
        : "+m"(x)
    );
    return x;
}

double tan(double x)
{
    return (sin(x) / cos(x));
}

double sqrt(double x)
{
    asm("fldl %0;"
        "fsqrt;"
        "fstpl %0"
        : "+m"(x)
    );
    return x;
}

int abs(int v)
{
    return v > 0 ? v : -v;
}

double round(double x)
{
    if (x < 0.0)
        return (int)(x - 0.5);
    else
        return (int)(x + 0.5);
}

float roundf(float x)
{
    if (x < 0.0f)
        return (int)(x - 0.5f);
    else
        return (int)(x + 0.5f);
}

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))


// //phi = ϕ
// //theta = θ
// //
// //Transform:
// //[cos(ϕ)   sin(ϕ)sin(θ)    0   tx]
// //[0        cos(θ)          0   ty]
// //[sin(ϕ)   -cos(ϕ)sin(θ)   0   tz]
// //[0        0               0    1]
// void isometricProjection(double matrix[16], double ϕ, double θ, double scale, double tx, double ty, double tz)
// {
//     matrix[0] = scale * cos(ϕ);
//     matrix[1] = sin(ϕ) * sin(θ);
//     matrix[2] = 0;
//     matrix[3] = 0;

//     matrix[4] = 0;
//     matrix[5] = scale * cos(θ);
//     matrix[6] = 0;
//     matrix[7] = 0;

//     matrix[8] = sin(ϕ);
//     matrix[9] = -cos(ϕ) * sin(θ);
//     matrix[10] = 0;
//     matrix[11] = 0;

//     matrix[12] = 0;
//     matrix[13] = 0;
//     matrix[14] = 0;
//     matrix[15] = 1;
// }

#endif