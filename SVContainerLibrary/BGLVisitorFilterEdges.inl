//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file BGLFilterSearch.inl
/// All Rights Reserved 
//*****************************************************************************
/// This is the template methods for the Boost Graph Library
/// Visitor (Depth first search & Breadth first search), 
/// filters and in out edges
//******************************************************************************

namespace Sv { namespace Container
{
	//! Edge style filter template class
	template< typename EdgeTypeMapConst, typename EdgeType >
	class filter_edge_type
	{
	public:
		filter_edge_type() : m_pEdgeTypeMap( nullptr ), m_pEdgeTypeFilter( nullptr ) {}
		//! Constructor method
		//! \param rEdgeTypeMapValue [in] reference to the edge map properties
		//! \param rEdgeFilterType [in] reference to the edge filter type
		filter_edge_type( EdgeTypeMapConst* pEdgeTypeMap, const EdgeType* pEdgeTypeFilter ) : m_pEdgeTypeMap( pEdgeTypeMap ), m_pEdgeTypeFilter( pEdgeTypeFilter ) {}

		//! Operator method
		//! \param rEdge [in] reference to the current edge
		template< typename Edge >
		bool operator()(const Edge& rEdge) const
		{
			bool Result( false );

			if( nullptr != m_pEdgeTypeMap && nullptr != m_pEdgeTypeFilter )
			{
				const EdgeType& rEdgeType = (*m_pEdgeTypeMap)[rEdge];
				Result = rEdgeType.HasMember( *m_pEdgeTypeFilter );
			}
			return Result;
		}

	private:
		EdgeTypeMapConst* m_pEdgeTypeMap;
		const EdgeType* m_pEdgeTypeFilter;
	};

	//! Dependency visitor template class
	template< typename VertexName, typename InsertIter, typename baseVisitor >
	class dependency_visitor : public baseVisitor 
	{
	public:
		//! Constructor method
		//! \param rSrc [in] reference to the source vertex name
		//! \param Inserter [in] inserter iterator for dependency list
		dependency_visitor(const VertexName& rSrc, InsertIter Inserter  ) : m_rSrc( rSrc ), m_Inserter( Inserter ) {}

		//! Method to check and add matching outer edges
		//! \param rSrcVertex [in] reference to the source vertex to check
		//! \param rSrcName [in] reference to the 
		//! \param rGraph [in] reference to the graph
		template< typename Vertex, typename Graph >
		void addMatchingOuterEdges( const Vertex& rSrcVertex, const VertexName& rSrcName, const Graph& rGraph )
		{
			//This is the edge vector of the given graph (important due to the use of filtered and reverse graphs)
			typedef std::vector<boost::graph_traits<Graph>::edge_descriptor> EdgeVector;
			EdgeVector EdgesFound;
			outedge_connection( rGraph, rSrcVertex, std::back_inserter( EdgesFound ) );

			EdgeVector::const_iterator it;
			for( it = EdgesFound.begin(); it != EdgesFound.end(); ++it )
			{
				Vertex PartnerVertex( boost::target( *it, rGraph ) );
				const VertexName& rPartnerName = boost::get( boost::get( boost::vertex_name, rGraph ), PartnerVertex );
				if( rSrcName != rPartnerName )
				{
					typename typedef std::pair< VertexName, VertexName > Dependency;

					m_DestinationSet.insert( rPartnerName );
					Dependency Dependent( rSrcName, rPartnerName );
					*m_Inserter = Dependent;
				}
			}
		}

		//! Discover vertex method used by the boost graph library
		//! \param DiscoveredVertex [in] the current discovered vertex
		//! \param rGraph [in] reference to the graph
		template< typename Vertex, typename Graph >
		void discover_vertex( Vertex DiscoveredVertex, const Graph& rGraph )
		{
			const VertexName& rDiscoveredName = boost::get( boost::get(boost::vertex_name, rGraph ), DiscoveredVertex );

			if( m_rSrc == rDiscoveredName )
			{
				m_DestinationSet.insert( rDiscoveredName );
			}

			std::set<VertexName>::const_iterator Iter( m_DestinationSet.find( rDiscoveredName ) );

			if( m_DestinationSet.cend() != Iter )
			{
				addMatchingOuterEdges( DiscoveredVertex, rDiscoveredName, rGraph );
			}
		}

	private:
		const VertexName& m_rSrc;
		std::set<VertexName> m_DestinationSet;
		InsertIter m_Inserter;
	};

	//! Cycle detector template class
	class cycle_detector : public boost::dfs_visitor<>
	{
	public:
		//! Constructor method
		//! \param rHasCycle [in] reference to the has cycle flag
		cycle_detector( bool& rHasCycle ) : m_rHasCycle(rHasCycle) { }

		//! Back edge method used by the boost graph library
		//! \param DiscoveredVertex [in] the current discovered vertex
		//! \param CurrentEdge [in] the current edge
		//! \param rGraph [in] reference to the graph
		template < typename Edge, typename Graph >
		void back_edge( Edge CurrentEdge , Graph& rGraph ) { m_rHasCycle = true; }

	private:
		bool& m_rHasCycle;
	};

	//! In edge connection template method
	//! \param rGraph [in] reference to the graph
	//! \param rVertex [in] reference to the vertex
	//! \param Inserter [in] edge inserter iterator
	template < typename Graph, typename Vertex, typename InsertIter >
	void inedge_connection(const Graph& rGraph, const Vertex& rVertex, InsertIter Inserter)
	{
		typename boost::graph_traits<Graph>::in_edge_iterator inEdgeIt, inEdgeIt_end;
		for( boost::tie(inEdgeIt, inEdgeIt_end) = boost::in_edges(rVertex, rGraph); inEdgeIt != inEdgeIt_end; ++inEdgeIt )
		{
			*Inserter++ = *inEdgeIt;
		}
	}

	//! Out edge connection template method
	//! \param rGraph [in] reference to the graph
	//! \param rVertex [in] reference to the vertex
	//! \param Inserter [in] edge inserter iterator
	template < typename Graph, typename Vertex, typename InsertIter >
	void outedge_connection(const Graph& rGraph, const Vertex& rVertex, InsertIter Inserter)
	{
		typename boost::graph_traits<Graph>::out_edge_iterator outEdgeIt, outEdgeIt_end;
		for( boost::tie(outEdgeIt, outEdgeIt_end) = boost::out_edges(rVertex, rGraph); outEdgeIt != outEdgeIt_end; ++outEdgeIt )
		{
			*Inserter++ = *outEdgeIt;
		}
	}
} /* namespace SVContainerLibrary */ } /* namespace Seidenader */

