//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file ShareEvents.h 
/// All Rights Reserved 
//*****************************************************************************


//Singleton class handles two global events 
///The events are used to signal status of shared memory  
//******************************************************************************
#pragma once

namespace Seidenader { namespace SVSharedMemoryLibrary
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
		
		////! callbackfunction for watch thread 
		//void SetChangeFunction(boost::function<bool ()> CF);
		////! callbackfunction for watch thread 
		//void SetReadyFunction(boost::function<bool ()> RF);
		////! callbackfunction for watch thread 
		//void SetUpdateFunction(boost::function<void ()> RF);
		//! True if InitEventIsSet
		bool GetIsReady() const;
		
		//! The Ready counter is incremented when the ready flag is set to true
		long ShareEvents::GetReadyCounter() const;

		//!True if Init Flag isSet
		bool GetIsInit() const;
		
		//!Set Init flag. The init flag will bee reseted when changeing event occurs
		void SetIsInit();

	private:
		static const LPCTSTR GNameChangeEvent;
		static const LPCTSTR GNameReadyEvent;
		
		const static DWORD DELAY;
		
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
} /*namespace SVSharedMemoryLibrary*/ } /*namespace Seidenader*/

namespace SvSml = Seidenader::SVSharedMemoryLibrary;
