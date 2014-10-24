//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc. Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInspectionExporter
//* .File Name       : $Workfile:   SVInspectionExporter.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   24 Oct 2014 11:35:58  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include <boost/config.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/foreach.hpp>
#include <boost/function.hpp>
#include <string>
#include <fstream>
#include <set>
#include <iterator>
#include "SVInspectionExporter.h"
#include "SVObjectLibrary/SVObjectXMLWriter.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVConfigurationLibrary/SVConfigurationTags.h"
#include "SVUtilityLibrary/zip.h"
#include "SVInspectionProcess.h"
#include "SVPPQObject.h"
#include "SVFileNameManagerClass.h"
#include "SVIPDoc.h"
#pragma endregion Includes

#pragma region Declarations
static LPCTSTR scRunDirectory = _T("C:\\Run");
static LPCTSTR scAllRunDirectoryFiles = _T("C:\\Run\\*");
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

	SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess *>(pObject);
	if (pInspection)
	{
		SVPPQObject* pPPQ = pInspection->GetPPQ();
		if (pPPQ)
		{
			pPPQ->PersistInputs(rWriter);
		}
	}
	rWriter.EndElement();
}

static bool ShouldExcludeFile(LPCTSTR filename)
{
	bool bRetVal = false;
	LPCTSTR exclusions[] = { ".cvi", ".cca", ".cvf", ".odc", ".ogc" }; // exclude camera files
	TCHAR ext[_MAX_EXT];
	_tsplitpath(filename, NULL, NULL, NULL, ext);
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

	HZIP hZip = NULL;
	WIN32_FIND_DATA findFileData;
	HANDLE hFind = ::FindFirstFile(scAllRunDirectoryFiles, &findFileData);

	if (hFind != INVALID_HANDLE_VALUE) 
	{
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
				
				SVString extStr = ext;
				SVString lowercaseExt = extStr.MakeLower();

				if (lowercaseExt != scSVXConfigExt)
				{
					_variant_t value;
					value.SetString(findFileData.cFileName);
					rWriter.WriteAttribute(CTAG_FILENAME, value);
			
					if (hZip == NULL)
					{
						hZip = ::CreateZip(dstZipFile.c_str(), NULL);
					}
					if (hZip != NULL)
					{
						SVString srcFile = scRunDirectory;
						srcFile += "\\";
						srcFile += findFileData.cFileName;
						::ZipAdd(hZip, findFileData.cFileName, srcFile.c_str());
					}
					else
					{
						// what to do...
					}
				}
			}
		} while (::FindNextFile(hFind, &findFileData) != 0);
		::FindClose(hFind);

		rWriter.EndElement();
	}
	if (hZip != NULL)
	{
		::CloseZip(hZip);
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
	_splitpath(fname.c_str(), NULL, NULL, filename, ext);

	name = filename;
	name += ext;
	return  name;
}

static void MakeSingleZipFile(const SVString& dstFile, const SVString& xmlFilename, const SVString& zipFilename, const SVString& exportFileExt)
{
	SVString exportFile = GetFilenameWithoutExt(dstFile);
	exportFile += exportFileExt;

	if (::_access(exportFile.c_str(), 0) == 0)
	{
		::DeleteFile(exportFile.c_str());
	}

	HZIP hZip = ::CreateZip(exportFile.c_str(), NULL);
	if (hZip != NULL)
	{
		SVString dstXmlFilename = RemovePath(xmlFilename);
		SVString dstZipFilename = RemovePath(zipFilename);

		if (::_access(xmlFilename.c_str(), 0) == 0)
		{
			::ZipAdd(hZip, dstXmlFilename.c_str(), xmlFilename.c_str());
			::DeleteFile(xmlFilename.c_str());
		}
		if (::_access(zipFilename.c_str(), 0) == 0)
		{
			::ZipAdd(hZip, dstZipFilename.c_str(), zipFilename.c_str());
			::DeleteFile(zipFilename.c_str());
		}
		::CloseZip(hZip);
	}
}

static void PersistDocument(const SVGUID& inspectionGuid, SVObjectWriter& rWriter)
{
	SVIPDoc* pDoc =  SVObjectManagerClass::Instance().GetIPDoc(inspectionGuid);
	if (pDoc)
	{
		rWriter.StartElement(CTAG_SVIPDOC);

		// Save the View Info
		pDoc->SaveViews(rWriter);

		// Save the View Placements
		pDoc->SaveViewPlacements(rWriter);

		// Save Conditional History
		pDoc->SaveConditionalHistory(rWriter);

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
		hr =  SVObjectManagerClass::Instance().GetObjectByDottedName(inspectionName.c_str(), pObject);
		if (hr == S_OK)
		{
			SVString dstXmlFile = GetFilenameWithoutExt(filename);
			dstXmlFile += scXmlExt;
			SVString dstZipFile = GetFilenameWithoutExt(filename);
			dstZipFile += scDependentsZipExt;

			std::ofstream os;
			os.open( dstXmlFile.c_str(), std::ofstream::binary );
			if (os.is_open())
			{
				SVObjectXMLWriter writer(os);

				SVString rootNodeName(_T("Inspection_Export"));
				writer.WriteRootElement(rootNodeName);
				writer.WriteSchema();
				WriteBaseNode(writer);
				WriteVersion(writer, p_version);
				
				WritePPQInputs(writer, pObject);

				WriteDependentFileList(writer, dstZipFile);

				pObject->Persist(writer);

				// Persist Document Views, Conditional History...
				PersistDocument(pObject->GetUniqueObjectID(), writer);

				writer.EndElement(); // end of BaseNode
				writer.EndElement(); // end of Root Element

				os.close();
			}
			// zip the xml and dependent zip file together
			MakeSingleZipFile(filename, dstXmlFile, dstZipFile, (bColor) ? scColorExportExt : scExportExt);
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVInspectionExporter.cpp_v  $
 * 
 *    Rev 1.3   24 Oct 2014 11:35:58   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  942
 * SCR Title:  Create new Custom2 Filter SVO-324 SVO-67 SVO-74
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Changed file stream to binary format
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 Oct 2014 17:30:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  962
 * SCR Title:  Tool Groupings Not Imported When Importing an Inspection
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised PersistDocument method to export the ToolGroupings
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   04 Feb 2014 15:22:46   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  880
 * SCR Title:  Remove .SEC
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Removed static string scSECConfigExt.
 * Removed use of scSECConfigExt from the method WriteDependentFileList.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 11:01:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   05 Mar 2013 15:30:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  817
 * SCR Title:  Import/Export of Inspections
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
