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
#import <msxml6.dll>					// MSXML
#include "shellapi.h"
#include "SVInspectionImporter.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectLibrary/SVObjectBuilder.h"
#include "SVXMLLibrary/SVXMLClass.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#include "SVConfigurationLibrary/SVConfigurationTags.h"
#include "SVUtilityLibrary/ZipHelper.h"
#include "SVInspectionTreeParser.h"
#include "SVObjectScriptParser.h"
#include "SVParserProgressDialog.h"
#include "SVXMLLibrary/SVNavigateTree.h"
#include "SVInspectionProcess.h"

#include "SVConfigurationObject.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "SVGlobal.h"
#include "RootObject.h"
#include "SVXMLLibrary/SaxXMLHandler.h"
#include "SVStatusLibrary/GlobalPath.h"
#pragma endregion Includes


static LPCTSTR scImportNewExt = _T(".new.xml");
static LPCTSTR scDependentsZipExt = _T(".dependents.zip");
static LPCTSTR scTransformProgram = _T("SVImportTransform.exe");
static LPCTSTR scExportExt = _T(".bxp");
static LPCTSTR scColorExportExt = _T(".cxp");
static LPCTSTR scZipExt = _T(".zip");
static LPCTSTR scXMLExt = _T(".xml");

static bool isZipFile(const SVString& filename)
{
	bool bRetVal = false;
	char ext[_MAX_EXT];
	_splitpath(filename.c_str(), NULL, NULL, NULL, ext);
	if (strcmp(ext, scZipExt) == 0)
	{
		bRetVal = true;
	}
	return bRetVal;
}

static bool isXMLFile(const SVString& filename)
{
	bool bRetVal = false;
	char ext[_MAX_EXT];
	_splitpath(filename.c_str(), NULL, NULL, NULL, ext);
	if (strcmp(ext, scXMLExt) == 0)
	{
		bRetVal = true;
	}
	return bRetVal;
}

static bool isExportFile(const SVString& filename)
{
	bool bRetVal = false;
	char ext[_MAX_EXT];
	_splitpath(filename.c_str(), NULL, NULL, NULL, ext);
	if (strcmp(ext, scExportExt) == 0 || strcmp(ext, scColorExportExt) == 0)
	{
		bRetVal = true;
	}
	return bRetVal;
}

