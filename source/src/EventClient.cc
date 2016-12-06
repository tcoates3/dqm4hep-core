/// \file DQMEventClient.cc
/*
 *
 * DQMEventClient.cc source template automatically generated by a class generator
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
#include "dqm4hep/EventClient.h"
#include "dqm4hep/EventStreamer.h"
#include "dqm4hep/Event.h"
#include "dqm4hep/Logging.h"

namespace dqm4hep {

  namespace core {

    EventClient::EventClient() :
        m_pEventStreamer(NULL),
        m_maximumQueueSize(100),
        m_eventStreamerOwner(true)
    {
      pthread_mutex_init(&m_mutex, NULL);
    }

    //-------------------------------------------------------------------------------------------------

    EventClient::~EventClient()
    {
      this->clearQueue();

      if( m_eventStreamerOwner && NULL != m_pEventStreamer )
        delete m_pEventStreamer;

      pthread_mutex_destroy(&m_mutex);
    }

    //-------------------------------------------------------------------------------------------------

    void EventClient::setCollectorName(const std::string &collectorName)
    {
      // if connected to service, can't change collector name
      if( this->isConnectedToService() )
        return;

      m_collectorName = collectorName;
    }

    //-------------------------------------------------------------------------------------------------

    const std::string &EventClient::getCollectorName() const
    {
      return m_collectorName;
    }

    //-------------------------------------------------------------------------------------------------

    void EventClient::setEventStreamer(EventStreamer *pEventStreamer, bool owner)
    {
      if( m_eventStreamerOwner && NULL != m_pEventStreamer )
        delete m_pEventStreamer;

      m_pEventStreamer = pEventStreamer;
      m_eventStreamerOwner = owner;
    }

    //-------------------------------------------------------------------------------------------------

    EventStreamer *EventClient::getEventStreamer() const
    {
      return m_pEventStreamer;
    }

    //-------------------------------------------------------------------------------------------------

    void EventClient::setMaximumQueueSize(unsigned int maxQueueSize)
    {
      if( 0 == maxQueueSize )
        return;

      m_maximumQueueSize = maxQueueSize;

      scoped_lock( & this->m_mutex);

      // shrink the queue to fit the new max queue size
      while( m_eventQueue.size() >  m_maximumQueueSize )
        m_eventQueue.pop();
    }

    //-------------------------------------------------------------------------------------------------

    unsigned int EventClient::getMaximumQueueSize() const
    {
      return m_maximumQueueSize;
    }

    //-------------------------------------------------------------------------------------------------

    void EventClient::clearQueue()
    {
      scoped_lock( & this->m_mutex );

      while( ! m_eventQueue.empty() )
        m_eventQueue.pop();
    }

    //-------------------------------------------------------------------------------------------------

    void EventClient::setSubEventIdentifier(const std::string &identifier)
    {
      m_subEventIdentifier = identifier;
    }

    //-------------------------------------------------------------------------------------------------

    const std::string &EventClient::getSubEventIdentifier() const
    {
      return m_subEventIdentifier;
    }

    //-------------------------------------------------------------------------------------------------

    void EventClient::takeEvent(EventPtr &event)
    {
      scoped_lock( & this->m_mutex );

      if( ! m_eventQueue.empty() )
      {
        event = m_eventQueue.front();
        m_eventQueue.pop();
      }
    }

    //-------------------------------------------------------------------------------------------------

    void EventClient::addListener(EventClientListener *pListener)
    {
      scoped_lock( & this->m_mutex);

      if(NULL == pListener)
        return;

      m_listeners.insert(pListener);
    }

    //-------------------------------------------------------------------------------------------------

    void EventClient::removeListener(EventClientListener *pListener)
    {
      scoped_lock( & this->m_mutex);

      if(NULL == pListener)
        return;

      m_listeners.erase(pListener);
    }

    //-------------------------------------------------------------------------------------------------

    void EventClient::pushEvent(Event *pEvent)
    {
      if(NULL == pEvent)
        return;

      pthread_mutex_lock(& this->m_mutex);

      if( m_eventQueue.size() == m_maximumQueueSize )
        m_eventQueue.pop();

      EventPtr event(pEvent);
      m_eventQueue.push(event);

      // need unlock before notifying
      pthread_mutex_unlock(& this->m_mutex );

      for(std::set<EventClientListener*>::iterator iter = m_listeners.begin(), endIter = m_listeners.end() ;
          endIter != iter ; ++iter)
        (*iter)->eventPushed(this);
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode EventClient::connectToService()
    {
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->performServiceConnection());

      for(std::set<EventClientListener*>::iterator iter = m_listeners.begin(), endIter = m_listeners.end() ;
          endIter != iter ; ++iter)
        (*iter)->onEventClientConnnect(this);

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    StatusCode EventClient::disconnectFromService()
    {
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->performServiceDisconnection());

      for(std::set<EventClientListener*>::iterator iter = m_listeners.begin(), endIter = m_listeners.end() ;
          endIter != iter ; ++iter)
        (*iter)->onEventClientDisconnnect(this);

      return STATUS_CODE_SUCCESS;
    }

  }

}
