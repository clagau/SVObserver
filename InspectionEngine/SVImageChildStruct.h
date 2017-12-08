//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageChildStruct
//* .File Name       : $Workfile:   SVImageChildStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:48:16  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
#include "SVImageLibrary/SVImageInfoClass.h"
#include "SVUtilityLibrary/SVUtilityGlobals.h"
#include "SVImageObjectClass.h"
#pragma endregion Includes

struct SVImageChildStruct  
{
	SVImageChildStruct();
	SVImageChildStruct( const SVImageChildStruct &p_rsvValue );
	virtual ~SVImageChildStruct();

	const SVImageChildStruct &operator=( const SVImageChildStruct &p_rsvValue );

	SVImageInfoClass m_ImageInfo;
	SVImageObjectClassPtr m_pImageHandles;
};

typedef std::map< SVGUID, SVImageChildStruct > SVGuidImageChildMap;

