#include <app/tests.h>
#include <lk/err.h>
#include <app.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lib/fs.h>
 
#include <lk/trace.h>
#define LOCAL_TRACE 1


static int myfs(int argc, const console_cmd_args *argv) {
    
    int ret = 0;
	int status;
	filehandle *handle;
	
    fs_mount("/test", "memfs", NULL);
    const char *filename = "/test/deneme.txt";
    status = fs_create_file(filename, &handle, 24);
	LTRACEF("fs_open_create_status:%d\n",status);
    
    status = fs_open_file(filename, &handle);
	LTRACEF("fs_open_file_status:%d\n",status);
    
    char buf[1024];
    const char *content = "Hello World";
    const size_t content_len = strlen(content);
    
    ssize_t bytes = fs_write_file(handle, content, 0, content_len);
    if (bytes != content_len) {
        LTRACEF("fs_open_write_status:error\n");
    } else {
        LTRACEF("fs_open_write_status:success\n");
	}
    fs_close_file(handle);
    printf("file operations----!\n");

    return ret;
}

STATIC_COMMAND_START
STATIC_COMMAND("myfs", "myfs tests", &myfs)
STATIC_COMMAND_END(myfs);

APP_START(myfs)
APP_END

