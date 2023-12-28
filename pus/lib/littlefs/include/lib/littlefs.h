#ifndef LITTLEFS_H
#define LITTLEFS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#include <lib/lfs.h>

#define SECTOR_SIZE 65536
#define BLOCK_SZ 2097152

extern const bdev_t *lfs_dev;

void erase_64K_block(void *addr);

int lfs_bio_read(const struct lfs_config *c, lfs_block_t block,
	lfs_off_t offset, void *buffer, lfs_size_t size);

int lfs_bio_prog(const struct lfs_config *c, lfs_block_t block,
	lfs_off_t offset, const void *buffer, lfs_size_t size);

int lfs_bio_erase(const struct lfs_config *c, lfs_block_t block);

int lfs_bio_sync(const struct lfs_config *c);

int lfs_file_available(lfs_t *lfs, lfs_file_t *file);

extern const struct lfs_config lfs_cfg;
extern const char * lfs_dev_name;

#endif
