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
#include "InspectionEngine/SVTaskObjectList.h"
#include "Definitions/RangeEnum.h"
#include "SVStatusLibrary/MessageTextEnum.h"
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

#pragma region Declarations
class SVRangeClass;
#pragma endregion Declarations

class RangeClassHelper
{
#pragma region Constructor
public:
	RangeClassHelper(SVRangeClass* pRange = nullptr);
	virtual ~RangeClassHelper();
#pragma endregion Constructor

#pragma region Public Methods
	//************************************
	// Description:  sets the inspection data
	// Returns:  HRESULT:  S_OK if successful
	//************************************
	HRESULT SetInspectionData();

	//************************************
	// Description:  Get internal value Data From m_pRange 
	// Returns:  HRESULT:   S_OK if successful
	//************************************
	HRESULT GetAllInspectionData();

	//************************************
	// Description:  Get internal value Data From m_pRange for ra 
	// Parameter:  ra <in>:  what  limit 
	// Returns:  HRESULT  S_OK if successful
	//************************************
	HRESULT GetInspectionData(RangeEnum::ERange ra);

	//************************************
	/// Description: Check if data are plausible
	/// \param messageId [out] Id to message text
	/// \param messageList [out] additionalText to message text
	/// \returns HRESULT S_OK if successful
	//************************************
	HRESULT CheckInternalData(SvStl::MessageTextEnum &messageId, SvDef::StringVector &messageList) const;

	//************************************
	// Description:  Set the data
	// Parameter:  er <in>:  which range variable to set
	// Parameter:  lp <in>:  string what to set
	// Returns:  void, but throw an Exception from type ExceptionMgr1 if set failed
	//************************************
	 void SetInternalData(SvStl::MessageTextEnum er, LPCTSTR lp);

	//************************************
	// Description:  set m_pRange as TaskObject
	// Returns:  void
	//************************************
	void SetRangeTaskObject();

	//************************************
	// Description: Get the range value string from the RangeObject could be a numeric value or an indirect object
	// Parameter:  ra <in>:  the enum value of the object
	// Returns:  std::string   with the value 
	//************************************
	std::string GetStringFromRange( RangeEnum::ERange ra ) const;

	//************************************
	// Description:  get the Owner name of the Range Object 
	// Returns:  LPCTSTR: the Owner name of tghe range object or Null.
	//************************************
	LPCTSTR GetOwnerName() const;

	//************************************
	// Description:  Change the indirect names, if necessary
	// Parameter:  oldPefix <in>:  old tool name
	// Parameter:  newPrefix <in>:  new tool name
	// Returns:  bool:  true if an indirect name was changed
	//************************************
	bool RenameIndirectValues(LPCTSTR oldPefix, LPCTSTR newPrefix);

	//************************************
	/// \returns std::string the Fail High value as string. 
	/// If it is a indirect value it return the name of the linked value.
	//************************************
	std::string GetFailHighString();
		
	//************************************
	/// \returns std::string the Warn High value as string. 
	/// If it is a indirect value it return the name of the linked value.
	//************************************
	std::string GetWarnHighString();

	//************************************
	/// \returns std::string the Fail Low value as string. 
	/// If it is a indirect value it return the name of the linked value.
	//************************************
	std::string GetFailLowString();

	//************************************
	/// \returns std::string the Warn Low value as string. 
	/// If it is a indirect value it return the name of the linked value.
	//************************************
	std::string GetWarnLowString();

	//************************************
	/// Fill the Object selector with filter and object, 
	/// so that the caller can open the selector with showDialog.
	/// \returns bool true if fill was succeeded.
	//************************************
	bool FillObjectSelector();


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
	//************************************
	/// If indirect value is not empty it will return this, else it will return the direct value
	/// \param indirectString [in] The indirect string
	/// \param directValue [in,out] The direct value
	/// \returns std::string the value as string.
	//************************************
	std::string GetValueString(const std::string& indirectString, double directValue);

	//************************************
	/// Checks the indirect reference string if it is valid
	/// \param rInspectionName [in] reference to the inspection name
	/// \param rIndirectString [in] reference top the indirect string
	/// \returns true if valid
	//************************************
	bool isValidReference( const std::string& rInspectionName, const std::string& rIndirectString ) const;

	/// Change the indirect names, if necessary
	/// \param rIndirectString [in,out] reference to the indirect name. It will be changed if necessary
	/// \param oldPefix [in] old  name
	/// \param newPrefix [in] new name
	/// \returns bool
	bool RenameIndirectValue(std::string& rIndirectString, LPCTSTR oldPefix, LPCTSTR newPrefix);
#pragma endregion Private Methods

#pragma region Member variables
private:
	SVRangeClass* m_pRange;
	double m_FailHigh;
	double m_WarnHigh;
	double m_WarnLow;
	double m_FailLow;
	std::string m_FailHighIndirect;
	std::string m_WarnHighIndirect;
	std::string m_WarnLowIndirect;
	std::string m_FailLowIndirect;
#pragma endregion Member variables
};

