//******************************************************************************
//* COPYRIGHT (c) 2013 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVUserMaskOperatorClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   15 May 2014 15:04:56  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <fstream>
#include "SVShapeMaskHelperClass.h"
#include "SVUnaryImageOperatorList.h"
#include "SVUserMaskOperatorClass.h"
#include "Definitions/TextDefinesSvDef.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVMatroxLibrary/SVMatroxImageInterface.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "SVObjectLibrary/SVToolsetScriptTags.h"
#include "InspectionEngine/RunStatus.h"
#include "SVUtilityLibrary/SVSafeArray.h"
#include "Tools/SVTool.h"
#include "SVProtoBuf/Overlay.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( SVUserMaskOperatorClass, SvPb::UserMaskOperatorClassId)

SVUserMaskOperatorClass::SVUserMaskOperatorClass( SVObjectClass* POwner, int StringResourceID )
						:SVUnaryImageOperatorClass( POwner, StringResourceID ) 
{
	init();
}

SVUserMaskOperatorClass::~SVUserMaskOperatorClass()
{
	SVUserMaskOperatorClass::CloseObject();
}

void SVUserMaskOperatorClass::init()
{
	// Identify our output type
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVUnaryImageOperatorObjectType;
	m_ObjectTypeInfo.m_SubType = SvPb::SVUserMaskOperatorObjectType;

	m_userMaskImageInput.SetInputObjectType(SvPb::SVImageObjectType, SvPb::SVImageMonoType);
	registerInputObject( &m_userMaskImageInput, _T( "UserMaskImage" ), SvPb::UserMaskImageInputEId);
	m_userMaskImageInput.setAllowedMode(SvOi::InputAllowedMode::IsBeforeTool);


	// RRRRGGGGHHHHHH who created this stupid object structure????
	SVShapeMaskHelperClass* pShapeHelper = new SVShapeMaskHelperClass(this);

	bool bAddFriend = AddFriend( pShapeHelper );
	Log_Assert( bAddFriend );	UNREFERENCED_PARAMETER(bAddFriend);


	RegisterEmbeddedObject( &m_bvoActivated, SvPb::MaskEnabledEId, IDS_OBJECTNAME_ENABLED, false, SvOi::SVResetItemIP, true);
	RegisterEmbeddedObject( &m_dwvoMaskType, SvPb::MaskUseImageMaskEId, IDS_MASK_TYPE, false, SvOi::SVResetItemIP, true);
	RegisterEmbeddedObject( &m_evoCurrentMaskOperator, SvPb::MaskOperatorEId, IDS_OBJECTNAME_MASK_OPERATOR, false, SvOi::SVResetItemOwner, true);
	RegisterEmbeddedObject( &m_evoFillArea, SvPb::MaskFillAreaEId, IDS_OBJECTNAME_MASK_FILL_AREA, false, SvOi::SVResetItemOwner, true);
	RegisterEmbeddedObject( &m_lvoFillColor, SvPb::MaskFillColorEId, IDS_OBJECTNAME_MASK_FILL_COLOR, false, SvOi::SVResetItemOwner, true);
	RegisterEmbeddedObject( &m_evoDrawCriteria, SvPb::DrawCriteriaEId, IDS_OBJECTNAME_DRAW_CRITERIA, false, SvOi::SVResetItemOwner, true);
	RegisterEmbeddedObject(&m_bvoContRecalc, SvPb::ContRecalcEId, IDS_OBJECTNAME_CONTINUOUS_RECALC, false, SvOi::SVResetItemOwner, true);

	SvDef::NameValueVector EnumVector
	{
		{ _T("AND"), SVImageAnd },
		{ _T("OR"), SVImageOr },
		{ _T("XOR"), SVImageXOr },
		{ _T("NOR"), SVImageNor },
		{ _T("XNOR"), SVImageXNor },
		{ _T("NAND"), SVImageNand },
		{ _T("ADD"), SVImageAddSaturation },
		{ _T("SUB"), SVImageSubSaturation },
		{ _T("MULT"), SVImageMulSaturation },
		{ _T("DIV"), SVImageDiv },
		{ _T("MIN"), SVImageMin },
		{ _T("MAX"), SVImageMax },
		{ _T("PASS"), SVImagePass }
	};
	m_evoCurrentMaskOperator.SetEnumTypes(EnumVector);
	m_evoCurrentMaskOperator.SetDefaultValue(SVImageAnd, true);

	EnumVector.clear();
	EnumVector.push_back(SvDef::NameValuePair{ _T("Mask Area"), 0 });
	EnumVector.push_back(SvDef::NameValuePair{ _T("Background"), 1 });
	m_evoFillArea.SetEnumTypes(EnumVector);
	m_evoFillArea.SetDefaultValue(0l, true);	// mask area is default

	m_lvoFillColor.SetDefaultValue(0, true);	// black default

	EnumVector.clear();
	EnumVector.push_back(SvDef::NameValuePair{ _T("None"), SVNone });
	EnumVector.push_back(SvDef::NameValuePair{ _T("Non-Black"), SVNonBlackPixels });
	EnumVector.push_back(SvDef::NameValuePair{ _T("Non-White"), SVNonWhitePixels });
	m_evoDrawCriteria.SetEnumTypes(EnumVector);
	m_evoDrawCriteria.SetDefaultValue( SVNone, true );

	m_bvoActivated.SetDefaultValue( BOOL(false), true);
	m_bvoContRecalc.SetDefaultValue(BOOL(false), false);
	m_dwvoMaskType.SetDefaultValue(MASK_TYPE_STATIC, true);

	m_MaskBufferInfo.SetOwnerImage(SvDef::InvalidObjectId);
	m_MaskBufferInfo.SetOwner(SvDef::InvalidObjectId);
}

