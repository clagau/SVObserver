//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// This file is used to declare message managers which are dependent on resources
//******************************************************************************
#pragma once

#pragma region Includes
#include "MessageManager.h"
#include "SVOMFCLibrary/DisplayMessageBox.h"
#pragma endregion Includes

namespace Seidenader { namespace SVStatusLibrary
{
#pragma region Declarations
	//This declares message manager standard display which uses MessageHandler, MessageData and DisplayMessageBox::showDialog as the template parameters
	typedef MessageManager<MessageHandler, MessageData, &SvOml::DisplayMessageBox::showDialog> MessageMgrStdDisplay;
#pragma endregion Declarations
} /* namespace SVStatusLibrary */ } /* namespace Seidenader */

namespace SvStl = Seidenader::SVStatusLibrary;


