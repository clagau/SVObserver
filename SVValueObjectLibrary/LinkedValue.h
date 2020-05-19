//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \Author	Marc Ziegler
//*****************************************************************************
/// This class should managed value object which are linked to other value objects (or only a valid value).
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVStringValueObjectClass.h"
#include "SVVariantValueObjectClass.h"
#include "SVObjectLibrary\SVObjectReference.h"
#pragma endregion Includes

namespace SvOl
{
struct SVInObjectInfoStruct;
}

namespace SvVol
{

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
	/// \returns S_OK if successful
	//************************************
	virtual HRESULT GetValue(_variant_t& rValue, int Index = -1) const override;
	
	/// similar to GetValue but  for safe arrays 
	HRESULT   GetArrayValue(_variant_t& rValue);

	virtual HRESULT SetDefaultValue(const _variant_t& rValue, bool bResetAll = true) override;

	virtual HRESULT  SetValue(const _variant_t& rValue, int Index = -1) override;

	virtual HRESULT  setValue(const _variant_t& rValue, int Index = -1, bool fixArrasize = false) override;
	virtual HRESULT setValue(const std::string& rValue, int Index = -1) override;

	virtual void updateMemBlockData() const override;

	virtual bool DisconnectObjectInput(SvOl::SVInObjectInfoStruct* pObjectInInfo ) override;

	
	void UpdateLinkedName();

	SVStringValueObjectClass& getLinkedName() { return m_LinkedName; };

	virtual bool isIndirectValue() const override { return (nullptr != m_LinkedObjectRef.getObject()); };

	void setIndirectValueSaveFlag(bool shouldSaveValue);

	virtual DWORD GetType() const override;

	bool isCircularReference() const;

	template <typename T> T getValueAs() const  //this function may throw an exception if incompatible types are used, so use it with care!
	{
		_variant_t temporaryVariant;
		this->GetValue(temporaryVariant);
		return static_cast<T>(temporaryVariant);
	}

#pragma region Methods to replace processMessage
	virtual void OnObjectRenamed(const SVObjectClass& rRenamedObject, const std::string& rOldName) override { UpdateLinkedName(); };
#pragma endregion Methods to replace processMessage

	const SVObjectClass* GetLinkedObject()  const;

#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	//! Convert a string in a variant. Throw an exception if the string isn't convertible into a variant
	//! \param rValue [in] The input string
	//! \returns the converted value.
	virtual _variant_t ConvertString2Type( const std::string& rValue ) const override;
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
	SVObjectReference ConvertStringInObject( const std::string& rValue ) const;

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

} //namespace SvVol
