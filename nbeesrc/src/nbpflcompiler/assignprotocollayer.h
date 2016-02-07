/*****************************************************************************/
/*                                                                           */
/* Copyright notice: please read file license.txt in the NetBee root folder. */
/*                                                                           */
/*****************************************************************************/

/****************************************************************************/
/*																		    */
/* class developed by Ivano Cerrato - wed nov 30 2011			   			*/
/*																		    */
/****************************************************************************/

#pragma once


#include "defs.h"
#include "symbols.h"
#include <math.h>
//#include <iostream>
//#include <set>

#define INF 65535

using namespace std;


class AssignProtocolLayer
{

	EncapGraph	&m_Graph;
	
	void Initialize(void)
	{
		EncapGraph::NodeIterator i = m_Graph.FirstNode();
		for (; i != m_Graph.LastNode(); i++)
		{
			if((*i)->NodeInfo->ID!=0) //the node is not the startproto
				(*i)->NodeInfo->Level = INF;
			else
				(*i)->NodeInfo->Level = 1;
		} 
	
	}
	
	
	EncapGraph::GraphNode *GetMinSuccessor(EncapGraph::GraphNode &node)
	{
		EncapGraph::GraphNode *successor = NULL;
		double minLevel = INF;
		list<EncapGraph::GraphNode*> &successors = node.GetSuccessors();
		list<EncapGraph::GraphNode*>::iterator s = successors.begin();
		for (; s != successors.end(); s++)
		{
			if(*s == &node) //we must no consider this case
				continue;
			if ((*s)->NodeInfo->Level < minLevel)
			{
				minLevel = (*s)->NodeInfo->Level;
				successor = *s;
			}
		}
		return successor;
	}
	
	EncapGraph::GraphNode *GetMaxPredecessor(EncapGraph::GraphNode &node)
	{	
		EncapGraph::GraphNode *predecessor = NULL;
		double maxLevel = 0;
		list<EncapGraph::GraphNode*> &predecessors = node.GetPredecessors();
		list<EncapGraph::GraphNode*>::iterator p = predecessors.begin();
		for (; p != predecessors.end(); p++)
		{
			if(*p == &node) //we must no consider this case
				continue;
			if ((*p)->NodeInfo->Level >= maxLevel)
			{
				maxLevel = (*p)->NodeInfo->Level;
				predecessor = *p;
			}
		}
		return predecessor;
	}
	
	void DepthWalk(EncapGraph::GraphNode &node)
	{

		if (node.Visited)
			return;

		node.Visited = true;

		double nodeLevel = node.NodeInfo->Level;

		EncapGraph::GraphNode *minSuccessor = GetMinSuccessor(node);
		
		double nextLevel = (minSuccessor!=NULL) ? minSuccessor->NodeInfo->Level : INF;

		if (nextLevel <= nodeLevel)
		{
			EncapGraph::GraphNode *maxPredecessor = GetMaxPredecessor(node);
			nbASSERT(maxPredecessor != NULL, "maxPredecessor should not be NULL");
			double prevLevel = maxPredecessor->NodeInfo->Level;
			if (prevLevel < nextLevel)
				node.NodeInfo->Level = prevLevel + ((nextLevel - prevLevel) / 2);
		}

		double level = ceil(node.NodeInfo->Level + 1);
		list<EncapGraph::GraphNode*> &successors = node.GetSuccessors();
		list<EncapGraph::GraphNode*>::iterator i = successors.begin();
		for (; i != successors.end(); i++)
		{
			if (level < (*i)->NodeInfo->Level)
				(*i)->NodeInfo->Level = level;
		}
		list<EncapGraph::GraphNode*>::iterator ss = successors.begin();
		for (; ss != successors.end(); ss++)
			DepthWalk(**ss);

	}

public:

	AssignProtocolLayer(EncapGraph &graph)
		:m_Graph(graph){}
		
	void Classify(void)
	{
		Initialize();		
		m_Graph.ResetVisited();
		DepthWalk(m_Graph.GetStartProtoNode());
	}



};

