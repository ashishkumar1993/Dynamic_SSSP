//=======================================================================
// Copyright 2001 Jeremy G. Siek, Andrew Lumsdaine, Lie-Quan Lee, 
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


//g++ -Iboost_1_55_0/ -Lboost_1_55_0/lib SSSP_Runner.cpp


//=======================================================================
#include <boost/config.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/bellman_ford_shortest_paths.hpp>
#include <cstdlib>
#include <stack>

using namespace boost;

template < typename Graph, typename ParentMap > 
struct edge_writer
{
  edge_writer(const Graph & g, const ParentMap & p)
  : m_g(g), m_parent(p)
  {
  }

  template < typename Edge >
    void operator() (std::ostream & out, const Edge & e) const
  {
    out << "[label=\"" << get(edge_weight, m_g, e) << "\"";
    typename graph_traits < Graph >::vertex_descriptor
      u = source(e, m_g), v = target(e, m_g);
    if (m_parent[v] == u)
        out << ", color=\"black\"";
    else
        out << ", color=\"grey\"";
      out << "]";
  }
  const Graph & m_g;
  ParentMap m_parent;
};
template < typename Graph, typename Parent >
edge_writer < Graph, Parent >
make_edge_writer(const Graph & g, const Parent & p)
{
  return edge_writer < Graph, Parent > (g, p);
}

struct EdgeProperties {
  int weight;
};

double fill_len(class linear_tree *SP_tree, int node_num, std::vector<int> adj[100000])
{
	double len=1;
	for(int i=0;i<adj[node_num].size();i++)
	{
		SP_tree->node_val[adj[node_num][i]]=fill_len(SP_tree,adj[node_num][i],adj);
		len+=SP_tree->node_val[adj[node_num][i]];
	}
	return len;
}

