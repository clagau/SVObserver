//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015, 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \Author	Arvid
//*****************************************************************************
/// Contains the class ExtrasEngine which contains functionality for actions
/// chosen from the Extras Pull down in the SVObserver Main Menu
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "ObjectInterfaces/SVUserMessage.h"
#include "TextDefinesSvO.h"
#include "ObjectInterfaces/TextDefineSvOi.h"
#include "SVObserver.h" //needed only for one call to SVObserverApp::fileSaveAsSVX()
#include "SVMainFrm.h"
#include "SVOMFCLibrary\Utilities.h"
#include "SVSVIMStateClass.h"
#include "ExtrasEngine.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVOMFCLibrary/SVDeviceParams.h" //Arvid: added to avoid VS2015 compile Error

#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations


HINSTANCE ExtrasEngine::ms_FbwfDllInstance = nullptr;

const TCHAR* const FbwfCheckFunctionName= 
	_T("FbwfIsFilterEnabled");///< the third autosave temp directory name
const TCHAR* const FbwfEnableBatchName= 
	_T("fbwf Enable for next session.bat");///< name of the batch file that enables the file based write filter after the next reboot
const TCHAR* const FbwfDisableBatchName= 
	_T("fbwf Disable for next session.bat");///< name of the batch file that disables the file based write filter after the next reboot



#pragma region Constructor

ExtrasEngine::ExtrasEngine():m_lastAutoSaveTimestamp(0), m_AutoSaveEnabled(true), m_AutoSaveRequired(false), 
	m_AutoSaveDeltaTime_s(ms_defaultDeltaTimeInMinutes*SVClock::c_secondsPerMinute),
	m_FbwfAvailable(false), m_IsFbwfSelected(false), m_FbwfActive(false), m_FbwfActiveChanging(false)
{

	if (ms_FbwfDllInstance)
	{
		m_pfnFbwfIsFilterEnabled  = (FbwfIsFilterEnabledPtr)::GetProcAddress( ms_FbwfDllInstance, FbwfCheckFunctionName );
	}

	ResetAutoSaveInformation();

	ReadCurrentFbwfSettings(true); //FWBF Settings not to be read too often: here and after selection has been changed in GUI should suffice
}

#pragma endregion Constructor

#pragma region Public Methods

ExtrasEngine& ExtrasEngine::Instance()
{
	static ExtrasEngine s_theAutoSaver; ///< the one and only AutoSaverObject (initialized to a delta time of 10 minutes (this can be changed via setDeltaTimeInMinutes()))
	return s_theAutoSaver;
}

void ExtrasEngine::ExecuteAutoSaveIfAppropriate(bool always)
{
	if(!(IsEnabled() && IsAutoSaveRequired()))
	{
		return; //not enabled by user (or configuration not modified):do nothing
	}

	if(!IsAutoSaveTimestampOlderThanDeltaTime()) 
	{
		if(!always)
		{
			return; 
		}
	}	//this means the last autosave was less than m_AutoSaveDeltaTime_s seconds ago and the autosave is not automatic: nothing to do!

	CWnd *pMainFrame = TheSVObserverApp.GetMainFrame(); 

	CDialog autosavePopupDialog; 
	autosavePopupDialog.Create(IDD_AUTOSAVE_IN_PROGRESS, pMainFrame);

	autosavePopupDialog.CenterWindow(pMainFrame);

	autosavePopupDialog.ShowWindow(SW_SHOW);
	autosavePopupDialog.RedrawWindow(); //do this after ShowWindow(), otherwise no effect!

	SvStl::GlobalPath& rGlobalPath = SvStl::GlobalPath::Inst();
	//ensure that the autosave directory exists: create it step by step
	CreateDirectory(rGlobalPath.GetSecondObserverPath().c_str(), nullptr); //this will fail if the directory already exists, but so what?
	SVString  AutosavePath = rGlobalPath.GetAutoSaveRootPath();
	CreateDirectory(AutosavePath.c_str(), nullptr); //this will fail if the directory already exists, but so what?

	//now move the temporary directories around (if they exist already)
	
	moveContainedDirectory(AutosavePath.c_str(), rGlobalPath.GetAutosaveTempDirectory2Name().c_str(), rGlobalPath.GetAutosaveTempDirectory3Name().c_str());
	moveContainedDirectory(AutosavePath.c_str(), rGlobalPath.GetAutosaveTempDirectory1Name().c_str(), rGlobalPath.GetAutosaveTempDirectory2Name().c_str());

	CreateDirectory(SvStl::GlobalPath::Inst().GetAutoSaveTempPath().c_str(), nullptr);

	//save the current configuration in the AutoSave Directory
	TheSVObserverApp.fileSaveAsSVX(_T(""), true);

	moveContainedDirectory(AutosavePath.c_str(), rGlobalPath.GetAutosaveTempDirectoryName().c_str(), rGlobalPath.GetAutosaveTempDirectory1Name().c_str());

	autosavePopupDialog.DestroyWindow();
}


