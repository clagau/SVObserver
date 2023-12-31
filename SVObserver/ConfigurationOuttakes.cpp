//*****************************************************************************
// \copyright COPYRIGHT (c) 2021, 2021 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file ConfigurationOuttakes.cpp
/// contains configuration-related functions that were taken out of SVObserver in SVO-3442. 
/// These should be transferred to other source files when possible
//******************************************************************************

#include "stdafx.h"

#include "ConfigurationOuttakes.h"
#include "RootObject.h"
#include "SVIODoc.h"
#include "SVIOController.h"
#include "SVIPDoc.h"
#include "SVIPDocInfoImporter.h"
#include "SVMainFrm.h"
#include "SVOConfigAssistantDlg.h"
#include "SVObserver.h"
#include "SVObserverOuttakes.h"
#include "SVVisionProcessorHelper.h"

#include "Definitions/SVUserMessage.h"
#include "FilesystemUtilities/FileHelperManager.h"
#include "SVLibrary/SVPackedFile.h"
#include "SVMatroxLibrary/SVOLicenseManager.h"
#include "SVSecurity/SVSecurityManager.h"
#include "SVSharedMemoryLibrary/EditLock.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVStatusLibrary/SvimState.h"


struct ConfigFilenames
{
	ConfigFilenames();
	std::string prepareSvxFileSave();

	SvFs::FileHelper m_ConfigFileName;
	SvFs::FileHelper m_SvxFileName;
};

class SVConfigurationObject* UseConfigAssistant(class SVOConfigAssistantDlg& rDlg, class SVConfigurationObject*, bool newConfiguration);
class SVConfigurationObject* CreateConfigAssistant(class SVOConfigAssistantDlg& rDlg, bool newConfiguration);
HRESULT CloseConfigStandard(bool AskForSavingOrClosing, bool CloseWithoutHint);
void CloseConfigOtherwise();
bool DetermineConfigurationSaveName(); ///< determines the name under which a configuration is to be changed

ConfigFilenames g_ConfigFilenames; 

HRESULT ConstructMissingDocuments()
{
	HRESULT Status(S_OK);

	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (nullptr != pConfig)
	{
		SVIOController* pIOController = pConfig->GetIOController();

		if (nullptr != pIOController)
		{
			if (nullptr == pIOController->GetIODoc())
			{
				SVIODoc* pIODoc(NewSVIODoc(pIOController->GetName(), *pIOController));

				if (nullptr != pIODoc)
				{
					pIODoc->UpdateAllViews(nullptr);
				}
				else
				{
					Status = E_FAIL;
				}
			}
		}
		else
		{
			Status = E_FAIL;
		}

		const SVInspectionProcessVector& rInspections = pConfig->GetInspections();

		SVInspectionProcessVector::const_iterator Iter(rInspections.begin());

		while (S_OK == Status && Iter != rInspections.end())
		{
			SVInspectionProcess* pInspection(*Iter);

			if (nullptr != pInspection)
			{
				if (nullptr == GetIPDocByInspectionID(pInspection->getObjectId()))
				{
					SVIPDoc* pIPDoc(NewSVIPDoc(pInspection->GetName(), *pInspection));

					if (nullptr != pIPDoc)
					{
						pIPDoc->mbInitImagesByName = true;

						// Init Document
						if (!pIPDoc->InitAfterSystemIsDocked())
						{
							Status = E_FAIL;
						}
					}
				}
			}
			else
			{
				Status = E_FAIL;
			}

			++Iter;
		}
	}
	else
	{
		Status = E_FAIL;
	}


	return Status;
}

bool CreateAndUseConfigAssistant(bool newConfiguration)
{
	SVOConfigAssistantDlg assistantDlg;

	auto pConfig = CreateConfigAssistant(assistantDlg, newConfiguration);

	if (pConfig)
	{
		pConfig = UseConfigAssistant(assistantDlg, pConfig, newConfiguration);
	}

	return nullptr != pConfig;
}

bool PrepareForNewConfiguration()
{
	// Clean up SVObserver
	if (S_OK != CloseConfig())
	{
		// Needs Attention !!!
		// Should get error code if returned S_FALSE; 
		// If returned ERROR_CANCELLED, user cancelled
		return false;
	}

	RootObject* pRoot = nullptr;

	SVObjectManagerClass::Instance().GetRootChildObject(pRoot, SvDef::FqnRoot);
	if (nullptr != pRoot)
	{
		pRoot->createConfigurationObject(SVObjectManagerClass::Instance().GetMutex());
	}

	// Clean up Execution Directory...
	// Check path, create if necessary and delete contents...
	TheSVObserverApp().InitPath(std::string(SvFs::FileHelperManager::Instance().GetRunPathName() + _T("\\")).c_str(), true, true);

	// Ensure that CloseConfig() can do its work...
	SvimState::AddState(SV_STATE_READY);

	return true;
}

