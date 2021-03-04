//*****************************************************************************
/// \copyright (c) 2020,2020 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \Author	Marc Ziegler
//*****************************************************************************
/// This class should managed the inputs of a task.
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
#include "SVObjectClass.h"
#include "SVOResource/resource.h"
#pragma endregion Includes


namespace SvOl
{
class InputObject : public SVObjectClass
{
	SV_DECLARE_CLASS

	explicit InputObject(LPCTSTR ObjectName);
	InputObject(SVObjectClass* pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_INPUTCONNECTOBJECT);

	virtual ~InputObject();
	bool IsConnected() const;

	const SVObjectInfoStruct& GetInputObjectInfo() const;
	void SetInputObjectType(SvPb::SVObjectTypeEnum ObjectType = SvPb::SVNotSetObjectType, SvPb::SVObjectSubTypeEnum SubType = SvPb::SVNotSetSubObjectType, SvPb::EmbeddedIdEnum	embeddedID = SvPb::NoEmbeddedId);
	void SetInputObjectType( const SvDef::SVObjectTypeInfoStruct& rTypeInfo );

	void SetInputObject(uint32_t objectID);
	void SetInputObject(SVObjectClass* p_pObject );
	void SetInputObject(const SVObjectReference& p_rObject );

	void validateInput();
	virtual void disconnectObjectInput(uint32_t objectId) override;
	virtual void disconnectAllInputs() override;

	virtual UINT SetObjectAttributesAllowed(UINT Attributes, SvOi::SetAttributeType Type) override;

	virtual void Persist(SvOi::IObjectWriter& rWriter) override;

	template <typename T>
	T* getInput(bool bRunMode = false) const
	{
		if (IsConnected() && nullptr != GetInputObjectInfo().getFinalObject())
		{
			SVObjectClass* pObject = GetInputObjectInfo().getFinalObject();
			//! Use static_cast to avoid time penalty in run mode for dynamic_cast
			//! We are sure that when getObject() is not nullptr that it is the correct type
			return bRunMode ? static_cast<T*> (pObject) : dynamic_cast<T*> (pObject);
		}

		return nullptr;
	}

private:
	void correctDependencies();

private:
	// Who is my input object...
	//	To connect ( hook ) on input, the method ConnectObjectInput to
	//  the object that needs the input ID! And use a pointer of this structure 
	//	( of the input ) as a message parameter.
	SVObjectInfoStruct m_InputObjectInfo;	// this is the Object, this Object is using
	bool m_IsConnected = false;
};
} //namespace SvOl