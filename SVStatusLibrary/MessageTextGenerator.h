//*****************************************************************************
/// \copyright (c) $YEAR$,$YEAR$ by K�rber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
//*****************************************************************************
/// Class to generate a text from a ID (enum) and additional text list depending of language settings
//******************************************************************************
#pragma once
#pragma region Includes
#include "MessageTextEnum.h"
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

#pragma region Declarations
#pragma endregion Declarations

namespace SvStl
{
	class MessageTextGenerator //@TODO [Arvid][10.20][7.7.2021] this class has no data members! it should be removed and its methods converted to free functions
	{
#pragma region Constructor
	public:
		static MessageTextGenerator& Instance();

	private:
		MessageTextGenerator()=default; 
#pragma endregion Constructor

#pragma region Public Methods
	public:
		//************************************
		/// Return the message text
		/// \param messageId [in] The message ID
		/// \param additionalList [in] A list of the additional text parts
		/// \returns SVString
		//************************************
		std::string getText(MessageTextEnum messageId, const SvDef::StringVector &additionalList = SvDef::StringVector()) const;

		//************************************
		/// Convert an enum id to a string for setting it to the addtionalTextList.
		/// \param id [in] the id
		/// \returns SVString
		//************************************
		static std::string convertId2AddtionalText(MessageTextEnum id);

		static MessageTextEnum convertAdditionalText2Id(const std::string& text);
#pragma endregion Public Methods

#pragma region Protected Methods
	protected:

#pragma endregion Protected Methods

#pragma region Private Methods
	private:
		//************************************
		/// Convert a additional text to text. If input text an id, it convert it to text, else input text is output text.
		/// \param text [in] 
		/// \returns SVString
		//************************************
		std::string convertAddtionalListTextToString(const std::string& text) const;
#pragma endregion Private Methods

#pragma region Member Variables
	private:

#pragma endregion Member Variables
	};
} //namespace SvStl