//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file ObjectGraph.h
/// All Rights Reserved 
//*****************************************************************************
/// This is the object graph template for dependencies
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <fstream>
//Moved to precompiled header: #include <iostream>
//Moved to precompiled header: #include <iterator>
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <boost/bind.hpp>
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/function.hpp>
//Moved to precompiled header: #include <boost/graph/adjacency_list.hpp>
//Moved to precompiled header: #include <boost/graph/depth_first_search.hpp>
//Moved to precompiled header: #include <boost/graph/breadth_first_search.hpp>
//Moved to precompiled header: #include <boost/graph/reverse_graph.hpp>
//Moved to precompiled header: #include <boost/graph/filtered_graph.hpp>
//Moved to precompiled header: #include <boost/graph/graphviz.hpp>
//Moved to precompiled header: #include <boost/utility.hpp>
#include "IGraphNameLookup.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

namespace Sv { namespace Container
{
	#pragma region Declarations
	struct edge_type_t 
	{
		enum { num = 101 };
		typedef boost::edge_property_tag kind;
	};
	#pragma endregion Declarations

	template< typename VertexName, typename EdgeType >
	class ObjectGraph
	{
	#pragma region Declarations
	public:
		//! The graph dependency decelerations which are vertex pairs defining an edge
		typedef std::set< VertexName > Dependent;
		typedef std::pair< VertexName, VertexName > Dependency;
		typedef std::set< Dependency > Dependencies;
		typedef std::insert_iterator< Dependencies > DependencyInserter;

		//! The boost graph vertex and edge properties
		typedef typename boost::property< boost::vertex_name_t, VertexName > VertexObj;
		typedef boost::property< boost::vertex_index_t, size_t, VertexObj > VertexIndex;
		typedef boost::property< boost::vertex_color_t, boost::default_color_type, VertexIndex > VertexProperty;
		typedef boost::property< edge_type_t, EdgeType > EdgeProperty;

		typedef boost::function<LPCTSTR ( const VertexName& )> VertexNameFunc;

		//! The boost graph library deceleration using the adjacency type
		typedef boost::adjacency_list<boost::vecS,
			boost::listS,
			boost::bidirectionalS,
			VertexProperty,
			EdgeProperty>
			DependencyGraph;

		typename typedef boost::graph_traits<DependencyGraph>::vertex_descriptor VertexData;
		typename typedef boost::graph_traits<DependencyGraph>::edge_descriptor EdgeData;
		typename typedef boost::property_map<DependencyGraph, edge_type_t>::type EdgeTypeMap;
		typename typedef boost::property_map<DependencyGraph, edge_type_t>::const_type EdgeTypeMapConst;

		typename typedef std::vector<EdgeData> EdgeVector;
	
		typedef std::map<VertexName, VertexData> VertexNameDataMap;
	#pragma endregion Declarations

	#pragma region Constructor
	public:
		ObjectGraph();
		ObjectGraph( const ObjectGraph& rObjectGraphManager );
		ObjectGraph& operator=( const ObjectGraph& rObjectGraphManager );

		//! Graph constructor using dependency list and edge type
		//! \param rList [in] reference to the dependency list
		//! \param rEdgeType [in] reference to the dependency edge type
		ObjectGraph( const Dependencies& rList, const EdgeType& rEdgeType );
		virtual ~ObjectGraph();
	#pragma endregion Constructor

	#pragma region Public Methods
	public:
		//! Method to clear the current graph
		void Clear();

		//! Add a vertex to the graph
		//! \param rName [in] reference to the vertex name
		//! \returns S_OK on success
		HRESULT Add( const VertexName& rName );

		//! Remove a vertex from the graph and removes all the edges connected to the vertex
		//! \param rName [in] reference to the vertex name
		//! \returns S_OK on success
		HRESULT Remove( const VertexName& rName );

		//! Connect two vertex names with a certain edge type
		//! \param rSrcName [in] reference to the source vertex
		//! \param rDestName [in] reference to the destination vertex
		//! \param rEdgeType [in] reference to the edge type
		//! \returns S_OK on success
		HRESULT Connect( const VertexName& rSrcName, const VertexName& rDestName, const EdgeType& rEdgeType );

