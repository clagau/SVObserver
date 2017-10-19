//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInputObject
//* .File Name       : $Workfile:   SVInputObject.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:59:36  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
#include "SVObjectLibrary/SVObjectClass.h"
#pragma endregion Includes

class SVInputObject : public SVObjectClass
{
public:
	SVInputObject( LPCSTR strObjectName );
	SVInputObject( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVINPUTOBJECT );

	virtual ~SVInputObject();

	virtual bool Create();
	virtual bool Destroy();

	virtual HRESULT Read( _variant_t& p_rValue ) = 0;
};

typedef std::map< SVGUID, SVInputObject* > SVGuidSVInputObjectPtrMap;

