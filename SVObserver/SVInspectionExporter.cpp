//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc. Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInspectionExporter
//* .File Name       : $Workfile:   SVInspectionExporter.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   19 Dec 2014 04:06:24  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/foreach.hpp>
//Moved to precompiled header: #include <boost/function.hpp>
//Moved to precompiled header: #include <string>
//Moved to precompiled header: #include <fstream>
//Moved to precompiled header: #include <set>
//Moved to precompiled header: #include <iterator>
#include "SVInspectionExporter.h"
#include "RootObject.h"
#include "SVInspectionProcess.h"
#include "SVIPDoc.h"
#include "SVObserver.h"
#include "SVPPQObject.h"
#include "ObjectInterfaces/IObjectWriter.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectLibrary/DependencyManager.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVUtilityLibrary/ZipHelper.h"
#include "SVXMLLibrary/SVConfigurationTags.h"
#include "SVXMLLibrary/SVObjectXMLWriter.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "Definitions/StringTypeDef.h"
#include "Definitions/GlobalConst.h"
#pragma endregion Includes

#pragma region Declarations
static LPCTSTR scDependentsZipExt = _T(".dependents.zip");
static LPCTSTR scXmlExt = _T(".xml");
static LPCTSTR scExportExt = _T(".bxp");
static LPCTSTR scColorExportExt = _T(".cxp");
#pragma endregion Declarations

static void WriteBaseNode(SvOi::IObjectWriter& rWriter)
{
	//<NODE xmlns="x-schema:#SVR00001" Name="Base" Type="SV_BASENODE">
	_variant_t xmlnsValue;
	_variant_t value;

	xmlnsValue.SetString("x-schema:#SVR00001");
	value.SetString("SV_BASENODE");

	rWriter.StartElement("Base");
	rWriter.ElementAttribute("xmlns", xmlnsValue);
	rWriter.ElementAttribute("Type", value);
}

static void WriteVersion(SvOi::IObjectWriter& rWriter, unsigned long p_version)
{
	_variant_t version(p_version);

	rWriter.StartElement(SvXml::CTAG_ENVIRONMENT);
	rWriter.WriteAttribute(SvXml::CTAG_VERSION_NUMBER, version);
	rWriter.EndElement();
}

static void WritePPQInputs(SvOi::IObjectWriter& rWriter, SVObjectClass* pObject)
{
	rWriter.StartElement(SvXml::CTAG_PPQ);

	SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*> (pObject);
	if ( nullptr != pInspection )
	{
		SVPPQObject* pPPQ = pInspection->GetPPQ();
		if ( nullptr != pPPQ )
		{
			pPPQ->PersistInputs(rWriter);
		}
	}
	rWriter.EndElement();
}

static void WriteGlobalConstants(SvOi::IObjectWriter& rWriter, SVObjectClass* pObject)
{
	rWriter.StartElement(SvXml::CTAG_GLOBAL_CONSTANTS);

	SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*> (pObject);
	if ( nullptr != pInspection && nullptr != pInspection->GetToolSet() )
	{
		SvVol::BasicValueObjects::ValueVector GlobalConstantObjects;

		SvOl::DependencyManager::VertexSet GlobalConstantSet;
		RootObject::getRootChildObjectList( GlobalConstantObjects, SvDef::FqnGlobal, 0 );
		SvVol::BasicValueObjects::ValueVector::const_iterator Iter( GlobalConstantObjects.cbegin() );
		for( ; GlobalConstantObjects.end() != Iter; ++Iter )
		{
			if (nullptr != *Iter)
			{
				SVObjectClass* pGlobalConstantObject = dynamic_cast<SVObjectClass*> ((*Iter).get());
				if (nullptr != pGlobalConstantObject)
				{
					GlobalConstantSet.insert(pGlobalConstantObject->getObjectId());
				}
			}
		}
		//Only Global variables which the inspection is dependent on should be included
		SvOl::DependencyManager::Dependencies DependencyList;
		SvOl::DependencyManager::DependencyInserter Inserter(std::inserter(DependencyList, DependencyList.end()));
		SvOl::DependencyManager::Instance().getDependents(GlobalConstantSet, Inserter, SvOl::JoinType::Dependent);

		SvOl::DependencyManager::Dependencies::const_iterator PairIter( DependencyList.begin() );

		for( ; DependencyList.end() != PairIter; ++PairIter )
		{
			SVObjectClass* pObjectSupplier = SVObjectManagerClass::Instance().GetObject( PairIter->first );
			SVObjectClass* pObjectClient = SVObjectManagerClass::Instance().GetObject( PairIter->second );
			SVObjectClass* pOwner = (nullptr != pObjectClient) ? pObjectClient->GetAncestor(SvPb::SVInspectionObjectType) : nullptr;
			SvVol::BasicValueObject* pGlobalConstant = dynamic_cast<SvVol::BasicValueObject*> (pObjectSupplier);
			//! Check that the client is from the same inspection
			if( pOwner == pInspection && nullptr != pGlobalConstant )
			{
				rWriter.StartElement( pGlobalConstant->GetCompleteName().c_str() );

				_variant_t Value;
				pGlobalConstant->getValue( Value );
				rWriter.WriteAttribute( SvXml::CTAG_VALUE, Value );
				Value.Clear();
				std::string Description( pGlobalConstant->getDescription() );
				//This is needed to remove any CR LF in the description
				SvUl::AddEscapeSpecialCharacters( Description, true );
				Value.SetString( Description.c_str() );
				rWriter.WriteAttribute( SvXml::CTAG_DESCRIPTION, Value );

				rWriter.EndElement();
			}
		}
	}
	rWriter.EndElement();
}

