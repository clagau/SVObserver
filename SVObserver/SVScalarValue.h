//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVScalarValue
//* .File Name       : $Workfile:   SVScalarValue.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:02:34  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVObjectLibrary/SVObjectReference.h"
#pragma endregion Includes

class SVValueObjectClass;

class SVScalarValue
{
public:
	CString            strName;
	CString            strValue;
	CString            strType;
	SVObjectReference  object;
	unsigned long      status;

	SVScalarValue() : status(0) {}
	explicit SVScalarValue( const SVObjectReference& rhs ) : status(0) { object = rhs; strName = object.GetCompleteObjectName(); }
	SVScalarValue(const CString& p_rstrName, const CString& p_rstrValue, const SVObjectReference& p_Object = SVObjectReference(), const CString& p_rstrType=CString())
		: strName(p_rstrName), strValue(p_rstrValue), strType(p_rstrType), object(p_Object), status(0) {}
	void clear() { strName.Empty(); strValue.Empty(); strType.Empty(); object=SVObjectReference(); status=0; }
};

typedef std::map<CString, SVScalarValue>  SVScalarValueMapType;
typedef std::vector<SVScalarValue>        SVScalarValueVector;

