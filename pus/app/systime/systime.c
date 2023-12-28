#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <app.h>
#include <platform.h>
#include <kernel/thread.h>
#include <lk/console_cmd.h>
#include <platform/timer.h>
#include <inttypes.h>
#include <platform/rtc_time.h>
#include <lib/datetime.h>

static int systime(int argc, const console_cmd_args *argv) {
    // epoch format
    uint64_t time_rtc = rtc_current_time();
    printf("epoch_time:%lld\n",time_rtc);
    
    // gettimeofday practise
    //_gettimeofday(time_rtc);
    
    // formatted datetime
    DateTime cur_date;
    getCurrentDate(&cur_date);
    printf("%d-%d-%d %d:%d:%d UTC\n", 
    cur_date.year, cur_date.month, cur_date.day,
    cur_date.hours, cur_date.minutes, cur_date.seconds);  
    
    return 0;
}

STATIC_COMMAND_START
STATIC_COMMAND("systime", "systime tests", &systime)
STATIC_COMMAND_END(systime);

APP_START(systime)
APP_END
