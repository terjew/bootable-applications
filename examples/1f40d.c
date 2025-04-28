#include "lil_uefi/lil_uefi.h"
#include "shared/rand.h"
#include "shared/drawing.h"
#include "sprites/tile001.h"
#include "sprites/tile013.h"
#include "sprites/tile023.h"

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
            EFI_UINT32 coin = rand() & 0x3;
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
    // fillRandom(sprite->buffer, sprite->width * sprite->height);

    SPRITE t01 = loadSprite(32, 32, tile_001);
    SPRITE t23 = loadSprite(32, 32, tile_023);
    // SPRITE t23 = loadSprite(32, 32, tile_023);

    SPRITE * tiles[] = {
        &t01,
        &t01,
        &t23,
        NULL
    };

    clear(color(240, 127, 34));
    for (;;)
    {
        system_table->BootServices->WaitForEvent(1, &system_table->ConIn->WaitForKey, &event);
        system_table->ConIn->ReadKeyStroke(system_table->ConIn, &key);

        switch (key.UnicodeChar)
        {
            case 13: 
                goto end;
            case 'a':
                clearRandom();
                break;
            case 'r':
                drawRectangle(50, 100, 10, 20, color(255, 0, 0));
                break;      
            case 't':
                drawSprite(150, 100, sprite);
                break;
            case 'y':
                grabScreenToSprite(150, 100, sprite);
                break;
            case 'b':
                drawSpriteTransparent(250, 100, &t01);
                break;
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

end:
    return (0);
}
