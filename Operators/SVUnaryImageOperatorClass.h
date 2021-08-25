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
#pragma endregion Includes

namespace SvOp
{
class SVUnaryImageOperatorList;

class SVUnaryImageOperatorClass : public SVOperator
{
	///This class does not need to call SV_DECLARE_CLASS as it is a base class and only derived classes are instantiated
	//SV_DECLARE_CLASS
public:
	SVUnaryImageOperatorClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVUNARYIMAGEOPERATOR );
	virtual ~SVUnaryImageOperatorClass();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

	//First definition of onRun with this parameter (SVTaskObjectClass has different parameter), so override is not possible
	virtual bool Run( bool First, SvOi::SVImageBufferHandlePtr RInputImageHandle, SvOi::SVImageBufferHandlePtr ROutputImageHandle, SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr );

protected:
	//First definition of onRun with this parameter (SVTaskObjectClass has different parameter), so override is not possible
	virtual bool onRun( bool First, SvOi::SVImageBufferHandlePtr RInputImageHandle, SvOi::SVImageBufferHandlePtr ROutputImageHandle, SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr );
	const SvIe::SVImageClass* getReferenceImage() const;
	const SvIe::SVImageClass* getOutputImage(bool bRunMode = false) const;

	SVUnaryImageOperatorList* m_pCurrentUIOPL;
	
private:
	void init();
};

} //namespace SvOp
