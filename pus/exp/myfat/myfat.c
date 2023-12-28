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

#include <lib/ff.h>
#include <lib/diskio.h>


#define LOCAL_TRACE 1

static int myfat(int argc, const console_cmd_args *argv) {
    LTRACEF("opening the block device\n");
    const TCHAR * dev_name="virtio0";
    bdev_t *dev = bio_open(dev_name);
    if (!dev) {
        return -1;
    }
	
	// boundry, size
    void *ptr = memalign(0,30);
    FATFS fs;
    //fs.disk=dev;
    printf("mount:%d\n",f_mount(&fs, dev_name, 1));
    bio_close(dev);
    LTRACEF("block device closed\n");
    return 0;
}

STATIC_COMMAND_START
STATIC_COMMAND("myfat", "myfat tests", &myfat)
STATIC_COMMAND_END(myfat);

APP_START(myfat)
APP_END

