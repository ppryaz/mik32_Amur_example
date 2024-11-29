#include "mik32fat.h"


FAT_Status_t MIK32FAT_Init(FAT_Descriptor_t* fs, SD_Descriptor_t* sd_card)
{
    //TODO: сделать проверу томов на форматированность FAT32
    fs->card = sd_card;
    /* Read Master boot record */
    if (SD_SingleRead(fs->card, 0, fs->buffer) != 0) return FAT_DiskError;
    /* Read LBAs */
    uint8_t counter = 0;
    uint8_t type_code;
    uint8_t* ptr = fs->buffer + FAT_MBR_Partition0;
    /* Find not-empty partition */
    while (counter < 4)
    {
        type_code = ptr[FAT_Partition_TypeCode];
        /* 0x0B - FAT32 fs, 0x0C - FAT32 with LFN */
        if ((type_code == 0x0B) || (type_code == 0x0C)) break;
        counter += 1;
        ptr += FAT_MBR_Partition_Length;
    }
    if (counter == 4) return FAT_DiskNForm;
    /* Read LBA startaddr. It is a start address of file system */
    fs->fs_begin = (uint32_t)(ptr[FAT_Partition_LBA_Begin+3]<<24) |
        (uint32_t)(ptr[FAT_Partition_LBA_Begin+2]<<16) |
        (uint32_t)(ptr[FAT_Partition_LBA_Begin+1]<<8) |
        (uint32_t)ptr[FAT_Partition_LBA_Begin];
    /* Read LBA sector */
    if (SD_SingleRead(fs->card, fs->fs_begin, fs->buffer) != 0) return FAT_DiskError;
    fs->prev_sector = fs->fs_begin;

    fs->param.sec_per_clust = fs->buffer[FAT_BPB_SecPerClus];
    /* Read number of sectors of reserved file system's region */
    uint16_t num_of_res_sec = (uint16_t)(fs->buffer[FAT_BPB_RsvdSecCnt+1]<<8) |
        fs->buffer[FAT_BPB_RsvdSecCnt];
    /* Read FAT's startaddesses and length */
    fs->fat1_begin = fs->fs_begin + num_of_res_sec;
    fs->param.num_of_fats = fs->buffer[FAT_BPB_NumFATs];
    fs->param.fat_length = (uint32_t)(fs->buffer[FAT_BPB_FATSz32+3]<<24) |
        (uint32_t)(fs->buffer[FAT_BPB_FATSz32+2]<<16) |
        (uint32_t)(fs->buffer[FAT_BPB_FATSz32+1]<<8) |
        (uint32_t)(fs->buffer[FAT_BPB_FATSz32]);
    if (fs->param.num_of_fats == 2) fs->fat2_begin = fs->fat1_begin + fs->param.fat_length;
    fs->param.clust_len = 512 * fs->param.sec_per_clust;
    /* Calculate a start address of file system's data region */
    fs->data_region_begin = fs->fat1_begin + fs->param.num_of_fats * fs->param.fat_length;
    return FAT_OK;
}


/**
 * @brief Set file system temp-pointer [fs.temp.cluster] to root directory
 * @param fs file system's descriptor-structure
 * @return none
 */
void MIK32FAT_SetPointerToRoot(FAT_Descriptor_t* fs)
{
    //fs->temp.cluster = fs->data_region_begin;
    fs->temp.cluster = 0;
}


/**
 * @brief The function finds the cluster next to [fs.temp.cluster] cluster and puts
 * number of new cluster to [fs.temp.cluster]
 * @param fs file system's descriptor-structure
 * @returns
 * - FAT_OK the next cluster was found succesfully, its pointer was saved to [fs.temp.cluster]
 * - FAT_DiskError - the driver error occured, [fs.temp.cluster] not changed
 * - FAT_NotFound - there are not any next clusters, [fs.temp.cluster] not changed
 */
FAT_Status_t MIK32FAT_FindNextCluster(FAT_Descriptor_t* fs)
{
    /* Read FAT */
    uint32_t bias = (fs->temp.cluster / (512/4));
    if (SD_SingleRead(fs->card, fs->fat1_begin + bias, fs->buffer) != 0) return FAT_DiskError;
    fs->prev_sector = fs->fat1_begin + bias;
    /* Read field */
    uint32_t* ptr = (uint32_t*)fs->buffer;
    uint32_t link = ptr[fs->temp.cluster % (512/4)];
    if ((link & 0x0FFFFFFF) >= 0x0FFFFFF7) return FAT_NotFound;
    else
    {
        fs->temp.cluster = link;
        return FAT_OK;
    }
}


