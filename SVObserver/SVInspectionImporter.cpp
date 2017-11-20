//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc. Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInspectionImporter
//* .File Name       : $Workfile:   SVInspectionImporter.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   19 Dec 2014 04:06:24  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/any.hpp>
//Moved to precompiled header: #include <iterator>
//Moved to precompiled header: #include <deque>
#include "shellapi.h"
#include "SVInspectionImporter.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectLibrary/SVObjectBuilder.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#include "SVXMLLibrary/SVConfigurationTags.h"
#include "SVUtilityLibrary/ZipHelper.h"
#include "SVInspectionTreeParser.h"
#include "SVObjectScriptParser.h"
#include "SVParserProgressDialog.h"
#include "SVXMLLibrary/SVNavigateTree.h"
#include "SVInspectionProcess.h"

#include "SVConfigurationObject.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "RootObject.h"
#include "SVXMLLibrary/SaxXMLHandler.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVXMLLibrary/SaxExtractPropertiesHandler.h"
#include "Definitions/StringTypeDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

static LPCTSTR scImportNewExt = _T(".new.xml");
static LPCTSTR scDependentsZipExt = _T(".dependents.zip");
static LPCTSTR scTransformProgram = _T("SVImportTransform.exe");
static LPCTSTR scExportExt = _T(".bxp");
static LPCTSTR scColorExportExt = _T(".cxp");
static LPCTSTR scZipExt = _T(".zip");
static LPCTSTR scXMLExt = _T(".xml");

static bool isZipFile(const std::string& filename)
{
	bool bRetVal = false;
	char ext[_MAX_EXT];
	_splitpath(filename.c_str(), nullptr, nullptr, nullptr, ext);
	if (0 == strcmp(ext, scZipExt))
	{
		bRetVal = true;
	}
	return bRetVal;
}

static bool isXMLFile(const std::string& filename)
{
	bool bRetVal = false;
	char ext[_MAX_EXT];
	_splitpath(filename.c_str(), nullptr, nullptr, nullptr, ext);
	if (0 == strcmp(ext, scXMLExt))
	{
		bRetVal = true;
	}
	return bRetVal;
}

static bool isExportFile(const std::string& filename)
{
	bool bRetVal = false;
	char ext[_MAX_EXT];
	_splitpath(filename.c_str(), nullptr, nullptr, nullptr, ext);
	if (0 == strcmp(ext, scExportExt) || 0 == strcmp(ext, scColorExportExt) )
	{
		bRetVal = true;
	}
	return bRetVal;
}

static int LaunchTransform(const char* inFilename, const char* outFilename, const char* inspectionName)
{
	DWORD exitCode = -SvStl::Err_15018;

	std::string parameters;
	parameters = _T("/in=");
	parameters += _T("\""); // quote the filename
	parameters += inFilename;
	parameters += _T("\"");

	parameters += _T(" /out=");
	parameters += _T("\""); // quote the filename
	parameters += outFilename;
	parameters += _T("\"");

	parameters += _T(" /name=");
	parameters += _T("\""); // quote the inspection name
	parameters += inspectionName;
	parameters += _T("\"");

	// Get Current Exe Directory
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_PATH];
	TCHAR exePath[_MAX_PATH];
	::GetModuleFileName(nullptr, exePath, sizeof(exePath) / sizeof(TCHAR));
	_tsplitpath(exePath, drive, dir, nullptr, nullptr);
	std::string progName = drive;
	progName += dir;
	progName += scTransformProgram;

	SHELLEXECUTEINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cbSize = sizeof(SHELLEXECUTEINFO);
	si.fMask = SEE_MASK_NOCLOSEPROCESS;
	si.hwnd = nullptr;
	si.lpVerb = nullptr;
	si.lpFile = progName.c_str();
	si.lpParameters = parameters.c_str();
	si.lpDirectory = nullptr;
	si.nShow = SW_HIDE;
	si.hInstApp = nullptr;	
	if (::ShellExecuteEx(&si))
	{
		::WaitForSingleObject(si.hProcess, INFINITE);
		::GetExitCodeProcess(si.hProcess, &exitCode);
	}
	return exitCode;
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

