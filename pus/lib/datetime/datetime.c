#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <app.h>
#include <platform.h>
#include <kernel/thread.h>
#include <lk/console_cmd.h>

#include <inttypes.h>
#include <platform/rtc_time.h>
#include <lib/datetime.h>
/**
* @brief Calculate day of week
* @param[in] y Year
* @param[in] m Month of year (in range 1 to 12)
* @param[in] d Day of month (in range 1 to 31)
* @return Day of week (in range 1 to 7)
**/
uint8_t computeDayOfWeek(uint16_t y, uint8_t m, uint8_t d)
{
    uint8_t h;
    uint8_t j;
    uint8_t k;
  
    //January and February are counted as months 13 and 14 of the previous year
    if(m <= 2)
    {
       m += 12;
       y -= 1;
    }
  
    //J is the century
    j = y / 100;
    //K the year of the century
    k = y % 100;
  
    //Compute H using Zeller's congruence
    h = d + (26 * (m + 1) / 10) + k + (k / 4) + (5 * j) + (j / 4);
  
    //Return the day of the week
    return ((h + 5) % 7) + 1;
}

/**
* @brief Convert Unix timestamp to date
* @param[in] t Unix timestamp
* @param[out] date Pointer to a structure representing the date and time
**/
void convertUnixTimeToDate(uint64_t t, DateTime *date)
{
    uint32_t a;
    uint32_t b;
    uint32_t c;
    uint32_t d;
    uint32_t e;
    uint32_t f;
  
    //Negative Unix time values are not supported
    if(t < 1)
       t = 0;
  
    //Clear milliseconds
    date->milliseconds = 0;
  
    //Retrieve hours, minutes and seconds
    date->seconds = t % 60;
    t /= 60;
    date->minutes = t % 60;
    t /= 60;
    date->hours = t % 24;
    t /= 24;
  
    //Convert Unix time to date
    a = (uint32_t) ((4 * t + 102032) / 146097 + 15);
    b = (uint32_t) (t + 2442113 + a - (a / 4));
    c = (20 * b - 2442) / 7305;
    d = b - 365 * c - (c / 4);
    e = d * 1000 / 30601;
    f = d - e * 30 - e * 601 / 1000;
  
    //January and February are counted as months 13 and 14 of the previous year
    if(e <= 13)
    {
       c -= 4716;
       e -= 1;
    }
    else
    {
       c -= 4715;
       e -= 13;
    }
  
    //Retrieve year, month and day
    date->year = c;
    date->month = e;
    date->day = f;
  
    //Calculate day of week
    date->dayOfWeek = computeDayOfWeek(c, e, f);
}

/**
* @brief Get current date and time
* @param[out] date Pointer to a structure representing the date and time
**/
  
void getCurrentDate(DateTime *date)
{
    //Retrieve current time stamp in epoch format from goldfish rtc
    uint64_t time = rtc_current_time();
  
    //Convert Unix timestamp to date
    convertUnixTimeToDate(time, date);
}
