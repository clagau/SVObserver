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
#include "EquationArray.h"
#include "SVValueObjectLibrary/SVBoolValueObjectClass.h"
#include "SVValueObjectLibrary/SVByteValueObjectClass.h"
#pragma endregion Includes

namespace SvOp
{

class SVLUTEquation : public EquationArray
{
	SV_DECLARE_CLASS

public:
	SVLUTEquation( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVLUTEQUATION );
	virtual ~SVLUTEquation();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	bool SetDefaultFormula(SvStl::MessageContainerVector *pErrorMessages=nullptr);

protected:
	void init();
	virtual bool onRun( SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

//******************************************************************************
// Data Element(s):
//******************************************************************************
protected:
	// Flag if the values should be clipped (> 255 -> 255 and < 0 -> 0) 
	SvVol::SVBoolValueObjectClass m_isLUTFormulaClipped;

	// Contains the new LUT values after running.
	// Provides a LUT table size of 256!
	SvVol::SVByteValueObjectClass m_byteVectorResult;
};

} //namespace SvOp
