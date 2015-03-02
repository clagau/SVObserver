///******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// This file defines function to get access to data from SVObserverApp
//******************************************************************************

#pragma once

#pragma region Includes
#pragma endregion Includes


namespace Seidenader
{
	namespace ObjectInterfaces
	{	
		//************************************
		/// Return if the application has unrestricted file access.
		/// \returns bool
		//************************************
		bool isUnrestrictedFileAccess();
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;