#include <fs/vfs.h>
#include <fs/initrd.h>
#include <sys/logger.h>
#include <memory/kheap.h>

VFS_t *init_vfs()
{
    VFS_t *vfs = (VFS_t *)kmalloc(sizeof(VFS_t));
    if (vfs == NULL)
    {
        debug_log(__FILE__, __LINE__, __func__, "Failed to allocate memory for VFS stuct!");
        return NULL;
    }

    vfs->address = (uint64_t)vfs;
    cdebug_log(__func__, "VFS located at 0x%016X", vfs->address);

    vfs->drives = (drive_t *)kmalloc(sizeof(drive_t));
    if (vfs->drives == NULL)
    {
        debug_log(__FILE__, __LINE__, __func__, "Failed to allocate memory for VFS drives!");
        return NULL;
    }

    cdebug_log(__func__, "done.");
    return vfs;
}

vfs_op_status mount_drive(VFS_t *vfs, uint64_t driveAddr, vfs_drive_type type)
{
    if (vfs == NULL)
    {
        return STATUS_INVALID_ARGUMENTS;
    }

    drive_t *newDrive = (drive_t *)kmalloc(sizeof(drive_t));
    if (newDrive == NULL)
    {
        return STATUS_MALLOC_FAILURE;
    }

    newDrive->driveAddr = driveAddr;
    newDrive->driveType = type;

    if (type == TYPE_INITRD)
    {
        cdebug_log(__func__, "Mounted Ramdisk to VFS (0x%016X(initrd) -> 0x%016X(VFS))", driveAddr, vfs->address);
    }

    if (vfs->drives == NULL)
    {
        vfs->drives = (drive_t *)kmalloc(sizeof(drive_t));
        if (vfs->drives == NULL)
        {
            kfree(newDrive);
            return STATUS_MALLOC_FAILURE;
        }
    }
    else
    {
        drive_t *temp = (drive_t *)krealloc(vfs->drives, (vfs->numDrives + 1) * sizeof(drive_t));
        if (temp == NULL)
        {
            kfree(newDrive);
            return STATUS_MALLOC_FAILURE;
        }
        vfs->drives = temp;
    }

    vfs->drives[vfs->numDrives++] = *newDrive;
    kfree(newDrive);
    return STATUS_OK;
}

vfs_op_status umount_drive(VFS_t *vfs, int driveId)
{
    if (vfs == NULL || driveId < 0 || driveId >= vfs->numDrives)
    {
        return STATUS_INVALID_ARGUMENTS;
    }

    drive_t *driveToRemove = &(vfs->drives[driveId]);

    switch (driveToRemove->driveType)
    {
    case TYPE_INITRD:
        cdebug_log(__func__, "Unmounting Ramdisk from VFS (0x%016X)", driveToRemove->driveAddr);
        break;
    default:
        break;
    }

    for (int i = driveId; i < vfs->numDrives - 1; i++)
    {
        vfs->drives[i] = vfs->drives[i + 1];
    }

    vfs->numDrives--;

    if (vfs->numDrives > 0)
    {
        drive_t *temp = (drive_t *)krealloc(vfs->drives, vfs->numDrives * sizeof(drive_t));
        if (temp != NULL)
        {
            vfs->drives = temp;
        }
    }
    else
    {
        kfree(vfs->drives);
        vfs->drives = NULL;
    }

    return STATUS_OK;
}

vfs_op_status drive_read(VFS_t *vfs, int driveId, char *fileName, char **out)
{
    if (vfs == NULL)
    {
        debug_log(__FILE__, __LINE__, __func__, "A null pointer to the VFS was passed!");
        return STATUS_INVALID_ARGUMENTS;
    }

    if (driveId > vfs->numDrives)
    {
        debug_log(__FILE__, __LINE__, __func__, "Invalid driveId passed!");
        return STATUS_INVALID_ARGUMENTS;
    }

    drive_t *temp = &vfs->drives[driveId];

    switch (temp->driveType)
    {
    case TYPE_INITRD:
        uint32_t hash = hash_string(fileName);
        int fileId = find_file_by_hash((Ramdisk *)temp->driveAddr, hash);
        RamdiskEntry *tempEntry = (RamdiskEntry *)((Ramdisk *)temp->driveAddr)->content[fileId];
        *out = (char *)kmalloc(tempEntry->file->size);

        for (int i = 0; i < tempEntry->file->size; ++i)
        {
            (*out)[i] = tempEntry->file->content[i];
        }

        cdebug_log(__func__, "done.");
        break;
    default:
        debug_log(__FILE__, __LINE__, __func__, "Invalid drive type !");
        return STATUS_INVALID_DRIVE_TYPE;
    }

    return STATUS_OK;
}