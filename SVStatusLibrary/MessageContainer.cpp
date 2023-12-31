//*****************************************************************************
/// \copyright (c) 2015,2015 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
//*****************************************************************************
/// This is the message handler to generate messages for logging and displaying
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <codecvt>
//Moved to precompiled header: #include <locale>
#include "MessageContainer.h"
#include "SVMessage/SVMessage.h"
#include "RegistryAccess.h"
#include "Definitions/GlobalConst.h"
#include "Definitions/StringTypeDef.h"
#include "SVUtilityLibrary/RaiiLifeFlag.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVUtilityLibrary\LoadDll.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HINSTANCE SvStl::MessageContainer::m_MessageDll {nullptr};
bool SvStl::MessageContainer::m_circularCall {false};

constexpr const char* SvEventSource = _T("SVException");
constexpr const char* SvSecuritySource =  _T("SVSecurity");
constexpr const char* SvAccessSource =  _T("SVAccess");

constexpr const char* c_ShowDisplay = _T("g_ShowDisplay");
constexpr const char* c_Notify = _T("g_Notify");

constexpr const char* DetailsToken = _T("#Details#");
constexpr const char* DebugLogFormat = _T( "Exception: ErrorCode: {}\nMessage: {}\n" );
constexpr const char* RegPathEventLog = _T("HKEY_LOCAL_MACHINE\\System\\CurrentControlSet\\Services\\EventLog\\Application\\");
constexpr const char* EventMsgFile = _T( "EventMessageFile" );
constexpr const char* SourceCategoryEventFormat = _T("Source: {}\r\nCategory: {}\r\nEventID: {}\r\n");
constexpr const char* ErrorLoadingDll = _T("SVException\r\nSVMessage.dll could not be loaded!\r\nError: {:#x}");
constexpr const char* DefaultEventFormat = _T("Source File: {} [{}] ({})\r\nCompiled: {} {}\n");

constexpr int SubstituteStringNr = 9;

constexpr UINT CategoryNr = 31;
constexpr UINT CategoryBase = FAC_SVProtoBuf;
constexpr const char* const CategoryUnknown = _T("Unknown");
constexpr const char* const CategoryNone = _T("None");
constexpr const char* const CategorySystem = _T("System");
constexpr const char* const CategoryApplication = _T("Application");
constexpr const char* const TaskCategory[CategoryNr]= { 
	_T("SVProtoBuf"),
	_T("SVGateway"),  
	_T("TriggerRecordController"), 
	_T("SVRemoteCtrl"), 
	_T("SVWebSocket"),
	_T("SVIMCommand"),
	_T("Unused06"),
	_T("Unused07"),
	_T("SVLibrary"),
	_T("Unused08"), 
	_T("SVSharedMemory"),
	_T("Unused10"),
	_T("SVObserver"),
	_T("SVSecurity"),
	_T("Unused11"),
	_T("Unused12"),
	_T("SVCmdComServer"),
	_T("SVCmdComClient"),
	_T("SVDataManager"),
	_T("SVAccess"),
	_T("Unused13"),
	_T("Unused14"),
	_T("SVEquation"),
	_T("SVFileAcquisition"),
	_T("SVMatroxGige"),
	_T("SVLptIO"),
	_T("SVOLibrary"),
	_T("SVSystemLibrary"),
	_T("SVMatroxLibrary"),
	_T("Unused15"),
	_T("SVXMLLibrary") };

#pragma endregion Declarations

namespace SvStl
{
#pragma region Constructor

	MessageContainer::MessageContainer( long MessageCode, MessageTextEnum AdditionalTextId, SourceFileParams SourceFile, uint32_t objectId /*=0*/ )
	{
		setMessage( MessageCode, AdditionalTextId, SvDef::StringVector(), SourceFile, objectId );
	}

	MessageContainer::MessageContainer( long MessageCode, MessageTextEnum AdditionalTextId, const SvDef::StringVector& rAdditionalTextList, SourceFileParams SourceFile, uint32_t objectId /*=0*/  )
	{
		setMessage( MessageCode, AdditionalTextId, rAdditionalTextList, SourceFile, objectId );
	}

