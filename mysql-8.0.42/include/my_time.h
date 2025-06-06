/* Copyright (c) 2004, 2025, Oracle and/or its affiliates.

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License, version 2.0,
 as published by the Free Software Foundation.

 This program is designed to work with certain software (including
 but not limited to OpenSSL) that is licensed under separate terms,
 as designated in a particular file or component or in included license
 documentation.  The authors of MySQL hereby grant you an additional
 permission to link the program and your derivative works with the
 separately licensed software that they have either included with
 the program or referenced in the documentation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License, version 2.0, for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA */

#ifndef MY_TIME_INCLUDED
#define MY_TIME_INCLUDED

/**
  @ingroup MY_TIME
  @{

  @file include/my_time.h

  Interface for low level time utilities.
*/

#include "my_config.h"

#include <assert.h>  // assert
#include <algorithm>
#include <cstddef>  // std::size_t
#include <cstdint>  // std::int32_t
#include <cstring>  // strncpy
#include <limits>   // std::numeric_limits

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>  // struct timeval
#endif                 /* HAVE_SYS_TIME_H */
#ifdef _WIN32
#include <winsock2.h>  // struct timeval
#endif                 /* _WIN32 */

#include "field_types.h"
#include "my_time_t.h"
#include "mysql_time.h"  // struct MYSQL_TIME, shared with client code

extern const unsigned long long int log_10_int[20];
extern const unsigned char days_in_month[];
extern const char my_zero_datetime6[]; /* "0000-00-00 00:00:00.000000" */

constexpr const bool HAVE_64_BITS_TIME_T = sizeof(time_t) == sizeof(my_time_t);

/** Time handling defaults */
constexpr const int MYTIME_MAX_YEAR = HAVE_64_BITS_TIME_T ? 9999 : 2038;
constexpr const int TIMESTAMP_MAX_YEAR = 2038;

/** Two-digit years < this are 20XX; >= this are 19XX */
constexpr const int YY_PART_YEAR = 70;
constexpr const int MYTIME_MIN_YEAR = (1900 + YY_PART_YEAR - 1);

/** max seconds from epoch of host's time_t stored in my_time_t
    Windows allows up to 3001-01-18 23:59:59 UTC for localtime_r, so
    that is our effective limit, although Unixen allow higher time points.
    Hence the magic constant 32536771199.
 */
constexpr const my_time_t MYTIME_MAX_VALUE =
    HAVE_64_BITS_TIME_T ? 32536771199
                        : std::numeric_limits<std::int32_t>::max();

/**
  Zero represents the first time value we allow, i.e. UNIX epoch.
  We do not allow times before UNIX epoch, except for inside computations,
  hence we use a signed integer as the base type, cf. prepare_tz_info.
*/
constexpr const int MYTIME_MIN_VALUE = 0;

/** max seconds from epoch that can be stored in a column of type TIMESTAMP.
    This also impacts the max value that can be given to SET TIMESTAMP
*/
constexpr const std::int64_t TYPE_TIMESTAMP_MAX_VALUE =
    std::numeric_limits<std::int32_t>::max();
constexpr const std::int64_t TYPE_TIMESTAMP_MIN_VALUE = 1;

/** Flags to str_to_datetime and number_to_datetime */
using my_time_flags_t = unsigned int;

/** Allow zero day and zero month */
constexpr const my_time_flags_t TIME_FUZZY_DATE = 1;

/** Only allow full datetimes. */
constexpr const my_time_flags_t TIME_DATETIME_ONLY = 2;

constexpr const my_time_flags_t TIME_FRAC_TRUNCATE = 4;
constexpr const my_time_flags_t TIME_NO_DATE_FRAC_WARN = 8;

/** Don't allow zero day or zero month */
constexpr const my_time_flags_t TIME_NO_ZERO_IN_DATE = 16;

/** Don't allow 0000-00-00 date */
constexpr const my_time_flags_t TIME_NO_ZERO_DATE = 32;

/** Allow 2000-02-31 */
constexpr const my_time_flags_t TIME_INVALID_DATES = 64;

/** Allow only HH:MM:SS or MM:SS time formats */
constexpr const my_time_flags_t TIME_STRICT_COLON = 128;

