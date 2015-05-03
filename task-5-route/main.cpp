#ifndef __PROGTEST__
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <deque>
#include <map>
#include <set>
#include <list>
#include <algorithm>
using namespace std;
#endif /* __PROGTEST__ */

class NoRouteException { };
class RootNodeNotFound : NoRouteException {};

template <typename _T, typename _E> class CNode;

template <typename _T, typename _E>
class CEdge
{
public:
    _E *properties;
    CNode<_T, _E> *node1, *node2;

    CEdge( _E *properties, CNode<_T, _E> *node1, CNode<_T, _E> *node2 )
    {
        this->properties = properties;
        this->node1 = node1;
        this->node2 = node2;
    }
};

template <typename _T, typename _E>
class CNode
{
public:
    _T content;
    vector<CEdge<_T, _E>*> edges;

    CNode( _T content )
    {
        this->content = content;
    }

    void AssignEdge( CEdge<_T, _E> *edge )
    {
        edges.insert( edges.end(), edge );
    }
};

template <typename _T, typename _E>
class CRoute
{
public:
    CRoute()
    {

    }

    ~CRoute()
    {

    }

    CRoute &Add( _T from, _T to, _E edge )
    {
        auto nodeFrom = FindOrCreateNode( from );
        auto nodeTo = FindOrCreateNode( to );
        auto newEdge = new CEdge<_T, _E>( &edge, nodeFrom, nodeTo );
        nodeFrom->AssignEdge( newEdge );
        nodeTo->AssignEdge( newEdge );
        return *this;
    }

    list<_T> Find( _T from, _T to )
    {
        return Find( from, to, [] () { return true; } );
    }

    template<typename _F> list<_T> Find( _T from, _T to, const _F & filter )
    {
        queue<CNode<_T, _E>*> searchQueue;
        set<_T> explored;
        list<_T> route;
        map<CNode<_T, _E>*,CNode<_T, _E>*> previous;
        CNode<_T, _E> *node = nullptr;

        // Find the start node
        if ( nodes.count( from ) )
            searchQueue.push( nodes.at( from ) );
        else
            throw RootNodeNotFound();

        explored.insert( from );

        // Find the route
        while ( ! searchQueue.empty() )
        {
            // Dequeue
            CNode<_T, _E> *prev = node;
            node = searchQueue.front();
            searchQueue.pop();

            previous[node] = prev;

            cout << " # " << node->content << "( prev: " << (prev == NULL ? "start" : prev->content) << ")" << endl;

            // Did we found it?
            if ( node->content == to )
                cout << " - * FOUND " << endl;

            // Expand
            for ( CEdge<_T, _E> *edge : node->edges )
            {
                CNode<_T, _E> *other = edge->node1 == node ? edge->node2 : edge->node1;
                // todo: if fits in filter
                if ( true && explored.count( other->content ) == 0 )
                {
                    cout << "Adding " << other->content << " to queue " << endl;
                    searchQueue.push( other );
                    explored.insert( other->content );
                }
            }


        }
    }

private:
    map<_T,CNode<_T, _E>*> nodes;
    vector<CEdge<_T,_E>*> edges;

    CNode<_T, _E> *FindOrCreateNode( _T content )
    {
        if ( nodes.count( content ) > 0 )
        {
            // Return existing
            return nodes.at( content );
        }
        else
        {
            // Create new
            CNode<_T, _E> *node = new CNode<_T, _E>( content );
            nodes[ content ] = node;
            return node;
        }
    }
};

#ifndef __PROGTEST__
//=================================================================================================
struct CTrain
{
    CTrain ( const string & company, int speed )
            : m_Company ( company ), m_Speed ( speed ) { }

    string m_Company;
    int    m_Speed;
};

struct TrainFilterCompany
{
    TrainFilterCompany ( const set<string> & companies )
            : m_Companies ( companies ) { }

    bool operator () ( const CTrain & train ) const
    { return m_Companies . find ( train . m_Company ) != m_Companies . end (); }

    set <string> m_Companies;
};