int
main()
{
  class linear_tree SP_tree;
  int adj_mat[101][101]={0};
  int count_wt=0;
  int N=10;
  typedef std::pair < int, int >E;
  typedef adjacency_list < vecS, vecS, bidirectionalS,
    no_property, EdgeProperties> Graph;
  typedef boost::graph_traits<Graph>::edge_descriptor edg_des;
  typedef boost::graph_traits<Graph>::vertex_descriptor vtx_des;
  typedef boost::graph_traits<Graph>::vertex_iterator vtx_iterator;
  std::pair<vtx_iterator,vtx_iterator> vtx_list;
  E edge_array[100000];
  #if defined(BOOST_MSVC) && BOOST_MSVC <= 1300
  // VC++ can't handle the iterator constructor
    Graph g(N);
  #else
    Graph g(edge_array, edge_array, N);
  #endif

  std::vector<int> dist_dyn(N, (std::numeric_limits < short >::max)());
  std::vector<std::size_t> parent_dyn(N);
  int weight[100000];
  int n_edges=20;
  vtx_des reverse_vertex_map[10000];
  for(vtx_list=vertices(g);vtx_list.first!=vtx_list.second;vtx_list.first++)
  {
  	reverse_vertex_map[*(vtx_list.first)]=*(vtx_list.first);
  }

  for (int i = 0; i < N; ++i)
  {
	  	SP_tree.node_pointer[i]=NULL;
  }

  for(int iter=0;iter<30;iter++)
  {
  	  std::cout<<"Iteration "<<iter<<std::endl;
	  int filled_edgs=0;
	  for (int i=0;i<n_edges;i++){
		int init=rand()%N,fin=rand()%N;
		if(adj_mat[init][fin]!=1){
		edge_array[filled_edgs++]=E(init,fin);
		adj_mat[init][fin]=1;
		}
		//std::cout << init << "-"<<fin<<"::";
		}
	  //std::cout<<std::endl;
	  for (int i=0;i < filled_edgs;i++){
		int wt=rand()%20;
		weight[count_wt++]=1+wt;
		//std::cout<<wt<<"::";
		}
	  //std::cout<<std::endl;


	  std::pair<edg_des,bool> ins_edg;
	  for (std::size_t j = 0; j < filled_edgs; ++j){
	    ins_edg=add_edge(edge_array[j].first, edge_array[j].second, g);
	    }
	  typedef boost::graph_traits< Graph >::vertex_iterator vtx;
	  std::pair< vtx, vtx > vtx_itr;
	  graph_traits < Graph >::edge_iterator ei, ei_end;
	  vtx_itr=vertices(g);
	  typedef boost::graph_traits<Graph>::in_edge_iterator in_edg;
	  std::pair<in_edg,in_edg> e_it;
	  e_it = in_edges(*vtx_itr.first,g);

	  property_map<Graph, int EdgeProperties::*>::type 
	    weight_pmap = get(&EdgeProperties::weight, g);

	  int i = 0;
	  for (boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei, ++i)
	    {
		weight_pmap[*ei] = weight[i];
		//std::cout<<source(*ei,g)<<"-"<<target(*ei,g)<<": "<<weight[i]<<std::endl;
	    }

 

	  n_edges=1;
	  vtx_des affected_vertices[100000];
	  int update=0;
	  vtx_des vt_des_sr=source(ins_edg.first,g);
	  vtx_des vt_des=target(ins_edg.first,g);
	  //affected_vertices[update++]=vt_des_sr;
	  //affected_vertices[update++]=vt_des;
	  typedef graph_traits<Graph>::out_edge_iterator out_iter;
	  typedef std::pair<out_iter,out_iter> out_edg_itr;
	  typedef property_map<Graph, vertex_index_t>::type tvertex_index_map;
	  tvertex_index_map indices = get(vertex_index, g);
	  int vertices_idx[100000]={0};
	  int int_vertex=indices[vt_des_sr];
	  vertices_idx[int_vertex]=1;
	  int current_vt=1;
/*	  while(current_vt<=update){
		for(out_edg_itr out_e=out_edges(vt_des,g);out_e.first!=out_e.second;out_e.first++)
		{
			if(vertices_idx[indices[target(*(out_e.first),g)]]!=1){
				affected_vertices[update++]=target(*(out_e.first),g);
				vertices_idx[indices[target(*(out_e.first),g)]]=1;
				//std::cout<<indices[target(*(out_e.first),g)]<<", ";
			}
		}
		vt_des=affected_vertices[current_vt++];			
		
	  }*/
	  //std::cout<<std::endl;


	  //############## PARALLEL INITIALIZATION HERE ################################################
	if (iter!=0)
	{
		std::cout<<vt_des<<","<<vt_des_sr<<std::endl;
/*		for (int temp_i=0;temp_i<N;temp_i++)
		{
			std::cout<<dist_dyn[temp_i]<<" ";
		} 
		std::cout<<std::endl;
*/
		int delta_wt=dist_dyn[vt_des]-dist_dyn[vt_des_sr]-weight[count_wt-1];
//		std::cout<<"delta wt"<<delta_wt<<std::endl;
		node *aff_node=SP_tree.node_pointer[vt_des];
		//std::cout<<"Accessed"<<std::endl;
		double curr_node_val=aff_node->val;
//		std::cout<<"Accessed"<<std::endl;
		double start_lim=curr_node_val;
		double stop_lim=SP_tree.node_val[vt_des];
//		std::cout<<"Accessed 1"<<std::endl;
		stop_lim+=start_lim;
		if (delta_wt>0)
			while(curr_node_val<stop_lim && aff_node!=NULL)
			{
				std::cout<<aff_node->node_num<<std::endl;
				dist_dyn[aff_node->node_num]=dist_dyn[aff_node->node_num]-delta_wt;
				vtx_des eq_vtx=reverse_vertex_map[aff_node->node_num];
				for (out_edg_itr out_e=out_edges(eq_vtx,g);out_e.first!=out_e.second;out_e.first++)
				{
					node *temp=SP_tree.node_pointer[target(*(out_e.first),g)];
					if (temp->val<start_lim || temp->val >= stop_lim)
					{
						if(dist_dyn[target(*(out_e.first),g)]>dist_dyn[aff_node->node_num]+weight_pmap[*(out_e.first)])
						{
							dist_dyn[target(*(out_e.first),g)]=dist_dyn[aff_node->node_num]+weight_pmap[*(out_e.first)];
							affected_vertices[update++]=target(*(out_e.first),g);
							std::cout<<target(*(out_e.first),g)<<" Inserted"<<std::endl;
							parent_dyn[target(*(out_e.first),g)]=source(*(out_e.first),g);
						}
					}
				}
				aff_node=aff_node->next;

				if (aff_node!=NULL)
					curr_node_val=aff_node->val;
			}

	}

	  std::vector<int> dist_restart(N, (std::numeric_limits < short >::max)());
	  std::vector<std::size_t> parent(N);
	  for (i = 0; i < N; ++i){
	    parent[i] = i;
	  }
	  dist_restart[3] = 0;
	  dist_dyn[3] = 0;

	// Restarting Bellman Ford :
/*	#if defined(BOOST_MSVC) && BOOST_MSVC <= 1300
	  bool r = bellman_ford_shortest_paths
	    (g, int(N), weight_pmap, &parent[0], &dist_restart[0], 
	     closed_plus<int>(), std::less<int>(), default_bellman_visitor());
	#else*/
	  bool r = bellman_ford_shortest_paths
	    (g, int (N), weight_map(weight_pmap).distance_map(&dist_restart[0]).
	     predecessor_map(&parent[0]));
	//#endif
	//---------------------------------------

	// Dynamic Parallel SSSP
	/*#if defined(BOOST_MSVC) && BOOST_MSVC <= 1300
	  bool r_dyn = dynamic_bellman_ford_shortest_paths
	    (g, int(N), weight_pmap, &parent[0], &dist_dyn[0], 
	     closed_plus<int>(), std::less<int>(), default_bellman_visitor());
	#else*/
	  if(iter!=0){
	  std::cout<<"Exec Dyn"<<","<<update<<std::endl;
	  bool r_dyn = dynamic_bellman_ford_shortest_paths
	    (g, int (N), weight_map(weight_pmap).distance_map(&dist_dyn[0]).
	     predecessor_map(&parent_dyn[0]),affected_vertices, update, &SP_tree);
	    std::cout<<"Returned from function"<<std::endl;
	  }
	  else{
	  	for(i=0;i<N;i++){
			dist_dyn[i]=dist_restart[i];
			parent_dyn[i]=parent[i];
		}
		std::vector<int> adj[100000];
		for( int it=0;it<N;it++)
		{
			if(it!=3)
				adj[parent[it]].push_back(it);
		}
		// *****************************************INITIALIZE THE SP_tree*******************
		int curr=0;
		node *locator=SP_tree.tree_head;
		std::stack<int> st;
		st.push(3);
		while(!st.empty())
		{
			int temp=st.top();
			st.pop();
			for(int ls=0;ls<adj[temp].size();ls++)
			{
				st.push(adj[temp][ls]);
			}
			if(SP_tree.tree_head==NULL)
			{
				SP_tree.tree_head = new node;
				SP_tree.tree_head->node_num=temp;
				SP_tree.tree_head->val=curr++;
				SP_tree.tree_head->next=NULL;
				SP_tree.tree_head->prev=NULL;
				locator=SP_tree.tree_head;
			}
			else
			{
				locator->next=new node;
				locator->next->node_num=temp;
				locator->next->val=curr++;
				locator->next->next=NULL;
				locator->next->prev=locator;
				locator=locator->next;
			}
			//std::cout<<locator->node_num<<std::endl;
			SP_tree.node_pointer[locator->node_num]=locator;
			std::cout<<SP_tree.node_pointer[locator->node_num]->node_num<<" ";

		}
		for (int it=0;it <N;it++)
		{
			if (SP_tree.node_pointer[it]==NULL)
			{
				SP_tree.node_pointer[it]=new node;
				SP_tree.node_pointer[it]->node_num=it;
				SP_tree.node_pointer[it]->val=curr++;
				SP_tree.node_pointer[it]->next=NULL;
				SP_tree.node_pointer[it]->prev=NULL;
				SP_tree.node_val[it]=1;
			}
		}
		locator=SP_tree.tree_head;
		while(locator!=NULL)
		{
			std::cout<<locator->node_num<<" ";
			locator=locator->next;
		}
		std::cout<<std::endl;
		for(int it=0;it<N;it++)
		{
			std::cout<<it<<": ";
			for(int ls=0;ls<adj[it].size();ls++)
			{
				std::cout<<adj[it][ls]<<" ";
			}
			std::cout<<std::endl;
		}
		for(int it=0;it<N;it++)
		{
			if (SP_tree.node_pointer[it]!=NULL)
				std::cout<<SP_tree.node_pointer[it]->node_num<<" ";
		}
		std::cout<<std::endl;

		//************************8FILL IN THE LENGTH OF CHILD VALUES***************************
		SP_tree.node_val[3]=fill_len( &SP_tree, 3,adj);
		std::cout<<"Printing tree"<<std::endl;
		for(int it=0;it<N;it++)
				if (SP_tree.node_pointer[it]!=NULL)
					std::cout<<SP_tree.node_pointer[it]->node_num<<" ";
		std::cout<<std::endl;
	  }
	//#endif
	//--------------------------------------

/*	  std::vector<int> dist_dyn_new(N, (std::numeric_limits < short >::max)());
	  for (i = 0; i < N; ++i)
	    parent[i] = i;
	  dist_dyn_new[3] = 0;

	// Dynamic improved Parallel SSSP
	/* Call Here*/
	//--------------------------------------


// DISPLAY SHORTEST PATHS AND PARENTS OF NODES : 

/*	  if (r)
	    for (i = 0; i < N; ++i)
	      std::cout << i << ": " << std::setw(3) << dist_restart[i]
		<< " " << parent[i] << std::endl;
	  else
	    std::cout << "negative cycle" << std::endl;	
*/
	  int error=0;
	  for(i=0;i<N;i++){
		if (dist_restart[i]-dist_dyn[i]>=0)
		  error+=dist_restart[i]-dist_dyn[i];
		else
		  error+=dist_dyn[i]-dist_restart[i];
	  }
	  std::cout<<" error : "<<error<<std::endl;
	
	  std::ofstream dot_file("figs/bellman-eg.dot");
	  dot_file << "digraph D {\n"
	    << "  rankdir=LR\n"
	    << "  size=\"5,3\"\n"
	    << "  ratio=\"fill\"\n"
	    << "  edge[style=\"bold\"]\n" << "  node[shape=\"circle\"]\n";

	  {
	    for (boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei) {
	      graph_traits < Graph >::edge_descriptor e = *ei;
	      graph_traits < Graph >::vertex_descriptor
		u = source(e, g), v = target(e, g);
	      // VC++ doesn't like the 3-argument get function, so here
	      // we workaround by using 2-nested get()'s.
	      dot_file << u << " -> " << v
		<< "[label=\"" << get(get(&EdgeProperties::weight, g), e) << "\"";
	      if (parent[v] == u)
		dot_file << ", color=\"black\"";
	      else
		dot_file << ", color=\"grey\"";
	      dot_file << "]";
	    }
	  }
	  dot_file << "}";
	  }
	  return EXIT_SUCCESS;
}

