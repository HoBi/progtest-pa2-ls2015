#ifndef __PROGTEST__
#include <cassert>
#include <cstring>
#include <iostream>
#include <iomanip>
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
        /**
        * Inserts new item at the end of list
        */
        void Add( CCar* item )
        {
            list.insert( list.end(), item );
        }

        /**
        * @return iterated car's RZ
        */
        string RZ( void )
        {
            return list.at( position )->rz;
        }

        /**
        * Tells if we are at the end of the list
        * @return end
        */
        bool AtEnd( void )
        {
            return position == list.size();
        }

        /**
        * Iterates next item in list
        */
        void Next( void )
        {
            position++;
        }

    private:
        vector<CCar*> list;
        unsigned int position = 0;
};

/**
* @class CRegister
* @brief Main class of the car register
*/
class CRegister
{
    public:
        CRegister()
        {
            lastCarList = NULL;
        }

        ~CRegister()
        {
            /* Free up allocated memory by CCars */
            for ( CCar * car : carsByName )
            {
                delete car;
            }

            /* Clear last CCarList */
            if ( lastCarList != NULL )
                delete lastCarList;
        }

        /**
        * Tells if car, specified by its RZ, exists
        * @param rz registration identification
        * @return exists
        */
        bool Exists( const string & rz )
        {
            CCar needle = CCar( rz, "", "" );
            return binary_search( carsByRZ.begin(), carsByRZ.end(), &needle,
                    [] (CCar *a, CCar *b) -> bool { return a->rz < b->rz; }
            );
        }

        /**
        * Tells if car, specified by its owner's name and surname, exists
        * @param name owner's name
        * @param surname owner's surname
        * @return exists
        */
        bool Exists( const string & name, const string & surname )
        {
            CCar needle = CCar( "", name, surname );
            return binary_search( carsByName.begin(), carsByName.end(), &needle, [] (CCar *a, CCar *b) -> bool
                {
                    return ( a->ownerName == b->ownerName ? a->ownerSurname < b->ownerSurname : a->ownerName < b->ownerName );
                }
            );
        }

        /**
        * Search car by its RZ. The car must exists, otherwise a wrong iterator could be returned
        * @param registration identification
        * @return iterator, found element
        */
        vector<CCar*>::iterator Find( const string & rz )
        {
            CCar needle = CCar( rz, "", "" );
            auto result = lower_bound( carsByRZ.begin(), carsByRZ.end(), &needle,
                    [] (CCar *a, CCar *b) -> bool { return a->rz < b->rz; }
            );

            return result;

        }

        /**
        * Search car by its owner name and surname. The car must exists, otherwise a wrong iterator could be returned
        * @param name owner's name
        * @param surname owner's surname
        * @return iterator, found element
        */
        vector<CCar*>::iterator Find( const string & name, const string & surname )
        {
            CCar needle = CCar( "", name, surname );
            return lower_bound( carsByName.begin(), carsByName.end(), &needle, [] (CCar *a, CCar *b) -> bool
            {
                return ( a->ownerName == b->ownerName ? a->ownerSurname < b->ownerSurname : a->ownerName < b->ownerName );
            }
            );
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
            if ( ! Exists( rz ) )
            {
                CCar * car = new CCar( rz, name, surname );

                carsByName.insert( Find( name, surname ), car );
                carsByRZ.insert( Find( rz ), car );
                return true;
            }
            else
            {
                return false;
            }
        }

        /**
        * Removes a car from the register
        * @param rz registration identification of the car to remove
        * @return success
        */
        bool DelCar( const string & rz )
        {
            if ( Exists( rz ) )
            {
                auto it = Find( rz );
                CCar *fCar = *it;

                carsByRZ.erase( it );

                for ( vector<CCar*>::iterator i = Find( fCar->ownerName, fCar->ownerSurname ); i != carsByName.end(); ++i )
                {
                    CCar * car = *i;
                    if ( car == fCar ) { carsByName.erase( i ); break; }
                }

                delete fCar;

                return true;
            }
            else
                return false;
        }

