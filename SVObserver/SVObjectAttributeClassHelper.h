//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectAttributeClassHelper
//* .File Name       : $Workfile:   SVObjectAttributeClassHelper.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:40:04  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <vector>
#include "SVContainerLibrary\SVObjectArrayClassTemplate.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"

class SVObjectAttributeClassHelper
{
public:
	template <typename T>
	static BOOL GetArrayData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector<T>& raData, T defaultValue)
	{
		return pDataObject->GetArrayData(szName, raData, defaultValue);
	}

	template <typename T>
	static BOOL GetAttributeData(SVObjectAttributeClass* pDataObject, LPCTSTR szName, std::vector< std::vector<T> >& raaData, T defaultValue)
	{
		return pDataObject->GetAttributeData(szName, raaData, defaultValue);
	}

	template <typename T>
	static BOOL GetAttributeData(SVObjectAttributeClass* pDataObject ,LPCTSTR szName, SvCl::SVObjectArrayClassTemplate<T>& svData)
	{
		return pDataObject->GetAttributeData(szName, svData);
	}
};

