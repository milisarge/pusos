/*
 * Copyright (c) 2023 milisarge
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <app.h>
#include <platform.h>
#include <kernel/thread.h>
#include <lk/console_cmd.h>

#include <inttypes.h>
#include <lib/binn.h>


static int mybinn(int argc, const console_cmd_args *argv) {
    binn *obj;

	// create a new object
	obj = binn_object();

	// add values to it
	binn_object_set_int32(obj, "id", 1234);
	binn_object_set_str(obj, "name", "Room_10 Sensor Device");
	binn_object_set_double(obj, "last_value", 28.90);

	// release the binn structure but keeps the binn buffer allocated
	// returns the pointer to the buffer
	// must be released later with free() or the registered alloc/free function
	int id;
	char *name;
	double price;


	id = binn_object_int32(obj, "id");
	name = binn_object_str(obj, "name");
	price = binn_object_double(obj, "last_value");
	printf("id:%d name:%s last_value:%f \n",id , name, price);
	binn_free(obj);
    printf("atof_test:%f\n",atof("299.45454"));
    return 0;
}

STATIC_COMMAND_START
STATIC_COMMAND("mybinn", "mybinn tests", &mybinn)
STATIC_COMMAND_END(mybinn);

APP_START(mybinn)
APP_END

