//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// The message notification used to send via SVRC
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <concrt.h>
#include "MessageManager.h"
#pragma endregion Includes

namespace SvStl
{
	class MessageNotification
	{
	public:	
	
		MessageNotification();
	
		//************************************
		//! Set the memberVariables protected by critical section 
		//! \param notification type
		//! \param message number 
		//! \param message text 
		//************************************
		void SetNotification( SvStl::NotificationEnum Type, int MessageNumber, LPCTSTR MessageText );

		void setProcessed( bool Processed ) { m_IsProcessed = Processed; };

		bool isProcessed() const { return m_IsProcessed; };
		Concurrency::critical_section& getLock() { return m_Lock; };
		const SvStl::NotificationEnum& getType() const { return m_Type; };
		const int& getMessageNumber() const { return m_MessageNumber; };
		LPCTSTR getMessageText() const { return m_MessageText.c_str(); };

	protected:
		bool m_IsProcessed;
		Concurrency::critical_section m_Lock;
		SvStl::NotificationEnum m_Type;
		int m_MessageNumber;
		std::string m_MessageText;
	};
} //namespace SvStl