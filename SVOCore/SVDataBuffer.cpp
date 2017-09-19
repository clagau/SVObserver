//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDataBuffer
//* .File Name       : $Workfile:   SVDataBuffer.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 11:07:30  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVImageProcessingClass.h"
#include "SVDataBuffer.h"
#include "SVObjectLibrary/SVClsIds.h"
#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVDataBufferInfoClass::SVDataBufferInfoClass() : 
 pOwnerTool( nullptr )
,Length(0L)
,Type(0L)
{
}

SVDataBufferInfoClass::SVDataBufferInfoClass( const SVDataBufferInfoClass& rRhs )
{
	pOwnerTool	= rRhs.pOwnerTool;
	Length		= rRhs.Length;
	Type		= rRhs.Type;
	HBuffer		= rRhs.HBuffer;
}

SVDataBufferInfoClass SVDataBufferInfoClass::operator=( SVDataBufferInfoClass& rRhs )
{
	if( &rRhs != this )
	{
		pOwnerTool	= rRhs.pOwnerTool;
		Length		= rRhs.Length;
		Type		= rRhs.Type;
		HBuffer		= rRhs.HBuffer;
	}

	return ( *this );
}

SV_IMPLEMENT_CLASS( SVDataBufferClass, SVDataBufferClassGuid );

SVDataBufferClass::SVDataBufferClass( SVObjectClass* pOwner, int StringResourceID )
: SVObjectAppClass( pOwner, StringResourceID )
{
	init();
}

void SVDataBufferClass::init()
{
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVDataBufferClassObjectType;
}

SVDataBufferClass::~SVDataBufferClass()
{
	CloseObject();
}

bool SVDataBufferClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = false;

	if( SVObjectAppClass::CreateObject(rCreateStructure) )
	{
		if( m_DataInfo.Type != 0 && 0 < m_DataInfo.Length )
		{
			m_DataInfo.pOwnerTool = GetTool();
			// Create data buffer...
			bOk = S_OK == SVImageProcessingClass::CreateDataBuffer(&m_DataInfo);
		}
	}
	m_isCreated = bOk;

	return bOk;
}

bool SVDataBufferClass::CloseObject()
{
	bool rc = SVObjectAppClass::CloseObject();

	rc = (S_OK == SVImageProcessingClass::DestroyDataBuffer( &m_DataInfo ) ) && rc;

	return rc;
}

bool SVDataBufferClass::Resize( int NewLength )
{
	return S_OK == SVImageProcessingClass::ResizeDataBuffer( &m_DataInfo, NewLength );
}

SVDataBufferInfoClass& SVDataBufferClass::GetDataBufferInfo()
{
	return m_DataInfo;
}

SVDataBufferHandleStruct& SVDataBufferClass::GetDataBufferHandle()
{
	return m_DataInfo.HBuffer;
}

