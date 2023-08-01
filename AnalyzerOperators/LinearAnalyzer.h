// ******************************************************************************
// * COPYRIGHT (c) 2005 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : LinearAnalyzer
// * .File Name       : $Workfile:   LinearAnalyzer.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.3  $
// * .Check In Date   : $Date:   10 Jul 2014 17:44:52  $
// ******************************************************************************

#pragma once

#pragma region Includes
#include "Analyzer.h"
#include "SVLibrary/SVValueBaseNormalizerClass.h"
#include "SVObjectLibrary/InputObject.h"
#include "SVValueObjectLibrary/SVBoolValueObjectClass.h"
#include "ObjectInterfaces/ISVLinearAnalyzer.h"
#pragma endregion Includes

namespace SvOp
{
class SVLinearEdgeProcessingClass;
}

namespace SvAo
{

class LinearAnalyzer : public Analyzer, public SvOi::ISVLinearAnalyzer
{
	///This class does not need to call SV_DECLARE_CLASS as it is a base class and only derived classes are instantiated
	//SV_DECLARE_CLASS

	friend class SVSetupDialogManager;

public:
	LinearAnalyzer( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVLINEARANALYZER );
	virtual ~LinearAnalyzer();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual HRESULT GetSelectedEdgeOverlays( SVExtentMultiLineStruct &p_MultiLine ) const;

	virtual void addParameterForMonitorList(SvStl::MessageContainerVector& rMessages, std::back_insert_iterator<SvOi::ParametersForML> inserter) const override;

	SvOp::SVLinearEdgeProcessingClass* GetEdgeA() const;
	SvOp::SVLinearEdgeProcessingClass* GetEdgeB() const;

	HRESULT GetPixelDepth();

	virtual SvPb::GetNormalizerValuesResponse getNormalizerValues() override;
	virtual SvPb::GetNormalizerValuesResponse setNormalizerRanges(const SvPb::SetNormalizerRangesRequest&) override;

	SvVol::SVBoolValueObjectClass m_svShowAllEdgeAOverlays;
	SvVol::SVBoolValueObjectClass m_svShowAllEdgeBOverlays;

protected:
	virtual void addOverlayGroups(SVExtentMultiLineStructVector& rMultiLineArray, const SVImageExtentClass& rImageExtents) const override;
	virtual void addOverlayGroups(SvPb::Overlay& rOverlay) const override;

	virtual void addOverlayResults(SvPb::Overlay& , bool ) const {};
	virtual void addOverlayResultDetails(SvPb::Overlay& rOverlay, bool isVertical = false) const;

	virtual HRESULT updateImageExtent(bool init) override;
	HRESULT GetProjectedExtent( long p_lIndex, long &p_rlBottom, SVImageExtentClass& rImageExtent );

	HRESULT GetInputProfileOrientation(long& rProfileOrientation) const;
	HRESULT GetInputUseRotationAngle(bool& rUseRotationAngle);
	bool ValidateEdgeA(SvStl::MessageContainerVector *pErrorMessages=nullptr);
	bool ValidateEdgeB(SvStl::MessageContainerVector *pErrorMessages=nullptr);

	void addDPointResultToAvailableChildren(SvPb::EmbeddedIdEnum embeddedID, UINT idForClassnamePart1);
	void addScalarResultToAvailableChildren(SvPb::EmbeddedIdEnum embeddedID, SvPb::SVObjectSubTypeEnum subtype, UINT idForClassnamePart1, UINT idForClassnamePart2);

	/// Set the parameter to the list, if available and valid.
	/// \param rName [in] Full dotted name of the parameter.
	/// \param inserter [in] backinsert iterator to added the parameter.
	/// \returns bool True if successfully.
	bool setParameterToList(const std::string& rName, std::back_insert_iterator<SvOi::ParametersForML> inserter) const;

	virtual std::vector<std::string> getParameterNamesForML() const;

	SvOl::InputObject m_InputImage;
	SvOl::InputObject m_InputProfileOrientation;
	SvOl::InputObject m_InputUseRotationAngle;

	long m_lPixelDepth;

	DWORD m_dwMinThreshold;
	DWORD m_dwMaxThreshold;
	DWORD m_dwColorNumber;

	bool m_bEnableDirection;
	bool m_bEnableEdgeSelect;
	bool m_bEnablePolarisation;
	bool m_bEnablePosition;
	bool m_bEnableThreshold;

	SVValueBaseNormalizerClass m_svNormalizer;
};

} //namespace SvAo
