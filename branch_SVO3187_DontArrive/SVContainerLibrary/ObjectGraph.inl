//*****************************************************************************
/// \copyright (c) 2016,2016 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file ObjectGraph.inl
/// All Rights Reserved 
//*****************************************************************************
/// This is the object graph template for dependencies
//******************************************************************************

#pragma warning(push)
#pragma warning(disable : 4701 4703)

namespace SvCl
{
	#pragma region Constructor
	template< typename VertexName, typename EdgeType >
	ObjectGraph<VertexName, EdgeType>::ObjectGraph()
	{
	}

	template< typename VertexName, typename EdgeType >
	ObjectGraph<VertexName, EdgeType>::ObjectGraph( const ObjectGraph& rObjectGraphManager ) :
	m_Graph( rObjectGraphManager.getGraph() )
	{
	}

	template< typename VertexName, typename EdgeType >
	ObjectGraph<VertexName, EdgeType>::ObjectGraph( const DependencySet& rList, const EdgeType& rEdgeType )
	{
		typename DependencySet::const_iterator it( rList.cbegin() );
		for(; rList.cend() != it; ++it )
		{
			Add( it->first );
			Add( it->second );
			Connect( it->first, it->second, rEdgeType );
		}
	}

	template< typename VertexName, typename EdgeType >
	ObjectGraph<VertexName, EdgeType>::~ObjectGraph()
	{
	}
	#pragma endregion Constructor

	#pragma region Public Methods
	template< typename VertexName, typename EdgeType >
	void ObjectGraph<VertexName, EdgeType>::Clear()
	{
		m_VertexNameDataMap.clear();
		m_Graph.clear();
	}

	template< typename VertexName, typename EdgeType >
	HRESULT ObjectGraph<VertexName, EdgeType>::Add( const VertexName& rVertexName )
	{
		HRESULT Result( S_OK );

		VertexData v = getVertex( rVertexName );
		if( nullptr == v )
		{
			size_t index = m_Graph.m_vertices.size();
			v = boost::add_vertex( VertexProperty( boost::white_color, VertexIndex( index, rVertexName ) ), m_Graph );

			m_VertexNameDataMap.insert( std::make_pair(rVertexName, v) );
			Result = S_OK;
		}
		return Result;
	}

	template< typename VertexName, typename EdgeType >
	HRESULT ObjectGraph<VertexName, EdgeType>::Remove( const VertexName& rVertexName )
	{
		HRESULT Result( E_FAIL );

		typename VertexNameDataMap::iterator it = m_VertexNameDataMap.find( rVertexName );
		if( it != m_VertexNameDataMap.end() )
		{
			removeVertex( it->second );

			m_VertexNameDataMap.erase( it );
			Result = S_OK;
		}
		return Result;
	}

	template< typename VertexName, typename EdgeType >
	HRESULT ObjectGraph<VertexName, EdgeType>::Connect( const VertexName& rSrcName, const VertexName& rDestName, const EdgeType& rEdgeType )
	{
		HRESULT Result( E_FAIL );

		//Only make connections if vertex are not the same
		if( rSrcName != rDestName )
		{

			VertexData v1 = getVertex( rSrcName );
			VertexData v2 = getVertex( rDestName );

			if( nullptr !=v1 && nullptr != v2 )
			{
				EdgeData ConnectedEdge;

				if( getEdge( v1, v2, ConnectedEdge ) )
				{
					updateEdgeType( ConnectedEdge, rEdgeType, false );
				}
				else
				{
					boost::add_edge( v1, v2, rEdgeType, m_Graph );
				}
				Result = S_OK;
			}
		}
		return Result;
	}

	template< typename VertexName, typename EdgeType >
	HRESULT ObjectGraph<VertexName, EdgeType>::Disconnect( const VertexName& rSrcName, const VertexName& rDestName, const EdgeType& rEdgeType )
	{
		HRESULT Result( E_FAIL );

		VertexData v1 = getVertex( rSrcName );
		VertexData v2 = getVertex( rDestName );

		if( nullptr !=v1 && nullptr != v2 )
		{
			EdgeData ConnectedEdge;
			if( getEdge( v1, v2, ConnectedEdge ) )
			{
				updateEdgeType( ConnectedEdge, rEdgeType, true );
			}
			else
			{
				boost::remove_edge( v1, v2, m_Graph );
			}
			Result = S_OK;
		}

		return Result;
	}

