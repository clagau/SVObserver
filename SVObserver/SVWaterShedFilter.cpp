// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVWaterShedFilter
// * .File Name       : $Workfile:   SVWaterShedFilter.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 16:30:20  $
// ******************************************************************************

#include "stdafx.h"
#include "SVObjectLibrary/SVObjectLibrary.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVImageProcessingClass.h"
#include "SVTaskObjectList.h"
#include "SVToolSet.h"
#include "SVTool.h"
#include "SVWaterShedFilter.h"

////////////////////////////////////////////////////////////////////////////////
// Other Necessary Include File(s) - Module Link(s)
////////////////////////////////////////////////////////////////////////////////

//******************************************************************************
//* DEFINITIONS OF MODULE-LOCAL VARIABLES:
//******************************************************************************

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//******************************************************************************
//* CLASS METHOD IMPLEMENTATION(S):
//******************************************************************************

//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVWaterShedFilter
//* Note(s)    : Skeleton Filter Base Class
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

//******************************************************************************
// Adjustments
//******************************************************************************
SV_IMPLEMENT_CLASS( SVWatershedFilterClass, SVWatershedFilterClassGuid )

//******************************************************************************
// Constructor(s):
//******************************************************************************
SVWatershedFilterClass::SVWatershedFilterClass( SVObjectClass* POwner, int StringResourceID )
					: SVFilterClass( POwner, StringResourceID )
{
	init();
}

SVWatershedFilterClass::~SVWatershedFilterClass()
{
}

void SVWatershedFilterClass::init()
{

	RegisterEmbeddedObject( &m_lvoMinVariation,		SVWatershedFilterMinVariationGuid, IDS_OBJECTNAME_WATERSHEDFILTER_MINVARIATION, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_lvoControlFlag,		SVWatershedFilterControlFlagGuid, IDS_OBJECTNAME_WATERSHEDFILTER_CONTROLFLAG, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_bvoUseMarker,        SVWatershedFilterUseMarkerGuid,    IDS_OBJECTNAME_WATERSHEDFILTER_USEMARKER, false, SVResetItemNone );


	// Default Values
	m_bvoUseMarker.SetDefaultValue( FALSE, TRUE);
	m_lvoMinVariation.SetDefaultValue( 1, TRUE );
	m_lvoControlFlag.SetDefaultValue( SVImageWSWatershed + SVImageWSMinimaFill + SVImageWSRegular + SVImage4Connected, TRUE );

	// Set Attributes
	m_bvoUseMarker.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_lvoMinVariation.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;
	m_lvoControlFlag.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_SETABLE_ONLINE | SV_REMOTELY_SETABLE;

	m_MarkerImageInfo.SetInputObjectType( SVImageObjectType );
	m_MarkerImageInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_MarkerImageInfo, _T( "WatershedFilterMarkerImage" ) );

	
	// Set default inputs and outputs
	addDefaultInputObjects();

}
////////////////////////////////////////////////////////////////////////////////
// .Title       : CreateObject
// -----------------------------------------------------------------------------
// .Description : ...
//              :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
////////////////////////////////////////////////////////////////////////////////
BOOL SVWatershedFilterClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = TRUE;

	bOk &= SVFilterClass::CreateObject( PCreateStructure );

	return bOk;
}

#pragma region virtual method (IWatershedFilter)
HRESULT SVWatershedFilterClass::addControlFlagRequest(long value)
{
	return m_taskObjectValueInterface.AddInputRequest( &m_lvoControlFlag, value );
}

long SVWatershedFilterClass::getControlFlag() const
{ 
	long value = 0;
	m_lvoControlFlag.GetValue(value);
	return value;
}

HRESULT SVWatershedFilterClass::addMinVariationRequest(long value) 
{
	return m_taskObjectValueInterface.AddInputRequest( &m_lvoMinVariation, value );
}

long SVWatershedFilterClass::getMinVariation() const
{ 
	long value = 0;
	m_lvoMinVariation.GetValue(value);
	return value;
}

HRESULT SVWatershedFilterClass::addMarkerUsedRequest(bool value) 
{
	return m_taskObjectValueInterface.AddInputRequest( &m_bvoUseMarker, value );
}

bool SVWatershedFilterClass::isMarkerUsed() const
{ 
	bool value = 0;
	m_bvoUseMarker.GetValue(value);
	return value;
}

