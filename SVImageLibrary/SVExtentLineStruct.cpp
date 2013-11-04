//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExtentLineStruct
//* .File Name       : $Workfile:   SVExtentLineStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:39:50  $
//******************************************************************************

#include "stdafx.h"
#include "SVExtentLineStruct.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVExtentLineStruct::SVExtentLineStruct()
{
	Initialize();
}

SVExtentLineStruct::SVExtentLineStruct( const SVExtentLineStruct &p_rsvValue )
{
	*this = p_rsvValue;
}



SVExtentLineStruct::~SVExtentLineStruct()
{
	Initialize();
}

const SVExtentLineStruct &SVExtentLineStruct::operator=( const SVExtentLineStruct &p_rsvValue )
{
	if ( this != &p_rsvValue )
	{
		Initialize();

		m_svPointArray.Copy(p_rsvValue.m_svPointArray);

		m_dwColor = p_rsvValue.m_dwColor;
		
		m_bIsAngleValid = p_rsvValue.m_bIsAngleValid;
		m_dAngle = p_rsvValue.m_dAngle;
	}
	return *this;
}


HRESULT SVExtentLineStruct::Initialize()
{
	HRESULT l_hrOk = S_OK;

	m_svPointArray.RemoveAll();

	m_dwColor = 0;

	m_bIsAngleValid = false;
	m_dAngle = 0.0;

	return l_hrOk;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageLibrary\SVExtentLineStruct.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:39:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   20 Jun 2005 10:02:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added self assignment check in operator =
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   18 Mar 2005 09:03:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  463
 * SCR Title:  Add new Linear Measurement Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated Line data to include draw angle to remove rounding issues with drawing straight lines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Feb 2005 14:02:46   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   First Iteration of File
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/