//*****************************************************************************
/// \copyright (c) 2015,2015 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
/// \Author	Marc Ziegler
/// \file LinkedValue.h
//*****************************************************************************
/// This class should managed value object which are linked to other value objects (or only a valid value).
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVStringValueObjectClass.h"
#include "SVVariantValueObjectClass.h"
#include "ObjectInterfaces\ILinkedObject.h"
#include "SVObjectLibrary\SVObjectReference.h"
#include "SVEnumerateValueObjectClass.h"
#include "Operators\SVEquation.h"
#pragma endregion Includes

namespace SvVol
{

class LinkedValue : public SVVariantValueObjectClass, public SvOi::ILinkedObject
{
	SV_DECLARE_CLASS

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
	
	void  SetAllowVoidReference(bool set);
	bool  GetAllowVoidReference() const;

	virtual HRESULT SetValue(const _variant_t& rValue, int Index = -1) override;
	virtual HRESULT SetValueKeepType(LPCTSTR Value) override { return SetValueKeepType(Value, 0); }
	virtual HRESULT SetValueKeepType(LPCTSTR Value, int Index) override;
	//This version of the method is used for Tool Extents, but else the other methods setDirectValue or setIndirectValue
	virtual HRESULT setValue(const _variant_t& rValue, int Index = -1, bool fixArrasize = false) override;
	virtual HRESULT setValue(const SvPb::LinkedValue& rData) override;
	//This version of the method should not used for LinkedValue, but the other methods setDirectValue or setIndirectValue
	virtual HRESULT setValue(const std::string& rValueString, int Index = -1) override { Log_Assert(false); return __super::setValue(rValueString, Index); };
	bool setDirectValue(const _variant_t& rValue);
	bool setIndirectValue(const std::string& rValueString);
	bool setIndirectValue(const SVObjectReference& rReference);

	virtual void updateMemBlockData() override;
	
	void UpdateContent();

	void setIndirectValueSaveFlag(bool shouldSaveValue);

	virtual DWORD GetType() const override;

	bool isCircularReference() const;

	virtual void fillObjectList(std::back_insert_iterator<std::vector<SvOi::IObjectClass*>> inserter, const SvDef::SVObjectTypeInfoStruct& rObjectInfo, bool addHidden = false, bool stopIfClosed = false, bool firstObject = false) override;

	virtual void OnObjectRenamed(const SVObjectClass& , const std::string& ) override;

	virtual bool CreateObject(const SVObjectLevelCreateStruct& rCreateStructure) override;
	virtual bool CloseObject() override;

	virtual bool isCorrectType(SvPb::ObjectSelectorType requiredType) const;
	virtual const SvOi::IObjectClass* getLinkedObject() const override;
	virtual void fillLinkedData(SvPb::LinkedValue& rLinkedValue) const override;
	virtual void fillSelectorListForLink(std::back_insert_iterator<std::vector<SvPb::TreeItem>> treeInserter) const override;
	virtual _variant_t validateValue(const SvPb::LinkedValue& rLinkedValue) const override;

	virtual void disconnectObjectInput(uint32_t objectId) override;
	virtual void disconnectAllInputs() override;
	virtual void getOutputList(std::back_insert_iterator<std::vector<SvOi::IObjectClass*>> inserter) const override;
	virtual void fixInvalidInputs(std::back_insert_iterator<std::vector<SvPb::FixedInputData>> inserter) override;
	virtual void changeSource(const SVObjectReference& rOldObject, SVObjectClass& rNewObject) override;

	virtual SvOi::IObjectClass* getFirstObject(const SvDef::SVObjectTypeInfoStruct& rObjectTypeInfo, bool, const SvOi::IObjectClass* pRequestor) const override;

	virtual void fillSelectorList(std::back_insert_iterator<std::vector<SvPb::TreeItem>> treeInserter, SvOi::IsObjectAllowedFunc pFunctor, UINT attribute, bool wholeArray, SvPb::SVObjectTypeEnum nameToType, SvPb::ObjectSelectorType requiredType, bool stopIfClosed = false, bool firstObject = false) const override;
	/// Disconnected the input connection and set it to nullptr.
	void DisconnectInput();

	virtual HRESULT GetChildObject(SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index = 0) const;

	virtual bool isArray() const override;

	virtual HRESULT getValue(double& rValue, int Index = -1) const override;
	virtual HRESULT getValue(_variant_t& rValue, int Index = -1) const override { return __super::getValue(rValue, Index); };
	virtual HRESULT getValue(std::string& rValueString, int Index = -1, const std::string& rFormatString = _T("")) const;

