//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInObjectInfoStruct
//* .File Name       : $Workfile:   SVInObjectInfoStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:34:44  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
#include "SVObjectInfoStruct.h"
#pragma endregion Includes

class SVGUID;

namespace SvOl
{

struct SVInObjectInfoStruct : public SVObjectInfoStruct
{
	SVInObjectInfoStruct();
	SVInObjectInfoStruct( const SVInObjectInfoStruct& rSrc );

	virtual ~SVInObjectInfoStruct();

	const SVInObjectInfoStruct& operator = ( const SVInObjectInfoStruct& rhs );
	bool IsConnected() const;

	const SVObjectInfoStruct& GetInputObjectInfo() const;

	void SetInputObjectType(SvPb::SVObjectTypeEnum ObjectType = SvPb::SVNotSetObjectType, SvPb::SVObjectSubTypeEnum SubType = SvPb::SVNotSetSubObjectType, const SVGUID& rEmbeddedID = GUID_NULL);
	void SetInputObjectType( const SvDef::SVObjectTypeInfoStruct& rTypeInfo );

	void SetInputObject( const SVGUID& p_rObjectID );
	void SetInputObject( SVObjectClass* p_pObject );
	void SetInputObject( const SVObjectReference& p_rObject );

	const std::string& GetInputName() const;
	void SetInputName( const std::string& p_rInputName );

protected:
	// Who is my input object...
	//	To connect ( hook ) on input, the method ConnectObjectInput to
	//  the object that needs the input GUID! And use a pointer of this structure 
	//	( of the input ) as a message parameter.
	SVObjectInfoStruct m_InputObjectInfo;	// this is the Object, this Object is using

	bool m_IsConnected;

	std::string m_InputName;

};

typedef std::vector<SVInObjectInfoStruct> SVInObjectInfoStructVector;
typedef std::vector<SVInObjectInfoStruct*> SVInObjectInfoStructPtrVector;

void ValidateInput(SVInObjectInfoStruct& rInputObject);
void ValidateInputList(SVInObjectInfoStructPtrVector& rInputObjectList);

template <typename T>
T* getInput(const SVInObjectInfoStruct& rInputObject, bool bRunMode=false)
{
	if (rInputObject.IsConnected() && nullptr != rInputObject.GetInputObjectInfo().getObject())
	{
		SVObjectClass* pObject = rInputObject.GetInputObjectInfo().getObject();
		//! Use static_cast to avoid time penalty in run mode for dynamic_cast
		//! We are sure that when getObject() is not nullptr that it is the correct type
		return bRunMode ? static_cast<T*> (pObject) : dynamic_cast<T*> (pObject);
	}

	return nullptr;
}
} //namespace SvOl