//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEdge
//* .File Name       : $Workfile:   SVEdge.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:23:42  $
//******************************************************************************

#include "stdafx.h"
#include "SVLine.h"
#include "SVEdge.h"
#include "SVGlobal.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVEdgeListClass
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

//******************************************************************************
// Adjustments
//******************************************************************************

//******************************************************************************
// Serialization:
//******************************************************************************

//******************************************************************************
// Constructor(s):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVEdgeListClass
// -----------------------------------------------------------------------------
// .Description : Constructor
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :16.06.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
SVEdgeListClass::SVEdgeListClass()
{
	SetSize( 0, 20 );
}


//******************************************************************************
// Destructor(s):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// .Title       : ~SVEdgeListClass
// -----------------------------------------------------------------------------
// .Description : Destructor
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :16.06.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
SVEdgeListClass::~SVEdgeListClass()
{

}





//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVEdgeControlClass
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

//******************************************************************************
// Adjustments
//******************************************************************************

//******************************************************************************
// Constructor(s):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVEdgeControlClass
// -----------------------------------------------------------------------------
// .Description : Constructor
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :16.06.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
SVEdgeControlClass::SVEdgeControlClass()
{
	m_dwDirection			= SV_HEAD_TO_TAIL_DIRECTION;
	m_dwPolarisation		= SV_ANY_POLARISATION;
	m_dwEdgeSelect			= 0L;
	m_dEdgeSelectThisValue	= 1.0F;
	m_bIsFixedEdgeMarker	= FALSE;
	m_dwPosition			= 0L;
	m_dPositionOffsetValue	= 1.0F;
	m_dwLowerThresholdValue	= SV_DEFAULT_TOOL_LOWER_TRESH;
	m_dwUpperThresholdValue	= SV_DEFAULT_TOOL_UPPER_TRESH;
	m_dwUserFlags			= 0L;

	dLastValue = 0.0F;
}


////////////////////////////////////////////////////////////////////////////////
// .Title       : SVEdgeControlClass
// -----------------------------------------------------------------------------
// .Description : Assign Constructor
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :16.06.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
SVEdgeControlClass::SVEdgeControlClass( SVEdgeControlClass& S2 )
{
	m_dwDirection			= S2.m_dwDirection;
	m_dwPolarisation		= S2.m_dwPolarisation;
	m_dwEdgeSelect			= S2.m_dwEdgeSelect;
	m_dEdgeSelectThisValue	= S2.m_dEdgeSelectThisValue;
	m_bIsFixedEdgeMarker	= S2.m_bIsFixedEdgeMarker;
	m_dwPosition			= S2.m_dwPosition;
	m_dPositionOffsetValue	= S2.m_dPositionOffsetValue;
	m_dwLowerThresholdValue	= S2.m_dwLowerThresholdValue;
	m_dwUpperThresholdValue	= S2.m_dwUpperThresholdValue;
	m_dwUserFlags			= S2.m_dwUserFlags;

	edgeList.RemoveAll();
	for( int i = 0; i < S2.edgeList.GetSize(); ++i )
		edgeList.Add( S2.edgeList.GetAt( i ) );

	dLastValue		= S2.dLastValue;
	m_bIsEdgeA = TRUE;
}


//******************************************************************************
// Destructor(s):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// .Title       : ~SVEdgeControlClass
// -----------------------------------------------------------------------------
// .Description : Destructor
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :16.06.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
SVEdgeControlClass::~SVEdgeControlClass()
{
}

//******************************************************************************
// Operator(s):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// .Title       : operator=
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :16.06.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
SVEdgeControlClass& SVEdgeControlClass::operator=( SVEdgeControlClass& S2 )
{
	m_dwDirection			= S2.m_dwDirection;
	m_dwPolarisation		= S2.m_dwPolarisation;
	m_dwEdgeSelect			= S2.m_dwEdgeSelect;
	m_dEdgeSelectThisValue	= S2.m_dEdgeSelectThisValue;
	m_bIsFixedEdgeMarker	= S2.m_bIsFixedEdgeMarker;
	m_dwPosition			= S2.m_dwPosition;
	m_dPositionOffsetValue	= S2.m_dPositionOffsetValue;
	m_dwLowerThresholdValue	= S2.m_dwLowerThresholdValue;
	m_dwUpperThresholdValue	= S2.m_dwUpperThresholdValue;
	m_dwUserFlags			= S2.m_dwUserFlags;

	edgeList.RemoveAll();
	for( int i = 0; i < S2.edgeList.GetSize(); ++i )
		edgeList.Add( S2.edgeList.GetAt( i ) );

	dLastValue		= S2.dLastValue;

	return *this;
}



