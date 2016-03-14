//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOResourceApp
//* .File Name       : $Workfile:   SVOResource.h  $
//* .Description	 : The class is the main app of the dll
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Jul 2014 13:56:10  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "resource.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace SVOResource
	{
		class SVOResourceApp : public CWinApp
		{
		public:
		#pragma region Constructor
			/**********
			 The class constructor
			***********/
			SVOResourceApp();
		#pragma endregion Constructor

		public:
		#pragma region Public Methods
			DECLARE_MESSAGE_MAP()

			/**********
			 The method is the entry point of the application
			 \return true on success
			***********/
			virtual BOOL InitInstance();
		#pragma endregion Public Methods
		};
	} //namespace ObjectSelectorLibrary
} //namespace Seidenader

