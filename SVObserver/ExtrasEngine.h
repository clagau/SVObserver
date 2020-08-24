//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Contains the class ExtrasEngine which contains all functionality for menu entries 
/// in the Mainframe Pull-down Menu "Extras".
/// Currently: 
///		- Control of automatic backups of the current configuration
///		- Control of the File Based Write Filter
//******************************************************************************
#pragma once

typedef ULONG	(__stdcall *FbwfIsFilterEnabledPtr) (PULONG currentSession, PULONG nextSession);

class ExtrasEngine
{
#pragma region Constructor
private:
	ExtrasEngine(const ExtrasEngine&); // no copy allowed, i.e. '= delete' in C++11;

public:
	ExtrasEngine();

#pragma endregion Constructor

#pragma region Public Methods
public:
	//************************************
	/// Poor man's Singleton: provides access to the one and only ExtrasEngine object
	/// \returns ExtrasEngine& the one and only ExtrasEngine object
	//************************************
	static ExtrasEngine& Instance();

	//************************************
	/// sets the new delta time
	/// \param deltaTime[in] the new delta time in minutes
	//************************************
	void setDeltaTimeInMinutes(int deltaTime){m_AutoSaveDeltaTime_s=60.0*deltaTime;}

	//************************************
	/// toggles the enable status
	//************************************
	void ToggleEnableAutoSave();
	bool IsAutoSaveEnabled() const;
	void SetAutoSaveEnabled(bool enabled);

	/// Changes the FBWF status that will be active after the next reboot (and displays appropriate message boxes)
	void ChangeFbwfState();

	//! Reads the current Fbwf settings
	//! \returns true if Fbwf is active
	bool ReadCurrentFbwfSettings();

	bool IsFbwfAvailable() const {return m_FbwfAvailable;}
	bool IsFbwfActive()    const {return m_FbwfActive;}
	bool IsFbwfSelected()  const;
	bool IsFbwfChanging()  const {return m_FbwfActiveChanging;}

	//************************************
	/// performs an automatic configuration backup if enabled and required (when always is false, 
	/// only if IsAutoSaveTimestampOlderThanDeltaTime() returns true)
	/// \param always [in] forces an autosave regardless of last autosave timestamp)
	//************************************
	void ExecuteAutoSaveIfAppropriate(bool always);

	//************************************
	/// updates the autosave timestamp and resets the "autosave required" flag
	//************************************
	void ResetAutoSaveInformation();

#pragma endregion Public Methods

#pragma region Private Methods
private:

	//************************************
	/// Determines whether the last autosave happened at least 10 minutes ago
	/// \returns bool is the autosave timestamp old enough to warrant an autosave?
	//************************************
	bool IsAutoSaveTimestampOlderThanDeltaTime();

#pragma endregion Private Methods

#pragma region Member Variables

public: 
	static HINSTANCE ms_FbwfDllInstance; ///< the handle to the DLL where m_pfnFbwfIsFilterEnabled will be looked for
private:
	static const int ms_defaultDeltaTimeInMinutes=10;///< the minimum (for most purposes) autosave interval in seconds, m_AutoSaveDeltaTime_s is derived from it
	
	FbwfIsFilterEnabledPtr m_pfnFbwfIsFilterEnabled; ///< the function pointer modelled on FbwfIsFilterEnabled() in fbwfapi.h
	double m_AutoSaveDeltaTime_s; ///< the minimum (for most purposes) autosave interval in seconds
	time_t  m_lastAutoSaveTimestamp; ///< when was the latest automatic configuration backup done?

	bool m_FbwfAvailable;		///< is FBWF functionality available (i..e installed and DLL found)? 
	bool m_FbwfActive;			///< is FBWF functionality currently active?
	bool m_FbwfActiveChanging;	///< will FBWF functionality be active after the next reboot?

#pragma endregion Member Variables
};

