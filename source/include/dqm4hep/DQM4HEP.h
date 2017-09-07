/// \file DQM4HEP.h
/*
 *
 * DQM4HEP.h header template automatically generated by a class generator
 * Creation date : dim. oct. 5 2014
 *
 * This file is part of DQM4HEP libraries.
 *
 * DQM4HEP is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * based upon these libraries are permitted. Any copy of these libraries
 * must include this copyright notice.
 *
 * DQM4HEP is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DQM4HEP.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @author Remi Ete
 * @copyright CNRS , IPNL
 */


#ifndef DQM4HEP_H
#define DQM4HEP_H

// -- std headers
#include <set>
#include <iostream>
#include <sstream>
#include <utility>
#include <map>
#include <vector>
#include <list>
#include <queue>
#include <string>
#include <ctime>
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <exception>
#include <limits>
#include <pthread.h>

#ifndef __CINT__
#include <memory>
#include <chrono>
#include <ratio>
#endif


// apple stuff for stdint.h
#ifdef __APPLE__
#include <_types.h>
#include <_types/_uint8_t.h>
#include <_types/_uint16_t.h>
#include <_types/_uint32_t.h>
#include <_types/_uint64_t.h>
#include <sys/_types/_int16_t.h>
#include <sys/_types/_int64_t.h>
#include <sys/_pthread/_pthread_types.h>
#include <sys/_pthread/_pthread_mutex_t.h>
#else
#include <bits/pthreadtypes.h>
#include <stdint.h>
#endif

// -- dqm4hep headers
#include "DQMCoreConfig.h"

//-------------------------------------------------------------------------------------------------

#if defined(__GNUC__) && defined(BACKTRACE)
#include <cstdlib>
#include <execinfo.h>
#endif

// limits definitions platform independent
#ifdef _MSC_VER
#include <float.h>
#define IS_NAN _isnan
#define IS_INF !_finite
#else
#define IS_NAN std::isnan
#define IS_INF std::isinf
#endif

namespace dqm4hep {

  namespace core {

    class Event;
    class MonitorElement;
    template <typename T>
    class Streamer;
    class EventStreamer;
    class QualityTest;
    class QualityTestResult;
    class StatisticsService;
    class Path;
    struct Stats;

    /** allocator_helper class
     */
    template <typename BaseType, typename RealType, typename... Args>
    class allocator_helper
    {
    public:
      /** Create a new instance of RealType, inheriting
       *  from BaseType using Args type as constructor parameters
       */
      BaseType *create(Args ... args) const
      {
        return new RealType(args...);
      }
    };

    // C++11 stuff incompatible with CINT
#ifndef __CINT__
    // time
    typedef std::chrono::system_clock::time_point               TimePoint;
    typedef std::chrono::duration<double>                       TimeDuration;

    // event
    typedef std::queue<Event*>                                  EventQueue;
    typedef std::shared_ptr<Event>                              EventPtr;
    typedef std::queue<EventPtr>                                EventPtrQueue;

    typedef std::shared_ptr<MonitorElement>                     MonitorElementPtr;
    typedef std::vector<MonitorElementPtr>                      MonitorElementPtrList;
    typedef std::map<const std::string, MonitorElementPtr>      MonitorElementPtrMap;
    typedef std::shared_ptr<QualityTest>                        QualityTestPtr;

    typedef std::map<std::string, MonitorElementPtrList>        Publication;
#endif

    // typedefs for streaming
    typedef char                  dqm_char;
    typedef uint8_t               dqm_uchar;
    typedef int16_t               dqm_short;
    typedef uint16_t              dqm_ushort;
    typedef int16_t               dqm_short_int;
    typedef uint16_t              dqm_ushort_int;
    typedef int64_t               dqm_lint;
    typedef uint64_t              dqm_ulint;
    typedef float                 dqm_real;
    typedef float                 dqm_float;
    typedef double                dqm_double;
    typedef bool                  dqm_bool;
    typedef int64_t               dqm_int;
    typedef uint64_t              dqm_uint;

