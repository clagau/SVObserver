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
#include "stdafx.h"
#include <map>
#include <boost/config.hpp>
#include <boost/any.hpp>
#include <iterator>
#include <deque>
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
#include "SVXMLLibrary/SVNavigateTreeClass.h"
#include "SVInspectionProcess.h"
#include "SVXMLLibrary/SVXML2TreeConverter.h"

///////////////////////////////////////////////////////////
// SEJ Error Codes used by this program
///////////////////////////////////////////////////////////
#define SEJ_ErrorBase 15000
#define Err_15013 (SEJ_ErrorBase+13)
#define Err_15014 (SEJ_ErrorBase+14)
#define Err_15015 (SEJ_ErrorBase+15)
#define Err_15016 (SEJ_ErrorBase+16)
#define Err_15017 (SEJ_ErrorBase+17)
#define Err_15018 (SEJ_ErrorBase+18)
#define Err_15019 (SEJ_ErrorBase+19)

static LPCTSTR scRunDirectory = _T("C:\\Run");
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
	DWORD exitCode = -Err_15018;

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
	if (SVNavigateTreeClass::GetItemBranch(rTree, CTAG_PPQ, NULL, hItem))
	{
		SVTreeType::SVBranchHandle hItemInputs;
		if (SVNavigateTreeClass::GetItemBranch(rTree, CTAG_INPUT, hItem , hItemInputs))
		{
			SVTreeType::SVBranchHandle htiDataChild = NULL;
				
			rTree.GetFirstBranch(hItemInputs, htiDataChild);

			while (bOk && htiDataChild != NULL)
			{
				_bstr_t DataName;
				
				rTree.GetBranchName(htiDataChild, DataName.GetBSTR());

				SVTreeType::SVBranchHandle htiValueChild = NULL;

				rTree.GetFirstBranch(htiDataChild, htiValueChild);

				_bstr_t ValueName;
				
				rTree.GetBranchName(htiValueChild, ValueName.GetBSTR());

				long l_PPQPosition = -1;
				_variant_t svValue;
				CString strName;
				CString strType;
				long l_Index;

				bOk = SVNavigateTreeClass::GetItem(rTree, CTAG_IO_TYPE, htiDataChild, svValue);
				if (bOk)
				{
					strType = static_cast<LPCTSTR>(static_cast<_bstr_t>(svValue));
				}
				bOk = SVNavigateTreeClass::GetItem(rTree, CTAG_ITEM_NAME, htiDataChild, svValue);
				if (bOk)
				{
					DataName = svValue;
				}
				bOk = SVNavigateTreeClass::GetItem(rTree, CTAG_PPQ_POSITION, htiDataChild, svValue);
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

						bOk = SVNavigateTreeClass::GetItem(rTree, CTAG_REMOTE_INDEX, htiDataChild, svValue);
						if (bOk)
						{
							l_Index = svValue;
						}
						if (SVNavigateTreeClass::GetItem(rTree, CTAG_REMOTE_INITIAL_VALUE, htiDataChild, svValue))
						{
							l_Variant = svValue;
						}
						// Add Remote Input
						insertor = boost::any(SVImportedRemoteInput(DataName, l_PPQPosition, l_Index, l_Variant));
					}
				}
				rTree.GetNextBranch(hItemInputs, htiDataChild);
			}
		}
	}
	return bOk;
}

template< typename SVTreeType >
static bool IsSameVersion(SVTreeType& rTree, unsigned long version)
{
	bool bRetVal = false;
	SVTreeType::SVBranchHandle hItem = NULL;

	if ( SVNavigateTreeClass::GetItemBranch( rTree, CTAG_ENVIRONMENT, NULL, hItem ) )
	{
		_variant_t versionValue;
		if ( SVNavigateTreeClass::GetItem( rTree, CTAG_VERSION_NUMBER, hItem, versionValue ) )
		{
			bRetVal = (versionValue.ulVal == version);
		}
	}
	return bRetVal;
}


typedef std::insert_iterator<SVImportedInputList> InputListInsertor;

struct SVFindPredicate
{
	SVXMLMaterialsTree& m_rTree;
	SVString m_Name;