/**
 * @brief Find the number of 1st cluster of file/subdirectory in the directory [fs.temp.cluster]
 * and puts it to [fs.temp.cluster]
 * @param fs pointer to file system's structure-descriptor
 * @param name string of name. The last byte should be '\0' or '/'.
 * If the name contains a point '.' symbol, it cannot contain more than 8 meanung
 * symbols before point and more than 3 meaning symbols after point. Else, the
 * name cannot contain more than 8 meaning symbols
 * @return cluster - the 1st cluster of file/dir. dir_sector - the number of sector of directory.
 * len - length of file. entire_in_dir_clust - number of entire in dir_sector. status - status of file/dir
 */
FAT_Status_t MIK32FAT_FindByName(FAT_Descriptor_t* fs, char* name)
{
    char name_str[11];
    /* Preparing the name string */
    uint8_t pos = 0;
    bool ready = false;
    while ((name[pos] != '\0') && (name[pos] != '/') && !ready)
    {
        /* The point symbol has been found */
        if (name[pos] == '.')
        {
            if (pos > 8) return FAT_Error;
            memcpy(name_str, name, pos);
            for (uint8_t i=pos; i<8; i++) name_str[i] = 0x20;
            uint8_t i=0;
            while ((name[pos+1+i] != '\0') && (name[pos+1+i] != '/'))
            {
                if (pos+i > 11) return FAT_Error;
                name_str[8+i] = name[pos+1+i];
                i += 1;
            }
            for ((void)i; i<11-8; i++) name_str[i] = 0x20;
            ready = true;
        }
        pos += 1;
    }
    /* The point symbol has not been found */
    if (!ready)
    {
        uint8_t i=0;
        while ((name[i] != '\0') && (name[i] != '/'))
        {
            if (i > 8) return FAT_Error;
            name_str[i] = name[i];
            i += 1;
        }
        for ((void)i; i<11; i++) name_str[i] = 0x20;
    }
    //xprintf("\n*%s*\n", name_str);
    /* Finding process */
    FAT_Status_t res = FAT_OK;
    while (res == FAT_OK)
    {
        uint32_t sector;
        for (uint8_t sec=0; sec < fs->param.sec_per_clust; sec++)
        {
            /* Read sector data */
            sector = fs->data_region_begin + fs->temp.cluster * fs->param.sec_per_clust + sec;
            /* Read sector only if has not already been buffered */
            if (sector != fs->prev_sector)
            {
                if (SD_SingleRead(fs->card, sector, fs->buffer) != SD_OK) return FAT_DiskError;
                fs->prev_sector = sector;
            }
            //xprintf("\n*%u*\n", cluster);
            /* Try to find the name in sector */
            uint16_t entire = 0;
            while ((entire < 512) && memcmp(name_str, fs->buffer+entire, 11)) entire += 32;
            if (entire == 512) continue;
            /* The correct name has been found */
            /* Save parameters */
            fs->temp.entire_in_dir_clust = entire;
            fs->temp.dir_sector = fs->temp.cluster * fs->param.sec_per_clust + sec;
            fs->temp.cluster = (uint32_t)fs->buffer[entire+FAT_DIR_FstClusHI+0]<<16 |
                (uint32_t)fs->buffer[entire+FAT_DIR_FstClusHI+1]<<24 |
                (uint32_t)fs->buffer[entire+FAT_DIR_FstClusLO+0] |
                (uint32_t)fs->buffer[entire+FAT_DIR_FstClusLO+1]<<8;
            fs->temp.cluster -= 2;
            fs->temp.len = (uint32_t)fs->buffer[entire+FAT_DIR_FileSize+0] |
                (uint32_t)fs->buffer[entire+FAT_DIR_FileSize+1]<<8 |
                (uint32_t)fs->buffer[entire+FAT_DIR_FileSize+2]<<16 |
                (uint32_t)fs->buffer[entire+FAT_DIR_FileSize+3]<<24;
            fs->temp.status = fs->buffer[entire+FAT_DIR_Attr];
            return FAT_OK;
        }
        res = MIK32FAT_FindNextCluster(fs);
    }
    return res;
}


