//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// This is the message manager which controls the storing, displaying and logging
///	of information warning and error messages
//******************************************************************************

#pragma region Constructor
template <typename M_Container, typename M_Data>
MessageManager<M_Container, M_Data>::MessageManager( const MsgTypeEnum Type ) :
m_Type( Type )
{
	if( DataOnly != Type)
	{
		Initialize();
	}
}

template <typename M_Container, typename M_Data>
MessageManager<M_Container, M_Data>::~MessageManager()
{
}
#pragma endregion Constructor
 
#pragma region Public Methods
template <typename M_Container, typename M_Data>
void MessageManager<M_Container, M_Data>::setShowDisplayFunction( ShowDisplayFunctor ShowDisplay )
{
	Initialize();
	if( nullptr != m_pShowDisplay )
	{
		*m_pShowDisplay = ShowDisplay;
	}
}

template <typename M_Container, typename M_Data>
void MessageManager<M_Container, M_Data>::setNotificationFunction( NotifyFunctor Notify )
{
	Initialize();
	if( nullptr != m_pNotify )
	{
		*m_pNotify = Notify;
	}
}

template <typename M_Container, typename M_Data>
void MessageManager<M_Container, M_Data>::setType( const MsgTypeEnum Type )
{
	m_Type = Type;
}

template <typename M_Container, typename M_Data>
void MessageManager<M_Container, M_Data>::Throw()
{
	throw m_MessageHandler; 
}

template <typename M_Container, typename M_Data>
INT_PTR MessageManager<M_Container, M_Data>::Process( UINT MsgBoxType /*= MB_OK*/ )
{
	INT_PTR Result( IDCANCEL );

	Log();

	Result = Display( MsgBoxType );
	if( nullptr != m_pShowDisplay && !m_pShowDisplay->empty() && LogAndDisplay == m_Type)
	{
		//Message has bin displayed
		m_MessageHandler.setMessageDisplayed();
	}

	return Result;
}

template <typename M_Container, typename M_Data>
INT_PTR MessageManager<M_Container, M_Data>::setMessage( DWORD MessageCode, LPCTSTR AdditionalText, SourceFileParams SourceFile, DWORD ProgramCode /*= 0*/, const GUID& rObjectId /*= GUID_NULL*/, UINT MsgBoxType /*= MB_OK*/ )
{
	SvDef::StringVector textList;
	MessageTextEnum id = SvStl::Tid_Empty;
	if (nullptr != AdditionalText)
	{
		textList.push_back(AdditionalText);
		id = SvStl::Tid_Default;
	}

	return setMessage( MessageCode, id, textList, SourceFile, ProgramCode, rObjectId, MsgBoxType );
}

template <typename M_Container, typename M_Data>
INT_PTR MessageManager<M_Container, M_Data>::setMessage( DWORD MessageCode, MessageTextEnum AdditionalTextId, SourceFileParams SourceFile, DWORD ProgramCode /*= 0*/, const GUID& rObjectId /*= GUID_NULL*/, UINT MsgBoxType /*= MB_OK*/)
{
	return setMessage( MessageCode, AdditionalTextId, SvDef::StringVector(), SourceFile, ProgramCode, rObjectId, MsgBoxType );
}

template <typename M_Container, typename M_Data>
INT_PTR MessageManager<M_Container, M_Data>::setMessage( DWORD MessageCode, MessageTextEnum AdditionalTextId, const SvDef::StringVector& rAdditionalTextList, SourceFileParams SourceFile, DWORD ProgramCode /*= 0*/, const GUID& rObjectId /*= GUID_NULL*/ , UINT MsgBoxType /*= MB_OK*/ )
{
	INT_PTR Result( IDCANCEL );

	m_MessageHandler.setMessage( MessageCode, AdditionalTextId, rAdditionalTextList, SourceFile, ProgramCode, rObjectId );

	Result = Process( MsgBoxType );

	return Result;
}

