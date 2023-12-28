#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <app.h>
#include <platform.h>
#include <kernel/thread.h>
#include <lk/console_cmd.h>

#include <inttypes.h>



static int myapp(int argc, const console_cmd_args *argv) {
    for (int i=0;i<10;i++){
		printf("bu bir test uygulamasıdır.\n");
	}
   
    return 0;
}

STATIC_COMMAND_START
STATIC_COMMAND("myapp", "myapp tests", &myapp)
STATIC_COMMAND_END(myapp);

APP_START(myapp)
APP_END

