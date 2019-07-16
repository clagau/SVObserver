//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// This is the message manager which controls the storing, displaying and logging
///	of information warning and error messages
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "MessageManager.h"
#pragma endregion Includes

#pragma region Constructor


namespace SvStl
{
	//! The static functor as pointers so that different instances ( exe dlls can still use one common functor)
	ShowDisplayFunctor* MessageMgrStd::m_ppShowDisplay {nullptr};
	NotifyFunctor* MessageMgrStd::m_ppNotify {nullptr};

	MessageMgrStd::MessageMgrStd(MsgType type) :
		m_Type(type)
	{
		if (MsgType::Data != type)
		{
			Initialize();
		}
	}


	MessageMgrStd::~MessageMgrStd()
	{
	}
	#pragma endregion Constructor

	#pragma region Public Methods

	void MessageMgrStd::setShowDisplayFunction(ShowDisplayFunctor ShowDisplay)
	{
		Initialize();
		if (nullptr != m_ppShowDisplay)
		{
			*m_ppShowDisplay = ShowDisplay;
		}
	}


	void MessageMgrStd::setNotificationFunction(const NotifyFunctor& rNotify)
	{
		Initialize();
		if (nullptr != m_ppNotify)
		{
			*m_ppNotify = rNotify;
		}
	}



	void MessageMgrStd::Throw()
	{
		throw m_MessageHandler;
	}

	constexpr uint32_t s_maximumNumberOfCodeRepeats = 4;


	INT_PTR MessageMgrStd::Process(UINT MsgBoxType /*= MB_OK*/)
	{
		// We want to avoid to log or notify successive "identical" messages.
		// Comparing source file names was considered unnececessary, since messages with
		// identical MessageCodes and Line Numbers but different source files appear extremely unlikely
		static DWORD s_previousMessageCode = 0;
		static long s_previousLineNumber = 0;
		static uint32_t s_codeRepeats = 0;

		const auto &currentMessage = m_MessageHandler.getMessage();

		//Since we are only comparing numbers, no synchronisation was considered necessary.
		//If in the future strings or more complex data are to be compared, a mutex or similar will be necessary
		if (currentMessage.m_MessageCode == s_previousMessageCode && currentMessage.m_SourceFile.m_Line == s_previousLineNumber)
		{
			s_codeRepeats++;
		}
		else
		{
			s_codeRepeats = 0;
			s_previousMessageCode = currentMessage.m_MessageCode;
			s_previousLineNumber = currentMessage.m_SourceFile.m_Line;
		}

		if (s_codeRepeats <= s_maximumNumberOfCodeRepeats)
		{
			Log();

			Notify();
		}


		return Display(MsgBoxType);
	}


	INT_PTR MessageMgrStd::setMessage(DWORD MessageCode, LPCTSTR AdditionalText, const SourceFileParams& rSourceFile, DWORD ProgramCode /*= 0*/, const GUID& rObjectId /*= GUID_NULL*/, UINT MsgBoxType /*= MB_OK*/)
	{
		SvDef::StringVector textList;
		MessageTextEnum id = SvStl::Tid_Empty;
		if (nullptr != AdditionalText)
		{
			textList.push_back(AdditionalText);
			id = SvStl::Tid_Default;
		}

		return setMessage(MessageCode, id, textList, rSourceFile, ProgramCode, rObjectId, MsgBoxType);
	}


	INT_PTR MessageMgrStd::setMessage(DWORD MessageCode, MessageTextEnum AdditionalTextId, const SourceFileParams& rSourceFile, DWORD ProgramCode /*= 0*/, const GUID& rObjectId /*= GUID_NULL*/, UINT MsgBoxType /*= MB_OK*/)
	{
		return setMessage(MessageCode, AdditionalTextId, SvDef::StringVector(), rSourceFile, ProgramCode, rObjectId, MsgBoxType);
	}


	INT_PTR MessageMgrStd::setMessage(DWORD MessageCode, MessageTextEnum AdditionalTextId, const SvDef::StringVector& rAdditionalTextList, const SourceFileParams& rSourceFile, DWORD ProgramCode /*= 0*/, const GUID& rObjectId /*= GUID_NULL*/, UINT MsgBoxType /*= MB_OK*/)
	{
		INT_PTR Result(IDCANCEL);

		m_MessageHandler.setMessage(MessageCode, AdditionalTextId, rAdditionalTextList, rSourceFile, ProgramCode, rObjectId);

		Result = Process(MsgBoxType);

		return Result;
	}


