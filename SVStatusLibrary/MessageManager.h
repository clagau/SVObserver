//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// This is the message manager which controls the storing, displaying and logging
///	of information warning and error messages
//******************************************************************************

#pragma once
 
#pragma region Includes
#include "MessageContainer.h"
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

		enum NotificationEnum
		{
				MsgUknown		=0,
				StartMsgBox		=1,
				EndMsgBox		=2,
				MsgLog			=3
		};
	
#pragma endregion Declarations

	//************************************
	//! This is the message manager template class
	//! \param M_Container [in] the message data container which stores and logs the information
	//! \param M_Data [in] the message data structure
	//************************************
	template <typename M_Container, typename M_Data>
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
		//! Sets the show display function
		//! \param  [in] pointer to the display functor
		//************************************
		static void setShowDisplayFunction( ShowDisplayFunctor ShowDisplay );

		//************************************
		//! Sets the notification function
		//! \param  [in] pointer to the notification function
		//************************************
		static void setNotificationFunction( NotifyFunctor Notify );

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
		//! \param SourceFile <in> the source file standard parameters in which the code set
		//! \param ProgramCode [in] is the unique program error number
		//! \param rObjectId <in> reference to the unique object ID of the message owner
		//! \param MsgBoxType [in] is the display message box type
		//! \returns the result of the message box or IDCANCEL if not displayed
		//************************************
		INT_PTR setMessage( DWORD MessageCode, LPCTSTR sAdditionalText, SourceFileParams SourceFile, DWORD ProgramCode = 0, const GUID& rObjectId=SV_GUID_NULL, const UINT MsgBoxType = MB_OK );

		//************************************
		//! Sets the message data
		//! \param MessageCode [in] is the unique message number
		//! \param AdditionalTextId <in> Id for the additional text to the message code (without AdditionalTextList)
		//! \param SourceFile <in> the source file standard parameters in which the code set
		//! \param ProgramCode [in] is the unique program error number
		//! \param rObjectId <in> reference to the unique object ID of the message owner
		//! \param MsgBoxType [in] is the display message box type
		//! \returns the result of the message box or IDCANCEL if not displayed
		//************************************
		INT_PTR setMessage( DWORD MessageCode, SvOi::MessageTextEnum AdditionalTextId, SourceFileParams SourceFile, DWORD ProgramCode = 0, const GUID& rObjectId=SV_GUID_NULL, const UINT MsgBoxType = MB_OK );

		//************************************
		//! Sets the message data
		//! \param MessageCode [in] is the unique message number
		//! \param AdditionalTextId <in> Id for the additional text to the message code
		//! \param rAdditionalTextList <in> reference to list of strings for the additional text to the message code
		//! \param SourceFile <in> the source file standard parameters in which the code set
		//! \param ProgramCode [in] is the unique program error number
		//! \param rObjectId <in> reference to the unique object ID of the message owner
		//! \param MsgBoxType [in] is the display message box type
		//! \returns the result of the message box or IDCANCEL if not displayed
		//************************************
		INT_PTR setMessage( DWORD MessageCode, SvOi::MessageTextEnum AdditionalTextId, const SVStringArray& rAdditionalTextList, SourceFileParams SourceFile, DWORD ProgramCode = 0, const GUID& rObjectId=SV_GUID_NULL, const UINT MsgBoxType = MB_OK );

		//************************************
		//! Sets the message data
		//! \param rData[in] reference to the message data
		//! \param rObjectId <in> reference to the unique object ID of the message owner
		//! \param MsgBoxType [in] is the display message box type
		//! \returns the result of the message box or IDCANCEL if not displayed
		//************************************
		INT_PTR setMessage( const M_Data& rData, const GUID& rObjectId=SV_GUID_NULL, const UINT MsgBoxType = MB_OK );

		//************************************
		//! Gets the message container
		//! \returns a reference to the container
		//************************************
		M_Container& getMessageContainer();
#pragma endregion Public Methods

#pragma region Private Methods
	private:
		static void Initialize();

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
		M_Container m_MessageHandler;				//! The message handler
		MsgTypeEnum m_Type;							//! The message type
		static ShowDisplayFunctor* m_pShowDisplay;
		static NotifyFunctor* m_pNotify;
#pragma endregion Member variables
	};

#pragma region Inline
	#include "MessageManager.inl"
#pragma endregion Inline

#pragma region Declarations
	//This declares message manager standard which uses MessageContainer, MessageData as the template parameters
	typedef MessageManager<MessageContainer, MessageData> MessageMgrStd;

	//! The static functor as pointers so that different instances ( exe dlls can still use one common functor)
	ShowDisplayFunctor* MessageMgrStd::m_pShowDisplay( nullptr );
	NotifyFunctor* MessageMgrStd::m_pNotify( nullptr );
#pragma endregion Declarations

} /* namespace SVStatusLibrary */ } /* namespace Seidenader */

namespace SvStl = Seidenader::SVStatusLibrary;

 