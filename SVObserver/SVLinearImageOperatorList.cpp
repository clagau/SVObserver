//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLinearImageOperatorList
//* .File Name       : $Workfile:   SVLinearImageOperatorList.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   13 Aug 2013 10:08:52  $
//******************************************************************************

#include "stdafx.h"
#include "SVLinearImageOperatorList.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"
#include "SVLinearToolClass.h"
#include "SVUnaryImageOperatorClass.h"
#include "SVTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS( SVLinearImageOperatorListClass, SVLinearImageOperatorListClassGuid )


SVLinearImageOperatorListClass::SVLinearImageOperatorListClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
							    :SVStdImageOperatorListClass( BCreateDefaultTaskList, POwner, StringResourceID ) 
{
	init();
}

SVLinearImageOperatorListClass::~SVLinearImageOperatorListClass() 
{ 
	CloseObject();

}

BOOL SVLinearImageOperatorListClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL l_bOk = SVStdImageOperatorListClass::CreateObject( PCreateStructure );

	SVDataBufferInfoClass& rBufferInfo = m_svProfileResultData.GetDataBufferInfo();
	rBufferInfo.Type = SVDataBufferInfoClass::SVProjectResult;

	BOOL m_bUseRotation = TRUE;

	if( getUseRotationAngle() != NULL &&
		  ( getUseRotationAngle()->GetValue( m_bUseRotation ) == S_OK ) &&
			! m_bUseRotation )
	{
		outputImageObject.InitializeImage( SVImageTypeLogicalAndPhysical );
	}
	else
	{
		outputImageObject.InitializeImage( SVImageTypePhysical );
	}

	l_bOk &= UpdateLineExtentData() == S_OK;
	m_svLinearData.ObjectAttributesAllowedRef() &= ~( SV_VIEWABLE | SV_PRINTABLE );

	m_svMaxThreshold.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_svMinThreshold.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;


	return l_bOk;
}

BOOL SVLinearImageOperatorListClass::CloseObject()
{
	BOOL l_bOk = m_svProfileResultData.CloseObject();

	l_bOk &= SVStdImageOperatorListClass::CloseObject();

	return l_bOk;
}

