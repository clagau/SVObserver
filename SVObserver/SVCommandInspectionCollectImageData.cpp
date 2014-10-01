//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandInspectionCollectImageData
//* .File Name       : $Workfile:   SVCommandInspectionCollectImageData.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   30 Sep 2014 15:35:34  $
//******************************************************************************

#include "stdafx.h"
#include "SVCommandInspectionCollectImageData.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVImageProcessingClass.h"
#include "SVInspectionProcess.h"
#include "SVResultList.h"
#include "SVToolSet.h"
#include "SVObjectLibrary/GlobalConst.h"
#include "SVPPQObject.h"

SVCommandInspectionCollectImageData::SVCommandInspectionCollectImageData()
: m_InspectionId()
, m_ImageIds()
, m_Product()
{
}

SVCommandInspectionCollectImageData::SVCommandInspectionCollectImageData(const SVCommandInspectionCollectImageData& p_rObject)
: m_InspectionId( p_rObject.m_InspectionId )
, m_ImageIds( p_rObject.m_ImageIds )
, m_Product( p_rObject.m_Product )
{
}

SVCommandInspectionCollectImageData::SVCommandInspectionCollectImageData(const SVGUID& p_rInspectionId, const SVImageIdSet& p_rImageIds)
: m_InspectionId( p_rInspectionId )
, m_ImageIds( p_rImageIds )
, m_Product()
{
}

SVCommandInspectionCollectImageData::~SVCommandInspectionCollectImageData()
{
}