template<typename SVTreeType, typename Insertor>
static bool ImportPPQInputs(SVTreeType& rTree, Insertor insertor)
{
	bool bOk = true;
	
	SVTreeType::SVBranchHandle hItem;
	if (SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::CTAG_PPQ, nullptr, hItem))
	{
		SVTreeType::SVBranchHandle hItemInputs;
		if (SvXml::SVNavigateTree::GetItemBranch(rTree, SvXml::CTAG_INPUT, hItem , hItemInputs))
		{
			SVTreeType::SVBranchHandle htiDataChild( nullptr );
				
			htiDataChild = rTree.getFirstBranch(hItemInputs);

			while (bOk && nullptr != htiDataChild )
			{
				std::string DataName( rTree.getBranchName(htiDataChild) );
				
				long l_PPQPosition = -1;
				_variant_t svValue;
				std::string Type;
				long l_Index;

				bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_IO_TYPE, htiDataChild, svValue);
				if (bOk)
				{
					Type = SvUl::createStdString( svValue );
				}
				bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_ITEM_NAME, htiDataChild, svValue);
				if (bOk)
				{
					DataName = SvUl::createStdString( svValue );
				}
				bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_PPQ_POSITION, htiDataChild, svValue);
				if (bOk)
				{
					l_PPQPosition = svValue;
				}
				if (l_PPQPosition >= 0)
				{
					// This means it is a Digital input
					if( _T("Digital") == Type )
					{
						insertor = boost::any(SVImportedInput(DataName, l_PPQPosition));
					}
					// This means it is a Digital input
					else if( _T("Remote") == Type )
					{								
						_variant_t l_Variant = 0.0;

						bOk = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_REMOTE_INDEX, htiDataChild, svValue);
						if (bOk)
						{
							l_Index = svValue;
						}
						if (SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_REMOTE_INITIAL_VALUE, htiDataChild, svValue))
						{
							l_Variant = svValue;
						}
						// Add Remote Input
						insertor = boost::any(SVImportedRemoteInput(DataName, l_PPQPosition, l_Index, l_Variant));
					}
				}
				htiDataChild = rTree.getNextBranch(hItemInputs, htiDataChild);
			}
		}
	}
	return bOk;
}

template<typename SVTreeType>
static bool importGlobalConstants( SVTreeType& rTree, SvDef::GlobalConstantDataSet& rImportedGlobals )
{
	bool Result( true );

	SVTreeType::SVBranchHandle hItem;
	if (SvXml::SVNavigateTree::GetItemBranch( rTree, SvXml::CTAG_GLOBAL_CONSTANTS, nullptr, hItem ))
	{
		SVTreeType::SVBranchHandle hItemChild( nullptr );

		hItemChild = rTree.getFirstBranch( hItem );

		while( Result && nullptr != hItemChild )
		{
			SvDef::GlobalConstantData GlobalData;
			_variant_t Value;

			GlobalData.m_DottedName = rTree.getBranchName( hItemChild );

			Result = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_VALUE, hItemChild, Value );
			if( Result )
			{
				GlobalData.m_Value = Value;
			}
			Value.Clear();
			Result = SvXml::SVNavigateTree::GetItem( rTree, SvXml::CTAG_DESCRIPTION, hItemChild, Value );
			if( Result )
			{
				std::string Description = SvUl::createStdString( Value.bstrVal );
				//This is needed to insert any CR LF in the description which were replaced while saving
				SvUl::RemoveEscapedSpecialCharacters( Description, true );
				GlobalData.m_Description = Description;
			}
			rImportedGlobals.insert( GlobalData );
			
			hItemChild = rTree.getNextBranch( hItem, hItemChild );
		}
	}

	return Result;
}