	MessageContainer::MessageContainer(const MessageData& rMessage, uint32_t objectId /*= 0*/, bool clearData /*= true*/)
	{
		setMessage(rMessage, objectId, clearData );
	}

#pragma endregion Constructor

#pragma region Public Methods
	//! This is the virtual method of std::exception if unicode use rather the What method
	const char* MessageContainer::what() const
	{
#ifdef UNICODE
		return nullptr;
#else
		return m_What.c_str();
#endif
	}

	void MessageContainer::setMessage( long MessageCode, MessageTextEnum AdditionalTextId, SourceFileParams SourceFile, uint32_t objectId /*=0*/  )
	{
		setMessage( MessageCode, AdditionalTextId, SvDef::StringVector(), SourceFile, objectId );
	}

	void MessageContainer::setMessage( long MessageCode, MessageTextEnum AdditionalTextId, const SvDef::StringVector& rAdditionalTextList, SourceFileParams SourceFile, uint32_t objectId /*=0*/  )
	{
		clearMessage();
		m_Message.m_MessageCode = MessageCode;
		m_Message.m_AdditionalTextId = AdditionalTextId;
		m_Message.m_AdditionalTextList = rAdditionalTextList;
		m_Message.m_SourceFile = SourceFile;

		setMessage( m_Message, objectId, false );
	}

	void MessageContainer::setMessage( const MessageData& rMessage, uint32_t objectId /*=0*/, bool clearData /*= true*/ )
	{
		//! Make sure the dll is loaded
		setMessageDll();

		if( clearData )
		{
			clearMessage();
		}
		//Note if rMessage is the reference to m_Message then no copy is made
		m_Message = rMessage;
		//Set the object id only if it is not null
		if( 0 != objectId )
		{
			m_Message.m_ObjectId = objectId;
		}
		//Set the date time when this is being set
		m_Message.m_DateTime = std::time( 0 );

		if (0 == m_Message.m_MessageCode)
		{
			m_What = m_Message.getAdditionalText();
		}
		else
		{
			Format(m_What);
		}
	}

	void MessageContainer::addMessage( long MessageCode, MessageTextEnum AdditionalTextId, SvDef::StringVector AdditionalTextList, SourceFileParams SourceFile)
	{
		//Save the current message to the additional messages
		m_AdditionalMessages.push_back( m_Message );

		m_Message.m_MessageCode = MessageCode;
		m_Message.m_AdditionalTextId = AdditionalTextId;
		m_Message.m_AdditionalTextList = AdditionalTextList;
		m_Message.m_SourceFile = SourceFile;

		setMessage( m_Message, 0, false );
	}

	void MessageContainer::addMessage( const MessageData& rMessage, bool replaceMainMessage )
	{
		if( replaceMainMessage )
		{
			//Save the current message to the additional messages and make the new message the main message
			m_AdditionalMessages.push_back( m_Message );
			setMessage( rMessage, 0, false );
		}
		else
		{
			//In this case the main message stays the original message and the added message is placed into the additional list
			m_AdditionalMessages.push_back( rMessage );
			//Need to record the date and time when the message is added
			m_AdditionalMessages.back().m_DateTime = std::time( 0 );
		}
	}

	void MessageContainer::clearMessage()
	{
		m_Message.clear();
		m_AdditionalMessages.clear();
		m_What.clear();
	}

