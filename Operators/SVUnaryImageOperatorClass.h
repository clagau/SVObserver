//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVUnaryImageOperatorClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   12 Aug 2013 16:30:30  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOperator.h"
#include "SVUnaryImageOperatorList.h"
#pragma endregion Includes

namespace SvOp
{

class SVUnaryImageOperatorClass : public SVOperatorClass
{
	SV_DECLARE_CLASS( SVUnaryImageOperatorClass );
public:
	SVUnaryImageOperatorClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVUNARYIMAGEOPERATOR );
	virtual ~SVUnaryImageOperatorClass();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

	//First definition of onRun with this parameter (SVTaskObjectClass has different parameter), so override is not possible
	virtual bool Run( bool First, SvOi::SVImageBufferHandlePtr RInputImageHandle, SvOi::SVImageBufferHandlePtr ROutputImageHandle, SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr );

protected:
	//First definition of onRun with this parameter (SVTaskObjectClass has different parameter), so override is not possible
	virtual bool onRun( bool First, SvOi::SVImageBufferHandlePtr RInputImageHandle, SvOi::SVImageBufferHandlePtr ROutputImageHandle, SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr );
	const SvIe::SVImageClass* getReferenceImage() const;
	const SvIe::SVImageClass* getOutputImage(bool bRunMode = false) const;

	SVUnaryImageOperatorListClass* m_pCurrentUIOPL;
	
private:
	void init();
};

} //namespace SvOp
