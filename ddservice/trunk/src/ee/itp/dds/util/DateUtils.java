package ee.itp.dds.util;

import java.text.DateFormat;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.GregorianCalendar;

/**
 * DateUtil
 * <p/>
 * Set of utility methods to work with date
 * <p/>
 * .
 * 
 * @author Konstantin Tarletski
 */
public class DateUtils {
  

  /**
   * The Constant DATE_FORMAT.
   */
  private static final DateFormat DATE_FORMAT = createDateFormat();

  /**
   * The Constant TIME_FORMAT.
   */
  private static final DateFormat TIME_FORMAT = new SimpleDateFormat("HH:mm");

  /**
   * The Constant DATE_AND_TIME_FORMAT.
   */
  private static final DateFormat DATE_AND_TIME_FORMAT = new SimpleDateFormat(
      "dd.MM.yyyy HH:mm:ss");

  /**
   * The Constant MONTHYEAR_FORMAT.
   */
  private static final DateFormat MONTHYEAR_FORMAT = new SimpleDateFormat(
      "MM.yyyy");

  /**
   * The Constant MONTH_FORMAT.
   */
  private static final DateFormat MONTH_FORMAT = new SimpleDateFormat("MM");

  /**
   * The Constant YEAR_FORMAT.
   */
  private static final DateFormat YEAR_FORMAT = new SimpleDateFormat("yyyy");

  /**
   * The Constant SHORTYEAR_FORMAT.
   */
  private static final DateFormat SHORTYEAR_FORMAT = new SimpleDateFormat("yy");

  /**
   * Today.
   * 
   * @return todays date with truncated time
   */
  public static Date today() {
    return truncateTime(new Date());
  }

  /**
   * Yesterday.
   * 
   * @return the date
   */
  public static Date yesterday() {
    final Calendar calendar = new GregorianCalendar();
    calendar.add(Calendar.DAY_OF_MONTH, -1);
    return truncateTime(calendar).getTime();
  }

  /**
   * Previous month first day.
   * 
   * @return the date
   */
  public static Date previousMonthFirstDay() {
    final Calendar calendar = new GregorianCalendar();
    calendar.add(Calendar.MONTH, -1);
    calendar.set(Calendar.DAY_OF_MONTH, 1);
    return truncateTime(calendar).getTime();
  }

  /**
   * Previous month last day.
   * 
   * @return the date
   */
  public static Date previousMonthLastDay() {
    final Calendar calendar = new GregorianCalendar();
    calendar.add(Calendar.MONTH, -1);
    calendar.set(Calendar.DAY_OF_MONTH, calendar
        .getActualMaximum(Calendar.DAY_OF_MONTH));
    return truncateTime(calendar).getTime();
  }

  /**
   * Current month first day.
   * 
   * @return the date
   */
  public static Date currentMonthFirstDay() {
    final Calendar calendar = new GregorianCalendar();
    calendar.set(Calendar.DAY_OF_MONTH, 1);
    return truncateTime(calendar).getTime();
  }

  /**
   * Current month last day.
   * 
   * @return the date
   */
  public static Date currentMonthLastDay() {
    final Calendar calendar = new GregorianCalendar();
    calendar.set(Calendar.DAY_OF_MONTH, calendar
        .getActualMaximum(Calendar.DAY_OF_MONTH));
    return truncateTime(calendar).getTime();
  }

  /**
   * Calculates total amount of days in provided month
   * 
   * @param date
   *          provided date
   * 
   * @return days in given month
   */
  public static int countDaysInMonth(Date date) {
    Calendar calendar = createCalendar(date);
    return calendar.getActualMaximum(Calendar.DAY_OF_MONTH);
  }

  /**
   * Current year first day.
   * 
   * @return the date
   */
  public static Date currentYearFirstDay() {
    final Calendar calendar = new GregorianCalendar();
    calendar.set(Calendar.DAY_OF_YEAR, 1);
    return truncateTime(calendar).getTime();
  }

  /**
   * Checks wheiter given date is in past or not. Compares dates only (without
   * time!)
   * 
   * @param date
   *          date to check
   * 
   * @return true, if given date is in past; false otherwise
   */
  public static boolean isPast(Date date) {
    return today().after(truncateTime(date));
  }

  /**
   * Checks if is same day.
   * 
   * @param d1
   *          day one
   * @param d2
   *          day two
   * 
   * @return true, if given two dates are within same calendar day boundary
   *         (year+month+day are the same for both). Time value ignored
   */
  public static boolean isSameDay(Date d1, Date d2) {
    return !(d1 == null || d2 == null)
        && truncateTime(d1).equals(truncateTime(d2));
  }