SVString SVWatershedFilterClass::getMarkerImage() const
{
	SVString retValueString;
	if( m_MarkerImageInfo.IsConnected() )
	{
		SVImageClass *pCurrentSourceImage = dynamic_cast< SVImageClass* >( m_MarkerImageInfo.GetInputObjectInfo().PObject );
		if (nullptr != pCurrentSourceImage)
		{
			retValueString = pCurrentSourceImage->getDisplayedName();
		}
	}
	return retValueString;
}

HRESULT SVWatershedFilterClass::setMarkerImage(const SVString& imageName)
{
	HRESULT retVal = E_FAIL;
	SVToolSetClass* pToolSet = dynamic_cast <SVToolSetClass*> ( GetAncestor( SVToolSetObjectType ) );
	if( nullptr != pToolSet )
	{
		SVGetObjectDequeByTypeVisitor::SVObjectPtrDeque objects = getAvailableSourceImage(pToolSet->GetUniqueObjectID());
		for( SVGetObjectDequeByTypeVisitor::SVObjectPtrDeque::const_iterator l_Iter = objects.begin(); l_Iter != objects.end(); ++l_Iter )
		{
			SVImageClass* pImage = dynamic_cast< SVImageClass* >( const_cast< SVObjectClass* >( *l_Iter ) );

			if( isValidMarkerImage( pImage, *pToolSet ) && SVString( pImage->getDisplayedName() ) == imageName )
			{
				retVal = ConnectToImage( &m_MarkerImageInfo, pImage );
				break;
			}
		}
	}

	return retVal;
}

std::vector<SVString> SVWatershedFilterClass::getAvailableMarkerImageNames()
{
	std::vector<SVString> retImageNames;

	SVToolSetClass* pToolSet = dynamic_cast <SVToolSetClass*> ( GetAncestor( SVToolSetObjectType ) );
	if( nullptr != pToolSet && nullptr != pToolSet->getCurrentImage() )
	{
		SVGetObjectDequeByTypeVisitor::SVObjectPtrDeque objects = getAvailableSourceImage(pToolSet->GetUniqueObjectID());
		for( SVGetObjectDequeByTypeVisitor::SVObjectPtrDeque::const_iterator l_Iter = objects.begin(); l_Iter != objects.end(); ++l_Iter )
		{
			SVImageClass* pImage = dynamic_cast< SVImageClass* >( const_cast< SVObjectClass* >( *l_Iter ) );

			if( isValidMarkerImage( pImage, *pToolSet ) )
			{
				retImageNames.push_back( pImage->getDisplayedName() );
			}			
		}
	}
	return retImageNames;
}
#pragma endregion virtual method (IWatershedFilter)


BOOL SVWatershedFilterClass::OnValidate()
{
	BOOL bUseMarker;

	BOOL bRetVal = SVFilterClass::OnValidate();

	bRetVal = bRetVal && ( m_bvoUseMarker.GetValue(bUseMarker) == S_OK );

	if( bRetVal && bUseMarker )
	{
		bRetVal = bRetVal && m_MarkerImageInfo.IsConnected();
		bRetVal = bRetVal && m_MarkerImageInfo.GetInputObjectInfo().PObject != NULL;
	}

	return bRetVal;	
}



