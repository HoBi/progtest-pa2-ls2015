#ifndef __PROGTEST__
#include <cassert>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <set>
#include <list>
#include <map>
#include <algorithm>

using namespace std;

class CResult
{
public:
    CResult ( const string & name, unsigned int   studentID, const string & test, int result ) :
            m_Name ( name ), m_StudentID ( studentID ), m_Test ( test ), m_Result ( result )
    { }

    string m_Name;
    unsigned int m_StudentID;
    string m_Test;
    int m_Result;
};
#endif /* __PROGTEST__ */

string trimString( string &t )
{
    t.erase( t.begin(), find_if( t.begin(), t.end(), not1( ptr_fun<int, int>( isspace ) ) ) );
    t.erase( find_if( t.rbegin(), t.rend(), not1( ptr_fun<int, int>( isspace ) ) ).base(), t.end() );
    return t;
    // - inspired on stackoverflow.com/questions/216823
}

bool cmp_ID( const CResult &lhs, const CResult &rhs )
{
    return lhs.m_StudentID < rhs.m_StudentID;
}

bool cmp_Name( const CResult &lhs, const CResult &rhs )
{
    return lhs.m_Name < rhs.m_Name;
}

bool cmp_Result( const CResult &lhs, const CResult &rhs )
{
    return lhs.m_Result > rhs.m_Result;
}

class CStudent
{
public:
    string name;
    unsigned int ID;
    list<string> cards;
};

class CExam
{
public:
    static const int SORT_NONE = 0, SORT_ID = 1, SORT_NAME = 2, SORT_RESULT = 3;

    ~CExam()
    {
        // Free all students
        for ( const auto &it : students ) {
            delete it.second;
        }

        // Free all missing results
        for ( const auto &it : missing ) {
            for ( const auto &ot : it.second ) {
                delete ot.second;
            }
        }

        // Free all resolved results
        for ( const auto &it : resolved ) {
            for ( const auto &ot : it.second ) {
                delete ot;
            }
        }
    }

    bool Load( istream &cardMap )
    {
        char c;
        list<CStudent*> newStudents;

        while ( !cardMap.eof() )
        {
            CStudent *newStudent = new CStudent();
            newStudents.insert( newStudents.end(), newStudent );

            c = '\0';

            // Read ID
            cardMap >> newStudent->ID;
            cardMap.get( c );
            if ( c != ':' ) {
                if (!cardMap.eof()) return cancelLoad(newStudents);
                else {
                    newStudents.remove(newStudent);
                    delete newStudent;
                    break;
                }
            }

            // Check ID uniqueness
            if ( students.count( newStudent->ID ) > 0 )
            {
                return cancelLoad( newStudents );
            }

            // Read name
            getline( cardMap, newStudent->name, ':' );

            // Read assigned cards line
            string cardsString;
            stringstream cardsStringStream;
            getline( cardMap, cardsString );
            cardsStringStream.str( cardsString );

            // And read all the cards and save them
            while ( !cardsStringStream.eof() )
            {
                getline( cardsStringStream, cardsString, ',' );
                trimString( cardsString );
                if ( cardsString != "" && cards.count( cardsString ) == 0 )
                    newStudent->cards.push_back( cardsString );
                else
                    return cancelLoad( newStudents );
            }
        }

        // Add new students
        for ( CStudent *student : newStudents)
        {
            students[ student->ID ] = student;
            for ( string card : student->cards )
            {
                cards[ card ] = student;
            }
        }


        return cardMap.eof();
    }

    bool Register( const string &cardID, const string &test )
    {
        // Check if card exists
        if ( cards.count( cardID ) > 0 )
        {
            // Get student by card
            CStudent *student = cards.at( cardID );

            // Make sure the student isn't registered already
            if ( registered[ test ].count( student->ID ) > 0 ) return false;

            // Register
            registered[ test ].insert( student->ID );

            // Create new test and categorize to missing
            missing[test].insert( missing[test].end(),
                    make_pair( student->ID, new CResult( student->name, student->ID, test, -1 ) )
            );

            return true;
        }
        else return false;
    }

    bool Assess( unsigned int studentID, const string &test, int result )
    {
        // Check if the student exists
        if ( students.count( studentID ) == 0 ) return false;

        // Check if the student doesn't have result in this test
        if ( resolvedStudents[test].count( studentID ) > 0 ) return false;

        // Find the missing test result
        if ( missing[test].count( studentID ) > 0 )
        {
            CResult *resultInst = missing[test].at( studentID );

            // Store the result
            resultInst->m_Result = result;

            // Remove from missing
            missing[test].erase( studentID );

            // Categorize the result as resolved
            resolved[test].push_back( resultInst );

            // Mark student as resolved
            resolvedStudents[test].insert( studentID );

            return true;
        }
        else return false;
    }

    list<CResult> ListTest( const string &test, int sortBy ) const
    {
        // Clone the list
        list<CResult> list;
        if ( resolved.count( test ) > 0 )
            for ( CResult *resLink : resolved.at( test ) )
                list.push_back( *resLink );

        // Sort it, if needed
        if ( sortBy == CExam::SORT_ID ) list.sort( cmp_ID );
        if ( sortBy == CExam::SORT_NAME ) list.sort( cmp_Name );
        if ( sortBy == CExam::SORT_RESULT ) list.sort( cmp_Result );

        return list;
    }