  /**
   * Checks if is today.
   * 
   * @param date
   *          any date
   * 
   * @return true, if given date is today (time part ignored); false otherwise;
   *         also returns false if given date is null
   */
  public static boolean isToday(Date date) {
    return isSameDay(date, new Date());
  }

  /**
   * Checks if is same month.
   * 
   * @param d1
   *          day one
   * @param d2
   *          day two
   * 
   * @return true, if given two dates are within same calendar month boundary
   *         (year+month are the same for both). Day and time value ignored
   */
  public static boolean isSameMonth(Date d1, Date d2) {
    if (d1 == null || d2 == null) {
      return false;
    }
    final Calendar c1 = createCalendar(d1);
    final Calendar c2 = createCalendar(d2);
    return c1.get(Calendar.YEAR) == c2.get(Calendar.YEAR)
        && c1.get(Calendar.MONTH) == c2.get(Calendar.MONTH);
  }

  /**
   * Checks if is same year.
   * 
   * @param d1
   *          day one
   * @param d2
   *          day two
   * 
   * @return true, if given two dates are within same calendar year boundary
   *         (year are the same for both). Day and time value ignored
   */
  public static boolean isSameYear(Date d1, Date d2) {
    if (d1 == null || d2 == null) {
      return false;
    }
    final Calendar c1 = createCalendar(d1);
    final Calendar c2 = createCalendar(d2);
    return c1.get(Calendar.YEAR) == c2.get(Calendar.YEAR);
  }

  /**
   * Truncates time to 00:00:00 000.
   * 
   * @param date
   *          date to be truncated
   * 
   * @return truncated time
   */
  public static Date truncateTime(Date date) {
    final Calendar calendar = createCalendar(date);
    return truncateTime(calendar).getTime();
  }

  /**
   * Truncate time.
   * 
   * @param calendar
   *          the calendar
   * 
   * @return the calendar
   */
  public static Calendar truncateTime(Calendar calendar) {
    calendar.set(Calendar.HOUR, 0);
    calendar.set(Calendar.HOUR_OF_DAY, 0);
    calendar.set(Calendar.MINUTE, 0);
    calendar.set(Calendar.SECOND, 0);
    calendar.set(Calendar.AM_PM, Calendar.AM);
    calendar.set(Calendar.MILLISECOND, 0);
    return calendar;
  }

  /**
   * Calculates first day of month for a given date value. NB! No time
   * calculation applied
   * 
   * @param date
   *          {@link java.util.Date} instance
   * 
   * @return first day of month for a given date as {@link java.util.Date}
   */
  public static Date firstDayOfMonth(Date date) {
    final Calendar calendar = createCalendar(date);
    calendar.set(Calendar.DAY_OF_MONTH, 1);
    return calendar.getTime();
  }

  /**
   * Calculates first day of year for a given date value. NB! No time
   * calculation applied
   * 
   * @param date
   *          {@link java.util.Date} instance
   * 
   * @return first day of month for a given date as {@link java.util.Date}
   */
  public static Date firstDayOfYear(Date date) {
    final Calendar calendar = createCalendar(date);
    calendar.set(Calendar.DAY_OF_YEAR, 1);
    return calendar.getTime();
  }

  /**
   * Get the Name of the Month.
   * 
   * @param monthIndex
   *          the month index
   * 
   * @return month name by index
   */
  public static String getMonthName(int monthIndex) {
    final String[] monthNames = new String[] { "Jaanuar", "Veebruar", "MГ¤rts",
        "Aprill", "Mai", "Juuni", "Juuli", "August", "September", "Oktoober",
        "November", "Detsember" };
    if (monthIndex >= 0 && monthIndex < monthNames.length) {
      return monthNames[monthIndex];
    } else {
      return null;
    }
  }

  /**
   * Get current month number (0 - JANUARY, ... , 11 - DECEMBER)
   * 
   * @return int
   */
  public static int getCurrentMonthNumber() {
    final Calendar calendar = createCalendar(new Date());
    return calendar.get(Calendar.MONTH);
  }

  /**
   * Calculates last day of month for a given date value. NB! No time
   * calculation applied
   * 
   * @param date
   *          {@link java.util.Date} instance
   * 
   * @return last day of month for a given date as {@link java.util.Date}
   */
  public static Date lastDayOfMonth(Date date) {
    final Calendar calendar = createCalendar(date);
    calendar.set(Calendar.DAY_OF_MONTH, calendar
        .getActualMaximum(Calendar.DAY_OF_MONTH));
    return calendar.getTime();
  }

  /**
   * Creates date instance from the string date in format dd.MM.yyyy. Useful in
   * situations where date format is known
   * 
   * @param date
   *          date as string in format dd.MM.yyyy (ex. 12.05.2003)
   * 
   * @return parsed {@link java.util.Date} instance
   */
  public synchronized static Date createDate(String date) {
    try {
      return DATE_FORMAT.parse(date);
    } catch (ParseException e) {
      return null;
    }
  }