	SVFindPredicate( SVXMLMaterialsTree& p_rTree, const SVString& p_rName ) : m_rTree( p_rTree ), m_Name( p_rName ) {}

	bool operator()( const SVXMLMaterialsTree::SVBranchHandle& p_rRight ) const
	{
		_bstr_t l_Name;

		bool ok = true;
		
		ok = ok && ( m_rTree.GetBranchName( p_rRight, l_Name.GetBSTR() ) == S_OK );
		ok = ok && ( m_Name == SVString( l_Name ) );

		return ok;
	}
};

HRESULT LoadInspectionXml(const SVString& filename, const SVString& zipFilename, unsigned long version, const SVString& inspectionName, const SVString& cameraName, SVImportedInspectionInfo& inspectionInfo, SVIProgress& rProgress, int& currentOp, int numOperations)
{
	_bstr_t bstrRevisionHistory;
	_bstr_t bstrChangedNode;
	CWnd* pParentWnd = AfxGetApp()->GetMainWnd();

	SVMaterialsTree l_LocalTree;
	SVXMLMaterialsTree tree( l_LocalTree );
	SVXMLClass xml;

	HRESULT hr = xml.Initialize(0,	0);
	if (hr == S_OK)
	{
		xml.PreserveWhitespace(true);
		SVString msg = _T("Loading Inspection XML...");
		rProgress.UpdateText(msg.c_str());

		hr = xml.CopyXMLFileToDOM(filename.ToBSTR(), bstrRevisionHistory.GetAddress());
		if (hr == S_OK)
		{
			hr = SVXML2TreeConverter::CopyToTree(xml, tree, L"Base", false);
		
			if (hr == S_OK)
			{
				// check Version first
				if ( IsSameVersion( tree, version ) )
				{
					msg = _T("Importing Dependent Files...");
					rProgress.UpdateText(msg.c_str());
					rProgress.UpdateProgress(++currentOp, numOperations);

					SVStringSet Files;
					ZipHelper::unzipAll( zipFilename, SVString( scRunDirectory ), Files );

					msg = _T("Importing PPQ Inputs...");
					rProgress.UpdateText(msg.c_str());
					rProgress.UpdateProgress(++currentOp, numOperations);

					InputListInsertor insertor(inspectionInfo.m_inputList, inspectionInfo.m_inputList.begin());
					ImportPPQInputs(tree, insertor);

					msg = _T("Creating Inspection object...");
					rProgress.UpdateText(msg.c_str());
					rProgress.UpdateProgress(++currentOp, numOperations);

					SVXMLMaterialsTree::SVBranchHandle hItem;
					if ( SVNavigateTreeClass::GetItemBranch( tree, CTAG_INSPECTION_PROCESS, NULL, hItem ) )
					{
						SVXMLMaterialsTree::SVBranchHandle hItemToolset;
						if ( SVNavigateTreeClass::GetItemBranch( tree, CTAG_TOOLSET_SET, hItem, hItemToolset ) )
						{
							rProgress.UpdateProgress(++currentOp, numOperations);

							// Set the Caption
							SVString title = _T( "Loading Inspection(s) ..." );
							SVParserProgressDialog l_ParserProgressDialog(title.c_str());

							unsigned long parserHandle = SVObjectScriptParserClass::GetParserHandle();

							// Create Inspection process - leave unattached for now
							hr = SVInspectionTreeParser< SVXMLMaterialsTree >::CreateInspectionObject(inspectionInfo.m_inspectionGuid, tree, hItem);
							if (hr == S_OK)
							{
								msg = _T("Creating Toolset objects...");
								rProgress.UpdateText(msg.c_str());
								rProgress.UpdateProgress(++currentOp, numOperations);

								SVInspectionProcess* pInspection = NULL;
								SVObjectManagerClass::Instance().GetObjectByIdentifier(inspectionInfo.m_inspectionGuid, reinterpret_cast<SVObjectClass* &>(pInspection));
								if (pInspection)
								{
									pInspection->SetToolsetImage(cameraName.c_str());
									pInspection->CreateInspection(inspectionName.c_str());

									msg = _T("Setting Inspection Camera name...");
									rProgress.UpdateText(msg.c_str());
									rProgress.UpdateProgress(++currentOp, numOperations);

									// Launch parser progress
									SVObjectScriptParserClass* pParser = new SVObjectScriptParserClass(new SVInspectionTreeParser< SVXMLMaterialsTree >(tree, hItemToolset, parserHandle, inspectionInfo.m_inspectionGuid, pInspection, &l_ParserProgressDialog));
									if ( pParser != NULL)
									{
										// Set the Parser Object
										l_ParserProgressDialog.AddParser(parserHandle, pParser);

										// Show the Dialog
										l_ParserProgressDialog.DoModal();

										::SVSendMessage( pInspection, SVM_CONNECT_ALL_INPUTS, NULL, NULL );

										msg = _T("Parsing Complete.");
										rProgress.UpdateText(msg.c_str());
										rProgress.UpdateProgress(++currentOp, numOperations);
									}
									else
									{
										hr = -Err_15013;
									}
								}
								else
								{
									hr = -Err_15019;
								}
							}
						}
						else
						{
							hr = -Err_15014;
						}
					}
					else
					{
						hr = -Err_15015;
					}
				}
				else
				{
					hr = -Err_15016;
				}
			}
		}
	}

	if( hr == S_OK )
	{
		SVXMLMaterialsTree::SVBranchHandle l_Root;
		SVXMLMaterialsTree::SVBranchHandle l_IPDocItem;
		SVXMLMaterialsTree toTree( inspectionInfo.m_materialsTree );

		tree.GetRoot( l_Root );
		if( SVNavigateTreeClass::FindBranch( tree, l_Root, SVFindPredicate( tree, CTAG_SVIPDOC ), l_IPDocItem ) )
		{
			toTree.Clear();
			toTree.GetRoot( l_Root );

			if( !( SVNavigateTreeClass::Copy( toTree, l_Root, tree, l_IPDocItem ) ) )
			{
				hr = E_FAIL;
			}
		}
		else
		{
			hr = E_FAIL;
		}
	}

	return hr;
}

