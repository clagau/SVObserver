//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRangeClassHelper
//* .File Name       : $Workfile:   RangeClassHelper.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   19 Jan 2015 11:40:44  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "RangeClassHelper.h"
#include "SVMessage\SVMessage.h"
#include "SVObjectLibrary\SVObjectClass.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#pragma endregion Includes

#pragma region Constructor
RangeClassHelper::RangeClassHelper()
{
}

RangeClassHelper::~RangeClassHelper()
{
}
#pragma endregion Constructor

#pragma region Public Methods
bool RangeClassHelper::IsOwnedByRangeObject(const SVObjectClass& rObject)
{
	bool result = false;

	if(rObject.GetParent())
	{
		if(SvPb::SVRangeObjectType == rObject.GetParent()->GetObjectType())
		{
			result = true;
		}
	}

	return result;
}

bool RangeClassHelper::IsAllowedToSet(const SVObjectClass& ObjectRef, const std::string& rValue, bool bOnline, HRESULT& hres)
{
	if(!IsOwnedByRangeObject(ObjectRef))
	{
		return true;
	}

	bool res = true;
	bool IsReference = false;
	SvPb::EmbeddedIdEnum embeddedID = ObjectRef.GetEmbeddedID();
	if (embeddedID == SvPb::RangeClassFailHighIndirectEId ||
		embeddedID == SvPb::RangeClassFailLowIndirectEId ||
		embeddedID == SvPb::RangeClassWarnHighIndirectEId ||
		embeddedID == SvPb::RangeClassWarnLowIndirectEId)
	{
		IsReference = true;
	}

	if(IsReference)
	{
		if(bOnline)
		{
			res = false;
			hres = SVMSG_OBJECT_CANNOT_BE_SET_WHILE_ONLINE;
		}
		else
		{
			SVObjectReference ObjectRefValue;
			HRESULT hrFind = SVObjectManagerClass::Instance().GetObjectByDottedName( rValue.c_str(), ObjectRefValue );

			if( !rValue.empty() && ( S_OK != hrFind || nullptr == ObjectRefValue.getObject() ) )
			{
				//a not empty string and no reference
				hres = SVMSG_OBJECT_CANNOT_BE_SET_INVALID_REFERENCE;
				res = false;
			}
		}
	}

	return res;
}
#pragma endregion Public Methods

#pragma region Private Methods
#pragma endregion Private Methods

