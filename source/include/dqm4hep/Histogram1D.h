/// \file Histogram1D.h
/*
*
* Histogram1D.h header template automatically generated by a class generator
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


#ifndef HISTOGRAM1D_H
#define HISTOGRAM1D_H

#include "dqm4hep/DQM4HEP.h"
#include "dqm4hep/MonitorObject.h"

namespace dqm4hep {

  namespace core {

    /**
    * Histogram1D class
    */
    class Histogram1D final : public MonitorObject
    {
    public:
      typedef std::map<unsigned int, float>  HistogramMap;

      /**
      * Constructor
      */
      Histogram1D(const unsigned int nBins, const float &min, const float &max);

      /**
      * Destructor
      */
      ~Histogram1D();

      /**
      * [setTitle description]
      * @param title [description]
      */
      void setTitle(const std::string &title);

      /**
      * [getTitle description]
      * @return [description]
      */
      const std::string &getTitle() const;

      /**
      * [getNBins description]
      * @return [description]
      */
      unsigned int getNBins() const;

      /**
      * [getMin description]
      * @return [description]
      */
      float getMin() const;

      /**
      * [getMax description]
      * @return [description]
      */
      float getMax() const;

      /**
      * [getBinWidth description]
      * @return [description]
      */
      float getBinWidth() const;

      /**
      * [getBinContent description]
      * @param  bin [description]
      * @return     [description]
      */
      float getBinContent(const unsigned int bin) const;

      /**
      * [getBinNumber description]
      * @param  value [description]
      * @return       [description]
      */
      unsigned int getBinNumber(const float value) const;

      /**
      * [getMinBinNumber description]
      * @return [description]
      */
      unsigned int getMinBinNumber() const;

      /**
      * [getMaxBinNumber description]
      * @return [description]
      */
      unsigned int getMaxBinNumber() const;

      /**
       * [getCumulativeSum description]
       * @return [description]
       */
      float getCumulativeSum() const;

      /**
       * [getCumulativeSum description]
       * @param  lowBin  [description]
       * @param  highBin [description]
       * @return         [description]
       */
      float getCumulativeSum(const unsigned int lowBin, const unsigned int highBin) const;

      /**
       * [getMaximum description]
       * @param lowBin       [description]
       * @param highBin      [description]
       * @param maximumValue [description]
       * @param maximumBin   [description]
       */
      void getMaximum(const unsigned int lowBin, const unsigned int highBin, float &maximumValue, unsigned int &maximumBin) const;

      /**
       * [getMaximum description]
       * @param maximumValue [description]
       * @param maximumBin   [description]
       */
      void getMaximum(float &maximumValue, unsigned int &maximumBin) const;

      /**
       * [getMinimum description]
       * @param lowBin       [description]
       * @param highBin      [description]
       * @param minimumValue [description]
       * @param minimumBin   [description]
       */
      void getMinimum(const unsigned int lowBin, const unsigned int highBin, float &minimumValue, unsigned int &minimumBin) const;

      /**
       * [getMinimum description]
       * @param minimumValue [description]
       * @param minimumBin   [description]
       */
      void getMinimum(float &minimumValue, unsigned int &minimumBin) const;

      /**
       * [getMean description]
       * @param  lowBin  [description]
       * @param  highBin [description]
       * @return         [description]
       */
      float getMean(const unsigned int lowBin, const unsigned int highBin) const;

      /**
       * [getMean description]
       * @return [description]
       */
      float getMean() const;

      /**
       * [getStdDeviation description]
       * @param  lowBin  [description]
       * @param  highBin [description]
       * @return         [description]
       */
      float getStdDeviation(const unsigned int lowBin, const unsigned int highBin) const;

      /**
       * [getStdDeviation description]
       * @return [description]
       */
      float getStdDeviation() const;

      /**
       * [getRms description]
       * @param  lowBin  [description]
       * @param  highBin [description]
       * @return         [description]
       */
      float getRms(const unsigned int lowBin, const unsigned int highBin) const;

      /**
       * [getRms description]
       * @return [description]
       */
      float getRms() const;

      /**
       * [getSkewness description]
       * @param  lowBin  [description]
       * @param  highBin [description]
       * @return         [description]
       */
      float getSkewness(const unsigned int lowBin, const unsigned int highBin) const;

      /**
       * [getSkewness description]
       * @return [description]
       */
      float getSkewness() const;


      /**
       * [setBinContent description]
       * @param bin   [description]
       * @param value [description]
       */
      void setBinContent(const unsigned int bin, const float value);

      /**
       * [fill description]
       * @param value  [description]
       * @param weight [description]
       */
      void fill(const float value, const float weight = 1.f);

      /**
       * [scale description]
       * @param scaleFactor [description]
       */
      void scale(const float scaleFactor);

      /**
       * [clear description]
       */
      void clear();

      /**
       * [setLineWidth description]
       * @param width [description]
       */
      void setLineWidth(int width);

      /**
       * [setLineColor description]
       * @param color [description]
       */
      void setLineColor(Color color);

      /**
       * [setLineStyle description]
       * @param style [description]
       */
      void setLineStyle(LineStyle style);

      /**
       * [setLineAttributes description]
       * @param width [description]
       * @param color [description]
       * @param style [description]
       */
      void setLineAttributes(int width, Color color, LineStyle style);

      /**
       * [getLineAttributes description]
       * @param width [description]
       * @param color [description]
       * @param style [description]
       */
      void getLineAttributes(int &width, Color &color, LineStyle &style) const;

      /**
       * [setMarkerSize description]
       * @param size [description]
       */
      void setMarkerSize(int size);

      /**
       * [setMarkerColor description]
       * @param color [description]
       */
      void setMarkerColor(Color color);

      /**
       * [setMarkerColor description]
       * @param style [description]
       */
      void setMarkerColor(MarkerStyle style);

      /**
       * [setMarkerAttributes description]
       * @param size  [description]
       * @param color [description]
       * @param style [description]
       */
      void setMarkerAttributes(int size, Color color, MarkerStyle style);

      /**
       * [getMarkerAttributes description]
       * @param size  [description]
       * @param color [description]
       * @param style [description]
       */
      void getMarkerAttributes(int &size, Color &color, MarkerStyle &style) const;

      /**
       * [setFillColor description]
       * @param color [description]
       */
      void setFillColor(Color color);

      /**
       * [setFillStyle description]
       * @param style [description]
       */
      void setFillStyle(FillStyle style);

      /**
       * [setFillAttributes description]
       * @param color [description]
       * @param style [description]
       */
      void setFillAttributes(Color color, FillStyle style);

      /**
       * [getFillAttributes description]
       * @param color [description]
       * @param style [description]
       */
      void getFillAttributes(Color &color, FillStyle &style);

      /**
       * [getXAxis description]
       * @return [description]
       */
      const Axis &getXAxis() const;

      /**
       * [getXAxis description]
       * @return [description]
       */
      Axis &getXAxis();

      /**
       * [getYAxis description]
       * @return [description]
       */
      const Axis &getYAxis() const;

      /**
       * [getYAxis description]
       * @return [description]
       */
      Axis &getYAxis();

    private:
      const unsigned int          m_nBins;
      const float                 m_min;
      const float                 m_max;
      const float                 m_binWidth;
      float                      *m_pHistogramBins;

      std::string                 m_title;
      Axis                        m_xAxis;
      Axis                        m_yAxis;
    };

  }

}

#endif  //  HISTOGRAM1D_H