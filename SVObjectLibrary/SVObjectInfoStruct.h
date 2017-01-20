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
	SVObjectInfoStruct( const SVObjectInfoStruct& p_rsvObject );
	SVObjectInfoStruct( SVObjectClass* p_psvObject );
	SVObjectInfoStruct( SVObjectReference p_svObject );

	virtual ~SVObjectInfoStruct();

	const SVObjectInfoStruct& operator = ( const SVObjectInfoStruct& O2 );

	void clear();

	void ClearObjectInfo();

	HRESULT SetObject( const SVGUID& p_rObjectID );
	HRESULT SetObject( const SVString& p_rName );
	HRESULT SetObject( const SVObjectTypeInfoStruct& p_rTypeInfo );
	HRESULT SetObject( SVObjectClass* p_psvObject );
	HRESULT SetObject( const SVObjectReference& p_rObject );
	HRESULT SetObject( const SVObjectInfoStruct& p_rObject );

	bool operator == ( const SVObjectInfoStruct& rhs ) const;
	bool operator == ( const SVObjectReference& rhs ) const;

	SVObjectReference GetObjectReference() const;
	BOOL CheckExistence();

	SVObjectClass* PObject;					// Where I am
	SVGUID UniqueObjectID;					// Who I am
	SVObjectTypeInfoStruct ObjectTypeInfo;  // What I am
	SVObjectNameInfo m_ObjectNameInfo;
};

