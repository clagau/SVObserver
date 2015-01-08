//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ResultViewPPQInputReferences
//* .File Name       : $Workfile:   ResultViewPPQInputReferences.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   07 Jan 2015 17:35:06  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "ResultViewReferences.h"
#pragma endregion Includes

#pragma region Declarations
class SVInspectionProcess;
#pragma endregion Declarations

class ResultViewPPQInputReferences: public ResultViewReferences
{
public:
#pragma region Constructor
	//************************************
	// Description:  Default constructor for the ResultViewPPQInputReferences class.
	//************************************
	ResultViewPPQInputReferences();

	//************************************
	// Description:  Destructor for the ResultViewPPQInputReferences class.
	//************************************
	virtual ~ResultViewPPQInputReferences();
#pragma endregion Constructor

#pragma region Public Methods
	//************************************
	// Description:  
	// Parameter:  pInspection:  
	//************************************
	void BuildReferenceVector(SVInspectionProcess* pInspection);
#pragma endregion Public Methods
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\ResultViewPPQInputReferences.h_v  $
 * 
 *    Rev 1.0   07 Jan 2015 17:35:06   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  980
 * SCR Title:  Add Non-Inspection Objects to the Result View
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/