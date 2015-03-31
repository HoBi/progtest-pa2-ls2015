#ifndef __PROGTEST__
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <string>
#include <memory>
#include <vector>
#include <algorithm>
using namespace std;
#endif /* __PROGTEST__ */

/**
* @class CCar
* @brief Instance of car
*/
class CCar
{
    public:
        CCar( const string & rz, const string & name, const string & surname )
        {
            this->rz = rz;
            ownerName = name;
            ownerSurname = surname;
        }

        string rz;
        string ownerName;
        string ownerSurname;
};

/**
* @class CCarList
* @brief List of cars with iterator
*/
class CCarList
{
    public:
        CCarList( void )
        {
            iterator = list.begin();
        }

        /**
        * Inserts new item at the end of list
        * @return iterator
        */
        vector<CCar*>::iterator Add( CCar* item )
        {
            return list.insert( list.end(), item );
        }

        /**
        * @return iterated car's RZ
        */
        string RZ( void ) const
        {
            return (*iterator)->rz;
        }

        /**
        * Tells if we are at the end of the list
        * @return end
        */
        bool AtEnd( void ) const
        {
            return iterator == list.end();
        }

        /**
        * Iterates next item in list
        */
        void Next( void )
        {
            iterator++;
        }

    private:
        vector<CCar*> list;
        vector<CCar*>::iterator iterator;
};

/**
* @class CRegister
* @brief Main class of the car register
*/
class CRegister
{
    public:
        CRegister( void )
        {

        }

        /**
        * Adds a new car into the register
        * @param rz registration identification of the new car
        * @param name name of the car's owner
        * @param surname surname of the car's owner
        * @return success
        */
        bool AddCar( const string & rz, const string & name, const string & surname )
        {
            CCar * car = new CCar( rz, name, surname );

            carsByName.insert( carsByName.end(), car );
            carsByRZ.insert( carsByRZ.end(), car );

            return true; // todo
        }

        /**
        * Removes a car from the register
        * @param rz registration identification of the car to remove
        * @return success
        */
        bool DelCar( const string & rz )
        {
            return false; // todo
        }

        /**
        * Transfers a car to other owner
        * @param rz registration identification of the car to transfer
        * @param nName name of the new owner
        * @param nSurname surname of the new owner
        */
        bool Transfer( const string & rz, const string & nName, const string & nSurname)
        {
            return false; //todo
        }

        vector<CCar*>::iterator Find( const string & rz )
        {
            CCar needle = CCar( rz, "", "" );
            vector<CCar*>::iterator result = lower_bound( carsByRZ.begin(), carsByRZ.end(), &needle,
                    [] (CCar *a, CCar *b) -> bool { return a->rz < b->rz; }
            );
            return ( *result != NULL && (*result)->rz == rz ? result : carsByRZ.end() );
        }

        vector<CCar*>::iterator Find( const string & name, const string & surname )
        {
            CCar needle = CCar( "", name, surname );
            vector<CCar*>::iterator result = lower_bound( carsByName.begin(), carsByName.end(), &needle, [] (CCar *a, CCar *b) -> bool
                {
                    return ( a->ownerName + a->ownerSurname ) < ( b->ownerName + b->ownerSurname );
                }
            );
            return ( *result != NULL && (*result)->ownerName == name && (*result)->ownerSurname == surname ? result : carsByName.end() );
        }

        /**
        * List all cars owned by the specified person
        * @param name person name
        * @param surname person surname
        * @return list of owned cars
        */
        CCarList ListCars( const string & name, const string & surname ) const
        {
            CCarList list;

            return list; // todo
        }

        /**
        * Returns the count of owned card by specified person
        * @param name person name
        * @param surname person surname
        * @return owned cars count
        */
        int CountCars( const string & name, const string & surname ) const
        {
            return -1; // todo
        }

        void Print( int field = 1 )
        {
            vector<CCar*> carField = field == 2 ? carsByRZ : carsByName;

            cout << endl << " = REGISTER | field " << field << " | : " << endl;

            int o = 0;

            for ( CCar * i : carField )
            {
                cout << " - " << o << ": [" << i << "] " << i->rz << ", " << i->ownerName << " " << i->ownerSurname << endl;
                o++;
            }
        }

    private:
        vector<CCar*> carsByName;
        vector<CCar*> carsByRZ;
};

