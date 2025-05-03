#include "lil_uefi/lil_uefi.h"
#include "shared/rand.h"
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

#define NULL 0

void drawMap(SPRITE * sprites[4])
{
    clear(color(0, 0, 0));
    for (EFI_UINT32 ty = 0; ty < 49; ty++){
        for (EFI_UINT32 tx = 0; tx < 19; tx++)
        {
            EFI_UINT32 y = ty * 8;
            EFI_UINT32 x = tx * 32;
            if ((ty % 2) != 0) x += 16;
            EFI_UINT32 coin = rand() & 0xf;
            SPRITE * sprite = sprites[coin];
            if (sprite != NULL) drawSpriteTransparent(x, y, sprite);
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
    
    // TBD: Query modes and present the available options?
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

    SPRITE * sprite = createSprite(16,16);

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

    drawMap(tiles);
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
                drawMap(tiles);
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
