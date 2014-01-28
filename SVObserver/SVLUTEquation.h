//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLUTEquation
//* .File Name       : $Workfile:   SVLUTEquation.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   27 Jan 2014 15:44:20  $
//******************************************************************************

#ifndef SVLUTEQUATION_H
#define SVLUTEQUATION_H

#include "SVEquation.h"
#include "SVValueObject.h"

class SVLUTEquationClass : public SVEquationClass
{
	SV_DECLARE_CLASS( SVLUTEquationClass );

public:
	SVLUTEquationClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVLUTEQUATION );
	~SVLUTEquationClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );

	virtual HRESULT ResetObject();

	virtual BOOL OnValidate();

	BOOL SetDefaultFormula();

protected:
	void init();
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );


//******************************************************************************
// Data Element(s):
//******************************************************************************
protected:
	// Flag if the values should be clipped (> 255 -> 255 and < 0 -> 0) 
	SVBoolValueObjectClass m_isLUTFormulaClipped;

	// Contains the new LUT values after running.
	// Provides a LUT table size of 256!
	SVByteValueObjectClass m_byteVectorResult;

	// LUT Table Index Counter...
	// Can be used in Equation as an index variable running from 0 to 256.
	// NOTE:
	// If Equation supports vector calculations also in future time,
	// can be replaced with a vector with 256 elements [0...255].
	SVLongValueObjectClass	m_lutIndex;
};

#endif	// SVLUTEQUATION_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVLUTEquation.h_v  $
 * 
 *    Rev 1.1   27 Jan 2014 15:44:20   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  883
 * SCR Title:  Clip-mode for LUT formula mode
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Added member m_isLUTFormulaClipped.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 12:19:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   01 Aug 2005 12:00:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed VectorObject to ValueObject
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   17 Feb 2005 14:44:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Apr 2003 10:57:42   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   19 Apr 2001 14:39:00   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   23 Mar 2000 17:33:16   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  118
 * SCR Title:  Integration of LUT Operator
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Introduced SetDefaultFormulas() to set LUT index as default
 * formula.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Mar 2000 14:50:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  118
 * SCR Title:  Integration of LUT Operator
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin.
 * Provides Formula calculation for LUT Vector with 256 elements.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/