HRESULT SVLinearImageOperatorListClass::ResetObject()
{
	BOOL m_bUseRotation = TRUE;

	if( getUseRotationAngle() != NULL &&
		  ( getUseRotationAngle()->GetValue( m_bUseRotation ) == S_OK ) &&
			! m_bUseRotation )
	{
		outputImageObject.InitializeImage( SVImageTypeLogicalAndPhysical );
	}
	else
	{
		outputImageObject.InitializeImage( SVImageTypePhysical );
	}

	HRESULT l_hrOk = SVStdImageOperatorListClass::ResetObject();

	if( UpdateLineExtentData() != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

BOOL SVLinearImageOperatorListClass::OnValidate()
{
	if( SVStdImageOperatorListClass::OnValidate() )
	{
		if( getInputProfileOrientation() != NULL && 
			  m_aulLineData.size() > 0 )
		{
			return TRUE;
		}
	}
	SetInvalid();
	return FALSE;
}

BOOL SVLinearImageOperatorListClass::Run( SVRunStatusClass& RRunStatus )
{
	SVSmartHandlePointer output;
	SVSmartHandlePointer input;
	SVImageExtentClass l_svImageExtent;

	BOOL m_bUseRotation = TRUE;

	BOOL bRetVal = GetTool()->GetImageExtent( l_svImageExtent ) == S_OK;

	bRetVal = bRetVal && getUseRotationAngle() != NULL && ( getUseRotationAngle()->GetValue( m_bUseRotation ) == S_OK );

	
	SVMatroxImageInterface::SVStatusCode l_Code = SVMEE_STATUS_OK;

	// Get Input Image Names for Data Definition Lists.. Why here? because
	// we need a run status to set value object results
	CollectInputImageNames( RRunStatus );

	if( m_bUseRotation )
	{
		SVRunStatusClass ChildRunStatus;
		ChildRunStatus.m_lResultDataIndex  = RRunStatus.m_lResultDataIndex;
		ChildRunStatus.Images = RRunStatus.Images;
		ChildRunStatus.m_UpdateCounters = RRunStatus.m_UpdateCounters;
		
		// Run yourself...
		bRetVal &= onRun( RRunStatus );
		
		SVImageClass *l_psvInputImage = getInputImage();

		if( l_psvInputImage == NULL )
		{
			// Signal something is wrong...
			bRetVal = FALSE;
		}
		
		if( bRetVal )
		{
			bRetVal = outputImageObject.SetImageHandleIndex( RRunStatus.Images );
			if ( bRetVal )
			{
				SVImageBufferHandleImage l_InMilHandle;
				SVImageBufferHandleImage l_OutMilHandle;

				outputImageObject.GetImageHandle( output );
				l_psvInputImage->GetImageHandle( input );
				
				if( !( input.empty() ) && !( output.empty() ) )
				{
					input->GetData( l_InMilHandle );
					output->GetData( l_OutMilHandle );
				}

				POINT l_oInPoint;
				POINT l_oOutPoint;
				double dRotationAngle;

				l_svImageExtent.GetExtentProperty(SVExtentPropertyRotationAngle,dRotationAngle);
				l_svImageExtent.GetExtentProperty(SVExtentPropertyPositionPoint,l_oInPoint);
				l_svImageExtent.GetExtentProperty(SVExtentPropertyOutputPositionPoint,l_oOutPoint);

				SVMatroxImageRotateStruct l_Rotate( l_InMilHandle.GetBuffer() );
				l_Rotate.m_dAngle = dRotationAngle;
				l_Rotate.m_dSrcCenX = l_oInPoint.x;
				l_Rotate.m_dSrcCenY = l_oInPoint.y;
				l_Rotate.m_dDstCenX = l_oOutPoint.x;
				l_Rotate.m_dDstCenY = l_oOutPoint.y;
				l_Rotate.m_eInterpolation = SVNearestNeighOverScanClear;
				l_Code = SVMatroxImageInterface::Rotate( l_OutMilHandle.GetBuffer(), l_Rotate );


				// Run children...
				for( int i = 0; i < GetSize(); i++ )
				{
					ChildRunStatus.ResetRunStateAndToolSetTimes();
					
					SVUnaryImageOperatorClass*  pOperator = dynamic_cast<SVUnaryImageOperatorClass *>(GetAt( i ));
					if( pOperator )
					{
						pOperator->Run( FALSE, output, output, ChildRunStatus );
					}
					else
						bRetVal = FALSE;
					
					// Update our Run Status
					if( ChildRunStatus.IsDisabled() )
						RRunStatus.SetDisabled();
					
					if( ChildRunStatus.IsDisabledByCondition() )
						RRunStatus.SetDisabledByCondition();
					
					if( ChildRunStatus.IsWarned() )
						RRunStatus.SetWarned();
					
					if( ChildRunStatus.IsFailed() )
						RRunStatus.SetFailed();
					
					if( ChildRunStatus.IsPassed() )
						RRunStatus.SetPassed();

					if( ChildRunStatus.IsCriticalFailure() )
						RRunStatus.SetCriticalFailure();
				}
				
				// 'no operator was running' check...
				// RO_22Mar2000
				
			}
		}
	}
	else
	{
		bRetVal &= SVStdImageOperatorListClass::Run( RRunStatus );
	}

	if( bRetVal )
	{
		SVImageBufferHandleImage l_OutMilHandle;

		outputImageObject.GetImageHandle( output );

		if( !( output.empty() ) )
		{
			output->GetData( l_OutMilHandle );
		}

		SVDataBufferInfoClass& rDataBufferInfo = m_svProfileResultData.GetDataBufferInfo();

		double l_dProjectAngle = 0.0;

		SVEnumerateValueObjectClass *l_svProjectAngle = getInputProfileOrientation();

		bRetVal = m_aulLineData.size() != 0 &&
		          l_svProjectAngle != NULL && 
		          ( l_svProjectAngle->GetValue( l_dProjectAngle ) == S_OK );

		double l_dMin = 9999999.0;
		double l_dMax = 0.0;

		if( bRetVal )
		{
			double l_dProjectHeight = 0.0;

			l_Code = SVMatroxImageInterface::Project( rDataBufferInfo.HBuffer.milResult, l_OutMilHandle.GetBuffer(), l_dProjectAngle );

			l_Code = SVMatroxImageInterface::GetResult( rDataBufferInfo.HBuffer.milResult, m_aulLineData );

			if( l_dProjectAngle == 0.0 )
			{
				l_svImageExtent.GetExtentProperty(SVExtentPropertyHeight,l_dProjectHeight);
			}
			else
			{
				l_svImageExtent.GetExtentProperty(SVExtentPropertyWidth,l_dProjectHeight);
			}

			if( l_dProjectHeight == 0.0 || l_dProjectHeight == 1.0 )
			{
				for( unsigned long i = 0; i < m_ulLineLength; i++ )
				{
					double l_dValue = m_aulLineData[ i ];

					l_dMin = std::min( l_dValue, l_dMin );
					l_dMax = std::max( l_dValue, l_dMax );

					m_svArray.at( i ) = l_dValue;
				}
			}
			else
			{
				for( unsigned long i = 0; i < m_ulLineLength; i++ )
				{
					double l_dValue = m_aulLineData[ i ] / l_dProjectHeight;

					l_dMin = std::min( l_dValue, l_dMin );
					l_dMax = std::max( l_dValue, l_dMax );

					m_svArray.at( i ) = l_dValue;
				}
			}

			bRetVal = ( m_svMinThreshold.SetValue( RRunStatus.m_lResultDataIndex, l_dMin ) == S_OK ) && bRetVal;
			bRetVal = ( m_svMaxThreshold.SetValue( RRunStatus.m_lResultDataIndex, l_dMax ) == S_OK ) && bRetVal;

			bRetVal = ( m_svLinearData.SetArrayValues( RRunStatus.m_lResultDataIndex, m_svArray ) == S_OK ) && bRetVal;
			ASSERT( bRetVal );
		}
	}

	if( ! bRetVal )
	{
		// Something was wrong...
		
		SetInvalid();
		RRunStatus.SetInvalid();
	}
	
	// Get Status Color...
	DWORD dwValue = RRunStatus.GetStatusColor();
	statusColor.SetValue( RRunStatus.m_lResultDataIndex, dwValue );
	
	// Get Status...
	dwValue = RRunStatus.GetState();
	statusTag.SetValue( RRunStatus.m_lResultDataIndex, dwValue );
	
	return bRetVal;
}

SVBoolValueObjectClass* SVLinearImageOperatorListClass::getUseRotationAngle()
{
	if( inputUseRotationAngle.IsConnected() && 
		  inputUseRotationAngle.GetInputObjectInfo().PObject )
		return dynamic_cast<SVBoolValueObjectClass *>(inputUseRotationAngle.GetInputObjectInfo().PObject);

	return NULL;
}

SVEnumerateValueObjectClass* SVLinearImageOperatorListClass::getInputProfileOrientation()
{
	if( inputProfileOrientation.IsConnected() && 
		  inputProfileOrientation.GetInputObjectInfo().PObject )
		return dynamic_cast<SVEnumerateValueObjectClass *>(inputProfileOrientation.GetInputObjectInfo().PObject);

	return NULL;
}

void SVLinearImageOperatorListClass::init()
{
	// Identify our output type
	outObjectInfo.ObjectTypeInfo.ObjectType = SVUnaryImageOperatorListObjectType;
	outObjectInfo.ObjectTypeInfo.SubType	= SVLinearImageOperatorListObjectType;

	inputProfileOrientation.SetInputObjectType( SVProfileOrientationGuid, SVEnumValueObjectType );
	inputProfileOrientation.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputProfileOrientation, _T( "LinearImageOperatorListProfileOrientation" ) );

	inputUseRotationAngle.SetInputObjectType( SVLinearToolUseRotationGuid, SVBoolValueObjectType );
	inputUseRotationAngle.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputUseRotationAngle, _T( "LinearImageOperatorListUseRotationAngle" ) );

	m_svLinearData.SetLegacyVectorObjectCompatibility();

	RegisterEmbeddedObject(&m_svLinearData, SVLinearDataClassGuid,IDS_CLASSNAME_SVLINEARDATACLASS, false, SVResetItemNone );

	RegisterEmbeddedObject(&m_svMinThreshold, SVLinearThresholdMinObjectGuid, IDS_OBJECTNAME_LINEAR_THRESHOLD_MINVALUE, false, SVResetItemNone );
	RegisterEmbeddedObject(&m_svMaxThreshold, SVLinearThresholdMaxObjectGuid, IDS_OBJECTNAME_LINEAR_THRESHOLD_MAXVALUE, false, SVResetItemNone );

	m_svMinThreshold.SetDefaultValue( 0.0, TRUE );
	m_svMaxThreshold.SetDefaultValue( 0.0, TRUE );

	m_ulLineLength = 0;

	outputImageObject.InitializeImage( SVImageTypePhysical );

	// Set default inputs and outputs
	addDefaultInputObjects();

}

