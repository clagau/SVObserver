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
#include "Definitions/SVUserMessage.h"
#include "TextDefinesSvO.h"
#include "Definitions/GlobalConst.h"
#include "Definitions/TextDefineSVDef.h"
#include "SVObserver.h" //needed only for one call to SVObserverApp::fileSaveAsSVX()
#include "SVMainFrm.h"
#include "SVLibrary/DirectoryUtilities.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "ExtrasEngine.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "Definitions/StringTypeDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVUtilityLibrary/ZipHelper.h"
#include "RootObject.h"
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

ExtrasEngine::ExtrasEngine():m_lastAutoSaveTimestamp(0), 
	m_AutoSaveDeltaTime_s(ms_defaultDeltaTimeInMinutes*SvTl::c_secondsPerMinute),
	m_FbwfAvailable(false), m_FbwfActive(false), m_FbwfActiveChanging(false)
{

	if (ms_FbwfDllInstance)
	{
		m_pfnFbwfIsFilterEnabled  = (FbwfIsFilterEnabledPtr)::GetProcAddress( ms_FbwfDllInstance, FbwfCheckFunctionName );
	}

	ResetAutoSaveInformation();
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
	if(!(IsAutoSaveEnabled() && SVSVIMStateClass::IsAutoSaveRequired()))
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
	::CreateDirectory(rGlobalPath.GetSecondObserverPath().c_str(), nullptr); //this will fail if the directory already exists, but so what?
	std::string  AutosavePath = rGlobalPath.GetAutoSaveRootPath();
	::CreateDirectory(AutosavePath.c_str(), nullptr); //this will fail if the directory already exists, but so what?

	//now move the temporary files around (if they exist already)
	std::string Temp1Name {rGlobalPath.GetAutoSaveRootPath(rGlobalPath.GetAutosaveTemp1FileName().c_str())};
	std::string Temp2Name {rGlobalPath.GetAutoSaveRootPath(rGlobalPath.GetAutosaveTemp2FileName().c_str())};
	std::string Temp3Name {rGlobalPath.GetAutoSaveRootPath(rGlobalPath.GetAutosaveTemp3FileName().c_str())};

	::remove(Temp3Name.c_str());
	::MoveFile(Temp2Name.c_str(), Temp3Name.c_str());
	::MoveFile(Temp1Name.c_str(), Temp2Name.c_str());

	CreateDirectory(rGlobalPath.GetAutoSaveTempPath().c_str(), nullptr);

	//save the current configuration in the AutoSave Directory
	TheSVObserverApp.fileSaveAsSVX(_T(""), false, true);

	SvDef::StringVector fileNameList = findFiles(rGlobalPath.GetAutoSaveTempPath().c_str());

	SvUl::makeZipFile(Temp1Name, fileNameList, _T(""), true);
 
	autosavePopupDialog.DestroyWindow();
}


void ExtrasEngine::ResetAutoSaveInformation()
{
	time(&m_lastAutoSaveTimestamp);
	SVSVIMStateClass::SetAutoSaveRequired(false);
}


void ExtrasEngine::CopyDirectoryToTempDirectory(LPCTSTR SourceDir ) const 
{
	CopyFilesInDirectory( SourceDir, SvStl::GlobalPath::Inst().GetAutoSaveTempPath().c_str() );
}

void ExtrasEngine::ToggleEnableAutoSave()
{
	_variant_t Value;
	RootObject::getRootChildValue(SvDef::FqnEnvironmentAutoSave, Value);
	//Set the value with inverted state
	RootObject::setRootChildValue(SvDef::FqnEnvironmentAutoSave, Value ? false : true);
}

bool ExtrasEngine::IsAutoSaveEnabled() const
{
	_variant_t Value;
	RootObject::getRootChildValue(SvDef::FqnEnvironmentAutoSave, Value);
	return Value ? true : false;
}

void ExtrasEngine::SetAutoSaveEnabled(bool enabled)
{
	RootObject::setRootChildValue(SvDef::FqnEnvironmentAutoSave, enabled);
}

void ExtrasEngine::ChangeFbwfState()
{
	//Avoid recursive calls to ChangeFbwfState
	static bool ChangeStateActive{false};

	if(!ChangeStateActive)
	{
		ChangeStateActive = true;
		bool IsFbwfSelected{false};
		RootObject::getRootChildValue(SvDef::FqnEnvironmentDiskProtection, IsFbwfSelected);

		m_FbwfActiveChanging = (m_FbwfActive != IsFbwfSelected);

		std::string RequiredBatchFileName(SvO::NoneString);
		SvStl::MessageTextEnum msgId = SvStl::Tid_Empty;
		SvDef::StringVector msgList;

		if(IsFbwfSelected)
		{
			RequiredBatchFileName = FbwfEnableBatchName;
			if(m_FbwfActiveChanging)
			{
				msgId = SvStl::Tid_ActivatingDiskProtection;
			}
			else
			{
				msgId = SvStl::Tid_DiskProtectionRemainsActive;
			}
		}
		else
		{
			RequiredBatchFileName = FbwfDisableBatchName;
			if(m_FbwfActiveChanging)
			{
				msgId = SvStl::Tid_DeactivatingDiskProtection;
			}
			else
			{
				msgId = SvStl::Tid_DiskProtectionRemainsInactive;
			}
		}

		std::string BatchfilePath = SvUl::Format( _T("\"%s\\%s\""), SvStl::GlobalPath::Inst().GetBinPath().c_str(), RequiredBatchFileName.c_str());
		auto ret = system( BatchfilePath.c_str() );
		if(ret)
		{
			msgId = SvStl::Tid_CouldNotExecuteFormatString;
			msgList.push_back( BatchfilePath );
			// undo selection in this case
			IsFbwfSelected = !IsFbwfSelected;
			m_FbwfActiveChanging = (m_FbwfActive != IsFbwfSelected);
			RootObject::setRootChildValue(SvDef::FqnEnvironmentAutoSave, IsFbwfSelected);
		}

		SvStl::MessageMgrStd toggleFbwfMessage( SvStl::LogAndDisplay );
		toggleFbwfMessage.setMessage( (ret ? SVMSG_SVO_86_FBWF_CHANGE_ERROR : SVMSG_SVO_85_FBWF_CHANGE), msgId, msgList, SvStl::SourceFileParams(StdMessageParams) );

		ReadCurrentFbwfSettings();
		ChangeStateActive = false;
	}
} 


bool ExtrasEngine::ReadCurrentFbwfSettings()
{
	bool Result{false};
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
		Result = (next != 0);
		m_FbwfActiveChanging = (current != next);
	}

	return Result;
}

bool ExtrasEngine::IsFbwfSelected()  const
{
	_variant_t Value;
	RootObject::getRootChildValue(SvDef::FqnEnvironmentDiskProtection, Value);
	return Value ? true : false;
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