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

//Moved to precompiled header: #include <map>
#include "SVObjectLibrary/SVObjectClass.h"

class SVOutputObject : public SVObjectClass
{
public:
	SVOutputObject( LPCSTR strObjectName );
	SVOutputObject( SVObjectClass* POwner = NULL,
					int StringResourceID = IDS_CLASSNAME_SVOUTPUTOBJECT );

	virtual ~SVOutputObject();

	virtual BOOL Create();
	virtual BOOL Destroy();
	virtual BOOL IsCreated();

	virtual HRESULT Write( const _variant_t& p_rValue ) = 0;
	virtual HRESULT Reset() = 0;

	BOOL	m_bCreated;

};

typedef SVVector< SVOutputObject* > SVOutputObjectArray;
typedef std::map< SVGUID, SVOutputObject* > SVGuidSVOutputObjectPtrMap;