/**
 * @brief Find the number of 1st cluster of file by the path
 * @param fs pointer to file system's structure-descriptor
 * @param path string of path. The last byte should be '\0'.
 * If the path contain subdirectories, they are separated by '/' symbol (i.e.: "FOLDER/FILE").
 * If the name of file or subdir contains a point '.' symbol, it cannot contain more than 8 meanung
 * symbols before point and more than 3 meaning symbols after point. Else, the
 * name cannot contain more than 8 meaning symbols
 * @return cluster - the 1st cluster of file/dir. dir_sector - the number of sector of directory.
 * len - length of file. entire_in_dir_clust - number of entire in dir_sector. status - status of file/dir
 */
FAT_Status_t MIK32FAT_FindByPath(FAT_Descriptor_t* fs, char* path)
{
    /* calculate number of '/' symbols */
    uint8_t descend_number = 1;
    uint8_t i = 0;
    while (path[i] != '\0')
    {
        if (path[i] == '/') descend_number += 1;
        i += 1;
    }
    /* Descend into directories and files */
    FAT_Status_t res;
    char* ptr = path;
    for (uint8_t k=0; k<descend_number; k++)
    {
        res = MIK32FAT_FindByName(fs, ptr);
        if (res != FAT_OK) return res;
        /* Find next name in path */
        while (*ptr != '/') ptr += 1;
        ptr += 1;
    }
    return FAT_OK;
}


/**
 * @brief Find the number of 1st cluster of the file by path. If there are
 * not any subdirectories of file in the path, the function creates them 
 * @param fs pointer to file system's structure-descriptor
 * @param path string of path. The last byte should be '\0'.
 * If the path contain subdirectories, they are separated by '/' symbol (i.e.: "FOLDER/FILE").
 * If the name of file or subdir contains a point '.' symbol, it cannot contain more than 8 meanung
 * symbols before point and more than 3 meaning symbols after point. Else, the
 * name cannot contain more than 8 meaning symbols
 * @return cluster - the 1st cluster of file/dir. dir_sector - the number of sector of directory.
 * len - length of file. entire_in_dir_clust - number of entire in dir_sector. status - status of file/dir
 */
FAT_Status_t MIK32FAT_FindOrCreateByPath(FAT_Descriptor_t* fs, char* path)
{
    /* Adopted FAT_FBP. If dir/file not found, create it */
    /* calculate number of '/' symbols */
    uint8_t descend_number = 1;
    uint8_t i = 0;
    while (path[i] != '\0')
    {
        if (path[i] == '/') descend_number += 1;
        i += 1;
    }
    /* Descend into directories and files */
    FAT_Status_t res;
    bool not_found = false;
    char* ptr = path;
    for (uint8_t k=0; k<descend_number; k++)
    {
        if (!not_found)
        {
            res = MIK32FAT_FindByName(fs, ptr);
            if (res == FAT_NotFound) not_found = true;
            else if (res != FAT_OK) return res;
        }
        if (not_found)
        {
            res = MIK32FAT_Create(fs, ptr, k != (descend_number-1));
            if (res != FAT_OK) return res;
            /* Descend into created object */
            res = MIK32FAT_FindByName(fs, ptr);
            if (res != FAT_OK) return res;
        }
        while (*ptr != '/') ptr += 1;
        ptr += 1;
    }
    return FAT_OK;
}


/**
 * @brief Finding new free cluster
 * @param fs pointer to file system's structure-descriptor
 * @param new_cluster pointer to new cluster variable
 * @returns
 * - FAT_OK
 * - FAT_DiskError error while reading occurs
 * - FAT_NoFreeSpace there are not free space on the partition
 */
FAT_Status_t MIK32FAT_FindFreeCluster(FAT_Descriptor_t* fs, uint32_t* new_cluster)
{
    /* Find free cluster in FAT */
    uint32_t* ptr;
    int32_t x = -1;
    int32_t link;
    do {
        x += 1;
        if (SD_SingleRead(fs->card, fs->fat1_begin + x, fs->buffer) != SD_OK) return FAT_DiskError;
        fs->prev_sector = fs->fat1_begin + x;
        link = -4;
        do {
            link += 4;
            ptr = (uint32_t*)(fs->buffer + link);
            //xprintf("*%08X*\n", *ptr);
        } while ((*ptr != 0) && (link < 512));
    }
    while ((*ptr != 0) && (x < fs->param.fat_length));
    if (x >= fs->param.fat_length) return FAT_NoFreeSpace;
    /* link is number of free cluster in fat sector */
    /* Save number of new cluster */
    *new_cluster = (x * 128 + (link>>2));
    return FAT_OK;
}