HRESULT SVInspectionImporter::Import(const SVString& filename, const SVString& inspectionName, const SVString& cameraName, unsigned long version, SVImportedInspectionInfo& inspectionInfo, SVIProgress& rProgress)
{
	HRESULT hr = S_OK;
	::CoInitialize(NULL);

	SVString inFilename = filename;
	SVString zipFilename = GetFilenameWithoutExt(inFilename);
	zipFilename += scDependentsZipExt;

	SVString outFilename = scRunDirectory;
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
		ZipHelper::unzipAll( inFilename, SVString( scRunDirectory ), list );
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
	SVString msg = _T("Transforming Inspection...");
	rProgress.UpdateText(msg.c_str());

	int rc = LaunchTransform(inFilename.c_str(), outFilename.c_str(), inspectionName.c_str());
	if (rc == 0)
	{
		rProgress.UpdateProgress(++currentOp, numOperations);
		hr = LoadInspectionXml(outFilename, zipFilename, version, inspectionName, cameraName, inspectionInfo, rProgress, currentOp, numOperations);
		//
		::DeleteFile(outFilename.c_str());

		rProgress.UpdateProgress(++currentOp, numOperations);
		msg = _T("Import Complete.");
		rProgress.UpdateText(msg.c_str());
	}
	else
	{
		hr = -Err_15017;
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
		hr = xml.CopyXMLFileToDOM(filename.ToBSTR()); 
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
		ZipHelper::unzipAll( inFilename, SVString( scRunDirectory ), list );
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVInspectionImporter.cpp_v  $
 * 
 *    Rev 1.3   19 Dec 2014 04:06:24   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  978
 * SCR Title:  Copy and Paste a Tool within an Inspection or Between Different Inspections
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Zip functionality moved to ZipHelper
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   07 May 2013 08:20:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 011.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   18 Apr 2013 10:48:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  822
 * SCR Title:  Remove DDE Input and Output functionality from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed unused variables.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 May 2013 20:06:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 010.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Apr 2013 14:24:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  822
 * SCR Title:  Remove DDE Input and Output functionality from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed DDE functionality from the source code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 11:02:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   19 Mar 2013 14:42:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  817
 * SCR Title:  Import/Export of Inspections
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed extra whitespace
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