/** Conversion warnings */
constexpr const int MYSQL_TIME_WARN_TRUNCATED = 1;
constexpr const int MYSQL_TIME_WARN_OUT_OF_RANGE = 2;
constexpr const int MYSQL_TIME_WARN_INVALID_TIMESTAMP = 4;
constexpr const int MYSQL_TIME_WARN_ZERO_DATE = 8;
constexpr const int MYSQL_TIME_NOTE_TRUNCATED = 16;
constexpr const int MYSQL_TIME_WARN_ZERO_IN_DATE = 32;
constexpr const int MYSQL_TIME_WARN_DATETIME_OVERFLOW = 64;

/** Useful constants */
constexpr const int64_t SECONDS_IN_24H = 86400LL;

/** Limits for the TIME data type */
constexpr const int TIME_MAX_HOUR = 838;
constexpr const int TIME_MAX_MINUTE = 59;
constexpr const int TIME_MAX_SECOND = 59;

/**
 Note that this MUST be a signed type, as we use the unary - operator on it.
 */
constexpr const int TIME_MAX_VALUE =
    TIME_MAX_HOUR * 10000 + TIME_MAX_MINUTE * 100 + TIME_MAX_SECOND;

constexpr const int TIME_MAX_VALUE_SECONDS =
    TIME_MAX_HOUR * 3600 + TIME_MAX_MINUTE * 60 + TIME_MAX_SECOND;

constexpr const int DATETIME_MAX_DECIMALS = 6;

constexpr const int SECS_PER_MIN = 60;
constexpr const int MINS_PER_HOUR = 60;
constexpr const int HOURS_PER_DAY = 24;
constexpr const int DAYS_PER_WEEK = 7;
constexpr const int DAYS_PER_NYEAR = 365;
constexpr const int DAYS_PER_LYEAR = 366;
constexpr const int SECS_PER_HOUR = (SECS_PER_MIN * MINS_PER_HOUR);
constexpr const int SECS_PER_DAY = (SECS_PER_HOUR * HOURS_PER_DAY);
constexpr const int MONS_PER_YEAR = 12;

constexpr const int MAX_TIME_ZONE_HOURS = 14;

/** Flags for calc_week() function.  */
constexpr const unsigned int WEEK_MONDAY_FIRST = 1;
constexpr const unsigned int WEEK_YEAR = 2;
constexpr const unsigned int WEEK_FIRST_WEEKDAY = 4;

/** Daynumber from year 0 to 9999-12-31 */
constexpr const int64_t MAX_DAY_NUMBER = 3652424;

/**
  Structure to return status from
    str_to_datetime(), str_to_time(), number_to_datetime(), number_to_time()
    @note Implicit default constructor initializes all members to 0.
*/
struct MYSQL_TIME_STATUS {
  int warnings{0};
  unsigned int fractional_digits{0};
  unsigned int nanoseconds{0};
  struct DEPRECATION {  // We only report first offense
    enum DEPR_KIND {
      DP_NONE,         // no deprecated delimiter seen yet
      DP_WRONG_KIND,   // seen a delimiter in correct position, but wrong one
      DP_WRONG_SPACE,  // seen a space delimiter which isn't 0x20 ' '.
      DP_SUPERFLUOUS   // seen a superfluous delimiter
    } m_kind{DP_NONE};
    char m_delim_seen;
    bool m_colon;    // for DP_WRONG_KIND: true if we expect ':', else '-'
    int m_position;  // 0-based in m_arg
    char m_arg[40];  // the string argument we found a deprecation in
  } m_deprecation;
  ///< Register wrong delimiter if it's the first we see for this value
  ///< @param kind  what kind of deprecation did we see
  ///< @param arg   the string we try to interpret as a datetime value
  ///< @param end   points to the character after arg, usually a '\0'
  ///< @param delim what delimiter was used
  ///< @param colon used if kind==DP_WRONG_KIND. true: expect ':' else expect'-'
  void set_deprecation(DEPRECATION::DEPR_KIND kind, const char *arg,
                       const char *end, const char *delim, bool colon = false) {
    if (m_deprecation.m_kind == DEPRECATION::DP_NONE) {
      m_deprecation.m_kind = kind;
      m_deprecation.m_delim_seen = *delim;
      m_deprecation.m_colon = colon;
      const size_t bufsize = sizeof(m_deprecation.m_arg) - 1;  // -1: for '\0'
      const size_t argsize = end - arg;
      const size_t size = std::min(bufsize, argsize);
      // The input string is not necessarily zero-terminated,
      // so do not use snprintf().
      std::strncpy(m_deprecation.m_arg, arg, size);
      m_deprecation.m_arg[size] = '\0';
      m_deprecation.m_position = delim - arg;
    }
  }
  MYSQL_TIME_STATUS() = default;
  MYSQL_TIME_STATUS(const MYSQL_TIME_STATUS &) = default;
  /// Assignment: don't clobber an existing deprecation, first one wins
  MYSQL_TIME_STATUS &operator=(const MYSQL_TIME_STATUS &b) {
    warnings = b.warnings;
    fractional_digits = b.fractional_digits;
    nanoseconds = b.nanoseconds;
    // keep first deprecation
    if (m_deprecation.m_kind == DEPRECATION::DP_NONE) {
      m_deprecation = b.m_deprecation;
    }
    return *this;
  }
  void squelch_deprecation() { m_deprecation.m_kind = DEPRECATION::DP_NONE; }
};

