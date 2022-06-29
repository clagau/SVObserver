//******************************************************************************
//* COPYRIGHT (c) 2005 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInspectionProcessResetStruct
//* .File Name       : $Workfile:   SVInspectionProcessResetStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:02:48  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVObjectLibrary/SVObjectClass.h"
#include "ObjectInterfaces/IValueObject.h"
#pragma endregion Includes

namespace SvTo
{
class SVToolClass;
}

struct SVInspectionProcessResetStruct
{
	SVInspectionProcessResetStruct();
	SVInspectionProcessResetStruct( const SVInspectionProcessResetStruct &rObject );

	const SVInspectionProcessResetStruct &operator=( const SVInspectionProcessResetStruct &rObject );

	SvOi::SVResetItemEnum SetResetData( SvOi::SVResetItemEnum eResetItem, SVObjectClass *pObject );

	SvOi::SVResetItemEnum m_eResetItem;

	SVObjectPtrSet m_ObjectSet;
};

typedef std::map<SvTo::SVToolClass*, SVInspectionProcessResetStruct> SVStdMapSVToolClassPtrSVInspectionProcessResetStruct;

