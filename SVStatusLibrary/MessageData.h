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
#include "MessageTextEnum.h"
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

namespace SvStl
{
	struct MessageData
	{
	#pragma region Constructor
	public:
		MessageData() = default; 

		explicit MessageData(DWORD MessageCode, MessageTextEnum AdditionalTextId = SvStl::Tid_Empty, const SvDef::StringVector& rAdditionalTextList = SvDef::StringVector());

		virtual ~MessageData() = default;
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
		static std::string convertId2AdditionalText(MessageTextEnum id);

		static MessageTextEnum convertAdditionalText2Id(const std::string& text);
	#pragma endregion Public Methods

	#pragma region Member Variables
	public:
		mutable bool m_Logged{ false };		//Flag indicating if the message was written to the event log
		mutable bool m_Displayed{ false };	//Flag indicating if the message has been displayed
		DWORD m_MessageCode{ 0 };			//Number identifying the message
		MessageTextEnum m_AdditionalTextId{ SvStl::Tid_Empty }; //The main additional text id
		SvDef::StringVector m_AdditionalTextList; //String list to be inserted in the main additional text id
		SourceFileParams m_SourceFile;		//The source file standard parameters
		DWORD m_ProgramCode{ 0 };			//Program specific code.
		uint32_t m_ObjectId{ 0UL };			//! The object Id 
		std::time_t m_DateTime{ 0LL };		//The date and time when the data is set
	#pragma endregion Member Variables
	};

	#pragma region Declarations
		typedef std::vector< MessageData > Messages;
	#pragma endregion Declarations
} //namespace SvStl