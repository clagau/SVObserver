//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Harrisburg
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
#include "Definitions/TextDefineSvDef.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "InspectionEngine/SVTaskObjectInterfaceInputRequestStruct.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVMatroxLibrary/SVMatroxImageInterface.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "SVObjectLibrary/SVToolsetScriptTags.h"
#include "SVStatusLibrary/SVRunStatus.h"
#include "SVUtilityLibrary/SVSafeArray.h"
#include "Tools/SVTool.h"
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
	m_outObjectInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVUnaryImageOperatorObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVUserMaskOperatorObjectType;

	m_inObjectInfo.SetInputObjectType(SvPb::SVImageObjectType, SvPb::SVImageMonoType);
	m_inObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_inObjectInfo, _T( "UserMaskImage" ) );


	// RRRRGGGGHHHHHH who created this stupid object structure????
	SVShapeMaskHelperClass* pShapeHelper = new SVShapeMaskHelperClass(this);
	m_idShapeHelper = pShapeHelper->getObjectId();

	bool bAddFriend = AddFriend( pShapeHelper->getObjectId() );
	assert( bAddFriend );	UNREFERENCED_PARAMETER(bAddFriend);


	RegisterEmbeddedObject( &m_Data.bvoActivated, SvPb::MaskEnabledEId, IDS_OBJECTNAME_ENABLED, false, SvOi::SVResetItemIP );
	RegisterEmbeddedObject( &m_Data.dwvoMaskType, SvPb::MaskUseImageMaskEId, IDS_MASK_TYPE, false, SvOi::SVResetItemIP );
	RegisterEmbeddedObject( &m_Data.evoCurrentMaskOperator, SvPb::MaskOperatorEId, IDS_OBJECTNAME_MASK_OPERATOR, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject( &m_Data.evoFillArea, SvPb::MaskFillAreaEId, IDS_OBJECTNAME_MASK_FILL_AREA, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject( &m_Data.lvoFillColor, SvPb::MaskFillColorEId, IDS_OBJECTNAME_MASK_FILL_COLOR, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject( &m_Data.evoDrawCriteria, SvPb::DrawCriteriaEId, IDS_OBJECTNAME_DRAW_CRITERIA, false, SvOi::SVResetItemOwner );

	SvOi::NameValueVector EnumVector
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
	m_Data.evoCurrentMaskOperator.SetEnumTypes(EnumVector);
	m_Data.evoCurrentMaskOperator.SetDefaultValue(SVImageAnd, true);

	EnumVector.clear();
	EnumVector.push_back(SvOi::NameValuePair{ _T("Mask Area"), 0 });
	EnumVector.push_back(SvOi::NameValuePair{ _T("Background"), 1 });
	m_Data.evoFillArea.SetEnumTypes(EnumVector);
	m_Data.evoFillArea.SetDefaultValue(0l, true);	// mask area is default

	m_Data.lvoFillColor.SetDefaultValue(0, true);	// black default

	EnumVector.clear();
	EnumVector.push_back(SvOi::NameValuePair{ _T("None"), SVNone });
	EnumVector.push_back(SvOi::NameValuePair{ _T("Non-Black"), SVNonBlackPixels });
	EnumVector.push_back(SvOi::NameValuePair{ _T("Non-White"), SVNonWhitePixels });
	m_Data.evoDrawCriteria.SetEnumTypes(EnumVector);
	m_Data.evoDrawCriteria.SetDefaultValue( SVNone, true );

	m_Data.bvoActivated.SetDefaultValue( BOOL(false), true);
	m_Data.dwvoMaskType.SetDefaultValue(MASK_TYPE_STATIC, true);

	m_MaskBufferInfo.SetOwnerImage(SvDef::InvalidObjectId);
	m_MaskBufferInfo.SetOwner(SvDef::InvalidObjectId);

	// Set default inputs and outputs
	addDefaultInputObjects();
}

bool SVUserMaskOperatorClass::CloseObject()
{
	bool bOk = S_OK == DestroyLocalImageBuffer();

	if ( m_isCreated )
	{
		m_isCreated = false;

		bOk = SVUnaryImageOperatorClass::CloseObject();
	}

	return bOk;
}

bool SVUserMaskOperatorClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVUnaryImageOperatorClass::CreateObject(rCreateStructure);

	const UINT cAttributes = SvPb::printable | SvPb::remotelySetable | SvPb::setableOnline;
	// Set / Reset Printable Flag
	m_Data.bvoActivated.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );
	m_Data.evoDrawCriteria.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	m_Data.dwvoMaskType.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	m_Data.evoCurrentMaskOperator.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	m_Data.evoFillArea.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	m_Data.lvoFillColor.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );

	m_isCreated = bOk;

	return m_isCreated;
}

