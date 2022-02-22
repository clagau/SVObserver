//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileNameValueObjectClass
//* .File Name       : $Workfile:   SVFileNameValueObjectClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   01 Oct 2013 14:19:28  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <assert.h>
#include "SVFileNameValueObjectClass.h"
#include "SVFileSystemLibrary/SVFileNameManagerClass.h"
#pragma endregion Includes

namespace SvVol
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
constexpr int c_PathMaxSize = 256;
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(SVFileNameValueObjectClass, SvPb::FileNameValueClassId);

SVFileNameValueObjectClass::SVFileNameValueObjectClass( LPCTSTR ObjectName )
: SVStringValueObjectClass( ObjectName )
{
	LocalInitialize();
}

SVFileNameValueObjectClass::SVFileNameValueObjectClass(	SVObjectClass* pOwner, 	int StringResourceID )
: SVStringValueObjectClass( pOwner, StringResourceID )
{
	LocalInitialize();
}

SVFileNameValueObjectClass::SVFileNameValueObjectClass( const SVFileNameValueObjectClass& rhs)
: SVStringValueObjectClass()
{
	LocalInitialize();
	*this = rhs;
}

SVFileNameValueObjectClass& SVFileNameValueObjectClass::operator = ( const SVFileNameValueObjectClass& rhs )
{
	__super::operator = (rhs);
	return *this;
}

SVFileNameValueObjectClass::~SVFileNameValueObjectClass()
{
	SVFileNameManagerClass::Instance().RemoveItem(&m_FileName);
}

HRESULT SVFileNameValueObjectClass::SetValue( const std::string& rValue, int Index /*= -1*/ )
{
	Index = isArray() ? Index : 0;
	HRESULT Result = ValidateIndex(Index);

	if ( S_OK == Result || SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE == Result )
	{
		m_FileName.SetFullFileName( rValue.c_str() );
		SVFileNameManagerClass::Instance().LoadItem( &m_FileName );
		Result = __super::SetValue( m_FileName.GetFullFileName(), Index );
	}
	
	return Result;
}

HRESULT SVFileNameValueObjectClass::SetDefaultValue( const std::string& rValue, bool bResetAll )
{
	m_FileName.SetDefaultFullFileName( rValue.c_str() );
	
	return __super::SetDefaultValue( rValue, bResetAll );
}

void SVFileNameValueObjectClass::LocalInitialize()
{
	m_ObjectTypeInfo.m_SubType = SvPb::SVStringValueObjectType;
	
	SetObjectAttributesAllowed( SvPb::viewable | SvPb::publishable | SvPb::archivable | SvPb::embedable | SvPb::audittrail | SvPb::dataDefinitionValue, SvOi::SetAttributeType::OverwriteAttribute );

	SetMaxByteSize(c_PathMaxSize);

	SetTypeName( _T("FileName") );
	
	init();

	SVFileNameManagerClass::Instance().AddItem(&m_FileName);
}

} //namespace SvVol
