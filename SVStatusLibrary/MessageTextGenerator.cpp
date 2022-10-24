//*****************************************************************************
/// \copyright (c) 2016,2016 by Körber Pharma Inspection GmbH. All Rights Reserved
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

#pragma endregion Constructor

#pragma region Public Methods
	std::string MessageTextGenerator::getText(MessageTextEnum messageId, const SvDef::StringVector &additionalList)  const
	{
		std::string retString("");
		MessageTextMap::const_iterator it = cMessageTextMap.find(messageId);
		if (it != cMessageTextMap.end())
		{
			retString = it->second;

			for (int i = 0; i < static_cast<int> (additionalList.size()); i++)
			{
				std::string search {std::format("{{{}}}", i)};
				size_t pos = retString.find(search.c_str());

				if (std::string::npos != pos)
				{
					retString.replace(pos, search.size(), additionalList[i]);
				}
			}
		}
		return retString;
	}

	std::string MessageTextGenerator::convertId2AddtionalText(MessageTextEnum id)
	{
		return std::format(_T("[***{}***]"), static_cast<int>(id));
	}

	MessageTextEnum MessageTextGenerator::convertAdditionalText2Id(const std::string& text)
	{
		int number = -1;
		if (1 == sscanf_s(text.c_str(), _T("[***%d***]"), &number))
		{
			return static_cast<MessageTextEnum>(number);
		}
		return MessageTextEnum::Tid_Empty;
	}
#pragma endregion Public Methods

#pragma region Protected Methods

#pragma endregion Protected Methods

#pragma region Private Methods
	std::string MessageTextGenerator::convertAddtionalListTextToString(const std::string& text) const
	{
		int number = -1;
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