//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// This is the message handler to generate messages for logging and displaying
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "MessageContainer.h"
#include "SVMessage\SVMessage.h"
#include "SVRegistry.h"
#include "SVObjectLibrary\GlobalConst.h"
#include "SVUtilityLibrary\LoadDll.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const TCHAR* const SvEventSource = _T("SVException");
static const TCHAR* const SvSecuritySource =  _T("SVSecurity");
static const TCHAR* const SvAccessSource =  _T("SVAccess");

static const TCHAR* const DetailsToken = _T("#Details#");
static const TCHAR* const DebugLogFormat = _T( "Exception: ErrorCode: %d\nMessage: %s\n" );
static const TCHAR* const RegPathEventLog = _T("HKEY_LOCAL_MACHINE\\System\\CurrentControlSet\\Services\\EventLog\\Application\\");
static const TCHAR* const EventMsgFile = _T( "EventMessageFile" );
static const TCHAR* const SourceCategoryEventFormat = _T("Source: %s\r\nCategory: %s\r\nEventID: %d\r\n");
static const TCHAR* const ErrorLoadingDll = _T("SVException\r\nSVMessage.dll could not be loaded!\r\nError: 0X%X");
static const TCHAR* const DefaultEventFormat = _T("Source File: %s [%d] (%s)\r\nProgramCode: %d\r\nOSError: %d\r\nCompiled: %s %s\n");

static const UINT SubstituteStringNr = 12;

static const UINT CategoryNr = 31;
static const UINT CategoryBase = FAC_UNUSED01;
static const TCHAR* const CategoryUnknown = _T("Unknown");
static const TCHAR* const CategoryNone = _T("None");
static const TCHAR* const CategorySystem = _T("System");
static const TCHAR* const CategoryApplication = _T("Application");
static const TCHAR* const TaskCategory[CategoryNr]= { _T("Unused01"), _T("SVBatchReport"), _T("SVFocusNT"), _T("SVFocusDB"), _T("SVFocusDBManager"),
	_T("SVIMCommand"), _T("SVIPC"), _T("SVLanguageManager"), _T("SVLibrary"), _T("SVPipes"), 
	_T("Unused02"), _T("SVTCPIP"), _T("SVObserver"), _T("SVSecurity"), _T("Unused03"),
	_T("SVMachineMessage"), _T("SVCmdComServer"), _T("SVCmdComClient"), _T("SVDataManager"),
	_T("SVAccess"), _T("SVIntek"), _T("SVImageCompression"), _T("SVEquation"), _T("SVFileAcquisition"),
	_T("SVMatroxGige"), _T("SVTVicLpt"), _T("SVOLibrary"), _T("SVSystemLibrary"), _T("SVMatroxLibrary"),
	_T("SVCI"), _T("SVXMLLibrary") };

#pragma endregion Declarations

namespace Seidenader { namespace SVStatusLibrary
{
#pragma region Constructor
	MessageContainer::MessageContainer()
	{
	}

	MessageContainer::MessageContainer(const MessageContainer& rRhs) :
	m_Message( rRhs.m_Message )
		, m_AdditionalMessages( rRhs.m_AdditionalMessages )
		, m_What( rRhs.m_What )
	{
	}

	MessageContainer::MessageContainer( long MessageCode, LPCTSTR AdditionalText, LPCTSTR CompileDate, LPCTSTR CompileTime,
		LPCTSTR SourceFile, long SourceLine, LPCTSTR SourceDateTime,
		DWORD ProgramCode, DWORD OSErrorCode, LPCTSTR User )
	{
		setMessage( MessageCode, AdditionalText, CompileDate, CompileTime, SourceFile, SourceLine, SourceDateTime, ProgramCode, OSErrorCode, User );
	}

	const MessageContainer& MessageContainer::operator=(const MessageContainer& rRhs)
	{
		if( &rRhs != this )
		{
			m_Message = rRhs.m_Message;
			m_AdditionalMessages = rRhs.m_AdditionalMessages;
			m_What = rRhs.m_What;
		}

		return *this;
	}

