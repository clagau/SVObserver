// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxImageRotateStruct
// * .File Name       : $Workfile:   SVMatroxImageRotateStruct.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:21:30  $
// ******************************************************************************

#ifndef SV_MATROX_IMAGE_ROTATE_STRUCT_H
#define SV_MATROX_IMAGE_ROTATE_STRUCT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVMatroxBuffer.h"
#include "SVMatroxEnums.h"

/**
@SVObjectName Matrox Image Rotate Struct

@SVObjectOverview This class is used to hold data being sent to the ImageRotate function.

@SVObjectOperations This data class has no operations.

*/
class SVMatroxImageRotateStruct  
{
public:
	SVMatroxImageRotateStruct(SVMatroxBuffer& p_rSource);
	virtual ~SVMatroxImageRotateStruct();

	SVMatroxBuffer&			m_rSource;
	double					m_dAngle; 
	double					m_dSrcCenX;
	double					m_dSrcCenY;
	double					m_dDstCenX;
	double					m_dDstCenY;
	SVImageOperationTypeEnum m_eInterpolation;
};

#endif // #ifndef SV_MATROX_IMAGE_ROTATE_STRUCT_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxImageRotateStruct.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:21:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/