SVConfigurationObject* CreateConfigAssistant(SVOConfigAssistantDlg& rDlg, bool newConfiguration)
{
	SVIMProductEnum eSVIMType = TheSVObserverApp().GetSVIMType();

	rDlg.SetCurrentSystem(eSVIMType);

	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (nullptr != pConfig)
	{
		if (pConfig->GetProductType() != SVIM_PRODUCT_TYPE_UNKNOWN)
		{
			rDlg.SetConfigurationSystem(pConfig->GetProductType());
		}
		else
		{
			rDlg.SetConfigurationSystem(eSVIMType);
		}
	}
	else
	{
		rDlg.SetConfigurationSystem(eSVIMType);
	}

	rDlg.SetIOBoardCapabilities(TheSVObserverApp().m_rInitialInfoSvo.m_IOBoard, eSVIMType);

	rDlg.SetNewConfiguration(newConfiguration);

	SvSml::TemporaryState_Editing tse;
	if (false == tse.stateWasEntered())
	{
		return nullptr;
	}

	if (rDlg.DoModal() == IDOK)
	{
		if (rDlg.Modified())
		{
			SvimState::AddState(SV_STATE_MODIFIED);
		}
		if (nullptr != pConfig)
		{
			pConfig->ValidateRemoteMonitorList();
			SVIODoc* pIODoc = GetTheIODoc();
			if (pIODoc)
			{
				pIODoc->UpdateAllViews(nullptr);
			}
		}
		return pConfig;
	}
	return nullptr;
}

