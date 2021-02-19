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
#include "SVInspectionImporter.h"
#include "RootObject.h"
#include "SVConfigurationObject.h"
#include "SVInspectionTreeParser.h"
#include "SVInspectionProcess.h"
#include "SVIProgress.h"
#include "SVObjectScriptParser.h"
#include "SVParserProgressDialog.h"
#include "Definitions/StringTypeDef.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVUtilityLibrary/ZipHelper.h"
#include "SVXMLLibrary/SaxXMLHandler.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#include "SVXMLLibrary/SVConfigurationTags.h"
#include "SVXMLLibrary/SVNavigateTree.h"
#include "SVXMLLibrary/SaxExtractPropertiesHandler.h"
#pragma endregion Includes

static LPCTSTR scImportNewExt = _T(".new.xml");
static LPCTSTR scDependentsZipExt = _T(".dependents.zip");
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
				///Remote imput must be imported even it is not assigned to a ppq otherwise
				//we get corrupt configurations 
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
						long l_Index = 0;

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
static bool importGlobalConstants( SVTreeType& rTree, std::vector<SvUl::GlobalConstantData>& rImportedGlobals )
{
	bool Result( true );

	SVTreeType::SVBranchHandle hItem;
	if (SvXml::SVNavigateTree::GetItemBranch( rTree, SvXml::CTAG_GLOBAL_CONSTANTS, nullptr, hItem ))
	{
		SVTreeType::SVBranchHandle hItemChild( nullptr );

		hItemChild = rTree.getFirstBranch( hItem );

		while( Result && nullptr != hItemChild )
		{
			std::string dottedName = rTree.getBranchName( hItemChild );
			if (std::none_of(rImportedGlobals.begin(), rImportedGlobals.end(), [dottedName](const auto& rData) { return rData.m_DottedName == dottedName; }))
			{
				SvUl::GlobalConstantData& rGlobalData = rImportedGlobals.emplace_back();
				rGlobalData.m_DottedName = std::move(dottedName);
				_variant_t Value;
				Result = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_VALUE, hItemChild, Value);
				if (Result)
				{
					rGlobalData.m_Value = Value;
				}
				Value.Clear();
				Result = SvXml::SVNavigateTree::GetItem(rTree, SvXml::CTAG_DESCRIPTION, hItemChild, Value);
				if (Result)
				{
					std::string Description = SvUl::createStdString(Value.bstrVal);
					//This is needed to insert any CR LF in the description which were replaced while saving
					SvUl::RemoveEscapedSpecialCharacters(Description, true);
					rGlobalData.m_Description = Description;
				}
			}
			
			hItemChild = rTree.getNextBranch( hItem, hItemChild );
		}
	}

	return Result;
}

static void getLinkedValueUsingGlobalConst(SVTreeType& rTree, std::back_insert_iterator<std::vector<std::pair<std::string, SVTreeType::SVBranchHandle>>> listInserter, SVTreeType::SVBranchHandle startBranch = nullptr )
{
	if (nullptr != startBranch)
	{
		//check current branch is value object
		SVTreeType::SVBranchHandle hBranch = nullptr;
		SvXml::SVNavigateTree::GetItemBranch(rTree, scArrayElementsTag, startBranch, hBranch);
		if (nullptr != hBranch)
		{
			auto leafItem = rTree.findLeaf(hBranch, scElementTag);
			if (rTree.isValidLeaf(hBranch, leafItem))
			{
				_variant_t data;
				rTree.getLeafData(leafItem, data);
				if (VT_BSTR == data.vt)
				{
					std::string_view testString{ "Global." };
					std::string value{ SvUl::createStdString(data.bstrVal) };
					if (value._Starts_with(testString))
					{
						_variant_t classIDVariant;
						SvXml::SVNavigateTree::GetItem(rTree, scClassIDTag, startBranch, classIDVariant);
						SvPb::ClassIdEnum classId = calcClassId(classIDVariant);
						if (SvPb::StringValueClassId == classId)
						{
							auto parentBranch = rTree.getParentBranch(startBranch);
							auto previousBranch = rTree.getPreviousBranch(parentBranch, startBranch);
							SvXml::SVNavigateTree::GetItem(rTree, scClassIDTag, previousBranch, classIDVariant);
							classId = calcClassId(classIDVariant);
							if (SvPb::VariantValueClassId == classId)
							{
								listInserter = { value, previousBranch };
							}
						}
					}
				}
			}
		}
	}

	//check children
	SVTreeType::SVBranchHandle childBranch(rTree.getFirstBranch(startBranch));
	while (rTree.isValidBranch(childBranch))
	{
		getLinkedValueUsingGlobalConst(rTree, listInserter, childBranch);
		childBranch = rTree.getNextBranch(startBranch, childBranch);
	}
}

