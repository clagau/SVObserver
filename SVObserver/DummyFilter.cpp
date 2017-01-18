//*****************************************************************************
/// \copyright COPYRIGHT (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file DummyFilter.cpp
/// All Rights Reserved
//*****************************************************************************
  
/// The class DummyFilter is not meant to be used.
/// This file only serves to ensure that the object files in the 
/// project "Filter Operators" are used
/// It might be better to ensure linkage by using the option "Force Symbol References"
/// in the librarian in the project properties or by creating a FilterOperators DLL 
/// rather than a static library but these options were not tried because of time constraints
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "FilterOperators/SVFilterClass.h"

#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


GUID DummyFilterClassGuid = { 0xadd53485, 0x3f05, 0x4cef, { 0x8f, 0x2, 0x3, 0x77, 0x14, 0x2f, 0xa1, 0x2e } };


class DummyFilterClass : public SVFilterClass
{
	SV_DECLARE_CLASS( DummyFilterClass );
};


SV_IMPLEMENT_CLASS( DummyFilterClass, DummyFilterClassGuid )

