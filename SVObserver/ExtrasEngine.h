//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015,2015 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
//*****************************************************************************
/// Contains the class ExtrasEngine which contains all functionality for menu entries 
/// in the Mainframe Pull-down Menu "Extras".
/// Currently: 
///		- Control of automatic backups of the current configuration
//******************************************************************************
#pragma once

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

	static const int ms_defaultDeltaTimeInMinutes=10;///< the minimum (for most purposes) autosave interval in seconds, m_AutoSaveDeltaTime_s is derived from it
	
	double m_AutoSaveDeltaTime_s; ///< the minimum (for most purposes) autosave interval in seconds
	time_t  m_lastAutoSaveTimestamp; ///< when was the latest automatic configuration backup done?

#pragma endregion Member Variables
};

