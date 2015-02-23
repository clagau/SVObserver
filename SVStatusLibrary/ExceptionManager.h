//******************************************************************************
/// COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// .Module Name     : ExceptionManager
/// .File Name       : $Workfile:   ExceptionManager.h  $
/// .Description	 : The exception manager controls the storing, displaying and logging
///					   of information warning and error messages
/// ----------------------------------------------------------------------------
/// .Current Version : $Revision:   1.1  $
/// .Check In Date   : $Date:   23 Feb 2015 03:44:56  $
//******************************************************************************
#pragma once
 
#pragma region Includes
#include "SVException.h"
#include "SVOMFCLibrary/DisplayMessageBox.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace SVStatusLibrary
	{
		#pragma region Declarations
			//This is a define because it uses the standard macros
			#define StdExceptionParams _T(__DATE__), _T(__TIME__), _T(__FILE__), __LINE__, _T(__TIMESTAMP__)

			/**********
			 The exception type enumeration
			***********/
			enum ExpTypeEnum
			{
				ExpTypeNone,
				LogOnly,
				LogAndDisplay
			};
		#pragma endregion Declarations

		//************************************
		//! This is the exception manager template class
		//! \param EM_Data [in] the exception data container which stores the information
		//! \param EM_Display [in] the exception display class
		//! \param EM_Log [in] the exception log class
		//************************************
		template <typename EM_Data, typename EM_Display, typename EM_Log>
		class ExceptionManager
		{
		#pragma region Constructor
		public:
			ExceptionManager<EM_Data, EM_Display, EM_Log>();

			//************************************
			//! Constructor which sets the message type
			//! \param Type [in] Determines if the message is logged, or displayed and logged
			//************************************
			ExceptionManager<EM_Data, EM_Display, EM_Log>( const ExpTypeEnum Type );

			virtual ~ExceptionManager();
		#pragma endregion Constructor

		#pragma region Public Methods
		public:
			//************************************
			//! Sets the exception type
			//! \param Type [in] Determines if the exception is logged, or displayed and logged
			//************************************
			void setType( const ExpTypeEnum Type );

			//************************************
			//! Throws the data container as an exception
			//************************************
			void Throw();

			//************************************
			//! Processes the exception that has already been set
			//! \param MsgBoxType [in] is the display message box type
			//! \returns the result of the message box or IDCANCEL if not displayed
			//************************************
			INT_PTR Process( const UINT MsgBoxType = MB_OK );

			//************************************
			//! Sets the message data
			//! \param ErrorCode [in] is the unique error number
			//! \param ErrorText [in] the accompanying text to the error code
			//! \param CompileDate [in] the source file compile date
			//! \param CompileTime [in] the source file compile time
			//! \param SourceFile [in] the source file path
			//! \param SourceLine [in] the source file line number
			//! \param SourceDateTime [in] the source file save date time
			//! \param ProgramCode [in] is the unique program error number
			//! \param OSErrorCode [in] is the unique OS error number
			//! \param MsgBoxType [in] is the display message box type
			//! \returns the result of the message box or IDCANCEL if not displayed
			//************************************
			INT_PTR setMessage( long ErrorCode, LPCTSTR sErrorText, TCHAR* CompileDate, TCHAR* CompileTime, TCHAR* SourceFile, const long SourceLine, TCHAR* SourceDateTime, DWORD ProgramCode = 0, DWORD OSErrorCode = 0, const UINT MsgBoxType = MB_OK );

			//************************************
			//! Sets the message data
			//! \param rData[in] reference to the exception data container
			//! \param MsgBoxType [in] is the display message box type
			//! \returns the result of the message box or IDCANCEL if not displayed
			//************************************
			INT_PTR setMessage( const EM_Data& rData, const UINT MsgBoxType = MB_OK );

			//************************************
			//! Sets the message data
			//! \param ErrorCode [in] is the unique error number
			//! \param pErrorData [in] pointer to the binary error data
			//! \param ErrorDataSize [in] the binary data size
			//! \param CompileDate [in] the source file compile date
			//! \param CompileTime [in] the source file compile time
			//! \param SourceFile [in] the source file path
			//! \param SourceLine [in] the source file line number
			//! \param SourceDateTime [in] the source file save date time
			//! \param ProgramCode [in] is the unique program error number
			//! \param OSErrorCode [in] is the unique OS error number
			//! \param MsgBoxType [in] is the display message box type
			//! \returns the result of the message box or IDCANCEL if not displayed
			//************************************
			INT_PTR setMessage( long ErrorCode, LPVOID pErrorData, UINT ErrorDataSize, TCHAR* CompileDate, TCHAR* CompileTime, TCHAR* SourceFile, long SourceLine, TCHAR* SourceDateTime, DWORD ProgramCode = 0, DWORD OSErrorCode = 0, const UINT MsgBoxType = MB_OK );
		#pragma endregion Public Methods

		#pragma region Private Methods
		private:
			//************************************
			//! Logs the exception if the type is set to be logged
			//************************************
			void Log();

			//************************************
			//! Displays the exception if the type is set to be displayed
			//! \param MsgBoxType [in] is the display message box type
			//! \returns the result of the message box or IDCANCEL if not displayed
			//************************************
			INT_PTR Display( const UINT MsgBoxType ) const;

		#pragma endregion Private Methods

		#pragma region Member variables
		private:
			EM_Data m_Data;							//! The exception data container
			EM_Log m_Log;							//! The exception logger
			ExpTypeEnum m_Type;						//! The exception type
		#pragma endregion Member variables
		};

		#pragma region Inline
		#include "ExceptionManager.inl"
		#pragma endregion Inline

		//This declares exception manager 1 which uses SVException, DisplayMessageBox and SVException as the template parameters
		typedef ExceptionManager<SVException, SvOml::DisplayMessageBox, SVException> ExceptionMgr1;
	}
}

namespace SvStl = Seidenader::SVStatusLibrary;

//******************************************************************************
/// LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVStatusLibrary\ExceptionManager.h_v  $
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
 *    Rev 1.0   18 Feb 2015 03:12:16   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  984
 * SCR Title:  Exception Display Class with Exception Manager Template SVO-524
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Initial check in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 */
 