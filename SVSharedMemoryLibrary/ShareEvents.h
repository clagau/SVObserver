//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file ShareEvents.h 
/// All Rights Reserved 
//*****************************************************************************
//Singleton class handles two global events 
///The events are used to signal status of shared memory  
//******************************************************************************
#pragma once

namespace SvSml
{
	//!Singleton class handles two global events 
	///The events are used to signal status of shared memory  
	class ShareEvents
	{
	private:
		ShareEvents();
		ShareEvents( const ShareEvents& );
		~ShareEvents();
		static DWORD WINAPI WatchFunction( LPVOID lpParam );
		

	public:
		const static DWORD Delay_Before_CreateShare;
		const  static DWORD Delay_Before_ClearShare;
		typedef enum  {Change, Ready,UKnown} CallBackParam;

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
		//!callbackfunction for watch thread 
		void SetCallbackFunction(boost::function<bool(DWORD )>cbFct);  

		//! True if InitEventIsSet
		bool GetIsReady() const;
		
		//! The Ready counter is incremented when the ready flag is set to true
		//!The ready Counter is used in RRS because there exist two Nonitorlistcopies 
		long ShareEvents::GetReadyCounter() const;

		//!True if Init Flag isSet
		bool GetIsInit() const;
		
		//!Set Init flag. The init flag will bee reseted when changing event occurs
		void SetIsInit();

	private:
		static const LPCTSTR GNameChangeEvent;
		static const LPCTSTR GNameReadyEvent;
		
		
		
		mutable bool m_IsReady;
		mutable bool m_IsInit;
		mutable long m_ReadyCounter;

		
		boost::function<bool(DWORD )> m_CallBackFct;
		HANDLE m_hChangeEvent;
		HANDLE m_hReadyEvent;
		
		///Handles for WatchThread
		HANDLE m_StopEvent;
		HANDLE m_hWatchThread;
		DWORD m_ThreadId;
	};
} //namespace SvSml
