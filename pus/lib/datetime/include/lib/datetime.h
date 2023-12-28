#pragma once

#ifdef __cplusplus
extern "C" {
#endif



/**
* @brief Date and time representation
**/
typedef struct
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t dayOfWeek;
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
    uint16_t milliseconds;
} DateTime;

/**
* @brief Calculate day of week
* @param[in] y Year
* @param[in] m Month of year (in range 1 to 12)
* @param[in] d Day of month (in range 1 to 31)
* @return Day of week (in range 1 to 7)
**/
uint8_t computeDayOfWeek(uint16_t y, uint8_t m, uint8_t d);

/**
* @brief Convert Unix timestamp to date
* @param[in] t Unix timestamp
* @param[out] date Pointer to a structure representing the date and time
**/
void convertUnixTimeToDate(uint64_t t, DateTime *date);

/**
* @brief Get current date and time
* @param[out] date Pointer to a structure representing the date and time
**/
void getCurrentDate(DateTime *date);