bool SVUserMaskOperatorClass::CloseObject()
{
	DestroyLocalImageBuffer();
	return __super::CloseObject();
}

bool SVUserMaskOperatorClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVUnaryImageOperatorClass::CreateObject(rCreateStructure);

	const UINT cAttributes = SvPb::audittrail | SvPb::remotelySetable;
	// Set / Reset Printable Flag
	m_bvoActivated.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );
	m_evoDrawCriteria.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	m_dwvoMaskType.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	m_evoCurrentMaskOperator.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	m_evoFillArea.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	m_lvoFillColor.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	m_bvoContRecalc.SetObjectAttributesAllowed(cAttributes, SvOi::SetAttributeType::AddAttribute);

	//The return value of CreateLocalImageBuffer will be ignored here because :
	// -Currently(24.2.2021) the method can return an error(because of problems with scale factors), but it is called again in OnResetObjectand by then it works correctly.
	// - m_isCreated is only set in CreateObject and if it is set to false the object remains invalid forever.For this reason m_isCreated should only be set to failed if a problem is either
	//	persistent or will not be verified again in OnResetObject.
	CreateLocalImageBuffer();

	DWORD dwMaskType = MASK_TYPE_STATIC;
	m_dwvoMaskType.GetValue(dwMaskType);
	updateMaskImageFlags(dwMaskType);

	m_isCreated = bOk;

	return m_isCreated;
}

bool SVUserMaskOperatorClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = SVUnaryImageOperatorClass::ResetObject(pErrorMessages);

	DWORD dwMaskType = MASK_TYPE_STATIC;
	m_dwvoMaskType.GetValue(dwMaskType);
	updateMaskImageFlags(dwMaskType);

	BOOL bActive = false;
	m_bvoActivated.GetValue(bActive);

	SVShapeMaskHelperClass* pShapeHelper = GetShapeHelper();
	Log_Assert( pShapeHelper );
	if ( pShapeHelper )
	{
		Result = pShapeHelper->ResetObject(pErrorMessages) && Result;

		SvOi::SetAttributeType AddRemoveAnyType = bActive ? SvOi::SetAttributeType::AddAttribute : SvOi::SetAttributeType::RemoveAttribute;
		m_evoCurrentMaskOperator.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::viewable, AddRemoveAnyType);
		m_dwvoMaskType.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::viewable, AddRemoveAnyType);
		m_evoDrawCriteria.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::viewable, AddRemoveAnyType);

		SvOi::SetAttributeType AddRemoveShapeTypes = (bActive && dwMaskType == MASK_TYPE_SHAPE) ? SvOi::SetAttributeType::AddAttribute : SvOi::SetAttributeType::RemoveAttribute;
		pShapeHelper->SetObjectAttributesAllowed( SvPb::audittrail | SvPb::viewable, AddRemoveShapeTypes);
		pShapeHelper->SetObjectAttributesAllowed( SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute );
		m_evoFillArea.SetObjectAttributesAllowed( SvPb::audittrail | SvPb::viewable, AddRemoveShapeTypes);
		m_lvoFillColor.SetObjectAttributesAllowed( SvPb::audittrail | SvPb::viewable, AddRemoveShapeTypes);
		m_bvoContRecalc.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::viewable, AddRemoveShapeTypes);
	}
	else
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_NoShapeHelper, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
			pErrorMessages->push_back(Msg);
		}
	}

	// if image mask or overlays are turned on, set to true
	SVDrawCriteriaEnum l_eCriteria;
	long l_lValue;
	m_evoDrawCriteria.GetValue( l_lValue );
	l_eCriteria = ( SVDrawCriteriaEnum ) l_lValue;

	m_bUseOverlays = ( l_eCriteria != SVNone || dwMaskType == MASK_TYPE_IMAGE );

	if( S_OK != CreateLocalImageBuffer() )
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_CreateBufferFailed, SvStl::SourceFileParams(StdMessageParams), getObjectId() );
			pErrorMessages->push_back(Msg);
		}
	}

	return Result;
}