		//! Disconnect two vertex names with a certain edge type
		//! \param rSrcName [in] reference to the source vertex
		//! \param rDestName [in] reference to the destination vertex
		//! \param rEdgeType [in] reference to the edge type
		//! \returns S_OK on success
		HRESULT Disconnect( const VertexName& rSrcName, const VertexName& rDestName, const EdgeType& rEdgeType );

		//! Obtains a list of dependencies with the defined edge type
		//! \param rStartName [in] reference to the start vertex
		//! \param Inserter [in] inserter to fill the dependency list
		//! \param rEdgeType [in] reference to the edge type
		//! \param InvertEdge [in] flag to invert the edges of the graph
		//! \param DepthFirstSearch [in] flag to use Depth First Search instead of the Breadth First Search
		//! \returns S_OK on success
		HRESULT getDependents( const VertexName& rStartName, DependencyInserter Inserter, const EdgeType& rEdgeType, bool InvertEdge=false, bool DepthFirstSearch=false ) const;

		//! Obtains a list of dependencies with the defined edge type
		//! \param rStartName [in] reference to the start vertex
		//! \param Inserter [in] inserter to fill the dependency list
		//! \param rChildEdgeType [in] reference to the child edge type
		//! \param rEdgeType [in] reference to the edge type
		//! \param InvertEdge [in] flag to invert the edges of the graph
		//! \returns S_OK on success
		HRESULT getChildDependents( const VertexName& rStartName, DependencyInserter Inserter, const EdgeType& rChildEdgeType, const EdgeType& rEdgeType, bool InvertEdge=false ) const;

		//! Determines if the graph is cyclic
		//! \returns true if the graph is cyclic
		bool isCyclic();

		//! Returns the graph object
		//! \returns a constant reference to the graph
		const DependencyGraph& getGraph() const { return m_Graph; };

		//! Saves the graph dot file
		//! \param FileName [in] The name of the file to save
		//! \param nameLookup [in] Function used to get the vertex name
		//! \returns true on success
		bool saveGraphDot( LPCTSTR FileName, IGraphNameLookup& nameLookup );

		void updateVertexIndex();

		//! Get the graph dot string
		//! \returns a reference to the graph dot string
		const SVString& getDotGraph() const { return m_DotGraph; };
	#pragma endregion Public Methods
	
	#pragma region Private Methods
	private:
		//! Gets the vertex descriptor from the vertex name
		//! \param rName [in] reference to the vertex name
		//! \returns the vertex descriptor
		VertexData getVertex( const VertexName& rName );
		const VertexData getVertex( const VertexName& rName ) const;

		//! Removes the vertex from the graph
		//! \param v [in] vertex descriptor to remove
		void removeVertex( VertexData v );

		//! Gets the edge connecting two vertex descriptors
		//! \param rSrcVertex [in] reference to the source vertex 
		//! \param rDestVertex [in] reference to the destination vertex
		//! \param EdgeData [out] reference to place the edge descriptor data in
		//! \returns true on success
		bool getEdge( const VertexData& rSrcVertex, const VertexData& rDestVertex, EdgeData& rEdge ) const;

		//! Updates the edge type for a certain edge
		//! \param rEdge [in] reference to the edge to check
		//! \param rEdgeType [in] the edge type to update
		//! \param RemoveType [in] true when the edge type needs to be removed else add it
		//! \returns true on success
		void updateEdgeType( const EdgeData& rEdge, const EdgeType& rEdgeType, bool RemoveType );
	#pragma endregion Private Methods

	#pragma region Member Variables
	private:
		VertexNameDataMap m_VertexNameDataMap;
		DependencyGraph m_Graph;
		SVString m_DotGraph;
	#pragma endregion Member Variables
	};
} /* namespace SVContainerLibrary */ } /* namespace Seidenader */

#pragma region Inline
#include "GraphVizHelper.inl"
#include "BGLVisitorFilterEdges.inl"
#include "ObjectGraph.inl"
#pragma endregion Inline

namespace SvCl = Sv::Container;