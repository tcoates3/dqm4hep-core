/// \file DQMDimEventClient.cc
/*
 *
 * DQMDimEventClient.cc source template automatically generated by a class generator
 * Creation date : mar. sept. 8 2015
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
#include "dqm4hep/DQMDimEventClient.h"
#include "dqm4hep/DQMEventStreamer.h"
#include "dqm4hep/DQMXmlHelper.h"
#include "dqm4hep/DQMPluginManager.h"
#include "dqm4hep/DQMPlugin.h"

namespace dqm4hep {

  namespace core {

    DimEventRpcInfo::DimEventRpcInfo(DimEventClient *pEventClient) :
	    DimRpcInfo((char*) ("DQM4HEP/EventCollector/" + pEventClient->getCollectorName() + "/EVENT_RAW_BUFFER_UPDATE").c_str(), (void*) NULL, 0),
	    m_pEventClient(pEventClient)
    {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    void DimEventRpcInfo::rpcInfoHandler()
    {
      m_pEventClient->eventReception( (dqm_char *) getData(), (dqm_uint) getSize() );
    }

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    // plug the client class in the framework
    DQM_PLUGIN_DECL( DimEventClient , "DimEventClient" )

    DimEventClient::DimEventClient() :
    m_isConnected(false),
    m_pDimEventRpcInfo(NULL),
    m_updateMode(false),
    m_serverClientId(0),
    m_pReadBuffer(0),
    m_pWriteBuffer(0)
    {
      pthread_mutex_init(&m_mutex, NULL);

      m_pWriteBuffer = new xdrstream::BufferDevice(5*1024*1024);
    }

    //-------------------------------------------------------------------------------------------------

    DimEventClient::~DimEventClient()
    {
      if( this->isConnectedToService() )
        this->disconnectFromService();

      pthread_mutex_destroy(&m_mutex);

      if(m_pReadBuffer)
        delete m_pReadBuffer;

      if(m_pWriteBuffer)
        delete m_pWriteBuffer;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode DimEventClient::performServiceConnection()
    {
      if(isConnectedToService())
        return STATUS_CODE_SUCCESS;

      DimClient::setExitHandler( ("DQM4HEP/EventCollector/" + this->getCollectorName() ).c_str() );

      m_pDimEventRpcInfo = new DimEventRpcInfo(this);

      m_pClientIdInfo    = new DimUpdatedInfo(("DQM4HEP/EventCollector/" + this->getCollectorName() + "/CLIENT_REGISTERED").c_str(), static_cast<int>(0), this);
      m_pServerStateInfo = new DimUpdatedInfo(("DQM4HEP/EventCollector/" + this->getCollectorName() + "/SERVER_STATE").c_str(), static_cast<int>(0), this);
      m_pEventUpdateInfo = new DimUpdatedInfo(("DQM4HEP/EventCollector/" + this->getCollectorName() + "/EVENT_RAW_UPDATE").c_str(), (void*) NULL, 0, this);

      m_isConnected = true;

      DimCurrentInfo serverStateInfo(("DQM4HEP/EventCollector/" + this->getCollectorName() + "/SERVER_STATE").c_str(), static_cast<int>(0));
      int serverRunning = serverStateInfo.getInt();

      if(!serverRunning)
      {
        LOG4CXX_WARN( dqmMainLogger , "Server collector application not running yet" );
        return STATUS_CODE_SUCCESS;
      }

      // send command to register the client on the server
      // the server is expected to update m_pClientIdInfo info with the client id
      LOG4CXX_INFO( dqmMainLogger , "Registering client into server collector application !" );
      DimClient::sendCommandNB(("DQM4HEP/EventCollector/" + this->getCollectorName() + "/CLIENT_REGISTRATION").c_str(), 1);

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode DimEventClient::performServiceDisconnection()
    {
      if(!isConnectedToService())
        return STATUS_CODE_SUCCESS;

      delete m_pDimEventRpcInfo;
      delete m_pClientIdInfo;
      delete m_pServerStateInfo;
      delete m_pEventUpdateInfo;

      m_isConnected = false;

      // un-register client on the server
      if(m_serverClientId > 0)
      {
        DimClient::sendCommandNB(("DQM4HEP/EventCollector/" + this->getCollectorName() + "/CLIENT_REGISTRATION").c_str(), 0);
      }

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    bool DimEventClient::isConnectedToService() const
    {
      return m_isConnected;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode DimEventClient::sendEvent(const Event *const pEvent)
    {
      scoped_lock( & this->m_mutex);

      if( NULL == this->getEventStreamer() )
        return STATUS_CODE_NOT_INITIALIZED;

      if(NULL == pEvent)
        return STATUS_CODE_INVALID_PARAMETER;

      m_pWriteBuffer->reset();
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->getEventStreamer()->write(pEvent, m_pWriteBuffer));

      char *pBuffer = m_pWriteBuffer->getBuffer();
      unsigned int bufferSize = m_pWriteBuffer->getPosition();

      if(NULL == pBuffer || 0 == bufferSize)
        return STATUS_CODE_FAILURE;

      std::string commandName = "DQM4HEP/EventCollector/" + this->getCollectorName() + "/COLLECT_RAW_EVENT";
      DimClient::sendCommandNB((char*) commandName.c_str(), (void *) pBuffer, bufferSize);

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    void DimEventClient::setSubEventIdentifier(const std::string &identifier)
    {
      EventClient::setSubEventIdentifier(identifier);

      if( ! this->isConnectedToService() )
        return;

      std::string subEventIdentifierCommandName = "DQM4HEP/EventCollector/" + this->getCollectorName() + "/SUB_EVENT_IDENTIFIER";
      DimClient::sendCommandNB((char*) subEventIdentifierCommandName.c_str(), (char *) identifier.c_str());
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode DimEventClient::queryEvent(Event *&pEvent, int timeout)
    {
      scoped_lock( & this->m_mutex);

      if( ! this->isConnectedToService() )
        return STATUS_CODE_NOT_INITIALIZED;

      if( NULL == this->getEventStreamer() )
        return STATUS_CODE_NOT_INITIALIZED;

      // check for valid parameters
      if(timeout <= 0)
        return STATUS_CODE_INVALID_PARAMETER;

      // rpc service name
      std::string serviceName = "DQM4HEP/EventCollector/" + this->getCollectorName() + "/EVENT_RAW_REQUEST";
      pEvent = NULL;

      // declare the rpc service info
      DimRpcInfo eventDimRpcInfo((char*) serviceName.c_str(), timeout, (void*) NULL, 0);

      // send the query
      eventDimRpcInfo.setData((char*) this->getSubEventIdentifier().c_str());

      // receive the query result
      char *pEventRawBuffer = static_cast<char*>(eventDimRpcInfo.getData());
      int bufferSize = eventDimRpcInfo.getSize();

      // check for message validity
      if(NULL == pEventRawBuffer || 0 == bufferSize)
        return STATUS_CODE_FAILURE;

      if( ! m_pReadBuffer )
        m_pReadBuffer = new xdrstream::BufferDevice(pEventRawBuffer, bufferSize, false);
      else
        m_pReadBuffer->setBuffer(pEventRawBuffer, bufferSize, false);

      m_pReadBuffer->setOwner(false);

      // deserialize the event raw buffer
      return this->getEventStreamer()->read(pEvent, m_pReadBuffer);
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode DimEventClient::queryEvent()
    {
      scoped_lock( & this->m_mutex);

      if( ! this->isConnectedToService() )
        return STATUS_CODE_NOT_INITIALIZED;

      if( NULL == this->getEventStreamer() )
        return STATUS_CODE_NOT_INITIALIZED;

      // send the query
      m_pDimEventRpcInfo->setData((char*) this->getSubEventIdentifier().c_str());

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    void DimEventClient::setUpdateMode(bool updateMode)
    {
      m_updateMode = updateMode;

      if( ! this->isConnectedToService() )
        return;

      // send command to server
      DimClient::sendCommandNB((char*) (std::string("DQM4HEP/EventCollector/") + this->getCollectorName() + "/UPDATE_MODE" ).c_str(), static_cast<int>(m_updateMode));
    }

    //-------------------------------------------------------------------------------------------------

    bool DimEventClient::getUpdateMode() const
    {
      return m_updateMode;
    }

    //-------------------------------------------------------------------------------------------------

    void DimEventClient::infoHandler()
    {
      DimInfo *pInfo = getInfo();

      if(!pInfo)
        return;

      // client has just been registered
      if(pInfo == m_pClientIdInfo)
      {
        int clientId = pInfo->getInt();

        LOG4CXX_INFO( dqmMainLogger , "Client id on server : " << clientId );

        if(clientId <= 0)
          return;

        m_serverClientId = clientId;
        std::string subEventIdentifierCommandName = "DQM4HEP/EventCollector/" + this->getCollectorName() + "/SUB_EVENT_IDENTIFIER";
        std::string updateModeCommandName = "DQM4HEP/EventCollector/" + this->getCollectorName() + "/UPDATE_MODE";

        // send to server the client parameters
        DimClient::sendCommandNB((char*) updateModeCommandName.c_str(), static_cast<int>(m_updateMode));
        DimClient::sendCommandNB((char*) subEventIdentifierCommandName.c_str(), (char *) this->getSubEventIdentifier().c_str());

        return;
      }

      if(pInfo == m_pServerStateInfo)
      {
        int serverRunning = pInfo->getInt();

        // server is running
        if(serverRunning > 0)
        {
          // client is already registered, so nothing to do ...
          if(m_serverClientId > 0)
            return;

          // client is not registered yet, we need to do it !
          else
          {
            LOG4CXX_INFO( dqmMainLogger , "Registering client into event collector server application !" );
            DimClient::sendCommandNB(("DQM4HEP/EventCollector/" + this->getCollectorName() + "/CLIENT_REGISTRATION").c_str(), 1);
            return;
          }
        }
        // server is shutting down.
        // client is no longer connected and client id is no longer valid.
        else if(m_serverClientId > 0)
        {
          m_serverClientId = 0;
          LOG4CXX_INFO( dqmMainLogger , "Unregistered from event collector server application !" );
          return;
        }
      }

      if(pInfo == m_pEventUpdateInfo)
      {
        if(m_serverClientId == 0)
          return;

        char *pBuffer = (char*) pInfo->getData();
        unsigned int bufferSize = pInfo->getSize();

        if(NULL == pBuffer || 0 == bufferSize)
          return;

        this->eventReception(pBuffer, bufferSize);

        return;
      }
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode DimEventClient::eventReception(dqm_char *pBuffer, dqm_uint bufferSize)
    {
      if(NULL == pBuffer || 0 == bufferSize)
        return STATUS_CODE_INVALID_PARAMETER;

      if(strcmp(pBuffer, "EMPTY") == 0)
        return STATUS_CODE_SUCCESS;

      // set buffer
      scoped_lock( & this->m_mutex);

      if( ! m_pReadBuffer )
        m_pReadBuffer = new xdrstream::BufferDevice(pBuffer, bufferSize, false);
      else
        m_pReadBuffer->setBuffer(pBuffer, bufferSize, false);

      m_pReadBuffer->setOwner(false);

      // read event
      Event *pEvent = NULL;
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->getEventStreamer()->read(pEvent, m_pReadBuffer));
      pEvent->setEventSize(bufferSize);

      LOG4CXX_DEBUG( dqmMainLogger , "DimEventClient : Event received !" );

      // add it to event queue
      this->pushEvent(pEvent);

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode DimEventClient::readSettings(const TiXmlHandle &xmlHandle)
    {
      bool reconnect = this->isConnectedToService();

      // first disconnect
      if( reconnect )
        RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->disconnectFromService());

      // read all xml values first
      std::string collectorName = this->getCollectorName();
      RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::readParameterValue(xmlHandle,
          "collectorName", collectorName));

      std::string subEventIdentifier = this->getSubEventIdentifier();
      RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::readParameterValue(xmlHandle,
          "subEventIdentifier", subEventIdentifier));

      unsigned int maxEventQueueSize = this->getMaximumQueueSize();
      RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::readParameterValue(xmlHandle,
          "maxQueueSize", maxEventQueueSize));

      bool updateMode = this->getUpdateMode();
      RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::readParameterValue(xmlHandle,
          "updateMode", updateMode));

      std::string eventStreamerName;
      RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::readParameterValue(xmlHandle,
          "streamerName", eventStreamerName));

      // configure the client
      // connection to service will come afterward
      this->setCollectorName(collectorName);
      this->setSubEventIdentifier(subEventIdentifier);
      this->setMaximumQueueSize(maxEventQueueSize);
      this->setUpdateMode(updateMode);

      if( ! eventStreamerName.empty() )
      {
        EventStreamer *pEventStreamer = PluginManager::instance()->createPluginClass<EventStreamer>(eventStreamerName);

        if( ! pEventStreamer )
          return STATUS_CODE_FAILURE;

        this->setEventStreamer(pEventStreamer, true);
      }

      // reconnect only if connected before configuration
      if( reconnect )
        RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->connectToService());

      LOG4CXX_INFO( dqmMainLogger , "Collector name : " << collectorName );
      LOG4CXX_INFO( dqmMainLogger , "Sub event identifier : " << subEventIdentifier );
      LOG4CXX_INFO( dqmMainLogger , "Streamer name : " << eventStreamerName );
      LOG4CXX_INFO( dqmMainLogger , "Update mode : " << updateMode );

      return STATUS_CODE_SUCCESS;
    }

  }

}

