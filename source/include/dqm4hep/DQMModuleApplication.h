/// \file DQMModuleApplication.h
/*
 *
 * DQMModuleApplication.h header template automatically generated by a class generator
 * Creation date : dim. sept. 6 2015
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


#ifndef DQM4HEP_MODULEAPPLICATION_H
#define DQM4HEP_MODULEAPPLICATION_H

// -- dqm4hep headers
#include "dqm4hep/DQM4HEP.h"
#include "dqm4hep/DQMApplication.h"
#include "dqm4hep/DQMModule.h"
#include "dqm4hep/DQMAlertSystem.h"
#include "dqm4hep/DQMMonitorElementSender.h"
#include "dqm4hep/DQMMonitorElementManager.h"

namespace dqm4hep {

  namespace core {

    class Archiver;
    class ModuleApi;

    /** ModuleApplication class
     */
    class ModuleApplication : public Application
    {
      friend class Archiver;
      friend class ModuleApi;
    public:
      /** Constructor
       */
      ModuleApplication();

      /** Destructor
       */
      virtual ~ModuleApplication();

      /** Run the application
       */
      virtual StatusCode run() = 0;

      /** Stop the application
       */
      virtual StatusCode exit( int returnCode );

      /** Read the settings from file
       */
      virtual StatusCode readSettings( const std::string &settingsFile ) = 0;

      /** Get the application type
       */
      virtual const std::string &getType() const = 0;

      /** Get the application name
       */
      virtual const std::string &getName() const = 0;

      /** Get the module for this application
       */
      Module *getModule() const;

      /** Whether the module application should stop
       */
      bool shouldStopApplication() const;

      /** Set the application stop flag
       */
      void setStopApplication(bool stopApplication);

      /** Set the module name in used
       */
      StatusCode setModuleName(const std::string &name);

      /** Set the module type to look for (before calling readSettings())
       */
      StatusCode setModuleType(const std::string &type);

      /** Get the module name in used
       */
      const std::string &getModuleName() const;

      /** Get the module type in used
       */
      const std::string &getModuleType() const;

      /** Whether the application is initialized
       */
      bool isInitialized() const;

      /** Get the return code when application ends
       */
      StatusCode getReturnCode() const;

    protected:
      /** Set the application has initialized
       */
      void setInitialized(bool initialized);

      /** Get the monitor element manager
       */
      MonitorElementManager *getMonitorElementManager() const;

      /** Get the monitor element sender
       */
      MonitorElementSender *getMonitorElementSender() const;

      /** Get the alert notifier
       */
      AlertNotifier *getAlertNotifier() const;

      /** Set the module in use in this application
       */
      StatusCode setModule(Module *pModule);

      /** Create the alert notifier with the module name
       */
      StatusCode createAlertNotifier(const std::string moduleName);

    protected:
      StatusCode                   m_returnCode;
      Module                      *m_pModule;

    private:
      bool                         m_isInitialized;
      bool                         m_shouldStop;
      std::string                  m_moduleType;
      std::string                  m_moduleName;

      MonitorElementManager       *m_pMonitorElementManager;
      MonitorElementSender        *m_pMonitorElementSender;
      AlertNotifier               *m_pAlertNotifier;
    };

  }

} 

#endif  //  DQM4HEP_MODULEAPPLICATION_H