static void checkGlobalConstants(SVTreeType& rTree, SvUl::GlobalConflictPairVector& rGlobalConflicts )
{
	std::vector<SvUl::GlobalConstantData> importedGlobals;
	importGlobalConstants(rTree, importedGlobals);

	//search for GlobalConst in LinkedValues in InspectionTree
	std::vector< std::pair<std::string, SVTreeType::SVBranchHandle>> globalElementList;
	getLinkedValueUsingGlobalConst(rTree, std::back_inserter(globalElementList));

	for (auto& rGlobalImport : importedGlobals)
	{
		SvVol::BasicValueObjectPtr pGlobalConstant = RootObject::getRootChildObjectValue(rGlobalImport.m_DottedName.c_str() );
		if (nullptr == pGlobalConstant)
		{
			//add Global constant
			pGlobalConstant = RootObject::setRootChildValue(rGlobalImport.m_DottedName.c_str(), rGlobalImport.m_Value);
			if (nullptr != pGlobalConstant)
			{
				pGlobalConstant->setDescription(rGlobalImport.m_Description.c_str());
				if (rGlobalImport.m_Value.vt == VT_BSTR)
				{
					pGlobalConstant->SetObjectAttributesAllowed(SvPb::selectableForEquation, SvOi::SetAttributeType::RemoveAttribute);
				}
			}
			rGlobalImport.m_objectId = pGlobalConstant->getObjectId();
		}
		else
		{
			_variant_t valueTmp;
			pGlobalConstant->getValue(valueTmp);
			if (rGlobalImport.m_Value != valueTmp)
			{
				auto& rPair = rGlobalConflicts.emplace_back();
				rPair.first.m_objectId = pGlobalConstant->getObjectId();
				rPair.first.m_DottedName = pGlobalConstant->GetCompleteName();
				rPair.first.m_Value = valueTmp;
				//Default is that the current Global Constant is selected
				rPair.first.m_Selected = true;
				rPair.second = rGlobalImport;
			}
			rGlobalImport.m_objectId = pGlobalConstant->getObjectId();
		}

		//override objectId for GlobalConst in LinkedValues
		for (auto& rGlobalElementPair : globalElementList)
		{
			if (rGlobalElementPair.first == rGlobalImport.m_DottedName)
			{
				SVTreeType::SVBranchHandle hBranch = nullptr;
				SvXml::SVNavigateTree::GetItemBranch(rTree, scArrayElementsTag, rGlobalElementPair.second, hBranch);
				if (nullptr != hBranch)
				{
					auto leafItem = rTree.findLeaf(hBranch, scElementTag);
					assert(rTree.getTree().end() != leafItem);
					if (rTree.getTree().end() != leafItem)
					{
						rTree.setLeafData(leafItem, convertObjectIdToVariant(rGlobalImport.m_objectId));
					}
				}
			}
		}	
	}
}

typedef std::insert_iterator<SVImportedInputList> InputListInsertor;