	void MessageContainer::logMessage() const
	{
		SvDef::StringVector SubstituteStrings;
		const TCHAR *pSubstituteString[SubstituteStringNr];

#if defined (TRACE_THEM_ALL) || defined (TRACE_OTHER)
		std::string DebugString = std::format( DebugLogFormat, m_Message.m_MessageCode, m_Message.getAdditionalText().c_str() );
		::OutputDebugString( DebugString.c_str() );
#endif

		setSubstituteStrings( SubstituteStrings );

		for( int i=0; i < SubstituteStringNr; i++ )
		{
			if(i < static_cast<int> (SubstituteStrings.size()))
			{
				pSubstituteString[i] = SubstituteStrings[i].c_str();
			}
			else
			{
				pSubstituteString[i] = nullptr;
			}
		}

		if (!m_Message.m_Logged)
		{
			HANDLE hEventLog = RegisterEventSource ( nullptr, getFacilityName().c_str() );
			if( nullptr != hEventLog )
			{
				WORD Severity{0};

				switch (getSeverity())
				{
				case SEV_SUCCESS:
					Severity = EVENTLOG_SUCCESS;
					break;

				case SEV_INFORMATIONAL:
					Severity = EVENTLOG_INFORMATION_TYPE;
					break;

				case SEV_WARNING:
					Severity = EVENTLOG_WARNING_TYPE;
					break;

				case SEV_ERROR:
					Severity = EVENTLOG_ERROR_TYPE;
					break;
				default:
					break;
				}
				m_Message.m_Logged = ReportEvent (hEventLog, Severity, getCategory(), m_Message.m_MessageCode, nullptr, SubstituteStringNr, 0, pSubstituteString, nullptr) ? true : false;
				DeregisterEventSource (hEventLog);
			}
			else
			{
				DWORD dwError;
				dwError = GetLastError();
			}
		}
	}

	UINT MessageContainer::getSeverityIcon() const
	{
		UINT Icon( 0 );

		switch (getSeverity())
		{
		case SEV_SUCCESS :
			Icon = MB_ICONINFORMATION;
			break;

		case SEV_INFORMATIONAL :
			Icon = MB_ICONINFORMATION;
			break;

		case SEV_WARNING :
			Icon = MB_ICONWARNING;
			break;

		case SEV_ERROR :
			Icon = MB_ICONERROR;
			break;

		default:
			Icon = 0;
			break;
		}

		return Icon;
	}

	std::string MessageContainer::Format( std::string& rMessage ) const
	{
		std::string Result;
		std::string MsgDetails;

		rMessage.clear();
		//Default result
		Result = std::format( SourceCategoryEventFormat, getFacilityName().c_str(), getCategoryName().c_str(), getEventID() );

		if (nullptr != m_MessageDll )
		{
			SvDef::StringVector SubstituteStrings;
			const TCHAR *pSubstituteString[SubstituteStringNr];

			setSubstituteStrings( SubstituteStrings );

			for( int i=0; i < SubstituteStringNr; i++ )
			{
				if(i < static_cast<int> (SubstituteStrings.size()))
				{
					pSubstituteString[i] = SubstituteStrings[i].c_str();
				}
				else
				{
					pSubstituteString[i] = nullptr;
				}
			}

			LPVOID pMessage;

			if (FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_ARGUMENT_ARRAY |
				FORMAT_MESSAGE_FROM_HMODULE,
				(LPCVOID) m_MessageDll, m_Message.m_MessageCode,
				SvDef::LCID_USA, (LPTSTR) &pMessage, 11, (va_list *) pSubstituteString))
			{
				rMessage = (TCHAR *) pMessage;
				SvUl::searchAndReplace(rMessage, _T("\r\n\r\n"), _T("\r\n") );
				std::string SearchString( _T("\r\n") );
				SearchString += DetailsToken;
				size_t Index = rMessage.find( SearchString.c_str() );
				if ( std::string::npos != Index )
				{
					MsgDetails = SvUl::Mid( rMessage, Index +  SearchString.size() );
					//Remove unnecessary new lines from the details
					size_t Pos = MsgDetails.find_first_not_of(_T("\r\n"));
					if ( -1 != Pos )
					{
						MsgDetails = MsgDetails.substr( Pos );
					}
					rMessage = rMessage.substr( 0, Index );
				}
			}
			LocalFree (pMessage);
		}

		if (rMessage.empty())
		{
			rMessage = std::format( ErrorLoadingDll, m_Message.m_MessageCode);

			MsgDetails = std::format( DefaultEventFormat,
				m_Message.m_SourceFile.m_FileName.c_str(),
				m_Message.m_SourceFile.m_Line,
				m_Message.m_SourceFile.m_FileDateTime.c_str(),
				m_Message.m_SourceFile.m_CompileDate.c_str(),
				m_Message.m_SourceFile.m_CompileTime.c_str() );
		}
		Result += MsgDetails;

