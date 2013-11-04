//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeSystem
//* .File Name       : $Workfile:   SVMatroxGigeSystem.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:44:40  $
//******************************************************************************

#ifndef SVMATROXGIGESYSTEM_H
#define SVMATROXGIGESYSTEM_H

#include "SVMatroxLibrary/SVMatroxSystemRef.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVMatroxGigeDigitizerList.h"

class SVMatroxGigeSystem
{
private:
	SVMatroxGigeDigitizerList m_digitizers;

public:
	SVMatroxGigeSystem();
	SVMatroxGigeSystem( const SVMatroxGigeSystem& p_rValue );
	virtual ~SVMatroxGigeSystem();

	const SVMatroxGigeSystem& operator=( const SVMatroxGigeSystem& p_rValue );
	
	HRESULT AddDigitizer(SVMatroxGigeDigitizer& digitizer);
	HRESULT RemoveDigitizer(unsigned char digitizerHandle);

	bool IsValidHandle(unsigned char digitizerHandle) const;
	SVMatroxGigeDigitizer& GetDigitizer(unsigned char digitizerHandle, HRESULT& status);
	const SVMatroxGigeDigitizer& GetDigitizer(unsigned char digitizerHandle, HRESULT& status) const;

	SVMatroxGigeDigitizerList& GetDigitizerList();
	const SVMatroxGigeDigitizerList& GetDigitizerList() const;

	unsigned char m_Handle;
	SVString m_matroxSystemName;
	SVString m_Name;
	SVMatroxSystemRef m_System;
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxGige\SVMatroxGigeSystem.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:44:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jun 2009 17:19:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
