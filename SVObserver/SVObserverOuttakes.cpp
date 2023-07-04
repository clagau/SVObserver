//*****************************************************************************
// \copyright COPYRIGHT (c) 2021,2021 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file SVObserverOuttakes.cpp
/// contains functions that were taken out of SVObserver in SVO-3442. 
/// These should be transferred to other source files when possible
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "ExtrasEngine.h"
#include "RootObject.h"
#include "SVConfigurationObject.h"
#include "SVDirectX.h"
#include "SVGlobal.h"
#include "SVIPDoc.h"
#include "SVInspectionProcess.h"
#include "SVMainFrm.h"
#include "SVObserver.h"
#include "SVObserverOuttakes.h"
#include "SVSecurity/SVSecurityManager.h"
#include "SVPPQObject.h"
#include "SVRCWebsocketServer.h"
#include "SVToolSet.h"
#include "SVUtilities.h"
#include "SoftwareTriggerDlg.h"
#include "CameraLibrary/SVLongValueDeviceParam.h"
#include "Definitions/StringTypeDef.h"
#include "InspectionEngine/SVDigitizerProcessingClass.h"
#include "FilesystemUtilities/FileHelperManager.h"
#include "SVMatroxLibrary/SVMatroxApplicationInterface.h"
#include "SVMatroxLibrary/SVOLicenseManager.h"
#include "SVMessage/SVMessage.h"
#include "SVOLibrary/SVHardwareManifest.h"
#include "SVOLibrary/SVMemoryManager.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVSharedMemoryLibrary/SharedMemWriter.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVStatusLibrary/MessageContainer.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/SvimState.h"
#include "SVUtilityLibrary/Heapwalk.h"
#include "SVUtilityLibrary/LoadDll.h"
#include "Triggering/SVTriggerObject.h"
#include "Triggering/SVTriggerProcessingClass.h"
#pragma endregion Includes

long g_OfflineCount = 0;

constexpr int batExecutionTimeout = 2 * 60000;

long getSvoOfflineCount()
{
	return g_OfflineCount;
}

bool AddSecurityNode(HMODULE hMessageDll, long lId, LPCTSTR NTGroup, bool bForcePrompt)
{
	LPTSTR pTmp;

	if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS |
		FORMAT_MESSAGE_FROM_HMODULE,
		(LPCVOID)hMessageDll, lId,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR)&pTmp, 0, nullptr))
	{

		std::string Tmp(pTmp);
		Tmp = SvUl::Left(Tmp, Tmp.find('\n') - 1);
		TheSecurityManager().SVAdd(lId, Tmp.c_str(), NTGroup, bForcePrompt);
		LocalFree(pTmp);
		return true;
	}
	return false;
}

bool AddSecurityNode(HMODULE hMessageDll, long lId)
{
	bool l_bRet = true;
	LPTSTR pszTmp;

	if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS |
		FORMAT_MESSAGE_FROM_HMODULE,
		(LPCVOID)hMessageDll, lId,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR)&pszTmp, 0, nullptr))
	{
		CString strTmp = pszTmp;
		int l_pos = strTmp.Find('\n', 0);
		if (l_pos > 0)
		{
			TheSecurityManager().SVAdd(lId, strTmp.Left(l_pos - 1));
		}
		else
		{
			l_bRet = false;
		}
		LocalFree(pszTmp);
	}
	return l_bRet;
}


void executePreOrPostExecutionFile(const std::string& filepath, bool inRunMode)
{
	if (std::filesystem::exists(filepath) && inRunMode)
	{
		std::string filename = std::filesystem::path(filepath).filename().generic_string();
		SHELLEXECUTEINFO ShExecInfo = {0};
		ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		ShExecInfo.hwnd = NULL;
		ShExecInfo.lpVerb = NULL;
		ShExecInfo.lpFile = filename.c_str();
		ShExecInfo.lpParameters = "";
		ShExecInfo.lpDirectory = SvStl::GlobalPath::Inst().GetRunPath().c_str();
		ShExecInfo.nShow = SW_HIDE;
		ShExecInfo.hInstApp = NULL;

		ShellExecuteEx(&ShExecInfo);
		DWORD returnValue = WaitForSingleObject(ShExecInfo.hProcess, batExecutionTimeout);

		if (returnValue == WAIT_TIMEOUT)
		{
			SvDef::StringVector msgList;
			msgList.push_back(filename);
			SvStl::MessageManager Msg(SvStl::MsgType::Log);
			Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_BatchfileExecutionTimeout, msgList, SvStl::SourceFileParams(StdMessageParams));
		}
	}
}