//******************************************************************************
// Operation(s) Of Writing Access:
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// .Title       : GetEdgeList
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//  :
// .Output(s)
//	:
//  :
// .Return Value
//  :
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :16.06.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
SVEdgeListClass& SVEdgeControlClass::GetEdgeList()
{
	return edgeList;
}

//******************************************************************************
// Operation(s) Of Process;
//******************************************************************************

#define __FORMAT_EDGE_PARAMS2 _T("%ld %ld %ld %lf %d %ld %lf %ld %ld %ld")

void SVEdgeControlClass::ConvertStringToParams( LPCTSTR pszStrValue )
{

    int nCount;
	nCount = _stscanf( 
		(LPCTSTR)pszStrValue,             // The string to convert back to values.
		__FORMAT_EDGE_PARAMS2,
		/*DWORD*/	&m_dwDirection,
		/*DWORD*/	&m_dwPolarisation,
		/*DWORD*/	&m_dwEdgeSelect,
		/*double*/	&m_dEdgeSelectThisValue,
		/*BOOL*/	&m_bIsFixedEdgeMarker,
		/*DWORD*/	&m_dwPosition,
		/*double*/	&m_dPositionOffsetValue,
		/*DWORD*/	&m_dwLowerThresholdValue,
		/*DWORD*/	&m_dwUpperThresholdValue,
		/*DWORD*/	&m_dwUserFlags
	);

	ASSERT(nCount == 10 || nCount == -1);
}
////////////////////////////////////////////////////////////////////////////////
// .Title       : CalculateEdgeList
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	:
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :16.06.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVEdgeControlClass::CalculateEdgeList( SVLineClass* PLine, BOOL BCalcForDisplaying, BOOL BCurrentDisplayedEdgeIsA /* = TRUE */ )
{
	// Remove old edges...
	edgeList.SetSize( 0, 20 );

	if( PLine )
	{
		double dValue = 0.0;

		if( m_bIsEdgeA == BCurrentDisplayedEdgeIsA )
		{
			PLine->m_dwDisplayedMaxThresholdBarValue = m_dwUpperThresholdValue;
			PLine->m_dwDisplayedMinThresholdBarValue = m_dwLowerThresholdValue;
		}

		switch( m_dwDirection )
		{
			case SV_HEAD_TO_TAIL_DIRECTION:
			{
				// Calc edges...
				for( long l = 0; l < PLine->mlLineArrayLength; ++l )
				{
  					PLine->mNormalizedArray.GetAt( l, (void *)(&dValue) );

					if( findEdge( dValue, l == 0, PLine->m_dwMaxThreshold, PLine->m_dwMinThreshold ) )
						edgeList.Add( l );
				}
				return TRUE;
			}

			case SV_TAIL_TO_HEAD_DIRECTION:
			{
				// Calc edges...
				long max = PLine->mlLineArrayLength - 1;

				for( long l = max; l >= 0; --l )
				{
					PLine->mNormalizedArray.GetAt( l, (void *)(&dValue) );

					if( findEdge( dValue, l == max, PLine->m_dwMaxThreshold, PLine->m_dwMinThreshold ) )
						edgeList.Add( l );
				}
				return TRUE;
			}
		}
	}
	return FALSE;
}



