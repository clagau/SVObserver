//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \Author	Arvid Breitenbach
//*****************************************************************************
/// Contains the class AutoSaver which contains all functionality to 
/// control periodic backups of the current configuration
/// A DESCRIPTION
//******************************************************************************
#pragma once

#pragma region Includes
#pragma endregion Includes

#pragma region Declarations
#pragma endregion Declarations

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
	void ToggleEnable(){m_AutosaveEnabled=!m_AutosaveEnabled;}
	bool IsEnabled(){return m_AutosaveEnabled;}

	//************************************
	/// performs an automatic configuration backup (when always is false, 
	/// only if IsAutosaveTimestampOlderThanDeltaTime() returns true)
	/// \param always [in] forces an autosave regardless of last autosave timestamp)
	//************************************
	void ExecuteAutosaveIfSelected(bool always);

	//************************************
	/// updates the autosave timestamp
	//************************************
	void SetAutosaveTimestamp();

#pragma endregion Public Methods

#pragma region Protected Methods
protected:

#pragma endregion Protected Methods

#pragma region Private Methods
private:
	//************************************
	/// Determines whether the last autosave happened at least 10 minutes ago
	/// \returns bool is the autosave timestamp old enough to warrant an autosave?
	//************************************
	bool IsAutosaveTimestampOlderThanDeltaTime();

#pragma endregion Private Methods

#pragma region Member Variables

private:
	static const int ms_defaultDeltaTimeInMinutes=10;///< the minimum (for most purposes) autosave interval in seconds, m_AutoSaveDeltaTime_s is derived from it
	static const int ms_secondsPerMinute=60; ///< the number of seconds per minute //@TODO[Arvid] this should not be member of this class

	double m_AutoSaveDeltaTime_s; ///< the minimum (for most purposes) autosave interval in seconds
	time_t  m_lastAutosaveTimestamp; ///< when was the latest automatic configuration backup done?
	bool m_AutosaveEnabled; ///< is automatic saving of configurations enabled?

#pragma endregion Member Variables
};





