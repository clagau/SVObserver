//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ResultViewEnvReferences
//* .File Name       : $Workfile:   ResultViewEnvReferences.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   07 Jan 2015 17:35:04  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "ResultViewReferences.h"
#pragma endregion Includes

#pragma region Declarations
class SVInspectionProcess;
#pragma endregion Declarations

class ResultViewEnvReferences : public ResultViewReferences
{
public:
#pragma region Constructor
	//************************************
	// Description:  Default constructor for the ResultViewEnvReferences class.
	//************************************
	ResultViewEnvReferences();

	//************************************
	// Description:  Destructor for the ResultViewEnvReferences class.
	//************************************
	virtual ~ResultViewEnvReferences();
#pragma endregion Constructor

#pragma region Public Methods
	//************************************
	// Description:  Build reference vector from Environment and PPQ variables in the ObjectManager with VIEWABLE attribute set.
	// Parameter:  pProcess
	//************************************
	virtual void BuildReferenceVector(SVInspectionProcess* pProcess);
#pragma endregion Public Methods
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\ResultViewEnvReferences.h_v  $
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