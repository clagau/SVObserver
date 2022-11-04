// \copyright COPYRIGHT (c) 2021,2021 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file SVXFunctions.cpp
/// Various functions concerned with SVX and SVZ files that were taken out of the SVObserverApp class 

#include "stdafx.h"

#include "ExtrasEngine.h"
#include "LoadConfigWarning.h"
#include "RootObject.h"
#include "SVConfigurationObject.h"
#include "SVGlobal.h"
#include "SVIOController.h"
#include "SVIODoc.h"
#include "SVIPDoc.h"
#include "SVInspectionTreeParser.h"
#include "SVMainFrm.h"
#include "SVObjectScriptParser.h"
#include "SVParserProgressDialog.h"
#include "SVXFunctions.h"

#include "SVFileSystemLibrary/SVFileNameManagerClass.h"
#include "SVOLibrary/ObsoleteItemChecker.h"
#include "SVOLibrary/SVHardwareManifest.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVUtilityLibrary/SHA256.h"
#include "SVUtilityLibrary/Heapwalk.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVUtilityLibrary/SVClock.h"
#include "SVXMLLibrary/SVConfigurationTags.h"
#include "SVXMLLibrary/LoadConfiguration.h"
#include "SVXMLLibrary/SVNavigateTree.h"

namespace
{
	HRESULT ConstructDocuments(SVTreeType& p_rTree) //@TODO [Arvid][10.20][18.10.2021]: this function is too long
	{
		HRESULT l_Status(S_OK);

		SVTreeType::SVBranchHandle htiChild = nullptr;

		if (SvXml::SVNavigateTree::GetItemBranch(p_rTree, SvXml::CTAG_IO, nullptr, htiChild))
		{
			SVIOController* pIOController(nullptr);

			_variant_t svVariant;

			if (SvXml::SVNavigateTree::GetItem(p_rTree, SvXml::CTAG_UNIQUE_REFERENCE_ID, htiChild, svVariant))
			{
				pIOController = dynamic_cast<SVIOController*>(SVObjectManagerClass::Instance().GetObject(calcObjectId(svVariant)));
			}
			else
			{
				SVConfigurationObject* pConfig(nullptr);
				SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

				if (nullptr != pConfig)
				{
					pIOController = pConfig->GetIOController();

					l_Status = S_OK;
				}
				else
				{
					l_Status = E_FAIL;
				}
			}

			if (nullptr != pIOController)
			{
				SVIODoc* l_pIODoc(NewSVIODoc(pIOController->GetName(), *pIOController));

				if (nullptr != l_pIODoc)
				{
					l_pIODoc->UpdateAllViews(nullptr);
				}
				else
				{
					l_Status = E_FAIL;
				}
			}
			else
			{
				l_Status = E_FAIL;
			}
		}
		else
		{
			l_Status = E_FAIL;
		}

		if (S_OK == l_Status && SvXml::SVNavigateTree::GetItemBranch(p_rTree, SvXml::CTAG_INSPECTION, nullptr, htiChild))
		{
			SVTreeType::SVBranchHandle htiSubChild(nullptr);

			htiSubChild = p_rTree.getFirstBranch(htiChild);

			while (S_OK == l_Status && nullptr != htiSubChild)
			{
				SVInspectionProcess* pInspection(nullptr);
				SVTreeType::SVBranchHandle htiSVIPDoc = nullptr;
				SVTreeType::SVBranchHandle htiSVInspectionProcess = nullptr;

				SvXml::SVNavigateTree::GetItemBranch(p_rTree, SvXml::CTAG_INSPECTION_PROCESS, htiSubChild, htiSVInspectionProcess);
				SvXml::SVNavigateTree::GetItemBranch(p_rTree, SvXml::CTAG_SVIPDOC, htiSubChild, htiSVIPDoc);

				SVTreeType::SVBranchHandle htiTempItem = htiSVIPDoc;

				if (nullptr != htiSVInspectionProcess)
				{
					htiTempItem = htiSVInspectionProcess;
				}

				if (nullptr != htiTempItem)
				{
					_variant_t svVariant;

					if (SvXml::SVNavigateTree::GetItem(p_rTree, SvXml::CTAG_UNIQUE_REFERENCE_ID, htiTempItem, svVariant))
					{
						pInspection = dynamic_cast<SVInspectionProcess*>(SVObjectManagerClass::Instance().GetObject(calcObjectId(svVariant)));

						if (nullptr == pInspection)
						{
							l_Status = E_FAIL;
						}
					}
					else
					{
						l_Status = E_FAIL;
					}
				}
				else
				{
					l_Status = E_FAIL;
				}

				if (S_OK == l_Status)
				{
					SVIPDoc* pIPDoc(nullptr);
					//If Inspection pointer is nullptr then pIPDoc will also be nullptr
					if (nullptr != pInspection) { pIPDoc = NewSVIPDoc(pInspection->GetName(), *pInspection); }

					if (nullptr != pIPDoc)
					{
						pIPDoc->IsNew = false;

						if (!pIPDoc->SetParameters(p_rTree, htiSVIPDoc))
						{
							l_Status = E_FAIL;
						}

						if (S_OK == l_Status)
						{
							// Init Document
							if (!pIPDoc->InitAfterSystemIsDocked())
							{
								l_Status = E_FAIL;
							}
						}

					}
					else
					{
						l_Status = E_FAIL;
					}
				}

				htiSubChild = p_rTree.getNextBranch(htiChild, htiSubChild);
			}
		}
		else
		{
			l_Status = E_FAIL;
		}

		return l_Status;
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
						SVObjectScriptParser* pParser = new SVObjectScriptParser(new SVInspectionTreeParser(p_rTree, hItemToolset, parserHandle, ownerId, pInspect, &l_ParserProgressDialog));
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

	std::pair<bool, HRESULT> doStuffAfterConfigurationLoad(SVConfigurationObject* pConfig, SVTreeType& rXMLTree, SvOi::TRC_RAIIPtr globalInitRAII, SVIMProductEnum SVIMType)
	{
		HRESULT hr = pConfig->LoadGlobalConstants(rXMLTree);
		if (hr & SvDef::svErrorCondition)
		{
			return {false, hr};
		}

		try
		{
			//the globalInit have to be finished before RebuildInputOutputLists called, because it will do a reset and this need the images and memory.
			if (nullptr != globalInitRAII)
			{
				globalInitRAII->free();
				globalInitRAII = nullptr;
			}
		}
		catch (const SvStl::MessageContainer& rExp)
		{
			SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Exception.setMessage(rExp.getMessage());
			hr = E_FAIL;
			return {false, hr};
		}

		pConfig->RebuildInputOutputLists(true);

		hr = pConfig->LoadRemoteMonitorList(rXMLTree);
		if (hr & SvDef::svErrorCondition)
		{
			return {false, hr};
		}

		SVSVIMStateClass::AddState(SV_STATE_INTERNAL_RUN);
		pConfig->RunOnce();
		SVSVIMStateClass::RemoveState(SV_STATE_INTERNAL_RUN);

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
				msgList.emplace_back(isSvimPlc ? SvDef::SVO_PRODUCT_SVIM_NEO : SvDef::SVO_PRODUCT_SVIM_X2_GD8A);
				msgList.emplace_back(isConfigPlc ? SvDef::SVO_PRODUCT_SVIM_NEO : SvDef::SVO_PRODUCT_SVIM_X2_GD8A);
				Exception.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ModelTypeMismatch, msgList, SvStl::SourceFileParams(StdMessageParams));
			}
		}

		return {true, hr};
	}
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
		INT_PTR result = Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_SVObserver_WrongVersionNumber, msgList, SvStl::SourceFileParams(StdMessageParams), SvDef::InvalidObjectId, MB_YESNO);
		if (IDNO == result)
		{
			return false;
		}
	}
	return true;
}