    // basic typedefs
    typedef std::vector<int>            IntVector;
    typedef std::vector<unsigned int>   UIntVector;
    typedef std::vector<float>          FloatVector;
    typedef std::vector<double>         DoubleVector;
    typedef std::vector<std::string>    StringVector;
    typedef std::set<std::string>       StringSet;
    typedef std::map<std::string, std::string> ParameterMap;

    typedef std::vector<MonitorElement*>                        MonitorElementList;
    typedef std::map<const std::string, MonitorElement*>        MonitorElementMap;
    typedef std::map<std::string, QualityTestResult>            QualityTestResultMap;
    typedef std::map<std::string, QualityTest*>                 QualityTestMap;
    typedef std::vector<Stats>                                  StatsList;

    // typedef for messaging
    typedef std::map<std::string, std::string>                  MonitorElementInfo;
    typedef std::vector<MonitorElementInfo>                     MonitorElementInfoList;
    typedef std::map<std::string, std::string>                  HostInfo;
    typedef std::map<std::string, std::string>                  MonitorElementListNameRequest;
    typedef std::multimap<std::string, std::string>             MonitorElementRequest;
  }
}

//-------------------------------------------------------------------------------------------------

#define RETURN_RESULT_IF(StatusCode1, Operator, Command)		\
    {									\
  const dqm4hep::core::StatusCode statusCode(Command);			\
  if (statusCode Operator StatusCode1)				\
  {									\
    dqm_error( "{0} return {1}, "#Command, dqm4hep::core::statusCodeToString(statusCode) ); \
    dqm_error( "    in function: {0}", __FUNCTION__ ); \
    dqm_error( "    in file:     {0} line#: {1}", __FILE__, __LINE__ ); \
    return statusCode;						\
  }									\
    }

//-------------------------------------------------------------------------------------------------

#define RETURN_RESULT_IF_AND_IF(StatusCode1, StatusCode2, Operator, Command) \
    {									\
  const dqm4hep::core::StatusCode statusCode(Command);			\
  if ((statusCode Operator StatusCode1) && (statusCode Operator StatusCode2))	\
  {									\
    dqm_error( "{0} return {1}, "#Command, dqm4hep::core::statusCodeToString(statusCode) ); \
    dqm_error( "    in function: {0}", __FUNCTION__ ); \
    dqm_error( "    in file:     {0} line#: {1}", __FILE__, __LINE__ ); \
    return statusCode;						\
  }									\
    }

//-------------------------------------------------------------------------------------------------

#define THROW_RESULT_IF(StatusCode1, Operator, Command)			\
    {									\
  const dqm4hep::core::StatusCode statusCode(Command);			\
  if (statusCode Operator StatusCode1)				\
  {									\
    dqm_error( "{0} return {1}, "#Command, dqm4hep::core::statusCodeToString(statusCode) ); \
    dqm_error( "    in function: {0}", __FUNCTION__ ); \
    dqm_error( "    in file:     {0} line#: {1}", __FILE__, __LINE__ ); \
    throw dqm4hep::core::StatusCodeException(statusCode);			\
  }									\
    }

//-------------------------------------------------------------------------------------------------

#define THROW_RESULT_IF_AND_IF(StatusCode1, StatusCode2, Operator, Command)                     \
    {									\
  const dqm4hep::core::StatusCode statusCode(Command);			\
  if ((statusCode Operator StatusCode1) && (statusCode Operator StatusCode2))	\
  {									\
    dqm_error( "{0} return {1}, "#Command, dqm4hep::core::statusCodeToString(statusCode) ); \
    dqm_error( "    in function: {0}", __FUNCTION__ ); \
    dqm_error( "    in file:     {0} line#: {1}", __FILE__, __LINE__ ); \
    throw dqm4hep::core::StatusCodeException(statusCode);			\
  }									\
    }

//-------------------------------------------------------------------------------------------------

#define PROCESS_CODE_IF_AND_RETURN(StatusCode1, Operator, Command, Code) \
    {									\
  const dqm4hep::core::StatusCode statusCode(Command);			\
  if (statusCode Operator StatusCode1)				\
  {									\
    Code								\
    return statusCode;						\
  }									\
    }

//-------------------------------------------------------------------------------------------------

#define PROCESS_CODE_IF_AND_IF_AND_RETURN(StatusCode1, StatusCode2, Operator, Command, Code) \
    {									\
  const dqm4hep::core::StatusCode statusCode(Command);			\
  if ((statusCode Operator StatusCode1) && (statusCode Operator StatusCode2))	\
  {									\
    Code								\
    return statusCode;						\
  }									\
    }

//-------------------------------------------------------------------------------------------------

#define NOTIFY_METHOD_CALLED dqm_debug( "Method called : {0}", __FUNCTION__ );

//-------------------------------------------------------------------------------------------------

// macros for enumerators
#define GET_ENUM_ENTRY(a, b)  a,
#define GET_NAME_SWITCH(a, b) case a : return b;
#define GET_STR_COMPARE(a, b) if(str == b) return a;

#define GET_ENUM_ENTRY_2(a, b, c) a,
#define GET_NAME_SWITCH_2(a, b, c) case a : return b;
#define GET_PREFIX_SWITCH_2(a, b, c) case a : return c;
#define GET_STR_COMPARE_1(a, b, c) if(str == b) return a;
#define GET_STR_COMPARE_2(a, b, c) if(str == c) return a;


// definitions of the status code table
#define STATUS_CODE_TABLE(d)                                                                            \
    d(STATUS_CODE_SUCCESS,                  "STATUS_CODE_SUCCESS"                   )                   \
    d(STATUS_CODE_FAILURE,                  "STATUS_CODE_FAILURE"                   )                   \
    d(STATUS_CODE_NOT_FOUND,                "STATUS_CODE_NOT_FOUND"                 )                   \
    d(STATUS_CODE_NOT_INITIALIZED,          "STATUS_CODE_NOT_INITIALIZED"           )                   \
    d(STATUS_CODE_ALREADY_INITIALIZED,      "STATUS_CODE_ALREADY_INITIALIZED"       )                   \
    d(STATUS_CODE_ALREADY_PRESENT,          "STATUS_CODE_ALREADY_PRESENT"           )                   \
    d(STATUS_CODE_OUT_OF_RANGE,             "STATUS_CODE_OUT_OF_RANGE"              )                   \
    d(STATUS_CODE_NOT_ALLOWED,              "STATUS_CODE_NOT_ALLOWED"               )                   \
    d(STATUS_CODE_INVALID_PARAMETER,        "STATUS_CODE_INVALID_PARAMETER"         )                   \
    d(STATUS_CODE_UNCHANGED,                "STATUS_CODE_UNCHANGED"                 )                   \
    d(STATUS_CODE_INVALID_PTR,              "STATUS_CODE_INVALID_PTR"               )

//-------------------------------------------------------------------------------------------------

namespace dqm4hep {

  namespace core {

    /** StatusCode enumerator
     */
    enum StatusCode
    {
      STATUS_CODE_TABLE(GET_ENUM_ENTRY)
      NUMBER_OF_STATUS_CODES
    };

    std::string statusCodeToString(const StatusCode statusCode);

    //-------------------------------------------------------------------------------------------------

    /** StatusCodeException class
     */
    class StatusCodeException : public std::exception
    {
    public:

      /** Constructor
       */
      StatusCodeException(const StatusCode statusCode);

      /** Constructor
       */
      ~StatusCodeException() throw();

      /** Get status code
       */
      StatusCode getStatusCode() const;

      /** Get status code as a string
       */
      std::string toString() const;

      /** Get back trace at point of exception construction (gcc only)
       */
      const std::string &getBackTrace() const;

    private:

      const StatusCode    m_statusCode;   ///< The status code
      std::string          m_backTrace;    ///< The back trace
    };

    //-------------------------------------------------------------------------------------------------

    // definitions of the quality table
#define DQM_QUALITY_TABLE(d)                   \
    d(NO_QUALITY,          "NO_QUALITY")        \
    d(VERY_BAD_QUALITY,    "VERY_BAD_QUALITY")  \
    d(BAD_QUALITY,         "BAD_QUALITY")       \
    d(NORMAL_QUALITY,      "NORMAL_QUALITY")    \
    d(GOOD_QUALITY,        "GOOD_QUALITY")      \
    d(VERY_GOOD_QUALITY,   "VERY_GOOD_QUALITY")

    /** Quality enum
     */
    enum Quality
    {
      DQM_QUALITY_TABLE(GET_ENUM_ENTRY)
      NUMBER_OF_DQM_QUALITIES
    };

    inline std::string qualityToString(const Quality quality)
    {
      switch (quality)
      {
      DQM_QUALITY_TABLE(GET_NAME_SWITCH)
      default : throw dqm4hep::core::StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
      }
    }

    //-------------------------------------------------------------------------------------------------

    /** Key class
     */
    class Key
    {
    public:
      static const std::string    MODULE_NAME;
      static const std::string    DETECTOR_NAME;
      static const std::string    ME_NAME;
      static const std::string    ME_PATH;
      static const std::string    ME_TYPE;
      static const std::string    ME_DESCRIPTION;

      static const std::string    SYSTEM_NAME;
      static const std::string    NODE_NAME;
      static const std::string    RELEASE;
      static const std::string    VERSION;
      static const std::string    MACHINE;
      static const std::string    HOST_NAME;
    };

    //-------------------------------------------------------------------------------------------------

    /** DQM4HEP class.
     *
     *  Definitions and utilities
     */
    class DQM4HEP
    {
    public:
      /** Convert std::string to a type
       */
      template <class T>
      static bool stringToType(const std::string &s, T &t);

      /** Convert type to std::string
       */
      template <class T>
      static std::string typeToString(const T &t);

      /** Screen splash of DQM4HEP
       */
      static void screenSplash();

      /** Tokenize string with delimiter.
       */
      static void tokenize(const std::string &inputString, StringVector &tokens, const std::string &delimiter = " ");

      /** Return the quality that corresponds to
       *  a value between 0 and 1
       */
      static Quality scaleToQuality(float scale);

      /** Replace all occurrences of a variable in the target string.
       *  Example :
       *
       *  @code
       *  std::string myString("My key is ${key}");
       *  DQMHEP::replace(myString, "key", "A super key");
       *  // myString --> My key is A super key
       *  @endcode
       */
      static std::string &replace(std::string &str, const std::string &variable, const std::string &value);

      /**
       */
      template <typename T>
      static std::string &replace(std::string &str, const std::string &variable, const T &value);

      /** Replace all occurrences of a set of variables in the target string.
       */
      static std::string &replace(std::string &str, const ParameterMap &parameters);
    };

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    template <class T>
    inline bool DQM4HEP::stringToType(const std::string &s, T &t)
    {
      std::istringstream iss(s);
      return !(iss >> t).fail();
    }

    //-------------------------------------------------------------------------------------------------

    template <class T>
    inline std::string DQM4HEP::typeToString(const T &t)
    {
      std::ostringstream oss;

      if ((oss << t).fail())
        throw;

      return oss.str();
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline std::string &DQM4HEP::replace(std::string &str, const std::string &variable, const T &value)
    {
      return DQM4HEP::replace( str, variable, DQM4HEP::typeToString( value ) );
    }

    //-------------------------------------------------------------------------------------------------

    /** Specialization for booleans
     */
    template <>
    inline bool DQM4HEP::stringToType(const std::string &s, bool &t)
    {
      if(s == "on" || s == "1" || s == "true")
        t = true;
      else if(s == "off" || s == "0" || s == "false")
        t = false;
      else
        return false;

      return true;
    }

    //-------------------------------------------------------------------------------------------------

    inline std::string statusCodeToString(const StatusCode statusCode)
    {
      switch (statusCode)
      {
      STATUS_CODE_TABLE(GET_NAME_SWITCH)
      default : throw dqm4hep::core::StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
      }
    }

    //-------------------------------------------------------------------------------------------------

    // definitions of the state table
#define DQM_STATE_TABLE(d)               \
    d(STOPPED_STATE,    "STOPPED_STATE")  \
    d(RUNNING_STATE,    "RUNNING_STATE")  \
    d(PAUSED_STATE,     "PAUSED_STATE")

    /** State enum
     */
    enum State
    {
      DQM_STATE_TABLE(GET_ENUM_ENTRY)
      NUMBER_OF_DQM_STATES
    };

    inline std::string stateToString(const State state)
    {
      switch (state)
      {
      DQM_STATE_TABLE(GET_NAME_SWITCH)
      default : throw dqm4hep::core::StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
      }
    }

    //-------------------------------------------------------------------------------------------------

    // definitions of the monitor element type table
#define DQM_MONITOR_ELEMENT_TYPE_TABLE(d)                                \
    d(NO_ELEMENT_TYPE,                 "NO_ELEMENT_TYPE",                 "")                 \
    d(INT_ELEMENT_TYPE,                "INT_ELEMENT_TYPE",                "int")                \
    d(REAL_ELEMENT_TYPE,               "REAL_ELEMENT_TYPE",               "float")\
    d(SHORT_ELEMENT_TYPE,              "SHORT_ELEMENT_TYPE",              "short")\
    d(STRING_ELEMENT_TYPE,             "STRING_ELEMENT_TYPE",             "string")\
    d(INT_HISTOGRAM_1D_ELEMENT_TYPE,   "INT_HISTOGRAM_1D_ELEMENT_TYPE",   "TH1I")\
    d(REAL_HISTOGRAM_1D_ELEMENT_TYPE,  "REAL_HISTOGRAM_1D_ELEMENT_TYPE",  "TH1F")\
    d(SHORT_HISTOGRAM_1D_ELEMENT_TYPE, "SHORT_HISTOGRAM_1D_ELEMENT_TYPE", "TH1S")\
    d(CHAR_HISTOGRAM_1D_ELEMENT_TYPE,  "CHAR_HISTOGRAM_1D_ELEMENT_TYPE",  "TH1C")\
    d(INT_HISTOGRAM_2D_ELEMENT_TYPE,   "INT_HISTOGRAM_2D_ELEMENT_TYPE",   "TH2I")\
    d(REAL_HISTOGRAM_2D_ELEMENT_TYPE,  "REAL_HISTOGRAM_2D_ELEMENT_TYPE",  "TH2F")\
    d(CHAR_HISTOGRAM_2D_ELEMENT_TYPE,  "CHAR_HISTOGRAM_2D_ELEMENT_TYPE",  "TH2C")\
    d(SHORT_HISTOGRAM_2D_ELEMENT_TYPE, "SHORT_HISTOGRAM_2D_ELEMENT_TYPE", "TH2S")\
    d(INT_HISTOGRAM_3D_ELEMENT_TYPE,   "INT_HISTOGRAM_3D_ELEMENT_TYPE",   "TH3I")\
    d(REAL_HISTOGRAM_3D_ELEMENT_TYPE,  "REAL_HISTOGRAM_3D_ELEMENT_TYPE",  "TH3F")\
    d(PROFILE_1D_ELEMENT_TYPE,         "PROFILE_1D_ELEMENT_TYPE",         "TProfile")\
    d(PROFILE_2D_ELEMENT_TYPE,         "PROFILE_2D_ELEMENT_TYPE",         "TProfile2D")\
    d(USER_DEFINED_ELEMENT_TYPE,       "USER_DEFINED_ELEMENT_TYPE",       "user")

    /** MonitorElementType enum
     */
    enum MonitorElementType
    {
      DQM_MONITOR_ELEMENT_TYPE_TABLE(GET_ENUM_ENTRY_2)
      NUMBER_OF_DQM_MONITOR_ELEMENT_TYPES
    };

    inline std::string monitorElementTypeToString(const MonitorElementType type)
    {
      switch (type)
      {
      DQM_MONITOR_ELEMENT_TYPE_TABLE(GET_NAME_SWITCH_2)
      default : throw dqm4hep::core::StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
      }
    }

    inline MonitorElementType stringToMonitorElementType(const std::string &str)
    {
      DQM_MONITOR_ELEMENT_TYPE_TABLE(GET_STR_COMPARE_1)
	    else return NO_ELEMENT_TYPE;
    }

    inline MonitorElementType stringToMonitorElementRootType(const std::string &str)
    {
      DQM_MONITOR_ELEMENT_TYPE_TABLE(GET_STR_COMPARE_2)
	    else return NO_ELEMENT_TYPE;
    }

    //-------------------------------------------------------------------------------------------------

    // definitions of the reset policy table
#define DQM_RESET_POLICY_TABLE(d)                              \
    d(NO_RESET_POLICY,           "NoReset")             \
    d(END_OF_CYCLE_RESET_POLICY, "EndOfCycle")   \
    d(END_OF_RUN_RESET_POLICY,   "EndOfRun")

    /** ResetPolicy enum
     *
     *  Corresponds to the period at
     *  which the element have to be reset. If eNoReset is chosen
     *  then the user can call reset at any time in the processing.
     *  The default one is eResetAtEndOfCycle.
     */
    enum ResetPolicy
    {
      DQM_RESET_POLICY_TABLE(GET_ENUM_ENTRY)
      NUMBER_OF_DQM_RESET_POLICIES
    };

    inline std::string resetPolicyToString(const ResetPolicy policy)
    {
      switch (policy)
      {
      DQM_RESET_POLICY_TABLE(GET_NAME_SWITCH)
      default : throw dqm4hep::core::StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
      }
    }

    inline ResetPolicy stringToResetPolicy(const std::string &str)
    {
      DQM_RESET_POLICY_TABLE(GET_STR_COMPARE)
	    else return NO_RESET_POLICY;
    }

    //-------------------------------------------------------------------------------------------------

    /** Stats struct
     */
    struct Stats
    {
      time_t          m_timeStamp;
      unsigned int    m_statistics;
    };

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    template <typename T>
    class PositiveValidator
    {
    public:
      static bool validate(const T &t) { return t >= static_cast<T>(0); }
    };

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    class NegativeValidator
    {
    public:
      static bool validate(const T &t) { return t<0; }
    };

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    class NonNullValidator
    {
    public:
      static bool validate(const T &t) { return fabs(static_cast<float>(t) - 0.f) < std::numeric_limits<T>::epsilon(); }
    };

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    class IntervalValidator
    {
    public:
      IntervalValidator(const T &min, const T &max) : m_min(min), m_max(max) {}

      bool validate(const T &t) { return t>m_min && t<m_max; }
    private:
      T     m_min;
      T     m_max;
    };

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    class BiggerThanValidator
    {
    public:
      BiggerThanValidator(const T &compare) : m_compare(compare) {}

      bool operator ()(const T &t) const { return t>m_compare; }
    private:
      T     m_compare;
    };

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    class LessThanValidator
    {
    public:
      LessThanValidator(const T &compare) : m_compare(compare) {}

      bool operator ()(const T &t) const { return t<m_compare; }
    private:
      T     m_compare;
    };

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    /** scoped_mutex class
     */
    class scoped_lock
    {
    public:
      /** Constructor that locks the mutex
       */
      scoped_lock(pthread_mutex_t *pMutex);

      /** Destructor that unlocks the mutex
       */
      ~scoped_lock();
    private:
      pthread_mutex_t      *m_pMutex;
    };

    //-------------------------------------------------------------------------------------------------

    /** RpcResponse struct
     */
    struct RpcResponse
    {
      int      m_ok;
      char     m_pMessage[256];
    };

    //-------------------------------------------------------------------------------------------------

    /** Print the raw buffer
     */
    void printRawBuffer(char *pBuffer, int bufferSize);

  }
}

#endif  //  DQM4HEP_H
