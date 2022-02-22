//******************************************************************************
//* COPYRIGHT (c) 2014 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : Custom2Filter
//* .File Name       : $Workfile:   Custom2Filter.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Oct 2014 11:15:00  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVFilterClass.h"
#include "ObjectInterfaces/SVImageBufferHandleInterface.h"
#include "SVMatroxLibrary/SVMatroxBuffer.h"
#include "SVValueObjectLibrary/SVBoolValueObjectClass.h"
#include "SVValueObjectLibrary/SVLongValueObjectClass.h"
#pragma endregion Includes

class Custom2Filter : public SVFilterClass
{
public:
#pragma region Declarations
	SV_DECLARE_CLASS
#pragma endregion Declarations

public:
#pragma region Constructor
	//************************************
	// Description: The class constructor
	// Parameter: pOwner <in> The owner of this class
	// Parameter: StringResourceID <in> The class name resource ID
	//************************************
	Custom2Filter( SVObjectClass* pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_CUSTOM2FILTER );

	//************************************
	// Description: The class destructor
	//************************************
	virtual ~Custom2Filter();
#pragma endregion Constructor

#pragma region Public Methods
	//************************************
	// Description: Creates the object
	// Parameter: rCreateStructure <in> The create structure
	// Return: True on success
	//************************************
	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

#pragma region virtual method (IFilter)
	//************************************
	// Description: Determines if the IP Doc needs to be reset
	// Return: True as it should always reset
	//************************************
	virtual bool shouldResetInspection() const override { return true; }
#pragma endregion virtual method (IFilter)

#pragma endregion Public Methods

protected:
#pragma region Protected Methods
	//************************************
	// Description: Rebuilds the Kernel values required for the filter
	//************************************
	void RebuildKernel();

	//************************************
	// Description: This method is called when the mode is set to run
	// Parameter: First <in> 
	// Parameter: RInputImageHandle <in> The handle to the source image to implement the filter on
	// Parameter: ROutputImageHandle <out> The handle to the destination image
	// Parameter: rRunStatus <in> The status of the run mode
	// Return: True on success
	//************************************
	virtual bool onRun( bool First, SvOi::SVImageBufferHandlePtr RInputImageHandle, SvOi::SVImageBufferHandlePtr ROutputImageHandle, SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;
#pragma endregion Protected Methods

private:
#pragma region Private Methods
	//************************************
	// Description: This method is called when the object is initialized
	//************************************
	void init();

	//************************************
	// Description: The method checks if the width or height of the kernel is valid
	// Parameter: rKernelSize <in> The kernel parameter to check
	// Return: A valid kernel size 
	//************************************
	long validateKernelSize(SvVol::SVLongValueObjectClass& rKernelSize );

	bool ValidateLocal(SvStl::MessageContainerVector *pErrorMessages = nullptr) const;
#pragma endregion Private Methods

private:
#pragma region Member Variables
	SvVol::SVLongValueObjectClass m_KernelArray;			//The dynamic kernel array
	SvVol::SVLongValueObjectClass m_KernelWidth;			//The kernel width
	SvVol::SVLongValueObjectClass m_KernelHeight;			//The kernel height
	SvVol::SVLongValueObjectClass m_NormalizationFactor;	//The normalization factor
	SvVol::SVBoolValueObjectClass m_AbsoluteValue;			//The absolute value flag
	SvVol::SVBoolValueObjectClass m_ClippingEnabled;		//The clipping enabled flag

	SVMatroxBuffer m_milKernel;						//The Matrox buffer for the filter
#pragma endregion Member Variables
};

