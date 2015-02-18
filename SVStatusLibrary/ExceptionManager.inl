//******************************************************************************
/// COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//******************************************************************************
/// .Module Name     : ExceptionManager
/// .File Name       : $Workfile:   ExceptionManager.inl  $
/// .Description	 : The exception manager controls the storing, displaying and logging
///					   of information warning and error messages
/// ----------------------------------------------------------------------------
/// .Current Version : $Revision:   1.0  $
/// .Check In Date   : $Date:   18 Feb 2015 03:12:14  $
//******************************************************************************

#pragma region Constructor
template <typename EM_Data, typename EM_Display, typename EM_Log>
ExceptionManager<EM_Data, EM_Display, EM_Log>::ExceptionManager():
	m_Type( ExpTypeNone )
{
}

template <typename EM_Data, typename EM_Display, typename EM_Log>
ExceptionManager<EM_Data, EM_Display, EM_Log>::~ExceptionManager()
{
}
#pragma endregion Constructor
 
#pragma region Public Methods
template <typename EM_Data, typename EM_Display, typename EM_Log>
ExceptionManager<EM_Data, EM_Display, EM_Log>& ExceptionManager<EM_Data, EM_Display, EM_Log>::Instance()
{
	static ExceptionManager Object;

	return Object;
}

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
INT_PTR ExceptionManager<EM_Data, EM_Display, EM_Log>::Process( UINT MsgBoxType = MB_OK | MB_ICONEXCLAMATION )
{
	INT_PTR Result( IDCANCEL );

	Log();

	Result = Display( MsgBoxType );

	return Result;
}

template <typename EM_Data, typename EM_Display, typename EM_Log>
INT_PTR ExceptionManager<EM_Data, EM_Display, EM_Log>::setException( long ErrorCode, LPCTSTR ErrorText, TCHAR* CompileDate, TCHAR* CompileTime, TCHAR* SourceFile, long SourceLine, TCHAR* SourceDateTime, DWORD ProgramCode = 0, DWORD OSErrorCode = 0, UINT MsgBoxType = MB_OK | MB_ICONERROR)
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
INT_PTR ExceptionManager<EM_Data, EM_Display, EM_Log>::setException( const EM_Data& rData, UINT MsgBoxType = MB_OK | MB_ICONERROR)
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
INT_PTR ExceptionManager<EM_Data, EM_Display, EM_Log>::setException( long ErrorCode, LPVOID pErrorData, UINT ErrorDataSize, TCHAR* CompileDate, TCHAR* CompileTime, TCHAR* SourceFile, long SourceLine, TCHAR* SourceDateTime, DWORD ProgramCode = 0, DWORD OSErrorCode = 0, UINT MsgBoxType = MB_OK | MB_ICONERROR)
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
	if( ExpTypeEnum::LogAndDisplay == m_Type || ExpTypeEnum::LogAndDisplay == m_Type )
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

		MsgDetails = m_Data.Format(Msg);

		Result = EM_Display::showDialog( AfxGetMainWnd(), CString(Msg.c_str()), CString(MsgDetails.c_str()), MsgBoxType );
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