HRESULT SVLinearImageOperatorListClass::UpdateLineExtentData()
{
	HRESULT l_hrOk = S_FALSE;

	SVImageExtentClass l_svExtents;

	SVEnumerateValueObjectClass* pProjAngle = getInputProfileOrientation();

	double projAngle = 0.0;

	RECT l_oRect;

	// This is the new Absolute Extent of the Image
	if ( GetTool() != NULL && pProjAngle != NULL &&
			 GetTool()->GetImageExtent( l_svExtents ) == S_OK &&
			 l_svExtents.GetOutputRectangle( l_oRect ) == S_OK &&
			 ( pProjAngle->GetValue( projAngle ) == S_OK ) )
	{
		if( projAngle == 0.0 )
		{
			m_ulLineLength = l_oRect.right;
		}
		else if( projAngle == 90.0 )
		{
			m_ulLineLength = l_oRect.bottom;
		}

		m_svArray.resize( m_ulLineLength );

		m_aulLineData.resize( m_ulLineLength );

		if( m_svProfileResultData.Resize( m_ulLineLength ) )
		{
			l_hrOk = S_OK;
		}

		m_svLinearData.SetArraySize( m_ulLineLength );
	}

	return l_hrOk;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVLinearImageOperatorList.cpp_v  $
 * 
 *    Rev 1.1   13 Aug 2013 10:08:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Included SVUnaryImageOperatorClass.h instead of SVOperator.h.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 11:53:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   12 Jul 2012 14:58:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add in an additional string identifier for the input and fixed clear functionality assoicated with input structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   19 Jun 2012 12:57:14   jspila
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
 *    Rev 1.24   11 Apr 2011 18:49:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated missing run status flag for controlling counter update.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   30 Mar 2011 15:13:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.40 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use STL min/max functions instead of MFC macros.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   16 Mar 2011 13:40:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   11 Mar 2011 14:27:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix extent and connection issues between image classes and tool classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   25 Feb 2011 12:17:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   10 Feb 2011 14:58:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   27 Jan 2011 11:28:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   08 Dec 2010 12:56:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   04 Nov 2010 13:43:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   01 Jun 2010 14:41:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   05 Mar 2010 11:01:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new element to run status for critical errors.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   16 Dec 2009 11:28:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   11 Mar 2008 08:50:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added CollectInputImageNames in Run
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   24 Jul 2007 14:27:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Update to MIL 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   21 Jun 2007 13:27:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   02 Nov 2005 07:33:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed RunStatus.m_lResultImageIndex to .Images
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   14 Oct 2005 11:25:56   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated CreateObject method to fix print flag errors.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   09 Aug 2005 15:57:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   call SetLegacyVectorObjectCompatibility on m_svLinearData
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   02 Aug 2005 10:40:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added ASSERT, cleaned up code
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   01 Aug 2005 11:59:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed VectorObject to ValueObject
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   29 Jul 2005 12:39:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   21 Jun 2005 08:11:52   ebeyeler
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
 *    Rev 1.2   21 Mar 2005 15:00:24   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  463
 * SCR Title:  Add new Linear Measurement Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problems with the print flag
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 Mar 2005 11:18:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  463
 * SCR Title:  Add new Linear Measurement Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated Run method to divide by the correct profile height.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Feb 2005 10:43:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  463
 * SCR Title:  Add new Linear Measurement Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Adding new files for the new linear tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