	MessageContainer::~MessageContainer()
	{
	}
#pragma endregion Constructor

#pragma region Public Methods
	void MessageContainer::setMessage( long MessageCode, LPCTSTR AdditionalText, LPCTSTR CompileDate, LPCTSTR CompileTime, 
		LPCTSTR SourceFile, long SourceLine, LPCTSTR SourceDateTime, 
		DWORD ProgramCode, DWORD OsErrorCode, LPCTSTR User )
	{
		clearMessage();
		m_Message.m_MessageCode = MessageCode;
		m_Message.m_AdditionalText = AdditionalText;
		m_Message.m_CompileDate = CompileDate;
		m_Message.m_CompileTime = CompileTime;
		m_Message.m_SourceLine = SourceLine;
		m_Message.m_SourceDateTime = SourceDateTime;
		m_Message.m_SourceFile = SourceFile;
		m_Message.m_ProgramCode = ProgramCode;
		m_Message.m_OSErrorCode = OsErrorCode;
		m_Message.m_User = User;

		setMessage( m_Message, false );
	}

	void MessageContainer::setMessage( const MessageData& rMessage, bool clearData )
	{
		if( clearData )
		{
			clearMessage();
		}
		//Note if rMessage is the reference to m_Message then no copy is made
		m_Message = rMessage;
		//Set the date time when this is being set
		m_Message.m_DateTime = std::time( 0 );

		if( 0 == m_Message.m_OSErrorCode )
		{
			m_Message.m_OSErrorCode = GetLastError();
		}

		if( !m_Message.m_AdditionalText.empty() )
		{
			m_What = m_Message.m_AdditionalText;
		}
		else
		{
			Format( m_What );
		}
	}

	void MessageContainer::addMessage( long MessageCode, LPCTSTR AdditionalText, LPCTSTR CompileDate, LPCTSTR CompileTime, 
		LPCTSTR SourceFile, long SourceLine, LPCTSTR SourceDateTime , 
		DWORD ProgramCode, DWORD OsErrorCode, LPCTSTR User )
	{
		//Save the current message to the additional messages
		m_AdditionalMessages.push_back( m_Message );

		m_Message.m_MessageCode = MessageCode;
		m_Message.m_AdditionalText = AdditionalText;
		m_Message.m_CompileDate = CompileDate;
		m_Message.m_CompileTime = CompileTime;
		m_Message.m_SourceLine = SourceLine;
		m_Message.m_SourceDateTime = SourceDateTime;
		m_Message.m_SourceFile = SourceFile;
		m_Message.m_ProgramCode = ProgramCode;
		m_Message.m_OSErrorCode = OsErrorCode;
		m_Message.m_User = User;

		setMessage( m_Message, false );
	}