////////////////////////////////////////////////////////////////////////////////
// .Title       : CalulateCurrentEdgePosition
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	:
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :18.06.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
DWORD SVEdgeControlClass::CalculateCurrentEdgePosition( SVLineClass* PLine )
{
	if( PLine )
	{
		if ( m_bIsFixedEdgeMarker )
//		if( edgeControlData.BIsFixedEdgeMarker )
		{
			// Get the selected fixed position...
			switch ( m_dwPosition )
//			switch( edgeControlData.DWPosition )
			{
				case SV_START_POSITION:
				{
					// Get measure line start position
					// Allways 0!
					return 0L;
				}

				case SV_END_POSITION:
				{
					// Get measure line end position
					// Allways line length - 1, if length > 0!
					return( ( PLine->mlLineArrayLength > 0 ) ? PLine->mlLineArrayLength - 1L : 0L );
				}
			
				case SV_CENTER_POSITION:
				{
					// Get measure line center position
					// Allways ( line length / 2 ) - 1, if ( length / 2 ) > 0!
					long l = PLine->mlLineArrayLength / 2;
					return( ( l > 0 ) ? l - 1L : 0L );
				}

				case SV_OFFSET_POSITION:
				{
					// Get user defined measure line position
					// Allways user define position, if this position < line length!
					DWORD dwEndPos = ( DWORD ) ( PLine->mlLineArrayLength > 0 ) ? PLine->mlLineArrayLength - 1 : 0L;
					//DWORD dwOffset = ( DWORD ) edgeControlData.DPositionOffsetValue;
					DWORD dwOffset = ( DWORD ) m_dPositionOffsetValue;
					if( dwOffset <= dwEndPos )
						return dwOffset;
					else
						return -1; // Failed...
				}

				default:
					// Failed...
					return -1;
			}
		}
		else
		{
			// Get the selected searched position...
			switch ( m_dwEdgeSelect )
//			switch( edgeControlData.DWEdgeSelect )
			{
				case SV_FIRST_EDGE:
				{
					// Get first found edge position
					// Allways first edge, if edge number > 0!
					if( GetEdgeList().GetSize() > 0 )
					{
						long pos = GetEdgeList().GetAt( 0 );
						return( ( pos > 0 ) ? pos : 0L );
					}
					else
					{
						// Failed...
						return  -1;
					}
				}

				case SV_LAST_EDGE:
				{
					// Get last found edge position
					// Allways last edge, if edge number > 0!
					if( GetEdgeList().GetSize() > 0 )
					{
						long pos = GetEdgeList().GetAt( GetEdgeList().GetSize() - 1 );
						return( ( pos > 0 ) ? pos : 0L );
					}
					else
					{
						// Failed...
						return -1;
					}
				}

				case SV_THIS_EDGE:
				{
					// Get user defined edge position
					// Allways user defined edge, if edge number >= user defined edge
					// and result position is inside the line extent!
					// If the user defined edge is between two existing edges,
					// the result position is interpolated.
//					DWORD dwEdge = ( DWORD ) edgeControlData.DEdgeSelectThisValue;
					DWORD dwEdge = static_cast<unsigned long>(m_dEdgeSelectThisValue);
					if( GetEdgeList().GetSize() > 0 &&
						( ( UINT ) GetEdgeList().GetSize() ) >= dwEdge )
					{
						// It must be exists minimal one edge !

						double dEdgeRemain = m_dEdgeSelectThisValue - ( ( double ) dwEdge );

						switch( m_dwDirection )
						{
							case SV_HEAD_TO_TAIL_DIRECTION:
							{
								long firstPos   = ( dwEdge > 0 ) ? GetEdgeList().GetAt( dwEdge - 1 ) : 0;
								if( dEdgeRemain > 0.0 )
								{
									// We need to calculate an interpolated position...
									long lineEndPos = ( PLine->mlLineArrayLength > 0 ) ? PLine->mlLineArrayLength - 1 : 0L;
									long secondPos  = ( ( ( UINT ) GetEdgeList().GetSize() ) == dwEdge ) ? lineEndPos : GetEdgeList().GetAt( dwEdge );
									ASSERT( secondPos >= firstPos );
									firstPos =  ( long ) ( ( ( double ) ( secondPos - firstPos ) ) * dEdgeRemain ) + firstPos;
								}
								return( ( firstPos > 0 ) ? firstPos : 0L );
							}

							case SV_TAIL_TO_HEAD_DIRECTION:
							{
								long lineEndPos = ( PLine->mlLineArrayLength > 0 ) ? PLine->mlLineArrayLength - 1 : 0L;
								long firstPos   = ( dwEdge > 0 ) ? GetEdgeList().GetAt( dwEdge - 1 ) : lineEndPos;
								if( dEdgeRemain > 0.0 )
								{
									// We need to calculate an interpolated position...
									long secondPos  = ( ( ( UINT ) GetEdgeList().GetSize() ) == dwEdge ) ? 0L : GetEdgeList().GetAt( dwEdge );
									ASSERT( secondPos <= firstPos );
									firstPos =  ( long ) ( ( ( double ) ( secondPos - firstPos ) ) * dEdgeRemain ) + firstPos;
								}
								return( ( firstPos > 0 ) ? firstPos : 0L );
							}
						}
					}

					// Failed...
					return -1;
				}

				default:
					// Failed...
					return -1;
			}
		}
	}
	return -1; // Failed
}





