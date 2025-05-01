#include "lil_uefi/lil_uefi.h"
#include "shared/rand.h"
#include "shared/drawing.h"
#include "shared/math.h"
#include "shared/colorspace.h"

#define NULL 0

// Inspired by https://lodev.org/cgtutor/plasma.html
#define dist(a, b, c, d) sqrt((double)((a - c) * (a - c) + (b - d) * (b - d)))
void plasma(double time)
{
    Color_HSVA hsva;
    hsva.s = 255;
    hsva.v = 255;
    hsva.a = 255;

    EFI_GRAPHICS_OUTPUT_BLT_PIXEL pixel;

    int w = width;
    int h = height;
    double xPos = w / 2 + (w * cos(time / 13));
    double yPos = h/3 + (h * cos(time / 17));

    double p1x = w * 0.25;
    double p2x = w * 0.75;
    double p3x = w * 0.75;

    double p1y = h * 0.25;
    double p2y = h * 0.375;
    double p4y = h * 0.5;

    int stepSize = 2;
    for (int y = 0; y < h; y += stepSize)
    {
        for (int x = 0; x < w; x += stepSize)
        {
            double value = 0.0 +
                           sin(dist(x, y, p1x,  p1y) / 36.0) + // constant for each x,y. Distance from a point at 0.25,0.25 in screen coords
                           sin(dist(x, y, p2x,  p2y) / 32.0) + // constant for each x,y. Distance from a point at 0.75,0.375 in screen coords
                           sin(dist(x, y, p3x, yPos) / 16.0) + // moving vertically. Distance from a moving point starting at 0.75,0.25 in screen coords
                           sin(dist(x, y, xPos, p4y) / 24.0) + // moving horizontally. Distance from a moving point starting at 0.5,0.5 in screen coords
                           0;

            hsva.h = (unsigned char)(value * 90);
            pixels[y * stride + x] = HsvToRgb(hsva);
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
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gfx_out_prot;
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

    clear(color(0, 0, 0));

    double t = 0;
    plasma(t);

    EFI_UINTN index;
    EFI_EVENT loopEvent;
    status = boot_services->CreateEvent(EFI_EVT_TIMER, EFI_TPL_CALLBACK, NULL, (void *)NULL, &loopEvent);
    status = boot_services->SetTimer(loopEvent, EFI_TIMER_DELAY_Periodic, 1);
    for (;;)
    {
        status = boot_services->WaitForEvent(1, &loopEvent, &index);
        plasma(t);
        t += 0.1f;
    }

end:
    return (0);
}