    set<string> ListMissing( const string &test ) const
    {
        set<string> students;

        if ( missing.count( test ) > 0 )
        {
            for (const auto &ot : missing.at(test)) {
                students.insert(ot.second->m_Name);
            }
        }
        return students;
    }

private:
    map<string, CStudent*> cards;
    map<unsigned int, CStudent*> students;
    map<string, set<unsigned int>> registered;
    map<string, vector<CResult*>> resolved;
    map<string, set<unsigned int>> resolvedStudents;
    map<string, map<unsigned int, CResult*>> missing;

    bool cancelLoad( list<CStudent*> createdStudents )
    {
        for ( CStudent* student : createdStudents )
            delete student;

        return false;
    }
};

#ifndef __PROGTEST__
string toString ( const list<CResult> & l )
{
    ostringstream oss;
    for ( auto const & x : l )
        oss << x . m_Name << ":" << x . m_StudentID << ":" << x . m_Test << ":" << x . m_Result << endl;
    return oss . str ();
}
string toString ( const set<string> & s )
{
    ostringstream oss;
    for ( const auto & x : s )
        oss << x << endl;
    return oss . str ();
}
int main ( void )
{
    istringstream iss;
    CExam         m;
    iss . clear ();
    iss . str ( "123456:Smith John:er34252456hjsd2451451, 1234151asdfe5123416, asdjklfhq3458235\n"
                        "654321:Nowak Jane: 62wtsergtsdfg34\n"
                        "456789:Nowak Jane: okjer834d34\n"
                        "987:West Peter Thomas:sdswertcvsgncse\n" );
    assert ( m . Load ( iss ) );

    assert ( m . Register ( "62wtsergtsdfg34", "PA2 - #1" ) );
    assert ( m . Register ( "62wtsergtsdfg34", "PA2 - #2" ) );
    assert ( m . Register ( "er34252456hjsd2451451", "PA2 - #1" ) );
    assert ( m . Register ( "er34252456hjsd2451451", "PA2 - #3" ) );
    assert ( m . Register ( "sdswertcvsgncse", "PA2 - #1" ) );
    assert ( !m . Register ( "1234151asdfe5123416", "PA2 - #1" ) );
    assert ( !m . Register ( "aaaaaaaaaaaa", "PA2 - #1" ) );
    assert ( m . Assess ( 123456, "PA2 - #1", 50 ) );
    assert ( m . Assess ( 654321, "PA2 - #1", 30 ) );
    assert ( m . Assess ( 654321, "PA2 - #2", 40 ) );
    assert ( m . Assess ( 987, "PA2 - #1", 100 ) );
    assert ( !m . Assess ( 654321, "PA2 - #1", 35 ) );
    assert ( !m . Assess ( 654321, "PA2 - #3", 35 ) );
    assert ( !m . Assess ( 999999, "PA2 - #1", 35 ) );
    assert ( toString ( m . ListTest ( "PA2 - #1", CExam::SORT_RESULT ) ) ==
             "West Peter Thomas:987:PA2 - #1:100\n"
                     "Smith John:123456:PA2 - #1:50\n"
                     "Nowak Jane:654321:PA2 - #1:30\n" );
    assert ( toString ( m . ListTest ( "PA2 - #1", CExam::SORT_NAME ) ) ==
             "Nowak Jane:654321:PA2 - #1:30\n"
                     "Smith John:123456:PA2 - #1:50\n"
                     "West Peter Thomas:987:PA2 - #1:100\n" );
    assert ( toString ( m . ListTest ( "PA2 - #1", CExam::SORT_NONE ) ) ==
             "Smith John:123456:PA2 - #1:50\n"
                     "Nowak Jane:654321:PA2 - #1:30\n"
                     "West Peter Thomas:987:PA2 - #1:100\n" );
    assert ( toString ( m . ListTest ( "PA2 - #1", CExam::SORT_ID ) ) ==
             "West Peter Thomas:987:PA2 - #1:100\n"
                     "Smith John:123456:PA2 - #1:50\n"
                     "Nowak Jane:654321:PA2 - #1:30\n" );
    assert ( toString ( m . ListMissing ( "PA2 - #3" ) ) ==
             "Smith John\n" );
    iss . clear ();
    iss . str ( "888:Watson Joe:25234sdfgwer52, 234523uio, asdf234235we, 234234234\n" );
    assert ( m . Load ( iss ) );

    assert ( m . Register ( "234523uio", "PA2 - #1" ) );
    assert ( m . Assess ( 888, "PA2 - #1", 75 ) );
    iss . clear ();
    iss . str ( "555:Gates Bill:ui2345234sdf\n"
                        "888:Watson Joe:2345234634\n" );
    assert ( !m . Load ( iss ) ); // duplicate id 888

    assert ( !m . Register ( "ui2345234sdf", "PA2 - #1" ) );
    iss . clear ();
    iss . str ( "555:Gates Bill:ui2345234sdf\n"
                        "666:Watson Thomas:okjer834d34\n" ); // duplicate card id okjer834d34
    assert ( !m . Load ( iss ) );

    assert ( !m . Register ( "ui2345234sdf", "PA2 - #3" ) );
    iss . clear ();
    iss . str ( "555:Gates Bill:ui2345234sdf\n"
                        "666:Watson Thomas:jer834d3sdf4\n" );
    assert ( m . Load ( iss ) );

    assert ( m . Register ( "ui2345234sdf", "PA2 - #3" ) );
    assert ( toString ( m . ListMissing ( "PA2 - #3" ) ) ==
             "Gates Bill\n"
                     "Smith John\n" );
    return 0;
}
#endif /* __PROGTEST__ */