bool SVUserMaskOperatorClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = SVUnaryImageOperatorClass::ResetObject(pErrorMessages);

	SvOl::ValidateInput(m_inObjectInfo);

	DWORD dwMaskType;
	m_Data.dwvoMaskType.GetValue( dwMaskType );
	BOOL bActive = false;
	m_Data.bvoActivated.GetValue(bActive);

	SVShapeMaskHelperClass* pShapeHelper = GetShapeHelper();
	assert( pShapeHelper );
	if ( pShapeHelper )
	{
		Result = pShapeHelper->ResetObject(pErrorMessages) && Result;

		SvOi::SetAttributeType AddRemoveType = (dwMaskType == MASK_TYPE_SHAPE) ? SvOi::SetAttributeType::AddAttribute : SvOi::SetAttributeType::RemoveAttribute;
		pShapeHelper->SetObjectAttributesAllowed( SvPb::printable | SvPb::viewable, AddRemoveType );
		pShapeHelper->SetObjectAttributesAllowed( SvPb::setableOnline | SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute );
		AddRemoveType = (dwMaskType == MASK_TYPE_SHAPE) && bActive ? SvOi::SetAttributeType::AddAttribute : SvOi::SetAttributeType::RemoveAttribute;
		m_Data.evoFillArea.SetObjectAttributesAllowed( SvPb::printable | SvPb::viewable, AddRemoveType  );
		m_Data.lvoFillColor.SetObjectAttributesAllowed( SvPb::printable | SvPb::viewable, AddRemoveType  );
	}
	else
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_NoShapeHelper, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
			pErrorMessages->push_back(Msg);
		}
	}

	// if image mask or overlays are turned on, set to true
	SVDrawCriteriaEnum l_eCriteria;
	long l_lValue;
	m_Data.evoDrawCriteria.GetValue( l_lValue );
	l_eCriteria = ( SVDrawCriteriaEnum ) l_lValue;

	m_bUseOverlays = ( l_eCriteria != SVNone || dwMaskType == MASK_TYPE_IMAGE );

	if( S_OK != CreateLocalImageBuffer() )
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_CreateBufferFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
			pErrorMessages->push_back(Msg);
		}
	}

	return Result;
}

HRESULT SVUserMaskOperatorClass::GetFillProperties(SVMaskFillPropertiesStruct& rFillStruct)
{
	long lValue=0;
	m_Data.evoFillArea.GetValue( lValue );
	rFillStruct.bFillMaskArea = (lValue == 0 ? true : false);
	m_Data.lvoFillColor.GetValue( lValue );
	rFillStruct.rgbFillColor = RGB(lValue, lValue, lValue);

	SVShapeMaskHelperClass* pShapeHelper = GetShapeHelper();
	if ( pShapeHelper )
	{
		pShapeHelper->GetFillProperties( rFillStruct );
	}
	return S_OK;
}

