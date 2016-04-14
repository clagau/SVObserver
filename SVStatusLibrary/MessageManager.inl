//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// This is the message manager which controls the storing, displaying and logging
///	of information warning and error messages
//******************************************************************************

#pragma region Constructor
template <typename M_Container, typename M_Data, ShowDialog M_Display , Notify M_Notify>
MessageManager<M_Container, M_Data, M_Display, M_Notify>::MessageManager( const MsgTypeEnum Type ) :
m_Type( Type )
{
}

template <typename M_Container, typename M_Data, ShowDialog M_Display , Notify M_Notify>
MessageManager<M_Container, M_Data, M_Display, M_Notify>::~MessageManager()
{
}
#pragma endregion Constructor
 
#pragma region Public Methods
template <typename M_Container, typename M_Data, ShowDialog M_Display, Notify M_Notify>
void MessageManager<M_Container, M_Data, M_Display, M_Notify>::setType( const MsgTypeEnum Type )
{
	m_Type = Type;
}

template <typename M_Container, typename M_Data, ShowDialog M_Display , Notify M_Notify>
void MessageManager<M_Container, M_Data, M_Display, M_Notify>::Throw()
{
	throw M_Container; 
}

template <typename M_Container, typename M_Data, ShowDialog M_Display , Notify M_Notify>
INT_PTR MessageManager<M_Container, M_Data, M_Display, M_Notify>::Process( UINT MsgBoxType = MB_OK )
{
	INT_PTR Result( IDCANCEL );

	Log();

	Result = Display( MsgBoxType );
	if( nullptr != M_Display && LogAndDisplay == m_Type)
	{
		//Message has bin displayed
		M_Container.setMessageDisplayed();
	}

	return Result;
}

template <typename M_Container, typename M_Data, ShowDialog M_Display , Notify M_Notify>
INT_PTR MessageManager<M_Container, M_Data, M_Display, M_Notify>::setMessage( DWORD MessageCode, LPCTSTR AdditionalText, LPCTSTR CompileDate, LPCTSTR CompileTime, LPCTSTR SourceFile, long SourceLine, LPCTSTR SourceDateTime, DWORD ProgramCode = 0, DWORD OSErrorCode = 0, LPCTSTR User=nullptr,  UINT MsgBoxType = MB_OK)
{
	SVStringArray textList;
	SvOi::MessageTextEnum id = SvOi::Tid_Empty;
	if (nullptr != AdditionalText)
	{
		textList.push_back(AdditionalText);
		id = SvOi::Tid_Default;
	}

	return setMessage( MessageCode, id, textList, CompileDate, CompileTime, SourceFile, SourceLine, SourceDateTime, ProgramCode, OSErrorCode, User, MsgBoxType );
}

template <typename M_Container, typename M_Data, ShowDialog M_Display , Notify M_Notify>
INT_PTR MessageManager<M_Container, M_Data, M_Display, M_Notify>::setMessage( DWORD MessageCode, SvOi::MessageTextEnum AdditionalTextId, LPCTSTR CompileDate, LPCTSTR CompileTime, LPCTSTR SourceFile, long SourceLine, LPCTSTR SourceDateTime, DWORD ProgramCode = 0, DWORD OSErrorCode = 0, LPCTSTR User=nullptr,  UINT MsgBoxType = MB_OK)
{
	return setMessage( MessageCode, AdditionalTextId, SVStringArray(), CompileDate, CompileTime, SourceFile, SourceLine, SourceDateTime, ProgramCode, OSErrorCode, User, MsgBoxType );
}

template <typename M_Container, typename M_Data, ShowDialog M_Display , Notify M_Notify>
INT_PTR MessageManager<M_Container, M_Data, M_Display, M_Notify>::setMessage( DWORD MessageCode, SvOi::MessageTextEnum AdditionalTextId, SVStringArray AdditionalTextList, LPCTSTR CompileDate, LPCTSTR CompileTime, LPCTSTR SourceFile, long SourceLine, LPCTSTR SourceDateTime, DWORD ProgramCode = 0, DWORD OSErrorCode = 0, LPCTSTR User=nullptr,  UINT MsgBoxType = MB_OK)
{
	INT_PTR Result( IDCANCEL );

	M_Container.setMessage( MessageCode, AdditionalTextId, AdditionalTextList, CompileDate, CompileTime, SourceFile, SourceLine, SourceDateTime, ProgramCode, OSErrorCode, User );

	Result = Process( MsgBoxType );

	return Result;
}

template <typename M_Container, typename M_Data, ShowDialog M_Display , Notify M_Notify>
INT_PTR MessageManager<M_Container, M_Data, M_Display, M_Notify>::setMessage( const M_Data& rData,  UINT MsgBoxType = MB_OK)
{
	INT_PTR Result( IDCANCEL );

	M_Container.setMessage( rData );

	Result = Process( MsgBoxType );

	return Result;
}

template <typename M_Container, typename M_Data, ShowDialog M_Display , Notify M_Notify>
M_Container& MessageManager<M_Container, M_Data, M_Display, M_Notify>::getMessageContainer()
{
	return M_Container; 
}
#pragma endregion Public Methods
 
#pragma region Private Methods
template <typename M_Container, typename M_Data, ShowDialog M_Display , Notify M_Notify>
void MessageManager<M_Container, M_Data, M_Display, M_Notify>::Log()
{
	if( LogOnly == m_Type || LogAndDisplay == m_Type )
	{
		M_Container.logMessage();
	}
}

template <typename M_Container, typename M_Data, ShowDialog M_Display , Notify M_Notify>
INT_PTR MessageManager<M_Container, M_Data, M_Display, M_Notify>::Display( const UINT MsgBoxType ) const
{
	INT_PTR Result( IDCANCEL );

	if( nullptr != M_Display && LogAndDisplay == m_Type)
	{
		SVString Msg;
		SVString MsgDetails;
		UINT Type ( MsgBoxType );

		MsgDetails = M_Container.Format(Msg);
		//Message box type icon is determined by the severity of the message so set to 0 then get it from the container
		Type &= ~MB_ICONMASK;
		Type |= M_Container.getSeverityIcon();

		if(nullptr != M_Notify )
		{
			M_Notify(SvStl::StartMsgBox,M_Container.getMessage().m_ProgramCode,Msg.c_str()  );
		}
		Result = M_Display( NULL, Msg.c_str(), MsgDetails.c_str(), Type );
		if(nullptr != M_Notify )
		{
			M_Notify(SvStl::EndMsgBox,M_Container.getMessage().m_ProgramCode,Msg.c_str()  );
		}
	}
	
	return Result;
}
#pragma endregion Private Methods
