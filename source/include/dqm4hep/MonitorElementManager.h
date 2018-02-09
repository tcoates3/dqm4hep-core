/// \file DQMMonitorElementManager.h
/*
 *
 * DQMMonitorElementManager.h header template automatically generated by a class generator
 * Creation date : lun. ao�t 17 2015
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

#ifndef DQM4HEP_MONITORELEMENTMANAGER_H
#define DQM4HEP_MONITORELEMENTMANAGER_H

// -- dqm4hep headers
#include <dqm4hep/Directory.h>
#include <dqm4hep/Internal.h>
#include <dqm4hep/Logging.h>
#include <dqm4hep/MonitorElement.h>
#include <dqm4hep/QualityTest.h>
#include <dqm4hep/StatusCodes.h>
#include <dqm4hep/Storage.h>
#include <dqm4hep/XmlHelper.h>

// -- root headers
#include <TFile.h>
#include <TObject.h>

class TH1F;
class TH1I;
class TH1C;
class TH1S;
class TH2F;
class TH2I;
class TH2C;
class TH2S;
class TH3F;
class TH3I;
class THStack;
class TMultiGraph;
class TH2Poly;
class TProfile;
class TProfile2D;
class TScalarInt;
class TScalarFloat;
class TScalarShort;
class TScalarString;
class TGraph;
class TGraphErrors;
class TGraph2D;

namespace dqm4hep {

  namespace core {

    class TDynamicGraph; // this one comes from DQM4HEP, not ROOT

    typedef allocator_helper<TObject, TH1F, const char *, const char *, int, float, float> TH1FAllocator;
    typedef allocator_helper<TObject, TH1I, const char *, const char *, int, float, float> TH1IAllocator;
    typedef allocator_helper<TObject, TH1C, const char *, const char *, int, float, float> TH1CAllocator;
    typedef allocator_helper<TObject, TH1S, const char *, const char *, int, float, float> TH1SAllocator;

    typedef allocator_helper<TObject, TH2F, const char *, const char *, int, float, float, int, float, float>
        TH2FAllocator;
    typedef allocator_helper<TObject, TH2I, const char *, const char *, int, float, float, int, float, float>
        TH2IAllocator;
    typedef allocator_helper<TObject, TH2C, const char *, const char *, int, float, float, int, float, float>
        TH2CAllocator;
    typedef allocator_helper<TObject, TH2S, const char *, const char *, int, float, float, int, float, float>
        TH2SAllocator;

    typedef allocator_helper<TObject, TH3F, const char *, const char *, int, float, float, int, float, float, int,
                             float, float>
        TH3FAllocator;
    typedef allocator_helper<TObject, TH3I, const char *, const char *, int, float, float, int, float, float, int,
                             float, float>
        TH3IAllocator;

    typedef allocator_helper<TObject, THStack, const char *, const char *> THStackAllocator;
    typedef allocator_helper<TObject, TH2Poly, const char *, const char *, double, double, double, double>
        TH2PolyAllocator;

    typedef allocator_helper<TObject, TProfile, const char *, const char *, int, float, float, float, float>
        TProfileAllocator;
    typedef allocator_helper<TObject, TProfile2D, const char *, const char *, int, float, float, int, float, float,
                             float, float>
        TProfile2DAllocator;

    typedef allocator_helper<TObject, TScalarInt, int> TScalarIntAllocator;
    typedef allocator_helper<TObject, TScalarFloat, float> TScalarFloatAllocator;
    typedef allocator_helper<TObject, TScalarShort, short> TScalarShortAllocator;
    typedef allocator_helper<TObject, TScalarString, std::string> TScalarStringAllocator;

    typedef allocator_helper<TObject, TGraph> TGraphAllocator;
    typedef allocator_helper<TObject, TGraphErrors> TGraphErrorsAllocator;
    typedef allocator_helper<TObject, TGraph2D> TGraph2DAllocator;
    typedef allocator_helper<TObject, TMultiGraph> TMultiGraphAllocator;

    typedef allocator_helper<TObject, TDynamicGraph> TDynamicGraphAllocator;

    /** MonitorElementManager class
     */
    class MonitorElementManager {
      typedef std::shared_ptr<QualityTest> QualityTestPtr;
      typedef std::shared_ptr<QualityTestFactory> QualityTestFactoryPtr;
      typedef std::map<const std::string, QualityTestPtr> QualityTestMap;
      typedef std::map<const std::string, const QualityTestFactoryPtr> QualityTestFactoryMap;

    public:
      /** Constructor
       */
      MonitorElementManager();

      /** Destructor
       */
      ~MonitorElementManager();

      /////////////////////////
      // DIRECTORY INTERFACE //
      /////////////////////////

      /** Go back to root directory
       */
      void cd();

      /** Go to directory 'dirName'
       */
      StatusCode cd(const std::string &dirName);

      /** Create the directory 'dirName'
       */
      StatusCode mkdir(const std::string &dirName);

      /** List the current directory content
       */
      // void ls(bool recursive = false);

      /** Get the current directory name
       */
      const std::string &pwd();

      /** Get the current directory full name
       */
      const std::string &fullPwd();

      /** Navigate backward in the directory structure
       */
      StatusCode goUp();

      /** Remove the directory 'dirName'
       */
      StatusCode rmdir(const std::string &dirName);

      /** Whether the directory exists
       */
      bool dirExists(const std::string &dirName) const;

    public:
      ///////////////////////////////
      // MONITOR ELEMENT INTERFACE //
      ///////////////////////////////

      /** Add a monitor element from an external source.
       *  WARNING : The ROOT object is owned by the framework.
       *  The caller must NOT delete the object
       */
      StatusCode addMonitorElement(const std::string &path, TObject *pObject, MonitorElementPtr &monitorElement);

      /** Read TObject from file and add it to list.
       *  The ROOT TObject is owned by the manager
       */
      StatusCode readMonitorElement(const std::string &fileName, const std::string &path, const std::string &name,
                                    MonitorElementPtr &monitorElement);

      /** Read TObject from file and add it to list.
       *  The ROOT TObject is owned by the manager
       */
      StatusCode readMonitorElement(TFile *pTFile, const std::string &path, const std::string &name,
                                    MonitorElementPtr &monitorElement);

      /** Book a monitor element using the ROOT TClass facility.
       *  The className is passed to TClass::GetClass() to get the corresponding
       *  TClass object handler. Note that to allocate the TObject:
       *    - the default constructor is used
       *    - ROOT object is disabled so that object with same can be allocated safely memory leak
       *  The resulting monitored TObject is owned by the manager
       */
      StatusCode bookMonitorElement(const std::string &className, const std::string &path, const std::string &name,
                                    MonitorElementPtr &monitorElement);

      /** Add a monitor element from an external source.
      *  WARNING : The ROOT object is NOT owned by the framework.
      *  The caller must delete the object on termination
       */
      StatusCode handleMonitorElement(const std::string &path, TObject *pObject, MonitorElementPtr &monitorElement);

      /** Book a monitor element. The objectType must inherit TObject and have a ROOT dictionnary.
       *
       */
      template <typename ObjectType, typename... Args>
      StatusCode bookObject(const std::string &path, const std::string &name, MonitorElementPtr &monitorElement,
                            allocator_helper<TObject, ObjectType, Args...> allocator, Args... args);

      /**
       * @brief  Open root file, find reference object and attach it to the monitor element
       *
       * @param  pMonitorElement the monitor element to attach the reference
       * @param  fileName        the root file name contaning the reference
       */
      StatusCode attachReference(MonitorElementPtr monitorElement, const std::string &fileName);

    public:
      ///////////////////////
      // GETTERS INTERFACE //
      ///////////////////////

      /** Get all the monitor elements already booked by this module in all the directories
       */
      void getMonitorElements(MonitorElementList &monitorElements) const;

      /** Get the monitor element in the current directory (result by ptr reference)
       */
      StatusCode getMonitorElement(const std::string &name, MonitorElementPtr &monitorElement) const;

      /** Get the monitor element in the given directory (result by ptr reference)
       */
      StatusCode getMonitorElement(const std::string &dirName, const std::string &name,
                                   MonitorElementPtr &monitorElement) const;

      ////////////////////////
      // DELETION INTERFACE //
      ////////////////////////

      /** Remove the monitor element
       */
      StatusCode removeMonitorElement(const std::string &path, const std::string &name);

    public:
      ////////////////////////////
      // QUALITY TEST INTERFACE //
      ////////////////////////////

      /** Create a quality test from the xml element.
       *  The xml element must contain the attribute 'type' and 'name'
       */
      StatusCode createQualityTest(TiXmlElement *const pXmlElement);

      /** Add a (already created) quality test to the monitor element
       */
      StatusCode addQualityTest(const std::string &path, const std::string &name, const std::string &qualityTestName);

      /** Remove a quality test from the monitor element
       */
      StatusCode removeQualityTest(const std::string &path, const std::string &name,
                                   const std::string &qualityTestName);

      /**
       */
      StatusCode runQualityTests(QReportStorage &reports);

      /**
       */
      StatusCode runQualityTests(const std::string &path, const std::string &name, QReportStorage &reports);

      /**
       */
      StatusCode runQualityTest(const std::string &path, const std::string &name, const std::string &qualityTestName,
                                QReportStorage &reports);

    private:
      /** Get the monitor element storage
       */
      const Storage<MonitorElement> &getStorage() const;

      typedef std::map<MonitorElementPtr, QualityTestMap> MonitorElementToQTestMap;

      Storage<MonitorElement> m_storage;
      QualityTestFactoryMap m_qualityTestFactoryMap;
      QualityTestMap m_qualityTestMap;
      MonitorElementToQTestMap m_monitorElementToQTestMap;
    };

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    template <typename ObjectType, typename... Args>
    StatusCode MonitorElementManager::bookObject(const std::string &path, const std::string &name,
                                                 MonitorElementPtr &monitorElement,
                                                 allocator_helper<TObject, ObjectType, Args...> allocator,
                                                 Args... args) {
      monitorElement = nullptr;

      if (!ObjectType::Class()->HasDictionary()) {
        dqm_error("Couldn't book object of type '{0}', because this class has no dictionnary!",
                  ObjectType::Class_name());
        return STATUS_CODE_NOT_ALLOWED;
      }

      if (ObjectType::Class()->IsForeign()) {
        dqm_error(
            "Couldn't book object of type '{0}', because this class is foreign (does not have a Streamer method))!",
            ObjectType::Class_name());
        return STATUS_CODE_NOT_ALLOWED;
      }

      if (!ObjectType::Class()->InheritsFrom("TNamed")) {
        dqm_error("Couldn't book object of type '{0}', because this class doesn't inherit TNamed (required to call "
                  "SetName())!",
                  ObjectType::Class_name());
        return STATUS_CODE_NOT_ALLOWED;
      }

      const bool objectStat(TObject::GetObjectStat());
      TObject::SetObjectStat(false);
      TObject *pTObject = allocator.create(args...);
      TObject::SetObjectStat(objectStat);

      if (!pTObject) {
        dqm_warning("Couldn't allocate monitor element of type '{0}', path '{1}', name '{2}'", ObjectType::Class_name(),
                    path, name);
        return STATUS_CODE_FAILURE;
      }

      ((TNamed *)pTObject)->SetName(name.c_str());

      return this->addMonitorElement(path, pTObject, monitorElement);
    }
  }
}

#endif //  DQM4HEP_MONITORELEMENTMANAGER_H
