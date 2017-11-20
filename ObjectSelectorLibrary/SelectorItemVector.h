//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// This is the data items to be used for the object selector
//******************************************************************************
#pragma once

#pragma region Includes
#include "ObjectInterfaces/ISelectorItemVector.h"
#include "SelectorItem.h"
#pragma endregion Includes

namespace SvOsl
{
	class SelectorItemVector : public SvOi::ISelectorItemVector
	{
	public:
		typedef std::vector<SelectorItem> SelectorItemStdVector;
		typedef SelectorItemStdVector::value_type value_type;
		typedef SelectorItemStdVector::iterator iterator;
		typedef SelectorItemStdVector::const_iterator const_iterator;
	#pragma region Constructor
	public:
		SelectorItemVector() {};

		virtual ~SelectorItemVector() {};
	#pragma endregion Constructor

	#pragma region Public Methods
	public:
		iterator begin() { return m_SelectorItemVector.begin(); };
		iterator end() { return m_SelectorItemVector.end(); };

		const_iterator begin() const { return m_SelectorItemVector.begin(); };
		const_iterator end() const { return m_SelectorItemVector.end(); };

		bool operator==(const SelectorItemVector& rRhs) const { return m_SelectorItemVector == rRhs.getSelectorItemVector(); };
		bool operator!=(const SelectorItemVector& rRhs) const { return m_SelectorItemVector != rRhs.getSelectorItemVector(); };

		const value_type operator[](size_t Index) const { return m_SelectorItemVector[Index]; };
		value_type operator[](size_t Index) { return m_SelectorItemVector[Index]; };

		iterator erase(const_iterator Iter) { return m_SelectorItemVector.erase(Iter); };
		void push_back(const value_type Item) { m_SelectorItemVector.push_back(Item); };

		size_t size() const { return m_SelectorItemVector.size(); };
		void clear() { m_SelectorItemVector.clear(); };

		void swap(SelectorItemVector& rVector) { m_SelectorItemVector.swap(rVector.getSelectorItemVector()); };
	#pragma endregion Public Methods

	#pragma region Private Methods
		private:
			SelectorItemStdVector& getSelectorItemVector() { return m_SelectorItemVector; };
			const SelectorItemStdVector& getSelectorItemVector() const { return m_SelectorItemVector; };
#pragma endregion Private Methods

	#pragma region Member Variables
	private:
		SelectorItemStdVector m_SelectorItemVector;
	#pragma endregion Member Variables
	};

} //namespace SvOsl