HRESULT LoadInspectionXml(SvXml::SVXMLMaterialsTree& rXmlTree, const std::string& zipFilename, const std::string& inspectionName, const std::string& cameraName, SVImportedInspectionInfo& inspectionInfo, SvUl::GlobalConflictPairVector& rGlobalConflicts, SVIProgress& rProgress, int& currentOp, int numOperations)
{
	HRESULT hr = S_OK;

	rProgress.UpdateText(_T("Importing Dependent Files..."));
	rProgress.UpdateProgress(++currentOp, numOperations);

	SvDef::StringVector Files;
	SvUl::unzipAll(zipFilename, SvStl::GlobalPath::Inst().GetRunPath(), Files);

	rProgress.UpdateText(_T("Importing PPQ Inputs..."));
	rProgress.UpdateProgress(++currentOp, numOperations);

	InputListInsertor insertor(inspectionInfo.m_inputList, inspectionInfo.m_inputList.begin());
	ImportPPQInputs(rXmlTree, insertor);

	rProgress.UpdateText(_T("Importing Global Constants..."));
	rProgress.UpdateProgress(++currentOp, numOperations);

	checkGlobalConstants(rXmlTree, rGlobalConflicts);

	rProgress.UpdateText(_T("Creating Inspection object..."));
	rProgress.UpdateProgress(++currentOp, numOperations);

	SvXml::SVXMLMaterialsTree::SVBranchHandle hItem;
	if (SvXml::SVNavigateTree::GetItemBranch(rXmlTree, SvXml::CTAG_INSPECTION_PROCESS, nullptr, hItem))
	{
		SvXml::SVXMLMaterialsTree::SVBranchHandle hItemToolset;
		if (SvXml::SVNavigateTree::GetItemBranch(rXmlTree, SvXml::CTAG_TOOLSET_SET, hItem, hItemToolset))
		{
			rProgress.UpdateProgress(++currentOp, numOperations);

			// Set the Caption
			SVParserProgressDialog l_ParserProgressDialog(_T("Loading Inspection(s) ..."));

			unsigned long parserHandle = SVObjectScriptParser::GetParserHandle();

			// Create Inspection process - leave unattached for now
			hr = SVInspectionTreeParser< SvXml::SVXMLMaterialsTree >::CreateInspectionObject(inspectionInfo.m_inspectionId, rXmlTree, hItem);
			if (S_OK == hr)
			{
				rProgress.UpdateText(_T("Creating Toolset objects..."));
				rProgress.UpdateProgress(++currentOp, numOperations);

				SVObjectClass* pObject = nullptr;
				SVObjectManagerClass::Instance().GetObjectByIdentifier(inspectionInfo.m_inspectionId, pObject);
				SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*>(pObject);
				if (nullptr != pInspection)
				{
					pInspection->SetToolsetImage(cameraName.c_str());
					pInspection->CreateInspection(inspectionName.c_str());

					rProgress.UpdateText(_T("Setting Inspection Camera name..."));
					rProgress.UpdateProgress(++currentOp, numOperations);

					SVConfigurationObject::updateConfTreeToNewestVersion(rXmlTree, hItemToolset);

					// Launch parser progress
					SVObjectScriptParser* pParser = new SVObjectScriptParser(new SVInspectionTreeParser< SvXml::SVXMLMaterialsTree >(rXmlTree, hItemToolset, parserHandle, inspectionInfo.m_inspectionId, pInspection, &l_ParserProgressDialog));
					if (nullptr != pParser)
					{
						// Set the Parser Object
						l_ParserProgressDialog.AddParser(parserHandle, pParser);

						// Show the Dialog
						l_ParserProgressDialog.DoModal();

						pInspection->connectAllInputs();

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


	if( SUCCEEDED(hr) )
	{
		SvXml::SVXMLMaterialsTree::SVBranchHandle Root(rXmlTree.getRoot() );
		SvXml::SVXMLMaterialsTree::SVBranchHandle IPDocItem( nullptr );

		if( SvXml::SVNavigateTree::FindBranch(rXmlTree, Root, SVFindPredicate(rXmlTree, SvXml::CTAG_SVIPDOC ), IPDocItem ) )
		{
			inspectionInfo.m_materialsTree.clear();
			inspectionInfo.m_materialsTree = SVMaterialsTree(*IPDocItem);
		}
		else
		{
			hr = E_FAIL;
		}
	}
	
	return hr;
}

HRESULT SVInspectionImporter::Import(const std::string& filename, const std::string& inspectionName, const std::string& cameraName, SVImportedInspectionInfo& inspectionInfo, SvUl::GlobalConflictPairVector& rGlobalConflicts, SVIProgress& rProgress)
{
	HRESULT result = S_OK;
	::CoInitialize(nullptr);

	std::string inFileName = filename;
	std::string zipFilename = GetFilenameWithoutExt(inFileName);
	zipFilename += scDependentsZipExt;

	int currentOp = 0;
	int numOperations = 10;
	rProgress.UpdateProgress(++currentOp, numOperations);

	SvDef::StringVector FileList;

	// Deal with single zip file
	if (isExportFile(inFileName))
	{
		if (SvUl::unzipAll(inFileName, SvStl::GlobalPath::Inst().GetRunPath(), FileList))
		{
			for (const auto rEntry : FileList)
			{
				if (isXMLFile(rEntry))
				{
					inFileName = rEntry;
				}
				else if (isZipFile(rEntry))
				{
					zipFilename = rEntry;
				}
			}
		}
		else
		{
			result = E_FAIL;
		}
	}

	rProgress.UpdateText(_T("Loading Inspection XML..."));
	SvXml::SVXMLMaterialsTree XmlTree;
	HRESULT hr = loadAndReplaceData(inFileName, inspectionName, XmlTree);
	if (S_OK == hr)
	{
		rProgress.UpdateProgress(++currentOp, numOperations);
		result = LoadInspectionXml(XmlTree, zipFilename, inspectionName, cameraName, inspectionInfo, rGlobalConflicts, rProgress, currentOp, numOperations);

		rProgress.UpdateProgress(++currentOp, numOperations);
		rProgress.UpdateText(_T("Import Complete."));
	}
	else
	{
		result = -SvStl::Err_15017;
	}

	// cleanup
	for (const auto rEntry : FileList)
	{
		::DeleteFile(rEntry.c_str());
	}
	::CoUninitialize();
	return result;
}



HRESULT SVInspectionImporter::GetProperties(const std::string& rFileName, long& rNewDisableMethod, long& rEnableAuxExtents, unsigned long& rVersionNumber)
{
	HRESULT Result = S_OK;
	::CoInitialize(nullptr);

	std::string inFileName = rFileName;
	SvDef::StringVector FileList;

	// Deal with single zip file
	if (isExportFile(inFileName))
	{
		if(SvUl::unzipAll( inFileName, SvStl::GlobalPath::Inst().GetRunPath(), FileList))
		{
			auto iter = find_if(FileList.rbegin(), FileList.rend(), [](const auto& rFile) { return isXMLFile(rFile); });
			if (FileList.rend() != iter)
			{
				inFileName = *iter;
			}
		}
	}
	
	SvXml::SaxExtractPropertiesHandler   SaxExtractHandler;
	Result =   SaxExtractHandler.ExtractProperties(inFileName.c_str(), rNewDisableMethod, rEnableAuxExtents, rVersionNumber);

	// cleanup
	for (const auto rEntry : FileList)
	{
		::DeleteFile(rEntry.c_str());
	}
	::CoUninitialize();
	return Result;
}

HRESULT SVInspectionImporter::loadAndReplaceData(const std::string& inFileName, const std::string& rNewInspectionName, SvXml::SVXMLMaterialsTree& rTree)
{
	SvXml::SVXMLMaterialsTree XmlTree;
	SvXml::SaxXMLHandler<SVTreeType>  SaxHandler;
	HRESULT	hr = SaxHandler.BuildFromXMLFile(&XmlTree, inFileName.c_str());
	if (S_OK != hr)
	{
		return hr;
	}
	std::string xmlString;
	std::ifstream FileStream;
	FileStream.open(inFileName.c_str(), std::ifstream::in | std::ifstream::binary | std::ifstream::ate);
	if (FileStream.is_open())
	{
		size_t FileSize(0);
		FileSize = static_cast<size_t> (FileStream.tellg());
		xmlString.resize(FileSize);
		FileStream.seekg(0, std::ios::beg);
		FileStream.read(&xmlString.at(0), FileSize);
		FileStream.close();
	}

	constexpr LPCTSTR cXmlSearch = _T("Type=\"VT_BSTR\">");
	/// This replacement is required to insure rXmlData does not have multiple unique ID values
	std::string searchString{ cXmlSearch };
	searchString += _T("{#");
	std::string replaceString{ cXmlSearch };
	replaceString += _T("($");
	SvUl::searchAndReplace(xmlString, searchString.c_str(), replaceString.c_str());

	//Replace each uniqueID with a new ID
	SvDef::StringVector UniqueIDVector = XmlTree.getLeafValues(XmlTree.getRoot(), std::string(scUniqueReferenceIDTag));
	for (auto rUniqueID : UniqueIDVector)
	{
		uint32_t newId = SVObjectManagerClass::Instance().getNextObjectId();
		std::string newIdString = convertObjectIdToString(newId);
		SvUl::searchAndReplace(rUniqueID, _T("{#"), _T("($"));
		SvUl::searchAndReplace(xmlString, rUniqueID.c_str(), newIdString.c_str());
	}
	SvUl::searchAndReplace(xmlString, replaceString.c_str(), searchString.c_str());

	SvXml::SVXMLMaterialsTree::SVBranchHandle hItem;
	if (SvXml::SVNavigateTree::GetItemBranch(XmlTree, SvXml::CTAG_INSPECTION_PROCESS, nullptr, hItem))
	{
		_variant_t oldInspectionNameVariant;
		SvXml::SVNavigateTree::GetItem(XmlTree, scObjectNameTag, hItem, oldInspectionNameVariant);
		std::string oldInspectionName = SvUl::createStdString(oldInspectionNameVariant);
		std::string replaceStrings[] = 
		{ 
			"<DATA Name=\"ObjectName\" Type=\"VT_BSTR\">%s</DATA>", 
			"<DATA Name=\"Element\" Type=\"VT_BSTR\">%s</DATA>", 
			"Type=\"VT_BSTR\">%s.Tool Set.",
			"Type=\"VT_BSTR\">%s.DIO.",
			"Type=\"VT_BSTR\">%s.Remote Input"
		};

		for (const auto& rString : replaceStrings)
		{
			std::string oldString = SvUl::Format(rString.c_str(), oldInspectionName.c_str());
			std::string newString = SvUl::Format(rString.c_str(), rNewInspectionName.c_str());
			boost::algorithm::replace_all(xmlString, oldString, newString);
			//alternative without boost:
			//SvUl::searchAndReplace(xmlString, oldString.c_str(), newString.c_str());
		}
	}

	return SaxHandler.BuildFromXMLString(&rTree, _variant_t(xmlString.c_str()));
}
