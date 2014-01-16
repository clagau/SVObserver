//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Markt Schwaben
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : FormulaController
//* .File Name       : $Workfile:   FormulaController.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Jan 2014 12:04:06  $
//******************************************************************************
//Description:  FormulaController is the class to get/set and
//              validate equation objects inside SVObserver.
//			    From outside SVObserver (in the GUI), use IFormulaController.

#pragma once
#pragma region Includes
#include "IFormulaController.h"
#include "SVTaskObject.h"
#include "SVMathContainer.h"
#include "SVEquation.h"
#pragma endregion

namespace Seidenader
{
	namespace SVObserver
	{
		class FormulaController : public IFormulaController
		{
#pragma region Constructor
		public:
			FormulaController( SVTaskObjectClass& pObject );
			FormulaController();
#pragma endregion

#pragma region Public Methods
		public:
#pragma region Virtual Methods (IFormulaController)
			virtual SVString getEquationText() const;
			virtual const std::vector<SVString> getPPQVariableNames() const;
			virtual SVToolSetClass* getToolSet() const;
			virtual HRESULT isToolAndEquationEnabled(bool& toolEnabled, bool& equationEnabled) const;

			virtual HRESULT setToolAndEquationEnabled(bool toolEnabled, bool equationEnabled);
			virtual void setTaskObject( SVTaskObjectClass& pObject );

			virtual int validateEquation(const SVString equationString, double& result) const;
			virtual int validateAndSetEquation(const SVString equationString, double& result);
#pragma endregion Virtual Methods
#pragma endregion

#pragma region Protected Methods
		protected:
			/**********
			 Set equation-class and struct.
			 \param pEquation [in] the equation class.
			 **********/
			void setEquation( SVEquationClass* pEquation );
#pragma endregion

#pragma region Member variables
		protected:
			SVInspectionProcess*   m_pInspection;
			SVToolSetClass*        m_pToolSet;
			SVEquationClass*   m_pEquation;
			SVEquationStruct*  m_pEquationStruct;
#pragma endregion
		};
	} //namespace SVObserver
} //namespace Seidenader

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\FormulaController.h_v  $
 * 
 *    Rev 1.0   14 Jan 2014 12:04:06   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  877
 * SCR Title:  Add undo-button to formula and conditional pages
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
