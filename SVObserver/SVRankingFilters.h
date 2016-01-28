//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRankingFilters
//* .File Name       : $Workfile:   SVRankingFilters.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   13 Aug 2013 09:58:58  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVFilterClass.h"
#include "SVMatroxLibrary/SVMatroxBuffer.h"
#pragma endregion Includes

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVRankingFilterClass
// -----------------------------------------------------------------------------
// .Description : Base class for all kinds of custom filters
// -----------------------------------------------------------------------------
// .Export
//	 Public Method				Description
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
////////////////////////////////////////////////////////////////////////////////
class SVRankingFilterClass : public SVFilterClass
{
	SV_DECLARE_CLASS( SVRankingFilterClass );

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
// Standard constructor
	SVRankingFilterClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVRANKINGFILTER );
	virtual ~SVRankingFilterClass();
	
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );

	virtual HRESULT ResetObject();

	virtual BOOL RebuildRanking();

#pragma region virtual method (IFilter)
	virtual bool shouldResetInspection() const override { return true; }
#pragma region virtual method (IFilter)

private:
	void init();

protected:
	virtual BOOL onRun( BOOL First, SVSmartHandlePointer RInputImageHandle, SVSmartHandlePointer ROutputImageHandle, SVRunStatusClass& RRunStatus );
	virtual BOOL OnValidate();

//******************************************************************************
// Operator(s):
//******************************************************************************

public:
	SVMatroxBuffer m_milRanking;

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
	SVLongValueObjectClass *m_plvoRankingCells[cellSize];

	SVLongValueObjectClass m_lvoRankingWidth;
	SVLongValueObjectClass m_lvoRankingHeight;
	SVLongValueObjectClass m_lvoRankingRank;

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
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVRankingFilters.h_v  $
 * 
 *    Rev 1.1   13 Aug 2013 09:58:58   bWalter
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
 *    Rev 1.0   23 Apr 2013 14:36:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   10 Feb 2011 15:11:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   27 Jan 2011 11:54:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   24 Jul 2007 15:44:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Update MIL 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 Aug 2005 08:09:44   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Feb 2005 15:23:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   04 Nov 2003 14:40:02   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  394
 * SCR Title:  Implement new RankingFilter for WindowTool that will also be remotely settable
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added new ranking filter class that has a 1-7x1-7 customizable neighborhood.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
//** EOF **