        /**
        * Transfers a car to other owner
        * @param rz registration identification of the car to transfer
        * @param nName name of the new owner
        * @param nSurname surname of the new owner
        */
        bool Transfer( const string & rz, const string & nName, const string & nSurname)
        {
            if ( Exists( rz ) )
            {
                CCar * car = *Find( rz );

                if ( nName != car->ownerName || nSurname != car->ownerSurname )
                {
                    // This may be slow
                    if ( ! DelCar( rz ) ) return false;
                    if ( ! AddCar( rz, nName, nSurname ) ) return false;

                    return true;
                } else
                    return false;
            }
            else
                return false;
        }

        /**
        * List all cars owned by the specified person
        * @param name person name
        * @param surname person surname
        * @return list of owned cars
        */
        CCarList ListCars( const string & name, const string & surname )
        {
            CCarList *list = new CCarList();

            if ( Exists( name, surname ) )
            {
                for ( vector<CCar*>::iterator i = Find( name, surname ); i != carsByName.end(); ++i )
                {
                    CCar *car = *i;

                    if ( car->ownerName == name && car->ownerSurname == surname )
                        list->Add( car );

                    else
                        break;
                }
            }

            if ( lastCarList != NULL )
                delete lastCarList;

            lastCarList = list;

            return *list;
        }

        /**
        * Returns the count of owned card by specified person
        * @param name person name
        * @param surname person surname
        * @return owned cars count
        */
        int CountCars( const string & name, const string & surname )
        {
            int count = 0;

            if ( Exists( name, surname ) )
            {
                for ( vector<CCar*>::iterator i = Find( name, surname ); i != carsByName.end(); i++ )
                {
                    CCar * car = *i;

                    if ( car->ownerName == name && car->ownerSurname == surname )
                        count++;
                    else
                        break;
                }
            }

            return count;
        }

        void PrintAllCars( int sortedBy = 0 )
        {
            for ( CCar * car : ( sortedBy == 1 ? carsByName : carsByRZ) )
            {
                cout << car->rz << ": " << car->ownerName << " " << car->ownerSurname << endl;
            }
        }

    private:
        vector<CCar*> carsByName;
        vector<CCar*> carsByRZ;
        CCarList * lastCarList;
};

#ifndef __PROGTEST__
int main ( void )
{
    CRegister b1;
    assert ( b1 . AddCar ( "ABC-12-34", "John", "Smith" ) == true );
    assert ( b1 . AddCar ( "ABC-32-22", "John", "Hacker" ) == true );
    assert ( b1 . AddCar ( "XYZ-11-22", "Peter", "Smith" ) == true );
    assert ( b1 . CountCars ( "John", "Hacker" ) == 1 );
    for ( CCarList l = b1 . ListCars ( "John", "Hacker" ); ! l . AtEnd (); l . Next () )
        cout << l . RZ () << endl;
    // the following plate

    // ABC-32-22

    assert ( b1 . Transfer ( "XYZ-11-22", "John", "Hacker" ) == true );
    assert ( b1 . AddCar ( "XYZ-99-88", "John", "Smith" ) == true );
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

    assert ( b1 . Transfer ( "XYZ-11-22", "Jane", "Black" ) == true );
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

    assert ( b1 . DelCar ( "XYZ-11-22" ) == true );
    assert ( b1 . CountCars ( "Jane", "Black" ) == 0 );
    for ( CCarList l = b1 . ListCars ( "Jane", "Black" ); ! l . AtEnd (); l . Next () )
        cout << l . RZ () << endl;
    // empty output

    assert ( b1 . AddCar ( "XYZ-11-22", "George", "White" ) == true );

    CRegister b2;
    assert ( b2 . AddCar ( "ABC-12-34", "John", "Smith" ) == true );
    assert ( b2 . AddCar ( "ABC-32-22", "John", "Hacker" ) == true );
    assert ( b2 . AddCar ( "XYZ-11-22", "Peter", "Smith" ) == true );
    assert ( b2 . AddCar ( "XYZ-11-22", "Jane", "Black" ) == false );
    assert ( b2 . DelCar ( "AAA-11-11" ) == false );
    assert ( b2 . Transfer ( "BBB-99-99", "John", "Smith" ) == false );
    assert ( b2 . Transfer ( "ABC-12-34", "John", "Smith" ) == false );
    assert ( b2 . CountCars ( "George", "White" ) == 0 );
    for ( CCarList l = b2 . ListCars ( "George", "White" ); ! l . AtEnd (); l . Next () )
        cout << l . RZ () << endl;
    // empty output

    return 0;
}
#endif /* __PROGTEST__ */