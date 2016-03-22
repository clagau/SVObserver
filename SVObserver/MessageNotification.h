//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// PLEASE 
/// ENTER 
/// A DESCRIPTION
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <string>
//Moved to precompiled header: #include <concrt.h>
#include "SVStatusLibrary\MessageManager.h"
#include "SVOMFCLibrary\DisplayMessageBox.h"
#pragma endregion Includes


#pragma region Declarations

#pragma endregion Declarations



/// \brief MessageNotification via SVRC 
class MessageNotification
{
public:	
	
	MessageNotification();
	
	//************************************
	//! Set the memberVariables protected by critical section 
	//! \param 
	//! \param ErrorNumber 
	//! \param messagetext 
	//! \returns void
	//************************************
	void SetNotification(SvStl::NotificationEnum, int ErrorNumber, LPCTSTR messagetext  );
	//************************************
	//! Write Json Command called by SVVisionProcessorHelper Thread 
	//! \returns void
	//************************************
	void ProcessNotification();
	//************************************
	//! FireNotification via SVRC 
	//! \param  errorType
	//! \param  errornumber
	//! \param  errorText
	//! \returns INT_PTR
	//************************************
	static INT_PTR FireNotify(SvStl::NotificationEnum , int, LPCTSTR );

protected:
	bool m_IsProcessed;
	Concurrency::critical_section m_Protect;
	SvStl::NotificationEnum m_MessageType;
	int m_MessageNumber;
	SVString m_MessageText;

};



