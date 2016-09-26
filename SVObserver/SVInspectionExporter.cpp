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
//Moved to precompiled header: #include <boost/assign/list_of.hpp>
//Moved to precompiled header: #include <boost/foreach.hpp>
//Moved to precompiled header: #include <boost/function.hpp>
//Moved to precompiled header: #include <string>
//Moved to precompiled header: #include <fstream>
//Moved to precompiled header: #include <set>
//Moved to precompiled header: #include <iterator>
#include "SVInspectionExporter.h"
#include "SVXMLLibrary\SVObjectXMLWriter.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVXMLLibrary/SVConfigurationTags.h"
#include "SVObjectLibrary/SVToolsetScriptTags.h"
#include "SVUtilityLibrary/ZipHelper.h"
#include "SVOMFCLibrary/StringEscape.h"
#include "SVInspectionProcess.h"
#include "SVPPQObject.h"
#include "SVObserver.h"
#include "SVIPDoc.h"
#include "RootObject.h"
#include "SVToolSet.h"
#include "SVStatusLibrary/GlobalPath.h"
#pragma endregion Includes

#pragma region Declarations
static LPCTSTR scSVXConfigExt = _T(".svx");
static LPCTSTR scDependentsZipExt = _T(".dependents.zip");
static LPCTSTR scXmlExt = _T(".xml");
static LPCTSTR scExportExt = _T(".bxp");
static LPCTSTR scColorExportExt = _T(".cxp");
#pragma endregion Declarations

static void WriteBaseNode(SVObjectXMLWriter& rWriter)
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

static void WriteVersion(SVObjectXMLWriter& rWriter, unsigned long p_version)
{
	_variant_t version(p_version);

	rWriter.StartElement(CTAG_ENVIRONMENT);
	rWriter.WriteAttribute(CTAG_VERSION_NUMBER, version);
	rWriter.EndElement();
}

