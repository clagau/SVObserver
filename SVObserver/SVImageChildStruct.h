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

#ifndef SVIMAGECHILDSTRUCT_H
#define SVIMAGECHILDSTRUCT_H

#include <map>
#include "SVImageLibrary/SVImageInfoClass.h"
#include "SVUtilityLibrary/SVUtilityGlobals.h"
#include "SVImageObjectClass.h"

struct SVImageChildStruct  
{
	SVImageChildStruct();
	SVImageChildStruct( const SVImageChildStruct &p_rsvValue );
	virtual ~SVImageChildStruct();

	const SVImageChildStruct &operator=( const SVImageChildStruct &p_rsvValue );

	SVImageInfoClass m_ImageInfo;
	SVImageObjectClassPtr m_pImageHandles;

};

typedef std::map< GUID, SVImageChildStruct > SVGuidImageChildMap;

#endif