SVShapeMaskHelperClass* SVUserMaskOperatorClass::GetShapeHelper()
{
	SVShapeMaskHelperClass* pMaskHelper = nullptr;

	// Get Friend Object
	for( size_t i = 0; i < m_friendList.size(); i++ )
	{
		const SVObjectInfoStruct& friendObjectInfo = m_friendList[i];
		if(nullptr != (pMaskHelper = dynamic_cast<SVShapeMaskHelperClass*> (friendObjectInfo.getObject())))
		{
			m_idShapeHelper = pMaskHelper->getObjectId();
			break;
		}
	}

	//!! temp workaround for 4.60 Beta 4
	if ( nullptr == pMaskHelper )
	{
		SVShapeMaskHelperClass* pShapeHelper = new SVShapeMaskHelperClass(this);
		m_idShapeHelper = pShapeHelper->getObjectId();

		bool bAddFriend = AddFriend( pShapeHelper->getObjectId() );
		assert(bAddFriend);	UNREFERENCED_PARAMETER(bAddFriend);

		if( CreateChildObject(pShapeHelper) )
		{
			pMaskHelper = pShapeHelper;
		}
	}

	assert( pMaskHelper );
	return pMaskHelper;
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
				rImagePtr = pImage->GetParentImageInterface()->getImageData();
			}
			else
			{ 
				rImagePtr = pImage->getImageData();
			}
			return true;
		}
	}
	return false;
}

HRESULT SVUserMaskOperatorClass::GetCancelData(SvIe::SVInputRequestStructMap& rMap)
{
	HRESULT hr = S_OK;

	rMap.Add( &m_Data.evoCurrentMaskOperator );
	rMap.Add( &m_Data.bvoActivated );
	rMap.Add( &m_Data.dwvoMaskType );
	rMap.Add( &m_Data.evoFillArea );
	rMap.Add( &m_Data.lvoFillColor );
	rMap.Add( &m_Data.evoDrawCriteria );

	SVShapeMaskHelperClass* pShapeHelper = GetShapeHelper();
	if ( pShapeHelper )
	{
		pShapeHelper->GetCancelData( rMap );
	}

	return hr;
}

