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

#include "SVcontainerLibrary/SVVector.h" 
#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVObjectInfoStruct.h"

struct SVInObjectInfoStruct : public SVObjectInfoStruct
{
	SVInObjectInfoStruct();
	SVInObjectInfoStruct( const SVInObjectInfoStruct& rSrc );

	virtual ~SVInObjectInfoStruct();

	const SVInObjectInfoStruct& operator = ( const SVInObjectInfoStruct& rhs );
	bool IsConnected() const;

	const SVObjectInfoStruct& GetInputObjectInfo() const;

	void SetInputObjectType( SVObjectTypeEnum p_ObjectType=SVNotSetObjectType, long p_SubType=SVNotSetSubObjectType );
	void SetInputObjectType( const SVGUID& p_rEmbeddedID, SVObjectTypeEnum p_ObjectType=SVNotSetObjectType, long p_SubType=SVNotSetSubObjectType );
	void SetInputObjectType( const SVObjectTypeInfoStruct& p_rTypeInfo );

	void SetInputObject( const SVString& p_rName );
	void SetInputObject( const SVGUID& p_rObjectID );
	void SetInputObject( SVObjectClass* p_pObject );
	void SetInputObject( const SVObjectReference& p_rObject );

	const SVString& GetInputName() const;
	void SetInputName( const SVString& p_rInputName );

protected:
	// Who is my input object...
	//	To connect ( hook ) on input, send a SVM_CONNECT_OBJECT_INPUT message to
	//  the object that needs the input GUID! And use a pointer of this structure 
	//	( of the input ) as a message parameter.
	SVObjectInfoStruct m_InputObjectInfo;	// this is the Object, this Object is using

	bool m_IsConnected;

	SVString m_InputName;

};

typedef SVVector < SVInObjectInfoStruct, SVInObjectInfoStruct& > SVVectorSVInObjectInfoStruct; 

