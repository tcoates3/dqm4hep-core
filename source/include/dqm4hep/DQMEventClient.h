/// \file DQMEventClient.h
/*
 *
 * DQMEventClient.h header template automatically generated by a class generator
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


#ifndef DQM4HEP_EVENTCLIENT_H
#define DQM4HEP_EVENTCLIENT_H

// -- dqm4hep headers
#include "dqm4hep/DQM4HEP.h"

// -- pthread headers
#include <pthread.h>

namespace dqm4hep {

  namespace core {

    class EventClient;
    class Event;
    class TiXmlHandle;

    /** EventClientListener class.
     */
    class EventClientListener
    {
    public:
      /** Destructor
       */
      virtual ~EventClientListener() {}

      /** Called back when an event client connects to the service
       */
      virtual void onEventClientConnnect(EventClient *const pClient) = 0;

      /** Called back when an event client disconnects from the service
       */
      virtual void onEventClientDisconnnect(EventClient *const pClient) = 0;

      /** Called back when an event has been pushed in the queue of the event client.
       *
       *  Listeners can accessed to the oldest event pushed in the queue of the client
       *  using EventClient::takeEvent(Event *&pEvent)
       */
      virtual void eventPushed(EventClient *const pClient) = 0;
    };

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    /** EventClient class.
     *
     *  Main interface for sending events to a collector (server part)
     *  and for event queries.
     *
     *  Note that pushing/taking events in the event queue is thread-safe.
     */
    class EventClient
    {
    public:
      /** Constructor
       */
      EventClient();

      /** Destructor
       */
      virtual ~EventClient();

      /** Set the collector to connect to.
       *  Can set the name only if client not yet connected
       */
      virtual void setCollectorName(const std::string &collectorName);

      /** Get the collector name
       */
      const std::string &getCollectorName() const;

      /** Set the streamer that will stream in/out the sent/received event(s).
       *  No default streamer is provided. User must provide one before querying/sending
       *  events from/to collector (server).
       *  Note that the client does not own the streamer !
       */
      virtual void setEventStreamer(EventStreamer *pEventStreamer, bool owner = true);

      /** Get the event streamer
       */
      EventStreamer *getEventStreamer() const;

      /** Set the queue size that stores the received events
       */
      virtual void setMaximumQueueSize(unsigned int queueSize);

      /** Get the maximum number of events that the client can store
       *  in its event queue
       */
      virtual unsigned int getMaximumQueueSize() const;

      /** Clear the event queue
       */
      virtual void clearQueue();

      /** Set the sub event identifier.
       *  This string is sent while querying events.
       *  The received event will be a sub event matching this identifier
       */
      virtual void setSubEventIdentifier(const std::string &identifier);

      /** Get the sub event identifier.
       */
      const std::string &getSubEventIdentifier() const;

      /** Take an event from the event queue (pop front) and return the
       *  pointer to the caller. The event is removed from the queue,
       *  meaning that the caller is responsible for the event deletion.
       *  If no event is available, the queue remains unchanged and the
       *  pointer is not set
       */
      void takeEvent(EventPtr &event);

      /** Add a listener to this event client
       */
      void addListener(EventClientListener *pListener);

      /** Remove a listener from this event client
       */
      void removeListener(EventClientListener *pListener);

      /** Connect to the collector service (server)
       */
      StatusCode connectToService();

      /** Disconnect the client from the collector (server)
       */
      StatusCode disconnectFromService();

    public:
      /** Whether the client is connected to the collector (server)
       */
      virtual bool isConnectedToService() const = 0;

      /** Send an event to the collector (server).
       *  Possible only if a connection has been created (connectToService())
       *  and an event streamer set.
       */
      virtual StatusCode sendEvent(const Event *const pEvent) = 0;

      /** Query an event to the collector (server) with a timeout
       *  and handle it without pushing it into the internal queue
       */
      virtual StatusCode queryEvent(Event *&pEvent, int timeout) = 0;

      /** Query an event to the collector.
       *
       *  A command is send to the collector in order to send back an event.
       *  This method does not wait for the event reception.
       *
       *  To query a single event that is directly handled by the caller,
       *  use querySingleEvent().
       *
       *  The received event is push in an internal event queue.
       */
      virtual StatusCode queryEvent() = 0;

      /** Set the update mode.
       *  If the update mode is set to true, a command
       *  is sent to the server in order to update the client
       *  as soon as an event is received in the collector server.
       */
      virtual void setUpdateMode(bool updateMode) = 0;

      /** Whether the update mode is set
       */
      virtual bool getUpdateMode() const = 0;

      /** Read settings from the xml handle
       */
      virtual StatusCode readSettings(const TiXmlHandle &xmlHandle) = 0;

    protected:
      /** Workhorse of the service connection
       */
      virtual StatusCode performServiceConnection() = 0;

      /** Workhorse of the service connection
       */
      virtual StatusCode performServiceDisconnection() = 0;

      /** Push a new event in the event queue.
       *
       *  If the event queue has reaches its maximum size
       *  the front element of the queue is first deleted and popped.
       *
       *  Listeners are notified of a 'push event'
       */
      void pushEvent(Event *pEvent);

    private:
      bool                                 m_eventStreamerOwner;
      EventStreamer                       *m_pEventStreamer;
      std::string                          m_collectorName;
      std::string                          m_subEventIdentifier;
      EventPtrQueue                        m_eventQueue;
      unsigned int                         m_maximumQueueSize;
      std::set<EventClientListener*>       m_listeners;
      mutable pthread_mutex_t              m_mutex;   // to prevent data race on access
    };

  }

} 

#endif  //  DQM4HEP_EVENTCLIENT_H
