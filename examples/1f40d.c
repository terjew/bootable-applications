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

#define NULL 0

const EFI_UINT32 TILE_WIDTH = 32;
const EFI_UINT32 TILE_HEIGHT = 32;

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
    EFI_UINT32 xpos = width / 2 - dx; //centered on screen    
    EFI_UINT32 ypos = height / 2 - map->height * dy - TILE_HEIGHT + dy; 

    for (EFI_UINT32 y = 0; y < map->height; y++)
    {
        for (EFI_UINT32 x = 0; x < map->width; x++)
        {
            EFI_UINT8 tileIndex = map->tiles[y * map->width + x];
            SPRITE * sprite = sprites[tileIndex];
            EFI_UINT32 xs = xpos + (x - y) * dx;
            EFI_UINT32 ys = ypos + (x + y) * dy;
            if (sprite != NULL) drawSpriteTransparent(xs, ys, sprite);
        }
    }

    float zrot[4][4]; rotateZ(zrot, -M_PI / 4);
    float xrot[4][4]; rotateX(xrot, 1.0472);//60 degrees 
    float trans[4][4]; translate(trans, xpos + 15.5, ypos + 8, 0);
    float scaleFactor = 22.60;//?
    float scalem[4][4]; scale(scalem, scaleFactor, scaleFactor, scaleFactor);
    float trans_scale[4][4]; mul(scalem, trans, trans_scale);
    float trans_scale_rot1[4][4]; mul(xrot, trans_scale, trans_scale_rot1);
    float mat[4][4]; mul(zrot, trans_scale_rot1, mat);

    float p1[] = {0,0,0.5,1};
    float p2[] = {map->width,0,0.5,1};
    float p3[] = {0,map->height,0.5,1};
    float p4[] = {map->width,map->height,0.5,1};

    float p1_trans[4];
    float p2_trans[4];
    float p3_trans[4];
    float p4_trans[4];

    transform(mat, p1, p1_trans);
    transform(mat, p2, p2_trans);
    transform(mat, p3, p3_trans);
    transform(mat, p4, p4_trans);

    EFI_GRAPHICS_OUTPUT_BLT_PIXEL green = color(0,255,0);
    drawLine(p1_trans[0], p1_trans[1], p2_trans[0], p2_trans[1], green);
    drawLine(p1_trans[0], p1_trans[1], p3_trans[0], p3_trans[1], green);
    drawLine(p2_trans[0], p2_trans[1], p4_trans[0], p4_trans[1], green);
    drawLine(p3_trans[0], p3_trans[1], p4_trans[0], p4_trans[1], green);

}


MAP * createMap(EFI_UINT32 width, EFI_UINT32 height)
{
    MAP * map = (MAP *) uefi_malloc(sizeof(MAP) + sizeof(EFI_UINT8) * width * height);
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
