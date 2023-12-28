#pragma once

#include <lk/compiler.h>
#include <sys/types.h>

__BEGIN_CDECLS
typedef enum handler_return (*platform_timer_callback)(void *arg, lk_time_t now);
lk_time_t rtc_current_time(void);
lk_bigtime_t rtc_current_time_hires(void);
status_t rtc_platform_set_oneshot_timer (platform_timer_callback callback, void *arg, lk_time_t interval);

__END_CDECLS
