#include "lil_uefi/lil_uefi.h"
#include "shared/drawing.h"
#include "shared/lineset.h"
#include "sprites/teapot.h"

#define NULL 0

LINESET * teapot_data = NULL;
SPRITE * backBuffer = NULL;
void teapot(float zdeg)
{
    if (teapot_data == NULL)
    {
        teapot_data = createLineset(teapotVertsCount, teapotFaceIndexesCount, 3, 1);
        memcpy(teapot_data->vertices, teapotVerts, teapotVertsCount * sizeof(float) * 3);
        memcpy(teapot_data->lines, teapotFaceIndexes, teapotFaceIndexesCount * sizeof(int) * 3);  
        backBuffer = createSprite(width, height);
    }
    
    float mat[4][4];
    make_identity(mat);
    rotateY(mat, degToRad(zdeg));
    rotateX(mat, degToRad(-135));
    scale(mat, 300, 300, 300);
    translate(mat, 320, 300, 0);
    
    //clear backbuffer:
    fill(backBuffer->buffer, backBuffer->width * backBuffer->height, color(0,0,0));

    //render lines to backbuffer:
    renderLineset(teapot_data, mat, backBuffer->buffer, backBuffer->width, color(255, 200, 200));

    //render backbuffer to display:
    drawSprite(0, 0, backBuffer);
}

EFI_UINTN EfiMain(EFI_HANDLE handle, EFI_SYSTEM_TABLE *system_table)
{
    EFI_BOOT_SERVICES *boot_services = system_table->BootServices;
    EFI_STATUS status;

    initialize_memory(boot_services);

    EFI_GUID gfx_out_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gfx_out_prot;
    status = boot_services->LocateProtocol(&gfx_out_guid, 0, (void **)&gfx_out_prot);
    if (status != 0)
    {
        return status;
    }
    gfx_out_prot->SetMode(gfx_out_prot, 0);

    initialize_drawing(gfx_out_prot);

    double t = 0;
    teapot(t);

    EFI_UINTN index;
    EFI_EVENT loopEvent;
    int second = 10000000;
    int fps = 60;
    status = boot_services->CreateEvent(EFI_EVT_TIMER, EFI_TPL_CALLBACK, NULL, (void *)NULL, &loopEvent);
    status = boot_services->SetTimer(loopEvent, EFI_TIMER_DELAY_Periodic, second/fps);
    for (;;)
    {
        status = boot_services->WaitForEvent(1, &loopEvent, &index);
        teapot(t);
        t += 0.2f;
    }

    return 0;
}
