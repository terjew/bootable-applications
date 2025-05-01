#include "lil_uefi/lil_uefi.h"
#include "shared/rand.h"
#include "shared/drawing.h"
#include "shared/math.h"
#include "shared/colorspace.h"

//https://lodev.org/cgtutor/plasma.html
#define dist(a, b, c, d) sqrt((double)((a - c) * (a - c) + (b - d) * (b - d)))
void plasma(double time)
{
    clear(color(0,0,0));
    int w = 320;
    int h = 240;
    for(int y = 0; y < h; y+=2)
    for(int x = 0; x < w; x+=2)
    {
      double value = //0..4
            sin(dist(x + time, y, 128.0, 128.0) / 8.0)
             + sin(dist(x, y, 64.0, 64.0) / 8.0)
             + sin(dist(x, y + time / 7, 192.0, 64) / 7.0)
             + sin(dist(x, y, 192.0, 100.0) / 8.0);

      Color_HSVA hsva;
      hsva.h = (unsigned char)(value * 90);
      hsva.s = 255;
      hsva.v = 255;
      hsva.a = 255;
      Color_BGRA bgra = HsvToRgb(hsva);
      EFI_GRAPHICS_OUTPUT_BLT_PIXEL * pixel = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) (unsigned int *)&bgra;
      int y2 = y * 2;
      int x2 = x * 2;
      pixels[y2 * width + x2] = *pixel;
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

    double t = 2;
    int t2 = 0;
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
            case 'q':
                clear(color(240, 127, 34));
                break;
            default:              
                clear(randomColor());
                break;
            case 'p':
                plasma(t);
                t += 3.0f;
                break;
        }
    }

end:
    return (0);
}
