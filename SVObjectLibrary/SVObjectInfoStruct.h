//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectInfoStruct
//* .File Name       : $Workfile:   SVObjectInfoStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:50:42  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "Definitions/SVObjectTypeInfoStruct.h"
#include "SVObjectReference.h"
#pragma endregion

class SVObjectClass;

struct SVObjectInfoStruct
{
	SVObjectInfoStruct();
	SVObjectInfoStruct( const SVObjectInfoStruct& rObjectInfo );
	SVObjectInfoStruct( const SVObjectReference& rObjectRef );

	virtual ~SVObjectInfoStruct();

	const SVObjectInfoStruct& operator = ( const SVObjectInfoStruct& O2 );

	void clear();

	HRESULT SetObject(uint32_t objectID);
	HRESULT SetObject(SVObjectClass* pObject);
	HRESULT SetObject(const SvDef::SVObjectTypeInfoStruct& rTypeInfo);
	HRESULT SetObject(const SVObjectReference& rObjectRef);
	HRESULT SetObject(const SVObjectInfoStruct& rObjectInfo);

	bool operator == ( const SVObjectInfoStruct& rRhs ) const;
	bool operator == ( const SVObjectReference& rRhs ) const;

	SVObjectClass* getObject() { return m_ObjectRef.getObject(); };
	//The method is const however the returned pointer not, because of other const methods returning a non const pointer!
	SVObjectClass* getObject() const { return m_ObjectRef.getObject(); };
	uint32_t  getObjectId() const { return m_ObjectRef.getObjectId(); };

	SVObjectReference& GetObjectReference() { return m_ObjectRef; };
	const SVObjectReference& GetObjectReference() const { return m_ObjectRef; };
	bool CheckExistence() const;


	SvDef::SVObjectTypeInfoStruct m_ObjectTypeInfo;  // What I am
	
	SVObjectReference m_ObjectRef;
};

