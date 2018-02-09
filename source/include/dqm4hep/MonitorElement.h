/// \file MonitorElement.h
/*
 *
 * MonitorElement.h header template automatically generated by a class generator
 * Creation date : jeu. sept. 4 2014
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
 * @author Ete Remi
 * @copyright CNRS , IPNL
 */

#ifndef DQM4HEP_MONITORELEMENT_H
#define DQM4HEP_MONITORELEMENT_H

// -- dqm4hep headers
#include <dqm4hep/Internal.h>
#include <dqm4hep/PtrHandler.h>
#include <dqm4hep/StatusCodes.h>

// -- root headers
#include <Rtypes.h>
#include <TGraph.h>
#include <TObject.h>
#include <TPaveText.h>

namespace dqm4hep {

  namespace core {

    class MonitorElementManager;

    /**
     *  @brief  MonitorElement class.
     *
     *  @author Remi Ete, DESY
     */
    class MonitorElement : public std::enable_shared_from_this<MonitorElement> {
      friend class MonitorElementManager;

    public:
      /** Constructor
       */
      static MonitorElementPtr make_shared();

      /** Constructor with ROOT object
       */
      static MonitorElementPtr make_shared(TObject *pMonitorObject);

      /** Constructor with ROOT object and reference
       */
      static MonitorElementPtr make_shared(TObject *pMonitorObject, TObject *pReferenceObject);

      /** Constructor with ROOT object ptr
       */
      static MonitorElementPtr make_shared(const PtrHandler<TObject> &monitorObject);

      /** Constructor with ROOT object and reference ptr
       */
      static MonitorElementPtr make_shared(const PtrHandler<TObject> &monitorObject,
                                           const PtrHandler<TObject> &referenceObject);

      /** Get the monitor element type (class name)
       */
      std::string type() const;

      /** Get the object name
       */
      std::string name() const;

      /** Get the object title
       */
      std::string title() const;

      /** Get the object path (in directory structure)
       */
      std::string path() const;

      /** Whether the monitor element has a valid object ptr
       */
      bool hasObject() const;

      /** Whether the monitor element has a valid reference ptr
       */
      bool hasReference() const;

      /** Get the wrapped object
       */
      TObject *object();

      /** Get the wrapped object
       */
      const TObject *object() const;

      /** Get the wrapped reference object
       */
      TObject *reference();

      /** Get the wrapped reference object
       */
      const TObject *reference() const;

      /** Get a casted version of the wrapped object
       */
      template <typename T>
      T *objectTo();

      /** Get a casted version of the wrapped reference object
       */
      template <typename T>
      T *referenceTo();

      /** Set the wrapped object
       */
      void setMonitorObject(TObject *pMonitorObject);

      /** Set the wrapped object (ptr handle)
       */
      void setMonitorObject(const PtrHandler<TObject> &monitorObject);

      /** Set the wrapped reference object
       */
      void setReferenceObject(TObject *pReferenceObject);

      /** Set the wrapped reference object (ptr handle)
       */
      void setReferenceObject(const PtrHandler<TObject> &referenceObject);

      /** Set the wrapped object and reference object
       */
      void set(TObject *pMonitorObject, TObject *pReferenceObject);

      /** Set the wrapped object and reference object (ptr handle)
       */
      void set(const PtrHandler<TObject> &monitorObject, const PtrHandler<TObject> &referenceObject);

    private:
      /** Constructor
       */
      MonitorElement();

      /** Constructor with ROOT object
       */
      MonitorElement(TObject *pMonitorObject);

      /** Constructor with ROOT object and reference
       */
      MonitorElement(TObject *pMonitorObject, TObject *pReferenceObject);

      /** Constructor with ROOT object ptr
       */
      MonitorElement(const PtrHandler<TObject> &monitorObject);

      /** Constructor with ROOT object and reference ptr
       */
      MonitorElement(const PtrHandler<TObject> &monitorObject, const PtrHandler<TObject> &referenceObject);

      /** Set the monitor element object path
       */
      void setPath(const std::string &path);

      /** Add a quality test
       */
      StatusCode addQualityTest(QTestPtr qualityTest);

      /** Remove a quality test
       */
      StatusCode removeQualityTest(const std::string &name);

      /** Run all quality tests
       */
      StatusCode runQualityTests(QReportMap &reports);

      /** Run a specific quality test
       */
      StatusCode runQualityTest(const std::string &name, QReport &report);

