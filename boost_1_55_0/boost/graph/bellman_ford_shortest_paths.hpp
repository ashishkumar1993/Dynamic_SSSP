//
//=======================================================================
// Copyright 1997, 1998, 1999, 2000 University of Notre Dame.
// Authors: Andrew Lumsdaine, Lie-Quan Lee, Jeremy G. Siek
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
//

/*
  This file implements the function

  template <class EdgeListGraph, class Size, class P, class T, class R>
  bool bellman_ford_shortest_paths(EdgeListGraph& g, Size N, 
     const bgl_named_params<P, T, R>& params)
  
 */


#ifndef BOOST_GRAPH_BELLMAN_FORD_SHORTEST_PATHS_HPP
#define BOOST_GRAPH_BELLMAN_FORD_SHORTEST_PATHS_HPP

#include <boost/config.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/graph_concepts.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/relax.hpp>
#include <boost/graph/linear_tree.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/graph/named_function_params.hpp>
#include <boost/concept/assert.hpp>

namespace boost {

  template <class Visitor, class Graph>
  struct BellmanFordVisitorConcept {
    void constraints() {
      BOOST_CONCEPT_ASSERT(( CopyConstructibleConcept<Visitor> ));
      vis.examine_edge(e, g);
      vis.edge_relaxed(e, g);
      vis.edge_not_relaxed(e, g);
      vis.edge_minimized(e, g);
      vis.edge_not_minimized(e, g);
    }
    Visitor vis;
    Graph g;
    typename graph_traits<Graph>::edge_descriptor e;
  };

  template <class Visitors = null_visitor>
  class bellman_visitor {
  public:
    bellman_visitor() { }
    bellman_visitor(Visitors vis) : m_vis(vis) { }

    template <class Edge, class Graph>
    void examine_edge(Edge u, Graph& g) {
      invoke_visitors(m_vis, u, g, on_examine_edge());
    }
    template <class Edge, class Graph>
    void edge_relaxed(Edge u, Graph& g) {
      invoke_visitors(m_vis, u, g, on_edge_relaxed());      
    }
    template <class Edge, class Graph>
    void edge_not_relaxed(Edge u, Graph& g) {
      invoke_visitors(m_vis, u, g, on_edge_not_relaxed());
    }
    template <class Edge, class Graph>
    void edge_minimized(Edge u, Graph& g) {
      invoke_visitors(m_vis, u, g, on_edge_minimized());
    }
    template <class Edge, class Graph>
    void edge_not_minimized(Edge u, Graph& g) {
      invoke_visitors(m_vis, u, g, on_edge_not_minimized());
    }
  protected:
    Visitors m_vis;
  };
  template <class Visitors>
  bellman_visitor<Visitors>
  make_bellman_visitor(Visitors vis) {
    return bellman_visitor<Visitors>(vis);
  }
  typedef bellman_visitor<> default_bellman_visitor;

  template <class EdgeListGraph, class Size, class WeightMap,
            class PredecessorMap, class DistanceMap,
            class BinaryFunction, class BinaryPredicate,
            class BellmanFordVisitor>
  bool bellman_ford_shortest_paths(EdgeListGraph& g, Size N, 
                         WeightMap weight, 
                         PredecessorMap pred,
                         DistanceMap distance, 
                         BinaryFunction combine, 
                         BinaryPredicate compare,
                         BellmanFordVisitor v)
  {
    BOOST_CONCEPT_ASSERT(( EdgeListGraphConcept<EdgeListGraph> ));
    typedef graph_traits<EdgeListGraph> GTraits;
    typedef typename GTraits::edge_descriptor Edge;
    typedef typename GTraits::vertex_descriptor Vertex;
    BOOST_CONCEPT_ASSERT(( ReadWritePropertyMapConcept<DistanceMap, Vertex> ));
    BOOST_CONCEPT_ASSERT(( ReadablePropertyMapConcept<WeightMap, Edge> ));

    typename GTraits::edge_iterator i, end;

    for (Size k = 0; k < N; ++k) {
      bool at_least_one_edge_relaxed = false;
      for (boost::tie(i, end) = edges(g); i != end; ++i) {
        v.examine_edge(*i, g);
        if (relax(*i, g, weight, pred, distance, combine, compare)) {
          at_least_one_edge_relaxed = true;
          v.edge_relaxed(*i, g);
        } else
          v.edge_not_relaxed(*i, g);
      }
      if (!at_least_one_edge_relaxed)
        break;
    }

    for (boost::tie(i, end) = edges(g); i != end; ++i)
      if (compare(combine(get(distance, source(*i, g)), get(weight, *i)),
                  get(distance, target(*i,g))))
      {
        v.edge_not_minimized(*i, g);
        return false;
      } else
        v.edge_minimized(*i, g);

    return true;
  }