static void WritePPQInputs(SVObjectXMLWriter& rWriter, SVObjectClass* pObject)
{
	rWriter.StartElement(CTAG_PPQ);

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

static void WriteGlobalConstants(SVObjectXMLWriter& rWriter, SVObjectClass* pObject)
{
	rWriter.StartElement(CTAG_GLOBAL_CONSTANTS);

	SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*> (pObject);
	if ( nullptr != pInspection && nullptr != pInspection->GetToolSet() )
	{
		BasicValueObjects::ValueVector GlobalConstantObjects;
		SVObjectVector ObjectCheckList;

		RootObject::getRootChildObjectList( GlobalConstantObjects, SvOl::FqnGlobal, 0 );
		BasicValueObjects::ValueVector::const_iterator Iter( GlobalConstantObjects.cbegin() );
		//Need to convert list to SVObjectVector
		while ( GlobalConstantObjects.cend() != Iter  && !Iter->empty() )
		{
			SVObjectClass* pGlobalConstantObject = dynamic_cast<SVObjectClass*> ( (*Iter).get() );
			if( nullptr != pGlobalConstantObject )
			{
				ObjectCheckList.push_back( pGlobalConstantObject );
			}
			++Iter;
		}

		//Only Global variables which the inspection is dependent on should be included
		SVObjectPairVector DependencyList;
		BasicValueObject* pGlobalConstant( nullptr );
		_variant_t Value;
		pInspection->GetToolSet()->GetDependentsList( ObjectCheckList, DependencyList );
		SVObjectPairVector::const_iterator PairIter( DependencyList.cbegin() );

		while( DependencyList.cend() != PairIter )
		{
			pGlobalConstant = dynamic_cast<BasicValueObject*> (PairIter->second);
			if( nullptr != pGlobalConstant )
			{
				rWriter.StartElement( pGlobalConstant->GetCompleteObjectName() );

				Value.Clear();
				pGlobalConstant->getValue( Value );
				rWriter.WriteAttribute( CTAG_VALUE, Value );
				Value.Clear();
				CString Description( pGlobalConstant->getDescription() );
				//This is needed to remove any CR LF in the description
				SvOml::AddEscapeSpecialCharacters( Description, true );
				Value.SetString( Description );
				rWriter.WriteAttribute( CTAG_DESCRIPTION, Value );

				rWriter.EndElement();
			}
			++PairIter;
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

static void WriteDependentFileList(SVObjectXMLWriter& rWriter, const SVString& dstZipFile)
{
	// remove existing file
	if (::_access(dstZipFile.c_str(), 0) == 0)
	{
		::DeleteFile(dstZipFile.c_str());
	}

	WIN32_FIND_DATA findFileData;
	HANDLE hFind = ::FindFirstFile(SvStl::GlobalPath::Inst().GetRunPath(_T("*")).c_str() , &findFileData);

	if (hFind != INVALID_HANDLE_VALUE) 
	{
		SVStringSet DependencyFileNames;

		rWriter.StartElement(CTAG_DEPENDENT_FILES);
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
				
				SVString lowercaseExt = ext;
				SvUl_SF::MakeLower(lowercaseExt);

				if (lowercaseExt != scSVXConfigExt)
				{
					_variant_t value;
					value.SetString(findFileData.cFileName);
					rWriter.WriteAttribute(CTAG_FILENAME, value);

					SVString srcFile = SvStl::GlobalPath::Inst().GetRunPath().c_str();
					srcFile += "\\";
					srcFile += findFileData.cFileName;
					DependencyFileNames.insert( srcFile );
				}
			}
		} while (::FindNextFile(hFind, &findFileData) != 0);
		::FindClose(hFind);

		rWriter.EndElement();
		
		ZipHelper::makeZipFile( dstZipFile, DependencyFileNames, false );
	}
}

static std::string GetFilenameWithoutExt(const SVString& filename)
{
	std::string result;
	SVString::size_type pos = filename.find_last_of('.');
	if (pos != SVString::npos)
	{
		result = filename.substr(0, pos).c_str();
	}
	return result;
}

static std::string RemovePath(const SVString& fname)
{
	std::string name;

	char filename[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath(fname.c_str(), nullptr, nullptr, filename, ext);

	name = filename;
	name += ext;
	return  name;
}

static void PersistDocument(const SVGUID& inspectionGuid, SVObjectWriter& rWriter)
{
	SVIPDoc* pDoc =  TheSVObserverApp.GetIPDoc(inspectionGuid);
	if (pDoc)
	{
		rWriter.StartElement(CTAG_SVIPDOC);

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
HRESULT SVInspectionExporter::Export(const SVString& filename, const SVString& inspectionName, unsigned long p_version, bool bColor)
{
	HRESULT hr = S_OK;
	try
	{
		// Write out the SVObserver version - will be checked against when importing
		SVObjectClass* pObject;
		hr = SVObjectManagerClass::Instance().GetObjectByDottedName(inspectionName.c_str(), pObject);
		if (S_OK == hr)
		{
			SVString dstXmlFile = GetFilenameWithoutExt(filename);
			dstXmlFile += scXmlExt;
			SVString dstDependencyZipFile = GetFilenameWithoutExt(filename);
			dstDependencyZipFile += scDependentsZipExt;
			SVString dstZipFile = GetFilenameWithoutExt(filename);
			dstZipFile += (bColor) ? scColorExportExt : scExportExt;

			std::ofstream os;
			os.open( dstXmlFile.c_str() );
			if (os.is_open())
			{
				SVObjectXMLWriter writer(os);

				writer.WriteRootElement(_T("Inspection_Export"));
				writer.WriteSchema();
				writer.WriteStartOfBase();
				WriteVersion(writer, p_version);
				
				WritePPQInputs(writer, pObject);

				WriteGlobalConstants(writer, pObject);

				WriteDependentFileList(writer, dstDependencyZipFile);

				pObject->Persist(writer);

				// Persist Document Views, Conditional History...
				PersistDocument(pObject->GetUniqueObjectID(), writer);

				writer.EndElement(); // end of BaseNode
				writer.EndElement(); // end of Root Element

				os.close();
			}

			SVStringSet FileNames;
			FileNames.insert( dstXmlFile );
			FileNames.insert( dstDependencyZipFile );

			ZipHelper::makeZipFile( dstZipFile, FileNames, true );
		}
		else
		{
			hr = E_NOINTERFACE;
		}
	}
	catch (...)
	{
		hr = S_FALSE;
	}
	
	return hr;
}
#pragma endregion Public Methods

