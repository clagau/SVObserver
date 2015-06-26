//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : RootChildren
//* .File Name       : $Workfile:   RootChildren.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   16 Dec 2014 17:53:30  $
//* ----------------------------------------------------------------------------
//* This class is used as the root child object  to 
//* store specific objects 
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVUtilityLibrary\SVString.h"
#include "ObjectInterfaces\IRootObject.h"
#include "SVObjectLibrary\SVObjectClass.h"
#include "BasicValueObjects.h"
#pragma endregion Includes

#pragma region Declarations
//Environment variables
const TCHAR* const EnvironmentModelNumber		= _T("Environment.Model Number");
const TCHAR* const EnvironmentSerialNumber		= _T("Environment.Serial Number");
const TCHAR* const EnvironmentWinKey			= _T("Environment.Windows.Win Key");
const TCHAR* const EnvironmentImageUpdate		= _T("Environment.View.Image Display Update");
const TCHAR* const EnvironmentResultUpdate		= _T("Environment.View.Result Display Update");
const TCHAR* const EnvironmentModeIsRun			= _T("Environment.Mode.Is Run");
const TCHAR* const EnvironmentModeIsStop		= _T("Environment.Mode.Is Stop");
const TCHAR* const EnvironmentModeIsRegressionTest	= _T("Environment.Mode.Is Regression Test");
const TCHAR* const EnvironmentModeIsTest		= _T("Environment.Mode.Is Test");
const TCHAR* const EnvironmentModeIsEdit		= _T("Environment.Mode.Is Edit");
const TCHAR* const EnvironmentModeValue			= _T("Environment.Mode.Value");
#pragma endregion Declarations

class RootChildren
{
#pragma region Declarations
#pragma endregion Declarations

public:
#pragma region Constructor
	RootChildren();

	virtual ~RootChildren();
#pragma endregion Constructor

public:
#pragma region Public Methods
	//************************************
	//! The method searches the object with the corresponding name
	//! \param rpObject <out> a reference pointer to the object being searched
	//! \param rNameInfo <in> a reference to the name of the object to search for
	//! \param Index <in> the index of the name to search for
	//! \return SOK on success
	//************************************
	virtual HRESULT GetChildObject( SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, long Index = 0 ) const;

	//************************************
	//! The method gets the value object given a name
	//! \param DottedName <in> the dotted name of the object to get
	//! \returns The value object as a pointer
	//************************************
	BasicValueObjectPtr getValueObject( LPCTSTR DottedName ) const;

	//************************************
	//! The method gets the object list
	//! \param rObjectList <out> The returned list of objects.
	//! \param DottedBranch <in> the dotted branch name of object to return. Default = "", means all objects.
	//! \param AttributesAllowedFilter <in> These flags indicate which attributes must be set for the objects to be returned. For each flag, 1 means "must be set", and 0 mean "don't care".  Default = 0, means all objects.
	//************************************
	void getObjectList( BasicValueObjects::ValueVector& rObjectList, LPCTSTR DottedBranch,  UINT AttributesAllowedFilter ) const;

	//************************************
	//! The method gets the object list
	//! \param DottedName <in> the dotted name to get
	//! return SOK on success
	//************************************
	HRESULT deleteValue( LPCTSTR DottedName );

	//************************************
	//! The method gets the value of a name
	//! \param DottedName <in> the dotted name to get
	//! \param rValue <out> reference to the value of the object
	//! return SOK on success
	//************************************
	template <typename ELEMENT_TYPE>
	HRESULT getValue( LPCTSTR DottedName, ELEMENT_TYPE& rValue ) const;

	//************************************
	//! The method sets the value with the given name if does not exist it is created
	//! \param DottedName <in> the dotted name to set
	//! \param Value <in> the value to set the variable to
	//! \param pOwner <in> pointer to the owner of the object
	//! \param ObjectType <in> the object type
	//! \returns reference to the value object
	//************************************
	template <typename ELEMENT_TYPE>
	BasicValueObjectPtr setValue( LPCTSTR DottedName, const ELEMENT_TYPE Value, SVObjectClass* pOwner = nullptr, SVObjectTypeEnum ObjectType = SVBasicValueObjectType );
#pragma endregion Public Methods

#pragma region Member Variables
private:
	BasicValueObjects	m_RootChildrenValues;			//Container tree for all root children values
#pragma endregion Member Variables
};

#pragma region Inline
#include "RootChildren.inl"
#pragma endregion Inline

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\EnvironmentObject.h_v  $
 * 
 *    Rev 1.5   16 Dec 2014 17:53:30   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  933
 * SCR Title:  Add Filter Tab to Object Selector (SVO-377)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Added method getEnvironmentObjectNameList.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   09 Dec 2014 09:16:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  908
 * SCR Title:  Remove option for Operator Move (SVO 101)
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed EnvironmentModeIsEditMoveTool string.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   25 Aug 2014 02:40:18   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  934
 * SCR Title:  Add Remote Access to Environment.Mode Parameters
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Add Environment-Mode const string parameter
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   17 Jul 2014 17:39:40   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Removed namespaces and code review changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Jun 2014 09:07:08   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  900
 * SCR Title:  Separate View Image Update, View Result Update flags; remote access E55,E92
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Added the Environment names as TCHAR constants.
 * Removed getVariable and setVariable methods.
 * Added static methods getEnvironmentValue and setEnvironmentValue.
 * Added getValue and SetValue methods
 * Added include for .inl file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
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