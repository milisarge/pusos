/*
 * Copyright (c) 2023 milisarge
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <app.h>
#include <platform.h>
#include <kernel/thread.h>
#include <lk/console_cmd.h>

#include <lib/bio.h>
#include <lk/trace.h>


#include <lib/littlefs.h>

#define LOCAL_TRACE 1


static int datastore(int argc, const console_cmd_args *argv) {
    lfs_t lfs;
	lfs_file_t file;
    // block device açma
    //LTRACEF("opening the block device\n");
    lfs_dev = bio_open(lfs_dev_name);
    if (!lfs_dev) { return -1; }
    int err = lfs_mount(&lfs, &lfs_cfg);
    if (err) {
        lfs_format(&lfs, &lfs_cfg);
        lfs_mount(&lfs, &lfs_cfg);
    }
    
    // read current count
    float temp_value = 0.0;
    char x;
    lfs_file_open(&lfs, &file, "sensor_data", LFS_O_RDWR | LFS_O_CREAT);
    //printf(".%d\n",argv[1].i);
    for (int i=0;; i++) {
		if (argv[1].i == i+1) {
		    //printf("%d-%d\n",argv[1].i , i);
		    lfs_file_seek(&lfs, &file, 5*i, LFS_SEEK_CUR);
		    lfs_file_read(&lfs, &file, &temp_value, sizeof(temp_value));
		    printf("- %f\n",temp_value);
		    break;
		} else if (argv[1].i == 0) {
			lfs_file_read(&lfs, &file, &temp_value, sizeof(temp_value));
			printf("%d- %f\n", i+1, temp_value);
			lfs_file_seek(&lfs, &file, 1, LFS_SEEK_CUR);
			if (! lfs_file_available(&lfs, &file) || temp_value == 0.0) {
				break;
			}
		}
	}
    lfs_file_close(&lfs, &file);
    return 0;
}

STATIC_COMMAND_START
STATIC_COMMAND("datastore", "datastore tests", &datastore)
STATIC_COMMAND_END(datastore);

APP_START(datastore)
APP_END

