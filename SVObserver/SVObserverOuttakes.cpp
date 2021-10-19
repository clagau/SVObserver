// \copyright COPYRIGHT (c) 2021,2021 by Seidenader Maschinenbau GmbH. All Rights Reserved
/// \file SVObserverOuttakes.cpp
/// Various functions taken out of the SVObserverApp

#include "stdafx.h"

#include "LoadConfigWarning.h"
#include "RootObject.h"
#include "SVConfigurationObject.h"
#include "SVDiscreteInputsView.h"
#include "SVIODoc.h"
#include "SVIOTabbedView.h" 
#include "SVIPDoc.h"
#include "SVIPSplitterFrame.h"
#include "SVImageViewScroll.h"
#include "SVInspectionTreeParser.h"
#include "SVMainFrm.h"
#include "SVMultiDocTemplate.h"
#include "SVObjectScriptParser.h"
#include "SVObserverOuttakes.h"
#include "SVParserProgressDialog.h"

#include "SVFileSystemLibrary/SVFileNameManagerClass.h"
#include "SVOLibrary/ObsoleteItemChecker.h"
#include "SVOLibrary/SVHardwareManifest.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVUtilityLibrary/SHA256.h"
#include "SVXMLLibrary/LoadConfiguration.h"


HRESULT ParseToolsetScripts(SVTreeType& p_rTree, SVMainFrame* pMainFrame);
std::pair<bool, HRESULT> doStuffAfterConfigurationLoad(SVConfigurationObject* pConfig, SVTreeType& rXMLTree, bool isGlobalInit, SVIMProductEnum SVIMType);


CDocTemplate* CreateMultiDocTemplate1()
{
	// Dokumentvorlagen der Anwendung registrieren. Dokumentvorlagen
	//  dienen als Verbindung zwischen Dokumenten, Rahmenfenstern und Ansichten.
	return new SVMultiDocTemplate(IDR_SVOBSERVER_IPDOCTYPE,
		RUNTIME_CLASS(SVIPDoc),			 // Doc
		RUNTIME_CLASS(SVIPSplitterFrame),  // Frame
		RUNTIME_CLASS(SVImageViewScroll));// View
}

CDocTemplate* CreateMultiDocTemplate2()
{
	// IODoc
	return new SVMultiDocTemplate(IDR_SVOBSERVER_IODOCTYPE,
		RUNTIME_CLASS(SVIODoc),
		RUNTIME_CLASS(SVIOTabbedView),
		RUNTIME_CLASS(SVDiscreteInputsView));
}


void RemoveFileFromConfig(LPCTSTR FilePath)
{
	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (nullptr != pConfig)
	{
		const auto& rAdditionalFiles = pConfig->getAdditionalFiles();
		auto iter = std::find_if(rAdditionalFiles.begin(), rAdditionalFiles.end(), [&FilePath](const SVFileNameClass& rFile) {return FilePath == rFile.GetFullFileName(); });
		if (rAdditionalFiles.end() != iter)
		{
			const SVFileNameClass& rSVFileName = *iter;
			pConfig->getAdditionalFiles().remove(rSVFileName);
			SVFileNameManagerClass::Instance().RemoveItem(&rSVFileName);
			SVSVIMStateClass::AddState(SV_STATE_MODIFIED);
		}
	}
}

void AddFileToConfig(LPCTSTR FilePath)
{
	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (nullptr != pConfig)
	{
		const auto& rAdditionalFiles = pConfig->getAdditionalFiles();
		if (std::any_of(rAdditionalFiles.cbegin(), rAdditionalFiles.cend(), [&FilePath](const auto& rFile) { return FilePath == rFile.GetFullFileName(); }))
		{
			//File is already in additional file list
			return;
		}
		pConfig->getAdditionalFiles().emplace_back(SVFileNameClass {FilePath});
		SVFileNameManagerClass::Instance().AddItem(&pConfig->getAdditionalFiles().back());
		SVSVIMStateClass::AddState(SV_STATE_MODIFIED);
	}
}