static int LaunchTransform(const char* inFilename, const char* outFilename, const char* inspectionName)
{
	DWORD exitCode = -SvOi::Err_15018;

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
	::GetModuleFileName(NULL, exePath, sizeof(exePath) / sizeof(TCHAR));
	_tsplitpath(exePath, drive, dir, NULL, NULL);
	std::string progName = drive;
	progName += dir;
	progName += scTransformProgram;

	SHELLEXECUTEINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cbSize = sizeof(SHELLEXECUTEINFO);
	si.fMask = SEE_MASK_NOCLOSEPROCESS;
	si.hwnd = NULL;
	si.lpVerb = NULL;
	si.lpFile = progName.c_str();
	si.lpParameters = parameters.c_str();
	si.lpDirectory = NULL;
	si.nShow = SW_HIDE;
	si.hInstApp = NULL;	
	if (::ShellExecuteEx(&si))
	{
		::WaitForSingleObject(si.hProcess, INFINITE);
		::GetExitCodeProcess(si.hProcess, &exitCode);
	}
	return exitCode;
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

template<typename SVTreeType, typename Insertor>
static bool ImportPPQInputs(SVTreeType& rTree, Insertor insertor)
{
	bool bOk = true;
	
	SVTreeType::SVBranchHandle hItem;
	if (SVNavigateTree::GetItemBranch(rTree, CTAG_PPQ, NULL, hItem))
	{
		SVTreeType::SVBranchHandle hItemInputs;
		if (SVNavigateTree::GetItemBranch(rTree, CTAG_INPUT, hItem , hItemInputs))
		{
			SVTreeType::SVBranchHandle htiDataChild( nullptr );
				
			htiDataChild = rTree.getFirstBranch(hItemInputs);

			while (bOk && nullptr != htiDataChild )
			{
				SVString DataName( rTree.getBranchName(htiDataChild) );
				
				long l_PPQPosition = -1;
				_variant_t svValue;
				CString strType;
				long l_Index;

				bOk = SVNavigateTree::GetItem(rTree, CTAG_IO_TYPE, htiDataChild, svValue);
				if (bOk)
				{
					strType = static_cast<LPCTSTR>(static_cast<_bstr_t>(svValue));
				}
				bOk = SVNavigateTree::GetItem(rTree, CTAG_ITEM_NAME, htiDataChild, svValue);
				if (bOk)
				{
					DataName = SvUl_SF::createSVString(svValue);
				}
				bOk = SVNavigateTree::GetItem(rTree, CTAG_PPQ_POSITION, htiDataChild, svValue);
				if (bOk)
				{
					l_PPQPosition = svValue;
				}
				if (l_PPQPosition >= 0)
				{
					// This means it is a Digital input
					if( _T("Digital") == strType )
					{
						insertor = boost::any(SVImportedInput(DataName, l_PPQPosition));
					}
					// This means it is a Digital input
					else if (_T("Remote") == strType)
					{								
						_variant_t l_Variant = 0.0;

						bOk = SVNavigateTree::GetItem(rTree, CTAG_REMOTE_INDEX, htiDataChild, svValue);
						if (bOk)
						{
							l_Index = svValue;
						}
						if (SVNavigateTree::GetItem(rTree, CTAG_REMOTE_INITIAL_VALUE, htiDataChild, svValue))
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
static bool importGlobalConstants( SVTreeType& rTree, SvOi::GlobalConstantDataSet& rImportedGlobals )
{
	bool Result( true );

	SVTreeType::SVBranchHandle hItem;
	if (SVNavigateTree::GetItemBranch( rTree, CTAG_GLOBAL_CONSTANTS, NULL, hItem ))
	{
		SVTreeType::SVBranchHandle hItemChild( nullptr );

		hItemChild = rTree.getFirstBranch( hItem );

		while( Result && nullptr != hItemChild )
		{
			SvOi::GlobalConstantData GlobalData;
			_variant_t Value;

			GlobalData.m_DottedName = rTree.getBranchName( hItemChild );

			Result = SVNavigateTree::GetItem( rTree, CTAG_VALUE, hItemChild, Value );
			if( Result )
			{
				GlobalData.m_Value = Value;
			}
			Value.Clear();
			Result = SVNavigateTree::GetItem( rTree, CTAG_DESCRIPTION, hItemChild, Value );
			if( Result )
			{
				CString Description;
				Description = Value.bstrVal;
				//This is needed to insert any CR LF in the description which were replaced while saving
				::SVRemoveEscapedSpecialCharacters( Description, true );
				GlobalData.m_Description = Description;
			}
			rImportedGlobals.insert( GlobalData );
			
			hItemChild = rTree.getNextBranch( hItem, hItemChild );
		}
	}

	return Result;
}

static void checkGlobalConstants( const SvOi::GlobalConstantDataSet& rImportedGlobals, SvOi::GlobalConflictPairVector& rGlobalConflicts )
{
	SvOi::GlobalConstantDataSet CurrentGlobals;

	BasicValueObjects::ValueVector GlobalObjects;

	RootObject::getRootChildObjectList( GlobalObjects, SvOl::FqnGlobal, 0 );
	BasicValueObjects::ValueVector::const_iterator Iter( GlobalObjects.begin() );
	while( GlobalObjects.end() != Iter && !Iter->empty() )
	{
		SvOi::GlobalConstantData GlobalData;
		//Important here we intensionally do not fill the m_Guid value
		GlobalData.m_DottedName = (*Iter)->GetCompleteObjectName();
		(*Iter)->getValue( GlobalData.m_Value );
		GlobalData.m_Description = (*Iter)->getDescription();
		CurrentGlobals.insert( GlobalData );

		++Iter;
	}

	SvOi::GlobalConstantDataSet GlobalDiffs;

	std::set_difference( rImportedGlobals.begin(), rImportedGlobals.end(), CurrentGlobals.begin(), CurrentGlobals.end(), std::inserter( GlobalDiffs, GlobalDiffs.begin() ) );

	SvOi::GlobalConstantDataSet::const_iterator DiffIter( GlobalDiffs.cbegin() );
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
					pGlobalConstant->ObjectAttributesAllowedRef() &= ~SV_SELECTABLE_FOR_EQUATION;
				}
			}
		}
		else
		{
			SvOi::GlobalConstantData GlobalData;

			GlobalData.m_Guid = pGlobalConstant->GetUniqueObjectID();
			GlobalData.m_DottedName = pGlobalConstant->GetCompleteObjectName();
			pGlobalConstant->getValue( GlobalData.m_Value );
			//Default is that the current Global Constant is selected
			GlobalData.m_Selected = true;
			rGlobalConflicts.push_back( std::make_pair( GlobalData,  *DiffIter ) );
		}

		++DiffIter;
	}
}

typedef std::insert_iterator<SVImportedInputList> InputListInsertor;

HRESULT LoadInspectionXml(const SVString& filename, const SVString& zipFilename, const SVString& inspectionName, const SVString& cameraName, SVImportedInspectionInfo& inspectionInfo, SvOi::GlobalConflictPairVector& rGlobalConflicts, SVIProgress& rProgress, int& currentOp, int numOperations)
{
	_bstr_t bstrRevisionHistory;
	_bstr_t bstrChangedNode;
	CWnd* pParentWnd = AfxGetApp()->GetMainWnd();

	SvXml::SVXMLMaterialsTree XmlTree;
	SvOi::GlobalConstantDataSet ImportedGlobals;

	rProgress.UpdateText(_T("Loading Inspection XML..."));

	CA2W  pwXmlFilename(filename.c_str()); 
	SvXml::SaxXMLHandler<SVTreeType>  SaxHandler;
	HRESULT	hr =  SaxHandler.BuildFromXMLFile(&XmlTree, pwXmlFilename);
	
	
	if (SUCCEEDED(hr))
	{
		rProgress.UpdateText(_T("Importing Dependent Files..."));
		rProgress.UpdateProgress(++currentOp, numOperations);

		SVStringSet Files;
		ZipHelper::unzipAll( zipFilename, SVString( SvStl::GlobalPath::Inst().GetRunPath().c_str() ), Files );

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
		if ( SVNavigateTree::GetItemBranch( XmlTree, CTAG_INSPECTION_PROCESS, nullptr, hItem ) )
		{
			SvXml::SVXMLMaterialsTree::SVBranchHandle hItemToolset;
			if ( SVNavigateTree::GetItemBranch( XmlTree, CTAG_TOOLSET_SET, hItem, hItemToolset ) )
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

							::SVSendMessage( pInspection, SVM_CONNECT_ALL_INPUTS, NULL, NULL );

							rProgress.UpdateText(_T("Parsing Complete."));
							rProgress.UpdateProgress(++currentOp, numOperations);
						}
						else
						{
							hr = -SvOi::Err_15013;
						}
					}
					else
					{
						hr = -SvOi::Err_15019;
					}
				}
			}
			else
			{
				hr = -SvOi::Err_15014;
			}
		}
		else
		{
			hr = -SvOi::Err_15015;
		}

	}

	if( SUCCEEDED(hr) )
	{
		checkGlobalConstants( ImportedGlobals, rGlobalConflicts);

		SvXml::SVXMLMaterialsTree::SVBranchHandle Root( XmlTree.getRoot() );
		SvXml::SVXMLMaterialsTree::SVBranchHandle IPDocItem( nullptr );

		if( SVNavigateTree::FindBranch( XmlTree, Root, SVFindPredicate( XmlTree, CTAG_SVIPDOC ), IPDocItem ) )
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

HRESULT SVInspectionImporter::Import(const SVString& filename, const SVString& inspectionName, const SVString& cameraName, SVImportedInspectionInfo& inspectionInfo, SvOi::GlobalConflictPairVector& rGlobalConflicts, SVIProgress& rProgress)
{
	HRESULT hr = S_OK;
	::CoInitialize(NULL);

	SVString inFilename = filename;
	SVString zipFilename = GetFilenameWithoutExt(inFilename);
	zipFilename += scDependentsZipExt;

	SVString outFilename = SvStl::GlobalPath::Inst().GetRunPath().c_str();
	outFilename +=  _T("\\");
	outFilename += inspectionName;
	outFilename +=  scImportNewExt;

	int currentOp = 0;
	int numOperations = 10;
	rProgress.UpdateProgress(++currentOp, numOperations);

	SVStringSet list;

	// Deal with single zip file
	if (isExportFile(inFilename))
	{
		ZipHelper::unzipAll( inFilename, SVString( SvStl::GlobalPath::Inst().GetRunPath().c_str() ), list );
		for (SVStringSet::const_iterator it = list.begin();it != list.end();++it)
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
		hr = -SvOi::Err_15017;
	}

	// Deal with single zip file..
	if (0 != list.size())
	{
		// cleanup
		for (SVStringSet::const_iterator it = list.begin();it != list.end();++it)
		{
			::DeleteFile(it->c_str());
		}
	}
	::CoUninitialize();
	return hr;
}

static HRESULT ExtractProperties(const SVString& filename, long& rNewDisableMethod, long& rEnableAuxExtents, unsigned long& rVersionNumber)
{
	SVDOMClass xml;
	
	HRESULT hr = xml.Initialize(0);
	if (hr == S_OK)
	{
		hr = xml.CopyXMLFileToDOM(_bstr_t(filename.c_str())); 
		if (hr == S_OK)
		{
			_bstr_t bstrQueryNameSpace = L"xmlns:svr1=\"x-schema:#SVR00001\"";
			hr = xml.SetQueryNameSpace(bstrQueryNameSpace);
			if (hr == S_OK)
			{
				SVXML::IXMLDOMElementPtr root;
				xml.GetRootNode(root);
				SVXML::IXMLDOMNodeListPtr pXMLDomNodeList = root->selectNodes("//svr1:NODE[@Name='Inspection Process']/svr1:DATA[@Name='NewDisableMethod'] | //svr1:NODE[@Name='Inspection Process']/svr1:DATA[@Name='EnableAuxiliaryExtent'] | //svr1:NODE[@Name='Environment']/svr1:DATA[@Name='VersionNumber']");
				for (int i = 0;i < pXMLDomNodeList->length;i++)
				{
					SVXML::IXMLDOMNodePtr nodePtr;
					hr = pXMLDomNodeList->get_item(i, &nodePtr);
					if (hr == S_OK)
					{
						SVXML::IXMLDOMNamedNodeMapPtr attributesPtr = nodePtr->attributes;
						if (attributesPtr)
						{
							SVXML::IXMLDOMNodePtr attrNamePtr = attributesPtr->getNamedItem(L"Name");
							if (attrNamePtr)
							{
								_bstr_t bstrNodeName = attrNamePtr->GetnodeValue();
								VARIANT var;
								
								hr = xml.GetDOMNodeElementValue(nodePtr, &var);
								if (hr == S_OK)
								{
									_variant_t value;
									value.Attach(var);

									if (bstrNodeName == _bstr_t(L"NewDisableMethod"))
									{
										rNewDisableMethod = value.lVal;
									}
									else if (bstrNodeName == _bstr_t(L"EnableAuxiliaryExtent"))
									{
										rEnableAuxExtents = value.lVal;
									}
									else if (bstrNodeName == _bstr_t(L"VersionNumber"))
									{
										rVersionNumber = value.ulVal;
									}
								}
							}
							else
							{
								hr = E_FAIL;
							}
						}
						else
						{
							hr = E_FAIL;
						}
					}
				}
			}
		}
	}
	return hr;
}

HRESULT SVInspectionImporter::GetProperties(const SVString& filename, long& rNewDisableMethod, long& rEnableAuxExtents, unsigned long& rVersionNumber)
{
	HRESULT hr = S_OK;
	::CoInitialize(NULL);

	SVString inFilename = filename;
	SVString zipFilename = GetFilenameWithoutExt(inFilename);
	zipFilename += scDependentsZipExt;

	SVStringSet list;

	// Deal with single zip file
	if (isExportFile(inFilename))
	{
		ZipHelper::unzipAll( inFilename, SVString( SvStl::GlobalPath::Inst().GetRunPath().c_str() ), list );
		for (SVStringSet::const_iterator it = list.begin();it != list.end();++it)
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
	hr = ExtractProperties(inFilename, rNewDisableMethod, rEnableAuxExtents, rVersionNumber);
	// Deal with single zip file..
	if( 0 != list.size())
	{
		// cleanup
		for (SVStringSet::const_iterator it = list.begin();it != list.end();++it)
		{
			::DeleteFile(it->c_str());
		}
	}
	::CoUninitialize();
	return hr;
}

