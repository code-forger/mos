#include "hdd.h"
#include "port.h"
#include "terminal.h"

static struct channel channels[2];
static struct hdd drives[4];

static uint8_t ide_buffer[2048] = {0};

static void hdd_ident_write(uint8_t channel, uint8_t reg, uint8_t data) {
    if (reg > 0x07 && reg < 0x0C)
        hdd_ident_write(channel, 0x0C, 0x80 | channels[channel].noint);
    if (reg < 0x08)
        send_byte_to(channels[channel].addr  + reg - 0x00, data);
    else if (reg < 0x0C)
        send_byte_to(channels[channel].addr  + reg - 0x06, data);
    else if (reg < 0x0E)
        send_byte_to(channels[channel].ctrl  + reg - 0x0A, data);
    else if (reg < 0x16)
        send_byte_to(channels[channel].master + reg - 0x0E, data);
    if (reg > 0x07 && reg < 0x0C)
        hdd_ident_write(channel, 0x0C, channels[channel].noint);
}

uint8_t hdd_ident_read(uint8_t channel, uint8_t reg)
{
    uint8_t result;
    if (reg > 0x07 && reg < 0x0C)
        hdd_ident_write(channel, 0x0C, 0x80 | channels[channel].noint);
    if (reg < 0x08)
        result = get_byte_from(channels[channel].addr + reg - 0x00);
    else if (reg < 0x0C)
        result = get_byte_from(channels[channel].addr  + reg - 0x06);
    else if (reg < 0x0E)
        result = get_byte_from(channels[channel].ctrl  + reg - 0x0A);
    else if (reg < 0x16)
        result = get_byte_from(channels[channel].master + reg - 0x0E);
    if (reg > 0x07 && reg < 0x0C)
        hdd_ident_write(channel, 0x0C, channels[channel].noint);
    return result;
}

void hdd_ident_read_buffer(uint8_t channel, uint8_t reg, uint32_t buffer,
                     uint32_t quads) {
   if (reg > 0x07 && reg < 0x0C)
      hdd_ident_write(channel, 0x0C, 0x80 | channels[channel].noint);
   //asm("pushw %es; movw %ds, %ax; movw %ax, %es");
   if (reg < 0x08)
      insl(channels[channel].addr  + reg - 0x00, buffer, quads);
   else if (reg < 0x0C)
      insl(channels[channel].addr  + reg - 0x06, buffer, quads);
   else if (reg < 0x0E)
      insl(channels[channel].ctrl  + reg - 0x0A, buffer, quads);
   else if (reg < 0x16)
      insl(channels[channel].master + reg - 0x0E, buffer, quads);
   //asm("popw %es;");
   if (reg > 0x07 && reg < 0x0C)
      hdd_ident_write(channel, 0x0C, channels[channel].noint);
}

unsigned char hdd_wait(unsigned char channel, unsigned int advanced_check)
{
    for(int i = 0; i < 4; i++)
        hdd_ident_read(channel, HDD_REG_ALTSTATUS);
    while (hdd_ident_read(channel, HDD_REG_STATUS) & HDD_CS_BSY);
    if (advanced_check)
    {
        unsigned char state = hdd_ident_read(channel, HDD_REG_STATUS);
        if (state & HDD_CS_ERR)
            return 2;
        if (state & HDD_CS_DF)
            return 1;
        if ((state & HDD_CS_DRQ) == 0)
            return 3;

    }

    return 0;

}

