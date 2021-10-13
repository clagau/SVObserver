//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision, Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSVIMStateClass
//* .File Name       : $Workfile:   SVSVIMStateClass.h
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   09 Dec 2014 10:12:42  $
//******************************************************************************

#pragma once
#pragma region Includes
//Moved to precompiled header: #include <atomic>
//Moved to precompiled header: #include <functional>
#include "SVProtoBuf/SVRC-Enum.h"
#pragma endregion Includes

#define SV_STATE_UNKNOWN		0x00000000

#define SV_STATE_AVAILABLE		0x01000000
#define SV_STATE_READY			0x02000000
#define SV_STATE_RUNNING		0x04000000
#define SV_STATE_UNAVAILABLE	0x08000000

#define SV_STATE_CREATING		0x00100000
#define SV_STATE_LOADING		0x00200000
#define SV_STATE_SAVING			0x00400000
#define SV_STATE_CLOSING		0x00800000

#define SV_STATE_EDITING		0x00010000
#define SV_STATE_CANCELING		0x00020000
#define SV_STATE_INTERNAL_RUN	0x00040000

#define SV_STATE_START_PENDING	0x00001000
#define SV_STATE_STARTING		0x00002000
#define SV_STATE_STOP_PENDING	0x00004000
#define SV_STATE_STOPING		0x00008000

#define SV_STATE_TEST			0x00000100
#define SV_STATE_REGRESSION		0x00000200
#define SV_STATE_EDIT			0x00000400
#define SV_STATE_STOP			0x00000800

#define SV_STATE_MODIFIED		0x00000001
#define SV_STATE_REMOTE_CMD		0x00000002

typedef std::function<void(SvPb::NotifyType, const _variant_t&)> NotifyFunctor;

//This class manages the state variable and uses a lock to 
//
//This class supports the following states:
//AVAILABLE - SVIM waiting for a configuration.
//READY - SVIM has a valid configuration loaded.
//RUNNING - SVIM on-line (processing inspection documents).
//UNAVAILABLE - SVIM is busy or cannot respond to a command 
//at this time.
class SVSVIMStateClass
{
public:

	//RAII helper struct for m_LockCountSvrc
	struct SVRCBlocker
	{
	public:
		SVRCBlocker()
		{
			SVSVIMStateClass::m_LockCountSvrc++;
		}
		~SVRCBlocker()
		{
			SVSVIMStateClass::m_LockCountSvrc--;
		}

	};

	//RAII class to set and reset the state
	struct SetResetState
	{
		explicit SetResetState(DWORD state) :
			m_state(state)
		{
			SVSVIMStateClass::AddState(m_state);
		}

		~SetResetState()
		{
			SVSVIMStateClass::RemoveState(m_state);
		}
	private:
		DWORD m_state;
	};


	static long GetState() { return m_SVIMState; }

	//This operation adds a sub-state to the existing state 
	//value.  The value passed in as a parameter is ORed to 
	//the existing value.
	static void AddState( DWORD dwState );

	//This operation removes a particular sub-state from the 
	//existing state value.  This process takes the sub-state 
	//value and inverts it and ANDs it to the existing state 
	//value.
	static void RemoveState( DWORD dwState );

	//This operation checks the parameter state value against 
	//the internal value and outputs in the result parameter 
	//whether there is at least one bit (state) matching.
	static bool CheckState( DWORD dwState );

	/// This method do RemoveState and CheckState at once to avoid to send two notification
	/// \param addStates [in] States which should be added.
	/// \param removeStates [in] States which should be removed.
	static void changeState(DWORD addStates, DWORD removeStates);

	static SvPb::DeviceModeType getCurrentMode() { return m_CurrentMode; }

	static __time64_t getLastModifiedTime() { return m_lastModifiedTime; }

	static __time64_t getLoadedSinceTime() { return m_loadedSinceTime;}

	//************************************
	//! Sets the notification function
	//! \param  [in] pointer to the notification function
	//************************************
	static void setNotificationFunction(const NotifyFunctor& Notify);

	static bool IsAutoSaveRequired() { return m_AutoSaveRequired; }
	static void SetAutoSaveRequired(bool required) { m_AutoSaveRequired = required; }

	static bool isSvrcBlocked();

	static SvPb::DeviceModeType GetMode();
	
	static std::string GetHash();
	static void ConfigWasLoaded(LPCSTR hash = nullptr);
	static void ConfigWasUnloaded();
private:
	//************************************
	// Method: CheckModeNotify
	// Description: Determine if the mode has changed and fire the notification.
	// Returns: void
	//************************************
	static void CheckModeNotify();

	
	static  void SetHash(LPCSTR hash);
	
	static void setLastModifiedTime();

	//This constructor does nothing.
	SVSVIMStateClass();

	//This destructor does nothing.
	virtual ~SVSVIMStateClass();

	static NotifyFunctor m_pNotify;	//! Notify functor when state changes

	//This attribute contain the SVIM state value.
	static std::atomic_long m_SVIMState;

	static std::atomic<SvPb::DeviceModeType> m_CurrentMode;

	static std::atomic<__time64_t> m_lastModifiedTime;
	static std::atomic<__time64_t> m_loadedSinceTime;
	static std::mutex m_protectHash;
	static std::string m_hash;
	
	static bool m_AutoSaveRequired; ///< should an autosave be performed at the next appropriate time?
	///Lockcount >  0 prevents some  SVRC command to avoid crashes because of mult threading issues 
	static std::atomic<int>  m_LockCountSvrc; //<  
};


