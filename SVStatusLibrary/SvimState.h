//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SvimState
//* .File Name       : $Workfile:   SvimState.h
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   09 Dec 2014 10:12:42  $
//******************************************************************************

#pragma once
#pragma region Includes
#include "SVProtoBuf/SVRC-Enum.h"
#pragma endregion Includes

constexpr DWORD SV_STATE_UNKNOWN		= 0x00000000;

constexpr DWORD SV_STATE_AVAILABLE		= 0x01000000;
constexpr DWORD SV_STATE_READY			= 0x02000000;
constexpr DWORD SV_STATE_RUNNING		= 0x04000000;
constexpr DWORD SV_STATE_UNAVAILABLE	= 0x08000000;

constexpr DWORD SV_STATE_CREATING		= 0x00100000;
constexpr DWORD SV_STATE_LOADING		= 0x00200000;
constexpr DWORD SV_STATE_SAVING			= 0x00400000;
constexpr DWORD SV_STATE_CLOSING		= 0x00800000;

constexpr DWORD SV_STATE_EDITING		= 0x00010000;
constexpr DWORD SV_STATE_CANCELING		= 0x00020000;
constexpr DWORD SV_STATE_INTERNAL_RUN	= 0x00040000;

constexpr DWORD SV_STATE_START_PENDING	= 0x00001000;
constexpr DWORD SV_STATE_STARTING		= 0x00002000;
constexpr DWORD SV_STATE_STOP_PENDING	= 0x00004000;
constexpr DWORD SV_STATE_STOPING		= 0x00008000;

constexpr DWORD SV_STATE_TEST			= 0x00000100;
constexpr DWORD SV_STATE_REGRESSION		= 0x00000200;
constexpr DWORD SV_STATE_EDIT			= 0x00000400;
constexpr DWORD SV_STATE_STOP			= 0x00000800;
	
constexpr DWORD SV_STATE_MODIFIED		= 0x00000001;
constexpr DWORD SV_STATE_REMOTE_CMD		= 0x00000002;

constexpr DWORD SV_DEFAULT_NOT_ALLOWED_STATES = 
SV_STATE_UNAVAILABLE | SV_STATE_CREATING | SV_STATE_LOADING | SV_STATE_SAVING | SV_STATE_CLOSING | SV_STATE_EDITING |
SV_STATE_CANCELING | SV_STATE_INTERNAL_RUN | SV_STATE_START_PENDING | SV_STATE_STARTING | SV_STATE_STOP_PENDING | SV_STATE_STOPING | SV_STATE_REMOTE_CMD;


typedef std::function<void(SvPb::NotifyType, const _variant_t&)> NotifyFunctor;

//This class manages the state variable and uses a lock to 
//
//This class supports the following states:
//AVAILABLE - SVIM waiting for a configuration.
//READY - SVIM has a valid configuration loaded.
//RUNNING - SVIM on-line (processing inspection documents).
//UNAVAILABLE - SVIM is busy or cannot respond to a command 
//at this time.


class SvimState
{
public:
	//RAII helper struct for ms_LockCountSvrc
	struct SVRCBlocker
	{
	public:
		SVRCBlocker()
		{
			SvimState::ms_LockCountSvrc++;
		}
		~SVRCBlocker()
		{
			SvimState::ms_LockCountSvrc--;
		}

	};

	//RAII class to set and reset the state
	class SetResetState
	{

	public:
		explicit SetResetState(DWORD state, std::function<bool()> acquire = [](){return true; }, std::function<void()> release = [](){}) :
			m_state(state), m_ok(acquire()), m_release(release)
		{
			if(m_ok)
			{
				bool addState = true;
				if (m_state == SV_STATE_EDITING)
				{
					if (++ms_EditingStates > 1) // i.e. we _are_ already in that state: we don't enter it again!
					{
						addState = false;
					}
				}
				if (addState)
				{
					SvimState::AddState(m_state);
				}
			}
		}

		~SetResetState()
		{
			if (m_ok)
			{
				bool removeState = true;
				if (m_state == SV_STATE_EDITING)
				{
					removeState = (--ms_EditingStates <= 0);
					// i.e., we are currently leaving an editing state ON TOP on another editing state. Hence, we do not remove the flag yet!
				}

				if (removeState)
				{
					m_release();
					SvimState::RemoveState(m_state);
				}
			}
		}

		bool conditionOk() const
		{
			return m_ok;
		}

	private:

		const DWORD m_state;
		const bool m_ok;
		std::function<void()> m_release;
	};

	static void OutputDebugState();
	static long GetState() { return ms_fullState; }

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

	static HRESULT CheckNotAllowedState(DWORD States = SV_DEFAULT_NOT_ALLOWED_STATES);

	/// This method do RemoveState and CheckState at once to avoid to send two notification
	/// \param addStates [in] States which should be added.
	/// \param removeStates [in] States which should be removed.
	static void changeState(DWORD addStates, DWORD removeStates);

	static SvPb::DeviceModeType getCurrentMode() { return ms_CurrentMode; }

	static __time64_t getLastModifiedTime() { return ms_lastModifiedTime; }

	static __time64_t getLoadedSinceTime() { return ms_loadedSinceTime;}

	//************************************
	//! Sets the notification function
	//! \param  [in] pointer to the notification function
	//************************************
	static void setNotificationFunction(const NotifyFunctor& Notify);

	static bool IsAutoSaveRequired() { return ms_AutoSaveRequired; }
	static void SetAutoSaveRequired(bool required) { ms_AutoSaveRequired = required; }

	static bool isSvrcBlocked();

	static SvPb::DeviceModeType GetMode();
	
	static std::string GetHash();
	static void ConfigWasLoaded(LPCSTR hash = nullptr);
	static void ConfigWasUnloaded();

	static bool IsReloadedAfterCopyToolsToClipboard() { return ms_isReloadedAfterCopyToolsToClipboard; };
	static void SetReloadAfterCopyToolsToClipboard(bool flag) { ms_isReloadedAfterCopyToolsToClipboard = flag; };


private:
	//************************************
	// Method: CheckModeNotify
	// Description: Determine if the mode has changed and fire the notification.
	// Returns: void
	//************************************
	static void CheckModeNotify();
	
	static  void SetHash(LPCSTR hash);
	
	static void setLastModifiedTime();

	SvimState() = delete;

	virtual ~SvimState() = delete;

	static NotifyFunctor ms_notify;	//! Notify functor when state changes

	//This attribute contain the SVIM state value.
	static std::atomic_long ms_fullState;

	static std::atomic<SvPb::DeviceModeType> ms_CurrentMode;

	static std::atomic<__time64_t> ms_lastModifiedTime;
	static std::atomic<__time64_t> ms_loadedSinceTime;
	static std::mutex ms_protectHash;
	static std::string ms_hash;

	static bool ms_AutoSaveRequired; ///< should an autosave be performed at the next appropriate time?
	static std::atomic<int>  ms_LockCountSvrc; //< ms_LockCountSvrc >  0 prevents some  SVRC command to avoid crashes because of multi threading issues 

	static std::atomic<bool> ms_isReloadedAfterCopyToolsToClipboard;
	static int64_t ms_EditingStates;
};