  /**
   * Creates a calendar instance using given date, just to save up space and
   * effort of writing 2 lines of code each time you need a calendar from date.
   * 
   * @param date
   *          {@link java.util.Date} instance
   * 
   * @return instance of {@link java.util.Calendar}
   */
  public static Calendar createCalendar(Date date) {
    final Calendar calendar = new GregorianCalendar();
    calendar.setTime(date);
    return calendar;
  }

  /**
   * get day of week.
   * 
   * @param date
   *          date object
   * 
   * @return - int value of week day (1 is Sunday, 2 is Monday, ..., and 7 is
   *         Saturday)
   */

  public static int getDayOfWeek(Date date) {
    return createCalendar(date).get(Calendar.DAY_OF_WEEK);
  }

  /**
   * Returns day of month by given date
   * 
   * @param date
   *          given date
   * 
   * @return day of month int value
   */
  public static int getDayOfMonth(Date date) {
    return createCalendar(date).get(Calendar.DAY_OF_MONTH);
  }

  /**
   * Returns month number by given date, starting from 1
   * 
   * @param date
   *          given date
   * 
   * @return month number int value
   */
  public static int getMonth(Date date) {
    return createCalendar(date).get(Calendar.MONTH) + 1;
  }

  /**
   * Returns year number by given date
   * 
   * @param date
   *          given date
   * 
   * @return year number int value
   */
  public static int getYear(Date date) {
    return createCalendar(date).get(Calendar.YEAR);
  }

  /**
   * Formats date as string using dd.MM.yyyy format. Implementation is
   * synhronized.
   * 
   * @param date
   *          date object
   * 
   * @return date formatted as string
   */
  public static synchronized String formatDate(final Date date) {
    if (date == null) {
      return null;
    }
    return DATE_FORMAT.format(date);
  }

  /**
   * Formats date with given pattern. Formatter is created on each method call
   * 
   * @param date
   *          date to format
   * @param pattern
   *          pattern to format with
   * 
   * @return formatted date
   */
  public synchronized static String formatDate(final Date date, String pattern) {
    if (date == null) {
      return null;
    }
    return new SimpleDateFormat(pattern).format(date);
  }

  /**
   * Formats date as string using MM.yyyy format
   * 
   * @param date
   *          date object
   * 
   * @return date formatted as string
   */
  public synchronized static String formatMonthYear(final Date date) {
    if (date == null) {
      return null;
    }
    return MONTHYEAR_FORMAT.format(date);
  }

  /**
   * return date month as string format "MM".
   * 
   * @param date
   *          date object
   * 
   * @return date formatted as string
   */
  public synchronized static String formatMonth(final Date date) {
    if (date == null) {
      return null;
    }
    return MONTH_FORMAT.format(date);
  }

  /**
   * return date year as string format "yyyy".
   * 
   * @param date
   *          date object
   * 
   * @return date formatted as string
   */
  public synchronized static String formatYear(final Date date) {
    if (date == null) {
      return null;
    }
    return YEAR_FORMAT.format(date);
  }

  /**
   * Format short year.
   * 
   * @param date
   *          the date
   * 
   * @return the string
   */
  public synchronized static String formatShortYear(final Date date) {
    if (date == null) {
      return null;
    }
    return SHORTYEAR_FORMAT.format(date);
  }

  /**
   * Formats date as string using HH:mm format.
   * 
   * @param date
   *          date object
   * 
   * @return time formatted as string
   */
  public synchronized static String formatTime(final Date date) {
    if (date == null) {
      return null;
    }
    return TIME_FORMAT.format(date);
  }

  /**
   * Format date and time.
   * 
   * @param date
   *          the date
   * 
   * @return the string
   */
  public synchronized static String formatDateAndTime(final Date date) {
    return date == null ? "" : DATE_AND_TIME_FORMAT.format(date);
  }

  /**
   * Gets the fixed interval.
   * 
   * @param code
   *          the code
   * 
   * @return the fixed interval
   */
  public static String[] getFixedInterval(String code) {
    return getFixedInterval(code, new Date());

  }

  /**
   * Gets the running year.
   * 
   * @return the running year
   */
  public static int getRunningYear() {
    final Calendar calendar = createCalendar(new Date());
    return calendar.get(Calendar.YEAR);
  }

