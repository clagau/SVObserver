//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \Author	Arvid: Breitenbach
//*****************************************************************************
/// PLEASE 
/// ENTER 
/// A DESCRIPTION
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "resource.h"
#include "SVUserMessage.h"
#include "SVObserver.h" //Arvid: needed only for one call to SVObserverApp::fileSaveAsSVX()
#include "SVMainFrm.h"
#include "SVCmnLib\Utilities.h"
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

AutoSaver::AutoSaver():m_lastAutosaveTimestamp(0),m_AutosaveEnabled(true),m_AutoSaveDeltaTime_s(ms_defaultDeltaTimeInMinutes*ms_secondsPerMinute)//Arvid 10 minutes in seconds
{
	SetAutosaveTimestamp();
}

#pragma endregion Constructor

#pragma region Public Methods

AutoSaver& AutoSaver::Instance()
{
	static AutoSaver s_theAutoSaver; ///< Arvid: the one and only AutoSaverObject (initialized to a delta time of 10 minutes (this can be changed via setDeltaTimeInMinutes()))
	return s_theAutoSaver;
}

void AutoSaver::ExecuteAutosaveIfSelected(bool always)
{
	if(!IsEnabled())
	{
		return; //Arvid: not enabled by user:do nothing
	}

	if(!IsAutosaveTimestampOlderThanDeltaTime()) 
	{
		if(!always)
		{
			return; 
		}
	}	//Arvid: this means the last autosave was less than m_AutoSaveDeltaTime_s seconds ago and the autosave is not automatic: nothing to do!

	CWnd *pMainFrame=TheSVObserverApp.GetMainFrame(); 

	CDialog autosavePopupDialog; 
	autosavePopupDialog.Create(IDD_AUTOSAVE_IN_PROGRESS,pMainFrame);

	autosavePopupDialog.CenterWindow(pMainFrame);

	autosavePopupDialog.ShowWindow(SW_SHOW);
	autosavePopupDialog.RedrawWindow(); //Arvid: do this after ShowWindow(), otherwise no effect!

	//Arvid: ensure that the autosave directory exists.
	//Arvid: svObserverDirectoryPath should better not be hardcoded - but I could find no instance from which it could be obtained

	CString svObserverDirectoryPath="D:\\SVObserver";
	CString autosaveDirectoryPath="D:\\SVObserver\\Autosave";

	//Arvid: create the Autosave directory step by step
	CreateDirectory(svObserverDirectoryPath,NULL); //Arvid: this will fail if the directory already exists, but so what?
	CreateDirectory(autosaveDirectoryPath,NULL); //Arvid: this will fail if the directory already exists, but so what?
	moveContainedDirectory(autosaveDirectoryPath,"Temp2","Temp3");
	moveContainedDirectory(autosaveDirectoryPath,"Temp1MostRecent","Temp2");

	CreateDirectory(autosaveDirectoryPath+"\\Temp",NULL);

	//Arvid: save the current configuration
	TheSVObserverApp.fileSaveAsSVX(autosaveDirectoryPath+"\\Temp\\Autosave.svx",false);

	moveContainedDirectory(autosaveDirectoryPath,"Temp","Temp1MostRecent");

	//Arvid:	AutoSaver::Instance().SetAutosaveTimestamp() not required here: it was already done when the configuration was saved

	autosavePopupDialog.DestroyWindow();
}

void AutoSaver::SetAutosaveTimestamp()
{
	time(&m_lastAutosaveTimestamp);
}


#pragma endregion Public Methods

#pragma region Protected Methods

#pragma endregion Protected Methods

#pragma region Private Methods

bool AutoSaver::IsAutosaveTimestampOlderThanDeltaTime()
{
	time_t now;

	time(&now);

	double secondSinceLastAutosave=difftime(now, m_lastAutosaveTimestamp);

	bool isOlder = secondSinceLastAutosave > m_AutoSaveDeltaTime_s;

	return isOlder;
}


#pragma endregion Private Methods