HRESULT RebuildOutputObjectListHelper(SVIODoc* pIODoc)
{
	HRESULT l_Status = S_OK;

	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (nullptr != pConfig)
	{
		pConfig->ValidateRemoteMonitorList();
		pConfig->RebuildOutputObjectList();

		if (pIODoc)
		{
			pIODoc->UpdateAllViews(nullptr);
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}


HRESULT CheckDrive(const std::string& rDrive)
{
	HRESULT l_hr = S_OK;
	// Check if exists
	if (!PathFileExists(rDrive.c_str()))
	{
		std::string Drive = SvUl::MakeUpper(SvUl::Left(rDrive, 1).c_str());

		SvDef::StringVector msgList;
		msgList.push_back(Drive);

		SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Exception.setMessage(SVMSG_SVO_5051_DRIVEDOESNOTEXIST, SvStl::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams));
	}
	TCHAR VolumeName[100];
	TCHAR FileSystemName[32];
	DWORD dwSerialNumber;
	DWORD dwMaxFileNameLength;
	DWORD dwFileSystemFlags;

	if (GetVolumeInformation(rDrive.c_str(),
		VolumeName,
		sizeof(VolumeName),
		&dwSerialNumber,
		&dwMaxFileNameLength,
		&dwFileSystemFlags,
		FileSystemName,
		sizeof(FileSystemName)))
	{
		std::string Name(FileSystemName);
		if (std::string::npos == Name.find(_T("NTFS")))
		{
			std::string Drive = SvUl::MakeUpper(SvUl::Left(rDrive, 1).c_str());

			SvStl::MessageManager Exception(SvStl::MsgType::Log);
			Exception.setMessage(SVMSG_SVO_5052_DRIVENOTNTFSFORMAT, Drive.c_str(), SvStl::SourceFileParams(StdMessageParams));

#ifndef _DEBUG
			assert(false);
#else
#if defined (TRACE_THEM_ALL) || defined (TRACE_SVO)
			::OutputDebugString(Drive.c_str());
#endif
#endif
		}
	}

	return l_hr;
}


std::pair<bool, HRESULT> doStuffAfterConfigurationLoad(SVConfigurationObject* pConfig, SVTreeType& rXMLTree, bool isGlobalInit, SVIMProductEnum SVIMType)
{
	HRESULT hr = pConfig->LoadRemoteMonitorList(rXMLTree);
	if (hr & SvDef::svErrorCondition)
	{
		return {false, hr};
	}

	hr = pConfig->LoadGlobalConstants(rXMLTree);
	if (hr & SvDef::svErrorCondition)
	{
		return {false, hr};
	}

	ObjectAttributeList objectAttributeList;
	hr = pConfig->LoadObjectAttributesSet(rXMLTree, std::inserter(objectAttributeList, objectAttributeList.end()));
	if (S_OK == hr)
	{
		//Set ObjectAttributes needs before RebuildInputOutputLists, because digital Output will be set in this method and then the attributes must be set.
		pConfig->SetObjectAttributes(objectAttributeList);
	}
	else if (hr & SvDef::svErrorCondition)
	{
		return {false, hr};
	}

	if (isGlobalInit)
	{
		try
		{
			//the globalInit have to be finished before RebuildInputOutputLists called, because it will do a reset and this need the images and memory.
			SvOi::getTriggerRecordControllerRWInstanceThrow().finishGlobalInit();
		}
		catch (const SvStl::MessageContainer& rExp)
		{
			SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Exception.setMessage(rExp.getMessage());
			hr = E_FAIL;
			return {false, hr};
		}
	}

	pConfig->RebuildInputOutputLists(true);
	if (S_OK == hr)
	{
		//Set ObjectAttributes also after RebuildInputOutputLists, because the LinkedValue-Children will be create in this function.
		pConfig->SetObjectAttributes(objectAttributeList);
	}

	if (pConfig->IsConfigurationLoaded())
	{
		// Removes any invalid entries in the output list.
		if (SVMSG_SVO_70_DUPLICATE_DISCRETE_OUTPUT == pConfig->ValidateOutputList())
		{
			SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Exception.setMessage(SVMSG_SVO_70_DUPLICATE_DISCRETE_OUTPUT, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
		}
		// Upgrade the configuration depending on the version being loaded
		pConfig->UpgradeConfiguration();

		bool isSvimPlc = SVHardwareManifest::isPlcSystem(SVIMType);
		bool isConfigPlc = SVHardwareManifest::isPlcSystem(pConfig->GetProductType());
		if (isSvimPlc != isConfigPlc)
		{
			SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
			SvDef::StringVector msgList;
			msgList.emplace_back(isSvimPlc ? SvDef::SVO_PRODUCT_SVIM_NEO1 : SvDef::SVO_PRODUCT_SVIM_X2_GD8A);
			msgList.emplace_back(isConfigPlc ? SvDef::SVO_PRODUCT_SVIM_NEO1 : SvDef::SVO_PRODUCT_SVIM_X2_GD8A);
			Exception.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ModelTypeMismatch, msgList, SvStl::SourceFileParams(StdMessageParams));
		}
	}

	return {true, hr};
}


bool LoadSvxFilePart1(unsigned long& ulSVOConfigVersion, unsigned long svoVersion, std::string svxFilePath, SVTreeType& rXmlTree)
{
	HRESULT hr = S_OK;

	std::string hash;
	try
	{
		hash = SvUl::SHA256(svxFilePath.c_str());
	}
	catch (const std::exception&)
	{
		//hash = e.what();;
		hash.clear();
	}

	SVSVIMStateClass::ConfigWasLoaded(hash.c_str());

	try
	{
		hr = SvXml::SVOCMLoadConfiguration(ulSVOConfigVersion, svxFilePath.c_str(), rXmlTree);
	}
	catch (const SvStl::MessageContainer& rExp)
	{
		SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Exception.setMessage(rExp.getMessage());
		return false;
	}

	if (hr & SvDef::svErrorCondition)
	{
		return false;
	}

	std::string itemType;
	int errorCode(0);
	hr = SvXml::CheckObsoleteItems(rXmlTree, ulSVOConfigVersion, itemType, errorCode);
	if (hr & SvDef::svErrorCondition)
	{
		SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Exception.setMessage(SVMSG_SVO_76_CONFIGURATION_HAS_OBSOLETE_ITEMS, itemType.c_str(), SvStl::SourceFileParams(StdMessageParams), errorCode);
		return false;
	}

	checkVersionAndDisplayWarnings(svoVersion, ulSVOConfigVersion);

	if (ulSVOConfigVersion > svoVersion)
	{
		std::string File, App;

		::SVGetVersionString(App, svoVersion);
		::SVGetVersionString(File, ulSVOConfigVersion);
		SvDef::StringVector msgList;
		msgList.push_back(File);
		msgList.push_back(App);

		SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		INT_PTR result = Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_SVObserver_WrongVersionNumber, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10128, SvDef::InvalidObjectId, MB_YESNO);
		if (IDNO == result)
		{
			return false;
		}
	}
	return true;
}


