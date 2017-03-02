//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// This is the message handler to generate messages for logging and displaying
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <iterator>
//Moved to precompiled header: #include <boost\function.hpp>
#include "MessageData.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVUtilityLibrary/SVGUID.h"
#pragma endregion Includes

namespace Seidenader { namespace SVStatusLibrary
{
	typedef boost::function<INT_PTR ( HWND, LPCTSTR, LPCTSTR, UINT )> ShowDisplayFunctor;
	typedef boost::function<HRESULT ( int, int, LPCTSTR )> NotifyFunctor;

	class MessageContainer : public std::exception
	{
	#pragma region Constructor
	public:
		MessageContainer();

		//************************************
		//! This is the copy constructor
		//! \param rRhs <in> reference to the message handler to copy
		//************************************
		MessageContainer( const MessageContainer& rRhs );

		//************************************
		//! This is the constructor to set the data on construction
		//! \param MessageCode <in> the number identifying the message
		//! \param AdditionalTextId <in> the Id for the additional text to the code (without AdditionalTextList)
		//! \param SourceFile <in> the source file standard parameters in which the code set
		//! \param ProgramCode <in> the unique program ID for the code
		//! \param rObjectId <in> reference to the unique object ID of the message owner
		//************************************
		MessageContainer( long MessageCode, SvOi::MessageTextEnum AdditionalTextId,  SourceFileParams SourceFile, DWORD ProgramCode = 0, const GUID& rObjectId = SV_GUID_NULL );

		//************************************
		//! This is the constructor to set the data on construction
		//! \param MessageCode <in> the number identifying the message
		//! \param AdditionalTextId <in> the Id for the additional text to the code
		//! \param rAdditionalTextList <in> reference t list of strings for the additional text to the code
		//! \param SourceFile <in> the source file standard parameters in which the code set
		//! \param ProgramCode <in> the unique program ID for the code
		//! \param rObjectId <in> reference to the unique object ID of the message owner
		//************************************
		MessageContainer( long MessageCode, SvOi::MessageTextEnum AdditionalTextId, const SVStringVector& rAdditionalTextList, SourceFileParams SourceFile, DWORD ProgramCode = 0, const GUID& rObjectId = SV_GUID_NULL );

		//************************************
		//! This is the assignment operator
		//! \param rRhs <in> reference to a message handler with which to set the data
		//************************************
		const MessageContainer& operator=(const MessageContainer& rRhs);

		virtual ~MessageContainer();
	#pragma endregion Constructor
	
	#pragma region Public Methods
	public:

		inline const MessageData& getMessage() const { return m_Message; };
		inline const Messages& getAdditionalMessages() const { return m_AdditionalMessages; };
		//! The what method has to return a value of type char*
		virtual const char* what() const override;

		//************************************
		//! This is the method to return the What message
		//! \returns const reference to the what message as a SVString
		//************************************
		const SVString What() const { return m_What; };

		//************************************
		//! This is the method to set the data this will delete the m_AdditonalMessages list
		//! \param MessageCode <in> the number identifying the message
		//! \param AdditionalTextId <in> the Id for the additional text to the code (without AdditionalTextList)
		//! \param SourceFile <in> the source file standard parameters in which the code set
		//! \param ProgramCode <in> the unique program ID for the code
		//! \param rObjectId <in> reference to the unique object ID of the message owner
		//************************************
		void setMessage( long MessageCode, SvOi::MessageTextEnum AdditionalTextId, SourceFileParams SourceFile, DWORD ProgramCode=0, const GUID& rObjectId=SV_GUID_NULL );

		//************************************
		//! This is the method to set the data this will delete the m_AdditonalMessages list
		//! \param MessageCode <in> the number identifying the message
		//! \param AdditionalTextId <in> the Id for the additional text to the code
		//! \param rAdditionalTextList <in> Reference to list of strings for the additional text to the code
		//! \param SourceFile <in> the source file standard parameters in which the code set
		//! \param ProgramCode <in> the unique program ID for the code
		//! \param rObjectId <in> reference to the unique object ID of the message owner
		//************************************
		void setMessage( long MessageCode, SvOi::MessageTextEnum AdditionalTextId, const SVStringVector& rAdditionalTextList, SourceFileParams SourceFile, DWORD ProgramCode=0, const GUID& rObjectId=SV_GUID_NULL );

		//************************************
		//! This is the method to set the data this will delete the m_AdditonalMessages list
		//! \param rMessage <in> reference to the message data structure
		//! \param rObjectId <in> reference to the unique object ID of the message owner
		//! \param clearData <in> flag to determine if data should be cleared prior to setting the data
		//************************************
		void setMessage( const MessageData& rMessage, const GUID& rObjectId=SV_GUID_NULL, bool clearData = true );

