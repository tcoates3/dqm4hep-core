/// \file DQMShmProxyApplication.cc
/*
 *
 * DQMShmProxyApplication.cc source template automatically generated by a class generator
 * Creation date : mer. avr. 13 2016
 *
 * This file is part of DQMEvb libraries.
 * 
 * DQMEvb is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * based upon these libraries are permitted. Any copy of these libraries
 * must include this copyright notice.
 * 
 * DQMEvb is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with DQMEvb.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * @author Remi Ete
 * @copyright CNRS , IPNL
 */


#include "dqm4hep/evb/DQMShmProxyApplication.h"

// -- dqm4hep headers
#include "dqm4hep/DQMXmlHelper.h"
#include "dqm4hep/DQMPluginManager.h"
#include "dqm4hep/DQMCoreTool.h"
#include "dqm4hep/DQMEvent.h"
#include "dqm4hep/DQMEventStreamer.h"

namespace dqm4hep {

  namespace core {

    ShmProxyApplication::ShmProxyApplication() :
        m_pEventStreamer(NULL),
        m_pRunControlClient(NULL),
        m_pShmDriver(NULL),
        m_type("ShmProxyApplication"),
        m_name("UnknownShmProxy"),
        m_isInitialized(false),
        m_shouldStop(false),
        m_returnCode(STATUS_CODE_SUCCESS)
    {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    ShmProxyApplication::~ShmProxyApplication()
    {
      if( m_pRunControlClient )
        delete m_pRunControlClient;

      if( m_pShmDriver )
        delete m_pShmDriver;

      if( m_pEventStreamer )
        delete m_pEventStreamer;

      for( auto iter = m_eventClientList.begin(), endIter = m_eventClientList.end() ; endIter != iter ; ++iter )
        delete *iter;

      m_eventClientList.clear();
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode ShmProxyApplication::run()
    {
      if( ! this->isInitialized() )
        return STATUS_CODE_NOT_INITIALIZED;

      // Start DIM server
      DimServer::start( this->getName().c_str() );

      // connect to run control
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pRunControlClient->connectToService());

      while( ! this->shouldStopApplication() )
        CoreTool::sleep(std::chrono::milliseconds(100));

      LOG4CXX_INFO( dqmMainLogger , this->getName() << " , Exiting application !");

      return m_returnCode;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode ShmProxyApplication::readSettings(const std::string &settingsFile)
    {
      if( this->isInitialized() )
        return STATUS_CODE_ALREADY_INITIALIZED;

      TiXmlDocument xmlDocument(settingsFile);

      if ( ! xmlDocument.LoadFile() )
      {
        LOG4CXX_FATAL( dqmMainLogger , "ShmProxyApplication::readSettings - Invalid xml file." );
        return STATUS_CODE_FAILURE;
      }

      const TiXmlHandle xmlDocumentHandle(&xmlDocument);

      if( ! m_replacementParameters.empty() )
      {
        TiXmlElement *pXmlElement = xmlDocumentHandle.FirstChildElement().Element();
        RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::replaceAllXmlAttributes(pXmlElement, m_replacementParameters));
      }

      const TiXmlHandle xmlHandle = TiXmlHandle(xmlDocumentHandle.FirstChildElement().Element());

      TiXmlElement *pGlobalSettings = xmlHandle.FirstChild("global").Element();
      TiXmlElement *pProcessorsSettings = xmlHandle.FirstChild("evbprocessors").Element();

      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->configureGlobal(pGlobalSettings));
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->configureProcessors(pProcessorsSettings));

      this->setInitialized(true);

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode ShmProxyApplication::exit(int returnCode)
    {
      LOG4CXX_INFO( dqmMainLogger , "Exiting shm proxy application ..." );

      if( ! this->isInitialized() )
      {
        return STATUS_CODE_NOT_INITIALIZED;
      }
      if( returnCode >= 0 && returnCode < NUMBER_OF_STATUS_CODES )
      {
        m_returnCode = static_cast< StatusCode >( returnCode );
      }
      else
        m_returnCode = STATUS_CODE_FAILURE;

      this->setStopApplication(true);

      LOG4CXX_INFO( dqmMainLogger , "Exiting shm proxy application ... OK" );

      return m_returnCode;
    }

