//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015, 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \Author	Arvid: Breitenbach
//*****************************************************************************
/// Contains the class AutoSaver which contains all functionality to 
/// control automatic backups of the current configuration
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "ObjectInterfaces/SVUserMessage.h"
#include "SVObserver.h" //Arvid: needed only for one call to SVObserverApp::fileSaveAsSVX()
#include "SVMainFrm.h"
#include "SVCmnLib\Utilities.h"
#include "SVSVIMStateClass.h"
#include "AutoSaver.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations



#pragma region Constructor

AutoSaver::AutoSaver():m_lastAutoSaveTimestamp(0), m_AutoSaveEnabled(true), m_AutoSaveRequired(false), 
	m_AutoSaveDeltaTime_s(ms_defaultDeltaTimeInMinutes*ms_secondsPerMinute),
	m_SVObserverDirectoryPath(_T("D:\\SVObserver")),m_defaultAutoSavePath(_T("D:\\SVObserver\\Autosave"))
{
	ResetAutoSaveInformation();
}

#pragma endregion Constructor

#pragma region Public Methods

AutoSaver& AutoSaver::Instance()
{
	static AutoSaver s_theAutoSaver; ///< Arvid: the one and only AutoSaverObject (initialized to a delta time of 10 minutes (this can be changed via setDeltaTimeInMinutes()))
	return s_theAutoSaver;
}

void AutoSaver::ExecuteAutoSaveIfAppropriate(bool always)
{
	if(!(IsEnabled() && IsAutoSaveRequired()))
	{
		return; //Arvid: not enabled by user (or configuration not modified):do nothing
	}

	if(!IsAutoSaveTimestampOlderThanDeltaTime()) 
	{
		if(!always)
		{
			return; 
		}
	}	//Arvid: this means the last autosave was less than m_AutoSaveDeltaTime_s seconds ago and the autosave is not automatic: nothing to do!

	CWnd *pMainFrame = TheSVObserverApp.GetMainFrame(); 

	CDialog autosavePopupDialog; 
	autosavePopupDialog.Create(IDD_AUTOSAVE_IN_PROGRESS, pMainFrame);

	autosavePopupDialog.CenterWindow(pMainFrame);

	autosavePopupDialog.ShowWindow(SW_SHOW);
	autosavePopupDialog.RedrawWindow(); //Arvid: do this after ShowWindow(), otherwise no effect!

	//Arvid: ensure that the autosave directory exists.
	//Arvid: m_SVObserverDirectoryPath should better not be hardcoded - but I could find no instance from which it could be obtained

	//Arvid: create the AutoSave directory step by step
	CreateDirectory(m_SVObserverDirectoryPath, NULL); //Arvid: this will fail if the directory already exists, but so what?
	CreateDirectory(m_defaultAutoSavePath, NULL); //Arvid: this will fail if the directory already exists, but so what?
	//Arvid: now move the temporary directories around (if they exist already)
	moveContainedDirectory(m_defaultAutoSavePath, _T("Temp2"), _T("Temp3"));
	moveContainedDirectory(m_defaultAutoSavePath, _T("Temp1MostRecent"), _T("Temp2"));

	CreateDirectory(GetTempFolderRelPath(), NULL);

	//Arvid: save the current configuration in the AutoSave Directory
	TheSVObserverApp.fileSaveAsSVX(_T(""), true);

	moveContainedDirectory(m_defaultAutoSavePath, _T("Temp"), _T("Temp1MostRecent"));

	autosavePopupDialog.DestroyWindow();
}

void AutoSaver::ResetAutoSaveInformation()
{
	time(&m_lastAutoSaveTimestamp);
	SetAutoSaveRequired(false);
}

void AutoSaver::CopyDirectoryToTempDirectory(const CString &rSourceDir) const 
{
	CopyDir(rSourceDir, GetTempDirectoryPath());
}

#pragma endregion Public Methods

#pragma region Private Methods

bool AutoSaver::IsAutoSaveTimestampOlderThanDeltaTime()
{
	time_t now;

	time(&now);

	double secondSinceLastAutoSave = difftime(now, m_lastAutoSaveTimestamp);

	bool isOlder = secondSinceLastAutoSave > m_AutoSaveDeltaTime_s;

	return isOlder;
}


#pragma endregion Private Methods