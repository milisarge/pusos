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
// variables used by the filesystem

//mu  global scope
#include <lib/app600.h>

static int lfstest(int argc, const console_cmd_args *argv) {
    //void *buffer = malloc(lfs_cfg.read_size);
    lfs_t lfs;
	lfs_file_t file;
    lfs_dev = bio_open(lfs_dev_name);
    if (!lfs_dev) {
        return -1;
    }
    
  // mount the filesystem
    int err = lfs_mount(&lfs, &lfs_cfg);
    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (err) {
        lfs_format(&lfs, &lfs_cfg);
        lfs_mount(&lfs, &lfs_cfg);
    }
   
    int retdir = lfs_mkdir(&lfs, "/testdir");
    if (retdir == 0){
	  printf("testdir directory is opened\n");
	}
    // read current count
    uint32_t boot_count = 0;
    lfs_file_open(&lfs, &file, "/testdir/boot_count", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_read(&lfs, &file, &boot_count, sizeof(boot_count));

    // update boot count
    boot_count += 1;
    lfs_file_rewind(&lfs, &file);
    lfs_file_write(&lfs, &file, &boot_count, sizeof(boot_count));

    // remember the storage is not updated until the file is closed successfully
    lfs_file_close(&lfs, &file);
    
    
    // string
    char str[10];
    lfs_file_open(&lfs, &file, "boot_string", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_read(&lfs, &file, &str, sizeof(str));

    // update boot string
    memcpy(str,"deneme\n",8);
    //lfs_file_rewind(&lfs, &file);
    lfs_file_write(&lfs, &file, &str, sizeof(str));
	//////printf("boot_string: %s\n", str);
	printf("boot_string: %s\n", str);
    // remember the storage is not updated until the file is closed successfully
    lfs_file_close(&lfs, &file);
    
    
    
    //retdir = lfs_mkdir(&lfs, "/mu");
    /*if (retdir == 0){
	  printf("mu directory is opened\n");
	}*/
    /*
    char script[100];
    lfs_file_open(&lfs, &file, argv[1].str, LFS_O_RDWR | LFS_O_CREAT);

    lfs_file_read(&lfs, &file, &script, sizeof(script));
    // get the last read point
    lfs_soff_t read_end = lfs_file_tell(&lfs, &file);
    script[read_end] = '\0';
    lfs_file_close(&lfs, &file);
    
    
    printf("mu_code:%s\n",script);
    
    // mu code run
    if (mu_lock_600.magic != MUTEX_MAGIC) {
      //printf("mutex-mu_lock_600 init\n");
      mutex_init(&mu_lock_600);
    }
	if (mu_isnil(scope_600)){
	  //printf("scope_600 init\n");
	  scope_600= mu_tbl_extend(0, MU_BUILTINS);
	}
	mu_t ret;
    mutex_acquire(&mu_lock_600);
    if (!setjmp(mu_error_jmp)) {
	  //printf("mu_code2:%s:\n",script);
	  //ret = mu_eval(script, read_end-1, scope_600, 0);
	  ret = mu_eval(script, read_end-1, scope_600, 0);
	}
	//////printf("mu_ret:%d\n",ret);
	mutex_release(&mu_lock_600);
	*/
	
	
	
    // release any resources we were using
    lfs_unmount(&lfs);

    // print the boot count
    //////printf("boot_count: %d\n", boot_count);
    printf("boot_count: %d\n", boot_count);
    
    return 0;
}

STATIC_COMMAND_START
STATIC_COMMAND("lfstest", "lfstest tests", &lfstest)
STATIC_COMMAND_END(lfstest);

APP_START(lfstest)
APP_END

