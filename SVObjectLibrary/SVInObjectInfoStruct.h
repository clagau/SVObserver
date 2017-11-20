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

//Moved to precompiled header: #include <vector>
#include "SVUtilityLibrary/SVGUID.h"

#include "SVObjectInfoStruct.h"

struct SVInObjectInfoStruct : public SVObjectInfoStruct
{
	SVInObjectInfoStruct();
	SVInObjectInfoStruct( const SVInObjectInfoStruct& rSrc );

	virtual ~SVInObjectInfoStruct();

	const SVInObjectInfoStruct& operator = ( const SVInObjectInfoStruct& rhs );
	bool IsConnected() const;

	const SVObjectInfoStruct& GetInputObjectInfo() const;

	void SetInputObjectType( SvDef::SVObjectTypeEnum p_ObjectType=SvDef::SVNotSetObjectType, SvDef::SVObjectSubTypeEnum p_SubType=SvDef::SVNotSetSubObjectType );
	void SetInputObjectType( const SVGUID& p_rEmbeddedID, SvDef::SVObjectTypeEnum p_ObjectType=SvDef::SVNotSetObjectType, SvDef::SVObjectSubTypeEnum p_SubType=SvDef::SVNotSetSubObjectType );
	void SetInputObjectType( const SvDef::SVObjectTypeInfoStruct& p_rTypeInfo );

	void SetInputObject( const std::string& p_rName );
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

