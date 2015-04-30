//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Markt Schwaben
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : FormulaController
//* .File Name       : $Workfile:   FormulaController.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   17 Jul 2014 17:39:42  $
//******************************************************************************
//Description:  FormulaController is the class to get/set and
//              validate equation objects inside SVObserver.
//              From outside SVObserver (in the GUI), use IFormulaController.

#pragma once

#pragma region Includes
#include "ObjectInterfaces/IFormulaController.h"
#include "SVTaskObject.h"
#include "SVMathContainer.h"
#include "SVEquation.h"
#pragma endregion

class FormulaController : public SvOi::IFormulaController
{
#pragma region Constructor
public:
	FormulaController( SVTaskObjectClass& pObject );
	FormulaController();
#pragma endregion Constructor

#pragma region Public Methods
public:
#pragma region Virtual Methods (IFormulaController)
	virtual SVString getEquationText() const override;
	virtual SVStringArray getPPQVariableNames() const override;
	virtual SvOi::IInspectionProcess* getInspectionProcess() const override;
	virtual SvOi::IOutputInfoListClass& GetToolSetOutputList( ) const override;
	virtual HRESULT isToolAndEquationEnabled(bool& toolEnabled, bool& equationEnabled) const override;

	virtual HRESULT setToolAndEquationEnabled(bool toolEnabled, bool equationEnabled) override;
	virtual void setTaskObject( SvOi::IObjectClass& rObject ) override;

	//**********
	/// Validate an equationstring
	/// \param equationString [in] the equation string.
	/// \param result [out] return the result of the equation, if the validation is successfully.
	/// \param bRestore[in] boolean for determining whether to restore to the previous equation string.
	/// \return return the position of the failure. If the validation is successful, the value will be "validateSuccessful".
	//**********
	virtual int validateEquation(const SVString &equationString, double& result, bool bRestore) const override;
	virtual int validateAndSetEquation(const SVString &equationString, double& result) override;
#pragma endregion Virtual Methods (IFormulaController)
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	/**********
	Set the task object which use the equation. The virtual protected method, is called from public version.
	\param pObject [in] task object.
	**********/
	virtual void setTaskObjectClass( SVTaskObjectClass& rObject );

	/**********
	Set equation-class and struct.
	\param pEquation [in] the equation class.
	**********/
	void setEquation( SVEquationClass* pEquation );
#pragma endregion Protected Methods

#pragma region Member Variables
protected:
	SVInspectionProcess* m_pInspection;
	SVToolSetClass*      m_pToolSet;
	SVEquationClass*     m_pEquation;
#pragma endregion Member Variables
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\FormulaController.h_v  $
 * 
 *    Rev 1.2   17 Jul 2014 17:39:42   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Removed namespaces and code review changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 May 2014 04:48:44   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  877
 * SCR Title:  Add undo-button to formula and conditional pages
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   removed const for return value of method getPPQVariableNames
 * used reference for parameter of method validateEquation and validateAndSetEquation 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
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