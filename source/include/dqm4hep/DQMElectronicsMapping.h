/// \file DQMElectronicsMapping.h
/*
 *
 * DQMElectronicsMapping.h header template automatically generated by a class generator
 * Creation date : ven. avr. 8 2016
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


#ifndef DQM4HEP_ELECTRONICSMAPPING_H
#define DQM4HEP_ELECTRONICSMAPPING_H

#include "dqm4hep/DQM4HEP.h"
#include "dqm4hep/DQMLogging.h"
#include "dqm4hep/DQMCartesianVector.h"

namespace dqm4hep {

  namespace core {

    class TiXmlHandle;

    /** ElectronicsMapping interface.
     *
     *  Specific interface for position/cell/electronics conversions for
     *  calorimeter detectors
     */
    class ElectronicsMapping
    {
    public:
      /** Simple structure that holds i, j and layer of a calorimeter cell
       */
      struct Cell
      {
        unsigned int       m_iCell;
        unsigned int       m_jCell;
        unsigned int       m_layer;
      };

      /** Simple structure that holds dif, asic and channel ids
       */
      struct Electronics
      {
        unsigned int       m_difId;
        unsigned int       m_asicId;
        unsigned int       m_channelId;
      };

    public:
      /** Destructor
       */
      virtual ~ElectronicsMapping() {}

      /** Process conversion from cell ids to electronics ids
       */
      virtual StatusCode cellToElectronics( const Cell &cell, Electronics &electronics ) = 0;

      /** Process conversion from dif,asic,channel
       */
      virtual StatusCode electronicstoCell( const Electronics &electronics, Cell &cell ) = 0;

      /** Process conversion from position to cell ids
       */
      virtual StatusCode positionToCell(const CartesianVector &position, Cell &cell) = 0;

      /** Process conversion from cell ids to global position
       */
      virtual StatusCode cellToPosition(const Cell &cell, CartesianVector &position) = 0;

      /** Read settings from xml handle
       */
      virtual StatusCode readSettings(const TiXmlHandle xmlHandle) = 0;

    public:
      /** Process conversion from electronics ids to global position
       */
      virtual StatusCode electronicsToPosition(const Electronics &electronics, CartesianVector &position);

      /** Process conversion from global position to electronics ids
       */
      virtual StatusCode positionToElectronics(const CartesianVector &position, Electronics &electronics);
    };

    //------------------------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------------------------------------------------

    inline StatusCode ElectronicsMapping::electronicsToPosition(const Electronics &electronics, CartesianVector &position)
    {
      Cell cell;
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->electronicstoCell(electronics, cell));
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->cellToPosition(cell, position));

      return STATUS_CODE_SUCCESS;
    }

    //------------------------------------------------------------------------------------------------------------------------------------------

    inline StatusCode ElectronicsMapping::positionToElectronics(const CartesianVector &position, Electronics &electronics)
    {
      Cell cell;
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->positionToCell(position, cell));
      RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->cellToElectronics(cell, electronics));

      return STATUS_CODE_SUCCESS;
    }

  }

} 

#endif  //  DQM4HEP_ELECTRONICSMAPPING_H
