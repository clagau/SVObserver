//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file ShareEvents.h 
/// All Rights Reserved 
//*****************************************************************************
//Singleton class handles two global events 
///The events are used to signal status of shared memory  
//******************************************************************************
#pragma once
#pragma region Includes
//Moved to precompiled header: #include <atomic>
#pragma endregion Includes

namespace SvSml
{
struct ShareControlSettings;

//!Singleton class handles two global events 
///The events are used to signal status of shared memory  
class ShareEvents
{
private:
	ShareEvents();
	ShareEvents(const ShareEvents&) = delete;
	~ShareEvents();
	static DWORD WINAPI WatchFunction(LPVOID lpParam);


public:
typedef enum { Change, Ready, UKnown } CallBackParam;

	static ShareEvents& GetInstance()
	{
		static ShareEvents instanz;
		return instanz;
	}

	//!SetReadyFlag  
	void SignaltReadyStatus();

	//!SetChangingFlag  
	void SignalChangingStatus();

	void QuiesceSharedMemory();

	//! Starts a thread which monitor readyflag 
	bool StartWatch();

	//!Stops the Watchthread
	void StopWatch();
	//!callbackfunction for watch thread 
	void SetCallbackFunction(boost::function<bool(DWORD)>cbFct);

	//! True if InitEventIsSet
	bool GetIsReady() const;

	//! The Ready counter is incremented when the ready flag is set to true
	//!The ready Counter is used in RRS because there exist two Nonitorlistcopies 
	long GetReadyCounter() const;

	//!True if Init Flag isSet
	bool GetIsInit() const;

	//!Set Init flag. The init flag will bee reseted when changing event occurs
	void SetIsInit();
	void SetParameter(const ShareControlSettings& ControlParameter);
	
private:
	static const LPCTSTR GNameChangeEvent;
	static const LPCTSTR GNameReadyEvent;

	std::atomic<bool> m_isReady;
	std::atomic<bool> m_isInit;
	std::atomic<long> m_readyCounter;


	boost::function<bool(DWORD)> m_CallBackFct;
	HANDLE m_hChangeEvent {NULL};
	HANDLE m_hReadyEvent {NULL};

	///Handles for WatchThread
	HANDLE m_StopEvent {NULL};
	HANDLE m_hWatchThread {NULL};
	DWORD m_ThreadId {0};
	DWORD m_DelayBeforeCreateShare {60}; //SVObserver wait time after setting Change Event
	

};
} //namespace SvSml