SVShapeMaskHelperClass* SVUserMaskOperatorClass::GetShapeHelper()
{
	// Get Friend Object
	for (size_t j = 0; j < m_friendList.size(); ++j)
	{
		if(nullptr != m_friendList[j] && SvPb::SVShapeMaskHelperObjectType == m_friendList[j]->GetObjectSubType())
		{
			return static_cast<SVShapeMaskHelperClass*>  (m_friendList[j]);
		}
	}

	return nullptr;
}

const SVShapeMaskHelperClass* SVUserMaskOperatorClass::GetShapeHelper() const
{
	// Get Friend Object
	for (size_t j = 0; j < m_friendList.size(); ++j)
	{
		if (nullptr != m_friendList[j] && SvPb::SVShapeMaskHelperObjectType == m_friendList[j]->GetObjectSubType())
		{
			return static_cast<SVShapeMaskHelperClass*>  (m_friendList[j]);
		}
	}
	return nullptr;
}

SvDef::StringVector SVUserMaskOperatorClass::getSpecialImageList() const
{
	return {SvDef::MaskImageName, SvDef::ReferenceImageName};
}

bool SVUserMaskOperatorClass::getSpecialImage(const std::string& rName, SvOi::SVImageBufferHandlePtr& rImagePtr) const
{
	if (SvDef::MaskImageName == rName)
	{
		rImagePtr = m_MaskBufferHandlePtr;
		return true;
	}
	else if (SvDef::ReferenceImageName == rName)
	{
		const SvIe::SVImageClass* pImage = getReferenceImage();
		if (pImage)
		{
			if (SvPb::SVImageTypeEnum::SVImageTypePhysical == pImage->GetImageType())
			{
				rImagePtr = pImage->GetParentImageInterface()->getLastImage();
			}
			else
			{ 
				rImagePtr = pImage->getLastImage();
			}
			return true;
		}
	}
	return false;
}

bool SVUserMaskOperatorClass::isInputImage(uint32_t imageId) const
{
	bool Result(false);

	DWORD dwMaskType = 0;
	m_dwvoMaskType.GetValue( dwMaskType );

	if (MASK_TYPE_IMAGE == dwMaskType)
	{
		SvIe::SVImageClass* pImage = getMaskInputImage();
		if (nullptr != pImage && imageId == pImage->getObjectId())
		{
			Result = true;
		}
	}
	else
	{
		if (nullptr != m_pCurrentUIOPL)
		{
			SvIe::SVImageClass* pImage = m_pCurrentUIOPL->getInputImage();
			if (nullptr != pImage && imageId == pImage->getObjectId())
			{
				Result = true;
			}
		}
	}

	return Result;
}

void SVUserMaskOperatorClass::addOverlayGroups(SVExtentMultiLineStructVector& multiLineArray, const SVImageExtentClass& rImageExtents) const
{
	if ( m_bUseOverlays )
	{
		SVExtentMultiLineStruct	multiLine;
		BuildMaskLines(multiLine, rImageExtents );
		multiLine.m_Color = RGB( 0, 255, 255 );
		multiLine.m_bDrawFigureHatched = true;
		multiLineArray.push_back(multiLine);
	}
}

