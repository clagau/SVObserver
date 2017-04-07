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
#include "ObjectInterfaces/IObjectClass.h"
#include "SVSystemLibrary/SVCriticalSection.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/SVString.h"
#include "ObjectInterfaces/SVObjectTypeInfoStruct.h"
#include "SVObjectReference.h"
#pragma endregion

class SVObjectClass;

struct SVObjectInfoStruct
{
	SVObjectInfoStruct();
	SVObjectInfoStruct( const SVObjectInfoStruct& rObjectInfo );
	SVObjectInfoStruct( SVObjectClass* pObject );
	SVObjectInfoStruct( const SVObjectReference& rObjectRef );

	virtual ~SVObjectInfoStruct();

	const SVObjectInfoStruct& operator = ( const SVObjectInfoStruct& O2 );

	void clear();

	void ClearObjectInfo();

	HRESULT SetObject( const SVGUID& rObjectID );
	HRESULT SetObject( const SVString& rName );
	HRESULT SetObject( const SVObjectTypeInfoStruct& rTypeInfo );
	HRESULT SetObject( SVObjectClass* pObject );
	HRESULT SetObject( const SVObjectReference& rObjectRef );
	HRESULT SetObject( const SVObjectInfoStruct& rObjectInfo );

	bool operator == ( const SVObjectInfoStruct& rRhs ) const;
	bool operator == ( const SVObjectReference& rRhs ) const;

	SVObjectReference GetObjectReference() const;
	BOOL CheckExistence();

	SVObjectClass* m_pObject;					// Where I am
	SVGUID m_UniqueObjectID;					// Who I am
	SVObjectTypeInfoStruct m_ObjectTypeInfo;  // What I am
	SVObjectNameInfo m_ObjectNameInfo;
};

