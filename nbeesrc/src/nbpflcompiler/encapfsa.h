/*****************************************************************************/
/*                                                                           */
/* Copyright notice: please read file license.txt in the NetBee root folder. */
/*                                                                           */
/*****************************************************************************/



#pragma once

#include "defs.h"
#include "encapgraph.h"
#include "pflexpression.h"
#include "errors.h"
#include "sft/sft.hpp"
#include "sft/sft_writer.hpp"
#include "strutils.h"
#include <map>
#include "filtersubgraph.h"
#include "../nbee/globals/globals.h"

using namespace std;

class State;
class Transition;
class InnerExpressionNode;

typedef pair<SymbolProto*, SymbolProto*> EncapLabel_t;

class EncapLabelLess
{
	bool operator()(EncapLabel_t &a, EncapLabel_t &b)
	{
		if (a.first->ID < b.first->ID)
			return true;
		if (a.second->ID < b.second->ID)
			return true;
		return false;
	}
};

template <>
struct STraits<SymbolProto*>
{
	static SymbolProto* null()
	{
		return NULL;
	}


	static SymbolProto* Union(SymbolProto* s1, SymbolProto* s2)
	{
		return s1;
	}


	static std::string ToString(SymbolProto *s_info)
	{
		if (s_info != NULL)
		{
			std::string s;
			StringSplitter strSplit(s_info->ToString(), 10);
			while (strSplit.HasMoreTokens())
			{
				s += strSplit.NextToken();
				if (strSplit.HasMoreTokens())
					s += string("\\n");
			}
			return s;
		}
		return string("");
	}
};


template <>
struct LTraits<EncapLabel_t>
{
	static std::string ToString(EncapLabel_t &l_info)
	{
		std::string from(left(l_info.first->ToString(), 4));
		std::string to(left(l_info.second->ToString(), 4));
		return from + std::string("_") + to;
	}
};

template <>
struct PTraits<PFLTermExpression*>
{
	static PFLTermExpression* One()
	{
		return new PFLTermExpression(true);
	}
	static PFLTermExpression* Zero()
	{
		return new PFLTermExpression(false);
	}

	static PFLExpression* AndOp(PFLExpression* a, PFLExpression* b)
	{
          // optimizations
          if (a->ToString() == "true") return b;
          if (b->ToString() == "true") return a;

          return new PFLBinaryExpression(a, b, BINOP_BOOLAND);
	}

	static PFLExpression* OrOp(PFLExpression* a, PFLExpression* b)
	{
		return new PFLBinaryExpression(a, b, BINOP_BOOLOR);
	}

	static PFLExpression* NotOp(PFLExpression* a)
	{
		return new PFLUnaryExpression(a, UNOP_BOOLNOT);
	}


	static std::string ToString(PFLExpression* p_info)
	{
		assert(p_info != NULL);
		return p_info->ToString();
	}

	static bool DumpNeeded(PFLExpression* p_info)
	{
		if (p_info->IsTerm())
		{
			PFLTermExpression *termExp = (PFLTermExpression*)p_info;
			if (termExp->IsConst())
			{
				//the const expression is needed only if its value is false (useful for debugging purposes)
				if (!termExp->GetValue())
					return true;

				return false;
			}
			if (termExp->GetIRExpr() == NULL)
				return false;
		}
		return true;

	}


};

// These typedef require the previous traits to be already defined
typedef sftFsa<SymbolProto *, EncapLabel_t, PFLTermExpression*> EncapFSA_t;
typedef EncapFSA_t::Alphabet_t Alphabet_t;
typedef EncapFSA_t::trans_pair_t trans_pair_t;


/*!
 * \brief This class inherits from the sftFsa<> template and represents a concrete Filter FSA
 */

class EncapFSABuilder;

class EncapFSA: public EncapFSA_t
{
private:
  static void migrateTransitionsAndStates(EncapFSA *to, EncapFSA *from, StateIterator init);
  static EncapGraph *m_ProtocolGraph;

public:
	EncapFSA(Alphabet_t &alphabet)
		:EncapFSA_t(alphabet){}

	~EncapFSA()
	{
		//delete all the PFLExpression object associated to transitions
		EncapFSA_t::TransIterator j = FirstTrans();
		for (; j != LastTrans(); j++)
		{
			trans_pair_t transInfo = (*j).GetInfo();
			delete transInfo.second;
		}
	}
	
	//this static method set the encapsulation graph. if you call it two times, it is an error
	static void SetGraph(EncapGraph *graph){
		nbASSERT(m_ProtocolGraph==NULL,"you can use this method only a time!");
		m_ProtocolGraph = graph;
	}

	StateIterator GetStateIterator(State *s);

