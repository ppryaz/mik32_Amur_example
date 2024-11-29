

#include "sd.h"
#include "string.h"
#include "stdbool.h"
#include "xprintf.h"


/* Master boot record */
#define FAT_MBR_Partition0          0x1BE
#define FAT_MBR_Partition1          0x1CE
#define FAT_MBR_Partition2          0x1DE
#define FAT_MBR_Partition3          0x1EE
#define FAT_MBR_Signature           0x1FE
#define FAT_MBR_Partition_Length    16
/* Partition */
#define FAT_Partition_BootFlag      0
#define FAT_Partition_CHS_Begin     1
#define FAT_Partition_TypeCode      4
#define FAT_Partition_CHS_End       5
#define FAT_Partition_LBA_Begin     8
#define FAT_Partition_NumOfSec      12
/* File system's LBA */
#define FAT_BPB_BytsPerSec          0x0B
#define FAT_BPB_SecPerClus          0x0D
#define FAT_BPB_RsvdSecCnt          0x0E
#define FAT_BPB_NumFATs             0x10
#define FAT_BPB_FATSz32             0x24
#define FAT_BPB_RootClus            0x2C









#define FAT_DIR_Name            0
#define FAT_DIR_Attr            11
#define FAT_DIR_NTRes           12
#define FAT_DIR_CrtTimeTenth    13
#define FAT_DIR_CrtTime         14
#define FAT_DIR_CrtDate         16
#define FAT_DIR_LstAccDate      18
#define FAT_DIR_FstClusHI       20
#define FAT_DIR_WrtTime         22
#define FAT_DIR_WrtDate         24
#define FAT_DIR_FstClusLO       26
#define FAT_DIR_FileSize        28
#define FAT_ATTR_READ_ONLY      0x01
#define FAT_ATTR_HIDDEN         0x02
#define FAT_ATTR_SYSTEM         0x04
#define FAT_ATTR_VOLUME_ID      0x08
#define FAT_ATTR_DIRECTORY      0x10
#define FAT_ATTR_ARCHIVE        0x20

typedef struct
{
    char Name[8];
    char Extention[3];
    uint8_t Attr;
    uint8_t NTRes;
    uint8_t CrtTimeTenth;
    uint16_t CrtTime;
    uint16_t CrtDate;
    uint16_t LstAccDate;
    uint16_t FstClusHI;
    uint16_t WrtTime;
    uint16_t WrtDate;
    uint16_t FstClusLO;
    uint32_t FileSize;
} FAT_Entire_t;



typedef enum
{
    FAT_OK = 0,
    FAT_DiskError = 1,
    /* Disk not formatted for FAT32 */
    FAT_DiskNForm = 2,
    FAT_Error = 3,
    FAT_NotFound = 4,
    FAT_NoFreeSpace = 5,
} FAT_Status_t;



typedef struct 
{
    /* SD card descriptor */
    SD_Descriptor_t* card;
    /**
     * @brief One-sector buffer
     */
    uint8_t buffer[512];
    /**
     * The file system startaddr
     * It is a pointer to 0th cluster of file system containing information about
     */
    uint32_t fs_begin;
    /**
     * The 1st FAT startaddr
     */
    uint32_t fat1_begin;
    /**
     * The 2nd FAT startaddr
     */
    uint32_t fat2_begin;
    /**
     * The data region startaddr
     */
    uint32_t data_region_begin;
    /**
     * @brief Previously read sector
     */
    uint32_t prev_sector;
    /**
     * @brief File system parameters
     */
    struct __param {
        /**
         * @brief Number of sectors per cluster
         */
        uint8_t sec_per_clust;
        /**
         * @brief Number of FATs
         */
        uint8_t num_of_fats;
        /**
         * @brief The length of one FAT
         */
        uint32_t fat_length;
        /**
         * @brief The length of 1 cluster
         */
        uint32_t clust_len;
    } param;

    /**
     * @brief Temp object parameters
     */
    struct __temp
    {
        /**
         * Number of sector of previous directory that contains
         * file's entire
         */
        uint32_t dir_sector;
        /**
         * Number of entire of file in dir's sector
         */
        uint32_t entire_in_dir_clust;
        /**
         * Number cluster of temp cluster / subdirectory
         */
        uint32_t cluster;
        /**
         * Length of file (always 0 for directories)
         */
        uint32_t len;
        /**
         * Status of temp file / subdirectory
         */
        uint8_t status;
    } temp;
} FAT_Descriptor_t;



typedef struct 
{
    /**
     * @brief Указатель на дескриптор файловой системы
     */
    FAT_Descriptor_t* fs;
    /**
     * @brief Если бы файл был единым непрерывным массивом данных,
     * addr - это адрес, с которого начинается запись или чтение
     */
    uint32_t addr;
    /**
     * @brief Номер текущего кластера файла, Значение по адресу addr
     * попадает в текущий кластер
     */
    uint32_t cluster;
    /**
     * @brief Номер сектора директории, в котором лежит дескриптор файла
     */
    uint32_t dir_sector;
    /**
     * @brief Адрес дескриптора файла в секторе директории
     */
    uint32_t entire_in_dir_clust;
    /**
     * @brief Длина файла. При чтении декрементируется, при записи инкрементируется
     */
    uint32_t len;
    /**
     * @brief Статус файла
     */
    uint8_t status;
    /**
     * @brief Модификатор доступа к файлу
     */
    char modificator;
    /**
     * @brief
     */
    bool writing_not_finished;
} FAT_File_t;






FAT_Status_t MIK32FAT_Init(FAT_Descriptor_t* fs, SD_Descriptor_t* sd_card);

void MIK32FAT_SetPointerToRoot(FAT_Descriptor_t* local);
FAT_Status_t MIK32FAT_FindNextCluster(FAT_Descriptor_t* fs);
FAT_Status_t MIK32FAT_FindByName(FAT_Descriptor_t* local, char* name);
FAT_Status_t MIK32FAT_FindByPath(FAT_Descriptor_t* fs, char* path);
FAT_Status_t MIK32FAT_FindOrCreateByPath(FAT_Descriptor_t* fs, char* path);
FAT_Status_t MIK32FAT_TakeFreeCluster(FAT_Descriptor_t* fs, uint32_t clust, uint32_t* new_clust);
FAT_Status_t MIK32FAT_FileOpen(FAT_File_t* file, FAT_Descriptor_t* fs, char* name, char modificator);
FAT_Status_t MIK32FAT_FileClose(FAT_File_t* file);
uint32_t MIK32FAT_ReadFile(FAT_File_t* file, char* buf, uint32_t quan);
uint32_t MIK32FAT_WriteFile(FAT_File_t* file, const char* buf, uint32_t quan);
FAT_Status_t MIK32FAT_Create(FAT_Descriptor_t* fs, char* name, bool dir);
FAT_Status_t MIK32FAT_Delete(FAT_Descriptor_t* fs, char* name);