SVConfigurationObject* UseConfigAssistant(SVOConfigAssistantDlg& rAssistantDlg, SVConfigurationObject* pConfig, bool newConfiguration)
{
	CWaitCursor wait;

	std::string NewName = rAssistantDlg.GetConfigurationName();
	if (newConfiguration)
	{
		g_ConfigFilenames.m_ConfigFileName.SetFileNameOnly(NewName.c_str());
		g_ConfigFilenames.m_ConfigFileName.SetExtension(SvDef::cPackedConfigExtension);

		RootObject::setRootChildValue(SvDef::FqnEnvironmentConfigurationName, GetConfigFileNameOnly());

		if (nullptr != pConfig)
		{
			// Make all the PPQs build their default digital inputs
			long lPPQCount = pConfig->GetPPQCount();
			for (long lPPQ = 0; lPPQ < lPPQCount; lPPQ++)
			{
				SVPPQObject* pPPQ = pConfig->GetPPQ(lPPQ);
				Log_Assert(nullptr != pPPQ);
				pPPQ->RebuildInputList();
				pPPQ->RebuildOutputList();
			}
		}
	}

	bool bOk = true;

	if (nullptr != pConfig)
	{
		bOk = pConfig->RebuildInputOutputLists();

		ConstructMissingDocuments();

		// Check for Imported Inspection and adjust views, window placement and conditional history
		const SVImportedInspectionInfoList& infoList = rAssistantDlg.GetImportedInspectionInfoList();
		for (SVImportedInspectionInfoList::const_iterator it = infoList.begin(); it != infoList.end(); ++it)
		{
			const SVImportedInspectionInfo& info = (*it);
			SVIPDoc* pDoc = GetIPDocByInspectionID(info.m_inspectionId);
			if (pDoc)
			{
				SVIPDocInfoImporter::Import(pDoc, info);
			}
		}
		rAssistantDlg.ClearImportedInspectionInfoList();

		if (SVOLicenseManager::Instance().HasToolErrors())
		{
			SVOLicenseManager::Instance().ShowLicenseManagerErrors();
		}
		else
		{
			//close dialog if it is up and has no errors
			SVOLicenseManager::Instance().ClearLicenseErrors();
		}

		//
		// Set the tool selected for an operator to move if any.
		// 26 Jan 2000 - frb.
		//
		AfxGetMainWnd()->PostMessage(SV_SET_TOOL_SELECTED_IN_TOOL_VIEW, (WPARAM)TRUE);

		pConfig->RunOnce();
	}

	if (newConfiguration)
	{
		TheSVObserverApp().UpdateAllMenuExtrasUtilities();
		SvimState::changeState(SV_STATE_READY, SV_STATE_AVAILABLE);
	}

	// Rebuild PPQBar Buttons
	GetSvoMainFrame()->DestroyPPQButtons();
	GetSvoMainFrame()->BuildPPQButtons();

	// Validate Output List TB
	if (SVMSG_SVO_70_DUPLICATE_DISCRETE_OUTPUT == pConfig->ValidateOutputList())
	{
		SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Exception.setMessage(SVMSG_SVO_70_DUPLICATE_DISCRETE_OUTPUT, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
	}

	return bOk ? pConfig : nullptr;
}


const std::string& getConfigPathName()
{
	return g_ConfigFilenames.m_ConfigFileName.GetPathName();
}

const std::string& getConfigFileName()
{
	return g_ConfigFilenames.m_ConfigFileName.GetFileName();
}

const std::string getConfigFullFileName()
{
	return g_ConfigFilenames.m_ConfigFileName.GetFullFileName();
}

bool setConfigFullFileName(LPCTSTR csFullFileName, bool bLoadFile)
{
	g_ConfigFilenames.m_ConfigFileName.SetFullFileName(csFullFileName);

	bool bOk = true;

	if (g_ConfigFilenames.m_ConfigFileName.GetPathName().empty())
	{
		SvFs::FileHelperManager::Instance().SetConfigurationPathName(nullptr);
	}
	else
	{
		if (0 == SvUl::CompareNoCase(g_ConfigFilenames.m_ConfigFileName.GetPathName(), SvStl::GlobalPath::Inst().GetRunPath()))
		{
			SvFs::FileHelperManager::Instance().SetConfigurationPathName(nullptr);
		}
		else
		{
			bOk = SvFs::FileHelperManager::Instance().SetConfigurationPathName(g_ConfigFilenames.m_ConfigFileName.GetPathName().c_str());
			// if this returns FALSE, unable to access specified path!
			if (!bOk)
			{
				SvDef::StringVector msgList;
				msgList.push_back(SvStl::MessageData::convertId2AdditionalText(bLoadFile ? SvStl::Tid_Load : SvStl::Tid_Save));
				msgList.push_back(g_ConfigFilenames.m_ConfigFileName.GetPathName());
				SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
				Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_UnableConfig, msgList, SvStl::SourceFileParams(StdMessageParams));
			}
		}
	}

	if (bOk)
	{
		if (!SvFs::FileHelperManager::Instance().GetConfigurationPathName().empty())
		{
			AfxGetApp()->WriteProfileString(_T("Settings"), _T("ConfigurationFilePath"), SvFs::FileHelperManager::Instance().GetConfigurationPathName().c_str());
		}
	}

	RootObject::setRootChildValue(SvDef::FqnEnvironmentConfigurationFileName, csFullFileName);
	RootObject::setRootChildValue(SvDef::FqnEnvironmentConfigurationName, GetConfigFileNameOnly());

	return bOk;
}

bool fileSaveAsSVXWrapper(const std::string& rFileName, bool resetAutoSave)
{
	//When a file name is present it must have the correct extension
	if (!rFileName.empty() && std::string::npos == rFileName.find(SvDef::cPackedConfigExtension))
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_SVObserver_WrongPathnameEntered, SvStl::SourceFileParams(StdMessageParams));
		return false;
	}

	CWaitCursor wait;
	bool result {true};
	SvimState::AddState(SV_STATE_SAVING);
	ResetAllIPDocModifyFlag(false);

	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (nullptr != pConfig)
	{
		std::string svxFilePath = g_ConfigFilenames.prepareSvxFileSave();

		result = fileSaveAsSVX(pConfig, svxFilePath, rFileName, resetAutoSave);
	}

	SvimState::RemoveState(SV_STATE_SAVING);
	return result;
}

void SaveConfig(bool saveAs /*= false*/)
{
	if (SvPb::DeviceModeType::editModuleMode == SvimState::GetMode())
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ActionNotPossibleInModuleEditingMode, {getCompleteObjectNameForId(SvimState::getModuleEditingId())}, SvStl::SourceFileParams(StdMessageParams));
		return;
	}

	if (saveAs || getConfigPathName().empty())
	{
		if (false == DetermineConfigurationSaveName())
		{
			return;
		}
	}
	std::string fileName {getConfigFullFileName()};

	if (fileSaveAsSVXWrapper(fileName, true))
	{
		setConfigFullFileName(fileName.c_str(), false);
		if (0 == _access(fileName.c_str(), 0))
		{
			AfxGetApp()->AddToRecentFileList(fileName.c_str());
			SvimState::RemoveState(SV_STATE_MODIFIED);
		}

		((CMDIFrameWnd*)AfxGetMainWnd())->OnUpdateFrameTitle(TRUE);
	}
}