  template <class EdgeListGraph, class Size, class WeightMap,
            class PredecessorMap, class DistanceMap,
            class BinaryFunction, class BinaryPredicate,
            class BellmanFordVisitor>
  bool dynamic_bellman_ford_shortest_paths(EdgeListGraph& g, Size N, 
                         WeightMap weight, 
                         PredecessorMap pred,
                         DistanceMap distance, 
                         BinaryFunction combine, 
                         BinaryPredicate compare,
                         BellmanFordVisitor v,
			 typename graph_traits<EdgeListGraph>::vertex_descriptor *affected_vertices, int affected_number, class linear_tree *SP_tree)
  { 
	int aff_set[100000]={0};
	for(int it=0; it< affected_number ; it++)
	{
		aff_set[affected_vertices[it]]=1;
	}
    BOOST_CONCEPT_ASSERT(( EdgeListGraphConcept<EdgeListGraph> ));
    typedef graph_traits<EdgeListGraph> GTraits;
    typedef typename GTraits::edge_descriptor Edge;
    typedef typename GTraits::vertex_descriptor Vertex;
    BOOST_CONCEPT_ASSERT(( ReadWritePropertyMapConcept<DistanceMap, Vertex> ));
    BOOST_CONCEPT_ASSERT(( ReadablePropertyMapConcept<WeightMap, Edge> ));

    typedef typename GTraits::vertex_iterator vtx;
    typedef typename GTraits::out_edge_iterator in_edg;

    in_edg i;
    std::pair<in_edg,in_edg> e_it;
    std::pair< vtx, vtx > vtx_itr;
    vtx_itr=vertices(g);

    for (Size k = 0; k < N; ++k) { 
      bool at_least_one_edge_relaxed = false;
	for(int trav_vert=0; trav_vert<affected_number; ++trav_vert)
	{
			Vertex v_i=affected_vertices[trav_vert];
			for (e_it=out_edges(v_i,g); e_it.first != e_it.second; ++e_it.first)
			{
				i=e_it.first;
				v.examine_edge(*i, g);
				if (relax(*i, g, weight, pred, distance, combine, compare)) 
				{
					  at_least_one_edge_relaxed = true;				  	  
					  v.edge_relaxed(*i, g);
					  if(aff_set[target(*i,g)]==0)
					  {
							affected_vertices[affected_number++]=target(*i,g);
							aff_set[target(*i,g)]=1;
					  }
				} 
				else
					  v.edge_not_relaxed(*i, g);
			}
    }

      if (!at_least_one_edge_relaxed)
	        break;
  }

/*    for (boost::tie(i, end) = edges(g); i != end; ++i)
      if (compare(combine(get(distance, source(*i, g)), get(weight, *i)),
                  get(distance, target(*i,g))))
      {
        v.edge_not_minimized(*i, g);
        return false;
      } else
        v.edge_minimized(*i, g);*/

    return true;
  }


  namespace detail {

    template<typename VertexAndEdgeListGraph, typename Size, 
             typename WeightMap, typename PredecessorMap, typename DistanceMap,
             typename P, typename T, typename R>
    bool 
    bellman_dispatch2
      (VertexAndEdgeListGraph& g, 
       typename graph_traits<VertexAndEdgeListGraph>::vertex_descriptor s,
       Size N, WeightMap weight, PredecessorMap pred, DistanceMap distance,
       const bgl_named_params<P, T, R>& params)
    {
      typedef typename property_traits<DistanceMap>::value_type D;
      bellman_visitor<> null_vis;
      typedef typename property_traits<WeightMap>::value_type weight_type;
      typename graph_traits<VertexAndEdgeListGraph>::vertex_iterator v, v_end;
      for (boost::tie(v, v_end) = vertices(g); v != v_end; ++v) {
        put(distance, *v, (std::numeric_limits<weight_type>::max)());
        put(pred, *v, *v);
      }
      put(distance, s, weight_type(0));
      return bellman_ford_shortest_paths
               (g, N, weight, pred, distance,
                choose_param(get_param(params, distance_combine_t()),
                             closed_plus<D>()),
                choose_param(get_param(params, distance_compare_t()),
                             std::less<D>()),
                choose_param(get_param(params, graph_visitor),
                             null_vis)
                );
    }