    private:
      std::string m_path;                    ///< The monitor element path
      PtrHandler<TObject> m_monitorObject;   ///< The monitored object
      PtrHandler<TObject> m_referenceObject; ///< The reference object
      QTestMap m_qualityTests;               ///< The list of assigned quality tests
    };

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline T *MonitorElement::objectTo() {
      T *objCast = (T *)this->object(); // use old C cast for ROOT objects ...
      return objCast ? objCast : nullptr;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline T *MonitorElement::referenceTo() {
      T *objCast = (T *)this->reference(); // use old C cast for ROOT objects ...
      return objCast ? objCast : nullptr;
    }

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    /** TScalarObject class.
     *  Extension of a TObject for scalar values like int,
     *  float, double or string.
     */
    template <typename T>
    class TScalarObject : public TObject {
    public:
      /** Default constructor
       */
      TScalarObject();

      /** Constructor with scalar value
       */
      TScalarObject(const T &scalar);

      /** Destructor
       */
      ~TScalarObject() override;

      /** Clear the scalar object
       */
      void Clear(Option_t *option = "") override;

      /** Draw the scalar object (using a TPaveText)
       */
      void Draw(Option_t *option = "") override;

      /** The scalar value
       */
      void Set(const T &value);

      /** Get the scalar value
       */
      const T &Get() const;

      /** Convert the scalar value to std::string
       */
      std::string ToString() const;

    private:
      /** Initialize the scalar object (called only in C'tor)
       */
      void Init();

      T m_scalar;             ///< The scalar value
      TPaveText *m_pPaveText; ///< The pave text on which to draw the scalar value

      ClassDefOverride(TScalarObject, 1);
    };

    typedef TScalarObject<dqm_int> TScalarInt;
    typedef TScalarObject<dqm_float> TScalarReal;
    typedef TScalarObject<dqm_float> TScalarFloat;
    typedef TScalarObject<dqm_double> TScalarDouble;
    typedef TScalarObject<dqm_short> TScalarShort;
    typedef TScalarObject<long> TScalarLong;
    typedef TScalarObject<Long64_t> TScalarLong64_t;
    typedef TScalarObject<std::string> TScalarString;

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    /** TDynamicGraph class
     *
     *  A TGraph with a dynamic x range axis
     */
    class TDynamicGraph : public TGraph {
    public:
      enum { kDynamicRange = BIT(14), kShrinkToRange = BIT(15) };

      /** Constructor
       */
      TDynamicGraph();

      /** Destructor
       */
      ~TDynamicGraph() override;

      /** Set the range length.
       *  If kDynamicRange and kShrinkToRange bit are set
       *  then all points that do not fit the x range are removed
       */
      void SetRangeLength(Double_t rangeLength);

      /** Add a point at end of array.
       *  Remove points that do not fit the dynamic range
       *  if kDynamicRange and kShrinkToRange bit are set
       */
      void AddPoint(Double_t x, Double_t y);

      /** Draw dynamic graph. Apply dynamic range if bit set
       */
      void Draw(Option_t *option = "") override;

    private:
      /**
       */
      void ShrinkToRange();

    private:
      Double_t m_rangeLength;

      ClassDefOverride(TDynamicGraph, 1);
    };

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    template <typename T>
    TScalarObject<T>::TScalarObject() {
      Init();
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    TScalarObject<T>::TScalarObject(const T &scalar) : m_scalar(scalar), m_pPaveText(nullptr) {
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    TScalarObject<T>::~TScalarObject() {
      if (nullptr != m_pPaveText) {
        delete m_pPaveText;
        m_pPaveText = nullptr;
      }
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    void TScalarObject<T>::Clear(Option_t *option) {
      if (nullptr != m_pPaveText)
        m_pPaveText->Clear(option);

      m_scalar = 0;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    void TScalarObject<T>::Draw(Option_t *option) {
      std::string scalarStr = ToString();

      if (nullptr == m_pPaveText) {
        m_pPaveText = new TPaveText(0.1, 0.1, 0.9, 0.9, "NDC");
        m_pPaveText->SetFillColor(0);
        m_pPaveText->SetBorderSize(0);
        m_pPaveText->SetShadowColor(0);
      }

      m_pPaveText->Clear();
      m_pPaveText->AddText(scalarStr.c_str());
      m_pPaveText->SetTextAlign(11);
      m_pPaveText->Draw(option);
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    void TScalarObject<T>::Set(const T &value) {
      m_scalar = value;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    const T &TScalarObject<T>::Get() const {
      return m_scalar;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    std::string TScalarObject<T>::ToString() const {
      std::stringstream ss;
      ss << m_scalar;
      return ss.str();
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    void TScalarObject<T>::Init() {
      m_scalar = 0;
      m_pPaveText = nullptr;
    }

    //-------------------------------------------------------------------------------------------------
    // template specialization for std::string
    //-------------------------------------------------------------------------------------------------

    template <>
    inline void TScalarObject<std::string>::Clear(Option_t *option) {
      if (nullptr != m_pPaveText)
        m_pPaveText->Clear(option);

      m_scalar.clear();
    }

    //-------------------------------------------------------------------------------------------------

    template <>
    inline std::string TScalarObject<std::string>::ToString() const {
      return m_scalar;
    }

    //-------------------------------------------------------------------------------------------------

    template <>
    inline void TScalarObject<std::string>::Init() {
      m_scalar = "";
      m_pPaveText = nullptr;
    }

    //-------------------------------------------------------------------------------------------------

    template <>
    inline void TScalarObject<std::string>::Draw(Option_t *option) {
      StringVector lines;
      dqm4hep::core::tokenize(m_scalar, lines, "\n");

      if (nullptr == m_pPaveText) {
        m_pPaveText = new TPaveText(0.1, 0.1, 0.9, 0.9, "NDC");
        m_pPaveText->SetFillColor(0);
        m_pPaveText->SetBorderSize(0);
        m_pPaveText->SetShadowColor(0);
      }

      m_pPaveText->Clear();

      for (auto iter = lines.begin(), endIter = lines.end(); endIter != iter; ++iter)
        m_pPaveText->AddText((*iter).c_str());

      m_pPaveText->Draw(option);
    }
  }
}

#endif //  DQM4HEP_MONITORELEMENT_H
