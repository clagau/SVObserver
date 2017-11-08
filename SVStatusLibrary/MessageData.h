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
#include "SourceFileParams.h"
#include "SVStatusLibrary/MessageTextEnum.h"
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

namespace SvStl
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
		//! \param rAdditionalTextList <in> Reference to list of strings for the additional text
		//************************************
		MessageData( DWORD MessageCode, MessageTextEnum AdditionalTextId = SvStl::Tid_Empty, const SvDef::StringVector& rAdditionalTextList = SvDef::StringVector() );

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
		std::string getAdditionalText() const;

		//************************************
	/// Convert an enum id to a string for setting it to the addtionalTextList.
	/// \param id [in] the id
	/// \returns SVString
	//************************************
		static std::string convertId2AddtionalText(MessageTextEnum id);
	#pragma endregion Public Methods

	#pragma region Member Variables
	public:
		mutable bool m_Logged;				//Flag indicating if the message was written to the event log
		mutable bool m_Displayed;			//Flag indicating if the message has been displayed
		DWORD m_MessageCode;				//Number identifying the message
		MessageTextEnum m_AdditionalTextId; //The main additional text id
		SvDef::StringVector m_AdditionalTextList; //String list to be inserted in the main additional text id
		SourceFileParams m_SourceFile;		//The source file standard parameters
		DWORD m_ProgramCode;				//Program specific code.
		std::time_t m_DateTime;				//The date and time when the data is set
	#pragma endregion Member Variables
	};

	#pragma region Declarations
		typedef std::vector< MessageData > Messages;
	#pragma endregion Declarations
} //namespace SvStl