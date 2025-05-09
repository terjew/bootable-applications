#include "lil_uefi/lil_uefi.h"
#include "shared/rand.h"
#include "shared/math.h"
#include "shared/drawing.h"
#include "sprites/tile_000.h"
#include "sprites/tile_001.h"
#include "sprites/tile_002.h"
#include "sprites/tile_003.h"
#include "sprites/tile_004.h"
#include "sprites/tile_005.h"
#include "sprites/tile_006.h"
#include "sprites/tile_007.h"
#include "sprites/tile_008.h"
#include "sprites/tile_009.h"
#include "sprites/tile_010.h"
#include "sprites/tile_022.h"
#include "sprites/tile_023.h"
#include "sprites/tile_040.h"

#include "shared/matrix.h"
#include "shared/lineset.h"

#define NULL 0

const EFI_UINT32 TILE_WIDTH = 32;
const EFI_UINT32 TILE_HEIGHT = 32;
const EFI_UINT32 TILE_THICKNESS = 8;

typedef struct MAP
{
    EFI_UINT32 width;
    EFI_UINT32 height;
    EFI_UINT8 tiles[0];
} MAP;

EFI_GRAPHICS_OUTPUT_BLT_PIXEL depthgreen(float point[3])
{
    //z values from approx -50 to 250 seems to be the range for the scaled and rotated teapot
    int zscaled = 15 + MIN(MAX(0,point[2]/5), 240);
    return color(0,zscaled,0);
}

EFI_GRAPHICS_OUTPUT_BLT_PIXEL red(float point[3])
{
    return color(255,0,0);
}

BITMAP * backBuffer = 0;

void drawMap(MAP * map, BITMAP ** sprites, int selected[3])
{
    if (backBuffer == 0) {
        backBuffer = createBitmap(screen->width, screen->height);
    }
    fill(backBuffer->buffer, backBuffer->width * backBuffer->height, color(0,0,0));

    EFI_UINT32 dx = TILE_WIDTH / 2;
    EFI_UINT32 dy = dx / 2;
    EFI_UINT32 map_xpos = screen->width / 2 - dx - 0.3; //centered on screen    
    EFI_UINT32 map_ypos = screen->height / 2 - map->height * dy - TILE_HEIGHT + dy + 64; 

    EFI_UINT32 numLayers = 10;
    for (EFI_UINT32 z = 0; z < numLayers; z++){
        for (EFI_UINT32 y = 0; y < map->height; y++)
        {
            for (EFI_UINT32 x = 0; x < map->width; x++)
            {
                EFI_UINT8 tileIndex = map->tiles[y * map->width + x];
                BITMAP * sprite = sprites[tileIndex];
                EFI_UINT32 xs = map_xpos + (x - y) * dx;
                EFI_UINT32 ys = map_ypos + (x + y) * dy - z * TILE_THICKNESS;
                if (sprite != NULL) drawSpriteTransparent(xs, ys, sprite, backBuffer);
            }
        }
    }

    Matrix4 mat;
    make_identity(mat);
    
    //We move the coordinate system so the origin matches the top left corner of the top left tile
    translate(mat, map_xpos + 16, map_ypos + 8, 0);

    //Then we scale up so that 1 unit in xy space matches the extent of one tile sprite.
    //The scale factor (around 22) is the length of the "diagonal" in the tile image
    float scaleFactor = 32 * cos(degToRad(45));
    scale(mat, scaleFactor, scaleFactor, scaleFactor);

    // These two rotations take a square in xy space to a squashed 2:1 tile in projected space:
    rotateX(mat, degToRad(-60));//rotate the bottom of the square forward by 60 degrees along the scene x axis to achieve the squash
    rotateZ(mat, -degToRad(45));//then rotate 45 degrees along the object z axis to orient it along the diagonal tile grid

    float layerDepth = 0.41;
    float bottomZ = layerDepth * -1;
    float topZ = layerDepth * (numLayers-1);

    float min[3] = {0,0,bottomZ};
    float max[3] = {map->width,map->height,topZ};    
    LINESET * extents = createCuboid(min, max);
    renderLineset(extents, mat, backBuffer, depthgreen);

    float minCell[3] = {selected[0], selected[1], (selected[2] - 1) * layerDepth};
    float maxCell[3] = {selected[0] + 1, selected[1] + 1, selected[2] * layerDepth};
    LINESET * cell = createCuboid(minCell, maxCell);
    renderLineset(cell, mat, backBuffer, red);

    drawSpriteToScreen(0,0,backBuffer);
    
    free(extents);
    free(cell);
}


MAP * createMap(EFI_UINT32 width, EFI_UINT32 height)
{
    MAP * map = (MAP *) malloc(sizeof(MAP) + sizeof(EFI_UINT8) * width * height);
    map->width = width;
    map->height = height;
    return map;
}