static void checkGlobalConstants( const SvDef::GlobalConstantDataSet& rImportedGlobals, SvDef::GlobalConflictPairVector& rGlobalConflicts )
{
	SvDef::GlobalConstantDataSet CurrentGlobals;

	BasicValueObjects::ValueVector GlobalObjects;

	RootObject::getRootChildObjectList( GlobalObjects, SvDef::FqnGlobal, 0 );
	BasicValueObjects::ValueVector::const_iterator Iter( GlobalObjects.begin() );
	while( GlobalObjects.end() != Iter && !Iter->empty() )
	{
		SvDef::GlobalConstantData GlobalData;
		//Important here we intensionally do not fill the m_Guid value
		GlobalData.m_DottedName = (*Iter)->GetCompleteName();
		(*Iter)->getValue( GlobalData.m_Value );
		GlobalData.m_Description = (*Iter)->getDescription();
		CurrentGlobals.insert( GlobalData );

		++Iter;
	}

	SvDef::GlobalConstantDataSet GlobalDiffs;

	std::set_difference( rImportedGlobals.begin(), rImportedGlobals.end(), CurrentGlobals.begin(), CurrentGlobals.end(), std::inserter( GlobalDiffs, GlobalDiffs.begin() ) );

	SvDef::GlobalConstantDataSet::const_iterator DiffIter( GlobalDiffs.cbegin() );
	while( GlobalDiffs.cend() != DiffIter  )
	{
		BasicValueObjectPtr pGlobalConstant;
		pGlobalConstant = RootObject::getRootChildObjectValue( DiffIter->m_DottedName.c_str() );

		//If the Global Constant exists then conflict otherwise we can create it
		if( pGlobalConstant.empty() )
		{
			pGlobalConstant = RootObject::setRootChildValue( DiffIter->m_DottedName.c_str(), DiffIter->m_Value );
			if( !pGlobalConstant.empty() )
			{
				pGlobalConstant->setDescription( DiffIter->m_Description.c_str() );
				if( DiffIter->m_Value.vt == VT_BSTR)
				{
					pGlobalConstant->SetObjectAttributesAllowed( SvDef::SV_SELECTABLE_FOR_EQUATION, SvOi::SetAttributeType::RemoveAttribute );
				}
			}
		}
		else
		{
			SvDef::GlobalConstantData GlobalData;

			GlobalData.m_Guid = pGlobalConstant->GetUniqueObjectID();
			GlobalData.m_DottedName = pGlobalConstant->GetCompleteName();
			pGlobalConstant->getValue( GlobalData.m_Value );
			//Default is that the current Global Constant is selected
			GlobalData.m_Selected = true;
			rGlobalConflicts.push_back( std::make_pair( GlobalData,  *DiffIter ) );
		}

		++DiffIter;
	}
}

typedef std::insert_iterator<SVImportedInputList> InputListInsertor;

