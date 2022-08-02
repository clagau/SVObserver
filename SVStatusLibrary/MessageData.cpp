//*****************************************************************************
/// \copyright (c) 2015,2015 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
//*****************************************************************************
//*****************************************************************************
/// The structure contains the data for the message handler
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "MessageData.h"
#include "MessageTextGenerator.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace SvStl
{
#pragma region Constructor
	MessageData::MessageData( DWORD MessageCode, MessageTextEnum AdditionalTextId, const SvDef::StringVector& rAdditionalTextList ) :
	 m_MessageCode( MessageCode )
	,m_AdditionalTextId( AdditionalTextId )
	,m_AdditionalTextList( rAdditionalTextList )
	{
	}
#pragma endregion Constructor

#pragma region Public Methods
	void MessageData::clear()
	{
		m_Logged = false;
		m_Displayed = false;
		m_MessageCode = 0;
		m_AdditionalTextId = SvStl::Tid_Empty;
		m_AdditionalTextList.clear();
		m_SourceFile.clear();
		m_DateTime = 0;
	}

	std::string MessageData::getAdditionalText() const
	{
		return MessageTextGenerator::Instance().getText(m_AdditionalTextId, m_AdditionalTextList);
	}

	std::string MessageData::convertId2AdditionalText(MessageTextEnum id)
	{
		return MessageTextGenerator::convertId2AddtionalText(id);
	}

	MessageTextEnum MessageData::convertAdditionalText2Id(const std::string& text)
	{
		return MessageTextGenerator::convertAdditionalText2Id(text);
	}
#pragma endregion Public Methods
} //namespace SvStl