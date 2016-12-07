//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLUTEquation
//* .File Name       : $Workfile:   SVLUTEquation.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   27 Jan 2014 15:44:20  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVEquation.h"
#pragma endregion Includes

class SVLUTEquationClass : public SVEquationClass
{
	SV_DECLARE_CLASS( SVLUTEquationClass );

public:
	SVLUTEquationClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVLUTEQUATION );
	virtual ~SVLUTEquationClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure ) override;

	virtual HRESULT ResetObject() override;

	virtual BOOL OnValidate() override;

	BOOL SetDefaultFormula();

protected:
	void init();
	virtual BOOL onRun( SVRunStatusClass& RRunStatus ) override;

//******************************************************************************
// Data Element(s):
//******************************************************************************
protected:
	// Flag if the values should be clipped (> 255 -> 255 and < 0 -> 0) 
	SVBoolValueObjectClass m_isLUTFormulaClipped;

	// Contains the new LUT values after running.
	// Provides a LUT table size of 256!
	SVByteValueObjectClass m_byteVectorResult;

	// LUT Table Index Counter...
	// Can be used in Equation as an index variable running from 0 to 256.
	// NOTE:
	// If Equation supports vector calculations also in future time,
	// can be replaced with a vector with 256 elements [0...255].
	SVLongValueObjectClass	m_lutIndex;
};

