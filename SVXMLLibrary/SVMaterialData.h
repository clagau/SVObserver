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

#include <comdef.h>
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
	SVMaterialData( const SVMaterialData& p_rObject );
	SVMaterialData( const VARIANT& p_rVariant );

	virtual ~SVMaterialData();

	bool empty() const;

	void clear();

	operator const _variant_t & () const;

	const SVMaterialData& operator=( const SVMaterialData& p_rObject );
	const SVMaterialData& operator=( const VARIANT& p_rVariant );

  bool operator==( const SVMaterialData& p_rObject ) const;

  bool operator!=( const SVMaterialData& p_rObject ) const;

protected:
	_variant_t m_Variant;

};

typedef SVSharedPtr< SVMaterialData > SVMaterialDataPtr;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMaterialsLibrary\SVMaterialData.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:14:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   18 Sep 2012 18:00:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed redundent casting operator.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Nov 2008 07:23:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

