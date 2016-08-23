//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .File Name       : $Workfile:   TriggerBasics.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:58:36  $
//******************************************************************************

#pragma once

namespace Seidenader { namespace TriggerHandling {

	//! This struct was introduced to facilitate the passing of further parameters 
	//! (beyond 'Owner' and 'Data') - in particular in preparation for the SVPLCIO.dll.

	struct TriggerParameters
	{
		void* m_pOwner;
		void* m_pData;
		TriggerParameters(void* pOwner=nullptr, void* pData=nullptr): m_pOwner(pOwner), m_pData(pData){}
	};

	typedef HRESULT ( CALLBACK *SVTriggerCallbackPtr )(TriggerParameters triggerparams);

	//! this adds a callback function pointer and further informational variables
	//! (currently just one) to the information contained in TriggerParameters
	//! Note: assignment and comparison operators ('=' and '=='), which hat been implemented previously,
	//! were removed because the defaults for both TriggerDispatcher and m_TriggerParameters seemed 
	//! better (and safer)
	struct TriggerDispatcher
	{
		TriggerDispatcher(const SVTriggerCallbackPtr callback, const TriggerParameters &tp);
		TriggerDispatcher( const TriggerDispatcher &p_rOriginal );

		bool operator==(const TriggerDispatcher &p_rOriginal ) const;

		virtual ~TriggerDispatcher(){}

		void clear();
		void DispatchIfPossible() const ;
		void Dispatch() const;

		bool hasCallback() const {return (nullptr != m_pCallback);}

		bool m_IsStarted;

		const SVTriggerCallbackPtr getCallback() const {return m_pCallback;}
		const TriggerParameters &GetTriggerParameters() const  {return m_TriggerParameters;}

		void SetData(void *data){m_TriggerParameters.m_pData = data;}

	private:
		SVTriggerCallbackPtr m_pCallback;
		TriggerParameters m_TriggerParameters;
	};

} /* namespace TriggerHandling */ } /* namespace Seidenader */

namespace SvTh = Seidenader::TriggerHandling;

