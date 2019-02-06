//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// This is the message manager which controls the storing, displaying and logging
///	of information warning and error messages
//******************************************************************************

#pragma region Constructor
template <typename M_Container, typename M_Data>
MessageManager<M_Container, M_Data>::MessageManager(MsgType type) :
	m_Type(type)
{
	if (MsgType::Data != type)
	{
		Initialize();
	}
}

template <typename M_Container, typename M_Data>
MessageManager<M_Container, M_Data>::~MessageManager()
{
}
#pragma endregion Constructor

#pragma region Public Methods
template <typename M_Container, typename M_Data>
void MessageManager<M_Container, M_Data>::setShowDisplayFunction(ShowDisplayFunctor ShowDisplay)
{
	Initialize();
	if (nullptr != m_ppShowDisplay)
	{
		*m_ppShowDisplay = ShowDisplay;
	}
}

template <typename M_Container, typename M_Data>
void MessageManager<M_Container, M_Data>::setNotificationFunction(NotifyFunctor Notify)
{
	Initialize();
	if (nullptr != m_ppNotify)
	{
		*m_ppNotify = Notify;
	}
}


template <typename M_Container, typename M_Data>
void MessageManager<M_Container, M_Data>::Throw()
{
	throw m_MessageHandler;
}

template <typename M_Container, typename M_Data>
INT_PTR MessageManager<M_Container, M_Data>::Process(UINT MsgBoxType /*= MB_OK*/)
{
	INT_PTR Result(IDCANCEL);

	Log();

	Result = Display(MsgBoxType);
	if (nullptr != m_ppShowDisplay && nullptr != *m_ppShowDisplay && (MsgType::Display & m_Type))
	{
		//Message has bin displayed
		m_MessageHandler.setMessageDisplayed();
	}

	return Result;
}

template <typename M_Container, typename M_Data>
INT_PTR MessageManager<M_Container, M_Data>::setMessage(DWORD MessageCode, LPCTSTR AdditionalText, SourceFileParams SourceFile, DWORD ProgramCode /*= 0*/, const GUID& rObjectId /*= GUID_NULL*/, UINT MsgBoxType /*= MB_OK*/)
{
	SvDef::StringVector textList;
	MessageTextEnum id = SvStl::Tid_Empty;
	if (nullptr != AdditionalText)
	{
		textList.push_back(AdditionalText);
		id = SvStl::Tid_Default;
	}

	return setMessage(MessageCode, id, textList, SourceFile, ProgramCode, rObjectId, MsgBoxType);
}

template <typename M_Container, typename M_Data>
INT_PTR MessageManager<M_Container, M_Data>::setMessage(DWORD MessageCode, MessageTextEnum AdditionalTextId, SourceFileParams SourceFile, DWORD ProgramCode /*= 0*/, const GUID& rObjectId /*= GUID_NULL*/, UINT MsgBoxType /*= MB_OK*/)
{
	return setMessage(MessageCode, AdditionalTextId, SvDef::StringVector(), SourceFile, ProgramCode, rObjectId, MsgBoxType);
}

template <typename M_Container, typename M_Data>
INT_PTR MessageManager<M_Container, M_Data>::setMessage(DWORD MessageCode, MessageTextEnum AdditionalTextId, const SvDef::StringVector& rAdditionalTextList, SourceFileParams SourceFile, DWORD ProgramCode /*= 0*/, const GUID& rObjectId /*= GUID_NULL*/, UINT MsgBoxType /*= MB_OK*/)
{
	INT_PTR Result(IDCANCEL);

	m_MessageHandler.setMessage(MessageCode, AdditionalTextId, rAdditionalTextList, SourceFile, ProgramCode, rObjectId);

	Result = Process(MsgBoxType);

	return Result;
}

template <typename M_Container, typename M_Data>
INT_PTR MessageManager<M_Container, M_Data>::setMessage(const M_Data& rData, const GUID& rObjectId /*= GUID_NULL*/, UINT MsgBoxType /*= MB_OK */)
{
	INT_PTR Result(IDCANCEL);

	m_MessageHandler.setMessage(rData, rObjectId);

	Result = Process(MsgBoxType);

	return Result;
}

template <typename M_Container, typename M_Data>
M_Container& MessageManager<M_Container, M_Data>::getMessageContainer()
{
	return m_MessageHandler;
}
#pragma endregion Public Methods

#pragma region Private Methods
template <typename M_Container, typename M_Data>
void MessageManager<M_Container, M_Data>::Initialize()
{
	//Initialize the static members just once per instance
	if (nullptr == m_ppShowDisplay && nullptr == m_ppNotify)
	{
		M_Container MessageHandler;
		MessageHandler.setFunctorObjects(m_ppShowDisplay, m_ppNotify);
	}
}

template <typename M_Container, typename M_Data>
void MessageManager<M_Container, M_Data>::Log()
{
	if (MsgType::Log & m_Type)
	{
		m_MessageHandler.logMessage();
	}

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

template <typename M_Container, typename M_Data>
INT_PTR MessageManager<M_Container, M_Data>::Display(const UINT MsgBoxType) const
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
	}

	return Result;
}
#pragma endregion Private Methods
