//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : EnvironmentObject
//* .File Name       : $Workfile:   EnvironmentObject.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   16 Dec 2014 17:53:30  $
//* ----------------------------------------------------------------------------
//* This class is used as the root child object Environment to 
//* store specific objects 
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVUtilityLibrary\SVString.h"
#include "ObjectInterfaces\IEnvironmentObject.h"
#include "SVObjectLibrary\SVObjectClass.h"
#include "BasicValueObjects.h"
#pragma endregion Includes

#pragma region Declarations
//Environment variables
const TCHAR EnvironmentModelNumber[]	= _T("Model Number");
const TCHAR EnvironmentSerialNumber[]	= _T("Serial Number");
const TCHAR EnvironmentWinKey[]			= _T("Windows.Win Key");
const TCHAR EnvironmentImageUpdate[]	= _T("View.Image Display Update");
const TCHAR EnvironmentResultUpdate[]	= _T("View.Result Display Update");
const TCHAR EnvironmentModeIsRun[]		= _T("Mode.Is Run");
const TCHAR EnvironmentModeIsStop[]		= _T("Mode.Is Stop");
const TCHAR EnvironmentModeIsRegressionTest[]		= _T("Mode.Is Regression Test");
const TCHAR EnvironmentModeIsTest[]		= _T("Mode.Is Test");
const TCHAR EnvironmentModeIsEdit[]		= _T("Mode.Is Edit");
const TCHAR EnvironmentModeValue[]		= _T("Mode.Value");
#pragma endregion Declarations

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
	EnvironmentObject( SVObjectClass* pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_ENVIRONMENTOBJECT );

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
		The static method gets an environment value if available
		\param Name <in> the name of the object to get
		\param rValue <out> reference to the value of the object
	***********/
	template <typename ELEMENT_TYPE>
	static void getEnvironmentValue( LPCSTR Name, ELEMENT_TYPE& rValue );

	/**********
		The static method sets an environment value
		\param Name <in> the name of the object to set
		\param rValue <out> reference to the value of the object
		\return pointer to the value object
	***********/
	template <typename ELEMENT_TYPE>
	static BasicValueObject* setEnvironmentValue( LPCSTR Name, const ELEMENT_TYPE& rValue );

	/**********
		The method gets an environment object name list.
		\param rObjectNameList <out> The returned list of names.
		\param rPath <in> the path of object to return. Default = "", means all objects.
		\param AttributesAllowedFilter <in> the attributes of the objects must be set to return. Default = 0, means all objects.
	***********/
	static void getEnvironmentObjectNameList(SVStringArray& rObjectNameList, const SVString& rPath = _T(""), UINT AttributesAllowedFilter = 0);

	/**********
		The method gets an environment object name list.
		\param rObjectNameList <out> The returned list of names.
		\param rPath <in> the path of object to return. Default = "", means all objects.
		\param AttributesAllowedFilter <in> the attributes of the objects must be set to return. Default = 0, means all objects.
	***********/
	void getObjectNameList(SVStringArray& rObjectNameList, const SVString& rPath = _T(""), UINT AttributesAllowedFilter = 0) const;

	//************************************
	/// The method fill up a list with filtered environment objects.
	/// \param rObjectList [in,out] The list which will be filled.
	/// \param rPath [in] The path of the requested objects. Default = "", means all objects.
	/// \param AttributesAllowedFilter [in] the attributes of the objects must be set to added. Default = 0, means all objects.
	//************************************
	static void fillEnvironmentObjectList(BasicValueObjects::ValueList& rObjectList, const SVString& rPath = _T(""), UINT AttributesAllowedFilter = 0);
	//************************************
	/// The method fill up a list with filtered environment objects.
	/// \param rObjectList [in,out] The list which will be filled.
	/// \param rPath [in] The path of the requested objects. Default = "", means all objects.
	/// \param AttributesAllowedFilter [in] the attributes of the objects must be set to added. Default = 0, means all objects.
	//************************************
	void fillObjectList(BasicValueObjects::ValueList& rObjectList, const SVString& rPath = _T(""), UINT AttributesAllowedFilter = 0) const;

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
		\return pointer to the value object
	***********/
	template <typename ELEMENT_TYPE>
	BasicValueObject* setValue( LPCTSTR Name, const ELEMENT_TYPE Value );
#pragma endregion Public Methods

#pragma region Member Variables
private:
	BasicValueObjects m_EnvironmentValues; //Container for environment values

	//This parameter a temporary parameter to speed up the method fillObjectList. 
	//They are not a state of the object only a help for the method. To be able to set the method const, this parameter are mutable.
	mutable BasicValueObjects::ValueList m_lastListOfFillObjectList;  //<This list is saved for the method fillObjectList, to reuse it if the same filter (m_lastAttributesAllowedFilterOfFillObjectList) and path (m_lastPathOfFillObjectList) is used. 
	mutable SVString m_lastPathOfFillObjectList; //<This path was used in the last run of the method fillObjectList.
	mutable UINT m_lastAttributesAllowedFilterOfFillObjectList; //<This filter flag was used in the last run of the method fillObjectList.
#pragma endregion Member Variables
};

#pragma region Inline
#include "EnvironmentObject.inl"
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