	template< typename VertexName, typename EdgeType >
	HRESULT ObjectGraph<VertexName, EdgeType>::getDependents(const VertexSet& rVertexSet, DependencyInserter inserter, const EdgeType& rEdgeType, bool invertEdge) const
	{
		EdgeTypeMapConst ConstEdgeTypeMap( boost::get( edge_type_t(), m_Graph ) );
		filter_edge_type<EdgeTypeMapConst, EdgeType> FilterEdgeType( &ConstEdgeTypeMap, &rEdgeType );
		const EdgeTypeFilteredGraph filteredGraph( m_Graph, FilterEdgeType );

		getAdjacentDependency(filteredGraph, inserter, rVertexSet, invertEdge);

		return S_OK;
	}

	template< typename VertexName, typename EdgeType >
	ObjectGraph<VertexName, EdgeType>::VertexSet ObjectGraph<VertexName, EdgeType>::getAllDependents( const VertexSet& rVertexSet, const EdgeType& rEdgeType) const
	{
		VertexSet result;

		EdgeTypeMapConst ConstEdgeTypeMap(boost::get(edge_type_t(), m_Graph));
		filter_edge_type<EdgeTypeMapConst, EdgeType> FilterEdgeType(&ConstEdgeTypeMap, &rEdgeType);
		const EdgeTypeFilteredGraph filteredGraph(m_Graph, FilterEdgeType);

		DependencySet ChildDependencies;
		depthFirstSearch(filteredGraph, std::inserter(ChildDependencies, ChildDependencies.end()), rVertexSet);

		//Insert the main Vertex in the dependency check and all dependencies found
		result.insert(rVertexSet.begin(), rVertexSet.end());
		typename DependencySet::const_iterator Iter( ChildDependencies.begin() );
		for( ; ChildDependencies.end() != Iter; ++Iter )
		{
			result.insert( Iter->first );
			result.insert( Iter->second );
		}
		return result;
	}

	template< typename VertexName, typename EdgeType >
	bool ObjectGraph<VertexName, EdgeType>::isCyclic()
	{
		bool Result( false );

		cycle_detector Visitor( Result );
		boost::depth_first_search( m_Graph, boost::visitor( Visitor ));

		return Result;
	}

	template< typename VertexName, typename EdgeType >
	bool ObjectGraph<VertexName, EdgeType>::saveGraphDot( LPCTSTR FileName, IGraphNameLookup& rNameLookup )
	{
		bool bRetVal = true;
		VertexNameFunc NameLookupFunc = [&rNameLookup](const VertexName& rName) {return rNameLookup.getVertexName(rName); };
		m_DotGraph = OutputGraphviz<VertexName, EdgeType, DependencyGraph, VertexNameFunc> ( FileName, m_Graph, NameLookupFunc );
		return bRetVal;
	}

	template< typename VertexName, typename EdgeType >
	void ObjectGraph<VertexName, EdgeType>::updateVertexIndex( )
	{
		typename boost::property_map<DependencyGraph, boost::vertex_index_t>::type my_vertex_index_map=get(boost::vertex_index, m_Graph);
		std::size_t current_index=0;
		typename boost::graph_traits<DependencyGraph>::vertex_iterator b, e;
		for (boost::tie(b, e) = boost::vertices(m_Graph); b != e; ++b)
		{
			put(my_vertex_index_map, *b, current_index++);
		}
	}

	template< typename VertexName, typename EdgeType >
	typename ObjectGraph<VertexName, EdgeType>::VertexData ObjectGraph<VertexName, EdgeType>::getVertex( const VertexName& rVertexName )
	{
		VertexData Result( nullptr );

		typename VertexNameDataMap::iterator it = m_VertexNameDataMap.find( rVertexName );
		if( it != m_VertexNameDataMap.end() )
		{
			Result = it->second;
		}

		return Result;
	}

	template< typename VertexName, typename EdgeType >
	typename const ObjectGraph<VertexName, EdgeType>::VertexData ObjectGraph<VertexName, EdgeType>::getVertex( const VertexName& rVertexName ) const
	{
		VertexData Result( nullptr );

		typename VertexNameDataMap::const_iterator it = m_VertexNameDataMap.find( rVertexName );
		if( it != m_VertexNameDataMap.end() )
		{
			Result = it->second;
		}

		return Result;
	}

