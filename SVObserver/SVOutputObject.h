//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOutputObject
//* .File Name       : $Workfile:   SVOutputObject.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:15:50  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
#include "SVObjectLibrary/SVObjectClass.h"
#pragma endregion Includes

class SVOutputObject : public SVObjectClass
{
public:
	SVOutputObject( LPCSTR strObjectName );
	SVOutputObject( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVOUTPUTOBJECT );

	virtual ~SVOutputObject();

	virtual BOOL Create();
	virtual BOOL Destroy();

	virtual HRESULT Write( const _variant_t& rValue ) = 0;
	virtual HRESULT Reset() = 0;
	virtual bool IsCombined() const = 0;
	virtual bool GetCombinedValue() const = 0;
};

typedef SVVector< SVOutputObject* > SVOutputObjectPtrVector;
typedef std::map< SVGUID, SVOutputObject* > SVGuidSVOutputObjectPtrMap;

