/// \file DQMRun.h
/*
 *
 * DQMRun.h header template automatically generated by a class generator
 * Creation date : dim. mars 29 2015
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


#ifndef DQM4HEP_RUN_H
#define DQM4HEP_RUN_H

// -- dqm4hep headers
#include <dqm4hep/StatusCodes.h>
#include <dqm4hep/Internal.h>

// -- xdrstream headers
#include <xdrstream/xdrstream.h>

// -- json headers
#include <json/json.h>

// -- std headers
#include <ctime>
#include <string>

namespace dqm4hep {

  namespace core {

    /** Run class
     */
    class Run : public xdrstream::Streamable
    {
    public:
      /** Constructor with run number (optional)
       */
      Run(int runNumber = 0, const std::string &description = "", const std::string &detectorName = "");

      /** Destructor
       */
      ~Run();

      /** Get the run number
       */
      int getRunNumber() const;

      /** Get the start time
       */
      TimePoint getStartTime() const;

      /** Get the end time
       */
      TimePoint getEndTime() const;

      /** Get the run description
       */
      const std::string &getDescription() const;

      /** Get the detector name related to this run
       */
      const std::string &getDetectorName() const;

      /** Set the run number
       */
      void setRunNumber(int runNumber);

      /** Set the start time
       */
      void setStartTime(const TimePoint &startTime);

      /** Set the end time
       */
      void setEndTime(const TimePoint &endTime);

      /** Set the run description
       */
      void setDescription(const std::string &description);

      /** Set the detector name related to this run
       */
      void setDetectorName(const std::string &detectorName);

      /** Set a parameter.
       *  The template value is converted into string
       */
      template <typename T>
      void setParameter(const std::string &key, const T &value);

      /** Get a parameter.
       *  The value is converted into asked type.
       *  If the parameter is not found, the value remains unchanged
       */
      template <typename T>
      void getParameter(const std::string &key, T &value) const;

      /** Get the number of parameters
       */
      unsigned int getNParameters() const;

      /** Get the parameters keys
       */
      StringVector getParameterKeys() const;

      /** Reset the run
       */
       void reset();

      xdrstream::Status stream(xdrstream::StreamingMode mode, xdrstream::IODevice *pDevice,
          xdrstream::xdr_version_t version = 0);

      /** Convert run to json
       */
      void toJson(Json::Value &value) const;

      /** Read run info from json
       */
      void fromJson(const Json::Value &value);

    protected:
      int                                    m_runNumber;
      TimePoint                              m_startTime;
      TimePoint                              m_endTime;
      std::string                            m_detectorName;
      std::string                            m_description;
      std::map<std::string, std::string>     m_parametersMap;

      friend std::ostream &operator <<(std::ostream &, const Run &);
    };

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    inline std::ostream &operator <<(std::ostream &out, const Run &run)
    {
      out << "Run :\n"
          "  -> run number : " << run.getRunNumber() << "\n"
          "  -> detector : " << run.getDetectorName() << "\n"
          "  -> description : " << run.getDescription() << "\n"
          "  -> parameters : \n";

      for(auto iter = run.m_parametersMap.begin(), endIter = run.m_parametersMap.end() ;
          endIter != iter ; ++iter)
        out << "   * " << iter->first << " = " << iter->second << "\n";

      return out;
    }

    //-------------------------------------------------------------------------------------------------

    inline int Run::getRunNumber() const
    {
      return m_runNumber;
    }

    //-------------------------------------------------------------------------------------------------

    inline TimePoint Run::getStartTime() const
    {
      return m_startTime;
    }

    //-------------------------------------------------------------------------------------------------

    inline TimePoint Run::getEndTime() const
    {
      return m_endTime;
    }

    //-------------------------------------------------------------------------------------------------

    inline const std::string &Run::getDescription() const
    {
      return m_description;
    }

    //-------------------------------------------------------------------------------------------------

    inline const std::string &Run::getDetectorName() const
    {
      return m_detectorName;
    }

    //-------------------------------------------------------------------------------------------------

    inline void Run::setRunNumber(int runNumber)
    {
      m_runNumber = runNumber;
    }

    //-------------------------------------------------------------------------------------------------

    inline void Run::setStartTime(const TimePoint &startTime)
    {
      m_startTime = startTime;
    }

    //-------------------------------------------------------------------------------------------------

    inline void Run::setEndTime(const TimePoint &endTime)
    {
      m_endTime = endTime;
    }

    //-------------------------------------------------------------------------------------------------

    inline void Run::setDescription(const std::string &description)
    {
      m_description = description;
    }

    //-------------------------------------------------------------------------------------------------

    inline void Run::setDetectorName(const std::string &detectorName)
    {
      m_detectorName = detectorName;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline void Run::setParameter(const std::string &key, const T &value)
    {
      m_parametersMap[key] = dqm4hep::core::typeToString<T>(value);
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline void Run::getParameter(const std::string &key, T &value) const
    {
      std::map<std::string, std::string>::const_iterator findIter = m_parametersMap.find(key);

      if(m_parametersMap.end() == findIter)
        return;

      dqm4hep::core::stringToType(findIter->second, value);
    }

    //-------------------------------------------------------------------------------------------------

    template <>
    inline void Run::setParameter(const std::string &key, const std::string &value)
    {
      m_parametersMap[key] = value;
    }

    //-------------------------------------------------------------------------------------------------

    template <>
    inline void Run::getParameter(const std::string &key, std::string &value) const
    {
      std::map<std::string, std::string>::const_iterator findIter = m_parametersMap.find(key);

      if(m_parametersMap.end() == findIter)
        return;

      value = findIter->second;
    }

    //-------------------------------------------------------------------------------------------------

    inline unsigned int Run::getNParameters() const
    {
      return m_parametersMap.size();
    }

  }

}

#endif  //  DQM4HEP_RUN_H