/**
   Struct representing a duration. Content is similar to MYSQL_TIME
   but member variables are unsigned.
 */
struct Interval {
  unsigned long int year;
  unsigned long int month;
  unsigned long int day;
  unsigned long int hour;
  unsigned long long int minute;
  unsigned long long int second;
  unsigned long long int second_part;
  bool neg;
};

void my_init_time();

long calc_daynr(unsigned int year, unsigned int month, unsigned int day);
unsigned int calc_days_in_year(unsigned int year);
unsigned int year_2000_handling(unsigned int year);

bool time_zone_displacement_to_seconds(const char *str, size_t length,
                                       int *result);

void get_date_from_daynr(int64_t daynr, unsigned int *year, unsigned int *month,
                         unsigned int *day);
int calc_weekday(long daynr, bool sunday_first_day_of_week);
bool valid_period(long long int period);
uint64_t convert_period_to_month(uint64_t period);
uint64_t convert_month_to_period(uint64_t month);

/**
  Check for valid my_time_t value. Note: timestamp here pertains to seconds
  since epoch, not the legacy MySQL type TIMESTAMP, which is limited to
  32 bits even on 64 bit platforms.
*/
inline bool is_time_t_valid_for_timestamp(time_t x) {
  return (static_cast<int64_t>(x) <= static_cast<int64_t>(MYTIME_MAX_VALUE) &&
          x >= MYTIME_MIN_VALUE);
}

unsigned int calc_week(const MYSQL_TIME &l_time, unsigned int week_behaviour,
                       unsigned int *year);

bool check_date(const MYSQL_TIME &ltime, bool not_zero_date,
                my_time_flags_t flags, int *was_cut);
bool str_to_datetime(const char *str, std::size_t length, MYSQL_TIME *l_time,
                     my_time_flags_t flags, MYSQL_TIME_STATUS *status);
long long int number_to_datetime(long long int nr, MYSQL_TIME *time_res,
                                 my_time_flags_t flags, int *was_cut);
bool number_to_time(long long int nr, MYSQL_TIME *ltime, int *warnings);
unsigned long long int TIME_to_ulonglong_datetime(const MYSQL_TIME &my_time);
unsigned long long int TIME_to_ulonglong_date(const MYSQL_TIME &my_time);
unsigned long long int TIME_to_ulonglong_time(const MYSQL_TIME &my_time);
unsigned long long int TIME_to_ulonglong(const MYSQL_TIME &my_time);

unsigned long long int TIME_to_ulonglong_datetime_round(
    const MYSQL_TIME &my_time, int *warnings);
unsigned long long int TIME_to_ulonglong_time_round(const MYSQL_TIME &my_time);

/**
   Round any MYSQL_TIME timepoint and convert to ulonglong.
   @param my_time input
   @param[out] warnings warning vector
   @return time point as ulonglong
 */
inline unsigned long long int TIME_to_ulonglong_round(const MYSQL_TIME &my_time,
                                                      int *warnings) {
  switch (my_time.time_type) {
    case MYSQL_TIMESTAMP_TIME:
      return TIME_to_ulonglong_time_round(my_time);
    case MYSQL_TIMESTAMP_DATETIME:
      return TIME_to_ulonglong_datetime_round(my_time, warnings);
    case MYSQL_TIMESTAMP_DATE:
      return TIME_to_ulonglong_date(my_time);
    default:
      assert(false);
      return 0;
  }
}

/**
   Extract the microsecond part of a MYSQL_TIME struct as an n *
   (1/10^6) fraction as a double.

   @return microseconds part as double
 */
inline double TIME_microseconds(const MYSQL_TIME &my_time) {
  return static_cast<double>(my_time.second_part) / 1000000.0;
}

