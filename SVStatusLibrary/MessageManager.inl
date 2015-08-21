//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// This is the message manager which controls the storing, displaying and logging
///	of information warning and error messages
//******************************************************************************

#pragma region Constructor
template <typename M_Handler, typename M_Data, ShowDialog M_Display>
MessageManager<M_Handler, M_Data, M_Display>::MessageManager( const MsgTypeEnum Type ) :
m_Type( Type )
{
}

template <typename M_Handler, typename M_Data, ShowDialog M_Display>
MessageManager<M_Handler, M_Data, M_Display>::~MessageManager()
{
}
#pragma endregion Constructor
 
#pragma region Public Methods
template <typename M_Handler, typename M_Data, ShowDialog M_Display>
void MessageManager<M_Handler, M_Data, M_Display>::setType( const MsgTypeEnum Type )
{
	m_Type = Type;
}

template <typename M_Handler, typename M_Data, ShowDialog M_Display>
void MessageManager<M_Handler, M_Data, M_Display>::Throw()
{
	throw m_Handler; 
}

template <typename M_Handler, typename M_Data, ShowDialog M_Display>
INT_PTR MessageManager<M_Handler, M_Data, M_Display>::Process( UINT MsgBoxType = MB_OK )
{
	INT_PTR Result( IDCANCEL );

	Log();

	Result = Display( MsgBoxType );

	return Result;
}

template <typename M_Handler, typename M_Data, ShowDialog M_Display>
INT_PTR MessageManager<M_Handler, M_Data, M_Display>::setMessage( DWORD MessageCode, LPCTSTR AdditionalText, LPCTSTR CompileDate, LPCTSTR CompileTime, LPCTSTR SourceFile, long SourceLine, LPCTSTR SourceDateTime, DWORD ProgramCode = 0, DWORD OSErrorCode = 0, LPCTSTR User=nullptr,  UINT MsgBoxType = MB_OK)
{
	INT_PTR Result( IDCANCEL );

	m_Handler.setMessage( MessageCode, AdditionalText, CompileDate, CompileTime, SourceFile, SourceLine, SourceDateTime, ProgramCode, OSErrorCode, User );

	Result = Process( MsgBoxType );

	return Result;
}

template <typename M_Handler, typename M_Data, ShowDialog M_Display>
INT_PTR MessageManager<M_Handler, M_Data, M_Display>::setMessage( const M_Data& rData,  UINT MsgBoxType = MB_OK)
{
	INT_PTR Result( IDCANCEL );

	m_Handler.setMessage( rData );

	Result = Process( MsgBoxType );

	return Result;
}

template <typename M_Handler, typename M_Data, ShowDialog M_Display>
M_Handler& MessageManager<M_Handler, M_Data, M_Display>::getMessageHandler()
{
	return m_Handler; 
}
#pragma endregion Public Methods
 
#pragma region Private Methods
template <typename M_Handler, typename M_Data, ShowDialog M_Display>
void MessageManager<M_Handler, M_Data, M_Display>::Log()
{
	if( LogOnly == m_Type || LogAndDisplay == m_Type )
	{
		m_Handler.logMessage();
	}
}

template <typename M_Handler, typename M_Data, ShowDialog M_Display>
INT_PTR MessageManager<M_Handler, M_Data, M_Display>::Display( const UINT MsgBoxType ) const
{
	INT_PTR Result( IDCANCEL );

	if( nullptr != M_Display && LogAndDisplay == m_Type)
	{
		SVString Msg;
		SVString MsgDetails;
		UINT Type ( MsgBoxType );

		MsgDetails = m_Handler.Format(Msg);
		//Message box type icon is determined by the severity of the message so set to 0 then get it from the container
		Type &= ~MB_ICONMASK;
		Type |= m_Handler.getSeverityIcon();

		Result = M_Display( NULL, Msg.c_str(), MsgDetails.c_str(), Type );
	}
	
	return Result;
}
#pragma endregion Private Methods
