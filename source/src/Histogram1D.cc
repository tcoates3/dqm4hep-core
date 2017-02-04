  /// \file Histogram1D.cc
/*
 *
 * Histogram1D.cc source template automatically generated by a class generator
 * Creation date : jeu. f�vr. 2 2017
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


#include "dqm4hep/Histogram1D.h"

namespace dqm4hep {

  namespace core {

    Histogram1D::Histogram1D(const unsigned int nBins, const float &minValue, const float &maxValue) :
      MonitorObject(),
      m_nBins(nBins),
      m_min(std::min(minValue, maxValue)),
      m_max(std::max(maxValue, minValue)),
      m_binWidth((m_max-m_min)/m_nBins)
    {
      m_pHistogramBins = new float[m_nBins];
    }

    Histogram1D::~Histogram1D()
    {
      delete [] m_pHistogramBins;
    }

    void Histogram1D::setTitle(const std::string &title)
    {
      m_title = title;
    }


    const std::string &Histogram1D::getTitle() const
    {
      return m_title;
    }


    unsigned int Histogram1D::getNBins() const
    {
      return m_nBins;
    }

    float Histogram1D::getMin() const
    {
      return m_min;
    }

    float Histogram1D::getMax() const
    {
      return m_max;
    }

    float Histogram1D::getBinWidth() const
    {
      return m_binWidth;
    }


  }

}