/**
   Convert a MYSQL_TIME datetime to double where the integral part is
   the timepoint as an ulonglong, and the fractional part is the
   fraction of the second.

   @param my_time datetime to convert
   @return datetime as double
 */
inline double TIME_to_double_datetime(const MYSQL_TIME &my_time) {
  return static_cast<double>(TIME_to_ulonglong_datetime(my_time)) +
         TIME_microseconds(my_time);
}

/**
   Convert a MYSQL_TIME time to double where the integral part is
   the timepoint as an ulonglong, and the fractional part is the
   fraction of the second.

   @param my_time time to convert
   @return datetime as double
 */
inline double TIME_to_double_time(const MYSQL_TIME &my_time) {
  return static_cast<double>(TIME_to_ulonglong_time(my_time)) +
         TIME_microseconds(my_time);
}

/**
   Convert a MYSQL_TIME to double where the integral part is the
   timepoint as an ulonglong, and the fractional part is the fraction
   of the second. The actual time type is extracted from
   MYSQL_TIME::time_type.

   @param my_time MYSQL_TIME to convert
   @return MYSQL_TIME as double
 */
inline double TIME_to_double(const MYSQL_TIME &my_time) {
  return static_cast<double>(TIME_to_ulonglong(my_time)) +
         TIME_microseconds(my_time);
}

/**
   Return the fraction of the second as the number of microseconds.

   @param i timepoint as longlong
   @return frational part of an timepoint represented as an (u)longlong
*/
inline long long int my_packed_time_get_frac_part(long long int i) {
  return (i % (1LL << 24));
}

long long int year_to_longlong_datetime_packed(long year);
long long int TIME_to_longlong_datetime_packed(const MYSQL_TIME &my_time);
long long int TIME_to_longlong_date_packed(const MYSQL_TIME &my_time);
long long int TIME_to_longlong_time_packed(const MYSQL_TIME &my_time);
long long int TIME_to_longlong_packed(const MYSQL_TIME &my_time);

void TIME_from_longlong_datetime_packed(MYSQL_TIME *ltime, long long int nr);
void TIME_from_longlong_time_packed(MYSQL_TIME *ltime, long long int nr);
void TIME_from_longlong_date_packed(MYSQL_TIME *ltime, long long int nr);
void TIME_set_yymmdd(MYSQL_TIME *ltime, unsigned int yymmdd);
void TIME_set_hhmmss(MYSQL_TIME *ltime, unsigned int hhmmss);

void my_datetime_packed_to_binary(long long int nr, unsigned char *ptr,
                                  unsigned int dec);
long long int my_datetime_packed_from_binary(const unsigned char *ptr,
                                             unsigned int dec);

void my_time_packed_to_binary(long long int nr, unsigned char *ptr,
                              unsigned int dec);
long long int my_time_packed_from_binary(const unsigned char *ptr,
                                         unsigned int dec);

void my_timestamp_to_binary(const my_timeval *tm, unsigned char *ptr,
                            unsigned int dec);
void my_timestamp_from_binary(my_timeval *tm, const unsigned char *ptr,
                              unsigned int dec);

bool str_to_time(const char *str, std::size_t length, MYSQL_TIME *l_time,
                 MYSQL_TIME_STATUS *status, my_time_flags_t flags = 0);

bool check_time_mmssff_range(const MYSQL_TIME &my_time);
bool check_time_range_quick(const MYSQL_TIME &my_time);
bool check_datetime_range(const MYSQL_TIME &my_time);
void adjust_time_range(MYSQL_TIME *, int *warning);

/**
  Check whether the argument holds a valid UNIX time value
  (seconds after epoch). This function doesn't make precise check, but rather a
  rough estimate before time zone adjustments.

  @param my_time  timepoint to check
  @returns true if value satisfies the check above, false otherwise.
*/
inline bool validate_my_time(const MYSQL_TIME &my_time) {
  if (my_time.year < MYTIME_MIN_YEAR || my_time.year > MYTIME_MAX_YEAR)
    return false;

  return true;
}

my_time_t my_system_gmt_sec(const MYSQL_TIME &my_time, my_time_t *my_timezone,
                            bool *in_dst_time_gap);

void set_zero_time(MYSQL_TIME *tm, enum enum_mysql_timestamp_type time_type);
void set_max_time(MYSQL_TIME *tm, bool neg);
void set_max_hhmmss(MYSQL_TIME *tm);

