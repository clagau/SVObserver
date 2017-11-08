#pragma region Includes
#include "StdAfx.h"
#include "ShareEvents.h"
#include "WinBase.h" 
#include "SVStatusLibrary\CommandLineArgs.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVMessage\SVMessage.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvSml
{
	const LPCTSTR ShareEvents::GNameChangeEvent  = _T("Global\\Seidenader_ChangeEvent");
	const LPCTSTR ShareEvents::GNameReadyEvent  = _T("Global\\Seidenader_ReadyEvent");
	const  DWORD ShareEvents::Delay_Before_CreateShare =  60; //SVObserver waittime after setting Change Event
	const  DWORD ShareEvents::Delay_Before_ClearShare = 30; 

	ShareEvents::ShareEvents(): m_StopEvent(NULL), m_hWatchThread(NULL),m_IsReady(false), m_IsInit(false),m_ReadyCounter(0)
	{
		
		PSECURITY_DESCRIPTOR psd = (PSECURITY_DESCRIPTOR) LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH); 
		InitializeSecurityDescriptor(psd, SECURITY_DESCRIPTOR_REVISION);
		SetSecurityDescriptorDacl(psd, TRUE, NULL, FALSE);

		SECURITY_ATTRIBUTES sa = {0};
		sa.nLength = sizeof(sa);
		sa.lpSecurityDescriptor = psd;
		sa.bInheritHandle = FALSE;
		
		DWORD errorCode(0);
		m_hChangeEvent = ::CreateEvent(&sa,true,true, GNameChangeEvent);
		if(NULL == m_hChangeEvent)
		{
			errorCode = GetLastError();
			std::string  text = SvUl::Format(_T("Create Change event  Failed Errorcode: %x"),errorCode); 
			SvStl::MessageMgrStd Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_SVO_44_SHARED_MEMORY, text.c_str(), SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16221_FailToCreateEvent );
			
		}
		m_hReadyEvent = ::CreateEvent(&sa,true,false,GNameReadyEvent);
		if(NULL == m_hReadyEvent)
		{
			errorCode = GetLastError();
			std::string  text = SvUl::Format(_T("Create Ready event  Failed Errorcode: %x"),errorCode); 
			SvStl::MessageMgrStd Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_SVO_44_SHARED_MEMORY, text.c_str(), SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16222_FailToCreateEvent );
		}

		LocalFree(psd);
	}					  	

	void ShareEvents::SignaltReadyStatus()
	{
		ResetEvent(m_hChangeEvent);
		SetEvent(m_hReadyEvent);
		m_IsReady = true;
		m_ReadyCounter++;
		
	}
	void ShareEvents::SignalChangingStatus()
	{
		m_IsInit = false;
		m_IsReady = false;
		ResetEvent(m_hReadyEvent);
		SetEvent(m_hChangeEvent);
	}

	void ShareEvents::QuiesceSharedMemory()
	{
		SignalChangingStatus();
		::Sleep(Delay_Before_CreateShare);
	}

	bool ShareEvents::GetIsReady()  const
	{
		return m_IsReady;
	}
	
	long ShareEvents::GetReadyCounter() const
	{
		return m_ReadyCounter;
	}

	bool ShareEvents::GetIsInit()  const
	{
		return m_IsInit;
	}
	
	void ShareEvents::SetIsInit()
	{
		m_IsInit = true;
	}


	

	void ShareEvents::SetCallbackFunction(boost::function<bool(DWORD )>cbFct)
	{
		m_CallBackFct  = cbFct;
	}



	DWORD WINAPI ShareEvents::WatchFunction( LPVOID lpParam )
	{
		ShareEvents* pShareEvent =  reinterpret_cast<ShareEvents*> (lpParam);
		HANDLE HChange[2], HReady[2]; 
		HChange[0] = HReady[0] = pShareEvent->m_StopEvent;
		HChange[1] = pShareEvent->m_hChangeEvent;
		HReady[1] = pShareEvent->m_hReadyEvent;
		bool Continue(true);
		DWORD Event(0);
		bool Sucess(true);
		while(Continue)
		{
			Sucess = true;
			Event =  WaitForMultipleObjects(2,HReady,false,INFINITE);
			switch (Event)
			{
			case WAIT_OBJECT_0:
				{
					Continue = false;
					break;
				}
			case 	WAIT_OBJECT_0 +1:
				if(pShareEvent->m_CallBackFct)
				{
					Sucess =   pShareEvent->m_CallBackFct(Ready );
				}
				if (Sucess)
				{
					pShareEvent->m_IsReady = true;
					pShareEvent->m_ReadyCounter++;
				}
				break;
			default:
				{
					//Continue = false;
					break;
				}
			}
			if(!Continue)
			{
				break;
			}
			Event =  WaitForMultipleObjects(2,HChange,false,INFINITE);
			switch (Event)
			{
			case WAIT_OBJECT_0:
				{
					Continue = false;
					break;
				}
			case 	WAIT_OBJECT_0 +1:
				pShareEvent->m_IsReady = false;
				pShareEvent->m_IsInit = false;
				Sucess = true;

				if(pShareEvent->m_CallBackFct)
				{
					Sucess = pShareEvent->m_CallBackFct(Change );
				}
				
				break;
			default:
				{
					Continue = false;
					break;
				}
			}
		
		}
		return Event;
	}

	bool ShareEvents::StartWatch()
	{
		if(m_StopEvent ||  m_hWatchThread)
		{	
			return true;
		}
		
		m_StopEvent = CreateEvent(NULL,false,false,NULL);
		if(!m_StopEvent)
			return false;

		m_hWatchThread = CreateThread(NULL,0,WatchFunction,this,0, &m_ThreadId);
		return (m_hWatchThread != NULL);
	}
	
	ShareEvents::~ShareEvents()
	{
		if(m_StopEvent && m_hWatchThread)
		{
			SetEvent(m_StopEvent);
			if(::WaitForSingleObject(m_hWatchThread,2000) != WAIT_OBJECT_0)
			{
				::TerminateThread( m_hWatchThread, E_FAIL );
			}
		}
		
		CloseHandle(m_hChangeEvent);
		CloseHandle(m_hReadyEvent);

		CloseHandle(m_StopEvent);
		CloseHandle(m_hWatchThread);
	}
} //namespace SvSml