    template<typename VertexAndEdgeListGraph, typename Size, 
             typename WeightMap, typename PredecessorMap, typename DistanceMap,
             typename P, typename T, typename R>
    bool 
    bellman_dispatch2
      (VertexAndEdgeListGraph& g, 
       param_not_found,
       Size N, WeightMap weight, PredecessorMap pred, DistanceMap distance,
       const bgl_named_params<P, T, R>& params)
    {
      typedef typename property_traits<DistanceMap>::value_type D;
      bellman_visitor<> null_vis;
      return bellman_ford_shortest_paths
               (g, N, weight, pred, distance,
                choose_param(get_param(params, distance_combine_t()),
                             closed_plus<D>()),
                choose_param(get_param(params, distance_compare_t()),
                             std::less<D>()),
                choose_param(get_param(params, graph_visitor),
                             null_vis)
                );
    }

    template <class EdgeListGraph, class Size, class WeightMap,
              class DistanceMap, class P, class T, class R>
    bool bellman_dispatch(EdgeListGraph& g, Size N, 
                          WeightMap weight, DistanceMap distance, 
                          const bgl_named_params<P, T, R>& params)
    {
      dummy_property_map dummy_pred;
      return 
        detail::bellman_dispatch2
          (g, 
           get_param(params, root_vertex_t()),
           N, weight,
           choose_param(get_param(params, vertex_predecessor), dummy_pred),
           distance,
           params);
    }

    template<typename VertexAndEdgeListGraph, typename Size, 
             typename WeightMap, typename PredecessorMap, typename DistanceMap,
             typename P, typename T, typename R>
    bool 
    dynamic_bellman_dispatch2
      (VertexAndEdgeListGraph& g, 
       typename graph_traits<VertexAndEdgeListGraph>::vertex_descriptor s,
       Size N, WeightMap weight, PredecessorMap pred, DistanceMap distance,
       const bgl_named_params<P, T, R>& params, typename graph_traits<VertexAndEdgeListGraph>::vertex_descriptor *affected_vertices, 
	   int affected_number,class linear_tree *SP_tree)
    {
      typedef typename property_traits<DistanceMap>::value_type D;
      bellman_visitor<> null_vis;
      typedef typename property_traits<WeightMap>::value_type weight_type;
      typename graph_traits<VertexAndEdgeListGraph>::vertex_iterator v, v_end;
      for (boost::tie(v, v_end) = vertices(g); v != v_end; ++v) {
        put(distance, *v, (std::numeric_limits<weight_type>::max)());
        put(pred, *v, *v);
      }
      put(distance, s, weight_type(0));
      return dynamic_bellman_ford_shortest_paths
               (g, N, weight, pred, distance,
                choose_param(get_param(params, distance_combine_t()),
                             closed_plus<D>()),
                choose_param(get_param(params, distance_compare_t()),
                             std::less<D>()),
                choose_param(get_param(params, graph_visitor),
                             null_vis)
                , *affected_vertices, affected_number, SP_tree);
    }

    template<typename VertexAndEdgeListGraph, typename Size, 
             typename WeightMap, typename PredecessorMap, typename DistanceMap,
             typename P, typename T, typename R>
    bool 
    dynamic_bellman_dispatch2
      (VertexAndEdgeListGraph& g, 
       param_not_found,
       Size N, WeightMap weight, PredecessorMap pred, DistanceMap distance,
       const bgl_named_params<P, T, R>& params, typename graph_traits<VertexAndEdgeListGraph>::vertex_descriptor *affected_vertices,
	   int affected_number, class linear_tree *SP_tree)
    {
      typedef typename property_traits<DistanceMap>::value_type D;
      bellman_visitor<> null_vis;
      return dynamic_bellman_ford_shortest_paths
               (g, N, weight, pred, distance,
                choose_param(get_param(params, distance_combine_t()),
                             closed_plus<D>()),
                choose_param(get_param(params, distance_compare_t()),
                             std::less<D>()),
                choose_param(get_param(params, graph_visitor),
                             null_vis)
                , affected_vertices, affected_number, SP_tree);
    }

