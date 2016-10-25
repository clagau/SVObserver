//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \Author	Arvid Breitenbach
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
	void ToggleEnable(){m_AutoSaveEnabled=!m_AutoSaveEnabled;}
	bool IsEnabled() const {return m_AutoSaveEnabled;}
	void SetEnabled(bool enabled){m_AutoSaveEnabled = enabled;}

	/// toggles the FBWF status that will be active after the next reboot (and displays appropriate message boxes)
	void ToggleEnableFbwf();

	/// toggles the FBWF status (current and after reboot) using the fbwf api function FbwfIsFilterEnabled()
	void ReadCurrentFbwfSettings(bool onStart = false);

	bool IsFbwfAvailable() const {return m_FbwfAvailable;}		//AB simple getter 
	bool IsFbwfActive()    const {return m_FbwfActive;}			//AB simple getter 
	bool IsFbwfSelected()  const {return m_IsFbwfSelected;}		//AB simple getter 
	bool IsFbwfChanging()  const {return m_FbwfActiveChanging;} //AB simple getter 

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

	/// Used for copying the contents of sourceDir to the Autosave temp directory path.
	/// Since this path (in keeping with the somewhat unfortunate specification for SVO-168)
	/// does not always exist care must be exercised in using this function
	/// \param rSourceDir [in] the path of the directory the contents of which are to be copied
	void CopyDirectoryToTempDirectory(const CString &rSourceDir) const ;

	bool IsAutoSaveRequired(){return m_AutoSaveRequired;}
	void SetAutoSaveRequired(bool required){m_AutoSaveRequired=required;}

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
	
	static const CString ms_ObserverPath;         

	FbwfIsFilterEnabledPtr m_pfnFbwfIsFilterEnabled; ///< the function pointer modelled on FbwfIsFilterEnabled() in fbwfapi.h
	double m_AutoSaveDeltaTime_s; ///< the minimum (for most purposes) autosave interval in seconds
	time_t  m_lastAutoSaveTimestamp; ///< when was the latest automatic configuration backup done?
	bool m_AutoSaveEnabled; ///< is automatic saving of configurations enabled?
	bool m_AutoSaveRequired ; ///< should an autosave be performed at the next appropriate time?

	bool m_FbwfAvailable;		///< is FBWF functionality available (i..e installed and DLL found)? 
	bool m_IsFbwfSelected;		///< is FBWF functionality selected to be available after the next bootup
	bool m_FbwfActive;			///< is FBWF functionality currently active?
	bool m_FbwfActiveChanging;	///< will FBWF functionality be active after the next reboot?

#pragma endregion Member Variables
};

