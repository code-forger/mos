#pragma once
#include "../declarations.h"
#include "../paging/paging.h"

//** A SIGNIFICANT PORTION OF THE FOLLOWING CODE WAS COLLECTED FROM EXTERNAL SOURCES **/

#define HDD_CS_ERR     0x01
#define HDD_CS_IDX     0x02
#define HDD_CS_CORR    0x04
#define HDD_CS_DRQ     0x08
#define HDD_CS_DSC     0x10
#define HDD_CS_DF      0x20
#define HDD_CS_DRDY    0x40
#define HDD_CS_BSY     0x80

#define HDD_FE_AMNF     0x01
#define HDD_FE_TK0NF    0x02
#define HDD_FE_ABRT     0x04
#define HDD_FE_MCR      0x08
#define HDD_FE_MC       0x20
#define HDD_FE_IDNF     0x10
#define HDD_FE_UNC      0x40
#define HDD_FE_BBK      0x80

#define HDD_CMD_READ_PIO          0x20
#define HDD_CMD_READ_PIO_EXT      0x24
#define HDD_CMD_READ_DMA          0xC8
#define HDD_CMD_READ_DMA_EXT      0x25
#define HDD_CMD_WRITE_PIO         0x30
#define HDD_CMD_WRITE_PIO_EXT     0x34
#define HDD_CMD_WRITE_DMA         0xCA
#define HDD_CMD_WRITE_DMA_EXT     0x35
#define HDD_CMD_CACHE_FLUSH       0xE7
#define HDD_CMD_CACHE_FLUSH_EXT   0xEA
#define HDD_CMD_PACKET            0xA0
#define HDD_CMD_IDENTIFY_PACKET   0xA1
#define HDD_CMD_IDENTIFY          0xEC

#define HDD_CMD_READ       0xA8
#define HDD_CMD_EJECT      0x1B

#define HDD_IDENT_DEVICETYPE   0
#define HDD_IDENT_CYLINDERS    2
#define HDD_IDENT_HEADS        6
#define HDD_IDENT_SECTORS      12
#define HDD_IDENT_SERIAL       20
#define HDD_IDENT_MODEL        54
#define HDD_IDENT_CAPABILITIES 98
#define HDD_IDENT_FIELDVALID   106
#define HDD_IDENT_MAX_LBA      120
#define HDD_IDENT_COMMANDSETS  164
#define HDD_IDENT_MAX_LBA_EXT  200

#define IDE_ATA        0x00
#define IDE_ATAPI      0x01

#define ATA_CONTROLLER     0x00
#define ATA_RESPONDER      0x01

#define HDD_REG_DATA       0x00
#define HDD_REG_ERROR      0x01
#define HDD_REG_FEATURES   0x01
#define HDD_REG_SECCOUNT0  0x02
#define HDD_REG_LBA0       0x03
#define HDD_REG_LBA1       0x04
#define HDD_REG_LBA2       0x05
#define HDD_REG_HDDEVSEL   0x06
#define HDD_REG_COMMAND    0x07
#define HDD_REG_STATUS     0x07
#define HDD_REG_SECCOUNT1  0x08
#define HDD_REG_LBA3       0x09
#define HDD_REG_LBA4       0x0A
#define HDD_REG_LBA5       0x0B
#define HDD_REG_CONTROL    0x0C
#define HDD_REG_ALTSTATUS  0x0C
#define HDD_REG_DEVADDRESS 0x0D

// Channels:
#define HDD_PRIMARY      0x00
#define HDD_SECONDARY    0x01

// Directions:
#define HDD_READ      0x00
#define HDD_WRITE     0x01

struct channel {
   uint16_t addr;
   uint16_t ctrl;
   uint16_t controller;
   uint8_t  noint;
};

struct hdd {
   uint8_t  reserved;    // 0 (Empty) or 1 (This Drive really exists).
   uint8_t  channel;     // 0 (Primary Channel) or 1 (Secondary Channel).
   uint8_t  drive;       // 0 (Primary Drive) or 1 (Secondary Drive).
   uint16_t type;        // 0: ATA, 1:ATAPI.
   uint16_t signature;   // Drive Signature
   uint16_t capabilities;// Features.
   uint32_t commandSets; // Command Sets Supported.
   uint32_t size;        // Size in Sectors.
   char  model[41];   // Model in string.
};

void hdd_init();

//** A SIGNIFICANT PORTION OF THE PREVIOUS CODE WAS COLLECTED FROM EXTERNAL SOURCES **/
//** the following caching code was written by me.

struct block_meta
{
   uint32_t cache;
   uint32_t dirty;
};

struct block
{
   uint8_t data[512];
};

//(31 * 8) + (33 * 8) + (31 * 512) == 4096*4 == 0x4000
typedef struct block_table_type
{
    struct block_meta blocks[31];
    struct block_meta padding[33];
    struct block      block[31];
} block_table_type;

void hdd_write_cache();

void hdd_write(uint8_t b);
uint8_t hdd_read();

uint32_t hdd_remaining();
void hdd_reset();
void hdd_seek(uint32_t index);
uint32_t hdd_current();
uint32_t hdd_capacity();