HRESULT SVUserMaskOperatorClass::BuildMaskLines(SVExtentMultiLineStruct& p_MultiLine, const SVImageExtentClass& rImageExtents) const
{
	HRESULT l_hr = S_OK;

	// Multi Lines instead of points.
	p_MultiLine.Initialize();

	// Get Value Objects.
	BOOL Activated;
	m_bvoActivated.GetValue( Activated );

	SVDrawCriteriaEnum l_eCriteria;
	long l_lValue;
	m_evoDrawCriteria.GetValue(l_lValue );
	l_eCriteria = ( SVDrawCriteriaEnum ) l_lValue;

	DWORD dwMaskType;
	m_dwvoMaskType.GetValue( dwMaskType );

	SvIe::SVImageClass* pInputImage = getMaskInputImage();
	if (l_eCriteria != SVNone && Activated && nullptr != m_MaskBufferHandlePtr &&
		(MASK_TYPE_IMAGE != dwMaskType || nullptr != pInputImage))
	{
		RECT rec;
		l_hr = m_MaskBufferInfo.GetOutputRectangle(rec);
		// User Mask draw with lines 

		unsigned char* pSrcLine = nullptr;
		SVMatroxBufferInterface::GetHostAddress(&pSrcLine, m_MaskBufferHandlePtr->GetBuffer());
		long l_lSrcBytes;
		SVMatroxBufferInterface::Get(m_MaskBufferHandlePtr->GetBuffer(), SVPitchByte, l_lSrcBytes);

		long l_lSkip = 1;

		SVExtentLineStruct l_Line;
		l_Line.m_dwColor = RGB(0, 255, 255);

		for (int l_iRow = 0; l_iRow < rec.bottom; l_iRow += l_lSkip)
		{
			SVPoint<double> startPoint;

			bool l_bDrewLastPixel = false;

			for (int l_iCol = 0; l_iCol < rec.right; l_iCol++)
			{
				switch (l_eCriteria)
				{
					case  SVNonBlackPixels:
					{
						if (pSrcLine[l_iCol] != 0)
						{
							if (!l_bDrewLastPixel)
							{
								l_bDrewLastPixel = true;
								startPoint.m_x = l_iCol;
								startPoint.m_y = l_iRow;
								rImageExtents.TranslateFromOutputSpace(startPoint, startPoint);
							}
						}
						else if (l_bDrewLastPixel)
						{
							l_bDrewLastPixel = false;
							AddLine(l_iCol, l_iRow, startPoint, rImageExtents, l_Line, p_MultiLine);
						}
						break;
					}
					case SVNonWhitePixels:
					{
						if (pSrcLine[l_iCol] != 255)
						{
							if (!l_bDrewLastPixel)
							{
								l_bDrewLastPixel = true;
								startPoint.m_x = l_iCol;
								startPoint.m_y = l_iRow;
								rImageExtents.TranslateFromOutputSpace(startPoint, startPoint);
							}
						}
						else if (l_bDrewLastPixel)
						{
							l_bDrewLastPixel = false;
							AddLine(l_iCol, l_iRow, startPoint, rImageExtents, l_Line, p_MultiLine);
						}
						break;
					}
				}
			}//for( int l_iCol = 0 ; l_iCol < rec.right ; l_iCol++ )

			if (l_bDrewLastPixel)  // Finish the last line if the pixels were the same to the end.
			{
				AddLine(rec.right - 1, l_iRow, startPoint, rImageExtents, l_Line, p_MultiLine);
			}

			pSrcLine += l_lSrcBytes * l_lSkip;
		}//for( int l_iRow = 0 ; l_iRow < rec.bottom ; l_iRow += l_lSkip )

		p_MultiLine.m_Color = RGB(0, 255, 255);
	}

	return l_hr;
}

void SVUserMaskOperatorClass::AddLine(int iCol, int iRow, const SVPoint<double>& rStartPoint, const SVImageExtentClass& rExtent, SVExtentLineStruct& rLine, SVExtentMultiLineStruct& rMultiLine) const
{
	SVPoint<double> endPoint{static_cast<double> (iCol),  static_cast<double> (iRow)};
	rExtent.TranslateFromOutputSpace(endPoint, endPoint);
	if(rLine.m_PointVector.size() < 2)
	{
		rLine.m_PointVector.resize(2);
	}
	rLine.m_PointVector[0] = rStartPoint ;
	rLine.m_PointVector[1] = endPoint ;
	rMultiLine.m_svLineArray.emplace_back( rLine );
}
	