/**
 * @brief Continue file/directory with new free cluster
 * @param fs pointer to file system's structure-descriptor
 * @param cluster temporary file/directory cluster
 * @param new_cluster pointer to new cluster variable
 * @returns
 * - FAT_OK
 * - FAT_DiskError error while reading occurs
 * - FAT_NoFreeSpace there are not free space on the partition
 */
FAT_Status_t MIK32FAT_TakeFreeCluster(FAT_Descriptor_t* fs, uint32_t cluster, uint32_t* new_cluster)
{
    // Из-за некоей подставы FAT32
    cluster += 2;
    /* Мониторим FAT */
    uint32_t* ptr;
    int32_t x = -1;
    int32_t link;
    do {
        x += 1;
        /* Read sector only if has not already been buffered */
        if (fs->fat1_begin + x != fs->prev_sector)
        {
            if (SD_SingleRead(fs->card, fs->fat1_begin + x, fs->buffer) != SD_OK) return FAT_DiskError;
            fs->prev_sector = fs->fat1_begin + x;
        }
        link = -4;
        do {
            link += 4;
            ptr = (uint32_t*)(fs->buffer + link);
            //xprintf("*%08X*\n", *ptr);
        } while ((*ptr != 0) && (link < 512));
    }
    while ((*ptr != 0) && (x < fs->param.fat_length));

    if (x >= fs->param.fat_length) return FAT_NoFreeSpace;
    /* link is number of free cluster in fat sector */
    /* Save number of new cluster */
    uint32_t new_clus = (x * 128 + (link>>2));
    *new_cluster = new_clus - 2;
    /* Find sector of FAT containing previous cluster link */
    if (fs->fat1_begin + (cluster / (512/4)) != fs->prev_sector)
    {
        if (SD_SingleRead(fs->card, fs->fat1_begin + (cluster / (512/4)), fs->buffer) != SD_OK) return FAT_DiskError;
        fs->prev_sector = fs->fat1_begin + (cluster / (512/4));
    }
    ptr = (uint32_t*)(fs->buffer + (cluster * fs->param.sec_per_clust) % 512);
    *ptr = new_clus;
    //xprintf("Write link %04X on field %04X\n", new_clus, cluster);
    if (SD_SingleErase(fs->card, fs->fat1_begin + (cluster / (512/4))) != SD_OK) return FAT_DiskError;
    if (SD_SingleWrite(fs->card, fs->fat1_begin + (cluster / (512/4)), fs->buffer) != 0) return FAT_DiskError;
    if (SD_SingleErase(fs->card, fs->fat2_begin + (cluster / (512/4))) != SD_OK) return FAT_DiskError;
    if (SD_SingleWrite(fs->card, fs->fat2_begin + (cluster / (512/4)), fs->buffer) != 0) return FAT_DiskError;
    /* Find sector of FAT containing previous cluster link */
    if (fs->fat1_begin + (new_clus / (512/4)) != fs->prev_sector)
    {
        if (SD_SingleRead(fs->card, fs->fat1_begin + (new_clus / (512/4)), fs->buffer) != SD_OK) return FAT_DiskError;
        fs->prev_sector = fs->fat1_begin + (new_clus / (512/4));
    }
    ptr = (uint32_t*)(fs->buffer + link);
    *ptr = 0x0FFFFFFF;
    //xprintf("Write link 0x0FFFFFFF on field %04X\n", new_clus);
    if (SD_SingleErase(fs->card, fs->fat1_begin + (new_clus / (512/4))) != SD_OK) return FAT_DiskError;
    if (SD_SingleWrite(fs->card, fs->fat1_begin + (new_clus / (512/4)), fs->buffer) != 0) return FAT_DiskError;
    if (SD_SingleErase(fs->card, fs->fat2_begin + (new_clus / (512/4))) != SD_OK) return FAT_DiskError;
    if (SD_SingleWrite(fs->card, fs->fat2_begin + (new_clus / (512/4)), fs->buffer) != 0) return FAT_DiskError;
    return FAT_OK;
}


