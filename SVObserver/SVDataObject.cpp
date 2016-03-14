//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDataObjectClass
//* .File Name       : $Workfile:   SVDataObject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   23 Jan 2014 07:33:46  $
//******************************************************************************

#include "stdafx.h"

#include "SVDataObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVDataObjectClass
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

//******************************************************************************
// Adjustments
//******************************************************************************

IMPLEMENT_SERIAL( SVDataObjectClass, CObject, 0 )

//******************************************************************************
// Serialization:
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVDataObjectClass - Serialization
// -----------------------------------------------------------------------------
// .Description : This function provides the capability to read or write the 
//              : contents of the class SVDataObjectClass which are rather 
//				: necessary for rebuilding to a given archive.
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: CArchive&			Archive				File archive
// .Output(s)
//	: CArchive&			Archive				File archive
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:CObject::Serialize( Archive )
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	: CArchive&			Archive				File archive
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :10.06.1998 RO 			First Implementation
////////////////////////////////////////////////////////////////////////////////
void SVDataObjectClass::Serialize( CArchive& Archive )
{
    CObject::Serialize( Archive );

    if( Archive.IsStoring() )
	{
		Archive << GetSize() << objectType;

		switch( objectType )
		{
			case DWordType:
			{
				for( int i = 0; i < GetSize(); ++i )
					Archive << GetAt( i ).DWValue;
				break;
			}
		
			case DoubleType:
			{
				for( int i = 0; i < GetSize(); ++i )
					Archive << GetAt( i ).DValue;
				break;
			}
		}
	}
    else
	{
		RemoveAll();
		__int32 size;
		Archive >> size >> objectType;

		switch( objectType )
		{
			case DWordType:
			{
				DWORD dwDummy;
				for( __int32 i = 0; i < size; ++i )
				{
					Archive >> dwDummy;
					Add( dwDummy );
				}
				break;
			}
		
			case DoubleType:
			{
				double dDummy;
				for( __int32 i = 0; i < size; ++i )
				{
					Archive >> dDummy;
					Add( dDummy );
				}
				break;
			}
		}
	}
}



SVDataObjectClass::SVDataObjectClass( int Type /* = SVDataObjectClass::DWordType */ )
{
	switch( Type )
	{
		case DWordType:
		case DoubleType:
			objectType = Type;
			break;
		default:
			objectType = DWordType;
	}

	dataList.SetSize( 0, 20 );
}



SVDataObjectClass::~SVDataObjectClass()
{
	dataList.RemoveAll();
	dataList.FreeExtra();
}


BOOL SVDataObjectClass::IsDWordData()
{
	return( objectType == DWordType );
}


BOOL SVDataObjectClass::IsDoubleData()
{
	return( objectType == DoubleType );
}

int SVDataObjectClass::Add( double DVal )
{
	SVDataUnion data;
	switch( objectType )
	{
		case DWordType:
				data.DWValue = ( DWORD ) DVal;
				return static_cast<int>(dataList.Add( data ));

		case DoubleType:
				data.DValue = DVal;
				return static_cast<int>(dataList.Add( data ));
	}

	return -1;
}

int SVDataObjectClass::SetDataType( int Type )
{
	int oldType = objectType;
	switch( Type )
	{
		case DWordType:
		case DoubleType:
			objectType = Type;
			break;
		default:
			return 0;
	}
	return oldType;
}


void SVDataObjectClass::SetAt( int Index, double DVal )
{
	switch( objectType )
	{
		case DWordType:
				dataList.ElementAt( Index ).DWValue = ( DWORD ) DVal;
				break;

		case DoubleType:
				dataList.ElementAt( Index ).DValue = DVal;
				break;
	}
}


void SVDataObjectClass::SetDWordAt( int Index, DWORD DWVal )
{
	dataList.ElementAt( Index ).DWValue = DWVal;
}

void SVDataObjectClass::SetDoubleAt( int Index, double DVal )
{
	dataList.ElementAt( Index ).DValue = DVal;
}


void SVDataObjectClass::ResetAll( double DVal )
{
	switch( objectType )
	{
		case DWordType:
		{
				for( int i = 0; i < dataList.GetSize(); ++i )
					dataList.ElementAt( i ).DWValue = ( DWORD ) DVal;
				break;
		}

		case DoubleType:
		{
				for( int i = 0; i < dataList.GetSize(); ++i )
					dataList.ElementAt( i ).DValue = DVal;
				break;
		}
	}
}


void SVDataObjectClass::ResetDWordAll( DWORD DWVal )
{
	for( int i = 0; i < dataList.GetSize(); ++i )
		dataList.ElementAt( i ).DWValue = DWVal;
}

void SVDataObjectClass::ResetDoubleAll( double DVal )
{
	for( int i = 0; i < dataList.GetSize(); ++i )
		dataList.ElementAt( i ).DValue = DVal;
}



void SVDataObjectClass::RemoveAll()
{
	dataList.RemoveAll();
	dataList.SetSize( 0, 20 );
}

void SVDataObjectClass::SetSize( int NewSize, int GrowBy )
{
	dataList.SetSize( NewSize, GrowBy );
}


SVDataUnion& SVDataObjectClass::GetAt( int Index ) 
{ 
//	ASSERT( Index >= 0 && Index < dataList.GetSize() );
	return dataList.ElementAt( Index ); 
}

DWORD& SVDataObjectClass::GetDWordAt( int Index ) 
{
//	ASSERT( Index >= 0 && Index < dataList.GetSize() );
	return( dataList.ElementAt( Index ).DWValue );	
}

double& SVDataObjectClass::GetDoubleAt( int Index )
{
//	ASSERT( Index >= 0 && Index < dataList.GetSize() );
	return( dataList.ElementAt( Index ).DValue );
}

SVDataListClass& SVDataObjectClass::GetList()
{
	return dataList;
}

int SVDataObjectClass::GetSize()
{
	return static_cast<int>(dataList.GetSize());
}