void StopSvo()
{
	CWaitCursor wait;
	SvimState::SVRCBlocker block;

	GetSvoMainFrame()->SetStatusInfoText(_T(""));

	EnableTriggerSettings(false);

	if (!SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_EDIT | SV_STATE_REGRESSION))
	{
		return;
	}

#ifdef LOG_HEAP_INFO
	SvUl::logHeap(_T("Leaving run mode"));
#endif

	if (SvimState::CheckState(SV_STATE_STOPING))
	{
		if (SvimState::CheckState(SV_STATE_STOP_PENDING))
		{
			SvimState::RemoveState(SV_STATE_STOP_PENDING);
		}
		return;
	}

	if (SvimState::CheckState(SV_STATE_STARTING))
	{
		SvimState::AddState(SV_STATE_STOP_PENDING);
		return;
	}

	bool wasInRunMode = SvimState::CheckState(SV_STATE_RUNNING);

	SvimState::changeState(SV_STATE_UNAVAILABLE | SV_STATE_STOPING, SV_STATE_READY | SV_STATE_RUNNING | SV_STATE_STOP_PENDING);
	auto* pTrcRW = SvOi::getTriggerRecordControllerRWInstance();
	if (nullptr != pTrcRW)
	{
		pTrcRW->unlockReset();
	}

	SVObjectManagerClass::Instance().SetState(SVObjectManagerClass::ReadWrite);

	AfxGetApp()->SetThreadPriority(THREAD_PRIORITY_NORMAL);

	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (nullptr != pConfig)
	{
		pConfig->SetModuleReady(false);
	}

	// Signal Module Stop...
	StopAll();

	// Increment Offline Count
	g_OfflineCount++;

	long lSize = pConfig->GetPPQCount();
	for (long l = 0; l < lSize; l++)
	{
		SVPPQObject* pPPQ = pConfig->GetPPQ(l);
		if (nullptr != pPPQ) { pPPQ->GoOffline(); }
	}
	// Stop the FailStatus Stream PPQ Listeners

	SetAllIPDocumentsOffline();

	std::string TriggerCounts;
	GetTriggersAndCounts(TriggerCounts);

	//add message to event viewer - gone off-line
	SvStl::MessageManager Exception(SvStl::MsgType::Log);
	Exception.setMessage(SVMSG_SVO_28_SVOBSERVER_GO_OFFLINE, TriggerCounts.c_str(), SvStl::SourceFileParams(StdMessageParams));

	if (pConfig != nullptr)
	{
		executePreOrPostExecutionFile(pConfig->getPostRunExecutionFilePath(), wasInRunMode);
	}

	SvimState::changeState(SV_STATE_READY | SV_STATE_STOP, SV_STATE_UNAVAILABLE | SV_STATE_STOPING | SV_STATE_TEST | SV_STATE_REGRESSION | SV_STATE_EDIT);

	if (SvimState::CheckState(SV_STATE_START_PENDING))
	{
		GetSvoMainFrame()->PostMessage(WM_COMMAND, MAKEWPARAM(ID_MODE_RUN, 0), 0);
	}

	RootObject::setRootChildValue(SvDef::FqnEnvironmentSoftwareTrigger, false);
}

void StopAll()
{
	auto pSVMainFrame = GetSvoMainFrame();
	if (pSVMainFrame)
	{
		pSVMainFrame->SetStatusInfoText(_T(""));

		SvimState::RemoveState(SV_STATE_RUNNING | SV_STATE_TEST);

		SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);

		SoftwareTriggerDlg::Instance().ClearTriggers();
	}
}


HRESULT GetTriggersAndCounts(std::string& rTriggerCounts)
{
	HRESULT hr = S_FALSE;
	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	if (nullptr != pConfig)
	{
		long lCount = pConfig->GetPPQCount();
		for (int lPPQ = 0; lPPQ < lCount; lPPQ++)
		{
			SVPPQObject* pPPQ = pConfig->GetPPQ(lPPQ);
			//Returns true when pointer valid
			if (nullptr != pPPQ)
			{
				SvTrig::SVTriggerObject* pTrigger(pPPQ->GetTrigger());
				if (nullptr != pTrigger)
				{
					hr = S_OK;
					std::string Temp = std::format(_T("\n{} count-{:d}"), pTrigger->GetName(), pTrigger->getTriggerCount());
					rTriggerCounts += Temp;
				}
			}
		}
	}
	return hr;
}