HRESULT LoadSvxFile(unsigned long svoVersion, const std::string& rSvxFilePath, const std::string& rSvzFilePath, SVIMProductEnum SVIMType, SVMainFrame* pMainFrame, SvOi::TRC_RAIIPtr globalInitRAII)
{
	unsigned long configVer = 0;
	CWaitCursor wait;

	SVTreeType XMLTree;

	if (false == LoadSvxFilePart1(configVer, svoVersion, rSvxFilePath, XMLTree))
	{
		return E_FAIL;
	}

	double startTimeStamp = SvUl::GetTimeStamp();

	auto [ok, hr] = LoadSvxFilePart2(std::move(globalInitRAII), SVIMType, XMLTree, pMainFrame);

	if (false == ok)
	{
		return hr;
	}

#ifdef LOG_HEAP_INFO
	SvUl::logHeap(std::format(_T("Configuration '{}' loaded"), rSvxFilePath));
#endif

	wait.Restore();

	ConstructDocuments(XMLTree);

	pMainFrame->OnConfigurationFinishedInitializing();

	long timeUsed = static_cast<long>(SvUl::GetTimeStamp() - startTimeStamp);

	SvDef::StringVector msgList;
	msgList.push_back(rSvzFilePath);
	msgList.push_back(SvUl::Format(_T("%d"), timeUsed));
	SvStl::MessageManager Exception(SvStl::MsgType::Log);
	Exception.setMessage(SVMSG_SVO_29_SVOBSERVER_CONFIG_LOADED, SvStl::Tid_ConfigLoadTime, msgList, SvStl::SourceFileParams(StdMessageParams));

	return hr;
}


std::pair<bool, HRESULT> LoadSvxFilePart2(SvOi::TRC_RAIIPtr globalInitRAII, SVIMProductEnum SVIMType, SVTreeType& rXmlTree, SVMainFrame* pMainFrame)
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
		return doStuffAfterConfigurationLoad(pConfig, rXmlTree, std::move(globalInitRAII), SVIMType);
	}

	return {false, E_FAIL};
}

