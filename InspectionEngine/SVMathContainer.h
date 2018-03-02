//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMathContainer
//* .File Name       : $Workfile:   SVMathContainer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:29:16  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVTaskObject.h"
#pragma endregion Includes

class SVIPDoc;
class SVToolClass;

class SVMathContainerClass : public SVTaskObjectClass
{
	SV_DECLARE_CLASS( SVMathContainerClass )

public:
	SVMathContainerClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVMATHCONTAINER );
	virtual ~SVMathContainerClass();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

protected:
	SVDoubleValueObjectClass*	getInputMathResult();

protected:
	SVInObjectInfoStruct		m_inputMathResult;
};