HRESULT SavePackedConfiguration(LPCTSTR pFileName)
{
	HRESULT Result {S_OK};

	bool bRunning = SvimState::CheckState(SV_STATE_RUNNING);

	if (bRunning)
	{
		TheSVObserverApp().StopIfRunning();
	}

	if (SvimState::CheckState(SV_STATE_READY))
	{
		if (false == fileSaveAsSVXWrapper(pFileName, false))
		{
			Result = E_UNEXPECTED;
		}

		if (bRunning)
		{
			TheSVObserverApp().EnterRunMode();
		}
	}

	return Result;
}

bool DetermineConfigurationSaveName()
{
	SvFs::FileHelper svFileName = g_ConfigFilenames.m_ConfigFileName;
	svFileName.SetFileType(SvFs::FileType::svzConfig);
	//Need to remove the *.svx files being visible
	svFileName.SetFileExtensionFilterList(_T("SVResearch Configuration Files (*.svz)|*.svz||"));

	if (getConfigPathName().empty() ||
		0 == SvUl::CompareNoCase(getConfigPathName(), SvStl::GlobalPath::Inst().GetRunPath()))
	{
		svFileName.SetPathName(AfxGetApp()->GetProfileString(_T("Settings"), _T("ConfigurationFilePath"), SvStl::GlobalPath::Inst().GetRunPath().c_str()));

		if (0 == SvUl::CompareNoCase(svFileName.GetPathName(), SvStl::GlobalPath::Inst().GetRunPath()))
		{
			if (!SvFs::FileHelperManager::Instance().GetConfigurationPathName().empty())
			{
				svFileName.SetPathName(SvFs::FileHelperManager::Instance().GetConfigurationPathName().c_str());
			}
		}
	}

	if (svFileName.SaveFile())
	{
		if (setConfigFullFileName(svFileName.GetFullFileName().c_str(), false))
		{
			AfxGetApp()->WriteProfileString(_T("Settings"), _T("ConfigurationFilePath"), svFileName.GetPathName().c_str());
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	return true;
}

void GetAndDestroyConfigurationObject()
{
	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (nullptr != pConfig)
	{
		RootObject* pRoot = nullptr;
		SVObjectManagerClass::Instance().GetRootChildObject(pRoot, SvDef::FqnRoot);
		if (nullptr != pRoot)
		{
			pRoot->destroyConfigurationObject(SVObjectManagerClass::Instance().GetMutex());
		}
	}
}

HRESULT LoadPackedConfiguration(LPCTSTR pFileName, ConfigFileType fileType)
{
	
#ifdef LOG_LOADING
	std::string filename(pFileName);
	std::string  msg = std::format("LoadPackedConfiguration: {} ", filename);
	Log_Info(msg.c_str());
#endif	
	HRESULT l_Status = S_OK;
	std::string fileName {pFileName};

	if (0 == _access(fileName.c_str(), 0))
	{
		CloseConfig(false, true);
	}
	else
	{
		l_Status = E_UNEXPECTED;
	}

	if (S_OK == l_Status)
	{
		if (ConfigFileType::PackedFormat == fileType)
		{
			SVPackedFile PackedFile;
			if (PackedFile.UnPackFiles(fileName.c_str(), SvStl::GlobalPath::Inst().GetRunPath().c_str()))
			{
				if (PackedFile.getConfigFilePath().empty() || (_access(PackedFile.getConfigFilePath().c_str(), 0) != 0))
				{
					l_Status = E_UNEXPECTED;
				}
				else
				{
					fileName = PackedFile.getConfigFilePath();
				}
				fileType = ConfigFileType::SvzFormatPutConfig;
			}
			else
			{
				l_Status = E_UNEXPECTED;
			}
		}
	}

	if (S_OK == l_Status)
	{
		l_Status = TheSVObserverApp().OpenFile(fileName.c_str(), false, fileType);
	}
#ifdef LOG_LOADING	
	msg = std::format("LoadPackedConfiguration RESULT:{}  ", l_Status);
	Log_Info(msg.c_str());
#endif 	
	return l_Status;
}

HRESULT CloseConfig(bool AskForSavingOrClosing /* = true */, bool CloseWithoutHint /* = false */)
{
	SvimState::SVRCBlocker block;

	HRESULT hr = S_OK;

	if (SvimState::CheckState(SV_STATE_READY | SV_STATE_RUNNING | SV_STATE_TEST))
	{
		hr = CloseConfigStandard(AskForSavingOrClosing, CloseWithoutHint);
	}
	else
	{
		CloseConfigOtherwise();
	}

	if (ERROR_CANCELLED != hr)
	{
		DisconnectCameras();
		ConnectCameras();
	}

	::OutputDebugString(std::format(_T("Closing config {}\n"), getConfigFileName()).c_str());
	SVObjectManagerClass::Instance().listAllObjects();
	return hr;
}


bool DestroyConfigAndPpq()
{
	_variant_t configName;
	configName.SetString(getConfigFullFileName().c_str());
	SVVisionProcessorHelper::Instance().FireNotification(SvPb::NotifyType::configUnloaded, configName);
	SVOLicenseManager::Instance().ClearLicenseErrors();
	SvimState::ConfigWasUnloaded();
	SvimState::changeState(SV_STATE_UNAVAILABLE | SV_STATE_CLOSING, SV_STATE_READY | SV_STATE_MODIFIED | SV_STATE_EDIT | SV_STATE_STOP);

	CWaitCursor wait;

	wait.Restore();

	GetAndDestroyConfigurationObject();

	wait.Restore();

	GetSvoMainFrame()->SetStatusInfoText(_T(""));

	wait.Restore();

	// Destroy the current PPQ
	GetSvoMainFrame()->DestroyPPQButtons();
	GetSvoMainFrame()->InitToolBars();

	wait.Restore();

	SvimState::changeState(SV_STATE_AVAILABLE, SV_STATE_UNAVAILABLE | SV_STATE_CLOSING | SV_STATE_CANCELING);

	bool bOk = setConfigFullFileName(nullptr, true);

	wait.Restore();

	getIniInfoHandler().INIReset();

	return bOk;
}


HRESULT CloseConfigStandard(bool AskForSavingOrClosing, bool CloseWithoutHint)
{
	bool bOk = false;
	bool bClose = true;
	bool bCancel = false;
	HRESULT hr = S_OK;

	if (CloseWithoutHint)
	{
		SvimState::AddState(SV_STATE_CANCELING);
	}
	else
	{
		SvDef::StringVector msgList;
		msgList.push_back(getConfigFileName());
		SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		INT_PTR result = Msg.setMessage(SVMSG_SVO_94_GENERAL_Informational, SvStl::Tid_UserQuestionCloseConfig, msgList, SvStl::SourceFileParams(StdMessageParams), SvDef::InvalidObjectId, MB_YESNO);
		bClose = IDYES == result;
		if (!bClose)
		{
			hr = ERROR_CANCELLED;
		}
	}

	if (bClose)
	{
		if (SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_TEST))
		{
			StopSvo();
		}

		std::tie(bClose, bCancel) = OkToClose(AskForSavingOrClosing);
	}// end if ( bClose )

	if (bClose)
	{
		bClose = SvimState::CheckState(SV_STATE_READY);
	}

	if (bClose)
	{
		bOk = DestroyConfigAndPpq();
	}

	if (S_OK == hr)
	{
		hr = bOk ? S_OK : S_FALSE;
	}

	if (bCancel)
	{
		hr = ERROR_CANCELLED;
	}

	return hr;

}// end if bOk = ! SvimState::CheckState( SV_STATE_READY | SV_STATE_RUNNING );



void CloseConfigOtherwise()
{
	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (nullptr != pConfig)
	{
		RootObject* pRoot = nullptr;
		SVObjectManagerClass::Instance().GetRootChildObject(pRoot, SvDef::FqnRoot);
		if (nullptr != pRoot)
		{

			pRoot->destroyConfigurationObject(SVObjectManagerClass::Instance().GetMutex());
		}
	}

	GetSvoMainFrame()->SetStatusInfoText(_T(""));
}


std::pair<bool, bool> OkToClose(bool AskForSavingOrClosing)
{
	bool bClose = true;
	bool bCancel = false;
	if (AskForSavingOrClosing)
	{
		// Check if current config is modified and ask for saving
		if (SvimState::CheckState(SV_STATE_MODIFIED))
		{
			if (SvimState::isModuleEditing())
			{
				SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
				INT_PTR result = Msg.setMessage(SVMSG_SVO_94_GENERAL_Informational, SvStl::Tid_UserQuestionModuleEditCloseWithoutSaving, {getCompleteObjectNameForId(SvimState::getModuleEditingId())}, SvStl::SourceFileParams(StdMessageParams), SvDef::InvalidObjectId, MB_YESNO);
				if (IDYES == result)
				{
					SvimState::AddState(SV_STATE_CANCELING);
				}
				else
				{
					bCancel = true;
					bClose = false;
				}
			}
			else
			{
				SvDef::StringVector msgList;
				msgList.push_back(getConfigFileName());
				SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
				INT_PTR result = Msg.setMessage(SVMSG_SVO_94_GENERAL_Informational, SvStl::Tid_UserQuestionSaveChanges, msgList, SvStl::SourceFileParams(StdMessageParams), SvDef::InvalidObjectId, MB_YESNOCANCEL);
				switch (result)
				{
					case IDNO:
					{
						SvimState::AddState(SV_STATE_CANCELING);
						ResetAllIPDocModifyFlag(false);
						break;
					}
					case IDYES:
					{
						if (S_OK == TheSecurityManager().SVValidate(SECURITY_POINT_FILE_MENU_SAVE_CONFIGURATION))
						{
							SaveConfig();
							ResetAllIPDocModifyFlag(false);

							// Check whether config saving is done.
							// If not, an error or an user cancel
							// command occured!
							bClose = !std::string(getConfigFullFileName()).empty() &&
								!SvimState::CheckState(SV_STATE_MODIFIED);
						}
						break;
					}
					case IDCANCEL:  // Fall through to default case.
					default:	// Don�t close config!
					{
						bCancel = true;
						bClose = false;
						break;
					}
				}// end switch( result )
			}
		}// end if ( SvimState::CheckState( SV_STATE_MODIFIED ) )
	}
	else
	{
		SvimState::AddState(SV_STATE_CANCELING);
	}
	return {bClose, bCancel};
}


std::string GetSvxFileNameOnly()
{
	return g_ConfigFilenames.m_SvxFileName.GetFileNameOnly();
}

std::string GetConfigFileNameOnly()
{
	return g_ConfigFilenames.m_ConfigFileName.GetFileNameOnly();
}



void SetFullSvxFileName(const std::string& rFullFileName)
{
	return g_ConfigFilenames.m_SvxFileName.SetFullFileName(rFullFileName.c_str());
}


void LoadSvxFilenameFromManager()
{
	SvFs::FileHelperManager::Instance().LoadItem(&g_ConfigFilenames.m_SvxFileName);
}


void SetConfigFileNameOnly(const std::string& rOriginalFile)
{
	g_ConfigFilenames.m_ConfigFileName.SetFileNameOnly(rOriginalFile.c_str());
	g_ConfigFilenames.m_ConfigFileName.SetExtension(SvDef::cPackedConfigExtension);
	g_ConfigFilenames.m_ConfigFileName.SetPathName(nullptr);

	RootObject::setRootChildValue(SvDef::FqnEnvironmentConfigurationFileName, g_ConfigFilenames.m_ConfigFileName.GetFileName());
	RootObject::setRootChildValue(SvDef::FqnEnvironmentConfigurationName, rOriginalFile);
}


void RemoveSvxFilenameFromManager()
{
	SvFs::FileHelperManager::Instance().RemoveItem(&g_ConfigFilenames.m_SvxFileName);
}


void AddSvxFilenameToManager()
{
	SvFs::FileHelperManager::Instance().AddItem(&g_ConfigFilenames.m_SvxFileName);
}

std::string GetFullSvxFileName() 
{
	return g_ConfigFilenames.m_SvxFileName.GetFullFileName();
}

ConfigFilenames::ConfigFilenames()
{
	m_ConfigFileName.SetFileType(SvFs::FileType::svzConfig);
	m_ConfigFileName.setExcludeCharacters(SvDef::cExcludeCharsConfigName);
}

std::string ConfigFilenames::prepareSvxFileSave()
{
	m_SvxFileName.SetFileName(m_ConfigFileName.GetFileName().c_str());
	m_SvxFileName.SetPathName(SvFs::FileHelperManager::Instance().GetRunPathName().c_str());
	m_SvxFileName.SetExtension(SvDef::cConfigExtension);
	return GetFullSvxFileName();
}
