// ******************************************************************************
// * COPYRIGHT (c) 2007 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxOcrCreateStruct
// * .File Name       : $Workfile:   SVMatroxOcrCreateStruct.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:24:34  $
// ******************************************************************************


#include "stdafx.h"
#include "SVMatroxOcrCreateStruct.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVMatroxOcrCreateStruct::SVMatroxOcrCreateStruct()
	:m_eFontType(SVGeneral),
	m_lCharNumber(0),
	m_lCharCellSizeX(18),
	m_lCharCellSizeY(22),
	m_lCharOffsetX(0),
	m_lCharOffsetY(0),
	m_lCharSizeX(12),
	m_lCharSizeY(16),
	m_lCharThickness(2),
	m_lStringLength(5),
	m_eInitFlag(SVOcrForegroundBlack)
{
}

SVMatroxOcrCreateStruct::~SVMatroxOcrCreateStruct()
{

}

