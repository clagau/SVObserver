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
#include "SVSVIMStateClass.h"
#include "NotificationTypeEnum.h"
#pragma endregion Includes

namespace SvStl
{
	#pragma region Declarations
	/**********
		The message type enumeration
	***********/

	enum class MsgType : unsigned int
	{
		Data = 0,
		Log = 1 << 0,
		Display = 1 << 1,
		Notify = 1 << 2
	};

	inline constexpr MsgType operator|(MsgType lhs, MsgType rhs)
	{
		return static_cast<MsgType>(static_cast<unsigned int>(lhs) | static_cast<unsigned int>(rhs));
	}

	inline constexpr bool operator&(MsgType lhs, MsgType  rhs)
	{
		return (static_cast<unsigned int>(lhs) & static_cast<unsigned int>(rhs)) != 0;
	}

	enum NotificationMsgEnum
	{
		MsgUknown = 0,
		StartMsgBox = 1,
		EndMsgBox = 2,
		MsgLog = 3
	};

	#pragma endregion Declarations

	//************************************
	//! This is the message manager class
	//! \param M_Container [in] the message data container which stores and logs the information
	//! \param M_Data [in] the message data structure
	//************************************
	class MessageMgrStd
	{
	#pragma region Constructor
	public:
		//No default constructor

		//************************************
		//! Constructor which sets the message type 
		//! \param Type [in] Determines if the message is logged, or displayed and logged
		//************************************
		explicit MessageMgrStd(MsgType Type);

		virtual ~MessageMgrStd();
	#pragma endregion Constructor

	#pragma region Public Methods
	public:
		//************************************
		//! Sets the show display function
		//! \param  [in] pointer to the display functor
		//************************************
		static void setShowDisplayFunction(ShowDisplayFunctor ShowDisplay);

		//************************************
		//! Sets the notification function
		//! \param  [in] pointer to the notification function
		//************************************
		static void setNotificationFunction(const NotifyFunctor& rNotify);

		//************************************
		//! Sets the message type
		//! \param Type [in] Determines if the message is logged, or displayed and logged
		//************************************
		//void setType( const MsgTypeEnum Type );

		//************************************
		//! Throws the data container as an message
		//************************************
		__declspec(noreturn) void Throw();

		//************************************
		//! Processes the message that has already been set
		//! \param MsgBoxType [in] is the display message box type
		//! \returns the result of the message box or IDCANCEL if not displayed
		//************************************
		INT_PTR Process(const UINT MsgBoxType = MB_OK);

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
		INT_PTR setMessage(DWORD MessageCode, LPCTSTR sAdditionalText, const SourceFileParams& rSourceFile, DWORD ProgramCode = 0, const GUID& rObjectId = GUID_NULL, const UINT MsgBoxType = MB_OK);

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
		INT_PTR setMessage(DWORD MessageCode, MessageTextEnum AdditionalTextId, const SourceFileParams& rSourceFile, DWORD ProgramCode = 0, const GUID& rObjectId = GUID_NULL, const UINT MsgBoxType = MB_OK);

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
		INT_PTR setMessage(DWORD MessageCode, MessageTextEnum AdditionalTextId, const SvDef::StringVector& rAdditionalTextList, const SourceFileParams& rSourceFile, DWORD ProgramCode = 0, const GUID& rObjectId = GUID_NULL, const UINT MsgBoxType = MB_OK);

		//************************************
		//! Sets the message data
		//! \param rData[in] reference to the message data
		//! \param rObjectId <in> reference to the unique object ID of the message owner
		//! \param MsgBoxType [in] is the display message box type
		//! \returns the result of the message box or IDCANCEL if not displayed
		//************************************
		INT_PTR setMessage(const MessageData& rData, const GUID& rObjectId = GUID_NULL, const UINT MsgBoxType = MB_OK);

		//************************************
		//! Gets the message container
		//! \returns a reference to the container
		//************************************
		MessageContainer& getMessageContainer();
	#pragma endregion Public Methods

	#pragma region Private Methods
	private:
		static void Initialize();

		//!has this message been repeated so often it should be suppressed?
		bool MessageMgrStd::suppressLogAndNotify() const;

		//************************************
		//! Logs the message if the type is set to be logged
		//************************************
		void Log() const;

		//************************************
		//! Sends a notification if required
		//************************************
		void Notify() const;

		//************************************
		//! Displays the message if the type is set to be displayed
		//! \param MsgBoxType [in] is the display message box type
		//! \returns the result of the message box or IDCANCEL if not displayed
		//************************************
		INT_PTR Display(const UINT MsgBoxType);

	#pragma endregion Private Methods

	#pragma region Member variables
	private:
		MsgType m_Type {MsgType::Data};
		MessageContainer m_MessageHandler;				//! The message handler //@TODO [Arvid][8.20][16.7.2019] ein besserer Name für die Variable wäre vielleicht m_MessageContainer

		static ShowDisplayFunctor* m_ppShowDisplay;
		static NotifyFunctor* m_ppNotify;
	#pragma endregion Member variables
	};

} //namespace SvStl