std::pair<bool, HRESULT> LoadSvxFilePart2(bool isGlobalInit, SVIMProductEnum SVIMType, SVTreeType& rXmlTree, SVMainFrame* pMainFrame)
{
	HRESULT hr = S_OK;

	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	//If no configuration then it needs to be created
	if (nullptr == pConfig)
	{
		RootObject* pRoot = nullptr;

		SVObjectManagerClass::Instance().GetRootChildObject(pRoot, SvDef::FqnRoot);
		if (nullptr != pRoot)
		{
			pRoot->createConfigurationObject(SVObjectManagerClass::Instance().GetMutex());
			SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
		}
	}

	if (nullptr != pConfig)
	{
		try
		{
			hr = pConfig->LoadConfiguration(rXmlTree);
		}
		catch (const SvStl::MessageContainer& rSvE)
		{
			hr = E_FAIL;
			//SVMSG_SVO_IGNORE_EXCEPTION can be used to stop the loading but not display any messages
			if (SVMSG_SVO_IGNORE_EXCEPTION != rSvE.getMessage().m_MessageCode)
			{
				SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
				Exception.setMessage(rSvE.getMessage());
			}
		}
	}

	if (hr & SvDef::svErrorCondition)
	{
		return {false, hr};
	}

	ParseToolsetScripts(rXmlTree, pMainFrame);

	if (nullptr != pConfig)
	{
		return doStuffAfterConfigurationLoad(pConfig, rXmlTree, isGlobalInit, SVIMType);
	}

	return {false, E_FAIL};
}

