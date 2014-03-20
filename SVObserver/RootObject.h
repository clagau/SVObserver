//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : RootObject
//* .File Name       : $Workfile:   RootObject.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Mar 2014 15:10:24  $
//* ----------------------------------------------------------------------------
//* This class is used to define the root node for the class object manager
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVObjectLibrary\SVObjectClass.h"
#include "EnvironmentObject.h"
#include "SVConfigurationObject.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace SVObserver
	{
		class RootObject : public SVObjectClass
		{
		#pragma region Declarations
			SV_DECLARE_CLASS( RootObject );
		#pragma endregion Declarations

		public:
		#pragma region Constructor
			/**********
			 The class constructor
			 \param ObjectName <in> the name of the object
			***********/
			RootObject( LPCSTR ObjectName );

			/**********
			 The class constructor
			 \param pOwner <in> a pointer to the parent object
			 \param StringResourceID <in> the resource string ID for the name
			***********/
			RootObject( SVObjectClass *pOwner = NULL, int StringResourceID = IDS_CLASSNAME_ROOTOBJECT );

				/**********
			 The destructor
			***********/
			virtual ~RootObject();
		#pragma endregion Constructor

		public:
		#pragma region Public Methods
			/**********
			 The method creates the configuration object
			 \return SOK on success
			***********/
			BOOL createConfigurationObject();

			/**********
			 The method finds the object corresponding to the name
			 \param rpObject <out> a reference pointer to the object being searched
			 \param rNameInfo <in> a reference to the name of the object to search for
			 \param Index <in> the index of the name to search for
			 \return SOK on success
			***********/
			virtual HRESULT GetChildObject( SVObjectClass*& p_rpObject, const SVObjectNameInfo& p_rNameInfo, long p_Index = 0 ) const;
		#pragma endregion Public Methods

		private:
		#pragma region Private Methods
			/**********
			 The method creates the environment object
			 \return SOK on success
			***********/
			BOOL createEnvironmentObject();

			/**********
			 The method destroys the configuration object
			 \return SOK on success
			***********/
			BOOL destroyConfigurationObject();
		#pragma endregion Private Methods

		private:
		#pragma region Member Variables
			EnvironmentObject		m_EnvironmentObject;				//The main node environment object
			SVConfigurationObject	*m_pConfigurationObject;			//The main node configuration pointer object
		#pragma endregion Member Variables
		};
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\RootObject.h_v  $
 * 
 *    Rev 1.0   17 Mar 2014 15:10:24   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  869
 * SCR Title:  Add PPQ and Environment Variables to Object Manager and Update Pickers
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