HRESULT ConnectToolsetBuffers()
{
	// EB 2002 02 28
	// Added this to fix memory leak freeing MIL buffers
	// before we close Acq devices, we need to tell all toolsets to Close
	// reopen the connection between toolset images and the MIL acq image here.

	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	long lInspectionCount = 0;
	//If the pointer is a nullptr then the count will be 0
	if (nullptr != pConfig) { lInspectionCount = pConfig->GetInspectionCount(); }

	for (long lInspection = 0; lInspection < lInspectionCount; lInspection++)
	{
		SVInspectionProcess* pInspection = pConfig->GetInspection(lInspection);
		if (nullptr != pInspection)
		{
			if (nullptr != pInspection->GetToolSet())
			{
				pInspection->ConnectToolSetMainImage();
			}
		}
	}

	return S_OK;
}

HRESULT DisconnectToolsetBuffers()
{
	// EB 2002 02 28
	// Added this to fix memory leak freeing MIL buffers
	// before we close Acq devices, we need to tell all toolsets to Close
	// this closes the connection between toolset images and the MIL acq image.

	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

	long lInspectionCount = 0;

	//If the pointer is a nullptr then the count will be 0
	if (nullptr != pConfig) { lInspectionCount = pConfig->GetInspectionCount(); }

	for (long lInspection = 0; lInspection < lInspectionCount; lInspection++)
	{
		SVInspectionProcess* pInspection = pConfig->GetInspection(lInspection);
		if (nullptr != pInspection)
		{
			if (nullptr != pInspection->GetToolSet())
			{
				pInspection->DisconnectToolSetMainImage();
			}
		}
	}

	return S_OK;
}

HRESULT ConnectCameraBuffers()
{
	HRESULT hr = S_OK;

	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
	SvDef::StringVector Cameras;
	SvIe::SVDigitizerProcessingClass::Instance().GetAcquisitionDeviceList(Cameras);

	if (nullptr != pConfig)
	{
		SvDef::StringVector::const_iterator Iter(Cameras.begin());
		for (; Cameras.end() != Iter; ++Iter)
		{
			SvIe::SVAcquisitionClassPtr pAcqDevice;

			pAcqDevice = SvIe::SVDigitizerProcessingClass::Instance().GetAcquisitionDevice(Iter->c_str());
			if (nullptr != pAcqDevice)
			{
				// dummy vars
				SVLightReference* pLR;
				SvFs::FileNameContainer* pFiles;
				SVLut* pLut;
				SVDeviceParamCollection* pParams;

				if (pConfig->GetAcquisitionDevice(Iter->c_str(), pFiles, pLR, pLut, pParams))
				{
					SVImageInfoClass svImageInfo;
					pAcqDevice->GetImageInfo(&svImageInfo);
					hr = pAcqDevice->CreateBuffers(svImageInfo);
				}
			}
		}

		if (S_OK == hr)
		{
			hr = ConnectToolsetBuffers();
		}
	}

	return hr;
}

HRESULT DisconnectCameraBuffers()
{
	HRESULT hr = DisconnectToolsetBuffers();

	if (S_OK == hr)
	{
		hr = SvIe::SVDigitizerProcessingClass::Instance().DestroyBuffers();
	}

	return hr;
}

HRESULT ConnectCameras()
{
	HRESULT hr = S_OK;

	SvIe::SVDigitizerProcessingClass::Instance().ConnectDevices();

	try
	{
		hr = ConnectCameraBuffers();
	}
	catch (...)
	{
		Log_Assert(FALSE);
		hr = E_FAIL;
	}

	return hr;
}

HRESULT DisconnectCameras()
{
	HRESULT hr = S_OK;

	try
	{
		hr = DisconnectCameraBuffers();
	}
	catch (...)
	{
		Log_Assert(FALSE);
		hr = E_FAIL;
	}

	SvIe::SVDigitizerProcessingClass::Instance().DisconnectDevices();

	return hr;
}


void StartTrigger(SVConfigurationObject* pConfig)
{
	SvimState::SVRCBlocker block;
	//The pointer is usually checked by the caller
	if (nullptr != pConfig)
	{
		SvStl::MessageContainer Msg;
		long ppqCount = pConfig->GetPPQCount();
		for (long i = 0; i < ppqCount && 0 == Msg.getMessage().m_MessageCode; i++)
		{
			auto* pPPQ = pConfig->GetPPQ(i);
			if (nullptr != pPPQ)
			{
				SvTrig::SVTriggerObject* pTrigger {pPPQ->GetTrigger()};
				if (nullptr != pTrigger)
				{
					pPPQ->setOnline();
					if (!pTrigger->Start())
					{
						SvDef::StringVector msgList;
						msgList.push_back(pTrigger->GetName());
						Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_GoOnlineFailure_Trigger, msgList, SvStl::SourceFileParams(StdMessageParams));
					}
				}
			}
		}
		if (0 != Msg.getMessage().m_MessageCode)
		{
			//If an error has occurred need to clean up
			for (long i = 0; i < ppqCount; i++)
			{
				auto* pPPQ = pConfig->GetPPQ(i);
				if (nullptr != pPPQ)
				{
					pPPQ->GoOffline();
				}
			}
			throw Msg;
		}
	}
}


