//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015, 2015 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
//*****************************************************************************
/// Contains the class ExtrasEngine which contains functionality for actions
/// chosen from the Extras Pull down in the SVObserver Main Menu
//*****************************************************************************

#include "stdafx.h"
#include "ConfigurationOuttakes.h"
#include "ExtrasEngine.h"
#include "RootObject.h"
#include "SVMainFrm.h"
#include "Definitions/GlobalConst.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVStatusLibrary/SvimState.h"
#include "SVUtilityLibrary/SVClock.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

ExtrasEngine::ExtrasEngine() : m_lastAutoSaveTimestamp(0),
m_AutoSaveDeltaTime_s(ms_defaultDeltaTimeInMinutes* SvUl::c_secondsPerMinute)
{
	ResetAutoSaveInformation();
}

ExtrasEngine& ExtrasEngine::Instance()
{
	static ExtrasEngine s_theExtrasEngine; ///< the one and only ExtrasEngine
	return s_theExtrasEngine;
}

void ExtrasEngine::ExecuteAutoSaveIfAppropriate(bool always)
{
	if(!(IsAutoSaveEnabled() && SvimState::IsAutoSaveRequired()) || SvimState::isModuleEditing())
	{
		return; //not enabled by user (or configuration not modified) or Module is editing:do nothing
	}

	if(!IsAutoSaveTimestampOlderThanDeltaTime()) 
	{
		if(!always)
		{
			return; 
		}
	}	//this means the last autosave was less than m_AutoSaveDeltaTime_s seconds ago and the autosave is not automatic: nothing to do!

	CWnd *pMainFrame = GetSvoMainFrame();

	CDialog autosavePopupDialog; 
	autosavePopupDialog.Create(IDD_AUTOSAVE_IN_PROGRESS, pMainFrame);

	autosavePopupDialog.CenterWindow(pMainFrame);

	autosavePopupDialog.ShowWindow(SW_SHOW);
	autosavePopupDialog.RedrawWindow(); //do this after ShowWindow(), otherwise no effect!

	SvStl::GlobalPath& rGlobalPath = SvStl::GlobalPath::Inst();
	//ensure that the autosave directory exists: create it step by step
	if(-1 == _access(rGlobalPath.GetSecondObserverPath().c_str(), 0))
	{
		::CreateDirectory(rGlobalPath.GetSecondObserverPath().c_str(), nullptr);
	}
	std::string  AutosavePath = rGlobalPath.GetAutoSaveRootPath();
	if (-1 == _access(AutosavePath.c_str(), 0))
	{
		::CreateDirectory(AutosavePath.c_str(), nullptr);
	}

	//now move the temporary files around (if they exist already)
	std::string Temp1Name {rGlobalPath.GetAutoSaveRootPath(rGlobalPath.GetAutosaveTemp1FileName().c_str())};
	std::string Temp2Name {rGlobalPath.GetAutoSaveRootPath(rGlobalPath.GetAutosaveTemp2FileName().c_str())};
	std::string Temp3Name {rGlobalPath.GetAutoSaveRootPath(rGlobalPath.GetAutosaveTemp3FileName().c_str())};

	::remove(Temp3Name.c_str());
	::MoveFile(Temp2Name.c_str(), Temp3Name.c_str());
	::MoveFile(Temp1Name.c_str(), Temp2Name.c_str());

	//save the current configuration in the AutoSave Directory
	fileSaveAsSVXWrapper(Temp1Name, true);

	autosavePopupDialog.DestroyWindow();
}


void ExtrasEngine::ResetAutoSaveInformation()
{
	time(&m_lastAutoSaveTimestamp);
	SvimState::SetAutoSaveRequired(false);
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

bool ExtrasEngine::IsAutoSaveTimestampOlderThanDeltaTime()
{
	time_t now;

	time(&now);

	double secondSinceLastAutoSave = difftime(now, m_lastAutoSaveTimestamp);

	bool isOlder = secondSinceLastAutoSave > m_AutoSaveDeltaTime_s;

	return isOlder;
}