FAT_Status_t MIK32FAT_FileOpen(FAT_File_t* file, FAT_Descriptor_t* fs, char* path, char modificator)
{
    file->fs = fs;
    file->modificator = modificator;
    FAT_Status_t res;
    switch (modificator)
    {
        case 'R':
            MIK32FAT_SetPointerToRoot(file->fs);
            res = MIK32FAT_FindByPath(file->fs, path);
            if (res != FAT_OK) return res;
            /* Access settings */
            file->cluster = file->fs->temp.cluster;
            file->dir_sector = file->fs->temp.dir_sector;
            file->entire_in_dir_clust = file->fs->temp.entire_in_dir_clust;
            file->status = file->fs->temp.status;
            file->addr = 0;
            file->len = file->fs->temp.len;
            break;
        case 'W':
            MIK32FAT_SetPointerToRoot(file->fs);
            res = MIK32FAT_FindOrCreateByPath(file->fs, path);
            if (res != FAT_OK) return res;
            /* Access settings */
            file->dir_sector = file->fs->temp.dir_sector;
            file->entire_in_dir_clust = file->fs->temp.entire_in_dir_clust;
            file->status = file->fs->temp.status;
            file->addr = file->fs->temp.len;
            file->len = file->fs->temp.len;
            file->writing_not_finished = false;
            do {
                res = MIK32FAT_FindNextCluster(file->fs);
                //xprintf("Cluster change, status: %u\n", res);
            } while (res == FAT_OK);
            if (res != FAT_NotFound) return res;
            file->cluster = file->fs->temp.cluster;
            break;
        case 'A':
            MIK32FAT_SetPointerToRoot(file->fs);
            res = MIK32FAT_Delete(file->fs, path);
            if ((res != FAT_OK) && (res != FAT_NotFound)) return res;
            MIK32FAT_SetPointerToRoot(file->fs);
            res = MIK32FAT_FindOrCreateByPath(file->fs, path);
            if (res != FAT_OK) return res;
            /* Access settings */
            file->cluster = file->fs->temp.cluster;
            file->dir_sector = file->fs->temp.dir_sector;
            file->entire_in_dir_clust = file->fs->temp.entire_in_dir_clust;
            file->status = file->fs->temp.status;
            file->addr = 0;
            file->len = 0;
            file->writing_not_finished = false;
            break;
        default: return FAT_Error;
    }
    return FAT_OK;
}



/**
 * @brief File closing. It is necessary if file was written.
 * @param file pointer to file's structure-descriptor
 * @returns
 */
FAT_Status_t MIK32FAT_FileClose(FAT_File_t* file)
{
    if ((file->modificator == 'W') || (file->modificator == 'A'))
    {
        uint32_t sector;
        /* if writing_not_finished flag is set, write data into SD */
        if (file->writing_not_finished == true)
        {
            file->writing_not_finished = false;
            uint32_t clust_len = 512 * file->fs->param.sec_per_clust;
            sector = file->fs->data_region_begin + file->cluster * file->fs->param.sec_per_clust + (file->addr % clust_len) / 512;
            //xprintf("Write sector: %u", sector);
            if (SD_SingleErase(file->fs->card, sector) != SD_OK) return FAT_DiskError;
            if (SD_SingleWrite(file->fs->card, sector, file->fs->buffer) != SD_OK) return FAT_DiskError;
        }
        /* Write new file length */
        sector = file->fs->data_region_begin + file->dir_sector;
        if (SD_SingleRead(file->fs->card, sector, file->fs->buffer) != SD_OK) return FAT_DiskError;
        file->fs->prev_sector = sector;
        uint32_t* ptr = (uint32_t*)&file->fs->buffer[file->entire_in_dir_clust + FAT_DIR_FileSize];
        *ptr = file->len;
        if (SD_SingleErase(file->fs->card, sector) != SD_OK) return FAT_DiskError;
        if (SD_SingleWrite(file->fs->card, sector, file->fs->buffer) != 0) return FAT_DiskError;
    }
    return FAT_OK;
}




/**
 * @brief Read data from the file
 * @param file pointer to file's structure-descriptor
 * @param buf buffer for data
 * @param quan number of bytes to read
 * @return number of read bytes
 */