    template <class EdgeListGraph, class Size, class WeightMap,
              class DistanceMap, class P, class T, class R>
    bool dynamic_bellman_dispatch(EdgeListGraph& g, Size N, 
                          WeightMap weight, DistanceMap distance, 
                          const bgl_named_params<P, T, R>& params, typename graph_traits<EdgeListGraph>::vertex_descriptor *affected_vertices,
						  int affected_number, class linear_tree *SP_tree)
    {
      dummy_property_map dummy_pred;
      return 
        detail::dynamic_bellman_dispatch2
          (g, 
           get_param(params, root_vertex_t()),
           N, weight,
           choose_param(get_param(params, vertex_predecessor), dummy_pred),
           distance,
           params, affected_vertices, affected_number, SP_tree);
    }

  } // namespace detail

  template <class EdgeListGraph, class Size, class P, class T, class R>
  bool bellman_ford_shortest_paths
    (EdgeListGraph& g, Size N, 
     const bgl_named_params<P, T, R>& params)
  {                                
    return detail::bellman_dispatch
      (g, N,
       choose_const_pmap(get_param(params, edge_weight), g, edge_weight),
       choose_pmap(get_param(params, vertex_distance), g, vertex_distance),
       params);
  }

  template <class EdgeListGraph, class Size>
  bool bellman_ford_shortest_paths(EdgeListGraph& g, Size N)
  {                                
    bgl_named_params<int,int> params(0);
    return bellman_ford_shortest_paths(g, N, params);
  }

  template <class VertexAndEdgeListGraph, class P, class T, class R>
  bool bellman_ford_shortest_paths
    (VertexAndEdgeListGraph& g, 
     const bgl_named_params<P, T, R>& params)
  {               
    BOOST_CONCEPT_ASSERT(( VertexListGraphConcept<VertexAndEdgeListGraph> ));
    return detail::bellman_dispatch
      (g, num_vertices(g),
       choose_const_pmap(get_param(params, edge_weight), g, edge_weight),
       choose_pmap(get_param(params, vertex_distance), g, vertex_distance),
       params);
  }


  template <class EdgeListGraph, class Size, class P, class T, class R>
  bool dynamic_bellman_ford_shortest_paths
    (EdgeListGraph& g, Size N, 
     const bgl_named_params<P, T, R>& params, typename graph_traits<EdgeListGraph>::vertex_descriptor *affected_vertices,
	 int affected_number, class linear_tree *SP_tree)
  {                                

    return detail::dynamic_bellman_dispatch
      (g, N,
       choose_const_pmap(get_param(params, edge_weight), g, edge_weight),
       choose_pmap(get_param(params, vertex_distance), g, vertex_distance),
       params, affected_vertices, affected_number, SP_tree);
  }

  template <class EdgeListGraph, class Size>
  bool dynamic_bellman_ford_shortest_paths(EdgeListGraph& g, Size N, typename graph_traits<EdgeListGraph>::vertex_descriptor *affected_vertices, 
										   int affected_number, class linear_tree *SP_tree)
  {                                
    bgl_named_params<int,int> params(0);
    return dynamic_bellman_ford_shortest_paths(g, N, params, affected_vertices, affected_number, SP_tree);
  }

  template <class VertexAndEdgeListGraph, class P, class T, class R>
  bool dynamic_bellman_ford_shortest_paths
    (VertexAndEdgeListGraph& g, 
     const bgl_named_params<P, T, R>& params, typename graph_traits<VertexAndEdgeListGraph>::vertex_descriptor *affected_vertices,
	 int affected_number, class linear_tree *SP_tree)
  {               
    BOOST_CONCEPT_ASSERT(( VertexListGraphConcept<VertexAndEdgeListGraph> ));
    return detail::dynamic_bellman_dispatch
      (g, num_vertices(g),
       choose_const_pmap(get_param(params, edge_weight), g, edge_weight),
       choose_pmap(get_param(params, vertex_distance), g, vertex_distance),
       params, affected_vertices, affected_number, SP_tree);
  }
} // namespace boost

#endif // BOOST_GRAPH_BELLMAN_FORD_SHORTEST_PATHS_HPP
