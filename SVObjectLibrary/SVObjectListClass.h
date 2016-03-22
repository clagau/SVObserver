//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectListClass
//* .File Name       : $Workfile:   SVObjectListClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:57:24  $
//******************************************************************************

#pragma once

#include "SVContainerLibrary/SVVector.h"
#include "SVOMFCLibrary/SVTemplate.h"
#include "SVObjectInfoStruct.h"
#include "SVObjectScriptUsage.h"

class SVObjectAttributeClass;
class SVObjectClass;

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVObjectListClass
// -----------------------------------------------------------------------------
// .Description : Contains pointer to SVObjectClass items, but doesn't owns this
//				: SVObjectClass items. That means it doesn't construct
//				: or destruct the items! 
////////////////////////////////////////////////////////////////////////////////
class SVObjectListClass : public SVVector< SVObjectReference >
{
public:
	SVObjectListClass();
	virtual ~SVObjectListClass();

	BOOL InitObjectAt( int Index, SVObjectClass* PObject );
	void ResetNameAt( int Index );

	void SetNameAt( int Index, const CString& STRString );
	void SetObjectOwnerAt( int Index, SVObjectClass* PNewOwner );

	CString GetNameAt( int Index );
	int		GetNameLengthAt( int Index );

	CString GetObjectNameAt( int Index );
	int		GetObjectNameLengthAt( int Index );

	const SVObjectInfoStruct& GetObjectOwnerAt( int Index );

	CString GetCompleteObjectNameAt( int Index );
#ifdef USE_OBJECT_SCRIPT
	void	GetObjectScriptAt( int Index, CString& RStrScript, CString& RStrAliasTable );
#endif
	int		GetResourceIDAt( int Index );
	const GUID&	GetUniqueObjectIDAt( int Index ) const;

};