	INT_PTR MessageMgrStd::setMessage(const MessageData& rData, const GUID& rObjectId /*= GUID_NULL*/, UINT MsgBoxType /*= MB_OK */)
	{
		INT_PTR Result(IDCANCEL);

		m_MessageHandler.setMessage(rData, rObjectId);

		Result = Process(MsgBoxType);

		return Result;
	}


	MessageContainer& MessageMgrStd::getMessageContainer()
	{
		return m_MessageHandler;
	}
	#pragma endregion Public Methods

	#pragma region Private Methods

	void MessageMgrStd::Initialize()
	{
		//Initialize the static members just once per instance
		if (nullptr == m_ppShowDisplay && nullptr == m_ppNotify)
		{
			MessageContainer MessageHandler;
			MessageHandler.setFunctorObjects(m_ppShowDisplay, m_ppNotify);
		}
	}


	void MessageMgrStd::Log() const
	{
		if (MsgType::Log & m_Type)
		{
			m_MessageHandler.logMessage();
		}

	}



	void MessageMgrStd::Notify() const
	{
		bool doNotify = (MsgType::Notify & m_Type) || (SVSVIMStateClass::CheckState(SV_STATE_REMOTE_CMD) && (MsgType::Display& m_Type));

		if (nullptr != m_ppNotify && nullptr != *m_ppNotify && doNotify)
		{
			std::string Msg;
			m_MessageHandler.Format(Msg);
			long MsgCode = (0 != m_MessageHandler.getMessage().m_ProgramCode) ? m_MessageHandler.getMessage().m_ProgramCode : m_MessageHandler.getMessage().m_MessageCode;
			long msgNotify {static_cast<long> (NotificationType::message)};
			long logMsgBox {static_cast<long> (NotificationMsgEnum::MsgLog)};
			(*m_ppNotify)(msgNotify, logMsgBox, MsgCode, Msg.c_str());
		}
	}



	INT_PTR MessageMgrStd::Display(const UINT MsgBoxType)
	{
		INT_PTR Result(IDCANCEL);

		//If the message has already been displayed do not display again
		if (nullptr != m_ppShowDisplay && nullptr != *m_ppShowDisplay && (MsgType::Display & m_Type)
			&& !SVSVIMStateClass::CheckState(SV_STATE_REMOTE_CMD)
			&& !m_MessageHandler.getMessage().m_Displayed)
		{
			std::string Msg;
			std::string MsgDetails;
			UINT Type(MsgBoxType);
			int MsgCode(0);

			MsgDetails = m_MessageHandler.Format(Msg);
			MsgCode = (0 != m_MessageHandler.getMessage().m_ProgramCode) ? m_MessageHandler.getMessage().m_ProgramCode : m_MessageHandler.getMessage().m_MessageCode;
			//Message box type icon is determined by the severity of the message so set to 0 then get it from the container
			Type &= ~MB_ICONMASK;
			Type |= m_MessageHandler.getSeverityIcon();

			if (nullptr != m_ppNotify && nullptr != *m_ppNotify)
			{
				long msgNotify {static_cast<long> (NotificationType::message)};
				long startMsgBox {static_cast<long> (NotificationMsgEnum::StartMsgBox)};
				(*m_ppNotify)(msgNotify, startMsgBox, MsgCode, Msg.c_str());
			}
			Result = (*m_ppShowDisplay)(nullptr, Msg.c_str(), MsgDetails.c_str(), Type);
			//Message has been displayed do not display again
			m_MessageHandler.getMessage().m_Displayed = true;
			if (nullptr != m_ppNotify && nullptr != *m_ppNotify)
			{
				long msgNotify {static_cast<long> (NotificationType::message)};
				long endMsgBox {static_cast<long> (NotificationMsgEnum::EndMsgBox)};
				(*m_ppNotify)(msgNotify, endMsgBox, MsgCode, Msg.c_str());
			}

			//the message now has been displayed
			m_MessageHandler.setMessageDisplayed();
		}

		return Result;
	}
	#pragma endregion Private Methods
}