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
#include "SVUserMessage.h"
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
	m_AutoSaveDeltaTime_s(ms_defaultDeltaTimeInMinutes*ms_secondsPerMinute)
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
	//Arvid: svObserverDirectoryPath should better not be hardcoded - but I could find no instance from which it could be obtained

	CString svObserverDirectoryPath = "D:\\SVObserver";
	CString autosaveDirectoryPath = "D:\\SVObserver\\Autosave";

	//Arvid: create the AutoSave directory step by step
	CreateDirectory(svObserverDirectoryPath, NULL); //Arvid: this will fail if the directory already exists, but so what?
	CreateDirectory(autosaveDirectoryPath, NULL); //Arvid: this will fail if the directory already exists, but so what?
	moveContainedDirectory(autosaveDirectoryPath, "Temp2", "Temp3");
	moveContainedDirectory(autosaveDirectoryPath, "Temp1MostRecent", "Temp2");

	CreateDirectory(autosaveDirectoryPath+"\\Temp", NULL);

	//Arvid: save the current configuration
	CString path=autosaveDirectoryPath + "\\Temp\\Autosave.svx";
	bool isRegular = false;
	TheSVObserverApp.fileSaveAsSVX(path, isRegular);

	moveContainedDirectory(autosaveDirectoryPath, "Temp", "Temp1MostRecent");

	autosavePopupDialog.DestroyWindow();
}

void AutoSaver::ResetAutoSaveInformation()
{
	time(&m_lastAutoSaveTimestamp);
	SetAutoSaveRequired(false);
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