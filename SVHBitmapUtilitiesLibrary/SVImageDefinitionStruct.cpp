// ******************************************************************************
// * COPYRIGHT (c) 2008 by Seidenader Vistion Inc., Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVImageDefinitionStruct
// * .File Name       : $Workfile:   SVImageDefinitionStruct.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 10:11:08  $
// ******************************************************************************

#include "stdafx.h"
#include "SVImageDefinitionStruct.h"


// SVImageDefinitionStruct
SVImageDefinitionStruct::SVImageDefinitionStruct( )
{
	Init();
}


SVImageDefinitionStruct::SVImageDefinitionStruct(const SVImageDefinitionStruct& rhs )
{
	Init();
	*this = rhs;
}

SVImageDefinitionStruct::~SVImageDefinitionStruct( )
{
	Clear();
}

const SVImageDefinitionStruct& SVImageDefinitionStruct::operator = (const SVImageDefinitionStruct& rhs )
{

	if( this != &rhs )
	{
		Clear();
		lHeight = rhs.lHeight;
		lWidth = rhs.lWidth;
		eImageFormat = rhs.eImageFormat;
		bstrDisplayName = SysAllocString(rhs.bstrDisplayName);
	}
	return *this;
}

void SVImageDefinitionStruct::Clear( )
{
	lHeight = 0;
	lWidth = 0;
	eImageFormat = SVImageFormatUnknown;
	if(bstrDisplayName)
		::SysFreeString(bstrDisplayName);

}

void SVImageDefinitionStruct::Init( )
{
	lHeight = 0;
	lWidth = 0;
	eImageFormat = SVImageFormatUnknown;
	bstrDisplayName = NULL;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVHBitmapUtilitiesLibrary\SVImageDefinitionStruct.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:11:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   03 Mar 2010 11:04:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  682
 * SCR Title:  Upgrade SVObserver version for 5.31 release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated PVCS header infromation.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/