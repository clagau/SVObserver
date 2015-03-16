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

#pragma endregion Member Variables
};