#ifndef __PROGTEST__
int main ( void )
{

    CRegister reg;

    reg.AddCar( "ABC-12-34", "Marian", "Hlavac" );



    cout << *(reg.Find("Marian", "Hlavac")) << endl;
    cout << *(reg.Find("Neexistuju", "Abrakadabra")) << endl;

    reg.AddCar( "LOL-66-66", "Michal", "Hoobatka" );
    reg.AddCar( "ASS-69-69", "Tvoje", "Mama" );

    reg.Print( 1 );
    reg.Print( 2 );

    cout << *reg.Find( "Michal", "Hoobatka" ) << endl;
    cout << *reg.Find( "Neexistuju", "Pyco" ) << endl;

/*
    CRegister b1;
    assert ( b1 . AddCar ( "ABC-12-34", "John", "Smith" ) );
    assert ( b1 . AddCar ( "ABC-32-22", "John", "Hacker" ) );
    assert ( b1 . AddCar ( "XYZ-11-22", "Peter", "Smith" ) );
    assert ( b1 . CountCars ( "John", "Hacker" ) == 1 );
    for ( CCarList l = b1 . ListCars ( "John", "Hacker" ); ! l . AtEnd (); l . Next () )
        cout << l . RZ () << endl;
    // the following plate

    // ABC-32-22

    assert ( b1 . Transfer ( "XYZ-11-22", "John", "Hacker" ) );
    assert ( b1 . AddCar ( "XYZ-99-88", "John", "Smith" ) );
    assert ( b1 . CountCars ( "John", "Smith" ) == 2 );
    for ( CCarList l = b1 . ListCars ( "John", "Smith" ); ! l . AtEnd (); l . Next () )
        cout << l . RZ () << endl;
    // the following 2 licence plates, in any order:

    // ABC-12-34
    // XYZ-99-88

    assert ( b1 . CountCars ( "John", "Hacker" ) == 2 );
    for ( CCarList l = b1 . ListCars ( "John", "Hacker" ); ! l . AtEnd (); l . Next () )
        cout << l . RZ () << endl;
    // the following 2 licence plates, in any order:

    // ABC-32-22
    // XYZ-11-22

    assert ( b1 . CountCars ( "Peter", "Smith" ) == 0 );
    for ( CCarList l = b1 . ListCars ( "Peter", "Smith" ); ! l . AtEnd (); l . Next () )
        cout << l . RZ () << endl;
    // empty output

    assert ( b1 . Transfer ( "XYZ-11-22", "Jane", "Black" ) );
    assert ( b1 . CountCars ( "Jane", "Black" ) == 1 );
    for ( CCarList l = b1 . ListCars ( "Jane", "Black" ); ! l . AtEnd (); l . Next () )
        cout << l . RZ () << endl;
    // the following plate

    // XYZ-11-22

    assert ( b1 . CountCars ( "John", "Smith" ) == 2 );
    for ( CCarList l = b1 . ListCars ( "John", "Smith" ); ! l . AtEnd (); l . Next () )
        cout << l . RZ () << endl;
    // the following 2 licence plates, in any order:

    // ABC-12-34
    // XYZ-99-88

    assert ( b1 . DelCar ( "XYZ-11-22" ) );
    assert ( b1 . CountCars ( "Jane", "Black" ) == 0 );
    for ( CCarList l = b1 . ListCars ( "Jane", "Black" ); ! l . AtEnd (); l . Next () )
        cout << l . RZ () << endl;
    // empty output

    assert ( b1 . AddCar ( "XYZ-11-22", "George", "White" ) );

    CRegister b2;
    assert ( b2 . AddCar ( "ABC-12-34", "John", "Smith" ) );
    assert ( b2 . AddCar ( "ABC-32-22", "John", "Hacker" ) );
    assert ( b2 . AddCar ( "XYZ-11-22", "Peter", "Smith" ) );
    assert ( !b2 . AddCar ( "XYZ-11-22", "Jane", "Black" ) );
    assert ( !b2 . DelCar ( "AAA-11-11" ) );
    assert ( !b2 . Transfer ( "BBB-99-99", "John", "Smith" ) );
    assert ( !b2 . Transfer ( "ABC-12-34", "John", "Smith" ) );
    assert ( b2 . CountCars ( "George", "White" ) == 0 );
    for ( CCarList l = b2 . ListCars ( "George", "White" ); ! l . AtEnd (); l . Next () )
        cout << l . RZ () << endl;
    // empty output

    */

    return 0;
}
#endif /* __PROGTEST__ */