//Idea for more advanced generation: Start with an ocean, create a land mass in the middle (like an island). 
//Correct the ocean tiles closest to the island to ones that match the placement of land tiles next to them.
//Possibly make the "island" more like a thin land mass with ocean on both sides, but possible to scroll infinitely in one direction.
void fillMap(MAP * map, EFI_UINT32 numTiles)
{
    for (int y = 0; y < map->height; y++)
    {
        for (int x = 0; x < map->width; x++)
        {
            map->tiles[y * map->width + x] = randRange(0, numTiles - 1);
        }
    }
}

#define DIR_FORWARD 0x01
#define DIR_BACKWARD 0x02
#define DIR_RIGHT 0x03
#define DIR_LEFT 0x04
#define DIR_UP 0x09
#define DIR_DOWN 0x0a

int dimensions[3] = {20,20,10};
void move(int pos[3], int axis, int direction)
{
    int newpos = pos[axis] + direction;
    pos[axis] = MAX(0, MIN(dimensions[axis]-1, newpos));
}

// entry point
EFI_UINTN EfiMain(EFI_HANDLE handle, EFI_SYSTEM_TABLE *system_table)
{
    *((int volatile *)&_fltused)=0; //prevent LTO from removing the marker symbol _fltused

    EFI_BOOT_SERVICES *boot_services = system_table->BootServices;
    EFI_STATUS status;
    int ns;

    initialize_memory(boot_services);
    
    EFI_GUID gfx_out_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL * gfx_out_prot;
    status = boot_services->LocateProtocol(&gfx_out_guid, 0, (void **)&gfx_out_prot);
    if (status != 0)
    {
        return status;
    }
    gfx_out_prot->SetMode(gfx_out_prot, 0);

    initialize_drawing(gfx_out_prot);

    EFI_UINTN event;
    EFI_INPUT_KEY key;

    EFI_UINT32 *frame_buffer_addr = (EFI_UINT32 *)gfx_out_prot->Mode->frame_buffer_base;
    EFI_UINT64 frame_buffer_size = gfx_out_prot->Mode->frame_buffer_size;

    EFI_TIME time;
    system_table->RuntimeServices->GetTime(&time, 0);

    srand(time.Second);

    BITMAP * tiles[] = {
        loadBitmap(32, 32, 32, tile_000),
        loadBitmap(32, 32, 32, tile_001),
        loadBitmap(32, 32, 32, tile_002),
        loadBitmap(32, 32, 32, tile_004),
        loadBitmap(32, 32, 32, tile_005),
        loadBitmap(32, 32, 32, tile_006),
        loadBitmap(32, 32, 32, tile_007),
        loadBitmap(32, 32, 32, tile_008),
        loadBitmap(32, 32, 32, tile_009),
        loadBitmap(32, 32, 32, tile_010),
        loadBitmap(32, 32, 32, tile_022),
        loadBitmap(32, 32, 32, tile_023),
        loadBitmap(32, 32, 32, tile_040),
        NULL,
        NULL,
        NULL,
        NULL,
    };

    int selected[3] = {1,19,9};
    
    MAP * map = createMap(20,20);
    fillMap(map, 16);    
    drawMap(map, tiles, selected);

    for (;;)
    {
        system_table->BootServices->WaitForEvent(1, &system_table->ConIn->WaitForKey, &event);
        system_table->ConIn->ReadKeyStroke(system_table->ConIn, &key);

        int cont = 0;

        switch(key.ScanCode)
        {
            case 0x17: 
                system_table->RuntimeServices->ResetSystem(EFI_RESET_TYPE_Shutdown, 0, 0, NULL);
            case DIR_FORWARD:
                move(selected, 1, -1);
                drawMap(map, tiles, selected);
                break;
            case DIR_BACKWARD:
                move(selected, 1, 1);
                drawMap(map, tiles, selected);
                break;
            case DIR_LEFT:
                move(selected, 0, -1);
                drawMap(map, tiles, selected);
                break;
            case DIR_RIGHT:
                move(selected, 0, 1);
                drawMap(map, tiles, selected);
                break;
            case DIR_DOWN:
                move(selected, 2, -1);
                drawMap(map, tiles, selected);
                break;
            case DIR_UP:
                move(selected, 2, 1);
                drawMap(map, tiles, selected);
                break;
            default:
                cont = 1;
                break;
        }
        if (cont) switch (key.UnicodeChar)
        {
            case 'm':
                clearScreen(color(0,0,0));
                fillMap(map, 16);
                drawMap(map, tiles, selected);
                break;
            case 'q':
                clearScreen(color(240, 127, 34));
                break;
            default:              
                clearScreen(randomColor());
                break;
        }
    }

    return (0);
}
