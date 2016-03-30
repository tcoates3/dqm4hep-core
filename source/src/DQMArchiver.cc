  /// \file DQMArchiver.cc
/*
 *
 * DQMArchiver.cc source template automatically generated by a class generator
 * Creation date : mar. oct. 7 2014
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

// -- dqm4hep headers
#include "dqm4hep/DQMArchiver.h"
#include "dqm4hep/DQMDirectory.h"
#include "dqm4hep/DQMStorage.h"
#include "dqm4hep/DQMMonitorElement.h"
#include "dqm4hep/DQMMonitorElementManager.h"
#include "dqm4hep/DQMLogging.h"
#include "dqm4hep/DQMModule.h"
#include "dqm4hep/DQMModuleApplication.h"

// -- root headers
#include "TFile.h"
#include "TObjString.h"
#include "TDirectory.h"
#include "TSystem.h"

namespace dqm4hep
{

//-------------------------------------------------------------------------------------------------

DQMArchiver::DQMArchiver() :
		m_fileName(""),
		m_openingMode(""),
		m_pArchiveFile(NULL),
		m_isOpened(false),
		m_firstArchive(true)
{
	/* nop */
}

//-------------------------------------------------------------------------------------------------

DQMArchiver::DQMArchiver(const std::string &archiveFileName, const std::string &openingMode)
{
	THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, open(archiveFileName, openingMode));
}

//-------------------------------------------------------------------------------------------------

DQMArchiver::~DQMArchiver()
{
	if(m_isOpened)
		close();
}

//-------------------------------------------------------------------------------------------------

StatusCode DQMArchiver::open(const std::string &archiveFileName, const std::string &openingMode)
{
	// if already open write the archive if not done
	// and close it before to re-open
	if(m_isOpened)
		RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, close());

	if(archiveFileName.empty())
		return STATUS_CODE_INVALID_PARAMETER;

	std::string fullArchiveName;
	std::string baseArchiveName;

	if(archiveFileName.size() > 5 && archiveFileName.substr(archiveFileName.size()-5) == ".root")
	{
		fullArchiveName = archiveFileName;
		baseArchiveName = archiveFileName.substr(0, archiveFileName.size()-5);
	}
	else
	{
		fullArchiveName = archiveFileName + ".root";
		baseArchiveName = archiveFileName;
	}

	int fileId = 0;

	while(!gSystem->AccessPathName(fullArchiveName.c_str()))
	{
		fileId ++;
		std::stringstream ss;
		ss << baseArchiveName << "_" << fileId << ".root";
		fullArchiveName = ss.str();
	}

	m_fileName = fullArchiveName;
	m_openingMode = openingMode;

	LOG4CXX_INFO( dqmMainLogger , "Opening archive " << m_fileName );

	m_pArchiveFile = new TFile(m_fileName.c_str(), openingMode.c_str());

	if(NULL == m_pArchiveFile)
	{
		LOG4CXX_ERROR( dqmMainLogger , "Couldn't open archive " << m_fileName << " !" );
		return STATUS_CODE_FAILURE;
	}

	m_isOpened = true;
	m_firstArchive = true;

	return STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