/**
  Required buffer length for my_time_to_str, my_date_to_str,
  my_datetime_to_str and TIME_to_string functions. Note, that the
  caller is still responsible to check that given TIME structure
  has values in valid ranges, otherwise size of the buffer might
  well be insufficient. We also rely on the fact that even incorrect values
  sent using binary protocol fit in this buffer.
*/
constexpr const std::size_t MAX_DATE_STRING_REP_LENGTH =
    sizeof("YYYY-MM-DD AM HH:MM:SS.FFFFFF+HH:MM");

int my_time_to_str(const MYSQL_TIME &my_time, char *to, unsigned int dec);
int my_date_to_str(const MYSQL_TIME &my_time, char *to);
int my_datetime_to_str(const MYSQL_TIME &my_time, char *to, unsigned int dec);
int my_TIME_to_str(const MYSQL_TIME &my_time, char *to, unsigned int dec);

void my_date_to_binary(const MYSQL_TIME *ltime, unsigned char *ptr);
int my_timeval_to_str(const my_timeval *tm, char *to, unsigned int dec);

/**
  Available interval types used in any statement.

  'interval_type' must be sorted so that simple intervals comes first,
  ie year, quarter, month, week, day, hour, etc. The order based on
  interval size is also important and the intervals should be kept in a
  large to smaller order. (get_interval_value() depends on this)

  @note If you change the order of elements in this enum you should fix
  order of elements in 'interval_type_to_name' and 'interval_names'
  arrays

  @see interval_type_to_name, get_interval_value, interval_names
*/
enum interval_type {
  INTERVAL_YEAR,
  INTERVAL_QUARTER,
  INTERVAL_MONTH,
  INTERVAL_WEEK,
  INTERVAL_DAY,
  INTERVAL_HOUR,
  INTERVAL_MINUTE,
  INTERVAL_SECOND,
  INTERVAL_MICROSECOND,
  INTERVAL_YEAR_MONTH,
  INTERVAL_DAY_HOUR,
  INTERVAL_DAY_MINUTE,
  INTERVAL_DAY_SECOND,
  INTERVAL_HOUR_MINUTE,
  INTERVAL_HOUR_SECOND,
  INTERVAL_MINUTE_SECOND,
  INTERVAL_DAY_MICROSECOND,
  INTERVAL_HOUR_MICROSECOND,
  INTERVAL_MINUTE_MICROSECOND,
  INTERVAL_SECOND_MICROSECOND,
  INTERVAL_LAST
};

bool date_add_interval(MYSQL_TIME *ltime, interval_type int_type,
                       Interval interval, int *warnings);

/**
   Round the input argument to the specified precision by computing
   the remainder modulo log10 of the difference between max and
   desired precision.

   @param nr number to round
   @param decimals desired precision
   @return nr rounded according to the desired precision.
*/
inline long my_time_fraction_remainder(long nr, unsigned int decimals) {
  assert(decimals <= DATETIME_MAX_DECIMALS);
  return nr % static_cast<long>(log_10_int[DATETIME_MAX_DECIMALS - decimals]);
}

/**
   Truncate the number of microseconds in MYSQL_TIME::second_part to
   the desired precision.

   @param ltime time point
   @param decimals desired precision
*/
inline void my_time_trunc(MYSQL_TIME *ltime, unsigned int decimals) {
  ltime->second_part -=
      my_time_fraction_remainder(ltime->second_part, decimals);
}

/**
   Alias for my_time_trunc.

   @param ltime time point
   @param decimals desired precision
 */
inline void my_datetime_trunc(MYSQL_TIME *ltime, unsigned int decimals) {
  return my_time_trunc(ltime, decimals);
}

/**
   Truncate the tv_usec member of a posix timeval struct to the
   specified number of decimals.

   @param tv timepoint/duration
   @param decimals desired precision
 */
inline void my_timeval_trunc(my_timeval *tv, unsigned int decimals) {
  tv->m_tv_usec -= my_time_fraction_remainder(tv->m_tv_usec, decimals);
}

/**
   Predicate for fuzzyness of date.

   @param my_time time point to check
   @param fuzzydate bitfield indicating if fuzzy dates are premitted
   @retval true if TIME_FUZZY_DATE is unset and either month or day is 0
   @retval false otherwise
 */
inline bool check_fuzzy_date(const MYSQL_TIME &my_time,
                             my_time_flags_t fuzzydate) {
  return !(fuzzydate & TIME_FUZZY_DATE) && (!my_time.month || !my_time.day);
}

