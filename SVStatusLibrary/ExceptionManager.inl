//******************************************************************************
/// COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//******************************************************************************
/// .Module Name     : ExceptionManager
/// .File Name       : $Workfile:   ExceptionManager.inl  $
/// .Description	 : The exception manager controls the storing, displaying and logging
///					   of information warning and error messages
/// ----------------------------------------------------------------------------
/// .Current Version : $Revision:   1.1  $
/// .Check In Date   : $Date:   23 Feb 2015 03:44:56  $
//******************************************************************************

#pragma region Constructor
template <typename EM_Data, typename EM_Display, typename EM_Log>
ExceptionManager<EM_Data, EM_Display, EM_Log>::ExceptionManager() :
m_Type( LogOnly )
{
}

template <typename EM_Data, typename EM_Display, typename EM_Log>
ExceptionManager<EM_Data, EM_Display, EM_Log>::ExceptionManager( const ExpTypeEnum Type ) :
m_Type( Type )
{
}

template <typename EM_Data, typename EM_Display, typename EM_Log>
ExceptionManager<EM_Data, EM_Display, EM_Log>::~ExceptionManager()
{
}
#pragma endregion Constructor
 
#pragma region Public Methods
template <typename EM_Data, typename EM_Display, typename EM_Log>
void ExceptionManager<EM_Data, EM_Display, EM_Log>::setType( const ExpTypeEnum Type )
{
	m_Type = Type;
}

template <typename EM_Data, typename EM_Display, typename EM_Log>
void ExceptionManager<EM_Data, EM_Display, EM_Log>::Throw()
{
	throw m_Data; 
}

template <typename EM_Data, typename EM_Display, typename EM_Log>
INT_PTR ExceptionManager<EM_Data, EM_Display, EM_Log>::Process( UINT MsgBoxType = MB_OK )
{
	INT_PTR Result( IDCANCEL );

	Log();

	Result = Display( MsgBoxType );

	return Result;
}

template <typename EM_Data, typename EM_Display, typename EM_Log>
INT_PTR ExceptionManager<EM_Data, EM_Display, EM_Log>::setMessage( long ErrorCode, LPCTSTR ErrorText, TCHAR* CompileDate, TCHAR* CompileTime, TCHAR* SourceFile, long SourceLine, TCHAR* SourceDateTime, DWORD ProgramCode = 0, DWORD OSErrorCode = 0, UINT MsgBoxType = MB_OK)
{
	INT_PTR Result( IDCANCEL );

	m_Data.ResetException();
	m_Data.SetException(ErrorCode, CompileDate, CompileTime, ErrorText, SourceFile, SourceLine, SourceDateTime, ProgramCode, OSErrorCode);

	//If data and log are of the same type then they should be set to the same data
	if( std::is_same<EM_Data, EM_Log>::value )
	{
		m_Log = m_Data;
	}
	
	Result = Process( MsgBoxType );

	return Result;
}

template <typename EM_Data, typename EM_Display, typename EM_Log>
INT_PTR ExceptionManager<EM_Data, EM_Display, EM_Log>::setMessage( const EM_Data& rData, UINT MsgBoxType = MB_OK )
{
	INT_PTR Result( IDCANCEL );

	m_Data.ResetException();
	m_Data = rData;

	//If data and log are of the same type then they should be set to the same data
	if( std::is_same<EM_Data, EM_Log>::value )
	{
		m_Log = m_Data;
	}

	Result = Process( MsgBoxType );

	return Result;
}

template <typename EM_Data, typename EM_Display, typename EM_Log>
INT_PTR ExceptionManager<EM_Data, EM_Display, EM_Log>::setMessage( long ErrorCode, LPVOID pErrorData, UINT ErrorDataSize, TCHAR* CompileDate, TCHAR* CompileTime, TCHAR* SourceFile, long SourceLine, TCHAR* SourceDateTime, DWORD ProgramCode = 0, DWORD OSErrorCode = 0, UINT MsgBoxType = MB_OK )
{
	INT_PTR Result( IDCANCEL );

	m_Data.ResetException();
	m_Data.SetException(ErrorCode, CompileDate, CompileTime, pErrorData, ErrorDataSize, SourceFile, SourceLine, SourceDateTime, ProgramCode, OSErrorCode);

	//If data and log are of the same type then they should be set to the same data
	if( std::is_same<EM_Data, EM_Log>::value )
	{
		m_Log = m_Data;
	}

	Result = Process( MsgBoxType );

	return Result;
}
#pragma endregion Public Methods
 
#pragma region Private Methods
template <typename EM_Data, typename EM_Display, typename EM_Log>
void ExceptionManager<EM_Data, EM_Display, EM_Log>::Log()
{
	if( ExpTypeEnum::LogOnly == m_Type || ExpTypeEnum::LogAndDisplay == m_Type )
	{
		m_Log.LogException();
	}
}

template <typename EM_Data, typename EM_Display, typename EM_Log>
INT_PTR ExceptionManager<EM_Data, EM_Display, EM_Log>::Display( const UINT MsgBoxType ) const
{
	INT_PTR Result( IDCANCEL );

	if( ExpTypeEnum::LogAndDisplay == m_Type )
	{
		SVString Msg;
		SVString MsgDetails;
		UINT Type ( MsgBoxType );

		MsgDetails = m_Data.Format(Msg);
		//Message box type icon is determined by the severity of the message so set to 0 then get it from the container
		Type &= ~MB_ICONMASK;
		Type |= m_Data.getSeverityIcon();

		Result = EM_Display::showDialog( AfxGetMainWnd(), CString(Msg.c_str()), CString(MsgDetails.c_str()), Type );
	}
	
	return Result;
}

#pragma endregion Private Methods


//******************************************************************************
/// LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVStatusLibrary\ExceptionManager.inl_v  $
 * 
 *    Rev 1.1   23 Feb 2015 03:44:56   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  984
 * SCR Title:  Exception Display Class with Exception Manager Template SVO-524
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Removed exception manager singelton
 * Display Dialog icon set by message type
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Feb 2015 03:12:14   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  984
 * SCR Title:  Exception Display Class with Exception Manager Template SVO-524
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Initial check in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/