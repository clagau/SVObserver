//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOperator
//* .File Name       : $Workfile:   SVOperator.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   15 May 2014 11:21:50  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "InspectionEngine/SVTaskObject.h"
#pragma endregion Includes

namespace SvOp
{

class SVOperator : public SvIe::SVTaskObjectClass
{
	///This class does not need to call SV_DECLARE_CLASS as it is a base class and only derived classes are instantiated
	//SV_DECLARE_CLASS( SVOperator );

public:
	SVOperator( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVOPERATOR );
	virtual ~SVOperator();
	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

private:
	void init();
};

} //namespace SvOp
