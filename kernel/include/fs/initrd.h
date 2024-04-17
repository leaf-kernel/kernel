#ifndef __INITRD_H__
#define __INITRD_H__

#include <fs/tar.h>
#include <fs/path.h>
#include <stddef.h>
#include <stdint.h>

typedef struct
{
    TARFile *file;
    uint32_t hash;
    PathComponent *path;
} RamdiskEntry;


typedef struct
{
    RamdiskEntry** content;
} Ramdisk;

Ramdisk *init_ramdisk(const char *raw, const size_t size);

#endif // __INITRD_H__