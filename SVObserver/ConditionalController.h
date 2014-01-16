//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Markt Schwaben
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ConditionalController
//* .File Name       : $Workfile:   ConditionalController.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Jan 2014 12:04:04  $
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
#pragma endregion

namespace Seidenader
{
	namespace SVObserver
	{
		class ConditionalController : public FormulaController
		{
#pragma region Constructor
		public:
			ConditionalController( SVTaskObjectClass& pObject );
			ConditionalController();
#pragma endregion

#pragma region Public Methods
		public:
#pragma region Virtual Methods (IFormulaController)
			virtual HRESULT isToolAndEquationEnabled(bool& toolEnabled, bool& equationEnabled) const override;

			virtual HRESULT setToolAndEquationEnabled(bool toolEnabled, bool equationEnabled) override;
			virtual void setTaskObject( SVTaskObjectClass& pObject ) override;
#pragma endregion Virtual Methods
#pragma endregion

#pragma region Member variables
		private:
			SVIPDoc* m_pIPDoc;
			SVToolClass* m_pTool;
			SVTaskObjectValueInterface m_objectValueInterface;
#pragma endregion
		};
	} //namespace SVObserver
} //namespace Seidenader

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\ConditionalController.h_v  $
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