		return Result;
	}

	void MessageContainer::setFunctorObjects( ShowDisplayFunctor*& rpShowDisplay, MessageNotifyFunctor*& rpNotify )
	{
		setMessageDll();

		rpShowDisplay = reinterpret_cast<ShowDisplayFunctor*> (::GetProcAddress( m_MessageDll, c_ShowDisplay ));
		rpNotify = reinterpret_cast<MessageNotifyFunctor*> (::GetProcAddress( m_MessageDll, c_Notify ));
	}
#pragma endregion Public Methods

#pragma region Private Methods
	UINT MessageContainer::getSeverity() const
	{
		return (m_Message.m_MessageCode & 0xc0000000) >> 30;
	}

	UINT MessageContainer::getFacility() const
	{
		return (m_Message.m_MessageCode & 0x0fff0000) >> 16;
	}

	std::string MessageContainer::getFacilityName() const
	{
		std::string SourceName;

		switch( getFacility() )
		{
		case FAC_SVSECURITY:
			{
				SourceName = SvSecuritySource;
				break;
			}
		case FAC_SVACCESS:
			{
				SourceName = SvAccessSource;
				break;
			}
		default :
			{
				SourceName = SvEventSource;
				break;
			}
		}
	
		return SourceName;
	}

	UINT MessageContainer::getEventID() const
	{
		return ( m_Message.m_MessageCode & 0x0000ffff);
	}

	WORD MessageContainer::getCategory() const
	{
		return (getFacility() & 0x00ff);
	}

	std::string MessageContainer::getCategoryName() const
	{
		std::string Result( CategoryUnknown );

		switch( getFacility() )
		{
		case FAC_NONE:
			{
				Result = CategoryNone;
			}
			break;
		case FAC_SYSTEM:
			{
				Result = CategorySystem;
			}
			break;
		case FAC_APPLICATION:
			{
				Result = CategoryApplication;
			}
			break;
		default:
			{
				int Index(0);
				Index = getFacility() - CategoryBase;
				if( 0 <= Index && CategoryNr > Index )
				{
					Result = TaskCategory[Index];
				}
			}
			break;
		}

		return Result;
	}

	void MessageContainer::setSubstituteStrings( SvDef::StringVector& rSubstituteStrings ) const
	{
		rSubstituteStrings.clear();
		rSubstituteStrings.resize( SubstituteStringNr );
		rSubstituteStrings[0] = _T("\r\n");
		rSubstituteStrings[1] =  m_Message.m_SourceFile.m_FileName;
		rSubstituteStrings[2] = std::format( _T("{}"), m_Message.m_SourceFile.m_Line );
		rSubstituteStrings[3] = m_Message.m_SourceFile.m_FileDateTime;
		rSubstituteStrings[4] = m_Message.m_SourceFile.m_CompileDate;
		rSubstituteStrings[5] = m_Message.m_SourceFile.m_CompileTime;
		rSubstituteStrings[6] = std::format(_T("Object ID = {}"), m_Message.m_ObjectId);
		rSubstituteStrings[7] = m_Message.getAdditionalText();
	}

	HRESULT MessageContainer::setMessageDll()
	{
		HRESULT Result( S_OK );

		if( nullptr == m_MessageDll && false == m_circularCall)
		{
			SvDef::RaiiLifeFlag circularCheck {m_circularCall};
			//! Note all facilities use the same message dll 
			std::string RegKey( RegPathEventLog );
			RegKey += getFacilityName();

			try
			{
				RegistryAccess reg( RegKey.c_str());
				if (!reg.CreatedNewKey())
				{
					std::string MessageDll;
					if (reg.GetRegistryValue( EventMsgFile, MessageDll))
					{
						Result = SvUl::LoadDll::Instance().getDll( MessageDll.c_str(), m_MessageDll );
					}
				}
				else
				{
					reg.DeleteKey();
				}
			}
			catch( const MessageContainer&  )
			{
				//! Do nothing otherwise this can cause infinite loop;
			}
		}

		return Result;
	}
#pragma endregion Private Methods
} //namespace SvStl