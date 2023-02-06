//*****************************************************************************
/// \copyright (c) 2020,2020 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
/// \Author	Marc Ziegler
//*****************************************************************************
/// This class should managed the inputs of a task.
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
#include "SVObjectAppClass.h"
#include "SVOResource/resource.h"
#include "ObjectInterfaces/IInputObject.h"
#include "InspectionEngine/SVImageClass.h"
#pragma endregion Includes


namespace SvOl
{
class InputObject : public SVObjectAppClass, public SvOi::IInputObject
{
	SV_DECLARE_CLASS

	explicit InputObject(LPCTSTR ObjectName);
	InputObject(SVObjectClass* pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_INPUTCONNECTOBJECT);

	virtual ~InputObject();
	bool IsConnected() const;

	const SVObjectInfoStruct& GetInputObjectInfo() const;
	void SetInputObjectType(SvPb::SVObjectTypeEnum ObjectType = SvPb::SVNotSetObjectType, SvPb::SVObjectSubTypeEnum SubType = SvPb::SVNotSetSubObjectType, SvPb::EmbeddedIdEnum	embeddedID = SvPb::NoEmbeddedId);
	void SetInputObjectType( const SvDef::SVObjectTypeInfoStruct& rTypeInfo );

	virtual HRESULT checkAndSetInput(uint32_t connnectedInput) override;
	void SetInputObject(uint32_t objectID);
	void SetInputObject(SVObjectClass* p_pObject );
	void SetInputObject(const SVObjectReference& p_rObject );

	void validateInput();
	virtual void disconnectObjectInput(uint32_t objectId) override;
	virtual void disconnectAllInputs() override;

	virtual UINT SetObjectAttributesAllowed(UINT Attributes, SvOi::SetAttributeType Type) override;
	virtual std::tuple<SvDef::SVObjectTypeInfoStruct, uint32_t, SvOi::InputAllowedMode, uint32_t> getAvailableObjectParameter() const override;
	virtual SvPb::GetInputDataResponse getInputData(SvPb::SVObjectTypeEnum desiredFirstObjectTypeForConnectedName, bool excludeFirstObjectNameInConntectedName) const override;

	virtual void Persist(SvOi::IObjectWriter& rWriter, bool closeObject = true) const override;
	virtual void fixInvalidInputs(std::back_insert_iterator<std::vector<SvPb::FixedInputData>> inserter) override;
	virtual void changeSource(const SVObjectReference& rOldObject, SVObjectClass& rNewObject) override;
	void setDottedNameOfLoadedInput(const std::string& rName) { m_DottedNameOfFailedLoadInput = rName; };

	void setStartSearchId(uint32_t startSearchId) { m_startSearchId = startSearchId; };
	void setAllowedMode(SvOi::InputAllowedMode mode) { m_allowedMode = mode; };
	SvOi::InputAllowedMode getAllowedMode() const { return m_allowedMode; };

	template <typename T>
	T* getInput(bool bRunMode = false) const
	{
		if (IsConnected())
		{
			SVObjectClass* pObject = GetInputObjectInfo().getFinalObject();
			if (nullptr == pObject || 0 == (pObject->ObjectAttributesAllowed() & SvPb::useable))
			{
				return nullptr;
			}
			//! Use static_cast to avoid time penalty in run mode for dynamic_cast
			//! We are sure that when getObject() is not nullptr that it is the correct type
			return bRunMode ? static_cast<T*> (pObject) : dynamic_cast<T*> (pObject);
		}

		return nullptr;
	}

	template <>
	SvIe::SVImageClass* getInput<SvIe::SVImageClass>(bool /*bRunMode*/) const
	{
		if (IsConnected())
		{
			SVObjectClass* pObject = GetInputObjectInfo().getFinalObject();
			if (pObject && pObject->GetObjectType() == SvPb::SVImageObjectType && 0 != (pObject->ObjectAttributesAllowed() & SvPb::useable))
			{
				return static_cast<SvIe::SVImageClass*> (pObject);
			}
			else
			{
				return nullptr;
			}
		}
		return nullptr;
	}
	
	

	template <typename T>
	T* getInputObject(bool bRunMode = false) const
	{
		if (IsConnected() && nullptr != GetInputObjectInfo().getObject())
		{
			SVObjectClass* pObject = GetInputObjectInfo().getObject();
			//! Use static_cast to avoid time penalty in run mode for dynamic_cast
			//! We are sure that when getObject() is not nullptr that it is the correct type
			return bRunMode ? static_cast<T*> (pObject) : dynamic_cast<T*> (pObject);
		}

		return nullptr;
	}

private:
	void correctDependencies();
	bool checkIfAllowedObject();

private:
	// Who is my input object...
	//	To connect ( hook ) on input, the method ConnectObjectInput to
	//  the object that needs the input ID! And use a pointer of this structure 
	//	( of the input ) as a message parameter.
	SVObjectInfoStruct m_InputObjectInfo;	// this is the Object that this Object is using
	bool m_IsConnected = false;

	uint32_t m_startSearchId = SvDef::InvalidObjectId; //if Invalid, it will be changed to the inspectionId
	SvOi::InputAllowedMode m_allowedMode = SvOi::InputAllowedMode::Default;
	std::string m_DottedNameOfFailedLoadInput;
};
} //namespace SvOl