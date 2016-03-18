//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectLibrary
//* .File Name       : $Workfile:   SVObjectLibrary.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 09:42:28  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVStatusLibrary/SVStatusLibrary.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVClassRegisterListClass.h"
#include "ObjectInterfaces/ObjectDefines.h"
#pragma endregion Includes

class SVObjectClass;

//******************************************************************************
//* MACRO(S):
//******************************************************************************

// Creates new string using malloc and fills it with the GUID, use free() to deallocate the string memory...
#define SV_GUID_TO_NEW_STRING( XGUID, XLPTSTR )	\
 		{ \
			if( ( XLPTSTR ) = ( LPTSTR ) malloc( sizeof( TCHAR ) * 256 ) )	\
			{	\
				_stprintf( ( XLPTSTR ), "{ 0x%x, 0x%x, 0x%x, { 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x } }", \
				( XGUID ).Data1, ( XGUID ).Data2, ( XGUID ).Data3, \
				( XGUID ).Data4[0], ( XGUID ).Data4[1], ( XGUID ).Data4[2], ( XGUID ).Data4[3], \
				( XGUID ).Data4[4], ( XGUID ).Data4[5], ( XGUID ).Data4[6], ( XGUID ).Data4[7] );	\
			}	\
		}

// GUID String
// XLPSZSTRING must be a LPTSTR !!!
#define SV_GUID_TO_STRING( XGUID, XLPSZSTRING, XLENGTH ) \
		{	\
			if( ( XLPSZSTRING ) && ( XLENGTH ) > 0 ) \
			{ \
				TCHAR szBuf[ 100 ]; \
				_stprintf( szBuf, "{%x-%x-%x-%x %x %x %x %x %x %x %x}", \
						   ( XGUID ).Data1, ( XGUID ).Data2, ( XGUID ).Data3, \
						   ( XGUID ).Data4[0], ( XGUID ).Data4[1], ( XGUID ).Data4[2], ( XGUID ).Data4[3], \
						   ( XGUID ).Data4[4], ( XGUID ).Data4[5], ( XGUID ).Data4[6], ( XGUID ).Data4[7] ); \
				_tcsncpy( XLPSZSTRING, szBuf, min( ( XLENGTH ), 100 ) - 1 ); \
			} \
		}

// Should be in the class definition...
#define SV_DECLARE_CLASS( XCLASSNAME )					\
		public:	\
			virtual SVGUID GetClassID(); \
			virtual LPCTSTR GetClassName();


// Should be in the implementation file... ( .cpp )
#define SV_IMPLEMENT_CLASS( XCLASSNAME, XCLASSID )	\
		class XCLASSNAME##RegisterClass : public SVClassRegisterClass \
		{ \
			public: \
				XCLASSNAME##RegisterClass() \
				{ \
				SVClassRegisterListClass::Instance().Add( this ); \
					OutputDebugString( "SV_IMPLEMENT: " #XCLASSNAME ", " ); \
					OutputDebugString( SVGUID( XCLASSID ).ToString().c_str() ); \
					OutputDebugString( _T( "\n" ) ); \
				} \
				virtual SVGUID GetClassID() { return XCLASSID; } \
				virtual SVObjectClass* Construct() { return new ( XCLASSNAME ); } \
				virtual LPCTSTR GetClassName()	{ return #XCLASSNAME; } \
		};	\
		XCLASSNAME##RegisterClass The##XCLASSNAME##Register; \
		SVGUID XCLASSNAME::GetClassID() { return XCLASSID; } \
		LPCTSTR XCLASSNAME::GetClassName() { return #XCLASSNAME; }


//******************************************************************************
//* CONSTANT(S):
//******************************************************************************
struct SVObjectInfoStruct;
struct SVObjectTypeInfoStruct;

extern const SVObjectInfoStruct		SVInvalidObjectInfo;
extern const SVObjectTypeInfoStruct	SVInvalidObjectTypeInfo;

// GUID for the SVObjectImageClass
extern /*const*/ GUID SVObjectImageClassGuid;

//******************************************************************************
//* TYPE(S):
//******************************************************************************

/////////////////////////////////////////////////////////////////////////////
// SVHINDEX
typedef UINT SVHINDEX;
	// Not Valid Handle:	NULL
	// First Valid Handle:	0x1

/////////////////////////////////////////////////////////////////////////////
// SVProductCountType
typedef DWORD SVProductCountType;
	// Not Valid Product:	NULL
	// First Valid Product: 0x1



//******************************************************************************
//* FUNCTION DECLARATION(S):
//******************************************************************************

DWORD_PTR SVSendMessage( SVObjectClass* PObject, DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );
DWORD_PTR SVSendMessage( const GUID& RUniqueObjectID, DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

