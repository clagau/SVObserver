//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLine
//* .File Name       : $Workfile:   SVLine.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 13:12:46  $
//******************************************************************************

#ifndef SVLINE_H
#define SVLINE_H

#include "SVDataBuffer.h"
#include "SVValueBaseNormalizerClass.h"
#include "SVImageLibrary/SVImageExtentClass.h"
#include "SVGlobal.h"

class SVAnalyzerClass;
class SVImageClass;
class SVToolClass;

class SVDataArrayClass
{
public:
	SVDataArrayClass( DWORD DataType = SV_BYTE );

	~SVDataArrayClass()	{ FreeExtra(); };

	virtual BOOL SetSize( int NewSize = -1, int NewGrow = 20, DWORD NewDataType = 0 );
	virtual void RemoveAll() { SetSize( 0 ); };
	virtual void FreeExtra() { SetSize( -1 ); };

	virtual BOOL SetAt( int Index, void* PNewData );

public:
	virtual DWORD	GetData();
	virtual int		GetDataSize( DWORD NewDataType = 0L );
	virtual int		GetSize();

	virtual DWORD	GetAt( int Index, void* PReturnedData = NULL );

protected:

	BYTE*	pData;
	int		size;
	int		oldSize;
	int		grow;
	DWORD	type;
};


class SVLineClass : public SVObjectAppClass
{
	SV_DECLARE_CLASS( SVLineClass );

public:
	typedef std::vector< unsigned long > SVResultVector;

	SVLineClass();   
	virtual ~SVLineClass();

	virtual BOOL CloseObject();

	virtual HRESULT ResetObject();

	virtual BOOL Destroy();

	virtual HRESULT SetObjectValue( SVObjectAttributeClass* PDataObject );

	virtual void *GetDataBuffer();

	virtual BOOL ReadLine();
	virtual BOOL ReadData( double dProfileHeight = 1.0 );

	virtual HRESULT GetHeadPoint( POINT &p_oPoint );
	virtual HRESULT GetTailPoint( POINT &p_oPoint );

	virtual void Persist(SVObjectWriter& rWriter);

	SVImageExtentClass m_svLineExtent;

	long mlLinePixelLength;

	long mlLineArrayLength;

	SVDataArrayClass mLineArray;
	SVDataArrayClass mNormalizedArray;

	SVResultVector mResultArray;

	BOOL m_bDisplayGraph;

	DWORD m_dwLineArrayDataType;

	long m_lPixelDepth;
	
	//*
	DWORD m_dwMaxThreshold;
	DWORD m_dwMinThreshold;
	DWORD m_dwColorNumber;
	DWORD m_dwDisplayedMaxThresholdBarValue;
	DWORD m_dwDisplayedMinThresholdBarValue;

	SVValueBaseNormalizerClass	m_svNormalizer;
	//*/

protected:
	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVLine.h_v  $
 * 
 *    Rev 1.2   15 May 2014 13:12:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised processMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Feb 2014 10:48:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 11:47:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   11 Sep 2012 18:59:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated class to correct the persistance problem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   16 Aug 2012 18:55:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix issue when not clearing result array when edge data is not present.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   27 Jan 2011 11:28:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   28 Oct 2010 13:54:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   16 Dec 2009 11:24:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   22 Jun 2005 13:32:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   SetObjectValue now returns SVHRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   17 Feb 2005 14:37:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   22 Apr 2003 10:39:12   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   08 Nov 2001 14:20:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  255
 * SCR Title:  Add Sub-pixel resolution to Profile and Line Gage Tool Analyzers
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated method prototypes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   19 Apr 2001 14:41:04   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   03 Mar 2000 12:16:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  112
 * SCR Title:  Expose Extents of Images
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Extent and AbsoluteExtent variables to be private.
 * Added Get and Set methods to access extents.
 * Revised GetObjectScript to write out doubles for extents.
 * Revised SetObjectValue to accept doubles for extents
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   25 Feb 2000 16:21:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added variable for translation (POINT) to SVLineInfoClass.
 * Revised SVLineClass GetObjectScript/SetObjectValue
 * methods to save and restore the translation point.
 * REmoved unnecessary initialization in the SVLineClass CTor
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 12:58:12   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   Oct 04 1999 10:13:28   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changed to support 360 Degree Measurement.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   Sep 10 1999 17:59:42   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  33
 * SCR Title:  Port Profile Tool from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added ReadData()
 * Removed old PointDraw(), Draw(), DrawSingleLine()
 * Remove obsolete parameter of Line Info.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   04 Sep 1999 17:31:12   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to save/restore extents, absolute extents, 
 * rotationAngle, rotationCenter, and pixelDepth via
 * ObjectScript.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   30 Aug 1999 20:10:50   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added processMessage method to handle
 * SVMSGID_CREATE_ALL_OBJECTS.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   25 Aug 1999 12:31:56   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert Massinger
 * Change Description:  
 *   Ported to 3.0. Used by Line ROI and Line Analyzers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