HRESULT SVUserMaskOperatorClass::CreateLocalImageBuffer()
{
	HRESULT l_hrOk = S_OK;

	const SvIe::SVImageClass *pImage = getOutputImage();

	if( nullptr != pImage )
	{
		SVImageInfoClass l_MaskBufferInfo = pImage->GetImageInfo();

		bool bImageInfoChanged = (l_MaskBufferInfo != m_MaskBufferInfo);
		if (bImageInfoChanged)
		{
			m_MaskBufferInfo = l_MaskBufferInfo;
			m_MaskBufferInfo.setDibBufferFlag(true);

			DestroyLocalImageBuffer();
			l_hrOk = SvIe::SVImageProcessingClass::CreateImageBuffer(m_MaskBufferInfo, m_MaskBufferHandlePtr);
		}

		SVShapeMaskHelperClass* pShape = GetShapeHelper();
		if (nullptr != pShape)
		{
			BOOL isContRecalc;
			m_bvoContRecalc.GetValue(isContRecalc);
			DWORD dwMaskType = MASK_TYPE_STATIC;
			m_dwvoMaskType.GetValue(dwMaskType);
			long lCriteria;
			m_evoDrawCriteria.GetValue(lCriteria);
			pShape->createImageObject(MASK_TYPE_SHAPE == dwMaskType && isContRecalc && lCriteria != SVNone);
		}

		if( S_OK == l_hrOk )
		{
			if( ! Refresh() )
			{
				l_hrOk = S_FALSE;
			}
		}
	}

	return l_hrOk;
}