/**
 Predicate which returns true if at least one of the date members are non-zero.

 @param my_time time point to check.
 @retval false if all the date members are zero
 @retval true otherwise
 */
inline bool non_zero_date(const MYSQL_TIME &my_time) {
  return my_time.year || my_time.month || my_time.day;
}

/**
 Predicate which returns true if at least one of the time members are non-zero.

 @param my_time time point to check.
 @retval false if all the time members are zero
 @retval true otherwise
*/
inline bool non_zero_time(const MYSQL_TIME &my_time) {
  return my_time.hour || my_time.minute || my_time.second ||
         my_time.second_part;
}

/**
   "Casts" MYSQL_TIME datetime to a MYSQL_TIME time. Sets
   MYSQL_TIME::time_type to MYSQL_TIMESTAMP_TIME and zeroes out the
   date members.

   @param ltime timepoint to cast
 */
inline void datetime_to_time(MYSQL_TIME *ltime) {
  ltime->year = 0;
  ltime->month = 0;
  ltime->day = 0;
  ltime->time_type = MYSQL_TIMESTAMP_TIME;
}

/**
   "Casts" MYSQL_TIME datetime to a MYSQL_TIME date. Sets
   MYSQL_TIME::time_type to MYSQL_TIMESTAMP_DATE and zeroes out the
   time members.

   @param ltime timepoint to cast
*/
inline void datetime_to_date(MYSQL_TIME *ltime) {
  ltime->hour = 0;
  ltime->minute = 0;
  ltime->second = 0;
  ltime->second_part = 0;
  ltime->time_type = MYSQL_TIMESTAMP_DATE;
}

/**
   "Casts" a MYSQL_TIME to datetime by setting MYSQL_TIME::time_type to
   MYSQL_TIMESTAMP_DATETIME.
   @note There is no check to ensure that the result is a valid datetime.

   @param ltime timpoint to cast
*/
inline void date_to_datetime(MYSQL_TIME *ltime) {
  ltime->time_type = MYSQL_TIMESTAMP_DATETIME;
}

bool time_add_nanoseconds_with_truncate(MYSQL_TIME *ltime,
                                        unsigned int nanoseconds,
                                        int *warnings);
bool datetime_add_nanoseconds_with_truncate(MYSQL_TIME *ltime,
                                            unsigned int nanoseconds);
bool time_add_nanoseconds_with_round(MYSQL_TIME *ltime,
                                     unsigned int nanoseconds, int *warnings);

bool datetime_add_nanoseconds_with_round(MYSQL_TIME *ltime,
                                         unsigned int nanoseconds,
                                         int *warnings);

bool time_add_nanoseconds_adjust_frac(MYSQL_TIME *ltime,
                                      unsigned int nanoseconds, int *warnings,
                                      bool truncate);

bool datetime_add_nanoseconds_adjust_frac(MYSQL_TIME *ltime,
                                          unsigned int nanoseconds,
                                          int *warnings, bool truncate);

bool my_time_adjust_frac(MYSQL_TIME *ltime, unsigned int dec, bool truncate);
bool my_datetime_adjust_frac(MYSQL_TIME *ltime, unsigned int dec, int *warnings,
                             bool truncate);
bool my_timeval_round(my_timeval *tv, unsigned int decimals);
void mix_date_and_time(MYSQL_TIME *ldate, const MYSQL_TIME &my_time);

void localtime_to_TIME(MYSQL_TIME *to, const struct tm *from);
void calc_time_from_sec(MYSQL_TIME *to, long long int seconds,
                        long microseconds);
bool calc_time_diff(const MYSQL_TIME &my_time1, const MYSQL_TIME &my_time2,
                    int l_sign, long long int *seconds_out,
                    long *microseconds_out);
int my_time_compare(const MYSQL_TIME &my_time_a, const MYSQL_TIME &my_time_b);

long long int TIME_to_longlong_packed(const MYSQL_TIME &my_time,
                                      enum enum_field_types type);

void TIME_from_longlong_packed(MYSQL_TIME *ltime, enum enum_field_types type,
                               long long int packed_value);

long long int longlong_from_datetime_packed(enum enum_field_types type,
                                            long long int packed_value);

double double_from_datetime_packed(enum enum_field_types type,
                                   long long int packed_value);

/**
  @} (end of ingroup MY_TIME)
*/
#endif /* MY_TIME_INCLUDED */
