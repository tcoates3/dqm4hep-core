/// \file DQMMonitorElementCollectorApplication.cc
/*
 *
 * DQMMonitorElementCollectorApplication.cc source template automatically generated by a class generator
 * Creation date : jeu. juin 18 2015
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

// -- dqm4hep headers
#include "dqm4hep/DQMMonitorElementCollectorApplication.h"
#include "dqm4hep/DQMMonitorElementCollector.h"
#include "dqm4hep/DQMLogging.h"
#include "dqm4hep/DQMCoreTool.h"

// -- dim headers
#include "dis.hxx"

namespace dqm4hep {

  namespace core {

    MonitorElementCollectorApplication::MonitorElementCollectorApplication() :
		    m_applicationState(STOPPED_STATE),
		    m_shouldExit(false),
		    m_type("MonitorElementCollector")
    {
      m_pMonitorElementCollector = new MonitorElementCollector();
    }

    //-------------------------------------------------------------------------------------------------

    MonitorElementCollectorApplication::~MonitorElementCollectorApplication()
    {
      if(m_pMonitorElementCollector->isRunning())
        m_pMonitorElementCollector->stop();

      delete m_pMonitorElementCollector;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode MonitorElementCollectorApplication::run()
    {
      if(m_applicationState == RUNNING_STATE)
        return STATUS_CODE_FAILURE;

      m_applicationState = RUNNING_STATE;

      DimServer::addClientExitHandler(m_pMonitorElementCollector);
      DimServer::start( ("DQM4HEP/" + getType() + "/" + getName()).c_str() );

      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pMonitorElementCollector->start());

      while( ! m_shouldExit )
        CoreTool::sleep(std::chrono::seconds(1));

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode MonitorElementCollectorApplication::exit( int returnCode )
    {
      if(m_applicationState == STOPPED_STATE)
        return STATUS_CODE_FAILURE;

      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pMonitorElementCollector->stop());

      m_shouldExit = true;
      m_applicationState = STOPPED_STATE;

      StatusCode returnStatusCode;

      if(returnCode >= 0 && returnCode < NUMBER_OF_STATUS_CODES)
        returnStatusCode = static_cast<StatusCode>(returnCode);
      else
        returnStatusCode = STATUS_CODE_FAILURE;

      return returnStatusCode;
    }

    //-------------------------------------------------------------------------------------------------

    const std::string &MonitorElementCollectorApplication::getName() const
    {
      return m_pMonitorElementCollector->getCollectorName();
    }

    //-------------------------------------------------------------------------------------------------

    const std::string &MonitorElementCollectorApplication::getType() const
    {
      return m_type;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode MonitorElementCollectorApplication::setCollectorName(const std::string &collectorName)
    {
      return m_pMonitorElementCollector->setCollectorName(collectorName);
    }

  }

}