void SVUserMaskOperatorClass::DestroyLocalImageBuffer()
{
	m_MaskBufferHandlePtr.reset();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Refresh
// -----------------------------------------------------------------------------
// .Description : Refresh mask buffer...
//              :
////////////////////////////////////////////////////////////////////////////////
bool SVUserMaskOperatorClass::Refresh()
{
	if( nullptr != GetInspectionInterface() )
	{
		DWORD dwMaskType;
		m_dwvoMaskType.GetValue( dwMaskType );
		SVShapeMaskHelperClass* pShape = GetShapeHelper();
		if ( nullptr != pShape && ( MASK_TYPE_SHAPE == dwMaskType ) )
		{
			pShape->Refresh();
			return true;
		}
		else
		{
			// Init buffer to 0...
			// &&&
			if ( S_OK == SvIe::SVImageProcessingClass::InitBuffer( m_MaskBufferHandlePtr ) )
			{
				HDC dc;
				// Get DC...
				dc = SvIe::SVImageProcessingClass::CreateBufferDC( m_MaskBufferInfo, m_MaskBufferHandlePtr );
				if(dc != (HDC)0)        // && dc != (HDC)-1)
				{
					// Draw mask...
					RECT rect;

					SVImageExtentClass ex = m_MaskBufferInfo.GetExtents();

					ex.GetOutputRectangle( rect );

					m_graphixObject.Draw( dc, rect );

	#ifdef _DEBUG999
				{
					// Dump the images to .bmp files
					try
					{
						/*HRESULT l_Code = */
						SVMatroxBufferInterface::Export( m_MaskBufferHandlePtr.milImage, ( char* )_T("C:\\Temp\\MaskBuffer.bmp"), ImageFileFormat::bmp );
					}
					catch(...)
					{
					}

				}
	#endif //_DEBUG

					// Release DC...
					return S_OK == SvIe::SVImageProcessingClass::DestroyBufferDC( m_MaskBufferHandlePtr, dc );
				}// end if(dc != (HDC)0) 
			}
		}// end else if not shape
	}// end if( GetIPDoc() )
	return false;
}

void SVUserMaskOperatorClass::Persist( SvOi::IObjectWriter& rWriter, bool /*closeObject = true*/) const
{
	SVTaskObjectClass::Persist(rWriter);
	
	// Add Script of SVGraphixClass as a hex string
	// Prepare Archive...
	std::string data = m_graphixObject.store(true);

	_variant_t value;
	value.SetString(data.c_str());
	rWriter.WriteAttribute(scMaskDataTag, value);
	value.Clear();
}

// Restoration of trivial members
HRESULT SVUserMaskOperatorClass::SetObjectValue( SVObjectAttributeClass* pDataObject )
{
	HRESULT hr = S_FALSE;
	
	SvCl::SVObjectStdStringArrayClass StringArray;
	SvCl::SVObjectLongArrayClass svLongArray;
	bool bOk{pDataObject->GetAttributeData( _T("MaskOperator"), svLongArray)};

	if (bOk)
	{
		for( int i = 0; i < static_cast<int> (svLongArray.size()); i++ )
		{
			m_evoCurrentMaskOperator.SetValue(svLongArray[i]);
		}
	}
	else if (true == (bOk = pDataObject->GetAttributeData( _T("MaskData"), StringArray)))
	{
		for( int i = 0; i < static_cast<int> (StringArray.size()); i++ )
		{
			m_graphixObject.load(StringArray[i], true);
		}
	}
	else
	{
		hr = SVUnaryImageOperatorClass::SetObjectValue( pDataObject );
		return hr;
	}

	hr = bOk ? S_OK : S_FALSE;
	return hr;
}


SvStl::MessageTextEnum SVUserMaskOperatorClass::onRunImageType(SvIe::RunStatus& rRunStatus)
{
	if (nullptr == m_MaskBufferHandlePtr)
	{
		m_lastErrorPosData = SvStl::SourceFileParams(StdMessageParams);
		return SvStl::Tid_ErrorGettingInputs;
	}

	SvIe::SVImageClass* pMaskInputImage = getMaskInputImage(true);
	const SvIe::SVImageClass* pRefImage = getOutputImage(true);

	if (nullptr != pRefImage &&
		nullptr != pMaskInputImage)
	{
		SVPoint<double> point;

		SvOi::ITRCImagePtr pMaskBuffer = pMaskInputImage->getImageReadOnly(rRunStatus.m_triggerRecord.get());

		const SVImageExtentClass& rExtents = pRefImage->GetImageExtents();

		if (S_OK == rExtents.GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, point) &&
			nullptr != pMaskBuffer && !pMaskBuffer->isEmpty())
		{
			if (S_OK != pMaskInputImage->ValidateAgainstOutputExtents(rExtents))
			{
				HRESULT MatroxCode = SVMatroxBufferInterface::ClearBuffer(m_MaskBufferHandlePtr->GetBuffer(), 0.0);

				if (S_OK != MatroxCode)
				{
					m_lastErrorPosData = SvStl::SourceFileParams(StdMessageParams);
					return SvStl::Tid_UpdateBufferFailed;
				}
			}

			HRESULT MatroxCode = SVMatroxBufferInterface::CopyBuffer(m_MaskBufferHandlePtr->GetBuffer(), pMaskBuffer->getHandle()->GetBuffer(), static_cast<long> (-point.m_x), static_cast<long> (-point.m_y));

			if (S_OK != MatroxCode)
			{
				m_lastErrorPosData = SvStl::SourceFileParams(StdMessageParams);
				return SvStl::Tid_CopyImagesFailed;
			}
		}
		else
		{
			m_lastErrorPosData = SvStl::SourceFileParams(StdMessageParams);
			return SvStl::Tid_ErrorGettingInputs;
		}
		return SvStl::Tid_Empty;
	}
	else
	{
		m_lastErrorPosData = SvStl::SourceFileParams(StdMessageParams);
		return SvStl::Tid_ErrorGettingInputs;
	}
}

SvStl::MessageTextEnum SVUserMaskOperatorClass::onRunMask(bool First, SvOi::SVImageBufferHandlePtr rInputImageHandle, SvOi::SVImageBufferHandlePtr rOutputImageHandle, SvIe::RunStatus& rRunStatus, DWORD dwMaskType)
{
	if (dwMaskType == MASK_TYPE_IMAGE)
	{
		auto errorCode = onRunImageType(rRunStatus);
		if (SvStl::Tid_Empty != errorCode)
		{
			return errorCode;
		}
	}// end if ( dwMaskType == MASK_TYPE_IMAGE )
	else if (dwMaskType == MASK_TYPE_SHAPE)
	{
		SVShapeMaskHelperClass* pShapeHelper = GetShapeHelper();
		BOOL isContRecalc;
		m_bvoContRecalc.GetValue(isContRecalc);
		if (isContRecalc)
		{
			pShapeHelper->Refresh(&rRunStatus);
		}
	}

	long lMaskOperator = SVImageAnd;
	m_evoCurrentMaskOperator.GetValue(lMaskOperator);

	if (nullptr == m_MaskBufferHandlePtr)
	{
		m_lastErrorPosData = SvStl::SourceFileParams(StdMessageParams);
		return SvStl::Tid_ErrorGettingInputs;
	}
	HRESULT MatroxCode = SVMatroxImageInterface::Arithmetic(rOutputImageHandle->GetBuffer(),
		First ? rInputImageHandle->GetBuffer() : rOutputImageHandle->GetBuffer(),
		m_MaskBufferHandlePtr->GetBuffer(),
		static_cast<SVImageOperationTypeEnum>(lMaskOperator));


	if (S_OK != MatroxCode)
	{
		m_lastErrorPosData = SvStl::SourceFileParams(StdMessageParams);
		return SvStl::Tid_RunArithmeticFailed;
	}

	return SvStl::Tid_Empty;
}

