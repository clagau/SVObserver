//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// The structure contains the data for the message handler
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <ctime>
#include "SVUtilityLibrary/SVString.h"
#include "ObjectInterfaces/MessageTextEnum.h"
#pragma endregion Includes

namespace Seidenader { namespace SVStatusLibrary
{
	struct MessageData
	{
#pragma region Constructor
	public:
		MessageData();

		//************************************
		//! Constructor to set only the following parameters
		//! \param MessageCode <in> the message code
		//! \param AdditionalTextId <in> Id for the additional text
		//! \param AdditionalTextList <in> List of strings for the additional text
		//************************************
		MessageData( DWORD MessageCode, SvOi::MessageTextEnum AdditionalTextId = SvOi::Tid_Empty, SVStringArray AdditionalTextList = SVStringArray() );

		//************************************
		//! This is the copy constructor
		//! \param rRhs <in> reference to the message structure to copy
		//************************************
		MessageData( const MessageData& rRhs);

		//************************************
		//! This is the assignment operator
		//! \param rRhs <in> reference to a message structure with which to set the data
		//************************************
		const MessageData& operator=(const MessageData& rRhs);

		virtual ~MessageData();
#pragma endregion Constructor

#pragma region Public Methods
	public:
		//************************************
		//! This is the method to clear the data
		//************************************
		void clear();

		//************************************
		/// Create addtionalText and return it
		/// \returns SVString
		//************************************
		SVString getAdditionalText() const;

		//************************************
	/// Convert an enum id to a string for setting it to the addtionalTextList.
	/// \param id [in] the id
	/// \returns SVString
	//************************************
		static SVString convertId2AddtionalText(SvOi::MessageTextEnum id);
#pragma endregion Public Methods

#pragma region Member Variables
	public:
		bool m_Logged;						//Flag indicating if the message was written to the event log
		bool m_Displayed;					//Flag indicating if the message has been displayed
		DWORD m_MessageCode;				//Number identifying the message
		SvOi::MessageTextEnum m_AdditionalTextId;
		SVStringArray m_AdditionalTextList;
		SVString m_CompileDate;				//Date the source file was compiled.
		SVString m_CompileTime;				//Time the source file was compiled.
		SVString m_SourceFile;				//Name of the source file where the message was encountered.
		long m_SourceLine;					//Line number within the source file where the message was encountered.
		SVString m_SourceDateTime;			//Last modification date/time of the source file.
		DWORD m_ProgramCode;				//Program specific code.
		DWORD m_OSErrorCode;				//Operating system error code.
		SVString m_User;					//The user for security messages
		std::time_t m_DateTime;				//The date and time when the data is set
#pragma endregion Member Variables
	};

#pragma region Declarations
	typedef std::vector< MessageData > Messages;
#pragma endregion Declarations
} /* namespace SVStatusLibrary */ } /* namespace Seidenader */

namespace SvStl = Seidenader::SVStatusLibrary;
