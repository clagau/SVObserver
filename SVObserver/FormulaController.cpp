//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Markt Schwaben
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : FormulaController
//* .File Name       : $Workfile:   FormulaController.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   27 Aug 2014 01:22:48  $
//******************************************************************************
//Description:  FormulaController is the class to get/set and
//              validate equation objects inside SVObserver.
//			    From outside SVObserver (in the GUI), use IFormulaController.

#pragma region Includes
#include "Stdafx.h"
#include "FormulaController.h"
#include "SVInspectionProcess.h"
#include "SVObjectLibrary\SVObjectClass.h"
#include "SVTool.h"
#include "SVToolSet.h"
#pragma endregion

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

#pragma region Constructor
FormulaController::FormulaController( SVTaskObjectClass& pObject )
: m_pInspection(nullptr)
, m_pToolSet(nullptr)
, m_pEquation (nullptr)
{
	setTaskObject( pObject );
}

FormulaController::FormulaController()
: m_pInspection(nullptr)
, m_pToolSet(nullptr)
, m_pEquation (nullptr)
{
}
#pragma endregion Constructor

#pragma region Public Methods
#pragma region virtual Methods IFormulaController
SVString FormulaController::getEquationText() const
{
	SVString equationText("");
	ASSERT( m_pEquation );
	if( nullptr != m_pEquation )
	{
		m_pEquation->GetEquationText( equationText );
	}
	return equationText;
}

SVStringArray FormulaController::getPPQVariableNames() const
{
	
	if ( nullptr != m_pInspection )
	{
		return m_pInspection->getPPQVariableNames();
	}
	else
	{
				std::vector<SVString> retVals;
				return retVals;
	}
	
	
}

SvOi::IInspectionProcess* FormulaController::getInspectionProcess() const
{
	return m_pInspection;
}

SvOi::IOutputInfoListClass& FormulaController::GetToolSetOutputList( ) const
{
	//Need to be static not to loss information at return point
	static SVOutputInfoListClass outputInfoList;
	//Need to be deleted because it is static and can be used before.
	outputInfoList.RemoveAll();
	if (nullptr != m_pToolSet )
	{
		m_pToolSet->GetOutputList(outputInfoList);
	}
	
	return outputInfoList;
}

HRESULT FormulaController::isToolAndEquationEnabled(bool& toolEnabled, bool& equationEnabled) const
{
	if ( nullptr != m_pToolSet && nullptr != m_pEquation )
	{
		toolEnabled = m_pToolSet->IsEnabled();
		equationEnabled = (TRUE == m_pEquation->IsEnabled());
		return S_OK;
	}

	return S_FALSE;
}

HRESULT FormulaController::setToolAndEquationEnabled(bool toolEnabled, bool equationEnabled)
{
	// to nothing, because for this class is the function invalid.
	return S_FALSE;
}

void FormulaController::setTaskObject( SvOi::IObjectClass& rObject )
{
	SVTaskObjectClass &rTaskObject = dynamic_cast<SVTaskObjectClass&>(rObject);
	setTaskObjectClass(rTaskObject);
}

int FormulaController::validateEquation( const SVString &equationString, double& result, bool bRestore ) const
{
	int retValue = validateSuccessful;
	SVString oldString("");
	if (bRestore)
	{
		//save old string
		m_pEquation->GetEquationText(oldString);
	}
	m_pEquation->SetEquationText(equationString);

	SVEquationTestResult testResult = m_pEquation->Test();
	if (testResult.bPassed)
	{// set result and set return value to successful
		result = m_pEquation->GetYACCResult();
		retValue = validateSuccessful;
	}
	else
	{  // set return value to position of failed
		retValue = testResult.iPositionFailed;
	}
	if (bRestore)
	{
		//reset old string
		m_pEquation->SetEquationText(oldString);
	}
	return retValue;
}

int FormulaController::validateAndSetEquation( const SVString &equationString, double& result )
{
	int retValue = validateEquation( equationString, result, false);

	if ( validateSuccessful == retValue )
	{
		//set new equation text and reset all objects for using the new value
		m_pEquation->SetEquationText(equationString);
		SVObjectClass* object = dynamic_cast<SVObjectClass*>(m_pEquation->GetTool());
		if( nullptr != object )
		{
			DWORD_PTR l_dwRet = ::SVSendMessage( object, SVM_RESET_ALL_OBJECTS & ~SVM_NOTIFY_FRIENDS , 0, 0); // Do not reset friends because they may be invalid.
			if( SVMR_SUCCESS != l_dwRet )
			{
				return setFailed;
			}
		}
	}
	return retValue;
}
#pragma endregion virtual Methods IFormulaController
#pragma endregion Public Methods

#pragma region Protected Methods
void FormulaController::setTaskObjectClass( SVTaskObjectClass& rObject )
{
	m_pInspection = rObject.GetInspection();
	m_pToolSet = m_pInspection->GetToolSet();

	SVEquationClass* pEquation = dynamic_cast <SVEquationClass*> (&rObject);
	if ( pEquation == nullptr )
	{
		SVObjectTypeInfoStruct info;
		info.ObjectType = SVEquationObjectType;
		info.SubType    = SVMathEquationObjectType;	// we are not looking for conditional equation !!!
		pEquation = dynamic_cast < SVEquationClass* >
			( reinterpret_cast < SVObjectClass* > (::SVSendMessage( &rObject, SVM_GETFIRST_OBJECT | SVM_NOTIFY_ONLY_FRIENDS, 0, reinterpret_cast<DWORD_PTR>( &info ) )));
	}
	// Set the pointer to the Equation Class Object 
	setEquation( pEquation );
}

void FormulaController::setEquation( SVEquationClass* pEquation )
{
	m_pEquation = pEquation;
}
#pragma endregion Protected Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\FormulaController.cpp_v  $
 * 
 *    Rev 1.5   27 Aug 2014 01:22:48   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Fixed input variable sort problem
 * Changed methods: getPPQVariableNames
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   17 Jul 2014 17:39:40   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Removed namespaces and code review changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   15 May 2014 10:06:52   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised to correct casting issues
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 May 2014 04:47:40   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  877
 * SCR Title:  Add undo-button to formula and conditional pages
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   fixed use of the constructor
 * changed order of if-comparisons 
 * removed using
 * removed const for return value of method getPPQVariableNames
 * used reference for parameter of method validateEquation and validateAndSetEquation
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Feb 2014 10:16:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed sendmessage to use LONG_PTR instead of DWORD.
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