static bool ShouldExcludeFile(LPCTSTR filename)
{
	bool bRetVal = false;
	LPCTSTR exclusions[] = { ".cvi", ".cca", ".cvf", ".odc", ".ogc" }; // exclude camera files
	TCHAR ext[_MAX_EXT];
	_tsplitpath(filename, nullptr, nullptr, nullptr, ext);
	for (int i = 0;i < sizeof(exclusions) / sizeof(LPCTSTR); i++)
	{
		// Exclude certain file extensions
		if (_tcscmp(ext, exclusions[i]) == 0)
		{
			bRetVal = true;
			break;
		}
	}
	return bRetVal;
}

static bool WriteDependentFileList(SvOi::IObjectWriter& rWriter, const std::string& dstZipFile)
{
	bool Result{false};

	// remove existing file
	if (::_access(dstZipFile.c_str(), 0) == 0)
	{
		::DeleteFile(dstZipFile.c_str());
	}

	WIN32_FIND_DATA findFileData;
	HANDLE hFind = ::FindFirstFile(SvStl::GlobalPath::Inst().GetRunPath(_T("*")).c_str() , &findFileData);

	if (hFind != INVALID_HANDLE_VALUE) 
	{
		SvDef::StringVector DependencyFileNames;

		rWriter.StartElement(SvXml::CTAG_DEPENDENT_FILES);
		do
		{
			if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
				!ShouldExcludeFile(findFileData.cFileName))
			{
				// check file extension of configuration files and exclude them...
				TCHAR drive[ _MAX_DRIVE];
				TCHAR dir[MAX_PATH];
				TCHAR filename[_MAX_FNAME];
				TCHAR ext[_MAX_EXT];
				_tsplitpath(findFileData.cFileName, drive, dir, filename, ext);
				
				std::string lowercaseExt = SvUl::MakeLower(ext);

				if (lowercaseExt != SvDef::cConfigExtension)
				{
					_variant_t value;
					value.SetString(findFileData.cFileName);
					rWriter.WriteAttribute(SvXml::CTAG_FILENAME, value);

					std::string srcFile = SvStl::GlobalPath::Inst().GetRunPath().c_str();
					srcFile += "\\";
					srcFile += findFileData.cFileName;
					DependencyFileNames.emplace_back(srcFile);
				}
			}
		} while (::FindNextFile(hFind, &findFileData) != 0);
		::FindClose(hFind);

		rWriter.EndElement();
		
		Result = 0 < DependencyFileNames.size();
		if(Result)
		{
			SvUl::makeZipFile( dstZipFile, DependencyFileNames, _T(""), false );
		}
	}

	return Result;
}

static std::string GetFilenameWithoutExt(const std::string& filename)
{
	std::string result;
	std::string::size_type pos = filename.find_last_of('.');
	if (pos != std::string::npos)
	{
		result = filename.substr(0, pos).c_str();
	}
	return result;
}

static std::string RemovePath(const std::string& fname)
{
	std::string name;

	char filename[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath(fname.c_str(), nullptr, nullptr, filename, ext);

	name = filename;
	name += ext;
	return  name;
}

static void PersistDocument(uint32_t inspectionId, SvOi::IObjectWriter& rWriter)
{
	SVIPDoc* pDoc =  TheSVObserverApp.GetIPDoc(inspectionId);
	if (pDoc)
	{
		rWriter.StartElement(SvXml::CTAG_SVIPDOC);

		// Save the View Info
		pDoc->SaveViews(rWriter);

		// Save the View Placements
		pDoc->SaveViewPlacements(rWriter);

		// Save Tool Groupings
		pDoc->SaveToolGroupings(rWriter);

		rWriter.EndElement();
	}
}

#pragma region Public Methods
HRESULT SVInspectionExporter::Export(const std::string& rFileName, const std::string& rInspectionName, unsigned long p_version, bool bColor)
{
	HRESULT result = S_OK;
	try
	{
		// Write out the SVObserver version - will be checked against when importing
		SVObjectClass* pObject;
		result = SVObjectManagerClass::Instance().GetObjectByDottedName(rInspectionName.c_str(), pObject);
		if (S_OK == result)
		{
			std::string dstXmlFile = GetFilenameWithoutExt(rFileName);
			dstXmlFile += scXmlExt;
			std::string dstDependencyZipFile = GetFilenameWithoutExt(rFileName);
			dstDependencyZipFile += scDependentsZipExt;
			std::string dstZipFile = GetFilenameWithoutExt(rFileName);
			dstZipFile += (bColor) ? scColorExportExt : scExportExt;

			bool hasDependentFiles{false};
			std::ofstream os;
			os.open( dstXmlFile.c_str() );
			if (os.is_open())
			{
				SvXml::SVObjectXMLWriter writer(os);

				writer.WriteRootElement(_T("Inspection_Export"));
				writer.WriteSchema();
				writer.WriteStartOfBase();
				WriteVersion(writer, p_version);
				
				WritePPQInputs(writer, pObject);

				WriteGlobalConstants(writer, pObject);

				hasDependentFiles = WriteDependentFileList(writer, dstDependencyZipFile);

				pObject->Persist(writer);

				// Persist Document Views, Conditional History...
				PersistDocument(pObject->getObjectId(), writer);

				writer.EndElement(); // end of BaseNode
				writer.EndElement(); // end of Root Element

				os.close();
			}

			SvDef::StringVector FileNames;
			FileNames.emplace_back(dstXmlFile);
			if(hasDependentFiles)
			{
				FileNames.emplace_back(dstDependencyZipFile);
			}
			
			if(!SvUl::makeZipFile( dstZipFile, FileNames, _T(""), true))
			{
				result = E_FAIL;
			}
		}
		else
		{
			result = E_NOINTERFACE;
		}
	}
	catch (...)
	{
		result = E_FAIL;
	}
	
	return result;
}
#pragma endregion Public Methods

