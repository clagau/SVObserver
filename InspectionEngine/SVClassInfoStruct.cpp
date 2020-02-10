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
#include "ObjectInterfaces/ITaskObject.h"
#pragma endregion Includes

namespace SvIe
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVClassInfoStruct
////////////////////////////////////////////////////////////////////////////////
SVObjectClass* SVClassInfoStruct::Construct()
{
	SVObjectClass* pObject = nullptr;

	SVObjectManagerClass::Instance().ConstructObject( m_ClassId, pObject );

	if( nullptr != pObject )
	{
		pObject->SetName( m_ClassName.c_str() );

		if( SvOi::ITaskObject* pTaskObject = dynamic_cast<SvOi::ITaskObject*>(pObject) )
		{
			pTaskObject->ResolveDesiredInputs(m_DesiredInputVector);
		}
	}
	return pObject;
}

} //namespace SvIe
