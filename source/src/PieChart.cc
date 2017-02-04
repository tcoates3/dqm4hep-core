  /// \file PieChart.cc
/*
 *
 * PieChart.cc source template automatically generated by a class generator
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


#include "dqm4hep/PieChart.h"

namespace dqm4hep {

  namespace core {

    PieChart::PieChart() :
      MonitorObject(),
      m_drawLegend(true)
    {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    PieChart::~PieChart()
    {
      this->clear();
    }

    //-------------------------------------------------------------------------------------------------

    PieChart *PieChart::create(const Json::Value &value)
    {
      PieChart * pPieChart = new PieChart();
      pPieChart->fromJson(value);

      return pPieChart;
    }

    //-------------------------------------------------------------------------------------------------

    void PieChart::setTitle(const std::string &title)
    {
      bool updated(false);

      if(m_title != title)
      {
        m_updateCache.set(TITLE, true);
        updated = true;
      }

      m_title = title;

      if(updated)
        this->updated();
    }

    //-------------------------------------------------------------------------------------------------

    const std::string &PieChart::getTitle() const
    {
      return m_title;
    }

    //-------------------------------------------------------------------------------------------------

    void PieChart::setDrawLegend(bool draw)
    {
      bool updated(false);

      if(draw != m_drawLegend)
      {
        m_updateCache.set(DRAW_LEGEND, true);
        updated = true;
      }

      m_drawLegend = draw;

      if(updated)
        this->updated();
    }

    //-------------------------------------------------------------------------------------------------

    bool PieChart::shouldDrawLegend() const
    {
      return m_drawLegend;
    }

    //-------------------------------------------------------------------------------------------------

    void PieChart::addEntry(const std::string &name, Color color, const float &value)
    {
      auto findIter = m_entries.find(name);

      if(findIter != m_entries.end())
      {
        findIter->second.m_color = color;
        findIter->second.m_value = value;

        this->normalize();
        m_updateCache.set(ENTRIES);
        this->updated();

        return;
      }

      EntryMetadata metadata;
      metadata.m_color = color;
      metadata.m_value = value;
      metadata.m_percentage = 0.f;

      m_entries[name] = metadata;

      this->normalize();
      m_updateCache.set(ENTRIES);
      this->updated();
    }

    //-------------------------------------------------------------------------------------------------

    void PieChart::setEntryColor(const std::string &name, Color color)
    {
      auto findIter = m_entries.find(name);

      if(findIter != m_entries.end())
        return;

      findIter->second.m_color = color;
      m_updateCache.set(ENTRIES);
      this->updated();
    }

    //-------------------------------------------------------------------------------------------------

    void PieChart::setEntryValue(const std::string &name, const float &value)
    {
      auto findIter = m_entries.find(name);

      if(findIter != m_entries.end())
        return;

      findIter->second.m_value = value;
      this->normalize();
      m_updateCache.set(ENTRIES);
      this->updated();
    }

    //-------------------------------------------------------------------------------------------------

    void PieChart::removeEntry(const std::string &name)
    {
      auto findIter = m_entries.find(name);

      if(findIter != m_entries.end())
        return;

      m_entries.erase(findIter);
      this->normalize();
      m_updateCache.set(ENTRIES);
      this->updated();
    }

    //-------------------------------------------------------------------------------------------------

    void PieChart::clear()
    {
      bool updated(false);

      if(!m_entries.empty())
      {
        m_updateCache.set(ENTRIES);
        updated = false;
      }

      if(!m_title.empty())
      {
        m_updateCache.set(TITLE);
        updated = false;
      }

      if(!m_drawLegend)
      {
        m_updateCache.set(DRAW_LEGEND);
        updated = false;
      }

      m_entries.clear();
      m_title.clear();
      m_drawLegend = true;

      if(updated)
        this->updated();
    }

    //-------------------------------------------------------------------------------------------------

    bool PieChart::isUpToDate() const
    {
      return m_updateCache.none();
    }

    //-------------------------------------------------------------------------------------------------

    void PieChart::fromJson(const Json::Value &value)
    {
      const std::string readMode(value.get("mode", "full").asString());

      if(readMode == "full")
        this->clear();

      m_drawLegend = value.get("leg", m_drawLegend).asBool();
      m_title = value.get("title", m_title).asString();

      Json::Value entryValues(value.get("entries", Json::Value()));

      for(unsigned int e=0 ; e<entryValues.size() ; e++)
      {
        Json::Value entryValue(entryValues[e]);

        const std::string name(entryValue.get("name", "").asString());
        const float value(entryValue.get("value", 0.f).asFloat());
        const Color color(static_cast<Color>(entryValue.get("color", Black).asInt()));

        if(name.empty())
          continue;

        this->addEntry(name, color, value);
      }

      this->resetCache();
    }

    //-------------------------------------------------------------------------------------------------

    void PieChart::toJson(Json::Value &value, bool full, bool resetCache)
    {
      value["mode"] = full ? "full" : "update";

      if( full || m_updateCache.test(DRAW_LEGEND))
        value["leg"] = m_drawLegend;

      if( full || m_updateCache.test(TITLE))
        value["title"] = m_title;

      if( full || m_updateCache.test(ENTRIES))
      {
        Json::Value entryValues(Json::arrayValue);
        unsigned int index(0);

        for(auto iter = m_entries.begin(), endIter = m_entries.end() ; endIter != iter ; ++iter)
        {
          Json::Value entryValue;

          entryValue["name"] = iter->first;
          entryValue["color"] = iter->second.m_color;
          entryValue["value"] = iter->second.m_value;

          entryValues[index] = entryValue;
          ++index;
        }

        value["entries"] = entryValues;
      }

      if(resetCache)
        this->resetCache();
    }

    //-------------------------------------------------------------------------------------------------

    void PieChart::normalize()
    {
      float sum(0.f);

      for(auto iter = m_entries.begin(), endIter = m_entries.end() ; endIter != iter ; ++iter)
        sum += iter->second.m_value;

      for(auto iter = m_entries.begin(), endIter = m_entries.end() ; endIter != iter ; ++iter)
        iter->second.m_percentage = (iter->second.m_value*sum)/100.f;
    }

    //-------------------------------------------------------------------------------------------------

    void PieChart::resetCache()
    {
      m_updateCache.reset();
    }

  }

}