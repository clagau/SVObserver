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
#include "ObjectInterfaces\ILinkedObject.h"
#include "SVObjectLibrary\SVObjectReference.h"
#pragma endregion Includes

namespace SvVol
{

class LinkedValue : public SVVariantValueObjectClass, public SvOi::ILinkedObject
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
	HRESULT GetArrayValue(_variant_t& rValue);

	virtual HRESULT SetDefaultValue(const _variant_t& rValue, bool bResetAll = true) override;

	virtual HRESULT SetValue(const _variant_t& rValue, int Index = -1) override;

	virtual HRESULT setValue(const _variant_t& rValue, int Index = -1, bool fixArrasize = false) override;
	virtual HRESULT setValue(const std::string& rValueString, int Index = -1) override;

	virtual void updateMemBlockData() override;
	
	void UpdateLinkedName();

	SVStringValueObjectClass& getLinkedName() { return m_LinkedName; };
	const SVStringValueObjectClass& getLinkedName() const { return m_LinkedName; };

	virtual bool isIndirectValue() const override { return (nullptr != m_LinkedObjectRef.getObject()); };

	void setIndirectValueSaveFlag(bool shouldSaveValue);

	virtual DWORD GetType() const override;

	bool isCircularReference() const;

	virtual void fillObjectList(std::back_insert_iterator<std::vector<SvOi::IObjectClass*>> inserter, const SvDef::SVObjectTypeInfoStruct& rObjectInfo, bool addHidden = false, bool stopIfClosed = false, bool firstObject = false) override;

	virtual void OnObjectRenamed(const SVObjectClass& , const std::string& ) override;

	virtual bool CreateObject(const SVObjectLevelCreateStruct& rCreateStructure) override;

	virtual bool isCorrectType(SvPb::ObjectSelectorType requiredType, const SVObjectClass* pTestObject = nullptr) const;
	virtual const SvOi::IObjectClass* getLinkedObject() const override;

	virtual void disconnectObjectInput(uint32_t objectId) override;
	virtual void disconnectAllInputs() override;
	/// Disconnected the input connection and set it to nullptr.
	void DisconnectInput();
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	//! Convert a string in a variant. Throw an exception if the string isn't convertible into a variant
	//! \param rValue [in] The input string
	//! \returns the converted value.
	virtual _variant_t ConvertString2Type( const std::string& rValue ) const override;
	virtual _variant_t ConvertString2Type(const std::string& rValue, const _variant_t& rDefaultValue) const override;
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	/// Update the input connection, dependent of the variant value.
	/// \param pErrorMessages [in,out] Pointer to a error list, if pointer != nullptr an error message will be added if an error is happend.
	/// \returns bool
	bool UpdateConnection(SvStl::MessageContainerVector *pErrorMessages=nullptr);

	/// Connect the input connection with a new object
	/// \returns bool
	bool ConnectInput();

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	/// Checks if the linked object is valid.
	/// \param pLinkedObject [in]  Pointer to the indirect object
	/// \param pErrorMessages [in,out] Pointer to a error list, if pointer != nullptr an error message will be added if an error is happend.
	/// \returns bool true if linked object is valid
	bool CheckLinkedObject( const SVObjectClass* const pLinkedObject, const variant_t& rDefault, SvStl::MessageContainerVector *pErrorMessages=nullptr ) const;
#pragma endregion Private Methods

#pragma region Member Variables
private:
	
	SVStringValueObjectClass m_LinkedName;
	SVObjectReference m_LinkedObjectRef;
	mutable bool m_CircularReference;					//! Use this flag during GetValue to make sure no circular references are present
#pragma endregion Member Variables


};

} //namespace SvVol