bool SVUserMaskOperatorClass::onRun( bool First, SvOi::SVImageBufferHandlePtr rInputImageHandle, SvOi::SVImageBufferHandlePtr rOutputImageHandle, SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{ 
	BOOL bActive;
	m_bvoActivated.GetValue( bActive );

	if( bActive && nullptr != rInputImageHandle && nullptr != rOutputImageHandle )
	{
		DWORD dwMaskType = MASK_TYPE_STATIC;
		m_dwvoMaskType.GetValue(dwMaskType);
		auto errorCode = onRunMask(First, rInputImageHandle, rOutputImageHandle, rRunStatus, dwMaskType);

		if (SvStl::Tid_Empty != errorCode)
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, errorCode, m_lastErrorPosData, getObjectId());
				pErrorMessages->push_back(Msg);
			}
			// Signal that something was wrong...
			rRunStatus.SetInvalid();
		}

		SVShapeMaskHelperClass* pShapeHelper = GetShapeHelper();
		if (pShapeHelper)
		{
			pShapeHelper->onRun(rRunStatus, pErrorMessages);
			DWORD dwShapeColor = m_statusColor.GetDefaultValue();
			if (MASK_TYPE_SHAPE == dwMaskType)
			{
				dwShapeColor = rRunStatus.GetStatusColor();
			}
			pShapeHelper->m_statusColor.SetValue(dwShapeColor);
		}

		return SvStl::Tid_Empty == errorCode;
	}

	// Was deactivated, but everything is ok !!!
	// So just return false, don't invalidate !!!
	SVShapeMaskHelperClass* pShapeHelper = GetShapeHelper();
	if ( pShapeHelper )
	{
		DWORD dwShapeColor = m_statusColor.GetDefaultValue();
		pShapeHelper->m_statusColor.SetValue(dwShapeColor);
	}
	return false;
}

