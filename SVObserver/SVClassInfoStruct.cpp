//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVClassInfoStruct
//* .File Name       : $Workfile:   SVClassInfoStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:53:26  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVClassInfoStruct.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVTaskObject.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVInterfaceListClass
////////////////////////////////////////////////////////////////////////////////
SVInterfaceListClass::SVInterfaceListClass()
{
}

SVInterfaceListClass::SVInterfaceListClass( const SVInterfaceListClass& RIn )
{
	Copy( RIn );
}

SVInterfaceListClass::~SVInterfaceListClass()
{
}

SVInterfaceListClass SVInterfaceListClass::operator=( const SVInterfaceListClass& RIn )
{
	Copy( RIn );
	return( *this );
}


////////////////////////////////////////////////////////////////////////////////
// .Title       : SVClassInfoStruct
////////////////////////////////////////////////////////////////////////////////
SVClassInfoStruct::SVClassInfoStruct()
{
	ClassId = SVInvalidGUID;
}

SVClassInfoStruct::SVClassInfoStruct( const SVClassInfoStruct& RClassInfo )
{
	ClassId					= RClassInfo.ClassId;
	ClassName				= RClassInfo.ClassName;
	ObjectTypeInfo			= RClassInfo.ObjectTypeInfo;
	DesiredInputInterface	= RClassInfo.DesiredInputInterface;
}

SVClassInfoStruct::~SVClassInfoStruct()
{
}

SVClassInfoStruct SVClassInfoStruct::operator=( const SVClassInfoStruct& RClassInfo )
{
	ClassId					= RClassInfo.ClassId;
	ClassName				= RClassInfo.ClassName;
	ObjectTypeInfo			= RClassInfo.ObjectTypeInfo;
	DesiredInputInterface	= RClassInfo.DesiredInputInterface;
	return( *this );
}


SVObjectClass* SVClassInfoStruct::Construct()
{
	SVObjectClass* pObject = nullptr;

	SVObjectManagerClass::Instance().ConstructObject( ClassId, pObject );

	if( pObject )
	{
		pObject->SetName( ClassName );

		if( SVTaskObjectClass* pTaskObject = dynamic_cast<SVTaskObjectClass*>(pObject) )
		{
			// Get Input Interface...
			SVInputInfoListClass inputInterface;
			pTaskObject->GetPrivateInputList( inputInterface );

			// Apply desired input interface...
			for( int i = 0; i < DesiredInputInterface.GetSize() && i < inputInterface.GetSize(); ++ i )
			{
				SVInObjectInfoStruct* pInInfo			= inputInterface[ i ];

				SVObjectTypeInfoStruct& rDesiredInType	= DesiredInputInterface[ i ];

				if( pInInfo )
					pInInfo->SetInputObjectType( rDesiredInType );

				SVObjectTypeInfoStruct info = pInInfo->GetInputObjectInfo().ObjectTypeInfo;

				ASSERT( pInInfo->GetInputObjectInfo().UniqueObjectID != SVInvalidGUID ||
				        info.EmbeddedID != SVInvalidGUID || 
				        info.ObjectType != SVNotSetObjectType ||
				        info.SubType != SVNotSetSubObjectType );
			}
		}
	}
	return pObject;
}

SVClassInfoStructListClass::SVClassInfoStructListClass()
{
}

SVClassInfoStructListClass::~SVClassInfoStructListClass() 
{
	RemoveAll();
}

int SVClassInfoStructListClass::Find( const GUID& ClassID ) 
{ 
	for( int i = 0; i < GetSize(); i++ )
	{
		if( !memcmp( &ClassID, &GetAt( i ).ClassId, sizeof( GUID ) ) )
			return i;
	}
	return -1;
};