  /**
   * Gets the fixed interval.
   * 
   * @param code
   *          the code
   * @param date
   *          the date
   * 
   * @return the fixed interval
   */
  public static String[] getFixedInterval(String code, Date date) {
    String[] interval = new String[3];
    final Calendar calendar = createCalendar(date);
    if ("EILE".equals(code)) {
      calendar.add(Calendar.DAY_OF_MONTH, -1);
      interval[0] = formatDate(calendar.getTime());
      interval[1] = interval[0];
    } else if ("TANA".equals(code)) {
      interval[0] = formatDate(calendar.getTime());
      interval[1] = interval[0];
    } else if ("HOMME".equals(code)) {
      calendar.add(Calendar.DAY_OF_MONTH, 1);
      interval[0] = formatDate(calendar.getTime());
      interval[1] = interval[0];
    } else if ("EELKUU".equals(code)) {
      calendar.add(Calendar.MONTH, -1);
      interval[0] = formatDate(firstDayOfMonth(calendar.getTime()));
      interval[1] = formatDate(lastDayOfMonth(calendar.getTime()));
    } else if ("EELKUUALG".equals(code)) {
      calendar.add(Calendar.MONTH, -1);
      interval[0] = formatDate(firstDayOfMonth(calendar.getTime()));
      interval[1] = "";
    }

    else if ("JKSKUU".equals(code)) {
      interval[0] = formatDate(firstDayOfMonth(calendar.getTime()));
      interval[1] = formatDate(lastDayOfMonth(calendar.getTime()));
    } else if ("JRGKUU".equals(code)) {
      calendar.add(Calendar.MONTH, 1);
      interval[0] = formatDate(firstDayOfMonth(calendar.getTime()));
      interval[1] = formatDate(lastDayOfMonth(calendar.getTime()));
    } else if ("AASTAALG".equals(code)) {
      interval[0] = formatDate(firstDayOfYear(calendar.getTime()));
      interval[1] = "";
    } else {
      interval[0] = "";
      interval[1] = "";

    }
    interval[2] = code;
    return interval;
  }

  /**
   * Adds the days.
   * 
   * @param days
   *          the days
   * @param date
   *          the date
   * 
   * @return the date
   */
  public static Date addDays(int days, Date date) {
    return addUnits(Calendar.DATE, days, date);
  }

  public static Date addHours(int hours, Date date) {
    return addUnits(Calendar.HOUR, hours, date);
  }
  
  public static Date addMinutes(int minutes, Date date) {
    return addUnits(Calendar.MINUTE, minutes, date);
  }

  private static Date addUnits(int type, int units, Date date) {
    Calendar c = GregorianCalendar.getInstance();
    c.setTime(date);
    c.add(type, units);
    return c.getTime();
  }

  /**
   * Parses the date.
   * 
   * @param date
   *          the date
   * @param pattern
   *          the pattern
   * 
   * @return the date
   */
  public static Date parseDate(String date, String pattern) {
    try {
      return new SimpleDateFormat(pattern).parse(date);
    } catch (ParseException e) {
      return null;
    }
  }

  /**
   * Parses the date.
   * 
   * @param date
   *          the date
   * @param pattern
   *          the pattern
   * 
   * @return the date
   */
  public static Date parseDateLenient(String date, String pattern) {
    try {
      SimpleDateFormat sdf = new SimpleDateFormat(pattern);
      sdf.setLenient(false);
      return sdf.parse(date);
    } catch (ParseException e) {
      return null;
    }
  }

  /**
   * Calculates amount of days covered between given dates. Both dates are
   * inclusive, this means that for ex. from 10-th to 11-th may will include 2
   * days (10-th and 11-th)
   * 
   * @param startDate
   *          period start date
   * @param endDate
   *          period end date
   * 
   * @return number of dates covered by a given period or ZERO if start date is
   *         later than end date; -1 if either startDate or endDate is NULL
   */
  public static int getDaysIncluded(Date startDate, Date endDate) {
    if (startDate == null || endDate == null) {
      return -1;
    }
    final long interval = endDate.getTime() - startDate.getTime();
    if (interval < 0) {
      return 0;
    }
    return (int) (interval / (60 * 60 * 24 * 1000)) + 1;
  }

  /**
   * Parses the date.
   * 
   * @param date
   *          the date
   * @param format
   *          the format
   * 
   * @return the date
   */
  public static Date parseDate(String date, DateFormat format) {
    try {
      return format.parse(date);
    } catch (ParseException e) {
      return null;
    }
  }

  /**
   * Creates the date format.
   * 
   * @return the date format
   */
  public static DateFormat createDateFormat() {
    return new SimpleDateFormat("dd.MM.yyyy");
  }

  /**
   * Resets the time part of a given date to the last millisecond of a given day
   * (23.59.59.999)
   * 
   * @param date
   *          date
   * 
   * @return new Date with resetted time partn
   */
  public static Date endOfDay(final Date date) {
    final Calendar calendar = truncateTime(createCalendar(date));
    calendar.add(Calendar.DAY_OF_MONTH, 1);
    calendar.add(Calendar.MILLISECOND, -1);
    return calendar.getTime();
  }

}
