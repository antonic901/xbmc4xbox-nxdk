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

void* __RTtypeid(void * inptr)
{
  return nullptr;
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
  TIME_FIELDS timefields;
  timefields.Year = lpSystemTime->wYear;
  timefields.Day = lpSystemTime->wDay;
  timefields.Hour = lpSystemTime->wHour;
  timefields.Minute = lpSystemTime->wMinute;
  timefields.Second = lpSystemTime->wSecond;
  timefields.Millisecond = lpSystemTime->wMilliseconds;

  LARGE_INTEGER filetime;
  if (!RtlTimeFieldsToTime(&timefields, &filetime))
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }

  lpFileTime->dwLowDateTime = filetime.LowPart;
  lpFileTime->dwHighDateTime = filetime.HighPart;
  return TRUE;
}

LONG CompareFileTime(const FILETIME *lpFileTime1, const FILETIME *lpFileTime2)
{
  ULARGE_INTEGER filetime1;
  ULARGE_INTEGER filetime2;

  filetime1.LowPart = lpFileTime1->dwLowDateTime;
  filetime1.HighPart = lpFileTime1->dwHighDateTime;
  filetime2.LowPart = lpFileTime2->dwLowDateTime;
  filetime2.HighPart = lpFileTime2->dwHighDateTime;

  if (filetime1.QuadPart < filetime2.QuadPart)
    return -1;
  if (filetime1.QuadPart > filetime2.QuadPart)
    return 1;
  return 0;
}

BOOL FileTimeToSystemTime(const FILETIME *lpFileTime, LPSYSTEMTIME lpSystemTime)
{
  LARGE_INTEGER filetime;
  filetime.HighPart = lpFileTime->dwHighDateTime;
  if (filetime.QuadPart < 0)
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }

  TIME_FIELDS timefields;
  RtlTimeToTimeFields(&filetime, &timefields);

  lpSystemTime->wYear = timefields.Year;
  lpSystemTime->wMonth = timefields.Month;
  lpSystemTime->wDay = timefields.Day;
  lpSystemTime->wDayOfWeek = timefields.Weekday;
  lpSystemTime->wHour = timefields.Hour;
  lpSystemTime->wMinute = timefields.Minute;
  lpSystemTime->wSecond = timefields.Second;
  lpSystemTime->wMilliseconds = timefields.Millisecond;

  return TRUE;
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
