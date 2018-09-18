//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRangeClassHelper
//* .File Name       : $Workfile:   RangeClassHelper.h  $
//* .Description     : Helper class for the SVRangeClass.  Implements the interface 
//*                    Contains variables for range values.
//*                    Allows the RangeClass to get, set, and check the values.
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   14 Jan 2015 16:41:56  $
//******************************************************************************

#pragma once

#pragma region Includes
#pragma endregion Includes

#pragma region Declarations
class SVObjectClass;
#pragma endregion Declarations

class RangeClassHelper
{
#pragma region Constructor
public:
	RangeClassHelper();
	virtual ~RangeClassHelper();
#pragma endregion Constructor

#pragma region Public Methods
	//************************************
	// Description:  Checks if the owner of ref is a SvDef::SVRangeObjectType
	// Parameter:  ref:  const reference to the object to check
	// Returns:  bool:  true if the object is owned by a range object else false
	//************************************
	static bool IsOwnedByRangeObject(const SVObjectClass& rObject);

	//************************************
	// Description:  Checks if the value can be set to the  object is allowed to be set via remote interface
	// if the object is a reference object it is only allowed when we are not in run mode and it is a valid reference
	// Parameter:  ref <in>: the object
	// Parameter:  value<in?>:   
	// Parameter:  bOnline <in>:  true if we are in run mode
	// Parameter:  status <out>:  the reason why it is not allowed
	// Returns:  bool:  if true the value can be set
	//************************************
	static bool IsAllowedToSet(const SVObjectClass& ObjectRef, const std::string& rValue, bool bOnline, HRESULT& hres);
#pragma endregion Public Methods

#pragma region Private Methods

#pragma endregion Private Methods

#pragma region Member variables
private:
#pragma endregion Member variables
};