HRESULT LoadInspectionXml(const std::string& filename, const std::string& zipFilename, const std::string& inspectionName, const std::string& cameraName, SVImportedInspectionInfo& inspectionInfo, SvDef::GlobalConflictPairVector& rGlobalConflicts, SVIProgress& rProgress, int& currentOp, int numOperations)
{
	_bstr_t bstrRevisionHistory;
	_bstr_t bstrChangedNode;
	CWnd* pParentWnd = AfxGetApp()->GetMainWnd();

	SvXml::SVXMLMaterialsTree XmlTree;
	SvDef::GlobalConstantDataSet ImportedGlobals;

	rProgress.UpdateText(_T("Loading Inspection XML..."));

	SvXml::SaxXMLHandler<SVTreeType>  SaxHandler;
	HRESULT	hr =  SaxHandler.BuildFromXMLFile(&XmlTree, filename.c_str());
	
	
	if (SUCCEEDED(hr))
	{
		rProgress.UpdateText(_T("Importing Dependent Files..."));
		rProgress.UpdateProgress(++currentOp, numOperations);

		SvDef::StringSet Files;
		ZipHelper::unzipAll( zipFilename, std::string( SvStl::GlobalPath::Inst().GetRunPath().c_str() ), Files );

		rProgress.UpdateText(_T("Importing PPQ Inputs..."));
		rProgress.UpdateProgress(++currentOp, numOperations);

		InputListInsertor insertor(inspectionInfo.m_inputList, inspectionInfo.m_inputList.begin());
		ImportPPQInputs(XmlTree, insertor);

		rProgress.UpdateText(_T("Importing Global Constants..."));
		rProgress.UpdateProgress(++currentOp, numOperations);

		importGlobalConstants( XmlTree, ImportedGlobals );

		rProgress.UpdateText(_T("Creating Inspection object..."));
		rProgress.UpdateProgress(++currentOp, numOperations);

		SvXml::SVXMLMaterialsTree::SVBranchHandle hItem;
		if ( SvXml::SVNavigateTree::GetItemBranch( XmlTree, SvXml::CTAG_INSPECTION_PROCESS, nullptr, hItem ) )
		{
			SvXml::SVXMLMaterialsTree::SVBranchHandle hItemToolset;
			if ( SvXml::SVNavigateTree::GetItemBranch( XmlTree, SvXml::CTAG_TOOLSET_SET, hItem, hItemToolset ) )
			{
				rProgress.UpdateProgress(++currentOp, numOperations);

				// Set the Caption
				SVParserProgressDialog l_ParserProgressDialog(_T( "Loading Inspection(s) ..." ));

				unsigned long parserHandle = SVObjectScriptParserClass::GetParserHandle();

				// Create Inspection process - leave unattached for now
				hr = SVInspectionTreeParser< SvXml::SVXMLMaterialsTree >::CreateInspectionObject(inspectionInfo.m_inspectionGuid, XmlTree, hItem);
				if (S_OK == hr)
				{
					rProgress.UpdateText(_T("Creating Toolset objects..."));
					rProgress.UpdateProgress(++currentOp, numOperations);

					SVObjectClass* pObject = nullptr;
					SVObjectManagerClass::Instance().GetObjectByIdentifier(inspectionInfo.m_inspectionGuid, pObject);
					SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*>(pObject);
					if ( nullptr != pInspection )
					{
						pInspection->SetToolsetImage(cameraName.c_str());
						pInspection->CreateInspection(inspectionName.c_str());

						rProgress.UpdateText(_T("Setting Inspection Camera name..."));
						rProgress.UpdateProgress(++currentOp, numOperations);

						SVConfigurationObject::updateConfTreeToNewestVersion(XmlTree, hItemToolset);

						// Launch parser progress
						SVObjectScriptParserClass* pParser = new SVObjectScriptParserClass(new SVInspectionTreeParser< SvXml::SVXMLMaterialsTree >(XmlTree, hItemToolset, parserHandle, inspectionInfo.m_inspectionGuid, pInspection, &l_ParserProgressDialog));
						if ( nullptr != pParser )
						{
							// Set the Parser Object
							l_ParserProgressDialog.AddParser(parserHandle, pParser);

							// Show the Dialog
							l_ParserProgressDialog.DoModal();

							pInspection->ConnectAllInputs();

							rProgress.UpdateText(_T("Parsing Complete."));
							rProgress.UpdateProgress(++currentOp, numOperations);
						}
						else
						{
							hr = -SvStl::Err_15013;
						}
					}
					else
					{
						hr = -SvStl::Err_15019;
					}
				}
			}
			else
			{
				hr = -SvStl::Err_15014;
			}
		}
		else
		{
			hr = -SvStl::Err_15015;
		}

	}

	if( SUCCEEDED(hr) )
	{
		checkGlobalConstants( ImportedGlobals, rGlobalConflicts);

		SvXml::SVXMLMaterialsTree::SVBranchHandle Root( XmlTree.getRoot() );
		SvXml::SVXMLMaterialsTree::SVBranchHandle IPDocItem( nullptr );

		if( SvXml::SVNavigateTree::FindBranch( XmlTree, Root, SVFindPredicate( XmlTree, SvXml::CTAG_SVIPDOC ), IPDocItem ) )
		{
			inspectionInfo.m_materialsTree.clear();
			inspectionInfo.m_materialsTree = *IPDocItem;
		}
		else
		{
			hr = E_FAIL;
		}
	}
	
	return hr;
}

