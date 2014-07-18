//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Markt Schwaben
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ConditionalController
//* .File Name       : $Workfile:   ConditionalController.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   17 Jul 2014 17:39:40  $
//******************************************************************************
//Description:  ConditionalController is the class to get/set and
//              validate equation objects inside SVObserver.
//			    From outside SVObserver (in the GUI), use IFormulaController.

#pragma once

#pragma region Includes
#include "IFormulaController.h"
#include "FormulaController.h"
#include "SVTaskObject.h"
#include "SVMathContainer.h"
#include "SVEquation.h"
#include "SVTaskObjectValueInterface.h"
#pragma endregion Includes

class ConditionalController : public FormulaController
{
#pragma region Constructor
public:
	ConditionalController( SVTaskObjectClass& pObject );
	ConditionalController();
#pragma endregion Constructor

#pragma region Public Methods
public:
#pragma region Virtual Methods (IFormulaController)
	virtual HRESULT isToolAndEquationEnabled(bool& toolEnabled, bool& equationEnabled) const override;

	virtual HRESULT setToolAndEquationEnabled(bool toolEnabled, bool equationEnabled) override;
	virtual void setTaskObject( SVTaskObjectClass& pObject ) override;
#pragma endregion Virtual Methods (IFormulaController)
#pragma endregion Public Methods

#pragma region Member Variables
private:
	SVIPDoc* m_pIPDoc;
	SVToolClass* m_pTool;
	SVTaskObjectValueInterface m_objectValueInterface;
#pragma endregion Member Variables
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\ConditionalController.h_v  $
 * 
 *    Rev 1.1   17 Jul 2014 17:39:40   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Removed namespaces and code review changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Jan 2014 12:04:04   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  877
 * SCR Title:  Add undo-button to formula and conditional pages
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/