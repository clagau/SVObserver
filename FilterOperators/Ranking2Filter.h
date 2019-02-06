//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by Seidenader Maschinenbau GmbH
/// \file Ranking2Filter.h
/// All Rights Reserved
//*****************************************************************************
/// The class for the filter Ranking2Filter.
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVFilterClass.h"
#include "ObjectInterfaces/SVImageBufferHandleInterface.h"
#include "SVMatroxLibrary/SVMatroxBuffer.h"
#pragma endregion Includes

class Ranking2Filter : public SVFilterClass
{
public:
#pragma region Declarations
	SV_DECLARE_CLASS(Ranking2Filter);
#pragma endregion Declarations

public:
#pragma region Constructor
	/// The class constructor
	/// \param pOwner [in] The owner of this class
	/// \param StringResourceID [in] The class name resource ID
	/// \returns 
	Ranking2Filter(SVObjectClass* pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_RANKING2FILTER);

	virtual ~Ranking2Filter();
#pragma endregion Constructor

#pragma region Public Methods
	virtual bool CreateObject(const SVObjectLevelCreateStruct& rCreateStructure) override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages = nullptr) override;

#pragma region virtual method (IFilter)
	/// Determines if the IP Doc needs to be reset
	/// \returns bool True as it should always reset
	virtual bool shouldResetInspection() const override { return true; }
#pragma endregion virtual method (IFilter)

#pragma endregion Public Methods

protected:
#pragma region Protected Methods
	/// Description: Rebuilds the Kernel values required for the filter
	void RebuildKernel();

	virtual bool onRun(bool First, SvOi::SVImageBufferHandlePtr RInputImageHandle, SvOi::SVImageBufferHandlePtr ROutputImageHandle, SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages = nullptr) override;
#pragma endregion Protected Methods

private:
#pragma region Private Methods
	void init();

	/// The method checks if the width or height of the kernel is valid
	/// \param rKernelSize [in] The kernel parameter to check
	/// \returns long A valid kernel size 
	long validateKernelSize(SvVol::SVLongValueObjectClass& rKernelSize);

	bool ValidateLocal(SvStl::MessageContainerVector *pErrorMessages = nullptr) const;
#pragma endregion Private Methods

private:
#pragma region Member Variables
	SvVol::SVLongValueObjectClass m_KernelArray;			//The dynamic kernel array
	SvVol::SVLongValueObjectClass m_KernelWidth;			//The kernel width
	SvVol::SVLongValueObjectClass m_KernelHeight;			//The kernel height
	SvVol::SVLongValueObjectClass m_RankingRank;

	SVMatroxBuffer m_milKernel;						//The Matrox buffer for the filter
#pragma endregion Member Variables
};