uint32_t MIK32FAT_ReadFile(FAT_File_t* file, char* buf, uint32_t quan)
{
    if (file->modificator != 'R') return 0;
    uint32_t counter = 0;
    uint32_t start_addr = (file->addr - file->addr % (512 * file->fs->param.sec_per_clust));

    while ((quan > 0) && (file->len > 0))
    {
        /* Cluster */
        uint32_t new_cluster = (file->addr - start_addr) / (512 * file->fs->param.sec_per_clust);
        if (new_cluster != 0) start_addr += 512 * file->fs->param.sec_per_clust;
        file->fs->temp.cluster = file->cluster;
        while (new_cluster > 0)
        {
            MIK32FAT_FindNextCluster(file->fs);
            new_cluster -= 1;
        }
        file->cluster = file->fs->temp.cluster;
        /* Read sector data */
        uint32_t sector = file->fs->data_region_begin + file->cluster * file->fs->param.sec_per_clust +
            ((file->addr - start_addr) / 512);
        /* Read sector only if has not already buffered */
        if (file->fs->prev_sector != sector)
        {
            if (SD_SingleRead(file->fs->card, sector, file->fs->buffer) != 0) return counter;
            file->fs->prev_sector = sector;
        }
        //xprintf("*%u*\n", sector);
        /* Reading sector */
        uint16_t x = file->addr % 512;
        while ((x < 512) && (quan > 0) && (file->len > 0))
        {
            buf[counter] = file->fs->buffer[x];
            counter += 1;
            x += 1;
            quan -= 1;
            file->addr += 1;
            file->len -= 1;
        }
    }
    return counter;
}



/**
 * @brief Writing file into SD card
 * @param file pointer to file's structure-descriptor
 * @param buf buffer for data
 * @param quan number of bytes to write
 * @return number of writen bytes
 */
uint32_t MIK32FAT_WriteFile(FAT_File_t* file, const char* buf, uint32_t quan)
{
    if ((file->modificator != 'W') && (file->modificator != 'A')) return 0;
    /* Index of buffer */
    uint32_t buf_idx = file->addr % 512;
    /* Number of written bytes */
    uint32_t counter = 0;
    //uint32_t clust_len = 512 * file->fs->param.sec_per_clust;

    while (quan > counter)
    {
        /* if writing is not started with 0 fs buffer address, read sector data to not lost previously written data */
        if ((buf_idx != 0) && (file->writing_not_finished == false))
        {
            uint32_t sector = file->fs->data_region_begin + file->cluster * file->fs->param.sec_per_clust + (file->addr % file->fs->param.clust_len) / 512;
            if (SD_SingleRead(file->fs->card, sector, file->fs->buffer) != SD_OK) return counter;
            file->fs->prev_sector = sector;
        }
        /* if data is written into a new cluster, find and mark new cluster */
        if ((file->addr % file->fs->param.clust_len == 0) && (file->addr != 0))
        {
            //Make new cluster
            uint32_t value_buf;
            FAT_Status_t res = MIK32FAT_TakeFreeCluster(file->fs, file->cluster, &value_buf);
            //xprintf("Clus %u -> clus %u\n", file->cluster, value_buf);
            if (res != FAT_OK) return counter;
            file->cluster = value_buf;
        }
        /* Copying source data into a fs buffer */
        while ((quan > counter) && (buf_idx < 512))
        {
            file->fs->buffer[buf_idx] = buf[counter];
            counter += 1;
            buf_idx += 1;
            file->addr += 1;
        }
        /* if source data was all written, switch on writing_not_finished flag */
        if (counter == quan) file->writing_not_finished = true;
        /* if fs buffer was overloaded, save its data & reset writing_not_finished flag */
        if (buf_idx >= 512)
        {
            file->writing_not_finished = false;
            uint32_t sector = file->fs->data_region_begin + file->cluster * file->fs->param.sec_per_clust + ((file->addr-1) % file->fs->param.clust_len) / 512;
            if (SD_SingleErase(file->fs->card, sector) != SD_OK) return counter;
            if (SD_SingleWrite(file->fs->card, sector, file->fs->buffer) != SD_OK) return counter;
            //
            buf_idx = 0;
        }
    }
    file->len += counter;
    return counter;
}