    //-------------------------------------------------------------------------------------------------

    const std::string &ShmProxyApplication::getType() const
    {
      return m_type;
    }

    //-------------------------------------------------------------------------------------------------

    void ShmProxyApplication::setName(const std::string &name)
    {
      m_name = name;
    }

    //-------------------------------------------------------------------------------------------------

    const std::string &ShmProxyApplication::getName() const
    {
      return m_name;
    }

    //-------------------------------------------------------------------------------------------------

    bool ShmProxyApplication::shouldStopApplication() const
    {
      return m_shouldStop;
    }

    //-------------------------------------------------------------------------------------------------

    void ShmProxyApplication::setStopApplication(bool stop)
    {
      m_shouldStop = stop;
    }

    //-------------------------------------------------------------------------------------------------

    void ShmProxyApplication::setInitialized(bool initialized)
    {
      m_isInitialized = initialized;
    }

    //-------------------------------------------------------------------------------------------------

    bool ShmProxyApplication::isInitialized() const
    {
      return m_isInitialized;
    }

    //-------------------------------------------------------------------------------------------------

    void ShmProxyApplication::onStartOfRun(Run *const pRun)
    {
      m_pShmDriver->start(pRun->getRunNumber());

      for(auto iter = m_processorList.begin(), endIter = m_processorList.end() ;
          endIter != iter ; ++iter)
      {
        THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, iter->second->startOfRun(pRun));
      }
    }

    //-------------------------------------------------------------------------------------------------

