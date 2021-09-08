//******************************************************************************
/// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
/// \Author: Steve Jones
//******************************************************************************

#pragma once

template<typename type>
class SVThreadSafeList
{
#pragma region Public
public:
	typedef std::vector<typename type> Container;
	typedef typename Container::size_type size_type;

	SVThreadSafeList() {}
	SVThreadSafeList(size_type InitSize) : m_list(InitSize) {}
	~SVThreadSafeList() {}

	inline bool empty() const 
	{ 
		std::lock_guard<std::mutex> lock(m_dataMutex);
		return m_list.empty(); 
	}
	inline size_t size() const 
	{ 
		std::lock_guard<std::mutex> lock(m_dataMutex);
		return m_list.size();
	}
	const type& operator[](size_type nIndex) const 
	{ 
		std::lock_guard<std::mutex> lock(m_dataMutex);
		if (nIndex >= 0 && nIndex < m_list.size())
		{
			return m_list[nIndex]; 
		}
		return m_emptytype;
	}
	
	/////////////////////////////////////////////////
	/// Adds an Element to the list
	/// \param newElement - item to be added
	/////////////////////////////////////////////////
	int Add(const type& newElement)
	{
		std::lock_guard<std::mutex> lock(m_dataMutex);
		m_list.push_back(newElement);
		return static_cast<int>(m_list.size() - 1);
	}

	/// Insert an Element to the list at a position
	/// \param position [in] The position of the add.
	/// \param newElement [in] item to be added.
	/// \returns int Position of the add.
	int Insert(size_t position, const type& newElement)
	{
		std::lock_guard<std::mutex> lock(m_dataMutex);
		if (0 <= position && m_list.size() > position)
		{
			m_list.insert(m_list.begin()+position, newElement);
			return static_cast<int>(position);
		}
		else
		{
			m_list.push_back(newElement);
			return static_cast<int>(m_list.size() - 1);
		}
	}

	/////////////////////////////////////////////////
	/// Removes an Element from the list, not concurrent safe
	/// \Param nIndex - index of Element to be removed
	/////////////////////////////////////////////////
	void RemoveAt(int nIndex)
	{
		std::lock_guard<std::mutex> lock(m_dataMutex);
		if (0 <= nIndex && m_list.size() > nIndex)
		{
			auto it = m_list.begin() + nIndex;
			m_list.erase(it);
		}
	}

	/////////////////////////////////////////////////
	/// Remove all Elements from the list, not concurrent safe
	/////////////////////////////////////////////////
	void RemoveAll()
	{
		std::lock_guard<std::mutex> lock(m_dataMutex);
		if (!m_list.empty())
		{
			m_list.clear();
		}
	}
#pragma endregion Public

#pragma region Protected
protected:
	mutable std::mutex m_dataMutex;
	Container m_list;
	type m_emptytype; // empty item to be returned if
#pragma endregion Protected

#pragma region Private
private:
	SVThreadSafeList(const SVThreadSafeList& rObject) = delete;// not copyable - do not implement
	const SVThreadSafeList& operator=(const SVThreadSafeList& rObject) = delete; // not assignable - do not implement
	const SVThreadSafeList& operator=(const Container& rObject) = delete; // not assignable - do not implement
	void resize(size_type Newsize) = delete; // not thread/concurrent safe - do not implement
	void clear() = delete; //not thread/concurrent safe - do not implement
#pragma endregion Private
};
