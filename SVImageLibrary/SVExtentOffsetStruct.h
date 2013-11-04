//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExtentOffsetStruct
//* .File Name       : $Workfile:   SVExtentOffsetStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:40:00  $
//******************************************************************************

#ifndef _SVEXTENTOFFSETSTRUCT_H
#define _SVEXTENTOFFSETSTRUCT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVExtentPointStruct.h"

class SVImageClass;

struct SVExtentOffsetStruct
{
	SVExtentOffsetStruct()
	{
		Initialize();
	};

	SVExtentOffsetStruct( const SVExtentOffsetStruct& p_rsvData )
	{
		*this = p_rsvData;
	};

	const SVExtentOffsetStruct& operator=( const SVExtentOffsetStruct& p_rsvData )
	{
		m_psvRootImage = p_rsvData.m_psvRootImage;
		m_psvImage = p_rsvData.m_psvImage;

		m_csImageName = p_rsvData.m_csImageName;

		m_bIsLinear = p_rsvData.m_bIsLinear;
		m_bAlwaysUpdate = p_rsvData.m_bAlwaysUpdate;

		m_svRotationPoint = p_rsvData.m_svRotationPoint;
		m_svOffset = p_rsvData.m_svOffset;

		m_dRotationAngle = p_rsvData.m_dRotationAngle;

		return *this;
	};

	HRESULT Initialize()
	{
		m_psvRootImage = NULL;
		m_psvImage = NULL;

		m_csImageName.Empty();

		m_bIsLinear = true;
		m_bAlwaysUpdate = false;

		m_svRotationPoint.Initialize();
		m_svOffset.Initialize();

		m_dRotationAngle = 0.0;

		return S_OK;
	};

	SVImageClass *m_psvRootImage;
	SVImageClass *m_psvImage;

	CString m_csImageName;

	bool m_bIsLinear;
	bool m_bAlwaysUpdate;

	SVExtentPointStruct m_svRotationPoint;
	SVExtentPointStruct m_svOffset;

	double m_dRotationAngle;
};

#endif //_SVEXTENTOFFSETSTRUCT_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageLibrary\SVExtentOffsetStruct.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:40:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jan 2006 16:49:56   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  534
 * SCR Title:  Add Image Select for Auxiliary Extent Source
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVImageExtentClass to use new SVExtentOffsetStruct for managing the extent offset data. Fixed check in problem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jan 2006 16:40:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  534
 * SCR Title:  Add Image Select for Auxiliary Extent Source
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVImageExtentClass to use new SVExtentOffsetStruct for managing the extent offset data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/