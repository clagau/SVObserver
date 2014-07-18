//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : BasicValueObjects
//* .File Name       : $Workfile:   BasicValueObjects.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   17 Jul 2014 17:39:38  $
//* ----------------------------------------------------------------------------
//* This class is used as the container to store BasicValueObject objects
//******************************************************************************

#pragma once

#pragma region Includes
#include "BasicValueObject.h"
#pragma endregion Includes

class BasicValueObjects
{
public:
#pragma region Declarations
	typedef std::vector<BasicValueObject*> ValueList;
#pragma endregion Declarations

#pragma region Constructor
	/**********
		The class constructor
	***********/
	BasicValueObjects();

	/**********
		The class destructor
	***********/
	virtual ~BasicValueObjects();
#pragma endregion Constructor

public:
#pragma region Public Methods
	/**********
		The method gets the value of a name
		\param Name <in> the name of the object to get
		\return The value object as a pointer
	***********/
	BasicValueObject* getValueObject(LPCTSTR Name) const;

	/**********
		The method gets the value of a name
		\param Name <in> the name of the object to get
		\param rValue <out> reference to the value of the object
		\return SOK on success
	***********/
	template <typename ELEMENT_TYPE>
	HRESULT getValue( LPCSTR Name, ELEMENT_TYPE& rValue ) const;

	/**********
		The method sets the value of a variable name if does not exist it is created
		\param Name <in> the variable name to set
		\param Value <in> the value to set the variable to
		\param pOwner <in> pointer to the owner of the object
		\return reference to the value object
	***********/
	template <typename ELEMENT_TYPE>
	BasicValueObject* setValueObject(LPCTSTR Name, const ELEMENT_TYPE Value, SVObjectClass* pOwner = nullptr);

	/**********
		The method gets the value list
		\return A reference to the value list
	***********/
	inline const ValueList& getValueList() const;
#pragma endregion Public Methods

private:
#pragma region Private Methods
	//The method creates the required variable objects
	BOOL Create();

	//The method destroys the stored variable objects
	BOOL Destroy();
#pragma endregion Private Methods

private:
#pragma region Member Variables
	//The list of value objects to be exposed
	ValueList m_Values;
#pragma endregion Member Variables
};

#pragma region Inline
#include "BasicValueObjects.inl"
#pragma endregion Inline

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\BasicValueObjects.h_v  $
 * 
 *    Rev 1.2   17 Jul 2014 17:39:38   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Removed namespaces and code review changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Jun 2014 08:55:50   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  900
 * SCR Title:  Separate View Image Update, View Result Update flags; remote access E55,E92
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Added method getValue.
 * Changed return type, parameter type for setValueObject.
 * setValueObject generates required nodes automatically.
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