	template< typename VertexName, typename EdgeType >
	void ObjectGraph<VertexName, EdgeType>::removeVertex(VertexData v)
	{
		//Find all edges of the vertex to remove these before removing the vertex itself
		EdgeVector DisconnectVector;

		inedge_connection( m_Graph, v, std::back_inserter( DisconnectVector ) );
		outedge_connection( m_Graph, v, std::back_inserter( DisconnectVector ) );

		typename EdgeVector::const_iterator it;
		for( it = DisconnectVector.begin(); it != DisconnectVector.end(); ++it )
		{
			VertexData v1 = boost::source( *it, m_Graph );
			VertexData v2 = boost::target( *it, m_Graph );

			boost::remove_edge( v1, v2, m_Graph );
		}

		boost::clear_vertex( v, m_Graph );
		boost::remove_vertex( v, m_Graph );
	}

	template< typename VertexName, typename EdgeType >
	bool ObjectGraph<VertexName, EdgeType>::getEdge( const VertexData& rV1, const VertexData& rV2, EdgeData& rEdge ) const
	{
		bool Result( false );

		typename DependencyGraph::out_edge_iterator EdgeIt, EdgeIt_end;
		for( boost::tie(EdgeIt, EdgeIt_end) = boost::out_edges( rV1, m_Graph ); EdgeIt != EdgeIt_end; ++EdgeIt )
		{
			const VertexData rTargetV = boost::target( *EdgeIt, m_Graph );

			if( rTargetV == rV2 )
			{
				rEdge = *EdgeIt;
				Result = true;
				break;
			}
		}
		return Result;
	}

	template< typename VertexName, typename EdgeType >
	void ObjectGraph<VertexName, EdgeType>::updateEdgeType( const EdgeData& rEdge, const EdgeType& rEdgeType, bool RemoveType )
	{
		if( RemoveType )
		{
			boost::get( edge_type_t(), m_Graph)[rEdge] -= rEdgeType;
		}
		else
		{
			boost::get( edge_type_t(), m_Graph)[rEdge] += rEdgeType;
		}
	}

	template< typename VertexName, typename EdgeType >
	void ObjectGraph<VertexName, EdgeType>::depthFirstSearch(const EdgeTypeFilteredGraph& rGraph, DependencyInserter inserter, const VertexSet& rVertexSet) const
	{
		typedef dependency_visitor<VertexName, DependencyInserter, boost::dfs_visitor<>> DfsVisitor;

		for (const auto rVertexName : rVertexSet)
		{
			const VertexData startVertex = getVertex(rVertexName);
			boost::depth_first_search(rGraph, boost::visitor(DfsVisitor(rVertexName, inserter)).root_vertex(startVertex));
		}
	}

	template< typename VertexName, typename EdgeType >
	void ObjectGraph<VertexName, EdgeType>::breadthFirstSearch(const EdgeTypeFilteredGraph& rGraph, DependencyInserter inserter, const VertexSet& rVertexSet) const
	{
		typedef dependency_visitor<VertexName, DependencyInserter, boost::bfs_visitor<>> BfsVisitor;
		for (const auto rVertexName : rVertexSet)
		{
			const VertexData startVertex = getVertex(rVertexName);
			boost::breadth_first_search(rGraph, startVertex, boost::visitor(BfsVisitor(rVertexName, inserter)));
		}
	}

	template< typename VertexName, typename EdgeType >
	void ObjectGraph<VertexName, EdgeType>::getAdjacentDependency(const EdgeTypeFilteredGraph& rGraph, DependencyInserter inserter, const VertexSet& rVertexSet, bool invertEdge) const
	{
		for (const VertexName& rVertexName : rVertexSet)
		{
			const VertexData startVertex = getVertex(rVertexName);
			EdgeVector edgesFound;
			invertEdge ? inedge_connection(rGraph, startVertex, std::back_inserter(edgesFound)) : outedge_connection(rGraph, startVertex, std::back_inserter(edgesFound));
			for (const auto& rEdge : edgesFound)
			{
				VertexData PartnerVertex = invertEdge ? boost::source(rEdge, rGraph) : boost::target(rEdge, rGraph);
				const VertexName& rPartnerName = boost::get(boost::get(boost::vertex_name, rGraph), PartnerVertex);
				if (rVertexName != rPartnerName)
				{
					*inserter = {rVertexName, rPartnerName};
				}
			}
		}
	}
} //namespace SvCl

#pragma warning(pop)