/**
 * @brief File or directory creation
 * @param fs pointer to file system's structure-descriptor
 * @param path string of path. The last byte should be '\0'.
 * If the path contain subdirectories, they are separated by '/' symbol (i.e.: "FOLDER/FILE").
 * If the name of file or subdir contains a point '.' symbol, it cannot contain more than 8 meanung
 * symbols before point and more than 3 meaning symbols after point. Else, the
 * name cannot contain more than 8 meaning symbols
 * @param dir true - create directory, false - create file
 * @returns
 */
FAT_Status_t MIK32FAT_Create(FAT_Descriptor_t* fs, char* name, bool dir)
{
    /* Find free cluster in FAT */
    uint32_t new_cluster;
    uint32_t* ptr;
    int32_t x = -1;
    int32_t link;
    do {
        x += 1;
        if (SD_SingleRead(fs->card, fs->fat1_begin + x, fs->buffer) != SD_OK) return FAT_DiskError;
        fs->prev_sector = fs->fat1_begin + x;
        link = -4;
        do {
            link += 4;
            ptr = (uint32_t*)(fs->buffer + link);
            //xprintf("*%08X*\n", *ptr);
        } while ((*ptr != 0) && (link < 512));
    }
    while ((*ptr != 0) && (x < fs->param.fat_length));
    if (x >= fs->param.fat_length) return FAT_NoFreeSpace;
    /* link is number of free cluster in fat sector */
    /* Save number of new cluster */
    new_cluster = (x * 128 + (link>>2));
    *ptr = 0x0FFFFFFF;
    if (SD_SingleErase(fs->card, fs->fat1_begin + x) != SD_OK) return FAT_DiskError;
    if (SD_SingleWrite(fs->card, fs->fat1_begin + x, fs->buffer) != SD_OK) return FAT_DiskError;
    if (SD_SingleErase(fs->card, fs->fat2_begin + x) != SD_OK) return FAT_DiskError;
    if (SD_SingleWrite(fs->card, fs->fat2_begin + x, fs->buffer) != SD_OK) return FAT_DiskError;

    /* Set /. and /.. entires in new cluster if directory (look MS FAT Specification) */
    if (dir == true)
    {
        memset(fs->buffer, 0x00, 512);
        FAT_Entire_t* ent = (FAT_Entire_t*)fs->buffer;
        memcpy(ent[0].Name, ".          ", 11);
        ent[0].Attr = 0x10;
        ent[0].FstClusLO = (uint16_t)new_cluster;
        ent[0].FstClusHI = (uint16_t)(new_cluster >> 16);
        memcpy(ent[1].Name, "..         ", 11);
        ent[1].Attr = 0x10;
        ent[1].FstClusLO = 0;
        ent[1].FstClusHI = 0;
        /* Write data */
        if (SD_SingleErase(fs->card, fs->data_region_begin+(new_cluster-2)*fs->param.sec_per_clust) != SD_OK) return FAT_DiskError;
        if (SD_SingleWrite(fs->card, fs->data_region_begin+(new_cluster-2)*fs->param.sec_per_clust, fs->buffer) != SD_OK) return FAT_DiskError;
    }

    /* Find free space for descriptor in directory */
    uint32_t sector;
    uint16_t entire;
    FAT_Status_t res = FAT_OK;
    while (res == FAT_OK)
    {
        sector = fs->data_region_begin + fs->temp.cluster * fs->param.sec_per_clust;
        for (uint8_t idx=0; idx < fs->param.sec_per_clust; idx++)
        {
            if (SD_SingleRead(fs->card, sector, fs->buffer) != SD_OK) return FAT_DiskError;
            fs->prev_sector = sector;
            sector += 1;
            entire = 0;
            while (entire < 512)
            {
                if ((fs->buffer[entire] == 0x00) || (fs->buffer[entire] == 0xE5)) break;
                entire += 32;
            }
            if (entire < 512)
            {
                sector -= 1;
                break;
            }
        }
        if (entire < 512) break;
        res = MIK32FAT_FindNextCluster(fs);
    }
    /* FAT_FNC error. if next cluster not found, take a free cluster */
    if (res == FAT_NotFound)
    {
        uint32_t value;
        res = MIK32FAT_TakeFreeCluster(fs, fs->temp.cluster, &value);
        if (res != FAT_OK) return res;
        entire = 0;
        sector = fs->data_region_begin + value * fs->param.sec_per_clust;
        //if (SD_SingleRead(fs->card, sector, fs->buffer) != SD_OK) return FAT_DiskError;
    }
    else if (res != FAT_OK) return res;
    /* entire contains pointer to descriptor in directory's sector (sector) */
    memset(fs->buffer+entire+11, 0x00, 32-11);

    if (SD_SingleRead(fs->card, sector, fs->buffer) != SD_OK) return FAT_DiskError;
    fs->prev_sector = sector;
    FAT_Entire_t* new_obj = (FAT_Entire_t*)(fs->buffer + entire);
    new_obj->FileSize = 0;
    new_obj->Attr = (dir ? FAT_ATTR_DIRECTORY : 0);
    new_obj->FstClusLO = (uint16_t)new_cluster;
    new_obj->FstClusHI = (uint16_t)(new_cluster >> 16);
    memset(new_obj->Name, 0x20, 8);
    memset(new_obj->Extention, 0x20, 3);
    uint8_t i=0;
    while ((name[i] != '.') && (name[i] != '\0') && (name[i] != '/') && (i<8))
    {
        new_obj->Name[i] = name[i];
        i += 1;
    }
    if ((name[i] != '\0') && (name[i] != '/') && (i < 8))
    {
        i += 1;
        uint8_t j=0;
        while ((name[i] != '\0') && (name[i] != '/') && (j<3))
        {
            new_obj->Extention[j] = name[i];
            i += 1;
            j += 1;
        }
    }
    if (SD_SingleErase(fs->card, sector) != SD_OK) return FAT_DiskError;
    if (SD_SingleWrite(fs->card, sector, fs->buffer) != SD_OK) return FAT_DiskError;
    return FAT_OK;
}


