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
#include "svModeEnum.h"

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

#define SV_STATE_SECURED		0x00000010
#define SV_STATE_RAID_FAILURE	0x00000020

#define SV_STATE_MODIFIED		0x00000001
#define SV_STATE_REMOTE_CMD		0x00000002

typedef boost::function<HRESULT(int, int, LPCTSTR)> NotifyFunctor;

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

	//This operation adds a sub-state to the existing state 
	//value.  The value passed in as a parameter is ORed to 
	//the existing value.
	static bool AddState( DWORD dwState );

	//This operation removes a particular sub-state from the 
	//existing state value.  This process takes the sub-state 
	//value and inverts it and ANDs it to the existing state 
	//value.
	static bool RemoveState( DWORD dwState );

	//This operation checks the parameter state value against 
	//the internal value and outputs in the result parameter 
	//whether there is at least one bit (state) matching.
	static bool CheckState( DWORD dwState );

	static svModeEnum getCurrentMode() { return m_CurrentMode; }
	static svModeEnum getPreviousMode() { return m_PreviousMode; }

	static __time32_t getCurrentTime() { return m_CurrentModifiedTime; }
	static __time32_t getPreviousTime() { return m_PreviousModifiedTime; }

	//************************************
	//! Sets the notification function
	//! \param  [in] pointer to the notification function
	//************************************
	static void setNotificationFunction(NotifyFunctor Notify);

	static void setPreviousToCurrentMode();
	static void setPreviousToCurrentTime();

	static bool IsAutoSaveRequired() { return m_AutoSaveRequired; }
	static void SetAutoSaveRequired(bool required) { m_AutoSaveRequired = required; }

private:
	//************************************
	// Method: CheckModeNotify
	// Description: Determine if the mode has changed and fire the notification.
	// Returns: void
	//************************************
	static void CheckModeNotify();

	//************************************
	// Method:    GetMode
	// Description:  Get the value of the mode
	// Returns:   svModeEnum Enum of the mode
	//************************************
	static svModeEnum GetMode();

	static HRESULT SetLastModifiedTime();
	static HRESULT FireModeChanged();

	//This constructor does nothing.
	SVSVIMStateClass();

	//This destructor does nothing.
	virtual ~SVSVIMStateClass();

	static NotifyFunctor m_Notify;	//! Notify functor when state changes

	//This attribute contain the SVIM state value.
	static long m_SVIMState;

	static volatile svModeEnum m_PreviousMode;
	static volatile svModeEnum m_CurrentMode;

	static volatile __time32_t m_PreviousModifiedTime;
	static volatile __time32_t m_CurrentModifiedTime;

	static bool m_AutoSaveRequired; ///< should an autosave be performed at the next appropriate time?
};