HRESULT ParseToolsetScripts(SVTreeType& p_rTree, SVMainFrame* pMainFrame)
{
	HRESULT l_Status(S_OK);

	SVTreeType::SVBranchHandle hItemToolset = nullptr;
	SVTreeType::SVBranchHandle htiChild = nullptr;

	if (SvXml::SVNavigateTree::GetItemBranch(p_rTree, SvXml::CTAG_INSPECTION, nullptr, htiChild))
	{
		// Set the Caption
		CString title = _T("Loading Toolset(s) ...");
		SVParserProgressDialog l_ParserProgressDialog(title, pMainFrame);

		SVTreeType::SVBranchHandle htiSubChild(p_rTree.getFirstBranch(htiChild));
		;
		while (S_OK == l_Status && nullptr != htiSubChild)
		{
			std::shared_ptr<std::string> pToolsetScript {new std::string};
			uint32_t ownerId = SvDef::InvalidObjectId;

			SVInspectionProcess* pInspect(nullptr);
			SVTreeType::SVBranchHandle htiSVIPDoc = nullptr;
			SVTreeType::SVBranchHandle htiDataChild(p_rTree.getFirstBranch(htiSubChild));

			while (nullptr != htiDataChild)
			{
				std::string DataName = p_rTree.getBranchName(htiDataChild);

				if (0 == DataName.compare(SvXml::CTAG_INSPECTION_PROCESS))
				{
					if (SvXml::SVNavigateTree::GetItemBranch(p_rTree, SvXml::CTAG_TOOLSET_SET, htiDataChild, hItemToolset))
					{
						_variant_t svVariant;

						if (SvXml::SVNavigateTree::GetItem(p_rTree, SvXml::CTAG_UNIQUE_REFERENCE_ID, htiDataChild, svVariant))
						{
							ownerId = calcObjectId(svVariant);
						}
					}
				}
				else if (0 == DataName.compare(SvXml::CTAG_SVIPDOC))
				{
					htiSVIPDoc = htiDataChild;
				}

				htiDataChild = p_rTree.getNextBranch(htiSubChild, htiDataChild);
			}

			if (nullptr != htiSVIPDoc)
			{
				_variant_t svVariant;

				if (SvDef::InvalidObjectId == ownerId)
				{
					if (SvXml::SVNavigateTree::GetItem(p_rTree, SvXml::CTAG_UNIQUE_REFERENCE_ID, htiSVIPDoc, svVariant))
					{
						ownerId = calcObjectId(svVariant);
					}
					else
					{
						l_Status = E_FAIL;
					}
				}

				if (S_OK == l_Status)
				{
					if (SvXml::SVNavigateTree::GetItem(p_rTree, SvXml::CTAG_TOOLSET_SCRIPT, htiSVIPDoc, svVariant))
					{
						std::string NewString = SvUl::LoadStdString(IDS_OBJECTNAME_PAT_DELTAPOS);

						std::string ToolsetScript = SvUl::createStdString(svVariant);

						// Pre-Parse Typo fix.
						// This peice of code fixes the Angel to Angle typo.
						SvUl::searchAndReplace(ToolsetScript, _T("Axis Secondary Angel"), _T("Axis Secondary Angle"));

						// Pre-Parse Typo fix number two.
						// This peice of code fixes the Principle to Principal typo.
						SvUl::searchAndReplace(ToolsetScript, _T("Axis Principle Angle"), _T("Axis Principal Angle"));

						// Pre-Parse Type fix number three.
						// This piece of code changes "Angle delta Positive" to "Angle Delta Positive".
						SvUl::searchAndReplace(ToolsetScript, _T("Angle delta Positive"), NewString.c_str());

						*pToolsetScript = ToolsetScript;
					}
					else if (nullptr == hItemToolset)
					{
						l_Status = E_FAIL;
					}
				}
			}
			else
			{
				l_Status = E_FAIL;
			}

			if (S_OK == l_Status)
			{
				if (SvDef::InvalidObjectId != ownerId)
				{
					pInspect = dynamic_cast<SVInspectionProcess*>(SVObjectManagerClass::Instance().GetObject(ownerId));

					if (nullptr == pInspect)
					{
						l_Status = E_FAIL;
					}
				}
				else
				{
					l_Status = E_FAIL;
				}
			}

			SVConfigurationObject* pConfig(nullptr);
			SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

			if (S_OK == l_Status && nullptr != pConfig)
			{
				unsigned long parserHandle = SVObjectScriptParser::GetParserHandle();
				if (nullptr == hItemToolset)
				{
					l_Status = E_FAIL;
				}
				else // new parsing
				{
					SVConfigurationObject::updateConfTreeToNewestVersion(p_rTree, hItemToolset);
					SVObjectScriptParser* pParser = new SVObjectScriptParser(new SVInspectionTreeParser< SVTreeType >(p_rTree, hItemToolset, parserHandle, ownerId, pInspect, &l_ParserProgressDialog));
					if (nullptr != pParser)
					{
						// Set the Parser Object
						l_ParserProgressDialog.AddParser(parserHandle, pParser);
					}
					else
					{
						l_Status = E_FAIL;
					}
				}
			}
			htiSubChild = p_rTree.getNextBranch(htiChild, htiSubChild);
		}

		if (S_OK == l_Status)
		{
			// Show the Dialog
			l_ParserProgressDialog.DoModal();
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

