/// \file EventStreamer.cc
/*
 *
 * EventStreamer.cc source template automatically generated by a class generator
 * Creation date : lun. mars 7 2016
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
 * @copyright (c)
 */

// -- dqm4hep headers
#include <dqm4hep/EventStreamer.h>
#include <dqm4hep/Event.h>
#include <dqm4hep/PluginManager.h>

namespace dqm4hep {

  namespace core {
    
    StatusCode EventStreamer::writeEvent(EventPtr event, xdrstream::IODevice *device) {
      // consistency check
      if (nullptr == event or nullptr == device)
        return STATUS_CODE_INVALID_PARAMETER;
      // setup event streamer
      std::string streamerName = event->getStreamerName();
      if(not m_streamer or m_streamerName != streamerName) {
        m_streamer = nullptr;
        m_streamer = PluginManager::instance()->create<EventStreamerPlugin>(streamerName);
        if(not m_streamer) {
          dqm_error( "EventStreamer::writeEvent: streamer '{0}' not registered in plugin manager !", streamerName );
          return STATUS_CODE_FAILURE;
        }
        m_streamerName = streamerName;
      }
      // write streamer name
      if (!XDR_TESTBIT(device->write(&m_streamerName), xdrstream::XDR_SUCCESS))
        return STATUS_CODE_FAILURE;
      // write base event data
      if (!XDR_TESTBIT(event->writeBase(device), xdrstream::XDR_SUCCESS))
        return STATUS_CODE_FAILURE;  
      // write user event data
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_streamer->write(event, device));
      return STATUS_CODE_SUCCESS;
    }
    
    //-------------------------------------------------------------------------------------------------
    
    StatusCode EventStreamer::readEvent(EventPtr &event, xdrstream::IODevice *device) {
      // consistency check
      if (nullptr == device)
        return STATUS_CODE_INVALID_PARAMETER;
      // read streamer name
      std::string streamerName;
      if (!XDR_TESTBIT(device->read(&streamerName), xdrstream::XDR_SUCCESS))
        return STATUS_CODE_FAILURE;
      // setup event streamer
      if(not m_streamer or m_streamerName != streamerName) {
        m_streamer = nullptr;
        m_streamer = PluginManager::instance()->create<EventStreamerPlugin>(streamerName);
        if(not m_streamer) {
          dqm_error( "EventStreamer::readEvent: streamer '{0}' not registered in plugin manager !", streamerName );
          return STATUS_CODE_FAILURE;
        }
        m_streamerName = streamerName;
      }
      // read user event data
      event = m_streamer->createEvent();
      event->setStreamerName(m_streamerName);
      
      // write base event data
      if (!XDR_TESTBIT(event->readBase(device), xdrstream::XDR_SUCCESS))
        return STATUS_CODE_FAILURE;
      
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_streamer->read(event, device));
      event->setEventSize(device->getPosition());
      return STATUS_CODE_SUCCESS;
    }
    
  }
  
}
