//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPublicAttributeEntryStruct
//* .File Name       : $Workfile:   SVPublicAttributeEntryStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 17:25:06  $
//******************************************************************************

#ifndef SVPUBLICATTRIBUTEENTRYSTRUCT_H
#define SVPUBLICATTRIBUTEENTRYSTRUCT_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

struct SVPublicAttributeEntryStruct
{
	SVPublicAttributeEntryStruct()
	{
		BIsPublic			= FALSE;
		BIsOpened			= FALSE;
		BIsActivatible		= TRUE;
	};

	SVPublicAttributeEntryStruct( SVPublicAttributeEntryStruct& ROriginal )
	{
		BIsPublic			= ROriginal.BIsPublic;
		BIsOpened			= ROriginal.BIsOpened;
		BIsActivatible		= ROriginal.BIsActivatible;
	};

	SVPublicAttributeEntryStruct operator=( SVPublicAttributeEntryStruct& ROriginal )
	{
		BIsPublic			= ROriginal.BIsPublic;
		BIsOpened			= ROriginal.BIsOpened;
		BIsActivatible		= ROriginal.BIsActivatible;

		return( *this );
	};

	BOOL			BIsPublic;
	BOOL			BIsOpened;
	BOOL			BIsActivatible;
};

////////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// DevStudio inserts additional declarations immediate in front of the preceding line
////////////////////////////////////////////////////////////////////////////////

#endif	//	SVPUBLICATTRIBUTEENTRYSTRUCT_H