	virtual int32_t getArraySize() const override;
	virtual int32_t getResultSize() const override;

	std::string getContentStr() const;

	virtual void Persist(SvOi::IObjectWriter& rWriter) const override;
	virtual HRESULT SetObjectValue(SVObjectAttributeClass* PDataObject) override;

	bool runEmbedded(SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector* pErrorMessages);

	virtual SvPb::LinkedSelectedOption getSelectedOption() const override;

	virtual HRESULT setIndirectStringForOldStruct(const std::vector<_variant_t>& rValueString) override;
	virtual void setChildIds(const std::vector<uint32_t>& rObjectIds) override;

	void setValueType(SvPb::LinkedValueTypeEnum type);
	void setExcludeSameLineageListForObjectSelector(std::vector<SvOi::IObjectClass*> value) { m_excludeSameLineageList = std::move(value); };
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

	bool updateFromOldStruct(SvStl::MessageContainerVector* pErrorMessages);

	void setDefaultDefaultIfEmpty(_variant_t& value);

	bool checkLinkedObjectRef(SvStl::MessageContainerVector* pErrorMessages = nullptr) const;
	bool updateLinkedValue(SVObjectReference& LinkedObjectRef, SvStl::MessageContainerVector* pErrorMessages);

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	/// Checks if the linked object is valid.
	/// \param pLinkedObject [in]  Pointer to the indirect object
	/// \param pErrorMessages [in,out] Pointer to a error list, if pointer != nullptr an error message will be added if an error is happend.
	/// \returns bool true if linked object is valid
	bool CheckLinkedObject( const SVObjectClass* const pLinkedObject, const variant_t& rDefault, SvStl::MessageContainerVector *pErrorMessages=nullptr ) const;

	void donotCheckForDependency() { m_checkForValidDependency = false; };


	/// Validate indirect value and return the current value. Throw an exception if validation failed.
	/// \param rIndirectIdString [in] The dotted name of the indirect value object.
	/// \param defaultValue [in] The default name.
	/// \returns variant_t
	variant_t validateIndirectValue(const std::string& rIndirectIdString, variant_t defaultValue) const;

	/// Validate formula and return the current value. Throw an exception if validation failed.
	/// \param rFormula [in] The formula text.
	/// \returns variant_t
	variant_t validateFormula(const std::string& rFormula) const;

	bool setValueFromDouble(double value);

	void setSelectedOption(SvPb::LinkedSelectedOption option);

	bool resetChildren(const SVObjectClass* const pLinkedObject, const std::vector<SvPb::EmbeddedIdEnum>& rEmbeddedIdList, SvStl::MessageContainerVector* pErrorMessages);
	bool resetChild(int pos, SvOi::IValueObject* pValue, SvStl::MessageContainerVector* pErrorMessages, const SVObjectLevelCreateStruct& rCreateStruct);

	void tryToFixIndirectInput(SvDef::SVObjectTypeInfoStruct info);
	void tryToFixInput();
	void addFixedData(std::back_insert_iterator<std::vector<SvPb::FixedInputData>> inserter, const std::string& rOldInput);

	void setOrRemoveLinkedImageToTRC();
#pragma endregion Private Methods

#pragma region Member Variables
private:
	SVObjectReference m_LinkedObjectRef;
	bool m_checkForValidDependency = true; //Child-LinkedValue should not check for valid dependency because it was already checked by the parent and is an indirect link.
	mutable bool m_CircularReference;					//! Use this flag during GetValue to make sure no circular references are present
	SVStringValueObjectClass m_Content;
	SVEnumerateValueObjectClass m_refOptionObject;
	SvPb::LinkedSelectedOption m_refOption = SvPb::None;

	std::string m_oldIndirectString; //! only used during load/create phase, if old config (older than 10.20) is load.

	_variant_t m_directValue;
	SVObjectReference m_indirectValueRef;
	std::string m_formulaString{ "" };
	SvOp::SVEquation m_equation;
	std::vector<std::unique_ptr<LinkedValue>> m_children;
	std::vector<uint32_t> m_childrenIds;
	bool m_allowVoidReference {false};
	SvPb::LinkedValueTypeEnum m_valueType {SvPb::LinkedValueTypeEnum::TypeDecimal};
	std::vector<SvOi::IObjectClass*> m_excludeSameLineageList; //used for fillSelectorListForLink, if empty AttributesAllowedFilter-mode used.
#pragma endregion Member Variables
};

} //namespace SvVol
