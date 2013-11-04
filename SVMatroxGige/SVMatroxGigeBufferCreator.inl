//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeBufferCreator
//* .File Name       : $Workfile:   SVMatroxGigeBufferCreator.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:39:12  $
//******************************************************************************

#include "SVMatroxGigeBufferCreator.h"
#include "SVMatroxLibrary/SVMatroxBuffer.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"

template <SVMatroxBufferAttributeEnum Attribute>
SVMatroxGigeBufferCreator<Attribute>::SVMatroxGigeBufferCreator()
{
}

template <SVMatroxBufferAttributeEnum Attribute>
SVMatroxGigeBufferCreator<Attribute>::~SVMatroxGigeBufferCreator()
{
}

template <SVMatroxBufferAttributeEnum Attribute>
HRESULT SVMatroxGigeBufferCreator<Attribute>::Create(const SVMatroxSystem& rSystem, unsigned long p_ulWidth, unsigned long p_ulHeight, unsigned long p_ulFormat, SVMatroxGigeBufferStruct& rBufferStruct)
{
	HRESULT l_hr = S_OK;

	rBufferStruct.m_Buffer = new SVMatroxBuffer;
	rBufferStruct.m_ulWidth = p_ulWidth;
	rBufferStruct.m_ulHeight = p_ulHeight;
	rBufferStruct.m_ulFormat = p_ulFormat;

	SVMatroxBufferCreateStruct createStruct;
	l_hr = BuildCreateStruct(p_ulWidth, p_ulHeight, p_ulFormat, createStruct);
	
	if (l_hr == S_OK)
	{
		SVMatroxBufferInterface::SVStatusCode l_Code = SVMatroxBufferInterface::Create( rSystem, *(rBufferStruct.m_Buffer.get()), createStruct);
		if (l_Code == SVMEE_STATUS_OK)
		{
			l_Code = SVMatroxBufferInterface::GetHostAddress(&(rBufferStruct.m_ulAddress), *(rBufferStruct.m_Buffer.get()));
		}
		l_hr = l_Code;
	}
	return l_hr;
}

template <SVMatroxBufferAttributeEnum Attribute>
HRESULT SVMatroxGigeBufferCreator<Attribute>::BuildCreateStruct(unsigned long p_ulWidth, unsigned long p_ulHeight, unsigned long p_ulFormat, SVMatroxBufferCreateStruct& rCreateStruct)
{
	HRESULT l_hr = S_OK;

	rCreateStruct.m_eAttribute = Attribute;

	rCreateStruct.m_lSizeX = p_ulWidth;
	rCreateStruct.m_lSizeY = p_ulHeight;

	switch (p_ulFormat)
	{
		case SVImageFormatMono8:
			rCreateStruct.m_lSizeBand = 1;
			rCreateStruct.m_eType = SV8BitUnsigned;
			break;

		case SVImageFormatMono16:
			rCreateStruct.m_lSizeBand = 1;
			rCreateStruct.m_eType = SV16BitUnsigned;
			break;

		case SVImageFormatRGB888:
		case SVImageFormatRGB8888:
			rCreateStruct.m_lSizeBand = 3;
			rCreateStruct.m_eType = SV8BitUnsigned;
			break;

		case SVImageFormatRGB161616:
			rCreateStruct.m_lSizeBand = 3;
			rCreateStruct.m_eType = SV16BitUnsigned;
			break;

		case SVImageFormatY411:
		case SVImageFormatY422:
		case SVImageFormatY444:
			rCreateStruct.m_lSizeBand = 3;
			rCreateStruct.m_eType = SV8BitUnsigned;
			break;

		default:
			l_hr = S_FALSE;
			break;
	}
	return l_hr;
}

template <SVMatroxBufferAttributeEnum Attribute>
HRESULT SVMatroxGigeBufferCreator<Attribute>::Destroy(SVMatroxGigeBufferStruct& rBufferStruct)
{
	HRESULT l_hr = S_OK;

	if( rBufferStruct.m_Buffer.get() != NULL )
	{
		rBufferStruct.m_Buffer.get()->clear();
	}
	else
	{
		l_hr = S_FALSE;
	}

	return l_hr;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxGige\SVMatroxGigeBufferCreator.inl_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:39:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   27 Jan 2011 10:21:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jun 2009 17:18:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
