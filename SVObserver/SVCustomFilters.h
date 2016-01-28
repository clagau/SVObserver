//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCustomFilters
//* .File Name       : $Workfile:   SVCustomFilters.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   13 Aug 2013 09:59:02  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVMatroxLibrary/SVMatroxBuffer.h"
#include "SVFilterClass.h"
#pragma endregion Includes

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVCustomFilterClass
// -----------------------------------------------------------------------------
// .Description : Base class for all kinds of custom filters
// -----------------------------------------------------------------------------
// .Export
//	 Public Method				Description
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
////////////////////////////////////////////////////////////////////////////////
class SVCustomFilterClass : public SVFilterClass
{
	SV_DECLARE_CLASS( SVCustomFilterClass );

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
// Standard constructor
	SVCustomFilterClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVCUSTOMFILTER );
	virtual ~SVCustomFilterClass();
	
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );

	virtual HRESULT ResetObject();

#pragma region virtual method (IFilter)
	virtual bool shouldResetInspection() const override { return true; }
#pragma region virtual method (IFilter)

private:
	void init();

protected:
	virtual BOOL RebuildKernel();

	virtual BOOL onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus );
	virtual BOOL OnValidate();

//******************************************************************************
// Operator(s):
//******************************************************************************

public:
	SVMatroxBuffer m_milKernel;

////////////////////////////////////////////////////////////////////////////////
// Create Operator
////////////////////////////////////////////////////////////////////////////////

//******************************************************************************
// Operation(s) Of Reading Access:
//******************************************************************************
public:
	SVLongValueObjectClass m_lvoCell01;
	SVLongValueObjectClass m_lvoCell02;
	SVLongValueObjectClass m_lvoCell03;
	SVLongValueObjectClass m_lvoCell04;
	SVLongValueObjectClass m_lvoCell05;
	SVLongValueObjectClass m_lvoCell06;
	SVLongValueObjectClass m_lvoCell07;
	SVLongValueObjectClass m_lvoCell08;
	SVLongValueObjectClass m_lvoCell09;
	SVLongValueObjectClass m_lvoCell10;
	SVLongValueObjectClass m_lvoCell11;
	SVLongValueObjectClass m_lvoCell12;
	SVLongValueObjectClass m_lvoCell13;
	SVLongValueObjectClass m_lvoCell14;
	SVLongValueObjectClass m_lvoCell15;
	SVLongValueObjectClass m_lvoCell16;
	SVLongValueObjectClass m_lvoCell17;
	SVLongValueObjectClass m_lvoCell18;
	SVLongValueObjectClass m_lvoCell19;
	SVLongValueObjectClass m_lvoCell20;
	SVLongValueObjectClass m_lvoCell21;
	SVLongValueObjectClass m_lvoCell22;
	SVLongValueObjectClass m_lvoCell23;
	SVLongValueObjectClass m_lvoCell24;
	SVLongValueObjectClass m_lvoCell25;
	SVLongValueObjectClass m_lvoCell26;
	SVLongValueObjectClass m_lvoCell27;
	SVLongValueObjectClass m_lvoCell28;
	SVLongValueObjectClass m_lvoCell29;
	SVLongValueObjectClass m_lvoCell30;
	SVLongValueObjectClass m_lvoCell31;
	SVLongValueObjectClass m_lvoCell32;
	SVLongValueObjectClass m_lvoCell33;
	SVLongValueObjectClass m_lvoCell34;
	SVLongValueObjectClass m_lvoCell35;
	SVLongValueObjectClass m_lvoCell36;
	SVLongValueObjectClass m_lvoCell37;
	SVLongValueObjectClass m_lvoCell38;
	SVLongValueObjectClass m_lvoCell39;
	SVLongValueObjectClass m_lvoCell40;
	SVLongValueObjectClass m_lvoCell41;
	SVLongValueObjectClass m_lvoCell42;
	SVLongValueObjectClass m_lvoCell43;
	SVLongValueObjectClass m_lvoCell44;
	SVLongValueObjectClass m_lvoCell45;
	SVLongValueObjectClass m_lvoCell46;
	SVLongValueObjectClass m_lvoCell47;
	SVLongValueObjectClass m_lvoCell48;
	SVLongValueObjectClass m_lvoCell49;
	static const int cellSize = 49;
	SVLongValueObjectClass *m_plvoKernelCells[cellSize];

	SVLongValueObjectClass m_lvoKernelWidth;
	SVLongValueObjectClass m_lvoKernelHeight;
	SVBoolValueObjectClass m_bvoClippingOn;
	SVBoolValueObjectClass m_bvoAbsoluteValue;	
	SVLongValueObjectClass m_lvoTransformationFactor;

//******************************************************************************
// Operation(s) Of Process:
//******************************************************************************
protected:

//******************************************************************************
// Data Element(s):
//******************************************************************************
protected:
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVCustomFilters.h_v  $
 * 
 *    Rev 1.1   13 Aug 2013 09:59:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Included SVFilterClass.h instead of SVOperator.h.
 *   Added SVMatroxBuffer include.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:04:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   10 Feb 2011 14:33:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   27 Jan 2011 10:58:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   24 Jul 2007 11:47:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upgrade to Matrox Imaging Library 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   08 Aug 2005 15:35:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   17 Feb 2005 13:30:44   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   04 Nov 2003 14:11:46   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  378
 * SCR Title:  Implement new CustomFilter for WindowTools that will also be remotely settable
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Update the custom filter class to be 1-7x1-7 customizable kernel instead of a static 3x3 one.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   11 Jul 2003 08:02:56   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added destructor to SVCustomFilter class to clean up MIL allocation.  Updated Constructor to initialize MIL handle to M_NULL.  Updated RebuildKernal to use the display system handle.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   08 Jul 2003 10:16:48   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  378
 * SCR Title:  Implement new CustomFilter for WindowTools that will also be remotely settable
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added new code and classes for the new custom filters objects and dialogs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
*/
//** EOF **
