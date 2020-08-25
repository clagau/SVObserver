//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVUnaryImageOperatorClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   09 Aug 2013 12:04:20  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVUnaryImageOperatorClass.h"
#include "SVUnaryImageOperatorList.h"
#include "SVStdImageOperatorListClass.h"
#include "SVInplaceImageOperatorListClass.h"
#include "SVStatusLibrary/RunStatus.h"
#pragma region Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations
///For this class it is not necessary to call SV_IMPLEMENT_CLASS as it is a base class and only derived classes are instantiated.
//SV_IMPLEMENT_CLASS( SVUnaryImageOperatorClass, SVUnaryImageOperatorClassGuid )

SVUnaryImageOperatorClass::SVUnaryImageOperatorClass( SVObjectClass* POwner, int StringResourceID )
						  :SVOperator( POwner, StringResourceID )
{
	init();
}

SVUnaryImageOperatorClass::~SVUnaryImageOperatorClass()
{
}

void SVUnaryImageOperatorClass::init()
{
	// Identify our output type
	m_outObjectInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVUnaryImageOperatorObjectType;

	// Identify our input type needs

	// Register Embedded Object(s)

	// Set Embedded defaults
	m_pCurrentUIOPL = nullptr;

	// Set default inputs and outputs
	addDefaultInputObjects();
}

bool SVUnaryImageOperatorClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	// Owner can only be : SVUnaryImageOperatorList !!!
	m_isCreated = __super::CreateObject(rCreateStructure) && (nullptr != (m_pCurrentUIOPL = dynamic_cast<SVUnaryImageOperatorList*>(GetParent())));
	return m_isCreated;
}


////////////////////////////////////////////////////////////////////////////////
// .Title       : Run
// -----------------------------------------------------------------------------
// .Description : Runs this operator.
//              : Returns FALSE, if operator cannot run ( may be deactivated ! )
////////////////////////////////////////////////////////////////////////////////
bool SVUnaryImageOperatorClass::Run( bool First, SvOi::SVImageBufferHandlePtr RInputImageHandle, SvOi::SVImageBufferHandlePtr ROutputImageHandle, RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	// Should call Base Class Run() here but we have different parameters!!!

	clearRunErrorMessages();
	
	// Run yourself...
	bool bRetVal =  onRun( First, RInputImageHandle, ROutputImageHandle, rRunStatus, &m_RunErrorMessages );

	// Get Status Color...
	DWORD dwValue = rRunStatus.GetStatusColor();
	m_statusColor.SetValue(dwValue);

	// Get Status...
	dwValue = rRunStatus.GetState();
	m_statusTag.SetValue(dwValue);

	if (nullptr != pErrorMessages && !m_RunErrorMessages.empty())
	{
		pErrorMessages->insert(pErrorMessages->end(), m_RunErrorMessages.begin(), m_RunErrorMessages.end());
	}

	return bRetVal;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : onRun
// -----------------------------------------------------------------------------
// .Description : Runs this operator. MUST BE OVERRIDDEN !!!
//              : Returns FALSE, if operator cannot run ( may be deactivated ! )
////////////////////////////////////////////////////////////////////////////////
bool SVUnaryImageOperatorClass::onRun( bool, SvOi::SVImageBufferHandlePtr, SvOi::SVImageBufferHandlePtr, RunStatus& , SvStl::MessageContainerVector* )
{
	assert(0);
	return false;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : getReferenceImage
// -----------------------------------------------------------------------------
// .Description : Returns reference image object.
//				: The reference object provides for you information about
//				: the target image object for this operator.
//				: 
//				: i.e. use this reference image in CreateObject(...) to determine
//				:	target size, in case you have to support an 
//				:	intermediate proc buffer,
//				:	or pixel depth, etc.
////////////////////////////////////////////////////////////////////////////////
const SvIe::SVImageClass* SVUnaryImageOperatorClass::getReferenceImage() const
{
	if( nullptr != m_pCurrentUIOPL )
	{
		return m_pCurrentUIOPL->getReferenceImage();
	}
	
	return nullptr;
}

const SvIe::SVImageClass* SVUnaryImageOperatorClass::getOutputImage(bool bRunMode /*= false*/) const
{
	if (nullptr != m_pCurrentUIOPL)
	{
		return m_pCurrentUIOPL->getOutputImage(bRunMode);
	}

	return nullptr;
}

} //namespace SvOp
