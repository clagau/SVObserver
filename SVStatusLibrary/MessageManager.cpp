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
#include "SVSVIMStateClass.h"
#pragma endregion Includes

#pragma region Constructor


namespace SvStl
{
	//! The static functor as pointers so that different instances ( exe dlls can still use one common functor)
	ShowDisplayFunctor* MessageManager::m_ppShowDisplay {nullptr};
	MessageNotifyFunctor* MessageManager::m_ppNotify {nullptr};

	MessageManager::MessageManager(MsgType type) :
		m_Type(type)
	{
		if (MsgType::Data != type)
		{
			Initialize();
		}
	}


	MessageManager::~MessageManager()
	{
	}
	#pragma endregion Constructor

	#pragma region Public Methods

	void MessageManager::setShowDisplayFunction(ShowDisplayFunctor ShowDisplay)
	{
		Initialize();
		if (nullptr != m_ppShowDisplay)
		{
			*m_ppShowDisplay = ShowDisplay;
		}
	}


	void MessageManager::setNotificationFunction(const MessageNotifyFunctor& rNotify)
	{
		Initialize();
		if (nullptr != m_ppNotify)
		{
			*m_ppNotify = rNotify;
		}
	}



	void MessageManager::Throw()
	{
		throw m_MessageContainer;
	}

	constexpr uint32_t s_maximumNumberOfCodeRepeats = 4;


	INT_PTR MessageManager::Process(UINT MsgBoxType /*= MB_OK*/)
	{
		if (!suppressLogAndNotify())
		{
			Log();
			Notify();
		}

		return Display(MsgBoxType);
	}


	INT_PTR MessageManager::setMessage(DWORD MessageCode, LPCTSTR AdditionalText, const SourceFileParams& rSourceFile, DWORD ProgramCode /*= 0*/, uint32_t objectId /*= 0*/, UINT MsgBoxType /*= MB_OK*/)
	{
		SvDef::StringVector textList;
		MessageTextEnum id = SvStl::Tid_Empty;
		if (nullptr != AdditionalText)
		{
			textList.push_back(AdditionalText);
			id = SvStl::Tid_Default;
		}

		return setMessage(MessageCode, id, textList, rSourceFile, ProgramCode, objectId, MsgBoxType);
	}


	INT_PTR MessageManager::setMessage(DWORD MessageCode, MessageTextEnum AdditionalTextId, const SourceFileParams& rSourceFile, DWORD ProgramCode /*= 0*/, uint32_t objectId /*= 0*/, UINT MsgBoxType /*= MB_OK*/)
	{
		return setMessage(MessageCode, AdditionalTextId, SvDef::StringVector(), rSourceFile, ProgramCode, objectId, MsgBoxType);
	}


	INT_PTR MessageManager::setMessage(DWORD MessageCode, MessageTextEnum AdditionalTextId, const SvDef::StringVector& rAdditionalTextList, const SourceFileParams& rSourceFile, DWORD ProgramCode /*= 0*/, uint32_t objectId /*= 0*/, UINT MsgBoxType /*= MB_OK*/)
	{
		INT_PTR Result(IDCANCEL);

		m_MessageContainer.setMessage(MessageCode, AdditionalTextId, rAdditionalTextList, rSourceFile, ProgramCode, objectId);

		Result = Process(MsgBoxType);

		return Result;
	}


	INT_PTR MessageManager::setMessage(const MessageData& rData, uint32_t objectId /*= 0*/, UINT MsgBoxType /*= MB_OK */)
	{
		INT_PTR Result(IDCANCEL);

		m_MessageContainer.setMessage(rData, objectId);

		Result = Process(MsgBoxType);

		return Result;
	}
	#pragma endregion Public Methods

	#pragma region Private Methods

	void MessageManager::Initialize()
	{
		//Initialize the static members just once per instance
		if (nullptr == m_ppShowDisplay && nullptr == m_ppNotify)
		{
			MessageContainer MessageHandler;
			MessageHandler.setFunctorObjects(m_ppShowDisplay, m_ppNotify);
		}
	}

	bool MessageManager::suppressLogAndNotify() const
	{
		// We want to avoid to log or notify successive "identical" messages.
		// Comparing source file names was considered unnecessary, since messages with
		// identical MessageCodes and Line Numbers but different source files appear extremely unlikely

		if ((MsgType::Log | MsgType::Notify) & m_Type)
		{
			static DWORD s_previousMessageCode = 0;
			static long s_previousLineNumber = 0;
			static uint32_t s_codeRepeats = 0;

			const auto &currentMessage = m_MessageContainer.getMessage();

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

			if (s_codeRepeats > s_maximumNumberOfCodeRepeats)
			{
				return true;
			}
		}

		return false;
	}

	void MessageManager::Log() const 
	{
		if (MsgType::Log & m_Type)
		{
			m_MessageContainer.logMessage();
		}
	}

	void MessageManager::Notify() const
	{
		bool doNotify = (MsgType::Notify & m_Type) || (SVSVIMStateClass::CheckState(SV_STATE_REMOTE_CMD) && (MsgType::Display& m_Type));

		if (nullptr != m_ppNotify && nullptr != *m_ppNotify && doNotify)
		{
			(*m_ppNotify)(m_MessageContainer, static_cast<int> (SvPb::MessageType::msgLog));
		}
	}

	INT_PTR MessageManager::Display(const UINT MsgBoxType)
	{
		INT_PTR Result(IDCANCEL);

		//If the message has already been displayed do not display again
		if (nullptr != m_ppShowDisplay && nullptr != *m_ppShowDisplay && (MsgType::Display & m_Type)
			&& !SVSVIMStateClass::CheckState(SV_STATE_REMOTE_CMD)
			&& !m_MessageContainer.getMessage().m_Displayed)
		{
			std::string Msg;
			std::string MsgDetails;
			UINT Type(MsgBoxType);

			MsgDetails = m_MessageContainer.Format(Msg);
			//Message box type icon is determined by the severity of the message so set to 0 then get it from the container
			Type &= ~MB_ICONMASK;
			Type |= m_MessageContainer.getSeverityIcon();

			if (nullptr != m_ppNotify && nullptr != *m_ppNotify)
			{
				(*m_ppNotify)(m_MessageContainer, static_cast<int> (SvPb::MessageType::startMsgBox));
			}
			Result = (*m_ppShowDisplay)(nullptr, Msg.c_str(), MsgDetails.c_str(), Type);
			//Message has been displayed do not display again
			m_MessageContainer.getMessage().m_Displayed = true;
			if (nullptr != m_ppNotify && nullptr != *m_ppNotify)
			{
				(*m_ppNotify)(m_MessageContainer, static_cast<int> (SvPb::MessageType::endMsgBox));
			}

			//the message now has been displayed
			m_MessageContainer.setMessageDisplayed();
		}

		return Result;
	}
	#pragma endregion Private Methods
}