//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMaterialData
//* .File Name       : $Workfile:   SVMaterialData.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:14:28  $
//******************************************************************************

#pragma once

#include <comutil.h>
#include "SVUtilityLibrary/SVSharedPtr.h"

/**
@SVObjectName Material Data

@SVObjectOverview

@SVObjectOperations

*/
class SVMaterialData
{
public:
	SVMaterialData();
	SVMaterialData( const SVMaterialData& rObject );
	SVMaterialData( const _variant_t& rVariant );

	virtual ~SVMaterialData();

	bool empty() const;

	void clear();

	operator const _variant_t & () const;

	const SVMaterialData& operator=( const SVMaterialData& rObject );
	const SVMaterialData& operator=( const _variant_t& rVariant );

  bool operator==( const SVMaterialData& rObject ) const;

  bool operator!=( const SVMaterialData& rObject ) const;

protected:
	_variant_t m_Variant;

};

typedef SVSharedPtr< SVMaterialData > SVMaterialDataPtr;