		//************************************
		//! This method first adds the current main message to the m_AdditionalMessage list 
		//! and then sets the main message with the parameters
		//! \param MessageCode <in> the number identifying the message
		//! \param AdditionalTextId <in> the Id for the additional text to the code
		//! \param AdditionalTextList <in> List of strings for the additional text to the code
		//! \param SourceFile <in> the source file standard parameters in which the code set
		//! \param ProgramCode <in> the unique program ID for the code
		//************************************
		void addMessage( long MessageCode, SvOi::MessageTextEnum AdditionalTextId, SVStringVector AdditionalTextList, SourceFileParams SourceFile, DWORD dwProgramCode = 0 );

		//************************************
		//! This method is the same as the addMessage above with a message structure when the flag is true which is the default
		//! However if the flag is false then the message parameter is added directly to the m_AdditionalMessages list
		//! \param rMessage <in> reference to the message data structure
		//! \param replaceMainMessage <in> flag to determine if main message is replaced
		//************************************
		void addMessage( const MessageData& rMessage, bool replaceMainMessage = true );

		//************************************
		//! This is the method to clear the data
		//************************************
		void clearMessage();

		//************************************
		//! This is the method to log the message into the event log
		//************************************
		void logMessage() const;

		//************************************
		//! This is the method to set that the message has been displayed
		//************************************
		inline void setMessageDisplayed() { m_Message.m_Displayed = true; };

		//************************************
		//! This is the method to get the corresponding severity icon
		//! \returns the icon ID corresponding to the severity
		//************************************
		UINT getSeverityIcon() const;

		//************************************
		//! This is the method to format the message for displaying
		//! \param rMessage <out> the reference for the formatted message
		//! \returns the detailed information to the message code
		//************************************
		SVString Format( SVString& rMessage ) const;

		//************************************
		//! This is the method to get the object id of the message owner
		//! \returns const reference to the object id
		//************************************
		const SVGUID& getObjectId( ) const { return m_ObjectId; };

		/// This is the method to set the object id of the message owner
		/// \param rGuid [in] reference to the object id
		void setObjectId( const SVGUID& rGuid ) { m_ObjectId = rGuid; };

		//************************************
		//! Sets the functor objects for display and notify
		//! \returns const reference to the object id
		//************************************
		void setFunctorObjects( ShowDisplayFunctor*& rpShowDisplay, NotifyFunctor*& rpNotify );
	#pragma endregion Public Methods

	#pragma region Private Methods
	private:
		//************************************
		//! This gets the filtered severity of the message code
		//! \returns the severity SEV_SUCCESS, SEV_INFORMATIONAL, SEV_WARNING or SEV_FATAL
		//************************************
		UINT getSeverity() const;

		//************************************
		//! This gets the filtered facility of the message code
		//! \returns the 12 bit facility code
		//************************************
		UINT getFacility() const;

		//************************************
		//! This gets the facility name of the message code
		//! \returns the name of the facility
		//************************************
		SVString getFacilityName() const;

		//************************************
		//! This gets the filtered EventID of the message code
		//! \returns the 16 bit EventID
		//************************************
		UINT getEventID() const;

		//************************************
		//! This gets the filtered category of the message code
		//! \returns the category
		//************************************
		WORD getCategory() const;

		//************************************
		//! This gets the category name of the message code
		//! \returns the name of the category
		//************************************
		SVString getCategoryName() const;

		//************************************
		//! The method sets the substitute strings for the event log
		//! \param rSubstituteStrings <out> the reference for the substitute strings
		//************************************
		void setSubstituteStrings( SVStringVector& rSubstituteStrings ) const;

		//************************************
		//! Method to set the message dll instance
		//! \returns S_OK on success
		//************************************
		HRESULT setMessageDll();
	#pragma endregion Private Methods

	#pragma region Member variables
	private:
		MessageData m_Message;				//! The main message
		Messages m_AdditionalMessages;		//! List of additional messages
		SVGUID m_ObjectId;					//! The object GUID 
		SVString m_What;					//! The what formatted message
		static HINSTANCE m_MessageDll;		//! The message dll instance as a static variable
	#pragma endregion Member variables
	};

	#pragma region Declarations
		typedef std::vector< MessageContainer > MessageContainerVector;
		typedef std::back_insert_iterator<MessageContainerVector>  MessageContainerInserter;
	#pragma endregion Declarations
} /* namespace SVStatusLibrary */ } /* namespace Seidenader */

namespace SvStl = Seidenader::SVStatusLibrary;