	void MessageContainer::addMessage( const MessageData& rMessage, bool replaceMainMessage )
	{
		if( replaceMainMessage )
		{
			//Save the current message to the additional messages and make the new message the main message
			m_AdditionalMessages.push_back( m_Message );
			setMessage( rMessage, false );
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

	void MessageContainer::logMessage()
	{
		SVString DebugString;
		SVStringArray SubstituteStrings;
		const TCHAR *pSubstituteString[SubstituteStringNr];

		DebugString.Format( DebugLogFormat, m_Message.m_MessageCode, m_Message.m_AdditionalText.c_str() );
		::OutputDebugString( DebugString.c_str() );

		setSubstituteStrings( SubstituteStrings );

		for( int i=0; i < SubstituteStringNr; i++ )
		{
			if( i < SubstituteStrings.size() )
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
			HANDLE hEventLog = RegisterEventSource ( NULL, getFacilityName().c_str() );
			if( NULL != hEventLog )
			{
				UINT Severity;

				switch (getSeverity())
				{
				case SEV_SUCCESS :
					Severity = EVENTLOG_SUCCESS;
					break;

				case SEV_INFORMATIONAL :
					Severity = EVENTLOG_INFORMATION_TYPE;
					break;

				case SEV_WARNING :
					Severity = EVENTLOG_WARNING_TYPE;
					break;

				case SEV_FATAL :
					Severity = EVENTLOG_ERROR_TYPE;
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

		case SEV_FATAL :
			Icon = MB_ICONERROR;
			break;

		default:
			Icon = 0;
			break;
		}

		return Icon;
	}

	SVString MessageContainer::Format( SVString& rMessage ) const
	{
		SVString Result;
		SVString MsgDetails;
		SVString MessageDll;
		LPVOID pMessage;
		HINSTANCE hMessageDll( nullptr );
		SVString RegKey;
		SVStringArray SubstituteStrings;
		const TCHAR *pSubstituteString[SubstituteStringNr];

		RegKey = RegPathEventLog;
		RegKey += getFacilityName();

		try 
		{

			SVRegistryClass reg( RegKey.c_str());
			rMessage.clear();

			if (!reg.CreatedNewKey())
			{
				if (reg.GetRegistryValue( EventMsgFile, MessageDll))
				{
					Result.Format( SourceCategoryEventFormat, getFacilityName().c_str(), getCategoryName().c_str(), getEventID() );

					HRESULT retValue = SvUl::LoadDll::Instance().getDll( MessageDll.c_str(), hMessageDll );
					if (S_OK == retValue && nullptr != hMessageDll )
					{
						setSubstituteStrings( SubstituteStrings );

						for( int i=0; i < SubstituteStringNr; i++ )
						{
							if( i < SubstituteStrings.size() )
							{
								pSubstituteString[i] = SubstituteStrings[i].c_str();
							}
							else
							{
								pSubstituteString[i] = nullptr;
							}
						}

						if (FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_ARGUMENT_ARRAY |
							FORMAT_MESSAGE_FROM_HMODULE,
							(LPCVOID) hMessageDll, m_Message.m_MessageCode,
							SvOl::LCID_USA, (LPTSTR) &pMessage, 11, (va_list *) pSubstituteString))
						{
							rMessage = (TCHAR *) pMessage;
							rMessage.replace(_T("\r\n\r\n"), _T("\r\n") );
							SVString SearchString( _T("\r\n") );
							SearchString += DetailsToken;
							size_t Index = rMessage.find( SearchString.c_str() );
							if ( -1 != Index )
							{
								MsgDetails = rMessage.Mid( Index +  SearchString.size() );
								//Remove unnecessary new lines from the details
								size_t Pos = MsgDetails.find_first_not_of(_T("\r\n"));
								if ( -1 != Pos )
								{
									MsgDetails = MsgDetails.substr( Pos );
								}
								rMessage = rMessage.Left( Index );
							}
						}
						LocalFree (pMessage);
					}
				}
			}
			else
			{
				reg.DeleteKey();
			}
		}
		catch(  MessageContainer& mh  )
		{
			//do nothing;
		}

		if (rMessage.empty())
		{
			rMessage.Format( ErrorLoadingDll, m_Message.m_MessageCode);

			MsgDetails.Format( DefaultEventFormat,
				m_Message.m_SourceFile.c_str(),
				m_Message.m_SourceLine,
				m_Message.m_SourceDateTime.c_str(),
				m_Message.m_ProgramCode,
				m_Message.m_OSErrorCode,
				m_Message.m_CompileDate.c_str(),
				m_Message.m_CompileTime.c_str() );
		}
		Result += MsgDetails;

		return Result;
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

	SVString MessageContainer::getFacilityName() const
	{
		SVString SourceName;

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

	SVString MessageContainer::getCategoryName() const
	{
		SVString Result( CategoryUnknown );

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

	void MessageContainer::setSubstituteStrings( SVStringArray& rSubstituteStrings ) const
	{
		rSubstituteStrings.clear();
		rSubstituteStrings.resize( SubstituteStringNr );
		rSubstituteStrings[0] = _T("\r\n");
		rSubstituteStrings[1] =  m_Message.m_SourceFile;
		rSubstituteStrings[2].Format( _T("%d"), m_Message.m_SourceLine );
		rSubstituteStrings[3] = m_Message.m_SourceDateTime;
		rSubstituteStrings[4].Format( _T("%d"), m_Message.m_OSErrorCode );
		rSubstituteStrings[5].Format( _T("0x%08x"), m_Message.m_OSErrorCode );
		rSubstituteStrings[6] = m_Message.m_CompileDate;
		rSubstituteStrings[7] = m_Message.m_CompileTime;
		rSubstituteStrings[8].Format( _T("%d"), m_Message.m_ProgramCode );
		rSubstituteStrings[9].Format( _T("0x%08x"), m_Message.m_ProgramCode );
		rSubstituteStrings[10] = m_Message.m_AdditionalText;
		rSubstituteStrings[11] = m_Message.m_User;
	}
#pragma endregion Private Methods
} /* namespace SVStatusLibrary */ } /* namespace Seidenader */
