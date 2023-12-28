/*
 * Copyright (c) 2023 milisarge
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <app.h>
#include <platform.h>
#include <kernel/thread.h>
#include <lk/console_cmd.h>
#include <lk/err.h>

#include <lib/minip.h>
#include <lk/trace.h>

// coap lib
#include <lib/coap.h>
// bio device ops
#include <lib/bio.h>
//generate msg ids
#include <rand.h>
#include <platform/rtc_time.h>
#include <lib/datetime.h>

#include <kernel/mutex.h>
#include <kernel/semaphore.h>
#include <assert.h>
#include "../mu/mu.h"
#include "../mu/tbl.h"
#include "../mu/buf.h"
#include "../mu/num.h"
#include "../mu/fn.h"
#include "../mu/str.h"

#include "app600.h"
static int app604(int argc, const console_cmd_args *argv) {
	//if (mu_isnil(scope_600)){
	//	scope_600= mu_tbl_extend(0, MU_BUILTINS);
	//}
	mu_t scope_x= mu_tbl_extend(0, MU_BUILTINS);
	//mutex_init(&mu_lock_600);
	//mutex_acquire(&mu_lock_600);
    char *cmd = "let count=1;count=count+1;print(count);";
    mu_eval(cmd, strlen(cmd), scope_x, 0);
	//mutex_release(&mu_lock_600);
	/*
	printf("pointer scope %p\n",scope_x);
	void * try = scope_x;
	for (int i=0;i<120;i++){
	  printf("%02x ", ((uint8_t*) try)[i]);
	}
	*/
	//////
	//char *cmd2 = "cfn=fn()->print('deneme');print(cfn)";
	//char *cmd2 = "cfn=fn()->print('deneme');tbl=[as:4];print(tbl.as)";
	char *cmd2 = "cfn=fn()->999;tbl=[[id:3,data:34.5],[id:5,data:55.66],[id:6,data:43.2]]";
	
	mu_eval(cmd2, strlen(cmd2), scope_x, 0);
	//mutex_release(&mu_lock_600);
	
	/*
	mu_t res = mu_eval(cmd2, strlen(cmd2), scope_x, 0xf);
    mu_t repr = mu_fn_call(MU_REPR, 0x21, res, mu_num_fromuint(2));
    
    char pr[mu_str_getlen(repr)-2];
    memcpy(pr, mu_str_getdata(repr)+1, mu_str_getlen(repr)-2);
	printf("%s\n",pr);
    mu_dec(repr);
    */
    
    mu_t fnc = mu_tbl_lookup(scope_x, mu_str_format("tbl"));
    printf("-------\n");
    if (!fnc || !mu_istbl(fnc)) {
        printf("fnc no tbl\n");
        return 0;
    }
    mu_t fncx;
    mu_t fncy;
    int id;
    float data;
    for (int i = 0; i < 4; i++) {
      fncx = mu_tbl_lookup(fnc, mu_num_fromuint(i));
      id = mu_num_getint(mu_tbl_lookup(fncx, mu_str_format("id")));
      data = mu_num_getfloat(mu_tbl_lookup(fncx, mu_str_format("data")));
      //mu_t code = mu_fn_call(fncx, 0x21, 0);
      printf("%d:%f\n", id, data);
	}
    
    // veri yapısı içeren bir tablodan veri çekmek için
    /*
    mu_t fnc = mu_tbl_lookup(scope_x, mu_str_format("tbl"));
    printf("-------\n");
    if (!fnc || !mu_istbl(fnc)) {
        printf("fnc no tbl\n");
        return 0;
    }
    mu_t fncx;
    mu_t fncy;
    int id;
    float data;
    for (int i = 0; i < 3; i++) {
      fncx = mu_tbl_lookup(fnc, mu_num_fromuint(i));
      id = mu_num_getint(mu_tbl_lookup(fncx, mu_str_format("id")));
      data = mu_num_getfloat(mu_tbl_lookup(fncx, mu_str_format("data")));
      //mu_t code = mu_fn_call(fncx, 0x21, 0);
      printf("%d:%f\n", id, data);
	}
    */
    
    // bir tablo verisine erişmek için
    /*
    char *cmd2 = "cfn=fn()->999;tbl=[as:4,555]";
    mu_t fnc = mu_tbl_lookup(scope_x, mu_str_format("tbl"));
    printf("-------\n");
    if (!fnc || !mu_istbl(fnc)) {
        printf("fnc no tbl\n");
        return 0;
    }
    //mu_t fncx = mu_tbl_lookup(fnc, mu_str_format("as"));
    mu_t fncx = mu_tbl_lookup(fnc, mu_num_fromuint(0));
    //mu_t code = mu_fn_call(fncx, 0x21, 0);
    printf("resp:%d\n",mu_num_getint(fncx));
    */
    
    // bir fonksiyon çağırarak veriyi yazdırma
    /*
    char *cmd2 = "cfn=fn()->999;tbl=[as:4,555]";
    mu_t fnc = mu_tbl_lookup(scope_x, mu_str_format("cfn"));
    printf("-------\n");
    if (!fnc || !mu_isfn(fnc)) {
        printf("fnc no fonks\n");
        return 0;
    }
    mu_t code = mu_fn_call(fnc, 0x21, 0);
    printf("resp:%d\n",mu_num_getint(code));
    */
    //////
    
    
    //char pr[mu_str_getlen(repr)-2];
    //memcpy(pr, mu_str_getdata(repr)+1, mu_str_getlen(repr)-2);
	//printf("%s\n",pr);
    
    //mu_print(mu_buf_getdata(b), n);
    //mu_buf_dec(b);
    //mu_tbl_dec(frame[0]);
	
	
	/*
	mu_t scope_y = mu_tbl_extend(0, MU_BUILTINS);
	mu_eval(mu_tbl_dump(scope_x,0), strlen(mu_tbl_dump(scope_x,0)), scope_y, 0);
	char *cmd3 = "print(cfn())";
	mu_eval(cmd3, strlen(cmd3), scope_y, 0);
	*/
	//mutex_init(&mu_lock_600);
	
	
	/*
	void * try2 = mu_tbl_dump(scope_x,0);
	for (int i=0;i<240;i++){
	  printf("%02x ", ((uint8_t*) try2)[i]);
	}
	*/

    return 0;
}

STATIC_COMMAND_START
STATIC_COMMAND("app604", "app604 tests", &app604)
STATIC_COMMAND_END(app604);

APP_START(app604)
APP_END