template <typename M_Container, typename M_Data>
INT_PTR MessageManager<M_Container, M_Data>::setMessage( const M_Data& rData, const GUID& rObjectId /*= GUID_NULL*/, UINT MsgBoxType /*= MB_OK */ )
{
	INT_PTR Result( IDCANCEL );

	m_MessageHandler.setMessage( rData, rObjectId );

	Result = Process( MsgBoxType );

	return Result;
}

template <typename M_Container, typename M_Data>
M_Container& MessageManager<M_Container, M_Data>::getMessageContainer()
{
	return m_MessageHandler;
}
#pragma endregion Public Methods
 
#pragma region Private Methods
template <typename M_Container, typename M_Data>
void MessageManager<M_Container, M_Data>::Initialize()
{
	//Initialize the static members just once per instance
	if( nullptr == m_pShowDisplay && nullptr == m_pNotify )
	{
		M_Container MessageHandler;
		MessageHandler.setFunctorObjects( m_pShowDisplay, m_pNotify );
	}
}

template <typename M_Container, typename M_Data>
void MessageManager<M_Container, M_Data>::Log()
{
	if( LogOnly == m_Type || LogAndDisplay == m_Type )
	{
		m_MessageHandler.logMessage();

		if( nullptr != m_pNotify && !m_pNotify->empty()  && LogOnly == m_Type )
		{
			//! Only notify log messages which are warnings or errors
			UINT MsgSeverity( m_MessageHandler.getSeverityIcon() );

			if( MB_ICONWARNING == MsgSeverity || MB_ICONERROR == MsgSeverity )
			{
				std::string Msg;
				m_MessageHandler.Format( Msg );
				int MsgCode = (0 != m_MessageHandler.getMessage().m_ProgramCode) ? m_MessageHandler.getMessage().m_ProgramCode : m_MessageHandler.getMessage().m_MessageCode;
				(*m_pNotify)( SvStl::MsgLog, MsgCode, Msg.c_str() );
			}
		}
	}
}

template <typename M_Container, typename M_Data>
INT_PTR MessageManager<M_Container, M_Data>::Display( const UINT MsgBoxType ) const
{
	INT_PTR Result( IDCANCEL );

	MsgTypeEnum Type{m_Type};

	//If Remote state command is active then only log the message do not display it!
	if(LogAndDisplay == Type &&  SVSVIMStateClass::CheckState(SV_STATE_REMOTE_CMD))
	{
		Type = LogOnly;
	}

	//If the message has already been displayed do not display again
	if( nullptr != m_pShowDisplay && !m_pShowDisplay->empty() && LogAndDisplay == Type  && !m_MessageHandler.getMessage().m_Displayed )
	{
		std::string Msg;
		std::string MsgDetails;
		UINT Type ( MsgBoxType );
		int MsgCode( 0 );

		MsgDetails = m_MessageHandler.Format( Msg );
		MsgCode = (0 != m_MessageHandler.getMessage().m_ProgramCode) ? m_MessageHandler.getMessage().m_ProgramCode : m_MessageHandler.getMessage().m_MessageCode;
		//Message box type icon is determined by the severity of the message so set to 0 then get it from the container
		Type &= ~MB_ICONMASK;
		Type |= m_MessageHandler.getSeverityIcon();

		if( nullptr != m_pNotify && !m_pNotify->empty() )
		{
			(*m_pNotify)( SvStl::StartMsgBox, MsgCode ,Msg.c_str() );
		}
		Result = (*m_pShowDisplay)( nullptr, Msg.c_str(), MsgDetails.c_str(), Type );
		//Message has been displayed do not display again
		m_MessageHandler.getMessage().m_Displayed = true;
		if( nullptr != m_pNotify && !m_pNotify->empty() )
		{
			(*m_pNotify)( SvStl::EndMsgBox, MsgCode ,Msg.c_str() );
		}
	}
	
	return Result;
}
#pragma endregion Private Methods
