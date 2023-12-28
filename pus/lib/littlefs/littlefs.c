#include <app/tests.h>
#include <lk/err.h>
#include <app.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lib/fs.h>
 
#include <lk/trace.h>
#define LOCAL_TRACE 1

#include <lib/bio.h>
#include <lib/littlefs.h>

#define SECTOR_SIZE 4096
#define BLOCK_SZ 2097152

static uint32_t baseaddr;

const bdev_t *lfs_dev;

const char *lfs_dev_name="virtio0";

const struct lfs_config lfs_cfg = {
    // block device operations
    .read  = &lfs_bio_read,
    .prog  = &lfs_bio_prog,
    .erase = &lfs_bio_erase,
    .sync  = &lfs_bio_sync,

    // block device configuration
    //.read_size = 128,
    //.prog_size = 128,
    //.cache_size = 512,
    
    // yavaş open-close için
     .read_size = 32,
    .prog_size = 32,
    .cache_size = 4096,
    
    
    .block_size = 4096, //SECTOR_SIZE,
    .block_count = 512,
    //.lookahead_size = 64,
    .lookahead_size = 4096,
    .block_cycles = 1024,
    .name_max = 32,
};

void erase_64K_block(void *addr)
{
	bio_erase(lfs_dev, addr, SECTOR_SIZE);
}

void erase_4K_block(void *addr)
{
	bio_erase(lfs_dev, addr, SECTOR_SIZE);
}

int lfs_bio_read(const struct lfs_config *c, lfs_block_t block,
	lfs_off_t offset, void *buffer, lfs_size_t size)
{
	baseaddr = 0;
	//printf("   prog rd: block=%d, offset=%d, size=%d\n", block, offset, size);
	const uint8_t *p = (uint8_t *)(baseaddr + block * SECTOR_SIZE + offset);
	//bio_read(lfs_dev, buffer, offset, size);
	bio_read(lfs_dev, buffer, p, size);
	return 0;
}

int lfs_bio_prog(const struct lfs_config *c, lfs_block_t block,
	lfs_off_t offset, const void *buffer, lfs_size_t size)
{
	//Serial.printf("   prog wr: block=%d, offset=%d, size=%d\n", block, offset, size);
	uint8_t *p = (uint8_t *)(baseaddr + block * SECTOR_SIZE + offset);
	bio_write(lfs_dev, buffer, p, size);
	return 0;
}

int lfs_bio_erase(const struct lfs_config *c, lfs_block_t block)
{
	uint8_t *p = (uint8_t *)(baseaddr + block * SECTOR_SIZE);
#if SECTOR_SIZE == 4096
	erase_4K_block(p);
#elif SECTOR_SIZE == 32768
	erase_32K_block(p);
#elif SECTOR_SIZE == 65536
	erase_64K_block(p);
#else
#error "Program SECTOR_SIZE must be 4096, 32768, or 65536"
#endif
	return 0;
}

int lfs_bio_sync(const struct lfs_config *c) {
	return 0;
}

int lfs_file_available(lfs_t *lfs, lfs_file_t *file) {
		if (!file) return 0;
		lfs_soff_t pos = lfs_file_tell(lfs, file);
		if (pos < 0) return 0;
		lfs_soff_t size = lfs_file_size(lfs, file);
		if (size < 0) return 0;
		return size - pos;
}