uint8_t ide_ata_access(uint8_t direction, uint8_t drive, uint32_t lba, uint8_t numsects, uint16_t selector, uint32_t edi)
{
    selector = selector;
    uint8_t lba_mode, cmd;
    uint8_t lba_io[6];
    uint32_t channel = drives[drive].channel;
    uint32_t slavebit = drives[drive].drive;
    uint32_t bus = channels[channel].addr;
    uint32_t words = 256;
    uint16_t cyl, i;
    uint8_t head, sect, err;

    hdd_ident_write(channel, 0x0C, channels[channel].noint = 0x02);

    if (lba >= 0x10000000)
    {
        lba_mode  = 2;
        lba_io[0] = (lba & 0x000000FF) >> 0;
        lba_io[1] = (lba & 0x0000FF00) >> 8;
        lba_io[2] = (lba & 0x00FF0000) >> 16;
        lba_io[3] = (lba & 0xFF000000) >> 24;
        lba_io[4] = 0;
        lba_io[5] = 0;
        head = 0;
    }
    else if (drives[drive].capabilities & 0x200)
    {
        lba_mode  = 1;
        lba_io[0] = (lba & 0x00000FF) >> 0;
        lba_io[1] = (lba & 0x000FF00) >> 8;
        lba_io[2] = (lba & 0x0FF0000) >> 16;
        lba_io[3] = 0;
        lba_io[4] = 0;
        lba_io[5] = 0;
        head = (lba & 0xF000000) >> 24;
    }
    else
    {
        lba_mode  = 0;
        sect = (lba % 63) + 1;
        cyl = (lba + 1  - sect) / (16 * 63);
        lba_io[0] = sect;
        lba_io[1] = (cyl >> 0) & 0xFF;
        lba_io[2] = (cyl >> 8) & 0xFF;
        lba_io[3] = 0;
        lba_io[4] = 0;
        lba_io[5] = 0;
        head = (lba + 1  - sect) % (16 * 63) / (63);
    }

    //printf("head %d\n", head);

    if (lba_mode == 0)
        hdd_ident_write(channel, HDD_REG_HDDEVSEL, 0xA0 | (slavebit << 4) | head);
    else
        hdd_ident_write(channel, HDD_REG_HDDEVSEL, 0xE0 | (slavebit << 4) | head);

    if (lba_mode == 2) {
        hdd_ident_write(channel, HDD_REG_SECCOUNT1,   0);
        hdd_ident_write(channel, HDD_REG_LBA3,   lba_io[3]);
        hdd_ident_write(channel, HDD_REG_LBA4,   lba_io[4]);
        hdd_ident_write(channel, HDD_REG_LBA5,   lba_io[5]);
    }
    hdd_ident_write(channel, HDD_REG_SECCOUNT0,   numsects);
    hdd_ident_write(channel, HDD_REG_LBA0,   lba_io[0]);
    hdd_ident_write(channel, HDD_REG_LBA1,   lba_io[1]);
    hdd_ident_write(channel, HDD_REG_LBA2,   lba_io[2]);

    if (lba_mode == 0 && direction == 0) cmd = HDD_CMD_READ_PIO;
    if (lba_mode == 1 && direction == 0) cmd = HDD_CMD_READ_PIO;
    if (lba_mode == 2 && direction == 0) cmd = HDD_CMD_READ_PIO_EXT;
    if (lba_mode == 0 && direction == 1) cmd = HDD_CMD_WRITE_PIO;
    if (lba_mode == 1 && direction == 1) cmd = HDD_CMD_WRITE_PIO;
    if (lba_mode == 2 && direction == 1) cmd = HDD_CMD_WRITE_PIO_EXT;
    hdd_ident_write(channel, HDD_REG_COMMAND, cmd);

    if (direction == 0)
    {
        for (i = 0; i < numsects; i++)
        {
            if ((err = hdd_wait(channel, 1)))
                return err; // Polling, set error and exit if there is.
            //asm("pushw %es");
            //asm("mov %%ax, %%es" : : "a"(selector));
            asm("rep insw" : : "c"(words), "d"(bus), "D"(edi)); // Receive Data.
            //asm("popw %es");
            edi += (words*2);
        }
    }
    else
    {
        for (i = 0; i < numsects; i++)
        {
            hdd_wait(channel, 0); // Polling.
            //asm("pushw %ds");
            //asm("mov %%ax, %%ds"::"a"(selector));
            asm("rep outsw"::"c"(words), "d"(bus), "S"(edi)); // Send Data
            //asm("popw %ds");
            edi += (words*2);
            hdd_wait(channel, 1); // Polling.
        }
        hdd_ident_write(channel, HDD_REG_COMMAND, (char []) {   HDD_CMD_CACHE_FLUSH,
        HDD_CMD_CACHE_FLUSH,
        HDD_CMD_CACHE_FLUSH_EXT}[lba_mode]);
        hdd_wait(channel, 1); // Polling.
    }

    return 0; // Easy, isn't it?
}

