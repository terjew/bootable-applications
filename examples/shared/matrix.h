#ifndef MATRIX_H
#define MATRIX_H

#include "math.h"

void identity(float matrix[4][4])
{
    matrix[0][0] = 1;
    matrix[0][1] = 0;
    matrix[0][2] = 0;
    matrix[0][3] = 0;

    matrix[1][0] = 0;
    matrix[1][1] = 1;
    matrix[1][2] = 0;
    matrix[1][3] = 0;

    matrix[2][0] = 0;
    matrix[2][1] = 0;
    matrix[2][2] = 1;
    matrix[2][3] = 0;

    matrix[3][0] = 0;
    matrix[3][1] = 0;
    matrix[3][2] = 0;
    matrix[3][3] = 1;
}

void rotateZ(float matrix[4][4], float θ) //yaw
{
    identity(matrix);
    matrix[0][0] = cos(θ);
    matrix[0][1] = -sin(θ);

    matrix[1][0] = sin(θ);
    matrix[1][1] = cos(θ);
}

void rotateY(float matrix[4][4], float θ) //pitch
{
    identity(matrix);
    matrix[0][0] = cos(θ);
    matrix[0][2] = sin(θ);

    matrix[2][0] = -sin(θ);
    matrix[2][2] = cos(θ);
}

void rotateX(float matrix[4][4], float θ) //roll
{
    identity(matrix);
    matrix[1][1] = cos(θ);
    matrix[1][2] = -sin(θ);

    matrix[2][1] = sin(θ);
    matrix[2][2] = cos(θ);
}

void translate(float matrix[4][4], float tx, float ty, float tz)
{
    identity(matrix);
    matrix[3][0] = tx;
    matrix[3][1] = ty;
    matrix[3][2] = tz;
}

void scale(float matrix[4][4], float sx, float sy, float sz)
{
    identity(matrix);
    matrix[0][0] = sx;
    matrix[1][1] = sy;
    matrix[2][2] = sz;
}

void mul(float src1[4][4], float src2[4][4], float dest[4][4])
{
    dest[0][0] = src1[0][0] * src2[0][0] + src1[0][1] * src2[1][0] + src1[0][2] * src2[2][0] + src1[0][3] * src2[3][0]; 
    dest[0][1] = src1[0][0] * src2[0][1] + src1[0][1] * src2[1][1] + src1[0][2] * src2[2][1] + src1[0][3] * src2[3][1]; 
    dest[0][2] = src1[0][0] * src2[0][2] + src1[0][1] * src2[1][2] + src1[0][2] * src2[2][2] + src1[0][3] * src2[3][2]; 
    dest[0][3] = src1[0][0] * src2[0][3] + src1[0][1] * src2[1][3] + src1[0][2] * src2[2][3] + src1[0][3] * src2[3][3]; 
    dest[1][0] = src1[1][0] * src2[0][0] + src1[1][1] * src2[1][0] + src1[1][2] * src2[2][0] + src1[1][3] * src2[3][0]; 
    dest[1][1] = src1[1][0] * src2[0][1] + src1[1][1] * src2[1][1] + src1[1][2] * src2[2][1] + src1[1][3] * src2[3][1]; 
    dest[1][2] = src1[1][0] * src2[0][2] + src1[1][1] * src2[1][2] + src1[1][2] * src2[2][2] + src1[1][3] * src2[3][2]; 
    dest[1][3] = src1[1][0] * src2[0][3] + src1[1][1] * src2[1][3] + src1[1][2] * src2[2][3] + src1[1][3] * src2[3][3]; 
    dest[2][0] = src1[2][0] * src2[0][0] + src1[2][1] * src2[1][0] + src1[2][2] * src2[2][0] + src1[2][3] * src2[3][0]; 
    dest[2][1] = src1[2][0] * src2[0][1] + src1[2][1] * src2[1][1] + src1[2][2] * src2[2][1] + src1[2][3] * src2[3][1]; 
    dest[2][2] = src1[2][0] * src2[0][2] + src1[2][1] * src2[1][2] + src1[2][2] * src2[2][2] + src1[2][3] * src2[3][2]; 
    dest[2][3] = src1[2][0] * src2[0][3] + src1[2][1] * src2[1][3] + src1[2][2] * src2[2][3] + src1[2][3] * src2[3][3]; 
    dest[3][0] = src1[3][0] * src2[0][0] + src1[3][1] * src2[1][0] + src1[3][2] * src2[2][0] + src1[3][3] * src2[3][0]; 
    dest[3][1] = src1[3][0] * src2[0][1] + src1[3][1] * src2[1][1] + src1[3][2] * src2[2][1] + src1[3][3] * src2[3][1]; 
    dest[3][2] = src1[3][0] * src2[0][2] + src1[3][1] * src2[1][2] + src1[3][2] * src2[2][2] + src1[3][3] * src2[3][2]; 
    dest[3][3] = src1[3][0] * src2[0][3] + src1[3][1] * src2[1][3] + src1[3][2] * src2[2][3] + src1[3][3] * src2[3][3]; 
}

void transformVec4(float m[4][4], float in[4], float out[4])
{
    float x = in[0], y = in[1], z = in[2], w = in[3];
    out[0] = m[0][0] * x + m[1][0] * y + m[2][0] * z + m[3][0] * w;
    out[1] = m[0][1] * x + m[1][1] * y + m[2][1] * z + m[3][1] * w;
    out[2] = m[0][2] * x + m[1][2] * y + m[2][2] * z + m[3][2] * w;
    out[3] = m[0][3] * x + m[1][3] * y + m[2][3] * z + m[3][3] * w;
}

void transformVec3(float m[4][4], float in[3], float out[3])
{
    float x = in[0], y = in[1], z = in[2];
    out[0] = m[0][0] * x + m[1][0] * y + m[2][0] * z + m[3][0];
    out[1] = m[0][1] * x + m[1][1] * y + m[2][1] * z + m[3][1];
    out[2] = m[0][2] * x + m[1][2] * y + m[2][2] * z + m[3][2];
}

#endif