    void ShmProxyApplication::onEndOfRun(const Run *const pRun)
    {
      m_pShmDriver->stop();

      for(auto iter = m_processorList.begin(), endIter = m_processorList.end() ;
          endIter != iter ; ++iter)
      {
        THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, iter->second->endOfRun(pRun));
      }
    }

    //-------------------------------------------------------------------------------------------------

    void ShmProxyApplication::setReplacementParameters( const ParameterMap &parameters )
    {
      m_replacementParameters = parameters;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode ShmProxyApplication::configureGlobal(TiXmlElement *pGlobalSettings)
    {
      if( ! pGlobalSettings )
      {
        LOG4CXX_ERROR( dqmMainLogger , "No <global> xml element found in settings file !" );
        return STATUS_CODE_NOT_FOUND;
      }

      TiXmlHandle globalHandle(pGlobalSettings);

      // read shm driver settings
      TiXmlElement *pShmSettings = globalHandle.FirstChild("shmdriver").Element();
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->configureShmDriver(pShmSettings));


      // read run control settings
      TiXmlElement *pRunControlSettings = globalHandle.FirstChild("runcontrol").Element();
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->configureRunControlClient(pRunControlSettings));


      // parse event clients
      TiXmlElement *pEventStreamerSettings = globalHandle.FirstChild("eventstreamer").Element();

      if( ! pEventStreamerSettings )
      {
        LOG4CXX_ERROR( dqmMainLogger , "No <eventstreamer> xml element in global section !" );
        return STATUS_CODE_NOT_FOUND;
      }

      std::string eventStreamerPlugin;
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::getAttribute(pEventStreamerSettings, "plugin", eventStreamerPlugin));

      m_pEventStreamer = PluginManager::instance()->createPluginClass<EventStreamer>(eventStreamerPlugin);

      if( ! m_pEventStreamer )
      {
        LOG4CXX_ERROR( dqmMainLogger , "Event streamer plugin of type '" << eventStreamerPlugin << "' not found !" );
        return STATUS_CODE_NOT_FOUND;
      }

      // parse event clients
      TiXmlElement *pEventClientListSettings = globalHandle.FirstChild("eventclientlist").Element();

      if( ! pEventClientListSettings )
      {
        LOG4CXX_ERROR( dqmMainLogger , "No <eventclientlist> xml element in global section !" );
        return STATUS_CODE_NOT_FOUND;
      }

      TiXmlHandle eventClientListHandle( pEventClientListSettings );

      for (TiXmlElement *pXmlElement = eventClientListHandle.FirstChild("eventclient").Element(); NULL != pXmlElement;
          pXmlElement = pXmlElement->NextSiblingElement("eventclient"))
      {
        std::string eventClientPlugin;
        RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::getAttribute(pXmlElement, "plugin", eventClientPlugin));

        EventClient *pEventClient = PluginManager::instance()->createPluginClass<EventClient>(eventClientPlugin);

        if( ! pEventClient )
        {
          LOG4CXX_ERROR( dqmMainLogger , "Event client plugin of type '" << eventClientPlugin << "' not found !" );
          return STATUS_CODE_NOT_FOUND;
        }

        RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pEventClient->readSettings(TiXmlHandle(pXmlElement)));

        pEventClient->setEventStreamer( m_pEventStreamer , false );

        m_eventClientList.push_back( pEventClient );
      }

      // parse evb processors to run
      TiXmlElement *pProcessorsSettings = globalHandle.FirstChild("evbprocessors").Element();

      if( ! pProcessorsSettings )
      {
        LOG4CXX_ERROR( dqmMainLogger , "No <evbprocessors> xml element in global section!" );
        return STATUS_CODE_NOT_FOUND;
      }

      TiXmlHandle processorsHandle(pProcessorsSettings);

      for (TiXmlElement *pXmlElement = processorsHandle.FirstChild("evbprocessor").Element(); NULL != pXmlElement;
          pXmlElement = pXmlElement->NextSiblingElement("evbprocessor"))
      {
        std::string processorName;
        RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::getAttribute(pXmlElement, "name", processorName));

        auto procIter = find_if( m_processorList.begin() , m_processorList.end() , [&] (const ProcessorList::value_type &value) {
          return value.first == processorName;
        });

        if( procIter != m_processorList.end() )
        {
          LOG4CXX_ERROR( dqmMainLogger , "Duplicated evb processor name '" << processorName << "' in global section !" );
          return STATUS_CODE_ALREADY_PRESENT;
        }

        ProcessorList::value_type entry(processorName , 0);

        // put a placeholder for future processor plugin
        m_processorList.push_back( entry );
      }

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode ShmProxyApplication::configureProcessors(TiXmlElement *pProcessorsSettings)
    {
      if( ! pProcessorsSettings )
      {
        LOG4CXX_ERROR( dqmMainLogger , "No <evbprocessors> xml element found in settings file !" );
        return STATUS_CODE_NOT_FOUND;
      }

      TiXmlHandle processorsHandle(pProcessorsSettings);

      for (TiXmlElement *pXmlElement = processorsHandle.FirstChild("evbprocessor").Element(); NULL != pXmlElement;
          pXmlElement = pXmlElement->NextSiblingElement("evbprocessor"))
      {
        std::string processorName, plugin;
        RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::getAttribute(pXmlElement, "plugin", plugin));
        RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::getAttribute(pXmlElement, "name", processorName));

        auto procIter = find_if( m_processorList.begin() , m_processorList.end() , [&] (const ProcessorList::value_type &value) {
          return value.first == processorName;
        });

        if( procIter == m_processorList.end() )
        {
          LOG4CXX_WARN( dqmMainLogger , "Evb processor name '" << processorName << "' set to be configured but not run !" );
          LOG4CXX_WARN( dqmMainLogger , "Skipping this processor ..." );
          continue;
        }

        if( NULL != procIter->second )
        {
          LOG4CXX_WARN( dqmMainLogger , "Evb processor name '" << processorName << "' already created and configured !" );
          LOG4CXX_WARN( dqmMainLogger , "Skipping secondary processor instance(s) ..." );
          continue;
        }

        ShmProcessor *pProcessor = PluginManager::instance()->createPluginClass<ShmProcessor>(plugin);

        if( ! pProcessor )
        {
          LOG4CXX_ERROR( dqmMainLogger , "Evb processor plugin '" << plugin << "' not registered in plugin manager !" )
    		    return STATUS_CODE_NOT_FOUND;
        }

        procIter->second = pProcessor;

        // read processor user settings
        RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pProcessor->readSettings(TiXmlHandle(pXmlElement)));
      }

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode ShmProxyApplication::configureShmDriver(TiXmlElement *pShmSettings)
    {
      if( ! pShmSettings )
      {
        LOG4CXX_ERROR( dqmMainLogger , "No <shmdriver> xml element in global section !" );
        return STATUS_CODE_NOT_FOUND;
      }

      TiXmlHandle shmHandle(pShmSettings);

      std::string shmDirectory = "/dev/shm/levbdim";
      RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::getAttribute(pShmSettings, "directory", shmDirectory));

      // configure shm driver
      m_pShmDriver = new levbdim::shmdriver(shmDirectory);
      m_pShmDriver->createDirectories();
      m_pShmDriver->cleanShm();
      m_pShmDriver->registerProcessor(this);

      TiXmlElement *pSourcesElement = shmHandle.FirstChild("datasources").Element();

      if( ! pSourcesElement )
      {
        LOG4CXX_ERROR( dqmMainLogger , "No <datasources> xml element in shmdriver section !" );
        return STATUS_CODE_NOT_FOUND;
      }

      TiXmlHandle sourcesHandle(pSourcesElement);

      for (TiXmlElement *pSourceElement = sourcesHandle.FirstChild("source").Element(); NULL != pSourceElement;
          pSourceElement = pSourceElement->NextSiblingElement("source"))
      {
        uint32_t detector, id;
        RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::getAttribute(pSourceElement, "detector", detector));
        RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::getAttribute(pSourceElement, "id", id));

        m_pShmDriver->registerDataSource(detector, id);
      }

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode ShmProxyApplication::configureRunControlClient(TiXmlElement *pRunControlSettings)
    {
      if( ! pRunControlSettings )
      {
        LOG4CXX_ERROR( dqmMainLogger , "No <runcontrol> xml element in global section !" );
        return STATUS_CODE_NOT_FOUND;
      }

      std::string plugin, runControlName;
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::getAttribute(pRunControlSettings, "plugin", plugin));
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::getAttribute(pRunControlSettings, "name", runControlName));

      m_pRunControlClient = PluginManager::instance()->createPluginClass<RunControlClient>(plugin);

      if( ! m_pRunControlClient )
      {
        LOG4CXX_ERROR( dqmMainLogger , "Run control client interface plugin '" << plugin << "' not found !" );
        return STATUS_CODE_NOT_FOUND;
      }

      m_pRunControlClient->setRunControlName(runControlName);
      m_pRunControlClient->addListener(this);

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    void ShmProxyApplication::processEvent(uint32_t key, std::vector<levbdim::buffer*> bufferList)
    {
      Event *pEvent = m_pEventStreamer->createEvent();

      try
      {
        // call evb processors to fill the event
        for(auto iter = m_processorList.begin(), endIter = m_processorList.end() ;
            endIter != iter ; ++iter)
        {
          THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, iter->second->processEvent(pEvent, key, bufferList));
        }

        for( auto iter = m_eventClientList.begin(), endIter = m_eventClientList.end() ; endIter != iter ; ++iter )
        {
          THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, (*iter)->sendEvent(pEvent));
        }
      }
      catch(StatusCodeException &exception)
      {
        LOG4CXX_ERROR( dqmMainLogger , "Caught status code exception : " << exception.toString() );
        delete pEvent;
        throw exception;
      }
      catch(...)
      {
        LOG4CXX_ERROR( dqmMainLogger , "Caught unknown exception !");
        delete pEvent;
        throw StatusCodeException(STATUS_CODE_FAILURE);
      }

      delete pEvent;
    }

  }

} 