//hdd_ident_read();

static uint32_t hdd_index;
static uint32_t hdd_size;
static block_table_type* block_table;

void hdd_init()
{
    paging_map_new_page_table(302);
    for(uint32_t i = 0; i < 4; i++)
    {
        paging_map_new_to_virtual(HDD_CACHE_TABLE + 0x1000*i);
    }

    block_table = (block_table_type*)HDD_CACHE_TABLE;

    for(uint32_t i = 0; i < 31; i++)
    {
        block_table->blocks[i].cache = 0xDEADBEEF;
        block_table->blocks[i].dirty = 0xDEADBEEF;
    }

    int count = 0;

    channels[0].addr  = 0x1F0;
    channels[0].ctrl  = 0x3F0;
    channels[1].addr  = 0x170;
    channels[1].ctrl  = 0x370;
    channels[0].master = 0;
    channels[1].master = 8;

    hdd_ident_write(0, 0x0C, 2);
    hdd_ident_write(1, 0x0C, 2);

    for (uint32_t i = 0; i < 2; i++)
    {
        for (uint32_t j = 0; j < 2; j++)
        {
            uint8_t err = 0, type = IDE_ATA, status;
            drives[count].reserved = 0; // Assuming that no drive here.

            // (I) Select Drive:
            hdd_ident_write(i, HDD_REG_HDDEVSEL, 0xA0 | (j << 4)); // Select Drive.
            for(uint32_t sleep; sleep < 1000000; sleep++); // Wait 1ms for drive select to work.

            // (II) Send ATA Identify Command:
            hdd_ident_write(i, HDD_REG_COMMAND, HDD_CMD_IDENTIFY);
            for(uint32_t sleep; sleep < 1000000; sleep++); // This function should be implemented in your OS. which waits for 1 ms.
            // it is based on System Timer Device Driver.

            // (III) Polling:
            if (hdd_ident_read(i, HDD_REG_STATUS) == 0) continue; // If Status = 0, No Device.

            while(1)
            {
                status = hdd_ident_read(i, HDD_REG_STATUS);
                if ((status & HDD_CS_ERR)) {err = 1; break;} // If Err, Device is not ATA.
                if (!(status & HDD_CS_BSY) && (status & HDD_CS_DRQ)) break; // Everything is right.
            }

            // (IV) Probe for ATAPI Devices:

            if (err != 0)
            {
                uint8_t cl = hdd_ident_read(i, HDD_REG_LBA1);
                uint8_t ch = hdd_ident_read(i, HDD_REG_LBA2);

                if (cl == 0x14 && ch ==0xEB)
                    type = IDE_ATAPI;
                else if (cl == 0x69 && ch == 0x96)
                    type = IDE_ATAPI;
                else
                    continue; // Unknown Type (may not be a device).

                hdd_ident_write(i, HDD_REG_COMMAND, HDD_CMD_IDENTIFY_PACKET);
                for(uint32_t sleep; sleep < 1000000; sleep++);
            }

            // (V) Read Identification Space of the Device:
            hdd_ident_read_buffer(i, HDD_REG_DATA, (uint32_t) ide_buffer, 128);

            // (VI) Read Device Parameters:
            drives[count].reserved     = 1;
            drives[count].type         = type;
            drives[count].channel      = i;
            drives[count].drive        = j;
            drives[count].signature    = *((uint16_t *)(ide_buffer + HDD_IDENT_DEVICETYPE));
            drives[count].capabilities = *((uint16_t *)(ide_buffer + HDD_IDENT_CAPABILITIES));
            drives[count].commandSets  = *((uint32_t *)(ide_buffer + HDD_IDENT_COMMANDSETS));

            // (VII) Get Size:
            if (drives[count].commandSets & (1 << 26))
                drives[count].size   = *((uint32_t *)(ide_buffer + HDD_IDENT_MAX_LBA_EXT));
            else
                drives[count].size   = *((uint32_t *)(ide_buffer + HDD_IDENT_MAX_LBA));

            for(uint32_t k = 0; k < 40; k += 2)
            {
                drives[count].model[k] = ide_buffer[HDD_IDENT_MODEL + k + 1];
                drives[count].model[k + 1] = ide_buffer[HDD_IDENT_MODEL + k];
            }
            drives[count].model[40] = 0;

            count++;
        }
    }

    /*for (uint32_t i = 0; i < 4; i++)
    {
        if (drives[i].reserved == 1)
        {
            printf("%d Found %s Drive %dMB - %s\n",
            i ,
            (const char *[]){"ATA", "ATAPI"}[drives[i].type],
            drives[i].size / 1024 / 2,
            drives[i].model);
        }
    }*/

    //const char * data = "qwerty";

    //printf("DRIVESIZE: %h %d\n", drives[1].size* 512, drives[1].size* 512);
    hdd_size = drives[1].size * 512;
    hdd_index = 0;
}