HRESULT SVCommandInspectionCollectImageData::Execute()
{
	HRESULT l_Status = S_OK;

	SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( GetInspectionId() );
	SVInspectionProcess* l_pInspection = dynamic_cast< SVInspectionProcess* >( l_pObject );

	if( l_pInspection != NULL )
	{
		SVProductInfoStruct l_Product = l_pInspection->LastProductGet( SV_DISPLAY );

		SVInspectionInfoStruct& l_rIPInfo = l_Product.m_svInspectionInfos[ GetInspectionId() ];
		SVStdMapSVVirtualCameraPtrSVCameraInfoStruct& l_rCameraInfos = l_Product.m_svCameraInfos;
		SVDataManagerHandle l_Handle = l_Product.oPPQInfo.m_ResultDataDMIndexHandle;
		long l_ResultDataIndex = l_Handle.GetIndex();
		SVImageIndexStruct l_ResultImageIndex;

		l_ResultImageIndex.m_ResultDMIndexHandle.Assign( l_rIPInfo.m_ResultImageDMIndexHandle, SV_DISPLAY );
		l_ResultImageIndex.m_PublishedResultDMIndexHandle.Assign( l_Product.oPPQInfo.m_ResultImagePublishedDMIndexHandle, SV_DISPLAY );

		double l_TriggerDistance = 0.0f;
		double l_ToolSetTime = 0.0f;

		if( l_Product.oTriggerInfo.m_PreviousTrigger > 0 )
		{
			l_TriggerDistance = (double) ( l_Product.oTriggerInfo.m_BeginProcess - l_Product.oTriggerInfo.m_PreviousTrigger );
		}

		if( ( l_rIPInfo.m_BeginToolset > 0 ) &&	( l_rIPInfo.m_EndToolset > 0 ) )
		{
			l_ToolSetTime = (double) ( l_rIPInfo.m_EndToolset - l_rIPInfo.m_BeginToolset );
		}

		m_Product.m_TriggerCount = l_Product.ProcessCount();

		UpdateResults( l_pInspection, m_Product.m_ResultData );

		m_Product.m_ResultData.m_ToolSetEndTime = l_rIPInfo.m_ToolSetEndTime;
		m_Product.m_ResultData.m_ToolSetAvgTime = l_rIPInfo.m_ToolSetAvgTime;
		m_Product.m_ResultData.m_ToolSetTime = l_ToolSetTime;
		m_Product.m_ResultData.m_TriggerDistance = l_TriggerDistance;

		const SVCommandInspectionCollectImageData::SVImageIdSet& l_rImageIds = GetImageIds();

		for( SVCommandInspectionCollectImageData::SVImageIdSet::const_iterator l_Iter = l_rImageIds.begin(); l_Iter != l_rImageIds.end(); ++l_Iter )
		{
			SVGUID l_ImageId = *l_Iter;

			SVIPImageDataElementMap::iterator l_FindIter = m_Product.m_ImageData.find( l_ImageId );

			if( l_FindIter == m_Product.m_ImageData.end() )
			{
				SVIPImageDataElement l_ImageData;

				::Sleep( 0 );

				if( UpdateBuffer( l_ImageId, l_rCameraInfos, l_ResultImageIndex, l_ResultDataIndex, l_ImageData.m_ImageDIB , l_ImageData.m_OverlayData ) == S_OK )
				{
					::Sleep( 0 );

					m_Product.m_ImageData[ l_ImageId ] = l_ImageData;
				}
			}
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

bool SVCommandInspectionCollectImageData::empty() const
{
	bool l_Status = true;

	l_Status = l_Status && ( m_InspectionId.empty() );
	l_Status = l_Status && ( m_ImageIds.empty() );
	l_Status = l_Status && ( m_Product.m_ImageData.empty() );
	l_Status = l_Status && ( m_Product.m_ResultData.m_ResultData.empty() );

	return l_Status;
}

void SVCommandInspectionCollectImageData::clear()
{
	m_InspectionId.clear();
	m_ImageIds.clear();
	m_Product.clear();
}

const SVGUID& SVCommandInspectionCollectImageData::GetInspectionId() const
{
	return m_InspectionId;
}

const SVCommandInspectionCollectImageData::SVImageIdSet& SVCommandInspectionCollectImageData::GetImageIds() const
{
	return m_ImageIds;
}

HRESULT SVCommandInspectionCollectImageData::SetCommandData(const SVGUID& p_rInspectionId, const SVImageIdSet& p_rImageIds)
{
	HRESULT l_Status = S_OK;

	m_InspectionId = p_rInspectionId;
	m_ImageIds = p_rImageIds;

	m_Product.clear();

	return l_Status;
}

const SVIPProductStruct& SVCommandInspectionCollectImageData::GetProduct() const
{
	return m_Product;
}

HRESULT SVCommandInspectionCollectImageData::UpdateResults( SVInspectionProcess* p_pInspection, SVIPResultData& p_rResultData )
{
	HRESULT hRet = S_OK;

	if( p_pInspection != NULL )
	{
		hRet = setPPQInputResultData(p_pInspection, p_rResultData);
		if ( S_OK == hRet )
		{
			hRet = setToolSetResultData(p_pInspection, p_rResultData);
		}

		if ( S_OK == hRet )
		{
			hRet = setPPQ_XParameterResultData(p_pInspection, p_rResultData);
		}

		if ( S_OK == hRet )
		{
			hRet = setEnvironmentResultData(p_rResultData);
		}
	}
	else
	{
		hRet = E_FAIL;
	}

	return hRet;
}

HRESULT SVCommandInspectionCollectImageData::UpdateBuffer(const SVGUID& p_rImageId, SVStdMapSVVirtualCameraPtrSVCameraInfoStruct& p_rsvCameraInfos,
										 SVImageIndexStruct p_svResultImageIndex, long p_ResultDataIndex,
										 SVByteVector& p_rImageDIB, SVExtentMultiLineStructCArray& p_rMultiLineArray)
{
	HRESULT l_Status = S_OK;

	SVImageClass* l_pImage = dynamic_cast< SVImageClass* >( SVObjectManagerClass::Instance().GetObject( p_rImageId.ToGUID() ) );

	if( l_pImage != NULL )
	{
		SVDataManagerHandle	l_SourceImageDMIndexHandle;

		SVImageIndexStruct svResultImageIndex = p_svResultImageIndex;

		SVCameraImageTemplate* l_psvMainImage = dynamic_cast< SVCameraImageTemplate* >( l_pImage );
		SVRGBImageClass* l_psvRGBImage = dynamic_cast< SVRGBImageClass* >( l_pImage );

		if( l_psvMainImage == NULL && l_psvRGBImage != NULL )
		{
			l_psvMainImage = dynamic_cast< SVCameraImageTemplate* >( l_pImage->GetParentImage() );
		}

		if( l_psvMainImage != NULL )
		{
			SVStdMapSVVirtualCameraPtrSVCameraInfoStruct::const_iterator l_svIter;

			l_svIter = p_rsvCameraInfos.find( l_psvMainImage->GetCamera() );

			if( l_svIter != p_rsvCameraInfos.end() )
			{
				l_SourceImageDMIndexHandle.Assign( l_svIter->second.GetSourceImageDMIndexHandle(), SV_DISPLAY );
			}
		}

		svResultImageIndex.m_CameraDMIndexHandle.Assign( l_SourceImageDMIndexHandle, SV_DISPLAY );

		SVSmartHandlePointer l_ImageBuffer;

		l_pImage->GetImageHandle( svResultImageIndex, l_ImageBuffer );

		if( !( l_ImageBuffer.empty() ) )
		{
			// Special check for Color Tool's RGBMainImage which is HSI
			if( SV_IS_KIND_OF( l_pImage, SVRGBMainImageClass ) )
			{
				l_Status = SVImageProcessingClass::Instance().ConvertImageBuffer( p_rImageDIB, l_ImageBuffer, SVImageHLSToRGB );
			}
			else
			{
				SVBitmapInfo l_Info;

				BITMAPINFOHEADER l_Header = l_pImage->GetImageInfo().GetBitmapInfoHeader();

				l_Info.Assign( l_Header.biWidth, l_Header.biHeight, l_Header.biBitCount, SVBitmapInfo::GetDefaultColorTable( l_Header.biBitCount ) );

				if( l_Info == l_ImageBuffer->GetBitmapInfo() )
				{
					l_Status = SVImageProcessingClass::Instance().CopyImageBuffer( p_rImageDIB, l_ImageBuffer );
				}
				else
				{
					l_Status = SVImageProcessingClass::Instance().CopyImageBuffer( p_rImageDIB, l_Info, l_ImageBuffer );
				}
			}
		}
		else
		{
			l_Status = E_INVALIDARG;
		}

		if( l_Status == S_OK )
		{
			if( l_pImage->GetInspection() != NULL )
			{
				l_pImage->GetInspection()->CollectOverlays( l_pImage, p_rMultiLineArray );
			}
			else
			{
				l_Status = E_INVALIDARG;
			}
		}
	}
	else
	{
		l_Status = E_INVALIDARG;
	}

	return l_Status;
}

HRESULT SVCommandInspectionCollectImageData::setToolSetResultData( SVInspectionProcess* p_pInspection, SVIPResultData &p_rResultData )
{
	HRESULT l_Status = S_OK;

	SVToolSetClass* pToolSet = p_pInspection->GetToolSet();
	if( pToolSet )
	{
		// Get the OutputList
		SVResultListClass* pResultList = pToolSet->GetResultList();
		if( pResultList )
		{
			for( size_t i = 0; i < pResultList->m_vecObjects.size(); ++i )
			{
				SVObjectReference ref = pResultList->m_vecObjects.at(i);
				if( ref.Object() != NULL )
				{
					SVIPResultItemDefinition l_Def;

					if( ref.IsIndexPresent() )
					{
						if( ref.IsEntireArray() )
						{
							l_Def = SVIPResultItemDefinition( ref.Guid(), SVString() );
						}
						else
						{
							l_Def = SVIPResultItemDefinition( ref.Guid(), _variant_t( ref.ArrayIndex(true) ) );
						}
					}
					else
					{
						l_Def = SVIPResultItemDefinition( ref.Guid() );
					}

					unsigned long l_Color = ref.Object()->GetObjectColor();
					if( ref.Object()->GetOwner() )
					{
						l_Color = ref.Object()->GetOwner()->GetObjectColor();
					}

					CString l_Value;

					SVValueObjectReference voref(ref);  // try to assign to value object
					if( voref.Object() )                // if successful
					{
						if( voref->GetObjectType() == SVStringValueObjectType)
						{
							CString l_strQuotes;
							CString l_strValue;
							voref.GetValue( l_strValue );
							// Wrap string in Quotes...
							l_strQuotes.Format(_T("\042%s\042"),l_strValue);
							l_Value = l_strQuotes;
						}
						else
						{
							HRESULT hr = voref.GetValue( l_Value );
							if ( hr == SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE )
							{
								l_Value = _T("< ") + l_Value + _T(" >");
							}
							else if ( hr != S_OK )
							{
								l_Value = _T( "<Not Valid>" );
							}
						}
					}

					p_rResultData.m_ResultData[ l_Def ] = SVIPResultItemData( static_cast< LPCTSTR >( l_Value ), l_Color );
				}
			}
		}
	}
	else
	{
		l_Status = E_FAIL;
	}	
	return l_Status;
}

HRESULT SVCommandInspectionCollectImageData::setPPQInputResultData( SVInspectionProcess* p_pInspection, SVIPResultData &p_rResultData )
{
	HRESULT hRet = S_OK;

	// Insert all PPQ input items that are picked for viewing
	for( size_t l = 0; p_pInspection && l < p_pInspection->m_PPQInputs.size(); ++l )
	{
		SVIOEntryStruct pIOEntry = p_pInspection->m_PPQInputs[l];
		hRet |= addViewableObject2ResultData(pIOEntry.m_IOEntryPtr->m_pValueObject, p_rResultData);
	}
	return hRet;
}

HRESULT SVCommandInspectionCollectImageData::setPPQ_XParameterResultData( SVInspectionProcess* p_pInspection, SVIPResultData &p_rResultData )
{
	HRESULT hRet = S_OK;
	SVObjectReferenceVector ObjectList;
	SVPPQObject *ppq = p_pInspection->GetPPQ();
	SVString PPQName = ppq->GetName();
	SVObjectManagerClass::Instance().getTreeList( PPQName, ObjectList, SV_VIEWABLE );
	for(SVObjectReferenceVector::const_iterator iter = ObjectList.begin(); iter != ObjectList.end(); ++iter) 
	{
		hRet |= addViewableObject2ResultData(iter->Object(), p_rResultData);
	}
	return hRet;
}

HRESULT SVCommandInspectionCollectImageData::setEnvironmentResultData( SVIPResultData &p_rResultData )
{
	HRESULT hRet = S_OK;
	SVObjectReferenceVector ObjectList;
	SVObjectManagerClass::Instance().getTreeList( Seidenader::SVObjectLibrary::FqnEnvironmentMode, ObjectList, SV_VIEWABLE );
	for(SVObjectReferenceVector::const_iterator iter = ObjectList.begin(); iter != ObjectList.end(); ++iter) 
	{
		hRet |= addViewableObject2ResultData(iter->Object(), p_rResultData);
	}	
	return hRet;
}

HRESULT SVCommandInspectionCollectImageData::addViewableObject2ResultData( SVObjectClass * object, SVIPResultData &p_rResultData, unsigned long colorValue )
{
	HRESULT hRet = S_OK;
	if( nullptr != object && object->ObjectAttributesSet() & SV_VIEWABLE )
	{
		SVIPResultItemDefinition def( object->GetUniqueObjectID() );
		SVString value = "";

		if( SV_IS_KIND_OF( object, SVValueObjectClass ) )
		{
			CString tmpValue;
			hRet = static_cast<SVValueObjectClass*> (object)->GetValue( tmpValue );
			value = tmpValue;
		}
		else if( SV_IS_KIND_OF( object, BasicValueObject ) )
		{
			hRet = static_cast <BasicValueObject*> (object)->getValue( value );
		}
		else
		{
			hRet = E_FAIL;
		}

		p_rResultData.m_ResultData[ def ] = SVIPResultItemData( value, colorValue );
	}
	return hRet;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVCommandInspectionCollectImageData.cpp_v  $
 * 
 *    Rev 1.2   30 Sep 2014 15:35:34   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  925
 * SCR Title:  Add PPQ Items and SVObserver Modes to Equation Editor Object Selector
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Changed UpdateResults method to allow PPQ items in equations.  Broke into multiple new methods:
 * setToolSetResultData,
 * setPPQInputResultData,
 * setPPQ_XParameterResultData,
 * setEnvironmentResultData,
 * and addViewableObject2ResultData.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 May 2013 16:01:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   29 Apr 2013 14:06:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 09:56:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   30 Nov 2012 08:30:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  800
 * SCR Title:  Fix Blob Array Result index string
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fix Array index in results view
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   01 Aug 2012 11:44:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new command functionality to allow for synchronous command execution through the inspection thread.  This will fix problems with race conditions with the inspection process.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   18 Jul 2012 13:23:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Jul 2012 16:39:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/