StatusCode DQMArchiver::close()
{
	if(!m_isOpened)
		return STATUS_CODE_FAILURE;

	m_pArchiveFile->Close();
	delete m_pArchiveFile;

	m_pArchiveFile = NULL;

	m_isOpened = false;
	m_firstArchive = false;
	m_openingMode = "";

	return STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

StatusCode DQMArchiver::archive(DQMModule *pModule, bool archiveAll)
{
	if(!isOpened())
		return STATUS_CODE_NOT_INITIALIZED;

	LOG4CXX_INFO( dqmMainLogger , "Archiving monitor elements of module : " << pModule->getName() );

	// archive directory
	m_pArchiveFile->cd();

	TDirectory *pArchiveDirectory = m_pArchiveFile->GetDirectory("root");

	if(pArchiveDirectory)
		m_pArchiveFile->rmdir("root");

	pArchiveDirectory = m_pArchiveFile->mkdir("root");

	if(!pArchiveDirectory)
		return STATUS_CODE_FAILURE;

	if(m_firstArchive)
	{
		// write module info in the file
		TObjString *pModuleNameString = new TObjString();
		pModuleNameString->SetString(pModule->getName().c_str());
		m_pArchiveFile->WriteTObject(pModuleNameString);
		delete pModuleNameString;

		TObjString *pModuleVersionString = new TObjString();
		pModuleVersionString->SetString(pModule->getVersion().toString().c_str());
		m_pArchiveFile->WriteTObject(pModuleVersionString);
		delete pModuleVersionString;

		TObjString *pModuleDetectorString = new TObjString();
		pModuleDetectorString->SetString(pModule->getDetectorName().c_str());
		m_pArchiveFile->WriteTObject(pModuleDetectorString);
		delete pModuleDetectorString;

		m_firstArchive = false;
	}

	// get the storage of the module
	DQMStorage *pStorage = pModule->getModuleApplication()->getMonitorElementManager()->getStorage();

	RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, archive(pStorage, pArchiveDirectory));

	delete pArchiveDirectory;

	return STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

StatusCode DQMArchiver::archive(DQMStorage *pStorage, TDirectory *pDirectory)
{
	RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, recursiveFill(pStorage->getRootDirectory(), pDirectory));

	m_pArchiveFile->cd();
	m_pArchiveFile->Write();

	return STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

const std::string &DQMArchiver::getFileName() const
{
	return m_fileName;
}

//-------------------------------------------------------------------------------------------------

bool DQMArchiver::isOpened() const
{
	return m_isOpened;
}

//-------------------------------------------------------------------------------------------------

const std::string &DQMArchiver::getOpeningMode() const
{
	return m_openingMode;
}

//-------------------------------------------------------------------------------------------------

StatusCode DQMArchiver::recursiveFill(DQMDirectory *pDirectory, TDirectory *pROOTDir)
{
	if(NULL == pDirectory || NULL == pROOTDir)
		return STATUS_CODE_INVALID_PTR;

	pROOTDir->cd();

	const std::vector<DQMDirectory*> &subDirList(pDirectory->getSubDirList());

	if(!subDirList.empty())
	{
		for(std::vector<DQMDirectory*>::const_iterator iter = subDirList.begin(), endIter = subDirList.end() ;
				endIter != iter ; ++iter)
		{
			DQMDirectory *pSubDir = *iter;
			TDirectory *pROOTSubDir = pROOTDir->mkdir(pSubDir->getName().c_str());

			if(NULL != pROOTSubDir)
			{
				RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, recursiveFill(pSubDir, pROOTSubDir));
			}
		}
	}

	const DQMMonitorElementPtrList &monitorElementList(pDirectory->getMonitorElementList());

	// write the monitor elements
	if( ! monitorElementList.empty() )
	{
		RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, writeMonitorElements(pDirectory, pROOTDir));
	}

	return STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

StatusCode DQMArchiver::writeMonitorElements(DQMDirectory *pDirectory, TDirectory *pROOTDir)
{
	if(NULL == pDirectory || NULL == pROOTDir)
		return STATUS_CODE_INVALID_PTR;

	pROOTDir->cd();

	const DQMMonitorElementPtrList &monitorElementList(pDirectory->getMonitorElementList());

	for(DQMMonitorElementPtrList::const_iterator iter = monitorElementList.begin(), endIter = monitorElementList.end() ;
			endIter != iter ; ++iter)
	{
		TObject *pObject = (*iter)->getObject();
		pROOTDir->WriteObjectAny(pObject, pObject->IsA(), (*iter)->getName().c_str());
	}

	return STATUS_CODE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

} 