static uint8_t* find_cached_block(uint32_t dirty)
{
    for (uint32_t i = 0; i < 31; i++)
    {
        if (((hdd_index + 0x8000000) >> 9) == block_table->blocks[i].cache)
        {
            if (block_table->blocks[i].dirty == 0)
                block_table->blocks[i].dirty = dirty;
            return block_table->block[i].data;
        }
    }
    return (uint8_t*)0;
}

void purge_cache()
{
    for(uint32_t i = 0; i < 31; i++)
    {
        block_table->blocks[i].cache = 0xDEADBEEF;
        block_table->blocks[i].dirty = 0xDEADBEEF;
    }
}

void hdd_write_cache()
{
    for(uint32_t i = 0; i < 31; i++)
    {
        if (block_table->blocks[i].dirty == 1)
        {
            ide_ata_access(1, 0, block_table->blocks[i].cache, 1, 0, (uint32_t)block_table->block[i].data);
            block_table->blocks[i].dirty = 0;
        }
    }
}

static uint8_t* find_free_block(uint32_t dirty)
{
    for (uint32_t i = 0; i < 31; i++)
    {
        if (0xDEADBEEF == block_table->blocks[i].cache)
        {
            block_table->blocks[i].cache = ((hdd_index + 0x8000000) >> 9);
            block_table->blocks[i].dirty = dirty;
            return block_table->block[i].data;
        }
    }

    hdd_write_cache();

    purge_cache();

    return find_free_block(dirty);
}

static uint8_t* request_write()
{
    uint8_t* buffer;

    if ((buffer = find_cached_block(1)))
    {
        return buffer;
    }

    buffer = find_free_block(1);

    ide_ata_access(0, 0, (hdd_index + 0x8000000) >> 9, 1, 0, (uint32_t)buffer);

    return buffer;
}

void hdd_write(uint8_t data)
{
    request_write()[(hdd_index++ & 0x1FF)] = data;
}

static uint8_t* request_read()
{
    uint8_t* buffer;

    if ((buffer = find_cached_block(0)))
    {
        return buffer;
    }

    buffer = find_free_block(0);

    ide_ata_access(0, 0, (hdd_index + 0x8000000) >> 9, 1, 0, (uint32_t)buffer);

    return buffer;
}

uint8_t hdd_read()
{
    return request_read()[(hdd_index++ & 0x1FF)];
}

uint32_t hdd_remaining()
{
    return hdd_size - hdd_index;
}

void hdd_reset()
{
    hdd_index = 0;
}

void hdd_seek(uint32_t index)
{
    hdd_index = index;
}

uint32_t hdd_current()
{
    return hdd_index;
}

uint32_t hdd_capacity()
{
    return hdd_size;
}