////////////////////////////////////////////////////////////////////////////////
// .Title       : findEdge
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	:
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :16.06.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVEdgeControlClass::findEdge( double DCurrentValue, BOOL First, double DMaxValue, double DMinValue )
{
	// Binarizing...
//	if( edgeControlData.DWLowerThresholdValue <= edgeControlData.DWUpperThresholdValue )
//		DCurrentValue = ( edgeControlData.DWLowerThresholdValue <= (DWORD)DCurrentValue && 
//		                  DCurrentValue <= edgeControlData.DWUpperThresholdValue ) ? DMaxValue : DMinValue;
//	else
//		DCurrentValue = ( edgeControlData.DWUpperThresholdValue <= (DWORD)DCurrentValue && 
//		                  DCurrentValue <= edgeControlData.DWLowerThresholdValue ) ? DMinValue : DMaxValue;
	// Binarizing...
	if( m_dwLowerThresholdValue <= m_dwUpperThresholdValue )
		DCurrentValue = ( m_dwLowerThresholdValue <= (DWORD)DCurrentValue && 
		                  DCurrentValue <= m_dwUpperThresholdValue ) ? DMaxValue : DMinValue;
	else
		DCurrentValue = ( m_dwUpperThresholdValue <= (DWORD)DCurrentValue && 
		                  DCurrentValue <= m_dwLowerThresholdValue ) ? DMinValue : DMaxValue;




	BOOL RetVal = FALSE;	// No edge found
	if( ! First )
		switch( m_dwPolarisation )
		{
			case SV_POSITIVE_POLARISATION:
			// Meaning: LastValue < CurrentValue
			//          DwMinValue:BLACK	---> DwMaxValue:WHITE
				if( dLastValue < DCurrentValue )
					RetVal = TRUE;
				break;

			case SV_NEGATIVE_POLARISATION:
			// Meaning: LastValue > CurrentValue
			//          DwMaxValue:WHITE ---> DwMinValue:BLACK
				if( dLastValue > DCurrentValue )
					RetVal = TRUE;
				break;
	
			case SV_ANY_POLARISATION:
			// Meaning: LastValue != CurrentValue
				if( dLastValue != DCurrentValue )
					RetVal = TRUE;
				break;
		}

	dLastValue = DCurrentValue;
	return RetVal;
}

//eof:

