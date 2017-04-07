//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVClassRegisterListClass
//* .File Name       : $Workfile:   SVClassRegisterListClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:35:30  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVClassRegisterListClass.h"
#include "SVClassRegisterClass.h"
#include "SVObjectClass.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVClsids.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SVClassRegisterListClass& SVClassRegisterListClass::Instance()
{
	static SVClassRegisterListClass Object;

	return Object;
}

SVClassRegisterListClass::SVClassRegisterListClass()
{
	m_ExchangeClassID = boost::assign::map_list_of<SVGUID, SVGUID>
		(SVGUID(SVStaticStringValueObjectClassGuid), SVGUID(SVStringValueObjectClassGuid)).convert_to_container<ExchangeClassMap>();
}

SVClassRegisterListClass::~SVClassRegisterListClass()
{
	Shutdown();
}

// Construct class by ClassID, 
// returns nullptr, if this class type is not available!
// Otherwise, it returns a SVObjectClass* to the new object.
// You have to destroy this new object by yourself by using delete
//
SVObjectClass* SVClassRegisterListClass::ConstructNewObject( const SVGUID& rUniqueClassID )
{
	SVObjectClass* pResult( nullptr );

	SVGUID ClassID( SVStaticStringValueObjectClassGuid );
	if( rUniqueClassID == ClassID )
	{
		ExchangeClassMap::const_iterator IterExchange( m_ExchangeClassID.find( ClassID ) );
		if( m_ExchangeClassID.end() != IterExchange )
		{
			ClassID = IterExchange->second;
		}
	}
	else
	{
		ClassID = rUniqueClassID;
	}

	SVRegisterClasses::const_iterator Iter = m_Registers.find( ClassID );

	if( m_Registers.end() != Iter )
	{
		if( nullptr != Iter->second )
		{
			pResult = Iter->second->Construct();
		}
	}

	return pResult;
}

void SVClassRegisterListClass::Add( SVClassRegisterClass* pClass )
{
	if( nullptr != pClass )
	{
		SVRegisterClasses::const_iterator l_Iter = m_Registers.find( pClass->GetClassID() );

		if( l_Iter == m_Registers.end() )
		{
			m_Registers[ pClass->GetClassID() ] = pClass;
		}
		#ifdef _DEBUG
			else
			{
				SVString Temp;
				SVGUID guid = pClass->GetClassID();

				Temp = SvUl_SF::Format( _T("Duplicate Class encountered\n ClassName: %s\n ClassID: %s"), pClass->GetClassName(), guid.ToString().c_str() );
				::OutputDebugString( Temp.c_str() );
				assert(false);
			}
		#endif
	}
	#ifdef _DEBUG
	else
	{
		assert(false);//("Invalid Class encountered\n");
	}
	#endif
}

void SVClassRegisterListClass::Shutdown()
{
	m_Registers.clear();
}

