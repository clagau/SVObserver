//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : BasicValueObject
//* .File Name       : $Workfile:   BasicValueObject.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   17 Jul 2014 17:39:38  $
//* ----------------------------------------------------------------------------
//* This class is used as a value object to store different types in the 
//* object manager 
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVObjectLibrary\SVObjectClass.h"
#include "SVOMFCLibrary\SVDeviceParams.h"
#pragma endregion Includes

class BasicValueObject : public SVObjectClass
{
public:
#pragma region Constructor
	/**********
		The class constructor
		\param ObjectName <in> the name of the object
	***********/
	BasicValueObject( LPCTSTR ObjectName, SVObjectClass* pOwner=nullptr, bool Node=false );

	/**********
		The class destructor
	***********/
	virtual ~BasicValueObject();
#pragma endregion Constructor

public:
#pragma region Public Methods
	/**********
		The method is the == operator
		\param rRhs <in> a reference to the value to compare
		\return True if the same
	***********/
	bool operator ==(const BasicValueObject& rRhs) const;

	/**********
		The method sets the value object
		\param rValue <in> a reference of the variant to set
		\return SOK on success
	***********/
	HRESULT setValue( const _variant_t& rValue );

	/**********
		The method sets the value object
		\param Value <in> a value of type string to set
		\return SOK on success
	***********/
	HRESULT setValue( const LPCTSTR Value );

	/**********
		The method sets the value object
		\param Value <in> a value of type BOOL to set
		\return SOK on success
	***********/
	HRESULT setValue( const BOOL Value );

	/**********
		The method sets the value object
		\param Value <in> a value of type long to set
		\return SOK on success
	***********/
	HRESULT setValue( const long Value );

	/**********
		The method sets the value object
		\param Value <in> a value of type __int64 to set
		\return SOK on success
	***********/
	HRESULT setValue( const __int64 Value );

	/**********
		The method sets the value object
		\param Value <in> a value of type double to set
		\return SOK on success
	***********/
	HRESULT setValue( const double Value );

	/**********
		The method gets the value object
		\param rValue <out> a reference to the variant to get
		\return SOK on success
	***********/
	HRESULT getValue( _variant_t& rValue ) const;

	/**********
		The method gets the value object
		\param Value <out> a value of type string to get
		\return SOK on success
	***********/
	HRESULT getValue( SVString& rValue ) const;

	/**********
		The method gets the value object
		\param rValue <out> a reference type bool to get
		\return SOK on success
	***********/
	HRESULT getValue( BOOL& rValue ) const;

	/**********
		The method gets the value object
		\param rValue <out> a reference type long to get
		\return SOK on success
	***********/
	HRESULT getValue( long& rValue ) const;

	/**********
		The method gets the value object
		\param rValue <out> a reference type __int64 to get
		\return SOK on success
	***********/
	HRESULT getValue( __int64& rValue ) const;

	/**********
		The method gets the value object
		\param rValue <out> a reference type double to get
		\return SOK on success
	***********/
	HRESULT getValue( double& rValue ) const;

	/**********
		The method updates the corresponding device parameter
		\param pDeviceParam <in> a reference to the parameter
		\return SOK on success
	***********/
	HRESULT updateDeviceParameter(SVDeviceParam* pDeviceParam);

	/**********
		The method checks if object is a node
		\return True if object is a node
		***********/
	inline bool isNode() const;
#pragma endregion Public Methods

private:
#pragma region Private Methods
	/**********
		The method creates the value object
		\return True on success
	***********/
	virtual BOOL Create( SVObjectClass* pOwner );

	/**********
		The method destroys the value object
		\return True on success
	***********/
	virtual BOOL Destroy();

	/**********
		The method locks the value object for writing
		\return True on success
	***********/
	BOOL Lock();

	/**********
		The method unlocks the value object
		\return True on success
	***********/
	BOOL Unlock();

	/**********
		The method refreshes the parent object
		\return True on success
	***********/
	BOOL RefreshOwner() const;

	/**********
		The method converts an array to variant data
		\return S_OK on success
	***********/
	HRESULT ConvertArrayToVariant( _variant_t& rValue ) const;
#pragma endregion Private Methods

private:
#pragma region Member Variables
	_variant_t			m_Value;			//The value object container
	CRITICAL_SECTION	m_CriticalSection;	//The critical section object
	bool				m_Created;			//Object is created
	bool				m_Node;				//Object is only a node in the tree structure
#pragma endregion Member Variables
};

#pragma region Inline
#include "BasicValueObject.inl"
#pragma endregion Inline

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\BasicValueObject.h_v  $
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
 *    Rev 1.1   02 Jun 2014 08:41:20   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  900
 * SCR Title:  Separate View Image Update, View Result Update flags; remote access E55,E92
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Added pOwner and Node to the constructor.
 * Changed parameter for getValue from CString to SVString.
 * Added isNode method.
 * Changed Create method to take pOwner parameter
 * Added method ConvertArrayToVariant
 * Changed m_Create from BOOL to bool
 * Added member bool m_Node
 * Added include for .inl file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Mar 2014 15:10:20   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  869
 * SCR Title:  Add PPQ and Environment Variables to Object Manager and Update Pickers
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/