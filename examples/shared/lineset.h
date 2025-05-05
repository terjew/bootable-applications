#ifndef LINESET_H
#define LINESET_H

#include "memory.h"
#include "matrix.h"
#include "drawing.h"

typedef struct LINESET
{
    int numVertices;
    int numLines;
    int lineLength;
    int loop;
    float * vertices;
    int * lines;
} LINESET;

LINESET * createLineset(int numVertices, int numLines, int lineLength, int loop)
{
    int verticesSize = numVertices * 3 * sizeof(float);
    int linesSize = numLines * lineLength * sizeof(int);
    LINESET * lineset = (LINESET*)malloc(sizeof(LINESET) + verticesSize + linesSize);
    lineset->numVertices = numVertices;
    lineset->numLines = numLines;
    lineset->lineLength = lineLength;
    lineset->loop = loop;
    lineset->vertices = (float *)((void*)(lineset) + sizeof(LINESET));
    lineset->lines = (int *)((void*)(lineset) + sizeof(LINESET) + verticesSize);
    return lineset;
}

void renderLineset(LINESET * lineset, float matrix[4][4], EFI_GRAPHICS_OUTPUT_BLT_PIXEL color)
{
    float * transformedVertices = (float*)malloc(lineset->numVertices * 3 * sizeof(float));
    for (int i = 0; i < lineset->numVertices; i++)
    {
        transformVec3(matrix, &(lineset->vertices[i * 3]), &(transformedVertices[i * 3]));
    }

    for (int i = 0; i < lineset->numLines; i++)
    {
        int * line = (int*)&(lineset->lines[i * lineset->lineLength]);
        for (int j = 1; j < lineset->lineLength; j++)
        {
            int i0 = line[j-1];
            int i1 = line[j];
            float * p0 = &(transformedVertices[i0 * 3]);
            float * p1 = &(transformedVertices[i1 * 3]);
            drawLine(p0[0], p0[1], p1[0], p1[1], color);
        }
        if (lineset->loop)
        {
            int i0 = line[lineset->lineLength - 1];
            int i1 = line[0];
            float * p0 = &(transformedVertices[i0 * 3]);
            float * p1 = &(transformedVertices[i1 * 3]);
            drawLine(p0[0], p0[1], p1[0], p1[1], color);
        }
    }
    
    free(transformedVertices);
}

#endif