	void BooleanNot();
	static EncapFSA* BooleanAND(EncapFSA *fsa1, EncapFSA *fsa2, bool fieldExtraction, NodeList_t toExtract);
	static EncapFSA* BooleanOR(EncapFSA *fsa1, EncapFSA *fsa2, bool fieldExtraction, NodeList_t toExtract);
	static EncapFSA* NFAtoDFA(EncapFSA *orig, bool fieldExtraction, NodeList_t toExtract, bool setInfo = true);
    void fixStateProtocols();
    void setFinalStates(bool fieldExtraction, NodeList_t toExtract);
    bool checkIfInsert(NodeList_t toExtract, SymbolProto *protocol);

    static void prettyDotPrint(EncapFSA *fsa, char *description, char *dotfilename, char *src_file=NULL, int src_line=-1);
        // use the following macro in most cases
#ifdef ENABLE_PFLFRONTEND_DBGFILES
#define PRINT_DOT(fsa,desc,filename) do {                               \
          EncapFSA::prettyDotPrint((fsa),(desc),(filename),__FILE__,__LINE__); \
        } while (0)
#else
#define PRINT_DOT(fsa,desc,filename)

#endif /* #ifdef ENABLE_PFLFRONTEND_DBGFILES */


	EncapFSA::StateIterator GetStateIterator(EncapFSA::State &state);

};

typedef EncapFSA::StateIterator EncapStIterator_t;
typedef EncapFSA::TransIterator EncapTrIterator_t;



/*!
	\brief
*/

class EncapFSABuilder
{
	typedef map<EncapGraph::GraphNode*, EncapStIterator_t> NodeMap_t;
	EncapGraph					      &m_Graph;
	EncapFSA::Alphabet_t				      &m_Alphabet;
	NodeMap_t					      m_NodeMap;
	int						      m_Status; //it can be nbSUCCESS, nbFAILURE or nbWARNING
	ErrorRecorder 					      m_ErrorRecorder; //needed to store a message error

        /* This method adds a new state to the provided 'fsa' and
         * fills it with 'origNode'. A pointer to the new state is
         * returned.
         *
         * If 'force' is false (default behaviour) and another state
         * with the same 'origNode' is already present, the insertion
         * is skipped and a pointer to the old node is returned.
         *
         * If 'force' is true, the insertion is executed in any case,
         * and a pointer to the inserted node is returned, but not
         * stored internally for the next returns (unless this is the
         * first call for the current 'origNode').
         */
	EncapStIterator_t AddState(EncapFSA &fsa, EncapGraph::GraphNode &origNode, bool force = false);
	
	void CreateFinalAcceptingState(EncapFSA *fsa, SymbolProto *protocol, list<PFLSetExpression*>* innerList, EncapStIterator_t finalNotAccept, list<EncapFSA::State*> previousState, map<EncapGraph::GraphNode*,EncapFSA::State*> statesBefore, bool mandatory, uint32 counter, list<EncapFSA::State*> *toBeRemoved, list<EncapGraph::GraphNode*> previousNodes, EncapFSA::State *targetState, PFLTermExpression *expr); //[icerrato]
	
	void CompleteFSA(EncapFSA *fsa, EncapFSA::State *targetState, uint32 index, EncapGraph::GraphNode *targetNode, EncapFSA::Alphabet_t empty, EncapStIterator_t finalNotAccept, EncapFSA::Transition *fromTargetToNotAcpt, PFLTermExpression *expr/*it is NULL if we have not the extenxded transition*/);
	
	void AddPath(EncapFSA *fsa, EncapFSA::State *targetState, uint32 index, EncapGraph::GraphNode *targetNode, EncapFSA::Alphabet_t empty, multimap<EncapGraph::GraphNode*,EncapGraph::GraphEdge> mp, EncapStIterator_t finalNotAccept,  EncapFSA::Transition *fromTargetToNotAcpt, PFLTermExpression *expr/*it is NULL if we have not the extenxded transition*/);
	
	
	
	bool LinkStateToFirstPart(EncapFSA::State *toState, EncapGraph::GraphNode *toNode, uint32 counter, map<EncapGraph::GraphNode*,EncapFSA::State*> statesBefore,EncapFSA *fsa, list<EncapFSA::State*> *toBeRemoved,EncapFSA::State *finalNotAccept, bool currentIsTarget, PFLTermExpression *expr, bool startIsFine = false); //[icerrato]
	
	bool LinkTwoStates(EncapFSA::State *stateTo,EncapGraph::GraphNode* nodeTo, uint32 counter, map<EncapGraph::GraphNode*,EncapFSA::State*> from,EncapFSA *fsa,list<EncapFSA::State*> *toBeRemoved,EncapFSA::State *finalNotAccept, list<EncapFSA::State*> *firstPredecessors, bool firstRec, bool currentIsTarget, PFLTermExpression *expr); //[icerrato]
	
	void RemoveUnlinkedStates(EncapFSA *fsa, list<EncapFSA::State*> *toBeRemoved); //[icerrato]
	