//******************************************************************************
// Operation(s) Of Process:
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// .Title       : onRun
// -----------------------------------------------------------------------------
// .Description : Runs this operator.
//              : Returns FALSE, if operator cannot run ( may be deactivated ! )
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
////////////////////////////////////////////////////////////////////////////////
BOOL SVWatershedFilterClass::onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus )
{ 

	// Force a copy forward to keep the display correct
	m_lvoMinVariation.CopyLastSetValue( RRunStatus.m_lResultDataIndex );
	m_lvoControlFlag.CopyLastSetValue( RRunStatus.m_lResultDataIndex );

	long lMinVariation;
	long lControlFlag;
	BOOL bUseMarker;
	m_bvoUseMarker.GetValue(bUseMarker);
	m_lvoMinVariation.GetValue(lMinVariation);
	m_lvoControlFlag.GetValue(lControlFlag);

	SVSmartHandlePointer ImageHandle;

	if( m_pCurrentUIOPL && !( RInputImageHandle.empty() ) && !( ROutputImageHandle.empty() ) )
	{
		SVImageBufferHandleImage l_InMilHandle;
		SVImageBufferHandleImage l_OutMilHandle;

		RInputImageHandle->GetData( l_InMilHandle );
		ROutputImageHandle->GetData( l_OutMilHandle );
		
		SVMatroxImageInterface::SVStatusCode l_Code;
		if( bUseMarker && m_MarkerImageInfo.IsConnected() )
		{
			SVImageClass* pInputImage = ( SVImageClass* )m_MarkerImageInfo.GetInputObjectInfo().PObject;
			if( pInputImage )
			{
				pInputImage->GetImageHandle( ImageHandle );
				
				SVImageBufferHandleImage l_MilHandle;
				if( !( ImageHandle.empty() ) )
				{
					ImageHandle->GetData( l_MilHandle );
				}

				l_Code = SVMatroxImageInterface::Watershed( l_OutMilHandle.GetBuffer(),
					( First == TRUE ) ? l_InMilHandle.GetBuffer() : l_OutMilHandle.GetBuffer(),
					l_MilHandle.GetBuffer(),
					lMinVariation,
					static_cast<SVImageWaterShedEnum>(lControlFlag));
			}

		}
		else
		{
			SVImageBufferHandleImage l_MilHandle;
			if( !( ImageHandle.empty() ) )
			{
				ImageHandle->GetData( l_MilHandle );
			}

			l_Code = SVMatroxImageInterface::Watershed( l_OutMilHandle.GetBuffer(),
				( First == TRUE ) ? l_InMilHandle.GetBuffer() : l_OutMilHandle.GetBuffer(),
				l_MilHandle.GetBuffer(),
				lMinVariation,
				static_cast<SVImageWaterShedEnum>(lControlFlag));
		}

		if( l_Code != SVMEE_STATUS_OK )
		{
			// Signal that something was wrong...
			SetInvalid();
			RRunStatus.SetInvalid();
			return FALSE;
		}

		// Success...
		return TRUE;
	}

	// Signal that something was wrong...
	SetInvalid();
	RRunStatus.SetInvalid();
	return FALSE;
}

bool SVWatershedFilterClass::isValidMarkerImage(const SVImageClass* pImage, const SVToolSetClass &rToolSet) const
{
	if( nullptr != pImage )
	{
		//is tool, is toolset or inspection
		SVToolClass* pImageOwnerTool = dynamic_cast <SVToolClass*> ( pImage->GetAncestor( SVToolObjectType ) );
		if ( !pImageOwnerTool )
		{
			if ( nullptr == pImage->GetAncestor( SVInspectionObjectType ) )
			{
				return false;
			}
		}
		else
		{
			if( (!( rToolSet.IsToolPreviousToSelected( pImageOwnerTool->GetUniqueObjectID() ) )) ||
				(GetTool() == pImageOwnerTool) )
			{
				return false;
			}
		}

		SVImageInfoClass imageInfo = pImage->GetImageInfo();

		long bandNumber = 1;
		imageInfo.GetImageProperty( SVImagePropertyBandNumber, bandNumber );
		if ( 1 == bandNumber )
		{
			return true;
		}
	}
	return false;
}

SVGetObjectDequeByTypeVisitor::SVObjectPtrDeque SVWatershedFilterClass::getAvailableSourceImage(const SVGUID toolsetGUID)
{
	// Find all available Images...
	SVObjectTypeInfoStruct imageObjectInfo;
	imageObjectInfo.ObjectType = SVImageObjectType;

	SVGetObjectDequeByTypeVisitor visitor( imageObjectInfo );
	SVObjectManagerClass::Instance().VisitElements( visitor, toolsetGUID );
	return visitor.GetObjects();
}
// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVWaterShedFilter.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 16:30:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   12 Jul 2012 15:12:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add in an additional string identifier for the input and fixed clear functionality assoicated with input structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   19 Jun 2012 14:28:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
 * Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   10 Feb 2011 15:17:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   27 Jan 2011 12:04:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   08 Dec 2010 13:48:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   04 Nov 2010 14:18:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   01 Jun 2010 15:19:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   25 Jul 2007 07:58:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upgrade to MIL 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   19 Feb 2007 16:44:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  594
 * SCR Title:  Remove issue with interpreting status response incorrectly
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated MIL error code methodology to use the new SVMatroxAppManager GetStatusCode operator.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   15 Feb 2007 11:57:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  594
 * SCR Title:  Remove issue with interpreting status response incorrectly
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated flags for the mappgeterror fucntion to remove the M_MESSAGE flag.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   29 Jul 2005 13:36:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   21 Jun 2005 08:36:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   object attributes now use accessor methods
 * value object functions now use HRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   19 May 2005 14:52:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added m_ prefix to member variables
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Feb 2005 15:06:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modifications to support new Reset Methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Sep 2004 10:01:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  446
 * SCR Title:  Add new filters to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in of Watershed Filter.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