FAT_Status_t MIK32FAT_DeleteTemp(FAT_Descriptor_t* fs)
{
    uint32_t sector;
    /* Set the 0th byte of entire as 0xE5 */
    sector = fs->data_region_begin + fs->temp.dir_sector;
    /* Read sector only if has not already been buffered */
    if (sector != fs->prev_sector)
    {
        if (SD_SingleRead(fs->card, sector, fs->buffer) != SD_OK) return FAT_DiskError;
        fs->prev_sector = sector;
    }
    if ((fs->buffer[fs->temp.entire_in_dir_clust] & ~0xE5) != 0)
    {
        if (SD_SingleErase(fs->card, sector) != SD_OK) return FAT_DiskError;
    }
    fs->buffer[fs->temp.entire_in_dir_clust] = 0xE5;
    if (SD_SingleWrite(fs->card, sector, fs->buffer) != SD_OK) return FAT_DiskError;

    /* Clear link to all file's clusters in FATs */
    uint32_t cluster = fs->temp.cluster + 2;
    uint32_t* ptr;
    sector = 0;
    do {
        if (sector != fs->fat1_begin + (cluster / (512/4)))
        {
            uint32_t dummy = sector;
            sector = fs->fat1_begin + (cluster / (512/4));
            if (dummy != 0)
            {
                if (SD_SingleErase(fs->card, dummy) != SD_OK) return FAT_DiskError;
                if (SD_SingleWrite(fs->card, dummy, fs->buffer) != SD_OK) return FAT_DiskError;
            }
            if (SD_SingleRead(fs->card, sector, fs->buffer) != SD_OK) return FAT_DiskError;
            fs->prev_sector = sector;
        }
        ptr = (uint32_t*)(fs->buffer + (cluster * fs->param.sec_per_clust) % 512);
        cluster = *ptr;
        *ptr = 0;
    } while ((cluster & 0x0FFFFFFF) < 0x0FFFFFF7);
    return FAT_OK;
}

/**
 * @brief File or directory deletion
 * @param fs pointer to file system's structure-descriptor
 * @param path string of path. The last byte should be '\0'.
 * If the path contain subdirectories, they are separated by '/' symbol (i.e.: "FOLDER/FILE").
 * If the name of file or subdir contains a point '.' symbol, it cannot contain more than 8 meanung
 * symbols before point and more than 3 meaning symbols after point. Else, the
 * name cannot contain more than 8 meaning symbols
 * @returns
 */
FAT_Status_t MIK32FAT_Delete(FAT_Descriptor_t* fs, char* path)
{
    FAT_Status_t res;
    res = MIK32FAT_FindByPath(fs, path);
    if (res != FAT_OK) return res;
    return MIK32FAT_DeleteTemp(fs);
}
