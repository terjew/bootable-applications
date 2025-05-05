#ifndef MEMORY_H
#define MEMORY_H

#include "lil_uefi/lil_uefi.h"

EFI_BOOT_SERVICES *boot_services;

void initialize_memory(EFI_BOOT_SERVICES *boot_services_ptr){
    boot_services = boot_services_ptr;
}

void * malloc(EFI_UINTN poolSize)
{
    EFI_STATUS error;
    void * handle;
    error = boot_services->AllocatePool(EFI_MEMORY_TYPE_EfiLoaderData, poolSize, &handle);
    return error ? 0 : handle;
}

void free(void * pool)
{
    boot_services->FreePool(pool);
}

void * memcpy(void * dst, const void * src, unsigned long long len)
{
    boot_services->CopyMem(dst, (void*)src, len);
    return dst;
}

#endif