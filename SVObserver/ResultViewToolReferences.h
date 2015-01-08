//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ResultViewToolReferences
//* .File Name       : $Workfile:   ResultViewToolReferences.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   07 Jan 2015 17:35:04  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "ResultViewReferences.h"
#include "SVInspectionProcess.h"
#pragma endregion Includes

class ResultViewToolReferences: public ResultViewReferences
{
public:
#pragma region Constructor
	//************************************
	// Description:  Default constructor for the ResultViewToolReferences class.
	//************************************
	ResultViewToolReferences();

	//************************************
	// Description:  Destructor for the ResultViewToolReferences class.
	//************************************
	virtual ~ResultViewToolReferences();
#pragma endregion Constructor

#pragma region Public Methods
	//************************************
	// Description:  Build the reference vector.  Insert all outputs from the Root Object which have the VIEWABLE attribute set.
	// Parameter:  pProcess
	//************************************
	virtual void BuildReferenceVector(SVInspectionProcess* pProcess);
#pragma endregion Public Methods
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\ResultViewToolReferences.h_v  $
 * 
 *    Rev 1.0   07 Jan 2015 17:35:04   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  980
 * SCR Title:  Add Non-Inspection Objects to the Result View
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/