struct TrainFilterSpeed
{
    TrainFilterSpeed ( int min, int max )
            : m_Min ( min ), m_Max ( max ) { }

    bool operator () ( const CTrain & train ) const
    { return train . m_Speed >= m_Min && train . m_Speed <= m_Max; }

    int m_Min, m_Max;
};

bool NurSchnellzug ( const CTrain & zug )
{
    return ( zug . m_Company == "OBB" || zug . m_Company == "DB" ) && zug . m_Speed > 100;
}
//=================================================================================================
static string      toText                                  ( const list<string> & l )
{
    ostringstream oss;

    auto it = l . cbegin();
    oss << *it;
    for ( ++it; it != l . cend (); ++it )
        oss << " > " << *it;
    return oss . str ();
}
//=================================================================================================
int main ( void )
{
    CRoute<string,CTrain> lines;

    lines . Add ( "Berlin", "Prague", CTrain ( "DB", 120 ) )
            . Add ( "Berlin", "Prague", CTrain ( "CD",  80 ) )
            . Add ( "Berlin", "Dresden", CTrain ( "DB", 160 ) )
            . Add ( "Dresden", "Munchen", CTrain ( "DB", 160 ) )
            . Add ( "Munchen", "Prague", CTrain ( "CD",  90 ) )
            . Add ( "Munchen", "Linz", CTrain ( "DB", 200 ) )
            . Add ( "Munchen", "Linz", CTrain ( "OBB", 90 ) )
            . Add ( "Linz", "Prague", CTrain ( "CD", 50 ) )
            . Add ( "Prague", "Wien", CTrain ( "CD", 100 ) )
            . Add ( "Linz", "Wien", CTrain ( "OBB", 160 ) )
            . Add ( "Paris", "Marseille", CTrain ( "SNCF", 300 ))
            . Add ( "Paris", "Dresden",  CTrain ( "SNCF", 250 ) );

    list<string> r1 = lines . Find ( "Berlin", "Linz" );
    assert ( toText ( r1 ) == "Berlin > Prague > Linz" );

    list<string> r2 = lines . Find ( "Linz", "Berlin" );
    assert ( toText ( r2 ) == "Linz > Prague > Berlin" );

    list<string> r3 = lines . Find ( "Wien", "Berlin" );
    assert ( toText ( r3 ) == "Wien > Prague > Berlin" );

    list<string> r4 = lines . Find ( "Wien", "Berlin", NurSchnellzug );
    assert ( toText ( r4 ) == "Wien > Linz > Munchen > Dresden > Berlin" );

    list<string> r5 = lines . Find ( "Wien", "Munchen", TrainFilterCompany ( set<string> { "CD", "DB" } ) );
    assert ( toText ( r5 ) == "Wien > Prague > Munchen" );

    list<string> r6 = lines . Find ( "Wien", "Munchen", TrainFilterSpeed ( 120, 200 ) );
    assert ( toText ( r6 ) == "Wien > Linz > Munchen" );

    list<string> r7 = lines . Find ( "Wien", "Munchen", [] ( const CTrain & x ) { return x . m_Company == "CD"; } );
    assert ( toText ( r7 ) == "Wien > Prague > Munchen" );

    list<string> r8 = lines . Find ( "Munchen", "Munchen" );
    assert ( toText ( r8 ) == "Munchen" );

    list<string> r9 = lines . Find ( "Marseille", "Prague" );
    assert ( toText ( r9 ) == "Marseille > Paris > Dresden > Berlin > Prague"
             || toText ( r9 ) == "Marseille > Paris > Dresden > Munchen > Prague" );

    try
    {
        list<string> r10 = lines . Find ( "Marseille", "Prague", NurSchnellzug );
        assert ( "Marseille > Prague connection does not exist!!" == NULL );
    }
    catch ( const NoRouteException & e ) { }
    try
    {
        list<string> r11 = lines . Find ( "London", "Oxford" );
        assert ( "London > Oxford connection does not exist!!" == NULL );
    }
    catch ( const NoRouteException & e ) { }
}
#endif  /* __PROGTEST__ */
