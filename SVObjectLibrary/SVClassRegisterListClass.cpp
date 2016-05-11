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
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVClassRegisterListClass& SVClassRegisterListClass::Instance()
{
	static SVClassRegisterListClass l_Object;

	return l_Object;
}

SVClassRegisterListClass::SVClassRegisterListClass()
{
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
SVObjectClass* SVClassRegisterListClass::ConstructNewObject( const SVGUID& RUniqueClassID )
{
	SVObjectClass* l_pObject = nullptr;

	SVRegisterClasses::const_iterator l_Iter = m_Registers.find( RUniqueClassID );

	if( l_Iter != m_Registers.end() )
	{
		if( nullptr != l_Iter->second )
		{
			l_pObject = l_Iter->second->Construct();
		}
	}

	return l_pObject;
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
				CString tmp;
				SVGUID guid = pClass->GetClassID();

				tmp.Format( "Duplicate Class encountered\n ClassName: %s\n ClassID: %s", pClass->GetClassName(), guid.ToString().c_str() );

				ASSERT(false);//( tmp );
			}
		#endif
	}
	#ifdef _DEBUG
	else
	{
		ASSERT(false);//("Invalid Class encountered\n");
	}
	#endif
}

void SVClassRegisterListClass::Shutdown()
{
	m_Registers.clear();
}

