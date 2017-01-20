//*****************************************************************************
/// \copyright (c) $YEAR$,$YEAR$ by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Class to generate a text from a ID (enum) and additional text list depending of language settings
//******************************************************************************
#pragma once
#pragma region Includes
#include "SVUtilityLibrary\SVString.h"
#include "ObjectInterfaces\MessageTextEnum.h"
#pragma endregion Includes

#pragma region Declarations
#pragma endregion Declarations

namespace Seidenader { namespace SVStatusLibrary
{
	class MessageTextGenerator
	{
#pragma region Constructor
	public:
		static MessageTextGenerator& Instance();

	private:
		MessageTextGenerator();
#pragma endregion Constructor

#pragma region Public Methods
	public:
		//************************************
		/// Return the message text
		/// \param messageId [in] The message ID
		/// \param additionalList [in] A list of the additional text parts
		/// \returns SVString
		//************************************
		SVString getText(SvOi::MessageTextEnum messageId, const SVStringVector &additionalList = SVStringVector()) const;

		//************************************
		/// Convert an enum id to a string for setting it to the addtionalTextList.
		/// \param id [in] the id
		/// \returns SVString
		//************************************
		static SVString convertId2AddtionalText(SvOi::MessageTextEnum id);
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
		SVString convertAddtionalListTextToString(const SVString& text) const;
#pragma endregion Private Methods

#pragma region Member Variables
	private:

#pragma endregion Member Variables
	};
} /* namespace SVStatusLibrary */ } /* namespace Seidenader */

namespace SvStl = Seidenader::SVStatusLibrary;