double SVEdgeControlClass::CalculateSubPixelEdgePosition( SVLineClass *PLine, long lEdge )
{
	double dResult = (double)( lEdge );

	if( PLine )
	{
		if( !m_bIsFixedEdgeMarker )
		{
			if ( 0.0 <= dResult )
			{
				double dThresh = -1.0;
				double dLeft = -1.0;
				double dRight = -1.0;

				double dLower = (m_dwLowerThresholdValue);
				double dUpper = (m_dwUpperThresholdValue);

				switch( m_dwDirection )
				{
					case SV_HEAD_TO_TAIL_DIRECTION:
					{
						dResult -= 1.0; 

						if( lEdge > 0L )
						{
							PLine->mNormalizedArray.GetAt( lEdge - 1L, (void *)(&dLeft) );
						}// end if
						PLine->mNormalizedArray.GetAt( lEdge, (void *)(&dRight) );

						if ( fabs( dLeft - dLower ) < fabs( dLeft - dUpper ) )
						{
							dThresh = dLower;
						}
						else
						{
							dThresh = dUpper;
						}

						break;
					}
					case SV_TAIL_TO_HEAD_DIRECTION:
					{
						PLine->mNormalizedArray.GetAt( lEdge, (void *)(&dLeft) );
						if( lEdge + 1L < PLine->mlLineArrayLength )
						{
							PLine->mNormalizedArray.GetAt( lEdge + 1L, (void *)(&dRight) );
						}// end if

						if ( fabs( dRight - dLower ) < fabs( dRight - dUpper ) )
						{
							dThresh = dLower;
						}
						else
						{
							dThresh = dUpper;
						}

						break;
					}
					default:
					{
						break;
					}
				}

				if ( 0.0 <= dThresh )
				{
					double dAdjustment = 0.0;
					double dDivisor = fabs ( dLeft - dRight );

					if ( dDivisor != 0.0 )
					{
						dAdjustment = fabs( dLeft - dThresh ) / dDivisor;
					}

					dResult = dResult + dAdjustment;
				}
			}

		}// end if
		else
		{
			switch( m_dwPosition )
			{
			case SV_START_POSITION :
				dResult = 0.0F;
				break;
			case SV_END_POSITION :
				dResult = (double) PLine->mlLineArrayLength;
				break;
			case SV_CENTER_POSITION :
				dResult = ( (double) PLine->mlLineArrayLength ) / 2.0F;
				break;
			case SV_OFFSET_POSITION :
				dResult = m_dPositionOffsetValue;
				break;
			}// end switch
			
		}// end else
	}

	return dResult;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVEdge.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:23:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.16   27 Jan 2011 11:06:46   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to fix issues with Matrox Buffer Management.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.15   27 Oct 2010 09:32:56   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  704
   SCR Title:  Upgrade SVObserver Version for 5.33 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to fix include issues and comment issues.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.14   16 Dec 2009 10:21:44   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  677
   SCR Title:  Fix problem in camera notify thread
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fix issues with includes and comments.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.13   21 Jun 2007 12:46:44   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  598
   SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
   Checked in by:  jSpila;  Joseph Spila
   Change Description:  
     These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12   17 Feb 2005 14:01:18   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Update class with new methodologies based on new base object, extents and reset structure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   18 Dec 2003 13:59:56   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Fixed sub pixel position for fixed edge markers and correct dialog display logic for dual edge profile analyzers.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   04 Dec 2003 12:15:10   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Redid edge control value objects: creation, setup dialogs, validation, runtime, persistence.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   13 Nov 2003 07:37:06   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     fixed problems with the calculating of the edges. 
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   08 Oct 2003 16:23:04   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Made more changes to the line analyzers to support printing and correct use of the edge related value objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   08 Oct 2003 10:55:40   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     added new enum for direction not used
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   22 Apr 2003 09:16:48   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   29 Jan 2003 15:38:16   joe
   Project:  SVObserver
   Change Request (SCR) nbr:  301
   SCR Title:  Implement Result Image Circle Buffer
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Update include file names.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   08 Nov 2001 13:10:24   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  255
   SCR Title:  Add Sub-pixel resolution to Profile and Line Gage Tool Analyzers
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated constructor, operator=, CalculateEdgeList and CalculateCurrentEdgePosition methods to include sub-pixel resolution.
   
   Updated findEdge method and changed parameters to include sub-pixel resolution.
   
   Added CalculateSubPixelEdgePosition method and changed parameters to include sub-pixel resolution.
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   04 Oct 2001 14:40:32   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  229
   SCR Title:  Upgrade SVObserver to Version 3.34 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Modified source code to remove compiler warning message.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   09 Jun 2000 10:28:40   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  156
   SCR Title:  Gage Tool Tail to Head problem
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised CalculateEdgeList for tail to head direction.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.1   Feb 01 2000 16:34:26   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  98
   SCR Title:  Gage Tool Edge Tool Crash
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Add code that checks for PLineArray[0] access overrun which caused a crash.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.0   23 Nov 1999 11:27:22   mike
   Project:  SVObserver
   Change Request (SCR) nbr:  61
   SCR Title:  Update PVCS versioning to version 3.0.
   Checked in by:  Mike;  Mike McCarl
   Change Description:  
     
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.4   11 Nov 1999 13:44:18   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  21
   SCR Title:  Port Gage Tool code from program version 2.42.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Revised to initialize dwLastValue in constructor
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.3   Sep 30 1999 13:44:14   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  41
   SCR Title:  Version 3.00 Beta 5 Versioning.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changes required to script edge control parameters.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