void SVUserMaskOperatorClass::addOverlayGroups(SvPb::Overlay& rOverlay) const
{
	BOOL activated;
	m_bvoActivated.GetValue(activated);

	SVDrawCriteriaEnum eCriteria;
	long lValue;
	m_evoDrawCriteria.GetValue(lValue);
	eCriteria = (SVDrawCriteriaEnum)lValue;

	if (eCriteria != SVNone && activated)
	{
		DWORD dwMaskType;
		m_dwvoMaskType.GetValue(dwMaskType);

		bool useStaticMask = false;
		SvIe::SVImageClass* pMaskImage = nullptr;
		switch (dwMaskType)
		{
		case MASK_TYPE_STATIC:
			useStaticMask = true;
			break;
		case MASK_TYPE_IMAGE:
			pMaskImage = getMaskInputImage();
			break;
		case MASK_TYPE_SHAPE:
			BOOL isContRecalc;
			m_bvoContRecalc.GetValue(isContRecalc);
			if (isContRecalc)
			{
				const SVShapeMaskHelperClass* pShapeHelper = GetShapeHelper();
				Log_Assert(pShapeHelper);
				if (pShapeHelper)
				{
					pMaskImage = pShapeHelper->getImage();
				}
			}
			else
			{
				useStaticMask = true;
			}
			break;
		default:
			break;
		}
		RECT rec;
		m_MaskBufferInfo.GetOutputRectangle(rec);
		long height{ rec.bottom - rec.top };
		long width{ rec.right - rec.left };
		if (useStaticMask)
		{
			if (m_MaskBufferHandlePtr && false == m_MaskBufferHandlePtr->empty())
			{
				unsigned char* pSrcLine = nullptr;
				SVMatroxBufferInterface::GetHostAddress(&pSrcLine, m_MaskBufferHandlePtr->GetBuffer());
				if (0 < height && 0 < width)
				{
					auto& shapeGroup = *rOverlay.add_shapegroups();
					shapeGroup.set_name("Mask");
					shapeGroup.set_detaillevel(SvPb::Level1);
					auto* pImageData = SvPb::generateBmpString(*shapeGroup.add_shapes(), rec);
					for (int y = 0; y < height; ++y)
					{
						for (int x = 0; x < width; ++x)
						{
							int offsetSrc = (y * width) + x;
							int offsetDest = (4 * y * width) + (4 * x);
							bool isSet = (SVNonBlackPixels == eCriteria && pSrcLine[offsetSrc]) || (SVNonWhitePixels == eCriteria && 0 == pSrcLine[offsetSrc]);
							pImageData[offsetDest + 0] = 0; // red
							pImageData[offsetDest + 1] = isSet ? 255: 0; // green
							pImageData[offsetDest + 2] = isSet ? 255: 0; // blue
							pImageData[offsetDest + 3] = 100; // alpha
						}
					}
				}
			}
		}
		else if (pMaskImage)
		{
			auto& shapeGroup = *rOverlay.add_shapegroups();
			shapeGroup.set_name("Mask");
			shapeGroup.set_detaillevel(SvPb::Level1);
			auto* pImage = shapeGroup.add_shapes()->mutable_image();
			pImage->mutable_x()->set_value(rec.top);
			pImage->mutable_y()->set_value(rec.left);
			pImage->mutable_w()->set_value(width);
			pImage->mutable_h()->set_value(height);
			auto* pData = pImage->mutable_imagedymdata();
			pData->set_trposimage(pMaskImage->getImagePosInTRC());
			if (SVNonBlackPixels == eCriteria)
			{
				pData->set_nonblack(0x64FFFF00);
			}
			else if (SVNonWhitePixels == eCriteria)
			{
				pData->set_black(0x64FFFF00);
			}			
		}		
	}
}

SvIe::SVImageClass* SVUserMaskOperatorClass::getMaskInputImage(bool bRunMode /*= false*/) const
{
	return m_userMaskImageInput.getInput<SvIe::SVImageClass>(bRunMode);
}

#pragma region IMask
HRESULT SVUserMaskOperatorClass::Import(const std::string& rFileName)
{
	HRESULT result{S_OK};

	std::ifstream importFile{rFileName.c_str(), std::ifstream::in | std::ifstream::binary | std::ifstream::ate};

	if (importFile.is_open())
	{
		std::string fileData;
		size_t fileSize(0);
		fileSize = static_cast<size_t> (importFile.tellg());
		if(fileSize > 0)
		{
			fileData.resize(fileSize);
			importFile.seekg(0, std::ios::beg);
			importFile.read(&fileData[0], fileSize);
			m_graphixObject.load(fileData);
		}
		importFile.close();

		Refresh(); 
	}
	else
	{
		result = E_INVALIDARG;
	}

	return result;
}

HRESULT SVUserMaskOperatorClass::Export(const std::string& rFileName)
{
	HRESULT result{S_OK};

	std::ofstream exportFile;

	exportFile.open(rFileName.c_str(), std::ofstream::binary);

	if (exportFile.is_open())
	{
		std::string data = m_graphixObject.store();
		if(data.size() > 0)
		{
			exportFile.write(&data[0], data.size());
		}
		exportFile.close();
	}
	else
	{
		result = E_INVALIDARG;
	}
	return result;
}
	
HGLOBAL SVUserMaskOperatorClass::GetMaskData() const
{
	return m_graphixObject.GetGraphixData();
}

bool SVUserMaskOperatorClass::SetMaskData(HGLOBAL hGlobal)
{
	return m_graphixObject.SetGraphixData(hGlobal) ? true : false;
}

void SVUserMaskOperatorClass::updateMaskImageFlags(DWORD dwMaskType)
{
	if (dwMaskType == MASK_TYPE_IMAGE)
	{
		m_userMaskImageInput.validateInput();
		m_userMaskImageInput.SetObjectAttributesAllowed(SvPb::audittrail | SvPb::useable, SvOi::SetAttributeType::OverwriteAttribute);
	}
	else
	{
		m_userMaskImageInput.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);
	}
}

#pragma endregion IMask

} //namespace SvOp
