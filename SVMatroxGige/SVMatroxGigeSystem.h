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

