//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \Author	Marc Ziegler
//*****************************************************************************
/// This class should managed value object which are linked to other value objects (or only a valid value).
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVObjectLibrary\SVInObjectInfoStruct.h"
#include "SVObjectLibrary\SVObjectReference.h"
#include "SVUtilityLibrary\SVString.h"
#include "SVStringValueObjectClass.h"
#include "SVVariantValueObjectClass.h"
#pragma endregion Includes

#pragma region Declarations
#pragma endregion Declarations

class LinkedValue : public SVVariantValueObjectClass
{
#pragma region Constructor
public:
	LinkedValue();

	virtual ~LinkedValue();
#pragma endregion Constructor

#pragma region Public Methods
public:

	//************************************
	/// Return the current value. If a valid linked value this will be returned otherwise it will return the the variant value.
	/// \param rValue [out] reference to the value to write to
	/// \param Index [in]  index of array to get
	/// \param Bucket [in] bucket to get
	/// \returns S_OK if successful
	//************************************
	virtual HRESULT GetValue(_variant_t& rValue, int Index = -1, int Bucket = -1) const override;

	virtual bool DisconnectObjectInput( SVInObjectInfoStruct* pObjectInInfo ) override;

	//************************************
	/// Update the linked name 
	/// \returns HRESULT
	//************************************
	void UpdateLinkedName();

	SVStringValueObjectClass& getLinkedName() { return m_LinkedName; };

	bool isIndirectValue() { return ( nullptr != m_LinkedObjectRef.getObject() ); };

#pragma region Methods to replace processMessage
	virtual void OnObjectRenamed(const SVObjectClass& rRenamedObject, const SVString& rOldName) override { UpdateLinkedName(); };
#pragma endregion Methods to replace processMessage
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	//! Convert a string in a variant. Throw an exception if the string isn't convertible into a variant
	//! \param rValue [in] The input string
	//! \returns the converted value.
	virtual _variant_t ConvertString2Type( const SVString& rValue ) const override;
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	/// Update the input connection, dependent of the variant value.
	/// \param pErrorMessages [in,out] Pointer to a error list, if pointer != nullptr an error message will be added if an error is happend.
	/// \returns bool
	bool UpdateConnection(SvStl::MessageContainerVector *pErrorMessages=nullptr);

	/// Disconnected the input connection and set it to nullptr.
	void DisconnectInput();

	/// Connect the input connection with a new object
	/// \returns bool
	bool ConnectInput();

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	/// Convert a string (dotted name) to an object.
	/// \param rValue [in] Input string
	/// \returns SVObjectReference A reference to the found object. 
	SVObjectReference ConvertStringInObject( const SVString& rValue ) const;

	/// Checks if the linked object is valid.
	/// \param pLinkedObject [in]  Pointer to the indirect object
	/// \param pErrorMessages [in,out] Pointer to a error list, if pointer != nullptr an error message will be added if an error is happend.
	/// \returns bool true if linked object is valid
	bool CheckLinkedObject( const SVObjectClass* const pLinkedObject, SvStl::MessageContainerVector *pErrorMessages=nullptr ) const;
#pragma endregion Private Methods

#pragma region Member Variables
private:
	SVStringValueObjectClass m_LinkedName;
	SVObjectReference m_LinkedObjectRef;
	mutable bool m_CircularReference;					//! Use this flag during GetValue to make sure no circular references are present
#pragma endregion Member Variables
};