bool OpenConfigFileFromMostRecentList(CRecentFileList* pRecentFileList, int nID)
{
	Log_Assert(nullptr != pRecentFileList);

	Log_Assert(nID >= ID_FILE_MRU_FILE1);
	Log_Assert(nID < ID_FILE_MRU_FILE1 + (int)pRecentFileList->GetSize());
	int nIndex = nID - ID_FILE_MRU_FILE1;
	Log_Assert((*pRecentFileList)[nIndex].GetLength() != 0);

#if defined (TRACE_THEM_ALL) || defined (TRACE_SVO)
	TRACE2("MRU: open file (%d) '%s'.\n", (nIndex)+1,
		(LPCTSTR)(*m_pRecentFileList)[nIndex]);
#endif
	bool bResult = false;

	// Check Security to see if save is allowed

	// Open and read the Configuration from the Most Recent Used
	// List under the menu File.
	//
	SvFs::FileHelper svFileName;

	svFileName.SetFullFileName((*pRecentFileList)[nIndex]);

	HRESULT hr = TheSVObserverApp().OpenFile((*pRecentFileList)[nIndex]);

	//if S_OK, check to see if it has any tool errors in the license manager
	if (S_OK == hr)
	{
		if (SVOLicenseManager::Instance().HasToolErrors())
		{
			SVOLicenseManager::Instance().ShowLicenseManagerErrors();
		}
	}

	if (S_FALSE == hr)    // don't do on ERROR_CANCELLED
	{
		pRecentFileList->Remove(nIndex);
	}
	bResult = (S_OK == hr);

	return bResult;
}

bool SetStatusText(LPCTSTR pStatusText)
{
	// If pStatusText is a nullptr, the Main Frame deletes the last status info.
	// If no Main Frame exists, the function returns false.
	// Otherwise ( and this should always be ) it returns false!

	auto pMainWnd = GetSvoMainFrame();
	if (pMainWnd)
	{
		pMainWnd->SetStatusInfoText(pStatusText);
		return true;
	}
	return false;
}

void StopRegression()
{
	//get list of IPDoc's.
	//see if the IPDoc is running regression, if so send command to stop.
	//since only 1 IPDoc can run regression at a time, as soon as 1 is found, break...
	SvimState::SVRCBlocker block;
	CDocTemplate* pDocTemplate = nullptr;
	CString strExt;
	POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();
	if (pos)
	{
		bool bDone = false;
		do
		{
			pDocTemplate = AfxGetApp()->GetNextDocTemplate(pos);
			if (pDocTemplate)
			{
				POSITION posDoc = pDocTemplate->GetFirstDocPosition();
				if (posDoc)
				{
					do
					{
						CDocument* newDoc = pDocTemplate->GetNextDoc(posDoc);
						if (newDoc)
						{
							SVIPDoc* pTmpDoc = dynamic_cast <SVIPDoc*> (newDoc);
							if (pTmpDoc)
							{
								if (pTmpDoc->IsRegressionTestRunning())
								{
									POSITION TmpPos = pTmpDoc->GetFirstViewPosition();
									CView* pIPView = pTmpDoc->GetNextView(TmpPos);
									CMDIChildWnd* pFrame = (CMDIChildWnd*)pIPView->GetParentFrame();
									pFrame->MDIActivate();
									pTmpDoc->RegressionTestModeChanged();
									bDone = true;
								}
							}
						}
					} while (posDoc && !bDone);
				}
			}
		} while (pos && !bDone);
	}

	StopSvo();
}

void EnableTriggerSettings(bool enable)
{
	if (enable == false)
	{
		SoftwareTriggerDlg::Instance().ShowWindow(SW_HIDE);
	}

	SVUtilities util;
	CWnd* pWindow = AfxGetMainWnd();
	if (pWindow)
	{
		CMultiDocTemplate* pDocTemplate = getIPDocTemplate();
		if (pDocTemplate)
		{
			CString szMenuText(_T("&View"));
			CMenu menu;
			menu.Attach(pDocTemplate->m_hMenuShared);
			CMenu* pSubMenu = util.FindSubMenuByName(&menu, szMenuText);
			if (pSubMenu)
			{
				if (enable)
				{
					pSubMenu->AppendMenu(MF_STRING, ID_TRIGGER_SETTINGS, _T("Software Trigger"));
				}
				else
				{
					pSubMenu->RemoveMenu(ID_TRIGGER_SETTINGS, MF_BYCOMMAND);
				}
			}
			menu.Detach();
			pWindow->DrawMenuBar();
		}
	}
}
