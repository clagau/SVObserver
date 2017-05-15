//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Class to generate a text from a ID (enum) and additional text list depending of language settings
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "MessageTextGenerator.h"
#include "MessageTextMap.h"
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
	MessageTextGenerator& MessageTextGenerator::Instance()
	{
		static MessageTextGenerator object;

		return object;
	}

	MessageTextGenerator::MessageTextGenerator()
	{
	}
#pragma endregion Constructor

#pragma region Public Methods
	SVString MessageTextGenerator::getText(MessageTextEnum messageId, const SVStringVector &additionalList) const
	{
		SVString retString("");
		MessageTextMap::const_iterator it = g_MessageTextMap.find(messageId);
		if (it != g_MessageTextMap.end())
		{
			retString = it->second;
			//replace %s with additionalList-strings
			for (int i = 0; i < additionalList.size(); i++)
			{
				size_t pos = retString.find(_T("%s"));

				if (SVString::npos != pos)
				{
					SVString addMessage = convertAddtionalListTextToString(additionalList[i]);
					retString.replace(pos, strlen(_T("%s")), addMessage);
				}
			}
		}
		return retString;
	}

	SVString MessageTextGenerator::convertId2AddtionalText(MessageTextEnum id)
	{
		return SvUl_SF::Format(_T("[***%d***]"), id);
	}
#pragma endregion Public Methods

#pragma region Protected Methods

#pragma endregion Protected Methods

#pragma region Private Methods
	SVString MessageTextGenerator::convertAddtionalListTextToString(const SVString& text) const
	{
		SVString retString;
		int number = -1;;
		if (1 == sscanf_s(text.c_str(), _T("[***%d***]"), &number))
		{
			MessageTextEnum id = static_cast<MessageTextEnum>(number);
			return getText(id);
		}
		else
		{
			return text;
		}
	}
#pragma endregion Private Methods
} //namespace SvStl