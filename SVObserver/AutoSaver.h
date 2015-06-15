//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \Author	Arvid Breitenbach
//*****************************************************************************
/// Contains the class AutoSaver which contains all functionality to 
/// control automatic backups of the current configuration
//******************************************************************************
#pragma once

class AutoSaver
{
#pragma region Constructor
public:

	AutoSaver();

#pragma endregion Constructor

#pragma region Public Methods
public:
	//************************************
	/// Poor man's Singleton: provides access to the one and only AutoSaver object
	/// \returns AutoSaver& the one and only AutoSaver object
	//************************************
	static AutoSaver& Instance();

	//************************************
	/// sets the new delta time
	/// \param deltaTime[in] the new delta time in minutes
	//************************************
	void setDeltaTimeInMinutes(int deltaTime){m_AutoSaveDeltaTime_s=60.0*deltaTime;}

	//************************************
	/// toggles the enable status
	//************************************
	void ToggleEnable(){m_AutoSaveEnabled=!m_AutoSaveEnabled;}
	bool IsEnabled(){return m_AutoSaveEnabled;}
	void SetEnabled(bool enabled){m_AutoSaveEnabled = enabled;}

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

	/// returns the full autosave temp directory path. (cf. comment to CopyDirectoryToTempDirectory())
	CString GetTempDirectoryPath() const {return m_defaultAutoSavePath+GetTempFolderRelPath()+_T("\\");}

	/// returns path of the autosave temp directory path relative to the main autosave directory
	CString GetTempFolderRelPath() const {return _T("\\Temp");}

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

private:
	static const int ms_defaultDeltaTimeInMinutes=10;///< the minimum (for most purposes) autosave interval in seconds, m_AutoSaveDeltaTime_s is derived from it
	static const int ms_secondsPerMinute=60; ///< the number of seconds per minute //@TODO ideally, this should not be member of this class (the number of seconds in a minute has nothing to do with the autosave functionality) but of some module with globals constants

	double m_AutoSaveDeltaTime_s; ///< the minimum (for most purposes) autosave interval in seconds
	time_t  m_lastAutoSaveTimestamp; ///< when was the latest automatic configuration backup done?
	bool m_AutoSaveEnabled; ///< is automatic saving of configurations enabled?
	bool m_AutoSaveRequired ; ///< should an autosave be performed at the next appropriate time?

	const CString m_SVObserverDirectoryPath; ///< the SVObserver directory (not the main one but the one on drive D:)
	const CString m_defaultAutoSavePath; ///< the SVObserver autosave directory


#pragma endregion Member Variables
};





