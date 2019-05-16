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
#pragma endregion Includes

namespace SvOr
{
	class SVOResourceApp : public CWinApp
	{
	public:
	#pragma region Constructor
		/**********
			The class constructor
		***********/
		SVOResourceApp();
		virtual ~SVOResourceApp();
	#pragma endregion Constructor

	public:
	#pragma region Public Methods
		DECLARE_MESSAGE_MAP()

		/**********
			The method is the entry point of the application
			\return true on success
		***********/
		virtual BOOL InitInstance() override;
	#pragma endregion Public Methods
	};
} //namespace SvOr