HRESULT SVInspectionImporter::Import(const std::string& filename, const std::string& inspectionName, const std::string& cameraName, SVImportedInspectionInfo& inspectionInfo, SvDef::GlobalConflictPairVector& rGlobalConflicts, SVIProgress& rProgress)
{
	HRESULT hr = S_OK;
	::CoInitialize(nullptr);

	std::string inFilename = filename;
	std::string zipFilename = GetFilenameWithoutExt(inFilename);
	zipFilename += scDependentsZipExt;

	std::string outFilename = SvStl::GlobalPath::Inst().GetRunPath().c_str();
	outFilename +=  _T("\\");
	outFilename += inspectionName;
	outFilename +=  scImportNewExt;

	int currentOp = 0;
	int numOperations = 10;
	rProgress.UpdateProgress(++currentOp, numOperations);

	SvDef::StringSet list;

	// Deal with single zip file
	if (isExportFile(inFilename))
	{
		ZipHelper::unzipAll( inFilename, std::string( SvStl::GlobalPath::Inst().GetRunPath().c_str() ), list );
		for (SvDef::StringSet::const_iterator it = list.begin();it != list.end();++it)
		{
			if (isXMLFile(*it))
			{
				inFilename = (*it);
			}
			else if (isZipFile(*it))
			{
				zipFilename = (*it);
			}
		}
	}
	rProgress.UpdateText(_T("Transforming Inspection..."));

	int rc = LaunchTransform(inFilename.c_str(), outFilename.c_str(), inspectionName.c_str());
	if (rc == 0)
	{
		rProgress.UpdateProgress(++currentOp, numOperations);
		hr = LoadInspectionXml(outFilename, zipFilename, inspectionName, cameraName, inspectionInfo, rGlobalConflicts, rProgress, currentOp, numOperations);
		//
		::DeleteFile(outFilename.c_str());

		rProgress.UpdateProgress(++currentOp, numOperations);
		rProgress.UpdateText(_T("Import Complete."));
	}
	else
	{
		hr = -SvStl::Err_15017;
	}

	// Deal with single zip file..
	if (0 != list.size())
	{
		// cleanup
		for (SvDef::StringSet::const_iterator it = list.begin();it != list.end();++it)
		{
			::DeleteFile(it->c_str());
		}
	}
	::CoUninitialize();
	return hr;
}



HRESULT SVInspectionImporter::GetProperties(const std::string& rFileName, long& rNewDisableMethod, long& rEnableAuxExtents, unsigned long& rVersionNumber)
{
	HRESULT hr = S_OK;
	::CoInitialize(nullptr);

	std::string inFileName = rFileName;
	std::string zipFilename = GetFilenameWithoutExt(rFileName);
	zipFilename += scDependentsZipExt;

	SvDef::StringSet list;

	// Deal with single zip file
	if (isExportFile(inFileName))
	{
		ZipHelper::unzipAll( inFileName, std::string( SvStl::GlobalPath::Inst().GetRunPath().c_str() ), list );
		for (SvDef::StringSet::const_iterator it = list.begin();it != list.end();++it)
		{
			if (isXMLFile(*it))
			{
				inFileName = (*it);
			}
			else if (isZipFile(*it))
			{
				zipFilename = (*it);
			}
		}
	}
	
	SvXml::SaxExtractPropertiesHandler   SaxExtractHandler;
	hr =   SaxExtractHandler.ExtractProperties(inFileName.c_str(), rNewDisableMethod, rEnableAuxExtents, rVersionNumber);

	// Deal with single zip file..
	if( 0 != list.size())
	{
		// cleanup
		for (SvDef::StringSet::const_iterator it = list.begin();it != list.end();++it)
		{
			::DeleteFile(it->c_str());
		}
	}
	::CoUninitialize();
	return hr;
}