void ExtrasEngine::ResetAutoSaveInformation()
{
	time(&m_lastAutoSaveTimestamp);
	SetAutoSaveRequired(false);
}


void ExtrasEngine::CopyDirectoryToTempDirectory(const CString &rSourceDir) const 
{
	CopyFilesInDirectory(rSourceDir, SvStl::GlobalPath::Inst().GetAutoSaveTempPath().c_str());
}


void ExtrasEngine::ToggleEnableFbwf()
{
	m_IsFbwfSelected = !m_IsFbwfSelected;

	m_FbwfActiveChanging = (m_FbwfActive != m_IsFbwfSelected);

	CString RequiredBatchFileName(SvO::NoneString);
	SvOi::MessageTextEnum msgId = SvOi::Tid_Empty;
	SVStringArray msgList;

	if(m_IsFbwfSelected)
	{
		RequiredBatchFileName = FbwfEnableBatchName;
		if(m_FbwfActiveChanging)
		{
			msgId = SvOi::Tid_ActivatingDiskProtection;
		}
		else
		{
			msgId = SvOi::Tid_DiskProtectionRemainsActive;
		}
	}
	else
	{
		RequiredBatchFileName = FbwfDisableBatchName;
		if(m_FbwfActiveChanging)
		{
			msgId = SvOi::Tid_DeactivatingDiskProtection;
		}
		else
		{
			msgId = SvOi::Tid_DiskProtectionRemainsInactive;
		}
	}

	CString BatchfilePath;
	BatchfilePath.Format("\"%s\\%s\"",SvStl::GlobalPath::Inst().GetBinPath().c_str(),RequiredBatchFileName);
	auto ret = system(BatchfilePath);
	if(ret)
	{
		msgId = SvOi::Tid_CouldNotExecuteFormatString;
		msgList.push_back(SVString(BatchfilePath));
		// undo selection in this case
		m_IsFbwfSelected = !m_IsFbwfSelected; 
		m_FbwfActiveChanging = (m_FbwfActive != m_IsFbwfSelected);
	}

	SvStl::MessageMgrStd toggleFbwfMessage( SvStl::LogAndDisplay );
	toggleFbwfMessage.setMessage( (ret ? SVMSG_SVO_86_FBWF_CHANGE_ERROR : SVMSG_SVO_85_FBWF_CHANGE), msgId, msgList, SvStl::SourceFileParams(StdMessageParams) );

	ReadCurrentFbwfSettings();
} 


void ExtrasEngine::ReadCurrentFbwfSettings(bool onStart)
{
	ULONG current = 0, next = 0;

	if (nullptr == m_pfnFbwfIsFilterEnabled )
	{
		m_FbwfAvailable = false;
	}
	else
	{
		ULONG ul= m_pfnFbwfIsFilterEnabled ( &current, &next ); // returns 2 (= ERROR_FILE_NOT_FOUND) if FBWF not available

		m_FbwfAvailable = (ul == NO_ERROR );
	}

	if(m_FbwfAvailable)
	{
		m_FbwfActive = (current != 0);
		if(onStart)
		{// on initialization: set selection to FBWF status that will be present after next bootup
			m_IsFbwfSelected = (next != 0);
		}
		m_FbwfActiveChanging = (current != next);
	}

} 


#pragma endregion Public Methods

#pragma region Private Methods

bool ExtrasEngine::IsAutoSaveTimestampOlderThanDeltaTime()
{
	time_t now;

	time(&now);

	double secondSinceLastAutoSave = difftime(now, m_lastAutoSaveTimestamp);

	bool isOlder = secondSinceLastAutoSave > m_AutoSaveDeltaTime_s;

	return isOlder;
}


#pragma endregion Private Methods