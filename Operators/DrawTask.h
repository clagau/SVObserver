//*****************************************************************************
/// \copyright (c) 2021,2021 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file DrawTask.h
/// All Rights Reserved 
//*****************************************************************************
/// Virtual class for DrawTask
//******************************************************************************
#pragma once

#pragma region Includes
#include "InspectionEngine/SVTaskObject.h"
#include "SVValueObjectLibrary/SVByteValueObjectClass.h"
#include "SVValueObjectLibrary/SVEnumerateValueObjectClass.h"
#pragma endregion Includes

namespace SvOp //< Operators
{
namespace
{
constexpr const char* gInside = _T("Inside");
constexpr const char* gOutside = _T("Outside");
constexpr const char* gContour = _T("Contour");
constexpr const char* gSector = _T("Sector");
constexpr const char* gPolyline = _T("Polyline");
constexpr const char* gNormal = _T("Normal");
constexpr const char* gInfinityLines = _T("Infinity Lines");
}

enum class DrawArea : long
{
	InsideArea = 1,
	OutsideArea = 2,
	ContourArea = 3,
	SectorArea = 4,
	PolylineArea = 5,
	NormalArea = 6,
	InfiniteArea = 7,
};

const static SvDef::NameValuePair gInsideAreaPair {gInside, static_cast<long>(DrawArea::InsideArea)};
const static SvDef::NameValuePair gOutsideAreaPair {gOutside, static_cast<long>(DrawArea::OutsideArea)};
const static SvDef::NameValuePair gContourAreaPair {gContour, static_cast<long>(DrawArea::ContourArea)};
const static SvDef::NameValuePair gSectorAreaPair {gSector, static_cast<long>(DrawArea::SectorArea)};
const static SvDef::NameValuePair gPolylineAreaPair {gPolyline, static_cast<long>(DrawArea::PolylineArea)};
const static SvDef::NameValuePair gNormalAreaPair {gNormal, static_cast<long>(DrawArea::NormalArea)};
const static SvDef::NameValuePair gInfinityLinesAreaPair {gInfinityLines, static_cast<long>(DrawArea::InfiniteArea)};


namespace
{
const static SvDef::NameValueVector gDrawAreaDefaultList {gInsideAreaPair, gOutsideAreaPair, gContourAreaPair};
}

class DrawTask : public SvIe::SVTaskObjectClass
{
#pragma region Constructor
public:
	explicit DrawTask(LPCTSTR ObjectName, SvDef::NameValueVector rDrawAreaList = gDrawAreaDefaultList, long defaultDrawArea = 1);
	DrawTask(SVObjectClass* POwner, int StringResourceID, SvDef::NameValueVector rDrawAreaList = gDrawAreaDefaultList, long defaultDrawArea = 1);
	virtual ~DrawTask();
#pragma endregion Constructor

#pragma region Public Methods
public:
	bool Run(SvOi::SVImageBufferHandlePtr pImageHandle, SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector* pErrorMessages = nullptr);
	virtual bool ResetObject(SvStl::MessageContainerVector* pErrorMessages = nullptr) override;
	void setImageInfo(const SVImageInfoClass& rImageInfo) {	m_imageInfo = rImageInfo; };
	MIL_ID getGraphicContextId() const { return m_graphicContextId; };
	MIL_DOUBLE getColor() const { return m_color; };
	MIL_DOUBLE getTmpColor() const { return m_tmpColor; };
	SvOi::SVImageBufferHandlePtr getTmpImage() const { return m_pTmpImage; };
	bool isColorImage() const;
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	virtual bool drawImage(SvOi::SVImageBufferHandlePtr pImageHandle, DrawArea drawArea) = 0;
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	void initialize(const SvDef::NameValueVector& drawAreaList, long defaultDrawArea);
	void buildEmbeddedObjectList(const SvDef::NameValueVector& drawAreaList, long defaultDrawArea);
#pragma endregion Private Methods

#pragma region Member Variables
private:
	SvVol::SVByteValueObjectClass  m_Color1Object;
	SvVol::SVByteValueObjectClass  m_Color2Object;
	SvVol::SVByteValueObjectClass  m_Color3Object;
	MIL_DOUBLE m_color = 0;
	MIL_DOUBLE m_tmpColor = 0;

	SvVol::SVEnumerateValueObjectClass  m_drawAreaObject;
	DrawArea m_drawArea = DrawArea::InsideArea;

	MIL_ID m_graphicContextId = M_NULL;
	SvOi::SVImageBufferHandlePtr m_pTmpImage;

	SVImageInfoClass m_imageInfo;
#pragma endregion Member Variables
};

} //namespace SvOp