bool SVUserMaskOperatorClass::isInputImage(uint32_t imageId) const
{
	bool Result(false);

	DWORD dwMaskType = 0;
	m_Data.dwvoMaskType.GetValue( dwMaskType );

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

HRESULT SVUserMaskOperatorClass::onCollectOverlays(SvIe::SVImageClass*, SVExtentMultiLineStructVector& p_MultiLineArray )
{
	HRESULT l_hr = S_FALSE;
	if ( m_bUseOverlays )
	{
		SVExtentMultiLineStruct	l_MultiLine;

		BuildMaskLines( l_MultiLine );
		l_MultiLine.m_Color = RGB( 0, 255, 255 );
		l_MultiLine.m_bDrawFigureHatched = true;
		p_MultiLineArray.push_back( l_MultiLine );
		l_hr = S_OK;
	}
	return l_hr;
}

HRESULT SVUserMaskOperatorClass::BuildMaskLines( SVExtentMultiLineStruct& p_MultiLine  )
{
	HRESULT l_hr = S_OK;

	// Multi Lines instead of points.
	p_MultiLine.Initialize();

	// Get Value Objects.
	BOOL Activated;
	m_Data.bvoActivated.GetValue( Activated );

	SVDrawCriteriaEnum l_eCriteria;
	long l_lValue;
	m_Data.evoDrawCriteria.GetValue(l_lValue );
	l_eCriteria = ( SVDrawCriteriaEnum ) l_lValue;

	DWORD dwMaskType;
	m_Data.dwvoMaskType.GetValue( dwMaskType );

	SvIe::SVImageClass* pInputImage = getMaskInputImage();
	if( l_eCriteria != SVNone && Activated && nullptr != m_MaskBufferHandlePtr &&
		( MASK_TYPE_IMAGE != dwMaskType || nullptr != pInputImage ) )
	{
		SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*>(GetTool());
		if( nullptr != pTool)
		{
			const SVImageExtentClass& rImageExtents = pTool->GetImageExtent();

			RECT l_rec;

			l_hr = m_MaskBufferInfo.GetOutputRectangle( l_rec );
			// User Mask draw with lines 

			unsigned char* pSrcLine = nullptr;
			SVMatroxBufferInterface::GetHostAddress( &pSrcLine, m_MaskBufferHandlePtr->GetBuffer() );
			long l_lSrcBytes;
			SVMatroxBufferInterface::Get(m_MaskBufferHandlePtr->GetBuffer(), SVPitchByte, l_lSrcBytes );

			long l_lSkip = 1;
		
			SVExtentLineStruct l_Line;
			l_Line.m_dwColor = RGB( 0, 255, 255 );

			for( int l_iRow = 0 ; l_iRow < l_rec.bottom ; l_iRow += l_lSkip )
			{
				SVPoint<double> startPoint;

				bool l_bDrewLastPixel = false;

				for( int l_iCol = 0 ; l_iCol < l_rec.right ; l_iCol++ )
				{
					switch( l_eCriteria )
					{
						case  SVNonBlackPixels :
						{
							if( pSrcLine[l_iCol] != 0 ) 
							{
								if( ! l_bDrewLastPixel )
								{
									l_bDrewLastPixel = true;
									startPoint.m_x = l_iCol;
									startPoint.m_y = l_iRow;
									rImageExtents.TranslateFromOutputSpace( startPoint, startPoint );
								}
							}
							else if( l_bDrewLastPixel )
							{
								l_bDrewLastPixel = false;
								AddLine( l_iCol, l_iRow, startPoint, rImageExtents, l_Line, p_MultiLine  );
							}
							break;
						}
						case SVNonWhitePixels :
						{
							if( pSrcLine[l_iCol] != 255 )
							{							
								if( ! l_bDrewLastPixel )
								{
									l_bDrewLastPixel = true;
									startPoint.m_x = l_iCol;
									startPoint.m_y = l_iRow;
									rImageExtents.TranslateFromOutputSpace( startPoint, startPoint );
								}
							}
							else if( l_bDrewLastPixel ) 
							{
								l_bDrewLastPixel = false;
								AddLine( l_iCol, l_iRow, startPoint, rImageExtents, l_Line, p_MultiLine  );
							}
							break;
						}
					}
				}//for( int l_iCol = 0 ; l_iCol < l_rec.right ; l_iCol++ )

				if( l_bDrewLastPixel )  // Finish the last line if the pixels were the same to the end.
				{
					AddLine( l_rec.right-1, l_iRow, startPoint, rImageExtents, l_Line, p_MultiLine );
				}

				pSrcLine += l_lSrcBytes*l_lSkip;
			}//for( int l_iRow = 0 ; l_iRow < l_rec.bottom ; l_iRow += l_lSkip )

			p_MultiLine.m_Color = RGB( 0, 255, 255 );
		}
		else
		{
			l_hr = E_POINTER;
		}
	}

	return l_hr;
}

HRESULT SVUserMaskOperatorClass::AddLine(int iCol, int iRow, const SVPoint<double>& rStartPoint, const SVImageExtentClass& rExtent, SVExtentLineStruct& rLine, SVExtentMultiLineStruct& rMultiLine  )
{
	HRESULT l_hr = S_OK;

	SVPoint<double> endPoint{static_cast<double> (iCol),  static_cast<double> (iRow)};
	rExtent.TranslateFromOutputSpace(endPoint, endPoint);
	if(rLine.m_PointVector.size() < 2)
	{
		rLine.m_PointVector.resize(2);
	}
	rLine.m_PointVector[0] = rStartPoint ;
	rLine.m_PointVector[1] = endPoint ;
	rMultiLine.m_svLineArray.emplace_back( rLine );
	return l_hr;
}
	
HRESULT SVUserMaskOperatorClass::CreateLocalImageBuffer()
{
	HRESULT l_hrOk = S_OK;

	const SvIe::SVImageClass *pImage = getOutputImage();

	if( nullptr != pImage )
	{
		SVImageInfoClass l_MaskBufferInfo = pImage->GetImageInfo();

		bool bImageInfoChanged = (l_MaskBufferInfo != m_MaskBufferInfo);
		if ( bImageInfoChanged )
		{
			m_MaskBufferInfo = l_MaskBufferInfo;
			m_MaskBufferInfo.setDibBufferFlag(true);
			/*l_hrOk = */DestroyLocalImageBuffer();
			l_hrOk = SvIe::SVImageProcessingClass::CreateImageBuffer( m_MaskBufferInfo, m_MaskBufferHandlePtr );
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

HRESULT SVUserMaskOperatorClass::DestroyLocalImageBuffer()
{
	HRESULT l_hrOk = S_OK;

	m_MaskBufferHandlePtr.reset();

	return l_hrOk;
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
		m_Data.dwvoMaskType.GetValue( dwMaskType );
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
						SVMatroxBufferInterface::Export( m_MaskBufferHandlePtr.milImage, ( char* )_T("C:\\Temp\\MaskBuffer.bmp"), SVFileBitmap );
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

void SVUserMaskOperatorClass::Persist( SvOi::IObjectWriter& rWriter )
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
			m_Data.evoCurrentMaskOperator.SetValue(svLongArray[i]);
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

////////////////////////////////////////////////////////////////////////////////
// .Title       : onRun
// -----------------------------------------------------------------------------
// .Description : Runs this operator.
//              : Returns false, if operator cannot run ( may be deactivated ! )
////////////////////////////////////////////////////////////////////////////////
bool SVUserMaskOperatorClass::onRun( bool First, SvOi::SVImageBufferHandlePtr rInputImageHandle, SvOi::SVImageBufferHandlePtr rOutputImageHandle, SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{ 
	BOOL bActive;
	m_Data.bvoActivated.GetValue( bActive );

	if( bActive && nullptr != rInputImageHandle && nullptr != rOutputImageHandle )
	{
		if( nullptr != m_MaskBufferHandlePtr )
		{
			HRESULT MatroxCode;

			DWORD dwMaskType = MASK_TYPE_STATIC;
			m_Data.dwvoMaskType.GetValue( dwMaskType );

			if ( dwMaskType == MASK_TYPE_IMAGE )
			{
				SvIe::SVImageClass* pMaskInputImage = getMaskInputImage(true);
				const SvIe::SVImageClass* pRefImage = getOutputImage(true);

				if ( nullptr != pRefImage &&
					nullptr != pMaskInputImage )
				{
					SVPoint<double> point;

					SvTrc::IImagePtr pMaskBuffer = pMaskInputImage->getImageReadOnly(rRunStatus.m_triggerRecord.get());

					const SVImageExtentClass& rExtents = pRefImage->GetImageExtents();

					if ( S_OK == rExtents.GetExtentProperty( SvPb::SVExtentPropertyPositionPoint, point ) &&
						nullptr != pMaskBuffer && !pMaskBuffer->isEmpty() )
					{
						if ( S_OK != pMaskInputImage->ValidateAgainstOutputExtents(rExtents) )
						{
							MatroxCode = SVMatroxBufferInterface::ClearBuffer(m_MaskBufferHandlePtr->GetBuffer(), 0.0 );

							if (S_OK != MatroxCode)
							{
								if (nullptr != pErrorMessages)
								{
									SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UpdateBufferFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
									pErrorMessages->push_back(Msg);
								}
								// Signal that something was wrong...
								rRunStatus.SetInvalid();
								return false;
							}
						}

						MatroxCode = SVMatroxBufferInterface::CopyBuffer(m_MaskBufferHandlePtr->GetBuffer(), pMaskBuffer->getHandle()->GetBuffer(), static_cast<long> (-point.m_x), static_cast<long> (-point.m_y));

						if (S_OK != MatroxCode)
						{
							if (nullptr != pErrorMessages)
							{
								SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_CopyImagesFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
								pErrorMessages->push_back(Msg);
							}
							// Signal that something was wrong...
							rRunStatus.SetInvalid();
							return false;
						}
					}
					else
					{
						if (nullptr != pErrorMessages)
						{
							SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
							pErrorMessages->push_back(Msg);
						}
						// Signal that something was wrong...
						rRunStatus.SetInvalid();
						return false;
					}
				}
			}// end if ( dwMaskType == MASK_TYPE_IMAGE )

			else if ( dwMaskType == MASK_TYPE_SHAPE )
			{
				SVShapeMaskHelperClass* pShapeHelper = GetShapeHelper();
				pShapeHelper->onRun(First, rInputImageHandle, rOutputImageHandle, rRunStatus);
			}

			long lMaskOperator = SVImageAnd;
			m_Data.evoCurrentMaskOperator.GetValue( lMaskOperator );

			MatroxCode = SVMatroxImageInterface::Arithmetic(rOutputImageHandle->GetBuffer(),
				First ? rInputImageHandle->GetBuffer() : rOutputImageHandle->GetBuffer(),
				m_MaskBufferHandlePtr->GetBuffer(),
				static_cast<SVImageOperationTypeEnum>(lMaskOperator) );

			if (S_OK != MatroxCode)
			{
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RunArithmeticFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
					pErrorMessages->push_back(Msg);
				}
				// Signal that something was wrong...
				rRunStatus.SetInvalid();
				return false;
			}

#ifdef _DEBUG999
			{
				// Dump the images to .bmp files
				try
				{
					/*MatroxCode = */SVMatroxBufferInterface::Export( m_MaskBufferHandlePtr.milImage,
						_T("C:\\Temp\\Mask.bmp"),
						SVFileBitmap );


					/*MatroxCode = */SVMatroxBufferInterface::Export( rInputImageHandle.milImage,
						_T("C:\\Temp\\Input.bmp"),
						SVFileBitmap);


					/*MatroxCode = */SVMatroxBufferInterface::Export( rOutputImageHandle.milImage,
						_T("C:\\Temp\\Output.bmp"),
						SVFileBitmap);

				}
				catch(...)
				{
				}

			}
#endif //_DEBUG999
			SVShapeMaskHelperClass* pShapeHelper = GetShapeHelper();
			if ( pShapeHelper )
			{
				DWORD dwShapeColor = m_statusColor.GetDefaultValue();
				if ( MASK_TYPE_SHAPE == dwMaskType )
				{
					dwShapeColor = rRunStatus.GetStatusColor();
				}
				pShapeHelper->m_statusColor.SetValue(dwShapeColor);
			}
			// Success...
			return true;
		}
		else
		{
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
				pErrorMessages->push_back(Msg);
			}
		}

		// Signal that something was wrong...
		rRunStatus.SetInvalid();
		SVShapeMaskHelperClass* pShapeHelper = GetShapeHelper();
		if ( pShapeHelper )
		{
			DWORD dwShapeColor = m_statusColor.GetDefaultValue();
			pShapeHelper->m_statusColor.SetValue(dwShapeColor);
		}
		return false;
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

SvIe::SVImageClass* SVUserMaskOperatorClass::getMaskInputImage(bool bRunMode /*= false*/) const
{
	return SvOl::getInput<SvIe::SVImageClass>(m_inObjectInfo, bRunMode);
}

bool SVUserMaskOperatorClass::hasToAskFriendForConnection( const SvDef::SVObjectTypeInfoStruct& rInfo, SVObjectClass*& rPOwner ) const
{
	bool Result(true);
	if (SvPb::SVImageObjectType == rInfo.m_ObjectType )
	{
		rPOwner = GetInspection();
		Result = false;
	}
	return Result;
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
#pragma endregion IMask

} //namespace SvOp
