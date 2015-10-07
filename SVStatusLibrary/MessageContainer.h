//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// This is the message handler to generate messages for logging and displaying
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVUtilityLibrary/SVString.h"
#include "MessageData.h"
#pragma endregion Includes

namespace Seidenader { namespace SVStatusLibrary
{
#pragma region Declarations
		//This is a define because it uses the standard macros
		#define StdMessageParams _T(__DATE__), _T(__TIME__), _T(__FILE__), __LINE__, _T(__TIMESTAMP__)
#pragma endregion Declarations

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
		//! \param AdditionalText <in> the additional text to the code
		//! \param CompileDate <in> the file compile date
		//! \param CompileTime <in> the file compile time
		//! \param SourceFile <in> the source file in which code set
		//! \param SourceLine <in> the source line number in which code set
		//! \param SourceDateTime <in> the source file date time in which code set
		//! \param ProgramCode <in> the unique program ID for the code
		//! \param OSErrorCode <in> the operating system error code
		//! \param User <in> the respective user for security messages
		//************************************
		MessageContainer( long MessageCode, LPCTSTR AdditionalText, LPCTSTR CompileDate, LPCTSTR CompileTime, 
						LPCTSTR SourceFile, long SourceLine, LPCTSTR SourceDateTime, 
						DWORD ProgramCode = 0, DWORD OSErrorCode = 0, LPCTSTR User = nullptr );

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
		inline virtual const TCHAR* what() const { return m_What.c_str(); };

		//************************************
		//! This is the method to set the data this will delete the m_AdditonalMessages list
		//! \param MessageCode <in> the number identifying the message
		//! \param AdditionalText <in> the additional text to the code
		//! \param CompileDate <in> the file compile date
		//! \param CompileTime <in> the file compile time
		//! \param SourceFile <in> the source file in which code set
		//! \param SourceLine <in> the source line number in which code set
		//! \param SourceDateTime <in> the source file date time in which code set
		//! \param ProgramCode <in> the unique program ID for the code
		//! \param OSErrorCode <in> the operating system error code
		//! \param User <in> the respective user for security messages
		//************************************
		void setMessage( long MessageCode, LPCTSTR AdditionalText, LPCTSTR CompileDate, LPCTSTR CompileTime, 
			LPCTSTR SourceFile, long SourceLine, LPCTSTR SourceDateTime, 
			DWORD dwProgramCode=0, DWORD dwOsErrorCode=0, LPCTSTR User=nullptr);

		//************************************
		//! This is the method to set the data this will delete the m_AdditonalMessages list
		//! \param rMessage <in> reference to the message data structure
		//! \param clearData <in> flag to determine if data should be cleared prior to setting the data
		//************************************
		void setMessage( const MessageData& rMessage, bool clearData = true );

		//************************************
		//! This method first adds the current main message to the m_AdditionalMessage list 
		//! and then sets the main message with the parameters
		//! \param MessageCode <in> the number identifying the message
		//! \param AdditionalText <in> the additional text to the code
		//! \param CompileDate <in> the file compile date
		//! \param CompileTime <in> the file compile time
		//! \param SourceFile <in> the source file in which code set
		//! \param SourceLine <in> the source line number in which code set
		//! \param SourceDateTime <in> the source file date time in which code set
		//! \param ProgramCode <in> the unique program ID for the code
		//! \param OSErrorCode <in> the operating system error code
		//! \param User <in> the respective user for security messages
		//************************************
		void addMessage( long MessageCode, LPCTSTR AdditionalText, LPCTSTR CompileDate, LPCTSTR CompileTime, 
			LPCTSTR SourceFile, long SourceLine, LPCTSTR SourceDateTime, 
			DWORD dwProgramCode=0, DWORD dwOsErrorCode=0, LPCTSTR User=nullptr);

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
		void logMessage();

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
		void setSubstituteStrings( SVStringArray& rSubstituteStrings ) const;
#pragma endregion Private Methods

#pragma region Member variables
	private:
		MessageData m_Message;				//The main message
		Messages m_AdditionalMessages;		//List of additional messages
		SVString m_What;					//The what formatted message
#pragma endregion Member variables
	};
} /* namespace SVStatusLibrary */ } /* namespace Seidenader */

namespace SvStl = Seidenader::SVStatusLibrary;
