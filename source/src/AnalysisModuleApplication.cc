/// \file DQMAnalysisModuleApplication.cc
/*
 *
 * DQMAnalysisModuleApplication.cc source template automatically generated by a class generator
 * Creation date : dim. nov. 9 2014
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
#include "dqm4hep/AnalysisModuleApplication.h"
#include "dqm4hep/Logging.h"
#include "dqm4hep/Plugin.h"
#include "dqm4hep/PluginManager.h"
#include "dqm4hep/MonitorElement.h"
#include "dqm4hep/RunControl.h"
#include "dqm4hep/Run.h"
#include "dqm4hep/FileHandler.h"
#include "dqm4hep/FileHandlerFactory.h"
#include "dqm4hep/EventStreamer.h"
#include "dqm4hep/AnalysisModule.h"
#include "dqm4hep/Cycle.h"
#include "dqm4hep/Archiver.h"
#include "dqm4hep/Event.h"
#include "dqm4hep/EventClient.h"
#include "dqm4hep/RunControlClient.h"
#include "dqm4hep/DimRunControlClient.h"
#include "dqm4hep/XmlHelper.h"
#include "dqm4hep/tinyxml.h"
#include "dqm4hep/Logging.h"

// -- std headers
#include <fstream>
#include <stdexcept>

// -- dim headers
#include "dis.hxx"

namespace dqm4hep {

  namespace core {

    AnalysisModuleApplication::AnalysisModuleApplication() :
        ModuleApplication(),
        m_pEventClient(NULL),
        m_pRunControlClient(NULL),
        m_pCycle(NULL),
        m_applicationType("AnalysisModule"),
        m_applicationName("UNKNOWN"),
        m_moduleLogStr("[Analysis Module]"),
        m_runNumber(-1)
    {
      m_pArchiver = new Archiver();
    }

    //-------------------------------------------------------------------------------------------------

    AnalysisModuleApplication::~AnalysisModuleApplication()
    {
      delete m_pArchiver;

      if(m_pEventClient)
        delete m_pEventClient;

      if(m_pRunControlClient)
        delete m_pRunControlClient;

      if(m_pCycle)
        delete m_pCycle;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode AnalysisModuleApplication::readSettings(const std::string &settingsFileName)
    {
      if(this->isInitialized())
        return STATUS_CODE_ALREADY_INITIALIZED;

      size_t splitterPosition = settingsFileName.find(":");

      std::string fileHandlerType;
      std::string filePattern;

      if(splitterPosition != std::string::npos)
      {
        fileHandlerType = settingsFileName.substr(0, splitterPosition);
        filePattern = settingsFileName.substr(splitterPosition+1);
      }
      else
      {
        fileHandlerType = "";
        filePattern = settingsFileName;
      }

      LOG4CXX_INFO( dqmMainLogger , "Starting analysis module application with file handler type " << fileHandlerType );

      FileHandlerFactory fileHandlerFactory;
      FileHandler *pFileHandler = fileHandlerFactory.createFileHandler(fileHandlerType);

      if(!pFileHandler)
        return STATUS_CODE_FAILURE;

      StatusCode statusCode = pFileHandler->download(filePattern);

      if(statusCode != STATUS_CODE_SUCCESS)
      {
        delete pFileHandler;
        return statusCode;
      }

      std::string localSettingsFile = pFileHandler->getLocalFileName();
      delete pFileHandler;

      TiXmlDocument xmlDocument(localSettingsFile);

      if (!xmlDocument.LoadFile())
      {
        LOG4CXX_FATAL( dqmMainLogger , "AnalysisModuleApplication::readSettings - Invalid xml file." );
        return STATUS_CODE_FAILURE;
      }

      const TiXmlHandle xmlDocumentHandle(&xmlDocument);

      if( ! m_replacementParameters.empty() )
      {
        TiXmlElement *pXmlElement = xmlDocumentHandle.FirstChildElement().Element();
        RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::replaceAllXmlAttributes(pXmlElement, m_replacementParameters));
      }

      const TiXmlHandle xmlHandle = TiXmlHandle(xmlDocumentHandle.FirstChildElement().Element());

      // configure module
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, configureModule(xmlHandle));

      DimBrowser browser;
      int nServers = browser.getServers();

      std::string moduleServerName = "DQM4HEP/Module/" + this->getModule()->getName();
      m_moduleLogStr = "[" + this->getModule()->getName() + "]";

      if(nServers)
      {
        char *serverName;
        char *node;

        while(browser.getNextServer(serverName, node))
        {
          if(moduleServerName == serverName)
          {
            LOG4CXX_FATAL( dqmMainLogger , m_moduleLogStr << " already registered over the network.\n"
                << "Please, change the module name or stop the other module application with the same name !" );
            return STATUS_CODE_ALREADY_PRESENT;
          }
        }
      }

      DimServer::start( moduleServerName.c_str() );

      // configure remaining part of the application
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->configureCycle(xmlHandle));
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->configureArchiver(xmlHandle));
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->configureNetwork(xmlHandle));

      m_settings.m_settingsFileName = settingsFileName;
      m_settings.print(m_moduleLogStr);

      setInitialized(true);

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    void AnalysisModuleApplication::setReplacementParameters(const ParameterMap &parametersMap)
    {
      m_replacementParameters = parametersMap;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode AnalysisModuleApplication::run()
    {
      if(!this->isInitialized())
        return STATUS_CODE_NOT_INITIALIZED;

      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->startServices());

      // get casted module for easier manipulation
      AnalysisModule *pAnalysisModule = dynamic_cast<AnalysisModule *>(this->getModule());
      std::string moduleName = pAnalysisModule->getName();

      // infinite loop
      while(!this->shouldStopApplication())
      {
        // wait for a start of run
        bool firstMessage = true;

        while(!m_pRunControlClient->isRunning() && !this->shouldStopApplication())
        {
          if(firstMessage)
          {
            LOG4CXX_INFO( dqmMainLogger , m_moduleLogStr << " , waiting for next run ..." );
            firstMessage = false;
          }

          CoreTool::sleep(std::chrono::seconds(1));
        }

        if(this->shouldStopApplication())
          break;

        Run *pRun = m_pRunControlClient->getCurrentRun();

        if(NULL == pRun)
          continue;

        m_runNumber = pRun->getRunNumber();
        LOG4CXX_INFO( dqmMainLogger , m_moduleLogStr << " , Starting new run no " << m_runNumber );

        // open an archive
        if(m_settings.m_shouldOpenArchive)
        {
          std::stringstream archiveFileName;
          std::string directory =
              m_settings.m_archiveDirectory.at(m_settings.m_archiveDirectory.size() - 1) == '/' ?
                  m_settings.m_archiveDirectory : m_settings.m_archiveDirectory + "/";

          archiveFileName << directory
              << "DQMArchive"
              << "_M" << moduleName
              <<  "_I" << pRun->getRunNumber();

          RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pArchiver->open(archiveFileName.str(), "RECREATE"));
          LOG4CXX_INFO( dqmMainLogger , m_moduleLogStr << " , Archive '" << m_pArchiver->getFileName() << "' opened" );
        }

        // start of run !
        RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pAnalysisModule->startOfRun(pRun));

        // start receiving data
        m_pEventClient->setUpdateMode(true);

        // while run has not ended, process cycles
        while( m_pRunControlClient->isRunning() && !this->shouldStopApplication() )
        {
          // process a cycle
          LOG4CXX_INFO( dqmMainLogger , m_moduleLogStr << " , Start of cycle");
          m_pCycle->startCycle();

          while(1)
          {
            if( m_pCycle->isEndOfCycleReached() )
              break;

            if( m_pCycle->isTimeoutReached() )
              break;

            if( this->shouldStopApplication() )
              break;

            EventPtr event;
            m_pEventClient->takeEvent(event);

            if(NULL == event)
              continue;

            try
            {
              StatusCode statusCode = pAnalysisModule->processEvent(event.get());
              m_pCycle->eventProcessed(event.get());
            }
            catch(StatusCodeException &exception)
            {
              LOG4CXX_ERROR( dqmMainLogger , m_moduleLogStr << " , Module::processEvent(evt) returned " << exception.toString() << " !");
            }
            catch(...)
            {
              LOG4CXX_ERROR( dqmMainLogger , m_moduleLogStr << " , Module::processEvent(evt) caught unknown exception !");
            }
          }

          m_pCycle->stopCycle();

          LOG4CXX_INFO( dqmMainLogger , m_moduleLogStr << " , End of cycle reached !");
          LOG4CXX_INFO( dqmMainLogger , m_moduleLogStr << " , Printing statistics :");
          LOG4CXX_INFO( dqmMainLogger , m_moduleLogStr << " , *** N processed events : " << m_pCycle->getNProcessedEvents());
          LOG4CXX_INFO( dqmMainLogger , m_moduleLogStr << " , *** Event rate         : " << m_pCycle->getProcessingRate() << " evts/s");
          LOG4CXX_INFO( dqmMainLogger , m_moduleLogStr << " , *** Processing time    : " << m_pCycle->getTotalCycleTime().count() << " s");

          // archive publication if user asked for
          if(m_settings.m_shouldOpenArchive)
            RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pArchiver->archive(pAnalysisModule));

          //			MonitorElementList monitorElementListToPublish;
          //			RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->getMonitorElementManager()
          //					->getMonitorElementListToPublish(monitorElementListToPublish));

          // TODO find a way to update the run number in monitor elements
          //			// set monitor elements infos
          //			for(MonitorElementList::iterator iter = monitorElementListToPublish.begin(), endIter = monitorElementListToPublish.end() ;
          //					endIter != iter ; ++iter)
          //				(*iter)->setRunNumber(this->getCurrentRunNumber());

          // send monitor elements to collector
          RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->getMonitorElementSender()->sendMonitorElements());

          // reset the monitor elements with end of run reset policy
          RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->getMonitorElementManager()
              ->resetMonitorElements(END_OF_CYCLE_RESET_POLICY));

          // stop current run if run number has changed or run stopped
          if(m_pRunControlClient->getCurrentRunNumber() != this->getCurrentRunNumber())
            break;
        }

        // stop receive data and clear contents
        m_pEventClient->setUpdateMode(false);
        m_pEventClient->clearQueue();

        // fill the run end time
        pRun->setEndTime(CoreTool::now());

        // process the end of run module stuff
        RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pAnalysisModule->endOfRun(pRun));

        // reset the monitor elements that have been reset at end of run
        RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->getMonitorElementManager()
            ->resetMonitorElements(END_OF_RUN_RESET_POLICY));

        // archive the monitor element if archive is opened
        if(m_settings.m_shouldOpenArchive)
          RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pArchiver->close());
      }

      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, stopServices());

      LOG4CXX_INFO( dqmMainLogger , m_moduleLogStr << " , Exiting application !");

      return this->getReturnCode();
    }

    //-------------------------------------------------------------------------------------------------

    Run *AnalysisModuleApplication::getCurrentRun() const
    {
      return m_pRunControlClient->getCurrentRun();
    }

    //-------------------------------------------------------------------------------------------------

    Cycle *AnalysisModuleApplication::getCycle() const
    {
      return m_pCycle;
    }

    //-------------------------------------------------------------------------------------------------

    const std::string &AnalysisModuleApplication::getType() const
    {
      return m_applicationType;
    }

    //-------------------------------------------------------------------------------------------------

    const std::string &AnalysisModuleApplication::getName() const
    {
      return m_applicationName;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode AnalysisModuleApplication::configureNetwork(const TiXmlHandle xmlHandle)
    {
      try
      {
        LOG4CXX_INFO( dqmMainLogger , m_moduleLogStr << " , Configuring network ...");

        //
        // Get all xml attributes needed to configure network options !
        //

        TiXmlElement *const pXmlElement = xmlHandle.FirstChildElement("network").Element();

        if(NULL == pXmlElement)
        {
          LOG4CXX_FATAL( dqmMainLogger , m_moduleLogStr << " , <network> element not found !");
          return STATUS_CODE_NOT_FOUND;
        }

        const TiXmlHandle networkHandle(pXmlElement);

        TiXmlElement *const pRunControlXmlElement = networkHandle.FirstChildElement("runcontrol").Element();
        TiXmlElement *const pEventCollectorXmlElement = networkHandle.FirstChildElement("eventcollector").Element();
        TiXmlElement *const pMonitorElementCollectorXmlElement = networkHandle.FirstChildElement("monitorelementcollector").Element();

        if(NULL == pRunControlXmlElement
            || NULL == pEventCollectorXmlElement
            || NULL == pMonitorElementCollectorXmlElement)
        {
          LOG4CXX_FATAL( dqmMainLogger , m_moduleLogStr << " , incomplete network xml element !");
          return STATUS_CODE_NOT_FOUND;
        }

        THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::getAttribute(pRunControlXmlElement, "name", m_settings.m_runControlName));
        THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::getAttribute(pRunControlXmlElement, "type", m_settings.m_runControlType));
        THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::getAttribute(pEventCollectorXmlElement, "type", m_settings.m_eventClientType));
        THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::getAttribute(pMonitorElementCollectorXmlElement, "name", m_settings.m_monitorElementCollector));

        //
        // Configure network form the extracted xml values
        //

        // monitor element collector name
        THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->getMonitorElementSender()->setCollectorName(m_settings.m_monitorElementCollector));

        // event client
        EventClient *pEventClient = PluginManager::instance()->createPluginClass<EventClient>(m_settings.m_eventClientType);

        if( ! pEventClient )
          throw StatusCodeException(STATUS_CODE_FAILURE);

        THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, pEventClient->readSettings( TiXmlHandle(pEventCollectorXmlElement) ));
        // force update mode to false for the moment
        pEventClient->setUpdateMode(false);
        m_pEventClient = pEventClient;

        // configure run control
        RunControlClient *pRunControlClient = PluginManager::instance()->createPluginClass<RunControlClient>(m_settings.m_runControlType);

        if(NULL == pRunControlClient)
          throw StatusCodeException(STATUS_CODE_NOT_FOUND);

        pRunControlClient->setRunControlName( m_settings.m_runControlName );
        m_pRunControlClient = pRunControlClient;

        LOG4CXX_INFO( dqmMainLogger , m_moduleLogStr << " , Configuring network ... OK");
      }
      catch(const StatusCodeException &exception)
      {
        return exception.getStatusCode();
      }

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode AnalysisModuleApplication::configureCycle(const TiXmlHandle xmlHandle)
    {
      try
      {
        LOG4CXX_INFO( dqmMainLogger , m_moduleLogStr << " , Configuring cycle ...");

        const TiXmlElement *const pXmlElement = xmlHandle.FirstChildElement("cycle").Element();

        // default values
        m_settings.m_cycleType = "TimerCycle";
        m_settings.m_cycleValue = 10; // 10 seconds
        m_settings.m_cycleTimeout = 3; // 3 seconds

        if(pXmlElement)
        {
          THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::getAttribute(pXmlElement, "type", m_settings.m_cycleType));
          THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::getAttribute(pXmlElement, "value", m_settings.m_cycleValue));
          THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::getAttribute(pXmlElement, "timeout", m_settings.m_cycleTimeout));
        }

        // find the cycle plug-in
        LOG4CXX_INFO( dqmMainLogger , m_moduleLogStr << " , Creating cycle of type " << m_settings.m_cycleType <<  "...");
        m_pCycle = PluginManager::instance()->createPluginClass<Cycle>(m_settings.m_cycleType);

        if( ! m_pCycle )
        {
          LOG4CXX_FATAL( dqmMainLogger , m_moduleLogStr << " , Cycle of type " << m_settings.m_cycleType << " not found !");
          throw StatusCodeException(STATUS_CODE_FAILURE);
        }

        LOG4CXX_INFO( dqmMainLogger , m_moduleLogStr << " , Creating cycle of type " << m_settings.m_cycleType <<  "... OK");

        m_pCycle->setCycleValue(m_settings.m_cycleValue);
        m_pCycle->setTimeout(m_settings.m_cycleTimeout);
        m_pCycle->addListener(this);

        LOG4CXX_INFO( dqmMainLogger , m_moduleLogStr << " , Configuring cycle ... OK");
      }
      catch(const StatusCodeException &exception)
      {
        LOG4CXX_FATAL( dqmMainLogger , m_moduleLogStr << " , Couldn't configure cycle !");
        return exception.getStatusCode();
      }

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode AnalysisModuleApplication::configureModule(const TiXmlHandle xmlHandle)
    {
      try
      {
        LOG4CXX_INFO( dqmMainLogger , m_moduleLogStr << " , Configuring module ...");

        TiXmlElement *const pXmlElement = xmlHandle.FirstChildElement("module").Element();

        std::string type;
        std::string name;

        THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::getAttribute(pXmlElement, "type", type));
        THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::getAttribute(pXmlElement, "name", name));

        if(this->getModuleType().empty())
          this->setModuleType(type);

        if(this->getModuleName().empty())
          this->setModuleName(name);

        LOG4CXX_INFO( dqmMainLogger , m_moduleLogStr << " , Querying analysis module to plugin manager ...");

        AnalysisModule *pAnalysisModule = PluginManager::instance()->createPluginClass<AnalysisModule>(this->getModuleType());

        if(!pAnalysisModule)
          throw StatusCodeException(STATUS_CODE_FAILURE);

        this->setModule(pAnalysisModule);

        LOG4CXX_INFO( dqmMainLogger , m_moduleLogStr << " , Querying analysis module to plugin manager ... OK");

        TiXmlHandle moduleHandle(pXmlElement);

        LOG4CXX_INFO( dqmMainLogger , m_moduleLogStr << " , Reading settings of active module '" << this->getModule()->getName() << "' ...");
        THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->getModule()->readSettings(moduleHandle));
        LOG4CXX_INFO( dqmMainLogger , m_moduleLogStr << " , Reading settings of active module '" << this->getModule()->getName() << "' ... OK");

        LOG4CXX_INFO( dqmMainLogger , m_moduleLogStr << " , Initializing active module '" << this->getModule()->getName() << "' ...");
        THROW_RESULT_IF( STATUS_CODE_SUCCESS, !=, this->getModule()->initModule() );
        LOG4CXX_INFO( dqmMainLogger , m_moduleLogStr << " , Initializing active module '" << this->getModule()->getName() << "' ... OK");

        m_applicationName = this->getModule()->getName();

        LOG4CXX_INFO( dqmMainLogger , m_moduleLogStr << " , Configuring module ... OK");
      }
      catch(const StatusCodeException &exception)
      {
        return exception.getStatusCode();
      }

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode AnalysisModuleApplication::configureArchiver(const TiXmlHandle xmlHandle)
    {
      try
      {
        LOG4CXX_INFO( dqmMainLogger , m_moduleLogStr << " , Configuring archiver ...");

        const TiXmlElement *const pXmlElement = xmlHandle.FirstChildElement("archiver").Element();

        THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::getAttribute(pXmlElement, "open", m_settings.m_shouldOpenArchive));
        THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::getAttribute(pXmlElement, "directory", m_settings.m_archiveDirectory));

        LOG4CXX_INFO( dqmMainLogger , m_moduleLogStr << " , Configuring archiver ... OK");
      }
      catch(const StatusCodeException &exception)
      {
        return exception.getStatusCode();
      }

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode AnalysisModuleApplication::startServices()
    {
      // start clients
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pEventClient->connectToService());
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pRunControlClient->connectToService());
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->getMonitorElementSender()->connectToService());

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode AnalysisModuleApplication::stopServices()
    {
      // start clients
      if(m_pEventClient && m_pEventClient->isConnectedToService())
        RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pEventClient->connectToService());

      if(m_pRunControlClient && m_pRunControlClient->isConnectedToService())
        RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pRunControlClient->connectToService());

      if(this->getMonitorElementSender() && this->getMonitorElementSender()->isConnectedToService())
        RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->getMonitorElementSender()->connectToService());

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    int AnalysisModuleApplication::getCurrentRunNumber() const
    {
      return m_runNumber;
    }

    //-------------------------------------------------------------------------------------------------

    void AnalysisModuleApplication::onCycleStarted(const Cycle *const /*pCycle*/)
    {
      THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->getModule()->startOfCycle());
    }

    //-------------------------------------------------------------------------------------------------

    void AnalysisModuleApplication::onCycleStopped(const Cycle *const /*pCycle*/)
    {
      THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->getModule()->endOfCycle());
    }

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    AnalysisModuleApplication::Settings::Settings() :
        m_shouldOpenArchive(false),
        m_runControlType("DimRunControlClient"),
        m_runControlName("DEFAULT"),
        m_eventClientType("DimEventClient"),
        m_monitorElementCollector("DEFAULT"),
        m_cycleType("TimeCycle"),
        m_cycleValue(30.f),
        m_cycleTimeout(5),
        m_archiveDirectory("/tmp")
    {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    void AnalysisModuleApplication::Settings::print(const std::string &moduleLogStr)
    {
      std::string openArchive = m_shouldOpenArchive ? "yes" : "no";
      LOG4CXX_INFO( dqmMainLogger , moduleLogStr << " Should open archive : " << openArchive);
      LOG4CXX_INFO( dqmMainLogger , moduleLogStr << " Run control type : " << m_runControlType);
      LOG4CXX_INFO( dqmMainLogger , moduleLogStr << " Run control name : " << m_runControlName);
      LOG4CXX_INFO( dqmMainLogger , moduleLogStr << " Event client type : " << m_eventClientType);
      LOG4CXX_INFO( dqmMainLogger , moduleLogStr << " Cycle type : " << m_cycleType);
      LOG4CXX_INFO( dqmMainLogger , moduleLogStr << " Cycle value : " << m_cycleValue);
      LOG4CXX_INFO( dqmMainLogger , moduleLogStr << " Cycle timeout : " << m_cycleTimeout);
      LOG4CXX_INFO( dqmMainLogger , moduleLogStr << " Archive directory : " << m_archiveDirectory);
      LOG4CXX_INFO( dqmMainLogger , moduleLogStr << " Monitor element collector name : " << m_monitorElementCollector);
    }

  }

} 
