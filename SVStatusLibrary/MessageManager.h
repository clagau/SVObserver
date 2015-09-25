//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// This is the message manager which controls the storing, displaying and logging
///	of information warning and error messages
//******************************************************************************

#pragma once
 
#pragma region Includes
#include "MessageHandler.h"
#pragma endregion Includes

namespace Seidenader { namespace SVStatusLibrary
{
#pragma region Declarations
		/**********
			The message type enumeration
		***********/
		enum MsgTypeEnum
		{
			DataOnly,
			LogOnly,
			LogAndDisplay
		};
		typedef INT_PTR (*ShowDialog)( HWND, LPCTSTR, LPCTSTR, UINT ); 

		enum NotificationEnum
		{
				MsgUknown =0,	
				StartMsgBox =1,
				EndMsgBox =2,
				MsgLog =3
		};
		typedef INT_PTR (*Notify)(NotificationEnum, int, LPCTSTR );
	
#pragma endregion Declarations

	//************************************
	//! This is the message manager template class
	//! \param M_Handler [in] the message data handler which stores and logs the information
	//! \param M_Struct [in] the message data structure
	//! \param M_Display [in] the message display class
	//************************************
	template <typename M_Handler, typename M_Data, ShowDialog M_Display, Notify M_Notify >
	class MessageManager
	{
#pragma region Constructor
	public:
		//No default constructor

		//************************************
		//! Constructor which sets the message type 
		//! \param Type [in] Determines if the message is logged, or displayed and logged
		//************************************
		MessageManager( const MsgTypeEnum Type );

		virtual ~MessageManager();
#pragma endregion Constructor

#pragma region Public Methods
	public:
		//************************************
		//! Sets the message type
		//! \param Type [in] Determines if the message is logged, or displayed and logged
		//************************************
		void setType( const MsgTypeEnum Type );

		//************************************
		//! Throws the data container as an message
		//************************************
		void Throw();

		//************************************
		//! Processes the message that has already been set
		//! \param MsgBoxType [in] is the display message box type
		//! \returns the result of the message box or IDCANCEL if not displayed
		//************************************
		INT_PTR Process( const UINT MsgBoxType = MB_OK );

		//************************************
		//! Sets the message data
		//! \param MessageCode [in] is the unique message number
		//! \param AdditionalText [in] the additional text to the message code
		//! \param CompileDate [in] the source file compile date
		//! \param CompileTime [in] the source file compile time
		//! \param SourceFile [in] the source file path
		//! \param SourceLine [in] the source file line number
		//! \param SourceDateTime [in] the source file save date time
		//! \param ProgramCode [in] is the unique program error number
		//! \param OSErrorCode [in] is the unique OS error number
		//! \param User [in] the user for security messages
		//! \param MsgBoxType [in] is the display message box type
		//! \returns the result of the message box or IDCANCEL if not displayed
		//************************************
		INT_PTR setMessage( DWORD MessageCode, LPCTSTR sAdditionalText, LPCTSTR CompileDate, LPCTSTR CompileTime, LPCTSTR SourceFile, const long SourceLine, LPCTSTR SourceDateTime, DWORD ProgramCode = 0, DWORD OSErrorCode = 0, LPCTSTR User = nullptr, const UINT MsgBoxType = MB_OK );

		//************************************
		//! Sets the message data
		//! \param rData[in] reference to the message data
		//! \param MsgBoxType [in] is the display message box type
		//! \returns the result of the message box or IDCANCEL if not displayed
		//************************************
		INT_PTR setMessage( const M_Data& rData, const UINT MsgBoxType = MB_OK );

		//************************************
		//! Gets the message handler container
		//! \returns a reference to the container
		//************************************
		M_Handler& getMessageHandler();
#pragma endregion Public Methods

#pragma region Private Methods
	private:
		//************************************
		//! Logs the message if the type is set to be logged
		//************************************
		void Log();

		//************************************
		//! Displays the message if the type is set to be displayed
		//! \param MsgBoxType [in] is the display message box type
		//! \returns the result of the message box or IDCANCEL if not displayed
		//************************************
		INT_PTR Display( const UINT MsgBoxType ) const;

	#pragma endregion Private Methods

	#pragma region Member variables
	private:
		M_Handler m_Handler;					//! The message handler
		MsgTypeEnum m_Type;						//! The message type
#pragma endregion Member variables
	};

#pragma region Inline
	#include "MessageManager.inl"
#pragma endregion Inline

#pragma region Declarations
	//Note the resource dependent message managers are declared in the header file MessageManagerResource.h
	//This declares message manager no display which uses MessageHandler, MessageData and nullptr (no display method) as the template parameters
	typedef MessageManager<MessageHandler, MessageData, nullptr,nullptr> MessageMgrNoDisplay;
#pragma endregion Declarations

} /* namespace SVStatusLibrary */ } /* namespace Seidenader */

namespace SvStl = Seidenader::SVStatusLibrary;

 