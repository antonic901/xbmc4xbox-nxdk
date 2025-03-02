#include "unimplemented.h"

void __std_terminate() { std::terminate(); }
void __CxxFrameHandler3() {}
void _CxxThrowException(void*, void*) {}

void testException()
{
  try
  {
    throw std::runtime_error("Test exception!");
  }
  catch (const std::exception& e)
  {
    // Catch standard exceptions
  }
  catch (...)
  {
    // Catch any other unknown exceptions
  }
}

void* __RTDynamicCast(void* ptr, long, void*, bool)
{
  return ptr;
}

int WideCharToMultiByte(unsigned int codePage, unsigned long flags,
                        const wchar_t* wideStr, int wideLen,
                        char* multiByteStr, int multiByteLen,
                        const char* defaultChar, bool* usedDefaultChar)
{
  if (codePage != CP_UTF8)
    return 0;

  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  std::string utf8Str = converter.to_bytes(wideStr, wideStr + wideLen);

  if (multiByteStr && multiByteLen > 0)
  {
    size_t copyLen = (utf8Str.size() < (size_t)multiByteLen) ? utf8Str.size() : (size_t)multiByteLen;
    memcpy(multiByteStr, utf8Str.c_str(), copyLen);
    return (int)copyLen;
  }

  return (int)utf8Str.size();
}

int MultiByteToWideChar(unsigned int codePage, unsigned long flags,
                        const char* multiByteStr, int multiByteLen,
                        wchar_t* wideStr, int wideLen)
{
  if (codePage != CP_UTF8)
    return 0;

  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  std::wstring wide = converter.from_bytes(multiByteStr, multiByteStr + multiByteLen);

  if (wideStr && wideLen > 0)
  {
    size_t copyLen = (wide.size() < (size_t)wideLen) ? wide.size() : (size_t)wideLen;
    wcsncpy(wideStr, wide.c_str(), copyLen);
    return (int)copyLen;
  }

  return (int)wide.size();
}

static const uint64_t EPOCH_DIFFERENCE = 116444736000000000ULL; // 1601 to 1970 in 100ns units
static const uint64_t HUNDRED_NS_PER_SECOND = 10000000ULL;

BOOL FileTimeToLocalFileTime(const FILETIME *lpFileTime, LPFILETIME lpLocalFileTime)
{
  uint64_t lpFileTimeValue = ((uint64_t)lpFileTime->dwHighDateTime << 32) | lpFileTime->dwLowDateTime;

  TIME_ZONE_INFORMATION tzInfo;
  DWORD result = GetTimeZoneInformation(&tzInfo);
  if (result == TIME_ZONE_ID_INVALID)
    return 0;

  int64_t bias = (int64_t)tzInfo.Bias * 600000000LL;
  if (result == TIME_ZONE_ID_STANDARD)
    bias += (int64_t)tzInfo.StandardBias * 600000000LL;
  else if (result == TIME_ZONE_ID_DAYLIGHT)
    bias += (int64_t)tzInfo.DaylightBias * 600000000LL;

  uint64_t localTimeValue = lpFileTimeValue - bias;

  lpLocalFileTime->dwLowDateTime = (uint32_t)localTimeValue;
  lpLocalFileTime->dwHighDateTime = (uint32_t)(localTimeValue >> 32);
  return 1;
}

BOOL SystemTimeToFileTime(const SYSTEMTIME *lpSystemTime, LPFILETIME lpFileTime)
{
  struct tm t = {0};
  t.tm_year = lpSystemTime->wYear - 1900;
  t.tm_mon = lpSystemTime->wMonth - 1;
  t.tm_mday = lpSystemTime->wDay;
  t.tm_hour = lpSystemTime->wHour;
  t.tm_min = lpSystemTime->wMinute;
  t.tm_sec = lpSystemTime->wSecond;

  time_t posixTime = mktime(&t);
  if (posixTime == -1)
    return 0;

  uint64_t timeIn100ns = ((uint64_t)posixTime * HUNDRED_NS_PER_SECOND) + EPOCH_DIFFERENCE;
  timeIn100ns += lpSystemTime->wMilliseconds * 10000ULL;

  lpFileTime->dwLowDateTime = (uint32_t)timeIn100ns;
  lpFileTime->dwHighDateTime = (uint32_t)(timeIn100ns >> 32);
  return 1;
}

LONG CompareFileTime(const FILETIME *lpFileTime1, const FILETIME *lpFileTime2)
{
  uint64_t time1 = ((uint64_t)lpFileTime1->dwHighDateTime << 32) | lpFileTime1->dwLowDateTime;
  uint64_t time2 = ((uint64_t)lpFileTime2->dwHighDateTime << 32) | lpFileTime2->dwLowDateTime;

  if (time1 < time2)
    return -1;
  else if (time1 > time2)
    return 1;
  else
    return 0;
}

BOOL FileTimeToSystemTime(const FILETIME *lpFileTime, LPSYSTEMTIME lpSystemTime)
{
  uint64_t lpFileTimeValue = ((uint64_t)lpFileTime->dwHighDateTime << 32) | lpFileTime->dwLowDateTime;

  time_t unixTime = (lpFileTimeValue / HUNDRED_NS_PER_SECOND) - EPOCH_DIFFERENCE;

  struct tm* utcTime = gmtime(&unixTime);
  if (!utcTime)
    return 0;

  lpSystemTime->wYear = utcTime->tm_year + 1900;
  lpSystemTime->wMonth = utcTime->tm_mon + 1;
  lpSystemTime->wDay = utcTime->tm_mday;
  lpSystemTime->wHour = utcTime->tm_hour;
  lpSystemTime->wMinute = utcTime->tm_min;
  lpSystemTime->wSecond = utcTime->tm_sec;
  lpSystemTime->wMilliseconds = (lpFileTimeValue % HUNDRED_NS_PER_SECOND) / 10000;
  lpSystemTime->wDayOfWeek = utcTime->tm_wday;

  return 1;
}

BOOL LocalFileTimeToFileTime(const FILETIME *lpLocalFileTime, LPFILETIME lpFileTime)
{
  uint64_t localTimeValue = ((uint64_t)lpLocalFileTime->dwHighDateTime << 32) | lpLocalFileTime->dwLowDateTime;

  TIME_ZONE_INFORMATION tzInfo;
  DWORD result = GetTimeZoneInformation(&tzInfo);
  if (result == TIME_ZONE_ID_INVALID)
    return 0;

  int64_t bias = (int64_t)tzInfo.Bias * 600000000LL;
  if (result == TIME_ZONE_ID_STANDARD)
    bias += (int64_t)tzInfo.StandardBias * 600000000LL;
  else if (result == TIME_ZONE_ID_DAYLIGHT)
    bias += (int64_t)tzInfo.DaylightBias * 600000000LL;

  uint64_t fileTimeValue = localTimeValue + bias;

  lpFileTime->dwLowDateTime = (uint32_t)fileTimeValue;
  lpFileTime->dwHighDateTime = (uint32_t)(fileTimeValue >> 32);
  return 1;
}

double atof (const char* str)
{
  return strtod(str, nullptr);
}
