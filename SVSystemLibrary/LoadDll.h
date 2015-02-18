//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : LoadDll
//* .File Name       : $Workfile:   LoadDll.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   18 Feb 2015 03:20:46  $
//******************************************************************************

#pragma once
#pragma region Includes
#include <map>
#include "SVUtilityLibrary\SVString.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace SVSystemLibrary
	{

		#pragma region Declarations
			//Loadable Dlls
			const TCHAR			SVOResourceDll[]	= _T("SVOResource.dll");
		#pragma endregion Declarations

		class LoadDll
		{
		public:
		#pragma region Declarations
			typedef std::map<SVString, HINSTANCE> DllMap;
		#pragma endregion Declarations

		public:
		#pragma region Constructor
			/**********
			 The class constructor
			***********/
			LoadDll();

			/**********
			 The class destructor
			***********/
			virtual ~LoadDll();
		#pragma endregion Constructor

		public:
		#pragma region Public Methods
			/**********
			 The method returns the instance to the class
			 \return the reference to the static object
			***********/
			static LoadDll& Instance();

			/**********
			 The method returns the Dll instance loads it if necessary
			 \return S_OK on success
			***********/
			HRESULT getDll( const SVString& DllName, HINSTANCE& Instance );

			/**********
			 The method free all loaded Dlls
			***********/
			void freeDlls();
		#pragma endregion Public Methods

		private:
		#pragma region Member Variables
				DllMap m_DllsLoaded;						//The list of Dlls loaded
		#pragma endregion Member Variables
		};
	}
}

namespace SvSyl = Seidenader::SVSystemLibrary;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSARCH65\PROJECTFILES\ARCHIVES\SVOBSERVER_SRC\SVSystemLibrary\LoadDll.h_v  $
 * 
 *    Rev 1.1   18 Feb 2015 03:20:46   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  984
 * SCR Title:  Exception Display Class with Exception Manager Template SVO-524
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Changed to use short namespace names
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Jul 2014 16:05:34   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
