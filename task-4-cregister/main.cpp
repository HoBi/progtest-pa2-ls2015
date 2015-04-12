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
        CCar( const string & rz, unsigned int vin, const string & name, const string & surname, vector<pair<string,string>> * oldOwners = NULL )
        {
            this->rz = rz;
            this->vin = vin;
            ownerName = name;
            ownerSurname = surname;
            if ( oldOwners != NULL ) ownerHistory = *oldOwners;
        }

        void NewOwner( const string & name, const string & surname )
        {
            ownerHistory.insert( ownerHistory.begin(), pair<string,string>(name, surname) );
        }

        string rz;
        unsigned int vin;
        string ownerName;
        string ownerSurname;
        vector<pair<string,string>> ownerHistory;
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
        * @return iterated car's VIN
        */
        unsigned int VIN( void )
        {
            return list.at( position )->vin;
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
* @class COwnerList
* @brief List of car owners (in correct historical sorting)
*/
class COwnerList
{
public:
    COwnerList()
    {

    }

    COwnerList( vector<pair<string,string>> prefill )
    {
        list = prefill;
    }

    /**
    * Inserts new item at the end of list
    */
    void Add( string name, string surname )
    {
        list.insert( list.end(), pair<string,string>( name, surname ) );
    }

    /**
    * @return iterated owner's name
    */
    string Name( void )
    {
        return list.at( position ).first;
    }

    /**
    * @return iterated car's RZ
    */
    string Surname( void )
    {
        return list.at( position ).second;
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
        vector<pair<string,string>> list;
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
            CCar needle = CCar( rz, 0, "", "" );
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
            CCar needle = CCar( "", 0, name, surname );
            return binary_search( carsByName.begin(), carsByName.end(), &needle, [] (CCar *a, CCar *b) -> bool
                {
                    return ( a->ownerName == b->ownerName ? a->ownerSurname < b->ownerSurname : a->ownerName < b->ownerName );
                }
            );
        }

        /**
        * Tells if car, specified by its VIN, exists
        * @param rz registration identification
        * @return exists
        */
        bool Exists( const unsigned int vin )
        {
            CCar needle = CCar( "", vin, "", "" );
            return binary_search( carsByVIN.begin(), carsByVIN.end(), &needle,
                    [] (CCar *a, CCar *b) -> bool { return a->vin < b->vin; }
            );
        }

        /**
        * Search car by its RZ. The car must exists, otherwise a wrong iterator could be returned
        * @param registration identification
        * @return iterator, found element
        */
        vector<CCar*>::iterator Find( const string & rz )
        {
            CCar needle = CCar( rz, 0, "", "" );
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
            CCar needle = CCar( "", 0, name, surname );
            return lower_bound( carsByName.begin(), carsByName.end(), &needle, [] (CCar *a, CCar *b) -> bool
            {
                return ( a->ownerName == b->ownerName ? a->ownerSurname < b->ownerSurname : a->ownerName < b->ownerName );
            }
            );
        }

        /**
        * Search car by its VIN number. The car must exists, otherwise a wrong iterator could be returned
        * @param vin VIN identificator
        * @return iterator, found element
        */
        vector<CCar*>::iterator Find( const unsigned int vin )
        {
            CCar needle = CCar( "", vin, "", "" );
            return lower_bound( carsByVIN.begin(), carsByVIN.end(), &needle,
                    [] (CCar *a, CCar *b) -> bool { return a->vin < b->vin; }
            );
        }

        /**
        * Adds a new car into the register
        * @param rz registration identification of the new car
        * @param vin VIN identification of the new car
        * @param name name of the car's owner
        * @param surname surname of the car's owner
        * @return success
        */
        bool AddCar( const string & rz, const unsigned int vin, const string & name, const string & surname, vector<pair<string,string>> * oldOwners = nullptr )
        {
            if ( Exists( rz ) || Exists( vin ) ) return false;
            else
            {
                CCar * car = new CCar( rz, vin, name, surname, oldOwners );
                car->NewOwner( name, surname );

                carsByName.insert( Find( name, surname ), car );
                carsByRZ.insert( Find( rz ), car );
                carsByVIN.insert( Find( vin ), car );
                return true;
            }
        }

        /**
        * Removes a car from the register by RZ
        * @param rz registration identification of the car to remove
        * @return success
        */
        bool DelCar( const string & rz )
        {
            if ( Exists( rz ) )
            {
                /* Remove from byRZ */
                auto it = Find( rz );
                CCar *fCar = *it;

                carsByRZ.erase( it );

                /* Remove from byName */
                for ( vector<CCar*>::iterator i = Find( fCar->ownerName, fCar->ownerSurname ); i != carsByName.end(); ++i )
                {
                    CCar * car = *i;
                    if ( car == fCar ) { carsByName.erase( i ); break; }
                }

                /* Remove from byVIN */
                carsByVIN.erase( Find( fCar->vin ) );

                delete fCar;

                return true;
            }
            else
                return false;
        }

        /**
        * Removes a car from the register by VIN
        * @param vin VIN identification of the car to remove
        * @return success
        */
        bool DelCar( const unsigned int vin )
        {
            if ( Exists( vin ) )
            {
                /* Remove from byVIN */
                auto it = Find( vin );
                CCar *fCar = *it;

                carsByVIN.erase( it );

                /* Remove from byName */
                for ( vector<CCar*>::iterator i = Find( fCar->ownerName, fCar->ownerSurname ); i != carsByName.end(); ++i )
                {
                    CCar * car = *i;
                    if ( car == fCar ) { carsByName.erase( i ); break; }
                }

                /* Remove from byRZ */
                carsByRZ.erase( Find( fCar->rz ) );

                delete fCar;

                return true;
            }
            else
                return false;
        }

        /**
        * Transfers a car to other owner by RZ
        * @param rz registration identification of the car to transfer
        * @param nName name of the new owner
        * @param nSurname surname of the new owner
        */
        bool Transfer( const string & rz, const string & nName, const string & nSurname)
        {
            if ( Exists( rz ) )
            {
                CCar * car = *Find( rz );

                unsigned int nVin = car->vin;
                vector<pair<string,string>> oldOwnerList = car->ownerHistory;

                if ( nName != car->ownerName || nSurname != car->ownerSurname )
                {
                    // This may be slow
                    if ( ! DelCar( rz ) ) return false;
                    if ( ! AddCar( rz, nVin, nName, nSurname, &oldOwnerList ) ) return false;

                    return true;
                } else
                    return false;
            }
            else
                return false;
        }


        /**
        * Transfers a car to other owner by VIN
        * @param rz registration identification of the car to transfer
        * @param nName name of the new owner
        * @param nSurname surname of the new owner
        */
        bool Transfer( const unsigned int vin, const string & nName, const string & nSurname)
        {
            if ( Exists( vin ) )
            {
                CCar * car = *Find( vin );

                string nRZ = car->rz;
                vector<pair<string,string>> oldOwnerList = car->ownerHistory;

                if ( nName != car->ownerName || nSurname != car->ownerSurname )
                {
                    // This may be slow
                    if ( ! DelCar( vin ) ) return false;
                    if ( ! AddCar( nRZ, vin, nName, nSurname, &oldOwnerList ) ) return false;

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

    /**
    * List all cars owned by the specified person
    * @param name person name
    * @param surname person surname
    * @return list of owned cars
    */
    COwnerList ListOwners( const string & rz )
    {
        if ( Exists( rz ) )
        {
            auto search = Find( rz );
            CCar *car = *search;

            return COwnerList( car->ownerHistory );
        }
        else
        {
            return COwnerList();
        }
    }

    /**
    * Returns the count of owned card by specified person
    * @param name person name
    * @param surname person surname
    * @return owned cars count
    */
    int CountOwners( const string & rz )
    {
        if ( Exists( rz ) )
        {
            auto search = Find( rz );
            CCar *car = *search;

            return car->ownerHistory.size();
        }
        else
        {
            return 0;
        }
    }

    /**
    * List all cars owned by the specified person
    * @param name person name
    * @param surname person surname
    * @return list of owned cars
    */
    COwnerList ListOwners( const unsigned int vin )
    {
        if ( Exists( vin ) )
        {
            auto search = Find( vin );
            CCar *car = *search;

            return car->ownerHistory;
        }
        else
        {
            return COwnerList();
        }
    }

    /**
    * Returns the count of owned card by specified person
    * @param name person name
    * @param surname person surname
    * @return owned cars count
    */
    int CountOwners( const unsigned int vin )
    {
        if ( Exists( vin ) )
        {
            auto search = Find( vin );
            CCar *car = *search;

            return car->ownerHistory.size();
        }
        else
        {
            return 0;
        }
    }

    private:
        vector<CCar*> carsByName;
        vector<CCar*> carsByRZ;
        vector<CCar*> carsByVIN;
        CCarList * lastCarList;
};

#ifndef __PROGTEST__
int main ( void )
{
    char name[50], surname[50];
    CRegister  b0;
    assert ( b0 . AddCar ( "ABC-12-34", "John", "Smith" ) == true );
    strncpy ( name, "John", sizeof ( name ) );
    strncpy ( surname, "Hacker", sizeof ( surname ) );
    assert ( b0 . AddCar ( "ABC-32-22", name, surname ) == true );
    strncpy ( name, "Peter", sizeof ( name ) );
    strncpy ( surname, "Smith", sizeof ( surname ) );
    assert ( b0 . AddCar ( "XYZ-11-22", name, surname ) == true );
    assert ( b0 . CountCars ( "John", "Hacker" ) == 1 );
    for ( CCarList l = b0 . ListCars ( "John", "Hacker" ); ! l . AtEnd (); l . Next () )
        cout << l . RZ () << endl;
    // the following license plate

    // ABC-32-22

    assert ( b0 . CountOwners ( "ABC-12-34" ) == 1 );
    for ( COwnerList l = b0 . ListOwners ( "ABC-12-34" ); ! l . AtEnd (); l . Next () )
        cout << l . Surname () << ", " << l . Name () << endl;
    // the following 1 owners in that order:

    // Smith, John

    CRegister b1 ( b0 );
    assert ( b0 . Transfer ( "XYZ-11-22", "John", "Hacker" ) == true );
    assert ( b0 . Transfer ( "XYZ-11-22", "Will", "Smith" ) == true );
    assert ( b1 . Transfer ( "XYZ-11-22", "John", "Smith" ) == true );
    assert ( b0 . CountOwners ( "XYZ-11-22" ) == 3 );
    for ( COwnerList l = b0 . ListOwners ( "XYZ-11-22" ); ! l . AtEnd (); l . Next () )
        cout << l . Surname () << ", " << l . Name () << endl;
    // the following 3 owners in that order:

    // Smith, Will
    // Hacker, John
    // Smith, Peter

    assert ( b1 . CountOwners ( "XYZ-11-22" ) == 2 );
    for ( COwnerList l = b1 . ListOwners ( "XYZ-11-22" ); ! l . AtEnd (); l . Next () )
        cout << l . Surname () << ", " << l . Name () << endl;
    // the following 2 owners in that order:

    // Smith, John
    // Smith, Peter

    b1 = b0;
    assert ( b0 . DelCar ( "XYZ-11-22" ) == true );
    assert ( b1 . DelCar ( "ABC-12-34" ) == true );
    assert ( b0 . CountCars ( "John", "Smith" ) == 1 );
    for ( CCarList l = b0 . ListCars ( "John", "Smith" ); ! l . AtEnd (); l . Next () )
        cout << l . RZ () << endl;
    // the following license plate

    // ABC-12-34

    assert ( b1 . CountCars ( "John", "Smith" ) == 0 );
    for ( CCarList l = b1 . ListCars ( "John", "Smith" ); ! l . AtEnd (); l . Next () )
        cout << l . RZ () << endl;
    // empty output


    CRegister  b2;
    assert ( b2 . AddCar ( "ABC-12-34", "John", "Smith" ) == true );
    assert ( b2 . AddCar ( "ABC-32-22", "John", "Hacker" ) == true );
    assert ( b2 . AddCar ( "XYZ-11-22", "Peter", "Smith" ) == true );
    assert ( b2 . AddCar ( "XYZ-11-22", "Jane", "Black" ) == false );
    assert ( b2 . DelCar ( "AAA-11-11" ) == false );
    assert ( b2 . DelCar ( "XYZ-11-22" ) == true );
    assert ( b2 . Transfer ( "BBB-99-99", "John", "Smith" ) == false );
    assert ( b2 . Transfer ( "ABC-12-34", "John", "Smith" ) == false );
    assert ( b2 . Transfer ( "XYZ-11-22", "John", "Smith" ) == false );
    assert ( b2 . CountCars ( "George", "White" ) == 0 );
    for ( CCarList l = b2 . ListCars ( "George", "White" ); ! l . AtEnd (); l . Next () )
        cout << l . RZ () << endl;
    // empty output

    assert ( b2 . CountOwners ( "AAA-AA-AA" ) == 0 );
    for ( COwnerList l = b2 . ListOwners ( "AAA-AA-AA" ); ! l . AtEnd (); l . Next () )
        cout << l . Surname () << ", " << l . Name () << endl;
    // the following 0 owners in that order:

    return 0;
}
#endif /* __PROGTEST__ */