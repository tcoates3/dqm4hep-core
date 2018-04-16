/// \file AnalysisHelper.cc
/*
 *
 * AnalysisHelper.cc source template automatically generated by a class generator
 * Creation date : mar. juil. 1 2014
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
 * @author Tom Coates
 * @copyright CNRS , IPNL
 */

// -- dqm4hep headers
#include <dqm4hep/AnalysisHelper.h>

namespace dqm4hep {

  namespace core {

    //----------------------------------------------------------------------------------------------------

    float AnalysisHelper::mainHelper(MonitorElementPtr pMonitorElement, std::string testType, float percentage)
    {

      if (nullptr == pMonitorElement) {
        throw StatusCodeException(STATUS_CODE_INVALID_PTR);
      }

      TH1 *h = pMonitorElement->objectTo<TH1>();
      TAxis *axis = h->GetXaxis();
      int nbins = axis->GetNbins();

      if (testType == "Mean")
	{
	  if (percentage < 1.0)
	    {
	      float result = AnalysisHelper::findMeanOfPercent(pMonitorElement, percentage);
	      return result;
	    }
	  else
	    {
	      float result = h->GetMean();
	      return result;
	    }
	}
      else if (testType == "RMS")
	{
	  if (percentage < 1.0)
	    {
	      float result = 0.0;
	      //float result = AnalysisHelper::findRMSOfPercent();
	      return result;
	    }
	  else
	    {
	      float result = h->GetRMS();
	      return result;
	    }
	}
      else if (testType == "Median")
	{
	  float result = AnalysisHelper::findMedian(pMonitorElement);
	  return result;
	}
      else
	{
	  throw StatusCodeException(STATUS_CODE_FAILURE); // The generic error statuscode is temporary until a specific statuscode for this exists, or a custom error message can be written here
	}
    }

    float AnalysisHelper::findMeanOfPercent(MonitorElementPtr pMonitorElement, float percentage = 1.0)
    {

      if (nullptr == pMonitorElement) {
        throw StatusCodeException(STATUS_CODE_INVALID_PTR);
      }

      TH1 *h = pMonitorElement->objectTo<TH1>();
      TAxis *axis = h->GetXaxis();
      int nbins = axis->GetNbins();
      int imean = axis->FindBin(h->GetMean());

      float entries = percentage*h->GetEntries();
      float w = h->GetBinContent(imean);
      float x = h->GetBinCenter(imean);
      float sumw = w;
      float sumwx = w*x;

      for (int i=1;i<nbins;i++)
	{
	  if (i>0)
	    {
	      w = h->GetBinContent(imean-i);
	      x = h->GetBinCenter(imean-i);
	      sumw += w;
	      sumwx += w*x;
	    }
	  if (i<= nbins) {
	    w = h->GetBinContent(imean+i);
	    x = h->GetBinCenter(imean+i);
	    sumw += w;
	    sumwx += w*x;
	  }
	  if (sumw > entries) break;
	}
	      
      float result = sumwx/sumw;
      return result;
	      
    }

    float AnalysisHelper::findMedian(MonitorElementPtr pMonitorElement)
    {

      if (nullptr == pMonitorElement) {
        throw StatusCodeException(STATUS_CODE_INVALID_PTR);
      }

      TH1 *h = pMonitorElement->objectTo<TH1>();

      Double_t xq[1];
      Double_t yq[1];
      xq[0] = 0.5;

      h->GetQuantiles(1, yq, xq);
      float result = yq[0];

      return result;
      
    }

  }

}
