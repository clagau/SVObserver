//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : EnvironmentObject
//* .File Name       : $Workfile:   EnvironmentObject.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Mar 2014 15:10:22  $
//* ----------------------------------------------------------------------------
//* This class is used as the root child object Environment to 
//* store specific objects 
//******************************************************************************

#pragma once
#pragma region Includes
#include "SVObjectLibrary\SVObjectClass.h"
#include "BasicValueObjects.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace SVObserver
	{
		class EnvironmentObject : public SVObjectClass
		{
		#pragma region Declarations
			SV_DECLARE_CLASS( EnvironmentObject );
		#pragma endregion Declarations

		public:
		#pragma region Constructor
			/**********
			 The class constructor
			 \param ObjectName <in> the name of the object
			***********/
			EnvironmentObject( LPCSTR ObjectName );

			/**********
			 The class constructor
			 \param pOwner <in> a pointer to the parent object
			 \param StringResourceID <in> the resource string ID for the name
			***********/
			EnvironmentObject( SVObjectClass* pOwner = NULL, int StringResourceID = IDS_CLASSNAME_ENVIRONMENTOBJECT );

			/**********
			 The destructor
			***********/
			virtual ~EnvironmentObject();
		#pragma endregion Constructor

		public:
		#pragma region Public Methods
			/**********
			 The method finds the object corresponding to the name
			 \param rpObject <out> a reference pointer to the object being searched
			 \param rNameInfo <in> a reference to the name of the object to search for
			 \param Index <in> the index of the name to search for
			 \return SOK on success
			***********/
			virtual HRESULT GetChildObject( SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, long Index = 0 ) const;

			/**********
			 The method sets the value of a variable name if it does not exist it is created
			 \param Name <in> the variable name to set
			 \param Value <in> the value to set the variable to
			***********/
			void setVariable(LPCTSTR Name, LPCTSTR Value);

			/**********
			 The method gets the value of a variable name
			 \param Name <in> the variable name to set
			 \return The value 
			***********/
			CString getVariable(LPCTSTR Name) const;
		#pragma endregion Public Methods

		#pragma region Member Variables
			BasicValueObjects	m_EnvironmentValues;		//Container for environment values
		#pragma endregion Member Variables
		};
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\EnvironmentObject.h_v  $
 * 
 *    Rev 1.0   17 Mar 2014 15:10:22   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  869
 * SCR Title:  Add PPQ and Environment Variables to Object Manager and Update Pickers
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
