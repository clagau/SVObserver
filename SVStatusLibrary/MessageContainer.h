//*****************************************************************************
/// \copyright (c) 2015,2015 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
//*****************************************************************************
/// This is the message handler to generate messages for logging and displaying
//******************************************************************************

#pragma once

#pragma region Includes
#include "MessageData.h"
#include "Definitions/StringTypeDef.h"
#include "SVProtoBuf/BasicStructure.h"
#pragma endregion Includes

namespace SvStl
{
	class MessageContainer;

	typedef std::function<INT_PTR ( HWND, LPCTSTR, LPCTSTR, UINT )> ShowDisplayFunctor;
	typedef std::function<void (const MessageContainer&, int)> MessageNotifyFunctor;

	class MessageContainer : public std::exception
	{
	#pragma region Constructor
	public:
		MessageContainer() = default;

		MessageContainer( long MessageCode, MessageTextEnum AdditionalTextId,  SourceFileParams SourceFile, DWORD ProgramCode = 0, uint32_t objectId = 0 );

		MessageContainer( long MessageCode, MessageTextEnum AdditionalTextId, const SvDef::StringVector& rAdditionalTextList, SourceFileParams SourceFile, DWORD ProgramCode = 0, uint32_t objectId = 0 );

		explicit MessageContainer(const MessageData& rMessage, uint32_t objectId = 0, bool clearData = true);

		virtual ~MessageContainer() = default;
	#pragma endregion Constructor
	
	#pragma region Public Methods
	public:

		inline const MessageData& getMessage() const { return m_Message; };
		inline const Messages& getAdditionalMessages() const { return m_AdditionalMessages; };
		//! The what method has to return a value of type char*
		virtual const char* what() const override;

		const std::string& What() const { return m_What; };

		void setMessage( long MessageCode, MessageTextEnum AdditionalTextId, SourceFileParams SourceFile, DWORD ProgramCode=0, uint32_t ojectId=0 );
		void setMessage( long MessageCode, MessageTextEnum AdditionalTextId, const SvDef::StringVector& rAdditionalTextList, SourceFileParams SourceFile, DWORD ProgramCode=0, uint32_t objectId=0 );
		void setMessage( const MessageData& rMessage, uint32_t objectId=0, bool clearData = true );
		void addMessage( long MessageCode, MessageTextEnum AdditionalTextId, SvDef::StringVector AdditionalTextList, SourceFileParams SourceFile, DWORD dwProgramCode = 0 );
		void addMessage( const MessageData& rMessage, bool replaceMainMessage = true );

		void clearMessage();
		void clearWhat() { m_What.clear(); }
		void logMessage() const;

		void setMessageDisplayed() { m_Message.m_Displayed = true; };

		UINT getSeverity() const;
		UINT getSeverityIcon() const;

		std::string Format( std::string& rMessage ) const;

		uint32_t getObjectId( ) const { return m_Message.m_ObjectId; };

		void setObjectId(uint32_t objectId) { m_Message.m_ObjectId = objectId; };

		void setFunctorObjects( ShowDisplayFunctor*& rpShowDisplay, MessageNotifyFunctor*& rpNotify );
	#pragma endregion Public Methods

	#pragma region Private Methods
	private:

		UINT getFacility() const;
		std::string getFacilityName() const;
		UINT getEventID() const;
		WORD getCategory() const;
		std::string getCategoryName() const;
		void setSubstituteStrings( SvDef::StringVector& rSubstituteStrings ) const;
		HRESULT setMessageDll();
	#pragma endregion Private Methods

	#pragma region Member variables
	private:
		MessageData m_Message;				//! The main message
		Messages m_AdditionalMessages;		//! List of additional messages
		std::string m_What;					//! The what formatted message
		static HINSTANCE m_MessageDll;		//! The message dll instance as a static variable
	#pragma endregion Member variables
	};

	#pragma region Declarations
		typedef std::vector< MessageContainer > MessageContainerVector;
		typedef std::back_insert_iterator<MessageContainerVector>  MessageContainerInserter;
	#pragma endregion Declarations
} //namespace SvStl