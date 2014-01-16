//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Markt Schwaben
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : FormulaController
//* .File Name       : $Workfile:   FormulaController.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Jan 2014 12:04:04  $
//******************************************************************************
//Description:  FormulaController is the class to get/set and
//              validate equation objects inside SVObserver.
//			    From outside SVObserver (in the GUI), use IFormulaController.

#pragma region Includes
#include "Stdafx.h"
#include "FormulaController.h"
#include "SVInspectionProcess.h"
#include "..\SVObjectLibrary\SVObjectClass.h"
#include "SVTool.h"
#include "SVToolSet.h"
#pragma endregion

using std::vector;

namespace Seidenader
{
	namespace SVObserver
	{
#pragma region Constructor
		FormulaController::FormulaController( SVTaskObjectClass& pObject )
		{
			FormulaController();
			setTaskObject( pObject );
		}

		FormulaController::FormulaController( )
		{
			m_pInspection = nullptr;
			m_pToolSet = nullptr;
			m_pEquation = nullptr;
			m_pEquationStruct = nullptr;
		}
#pragma endregion

#pragma region Public Methods
#pragma region virtual Methods IFormulaController
		SVString FormulaController::getEquationText() const
		{
			SVString equationText("");
			ASSERT( m_pEquationStruct );
			if( m_pEquationStruct != nullptr )
			{
				m_pEquationStruct->GetEquationText( equationText );
			}
			return equationText;
		}

		const std::vector<SVString> FormulaController::getPPQVariableNames() const
		{
			vector<SVString> retVals;
			if (m_pInspection != nullptr)
			{
				for( size_t i = 0; i < m_pInspection->m_PPQInputs.size(); i++ )
				{	
					SVIOEntryHostStructPtr ioEntryPtr = m_pInspection->m_PPQInputs[i].m_IOEntryPtr;
					
					//check if input is enable for this inspection
					if( ioEntryPtr->m_Enabled )
					{
						SVString name = ioEntryPtr->m_pValueObject->GetName();
						retVals.push_back( name );
					}					
				}// end for
			}
			return retVals;
		}

		SVToolSetClass* FormulaController::getToolSet() const
		{
			return m_pToolSet;
		}

		HRESULT FormulaController::isToolAndEquationEnabled(bool& toolEnabled, bool& equationEnabled) const
		{
			if (m_pToolSet != nullptr && m_pEquation != nullptr)
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

		void FormulaController::setTaskObject( SVTaskObjectClass& pObject )
		{
			m_pInspection = pObject.GetInspection();
			m_pToolSet = m_pInspection->GetToolSet();
				
			SVEquationClass* pEquation = dynamic_cast <SVEquationClass*> (&pObject);
			if ( pEquation == nullptr )
			{
				SVObjectTypeInfoStruct info;
				info.ObjectType = SVEquationObjectType;
				info.SubType    = SVMathEquationObjectType;	// we are not looking for conditional equation !!!
				pEquation = dynamic_cast < SVEquationClass* >
					( reinterpret_cast < SVObjectClass* > (::SVSendMessage( &pObject, SVM_GETFIRST_OBJECT | SVM_NOTIFY_ONLY_FRIENDS, 0, (DWORD) &info ) ));
			}
			// Set the pointer to the Equation Class Object 
			setEquation( pEquation );
		}

		int FormulaController::validateEquation( const SVString equationString, double& result ) const
		{
			int retValue = validateSuccessful;
			SVString oldString("");
			//save old string
			m_pEquationStruct->GetEquationText(oldString);
			m_pEquationStruct->SetEquationText(equationString);
			
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
			//reset old string
			m_pEquationStruct->SetEquationText(oldString);
			return retValue;
		}

		int FormulaController::validateAndSetEquation( const SVString equationString, double& result )
		{
			int retValue = validateEquation( equationString, result);

			if (retValue == validateSuccessful)
			{
				//set new equation text and reset all objects for using the new value
				m_pEquationStruct->SetEquationText(equationString);
				SVObjectClass* object = dynamic_cast<SVObjectClass*>(m_pEquation->GetTool());
				if( object != nullptr )
				{
					DWORD l_dwRet = ::SVSendMessage( object, SVM_RESET_ALL_OBJECTS & ~SVM_NOTIFY_FRIENDS , 0, 0); // Do not reset friends because they may be invalid.
					if( l_dwRet != SVMR_SUCCESS)
					{
						return setFailed;
					}
				}
			}
			return retValue;
		}
#pragma endregion
#pragma endregion

#pragma region Proteced Methods
		void FormulaController::setEquation( SVEquationClass* pEquation )
		{
			m_pEquation = pEquation;
			if( m_pEquation != nullptr )
			{
				m_pEquationStruct = m_pEquation->GetEquationStruct();
			}
			else
			{
				m_pEquationStruct = nullptr;
			}
		}
#pragma endregion

#pragma region Private Methods

#pragma endregion
	} //namespace SVObserver
} //namespace Seidenader

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\FormulaController.cpp_v  $
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
