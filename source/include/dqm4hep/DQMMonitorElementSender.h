/*
 *
 * DQMMonitorElementSender.h header template automatically generated by a class generator
 * Creation date : sam. mai 9 2015
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


#ifndef DQM4HEP_MONITORELEMENTSENDER_H
#define DQM4HEP_MONITORELEMENTSENDER_H

// -- dqm4hep headers
#include "dqm4hep/DQM4HEP.h"
#include "dqm4hep/DQMStreamingHelper.h"

#include "dic.hxx"

namespace dqm4hep {

  namespace core {

    class ModuleApi;
    class ModuleApplication;

    /** MonitorElementSender class
     */
    class MonitorElementSender : public DimClient
    {
      friend class ModuleApi;
    public:
      /** Constructor
       */
      MonitorElementSender(ModuleApplication *pApplication);

      /** Destructor
       */
      ~MonitorElementSender();

      /** Set the collector name to which the elements will be sent
       */
      StatusCode setCollectorName(const std::string &collectorName);

      /** Send the subscribed monitor element to the collector
       */
      StatusCode sendMonitorElements();

      /** Connect the sender to the collector
       */
      StatusCode connectToService();

      /** Disconnect the sender to the collector
       */
      StatusCode disconnectFromService();

      /** Whether the sender is connected to the collector
       */
      bool isConnectedToService() const;

    private:
      /**
       */
      void infoHandler();

      /**
       */
      void addAvailableMonitorElement(const MonitorElementPtr &monitorElement);

      /**
       */
      void removeAvailableMonitorElement(const std::string &monitorElementName);

      /**
       */
      void sendAvailableMonitorElementList();

    private:
      typedef std::map<std::string, MonitorElementInfo> MonitorElementInfoMap;

      // from ui
      ModuleApplication             *m_pApplication;          ///< The module application that sends elements to the collector
      std::string                    m_collectorName;         ///< The collector name to which the monitor elements will be sent

      xdrstream::BufferDevice        *m_pOutBuffer;           ///< The xdr buffer used to serialize dim info
      xdrstream::BufferDevice        *m_pInBuffer;            ///< The xdr buffer used to serialize the monitor elements

      // internal
      StringSet                      m_subscribedMeList;      ///< The subscribed monitor element list
      MonitorElementInfoMap          m_availableMeMap;        ///< The available monitor element list
      pthread_mutex_t                m_mutex;                 ///< Mutex to lock unlock on service update
      bool                           m_sendAvailableMeList;   ///< Whether the list of available me has to be re-sent to the collector
      bool                           m_isConnected;           ///< Whether the sender is connected to the collector

      DimUpdatedInfo                *m_pSubscribedListInfo;   ///< The dim info to receive subscribed me list from collector
      DimUpdatedInfo                *m_pCollectorStateInfo;   ///< The dim info to receive when the collector state changes
    };

  }

} 

#endif  //  DQM4HEP_MONITORELEMENTSENDER_H
