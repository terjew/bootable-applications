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

void drawMap(MAP * map, SPRITE ** sprites)
{
    EFI_UINT32 dx = TILE_WIDTH / 2;
    EFI_UINT32 dy = dx / 2;
    EFI_UINT32 map_xpos = width / 2 - dx; //centered on screen    
    EFI_UINT32 map_ypos = height / 2 - map->height * dy - TILE_HEIGHT + dy; 

    EFI_UINT32 numLayers = 4;
    for (EFI_UINT32 z = 0; z < numLayers; z++){
        for (EFI_UINT32 y = 0; y < map->height; y++)
        {
            for (EFI_UINT32 x = 0; x < map->width; x++)
            {
                EFI_UINT8 tileIndex = map->tiles[y * map->width + x];
                SPRITE * sprite = sprites[tileIndex];
                EFI_UINT32 xs = map_xpos + (x - y) * dx;
                EFI_UINT32 ys = map_ypos + (x + y) * dy - z * TILE_THICKNESS;
                if (sprite != NULL) drawSpriteTransparent(xs, ys, sprite);
            }
        }
    }

    Matrix4 mat;
    make_identity(mat);

    // These two rotations take a square in xy space to a squashed 2:1 tile in projected space:
    rotateZ(mat, -degToRad(45));
    rotateX(mat, degToRad(60));
    
    //Then we scale up so that 1 unit in xy space matches the extent of one tile sprite.
    //The scale factor (around 22) is the length of the "diagonal" in the tile image
    float scaleFactor = 32 * cos(degToRad(45));
    scale(mat, scaleFactor, scaleFactor, scaleFactor);

    //We move the coordinate system so the origin matches the top left corner of the top left tile
    translate(mat, map_xpos + 16, map_ypos + 8, 0);

    float layerDepth = -0.45;
    float bottomZ = layerDepth * -1;
    float topZ = layerDepth * 3;

    LINESET * lineset = createLineset(8, 6, 4, 1);
    float vertices[8][3] = {
        {0,0,bottomZ},
        {map->width,0,bottomZ},
        {map->width,map->height,bottomZ},
        {0,map->height,bottomZ},
        {0,0,topZ},
        {map->width,0,topZ},
        {map->width,map->height,topZ},
        {0,map->height,topZ},
    };
    memcpy(lineset->vertices, vertices, 8 * 3 * sizeof(float));

    int lines[6][4] = {
        {0,1,2,3},
        {4,5,6,7},
        {3,7,6,2},
        {0,4,5,1},
        {0,4,7,3},
        {1,2,6,5},
    };
    memcpy(lineset->lines, lines, 6 * 4 * sizeof(int));

    EFI_GRAPHICS_OUTPUT_BLT_PIXEL green = color(0,255,0);
    renderLineset(lineset, mat, pixels, stride, green);
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

    SPRITE * tiles[] = {
        loadSprite(32, 32, tile_000),
        loadSprite(32, 32, tile_001),
        loadSprite(32, 32, tile_002),
        loadSprite(32, 32, tile_004),
        loadSprite(32, 32, tile_005),
        loadSprite(32, 32, tile_006),
        loadSprite(32, 32, tile_007),
        loadSprite(32, 32, tile_008),
        loadSprite(32, 32, tile_009),
        loadSprite(32, 32, tile_010),
        loadSprite(32, 32, tile_022),
        loadSprite(32, 32, tile_023),
        loadSprite(32, 32, tile_040),
        NULL,
        NULL,
        NULL,
        NULL,
    };

    MAP * map = createMap(20,20);
    fillMap(map, 16);    
    drawMap(map, tiles);

    for (;;)
    {
        system_table->BootServices->WaitForEvent(1, &system_table->ConIn->WaitForKey, &event);
        system_table->ConIn->ReadKeyStroke(system_table->ConIn, &key);

        switch(key.ScanCode)
        {
            case 0x17: 
                system_table->RuntimeServices->ResetSystem(EFI_RESET_TYPE_Shutdown, 0, 0, NULL);
            default:
                break;
        }
        switch (key.UnicodeChar)
        {
            case 'm':
                clear(color(0,0,0));
                fillMap(map, 16);    
                drawMap(map, tiles);
                break;
            case 'q':
                clear(color(240, 127, 34));
                break;
            default:              
                clear(randomColor());
                break;
        }
    }

    return (0);
}
