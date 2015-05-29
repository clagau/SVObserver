//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \Author	Marc Ziegler
//*****************************************************************************
/// This class should managed value object which are linked to value object to other value objects (or only a valid value).
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVObjectLibrary\SVInObjectInfoStruct.h"
#include "SVVariantValueObjectClass.h"
#include "SVTaskObject.h"
#pragma endregion Includes


#pragma region Declarations
#pragma endregion Declarations

class LinkedValue
{
#pragma region Constructor
public:
	LinkedValue();

	~LinkedValue();
#pragma endregion Constructor

#pragma region Public Methods
public:

	//************************************
	/// Initialized the object. 
	/// \param pInspection [in] The pointer to the inspection of this objects.
	/// \param isInvalidStringSetToDefault [in] if the dotted name become invalid: m_isInvalidStringSetToDefault == true ->  it will reset to default, else (false) -> Object become invalid.
	//************************************
	void init(SVInspectionProcess *pInspection, bool isInvalidStringSetToDefault = true);

	//************************************
	/// Register the linked value object as input object and the variant value as embedded object to the task-object.
	/// \param taskObject [in] The task object where the objects should be registered
	/// \param inputConnectionName [in] input connection name for the linked value.
	/// \param rguidEmbeddedID [in] The GUID for the embedded object (variant value).
	/// \param iStringResourceID [in] String Resource IDof the embedded object name.
	/// \param bResetAlways [in] Should object always reset.
	/// \param eRequiredReset [in] The reset enum.
	//************************************
	void RegisterObjects(
		SVTaskObjectClass &taskObject, 
		const SVString& inputConnectionName, 
		const GUID& rguidEmbeddedID, 
		int iStringResourceID, 
		bool bResetAlways, 
		SVResetItemEnum eRequiredReset);

	//************************************
	/// Set the default value to the variant value.
	/// \param value [in] the new default value.
	/// \param isResetAll [in] Should default values from all buckets reseted.
	//************************************
	void setDefaultValue(const VARIANT value, bool isResetAll);

	//************************************
	/// Return the current value. If linked value valid, it return this value, otherwise it will return the value from the variant value.
	/// \returns _variant_t
	//************************************
	_variant_t GetValue() const;

	//************************************
	/// Return the input value. It is the dotted name or the variant value.
	/// \returns SVString
	//************************************
	SVString GetInputValue() const;

	//************************************
	/// Set the value. If string a valid dotted name of a value object, it connect it to the linked object.
	/// \param valueString [in] The string, have to be a valid dotted name of a value object or a valid value.
	/// \returns HRESULT S_OK, if set was OK.
	//************************************
	HRESULT SetInputValue(const SVString& valueString);

	//************************************
	/// Check if the new value string is valid and return the valid variant if valid.
	/// \param valueString [in] The string which should be converted.
	/// \param vtNew [out] The new variant if check was succeeded.
	/// \returns HRESULT S_OK, if string is valid.
	//************************************
	HRESULT convertStringToValue(const SVString& valueString, _variant_t &vtNew) const;

	//************************************
	/// Rename the linked value if his tool set name was renamed.
	/// \param pObject [in] The changed object.
	/// \param originalName [in] The old original name of this object
	//************************************
	void RenameToolSetSymbol(const SVObjectClass* pObject, const SVString& originalName);

	//************************************
	/// Reset the object and check the linked objects.
	/// \returns HRESULT
	//************************************
	HRESULT ResetObject();

	//************************************
	/// Check if this class and his members are valid.
	/// \returns bool
	//************************************
	bool IsValid() const;

	//************************************
	/// Set the attributes for the variant value object.
	/// \param bits [in] The bits of attributes to set.
	/// \param isOn [out] Define if bits should set (true) or unset (false).
	//************************************
	void SetAttributes(unsigned int bits, bool isOn);
#pragma endregion Public Methods

#pragma region Protected Methods
protected:

#pragma endregion Protected Methods

#pragma region Private Methods
private:
	//************************************
	/// Connect the input value to this object.
	/// \returns HRESULT
	//************************************
	HRESULT ConnectInput();

#pragma endregion Private Methods

#pragma region Member Variables
private:
	SVInObjectInfoStruct m_inputConnectionInfo; //<< used to connect to other value object
	SVVariantValueObjectClass m_variantObject; //<< our own value objects
	SVInspectionProcess *m_pInspection;
	bool m_isInvalidStringSetToDefault; //<< if the dotted name become invalid: m_isInvalidStringSetToDefault == true ->  it will reset to default, else (false) -> Object become invalid
	bool m_isValid;
#pragma endregion Member Variables
};