	bool LinkStateToFirstPartNotIn(EncapFSA::State *toState,EncapGraph::GraphNode *toNode, int counter, uint32 any, map<EncapGraph::GraphNode*,EncapFSA::State*> statesBefore,EncapFSA *fsa, list<EncapFSA::State*> *toBeRemoved,EncapFSA::State *finalNotAccept, set<EncapGraph::GraphNode*> toAvoidNodes, bool currentIsTarget, PFLTermExpression *expr); //[icerrato]
	
	bool LinkTwoStatesNotIn(EncapFSA::State *toState, EncapGraph::GraphNode *toNode, int counter, uint32 any, EncapFSA *fsa, list<EncapFSA::State*> *toBeRemoved,EncapFSA::State *finalNotAccept, set<EncapGraph::GraphNode*> toAvoidNodes, map<EncapGraph::GraphNode*,EncapFSA::State*> previous, bool currentIsTarget, PFLTermExpression *expr); //[icerrato]
	
	bool HasIncomingEpsilon(EncapFSA::State* state); //[icerrato]
	
	
	EncapFSA::State *GetStateFromProto(map<SymbolProto*,EncapFSA::State*> *states, EncapFSA *fsa,SymbolProto *protocol, EncapGraph::GraphNode *node, map</*EncapGraph::GraphNode**/EncapFSA::State*,EncapFSA::Transition*> *transitionsToDummy, EncapStIterator_t dummyState,map<EncapGraph::GraphNode*,set<EncapLabel_t> > *labelsFound/*, list<EncapFSA::State*> *createdStates*/); //[icerrato]
	
	EncapFSA *AddSecondPartTunneled(EncapFSA *fsa, set<pair<EncapFSA::State*,SymbolProto*> > transitions, EncapStIterator_t targetAcceptingState, map<EncapFSA::State*,EncapFSA::Transition*> toFail, EncapStIterator_t finalNotAccept, SymbolProto *target, Node *irExpr,PFLTermExpression *expr); //[icerrato]
	
	void AddLabels(map<EncapGraph::GraphNode*,set<EncapLabel_t> > labelsFound, map<EncapFSA::State*,EncapFSA::Transition*>transitionsToDummy, map<SymbolProto*,EncapFSA::State*>states); //[icerrato]
	
	void RemoveUselessTransitions(map<SymbolProto*,EncapFSA::State*>states, EncapStIterator_t dummyState, EncapFSA *fsa); //[icerrato]
	
	set<EncapLabel_t> FindLabels(EncapFSA::State *lastState, EncapFSA *fsa); //[icerrato]
	
	void AddSelfTransitions(map<SymbolProto*,EncapFSA::State*>states, EncapFSA *fsa); //[icerrato]

	/************************************************************************************************/
	//HEADER CHAIN

	bool StartAsFirst(list<PFLSetExpression*>* innerList);
	EncapFSA::Alphabet_t CompleteAlphabet(set<SymbolProto*> *allProtos);
  	bool AssignProtocol(EncapFSA* fsa);
	void CutOutgoingTransitions(EncapFSA *fsa,EncapFSA::State *state);
	bool RemoveUselessStates(EncapFSA* fsa);
	bool ExpandStates(EncapFSA* fsa);
	void ReplaceTransitions(EncapFSA *fsa,map<EncapLabel_t,EncapFSA::State*> labelToState,map<int,map<EncapLabel_t,EncapFSA::State*> > labelFromState,Alphabet_t onSelfLoop,map<SymbolProto*,EncapFSA::State*> newStates);
	void LinkToNotAccept(EncapFSA *fsa);
	void CreateNoMatchAutomaton(EncapFSA *fsa);
	void FindTraps(EncapFSA *fsa);	
	EncapFSA::State *CreateLink(EncapFSA *fsa,set<SymbolProto*>previousProtos,set<SymbolProto*>currentProtos,EncapFSA::State *previousState,PFLRepeatOperator repeatOp,EncapFSA::Alphabet_t completeAlphabet);
	EncapFSA::Alphabet_t CreateAlphabet(set<SymbolProto*>previousProtos,set<SymbolProto*>currentProtos,EncapFSA::Alphabet_t completeAlphabet);
	
public:
	EncapFSABuilder(EncapGraph &protograph, EncapFSA::Alphabet_t &alphabet);

	EncapFSA *BuildEncapFSA(SymbolProto *target, PFLTermExpression *pred);
	
	EncapFSA *BuildEncapFSATunnels(SymbolProto *target, PFLTermExpression *expr); //[icerrato]
	
	//new methods
		EncapFSA *BuildRegExpFSA(list<PFLSetExpression*>* innerList);
		void FixAutomaton(EncapFSA *fsa);

			
	EncapFSA *BuildSingleNodeFSA(); //[icerrato]
	
	EncapFSA *ExtractFieldsFSA(NodeList_t &toExtract, EncapFSA *fsa); //[icerrato]
	
	int GetStatus() 
	{
		return m_Status;
	}
	
	ErrorRecorder &GetErrRecorder(void)
	{
		return m_ErrorRecorder;
	}
};

