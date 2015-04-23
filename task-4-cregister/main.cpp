#ifndef __PROGTEST__
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <iomanip>
using namespace std;
#endif /* __PROGTEST__ */

class IndexOutOfBoundaryException {};
class ItemNotFoundException {};

/**
 * @class CStringFuncs
 */
class CStringFuncs
{
public:
    static bool smallerThan( const char * lhs, const char * rhs )
    {
        // todo: může to bejt na progtestu zakázaný
        return strcmp( lhs, rhs ) < 0;
    }

    static bool areEqual( const char * lhs, const char * rhs )
    {
        // todo: může to bejt na progtestu zakázaný
        return strcmp( lhs, rhs ) == 0;
    }

    static char * makeCopy( const char * from )
    {
        int length = strlen( from ) + 1;
        char * nStr = new char[ length ];
        strcpy( nStr, from );
        return nStr;
    }
};

/**
 * @class CList
 * @brief Simple list
 */
template <class T> class CList
{
public:
    static const int ITEM_NOT_FOUND = -1;
    int length = 0;

    CList()
    {
        length = 0;
        max = 64;
        list = new T[ max ];
    }

    CList( const CList & list )
    {
        this->length = list.length;
        this->max = list.max;
        this->list = new T[ max ];

        for ( int i = 0; i < length; i++ )
            this->list[ i ] = list.At( i );
    }

    virtual CList & operator=( const CList & list )
    {
        return *this;
    }

    /**
     * Returns item from list at specified location
     * @param index
     */
    T At( int index ) const
    {
        if ( index < 0 || index >= length )
            throw IndexOutOfBoundaryException();

        return list[ index ];
    }

    /**
     * Insert item at specified position
     * @param item Item for insertion
     * @param pos Position of insert
     */
    virtual void Insert( T item, int pos )
    {
        if ( pos < 0 || pos > length )
            throw IndexOutOfBoundaryException();

        ShiftArrayRight( pos );
        list[ pos ] = item;
    }

    /**
     * Remove item at specified position
     * @param pos Position for remove
     */
    void Remove( int pos )
    {
        if ( pos < 0 || pos >= length )
            throw IndexOutOfBoundaryException();

        ShiftArrayLeft( pos );
    }

    /**
     * Prints whole list to cout for debugging purposes
     */
    friend ostream & operator<<( ostream & os, const CList<T> &obj )
    {
        for ( int i = 0; i < obj.length; i++ )
            cout << obj.At( i ) << ", ";
        cout << endl;

        return os;
    }

    /**
    * Gets iterated item
    * @return end
    */
    T Get( void ) const
    {
        return At( position );
    }

    /**
    * Tells if we are at the end of the list
    * @return end
    */
    bool AtEnd( void ) const
    {
        return position == length;
    }

    /**
    * Iterates next item in list
    */
    void Next( void )
    {
        position++;
    }

protected:
    T* list;
    bool isUnique = false;
    int max, position = 0;

    /**
     * Checks if there is still enough room for storing more items in list
     */
    void CheckArrayBoundaries()
    {
        if ( length >= max - 2 )
        {
            // We should extend the array, allocating new room
            max *= 2;
            T* newList = new T[ max ];

            for ( int i = 0; i < length; i++ )
                newList[ i ] = list[ i ];

            delete[] list;
            list = newList;
        }
    }

    /**
     * Shifts array by one, from specified index to the end, by one item to the right.
     * e.g.:  [ 1, 2, 3, 4, 6, 12 ] shifted from index 3 will make [ 1, 2, 3, 4, 4, 6, 12 ]
     * @param from Starting index of shift
     */
    void ShiftArrayRight( int from )
    {
        CheckArrayBoundaries();

        for ( int i = length - 1; i >= from; i-- )
            list[ i + 1 ] = list[ i ];

        length++;
    }

    /**
     * Shifts array by one, from specified index to the end, by one item to the left.
     * e.g.:  [ 1, 2, 3, 4, 6, 12 ] shifted from index 3 will make [ 1, 2, 4, 6, 12 ]
     * @param from Starting index of shift
     */
    void ShiftArrayLeft( int from )
    {
        CheckArrayBoundaries();

        for ( int i = from; i < length - 1; i++ )
            list[ i ] = list[ i + 1 ];

        length--;
    }

    /**
     * Returns last occurrence of duplicate items
     * todo: not sure, if even usable
     * @param indexFrom Starting index for searching the last occurence
     */
    int LastOccurrenceIdx( int indexFrom ) const
    {
        while ( indexFrom < length - 1 )
        {
            if ( At( indexFrom ) == At( indexFrom ) )
                return indexFrom + 1;
            indexFrom++;
        }

        return indexFrom + 1;
    }

    /**
     * Returns midpoint, used for binary search
     */
    int getmidpoint( int from, int to ) const
    {
        return ( to - from ) / 2 + from;
    }
};

/**
 * @class CSortedList
 * @brief Sorted list
 */
template <class T> class CSortedList : public CList<T>
{
public:

    /**
     * @param unique Flag to forcing all items to be unique in the list
     */
    CSortedList( bool unique = true, bool pointers = false )
    {
        // First allocation of dynamic array
        this->length = 0;
        this->max = 64;
        this->list = new T[ this->max ];
        this->isUnique = unique;
        this->pointers = pointers;
    }

    CSortedList( const CSortedList & list )
    {
        this->length = list.length;
        this->max = list.max;
        this->list = new T[ this->max ];
        this->isUnique = list.isUnique;
        this->pointers = list.pointers;

        for ( int i = 0; i < this->length; i++ )
            this->list[ i ] = list.At( i );
    }

    virtual CSortedList & operator=( const CSortedList & cslist )
    {
        if ( &cslist != this )
        {
            this->length = cslist.length;
            this->max = cslist.max;
            this->list = new T[ this->max ];
            this->isUnique = cslist.isUnique;
            this->pointers = cslist.pointers;

            if ( this->pointers )
                for ( int i = 0; i < this->length; i++ )
                {
                    *(this->list[ i ]) = *cslist.list[ i ];
                }
            else
                for ( int i = 0; i < this->length; i++ )
                    this->list[ i ] = T( cslist.list[ i ] );
        }
        return *this;
    }

    /**
     * Finds specified element with binary search
     * @param needle Item to be searched for
     * @param searchForInsertionPos instead of searching real position, we search insertion point
     */
    int Find( T needle, bool searchForInsertionPos = false ) const
    {
        int from = 0, to = this->length > 0 ? this->length - 1 : 0, midpoint = 0;

        // When the list is empty
        if ( this->length == 0 ) return searchForInsertionPos ? 0 : this->ITEM_NOT_FOUND;

        // While the interval isn't empty
        while ( searchForInsertionPos ? to > from : to >= from )
        {
            // Calculate the midpoint and item stored at midpoint position
            midpoint = this->getmidpoint( from, to );
            T mpItem = this->At( midpoint );

            // Did we find the item?
            if ( pointers ? *mpItem == *needle : mpItem == needle )
                return searchForInsertionPos ? midpoint + 1 : midpoint;

                // Otherwise where are we searching further?
            else if ( pointers ? *mpItem < *needle : mpItem < needle )
                from = midpoint + 1;
            else
                to = midpoint - 1;
        }

        // Item not found...
        return searchForInsertionPos ? (
                ( pointers ? *needle > *this->At( from ) : needle > this->At( from ) )
                ? from + 1 : from ) : this->ITEM_NOT_FOUND;
    }

    /**
     * Insert new item into sorted list
     * @param item
     */
    bool Insert( T item )
    {
        // If list unique
        // todo: second check of uniqueness ( in AddCar, AddOwner it's before this )
        int it = Find( item );
        if ( isUnique && it != this->ITEM_NOT_FOUND ) return false;

        // Get insertion position
        int pos = Find( item, true );

        if ( pos < 0 || pos > this->length )
            throw IndexOutOfBoundaryException();

        this->ShiftArrayRight( pos );
        this->list[ pos ] = item;

        return true;
    }

protected:
    bool isUnique, pointers;

};

class CCar;

/**
 * @class CCarList
 * @brief List of cars with iterator
 */
class CCarList : public CSortedList<CCar*>
{
public:

    virtual CCarList & operator=( const CCarList & cslist );

    /**
     * @return Iterated car's RZ
     */
    const char * RZ( void );
};

/**
 * @class COwner
 * @brief Instance of owner
 */
class COwner
{
public:
    COwner()
    {
        this->name = "";
        this->surname = "";
        this->cars = new CCarList();
    }

    COwner( const char * name, const char * surname )
    {
        this->name = name;
        this->surname = surname;
        this->cars = new CCarList();
    }

    COwner( const COwner & own )
    {
        this->name = CStringFuncs::makeCopy( own.name );
        this->surname = CStringFuncs::makeCopy( own.surname );
        this->cars = new CCarList();

        for ( int i = 0; i < own.cars->length; i++ )
            this->cars->Insert( own.cars->At( i ) );
    }

    COwner & operator=( const COwner & own )
    {
        if ( &own != this )
        {
            this->name = CStringFuncs::makeCopy( own.name );
            this->surname = CStringFuncs::makeCopy( own.surname );
            this->cars = new CCarList();

            for ( int i = 0; i < own.cars->length; i++ )
                this->cars->Insert( own.cars->At( i ) );
        }
            return *this;
    }

    friend bool operator==( const COwner & lhs, const COwner & rhs) { return CStringFuncs::areEqual( lhs.name, rhs.name ) && CStringFuncs::areEqual( lhs.surname, rhs.surname); }
    friend bool operator!=( const COwner & lhs, const COwner & rhs) { return !operator==( lhs, rhs ); }
    friend bool operator<( const COwner & lhs, const COwner & rhs)
    {
        return lhs.name == rhs.name ? CStringFuncs::smallerThan( lhs.surname, rhs.surname ) : CStringFuncs::smallerThan( lhs.name, rhs.name );
    }
    friend bool operator>( const COwner & lhs, const COwner & rhs ) { return operator<( rhs, lhs ); }
    friend bool operator<=( const COwner & lhs, const COwner & rhs ) { return !operator>( lhs, rhs ); }
    friend bool operator>=( const COwner & lhs, const COwner & rhs ) { return !operator<( lhs, rhs ); }

    friend ostream & operator<<( ostream & os, const COwner & obj )
    {
        os << "COwner " << &obj << ": ( " << obj.name << ", " << obj.surname << " ) )";
        return os;
    }

    const char * name, * surname;
    CCarList * cars;
};

/**
* @class COwnerList
* @brief List of car owners (in correct historical sorting)
*/
class COwnerList : public CList<COwner*>
{
public:

    /**
     * @return Iterated owner's name
     */
    const char * Name( void )
    {
        return list[ position ]->name;
    }
    /**
     * @return Iterated owner's surname
     */
    const char * Surname( void )
    {
        return list[ position ]->surname;
    }
};

/**
* @class COwnerList
* @brief List of car owners (in correct historical sorting)
*/
class CSortedOwnerList : public CSortedList<COwner*>
{
public:

    /**
     * @return Iterated owner's name
     */
    const char * Name( void )
    {
        return list[ position ]->name;
    }
    /**
     * @return Iterated owner's surname
     */
    const char * Surname( void )
    {
        return list[ position ]->surname;
    }
};

/**
 * @class CCar
 * @brief Instance of car
 */
class CCar
{
    public:
        CCar()
        {
            this->rz = "";
            this->owner = nullptr;
            this->ownerHistory = new COwnerList();
        }

        CCar( const char * rz, COwner * owner )
        {
            this->rz = rz;
            this->owner = owner;
            this->ownerHistory = new COwnerList();
            ownerHistory->Insert( owner, ownerHistory->length );
        }

        CCar( const CCar & car )
        {
            this->rz = CStringFuncs::makeCopy( car.rz );
            this->owner = car.owner;
            this->ownerHistory = new COwnerList();

            for ( int i = 0; i < car.ownerHistory->length; i++ )
                this->ownerHistory[ i ] = car.ownerHistory[ i ];
        }

        CCar & operator=( const CCar & car )
        {
            if ( &car != this )
            {
                rz = CStringFuncs::makeCopy( car.rz );
                owner = car.owner;
                ownerHistory = new COwnerList();

                for ( int i = 0; i < car.ownerHistory->length; i++ )
                    ownerHistory[ i ] = car.ownerHistory[ i ];
            }
            return *this;
        }

        void TransferTo( COwner * newOwner )
        {
            ownerHistory->Insert( newOwner, ownerHistory->length );
            owner = newOwner;
        }

        friend bool operator==( const CCar & lhs, const CCar & rhs) { return CStringFuncs::areEqual(lhs.rz, rhs.rz); }
        friend bool operator!=( const CCar & lhs, const CCar & rhs) { return !operator==( lhs, rhs ); }
        friend bool operator<( const CCar & lhs, const CCar & rhs)
        {
            return CStringFuncs::smallerThan( lhs.rz, rhs.rz );
        }
        friend bool operator>( const CCar & lhs, const CCar & rhs ) { return operator<( rhs, lhs ); }
        friend bool operator<=( const CCar & lhs, const CCar & rhs ) { return !operator>( lhs, rhs ); }
        friend bool operator>=( const CCar & lhs, const CCar & rhs ) { return !operator<( lhs, rhs ); }

        friend ostream & operator<<( ostream & os, const CCar & obj )
        {
            os << " CCar " << &obj << ": ( " << obj.rz << ", " << *obj.owner << " )";
            return os;
        }

        const char * rz;
        COwner *owner;
        COwnerList *ownerHistory;
};


const char * CCarList::RZ( void )
{
    return list[ position ]->rz;
}

CCarList & CCarList::operator=( const CCarList & cslist )
{
    if ( &cslist != this )
    {
        this->length = cslist.length;
        this->max = cslist.max;
        this->list = new CCar*[ this->max ];
        this->isUnique = cslist.isUnique;
        this->pointers = cslist.pointers;

        for ( int i = 0; i < this->length; i++ )
            this->list[ i ] = new CCar( *cslist.list[ i ] );
    }
    return *this;
}

/**
* @class CRegister
* @brief Main class of the car register
*/
class CRegister
{
    public:
        CRegister()
        {
            this->cars = CCarList( );
            this->owners = CSortedOwnerList( );
        }

        CRegister( const CRegister & reg )
        {
            cout << "register ctor";
            this->cars = reg.cars;
            this->owners = reg.owners;
        }

        CRegister & operator=( const CRegister & rhs )
        {
            if ( &rhs != this )
            {
                cout << "register ctor (=em)";
                this->cars = rhs.cars;
                this->owners = rhs.owners;
            }
            return *this;
        }

        COwner * GetOrCreate( COwner * owner )
        {
            int search = owners.Find( owner );
            if ( search != owners.ITEM_NOT_FOUND )
                return owners.At( search );
            else if ( ! owners.Insert( owner ) ) return nullptr;
            return owner;
        }

        friend ostream & operator<<( ostream & os, CRegister & obj )
        {
            os << "CRegister " << &obj << ": " << endl;

            os << "  -  CCarList " << &(obj.cars) << ": " << endl;
            for ( int i = 0; i < obj.cars.length; i++ )
            {
                CCar * car = obj.cars.At( i );
                os << "     -  CCar " << car << ": " << (*car).rz << ", " << (*car).owner->name << " " << (*car).owner->surname << endl;

                os << "        -  COwnerList " << car->ownerHistory << ": " << endl;

                for ( int o = 0; o < car->ownerHistory->length; o ++)
                {
                    os << "           -  COwner " << car->ownerHistory->At( o ) << ": " << car->ownerHistory->At( o )->name << " " << car->ownerHistory->At( o )->surname << endl;
                }
            }

            os << "  -  COwnerList " << &(obj.owners) << ": " << endl;
            for ( int i = 0; i < obj.owners.length; i++ )
            {
                COwner * ow = obj.owners.At( i );
                os << "     -  COwner " << ow << ": " << (*ow).name << " " << (*ow).surname << endl;
            }

            cout << endl;

            return os;
        }

        /**
        * Adds a new car into the register
        * @param rz registration identification of the new car
        * @param name name of the car's owner
        * @param surname surname of the car's owner
        * @return success
        */
        bool AddCar( const char * rz, const char * name, const char * surname )
        {
            char *nName = CStringFuncs::makeCopy( name ),
                    *nSurname = CStringFuncs::makeCopy( surname ),
                    *nRZ = CStringFuncs::makeCopy( rz );

            // Get or create new owner
            COwner * owner = this->GetOrCreate( new COwner( nName, nSurname ) );
            if ( owner == nullptr ) return false;

            // Create car
            CCar * car = new CCar( nRZ, owner );
            if ( !cars.Insert( car ) ) return false;

            // Add to owner's list
            return owner->cars->Insert( car );

        }

        /**
        * Removes a car from the register by RZ
        * @param rz registration identification of the car to remove
        * @return success
        */
        bool DelCar( const char * rz )
        {
            CCar * needle = new CCar( rz, nullptr );
            int searchResult = cars.Find( needle );
            delete needle;

            if ( searchResult != cars.ITEM_NOT_FOUND )
            {
                cars.Remove( searchResult );
                return true;
            }
            else return false;
        }

        /**
        * Transfers a car to other owner by RZ
        * @param rz registration identification of the car to transfer
        * @param nName name of the new owner
        * @param nSurname surname of the new owner
        */
        bool Transfer( const char * rz, const char * nName, const char * nSurname )
        {
            // Create string copies
            nName = CStringFuncs::makeCopy( nName );
            nSurname = CStringFuncs::makeCopy( nSurname );

            // Get or create new owner
            COwner * owner = this->GetOrCreate( new COwner( nName, nSurname ) );
            if ( owner == nullptr ) return false;

            // Find the car and change its owner
            CCar * needle = new CCar( rz, nullptr );
            int searchResult = cars.Find( needle );
            delete needle;

            if ( searchResult != cars.ITEM_NOT_FOUND )
            {
                cars.At( searchResult )->TransferTo( owner );
                return true;
            }
            else return false;
        }

        /**
        * List all cars owned by the specified person
        * @param name person name
        * @param surname person surname
        * @return list of owned cars
        */
        CCarList ListCars( const char * name, const char * surname ) const
        {
            // Find the owner
            COwner * needle = new COwner( name, surname );
            int searchResult = owners.Find( needle );
            delete needle;

            // And return his cars
            return ( (searchResult != owners.ITEM_NOT_FOUND) ? *(owners.At( searchResult )->cars) : CCarList() );
        }

        /**
        * Returns the count of owned card by specified person
        * @param name person name
        * @param surname person surname
        * @return owned cars count
        */
        int CountCars( const char * name, const char * surname ) const
        {
            // Find the owner
            COwner * needle = new COwner( name, surname );
            int searchResult = owners.Find( needle );
            delete needle;

            // And return his count of cars
            return searchResult != owners.ITEM_NOT_FOUND ? owners.At( searchResult )->cars->length : 0;
        }

        /**
        * List all owners, which owned specified car before
        * @param rz
        * @return list of owners
        */
        COwnerList ListOwners( const char * rz ) const
        {
            // Find the car
            CCar * needle = new CCar( rz, nullptr );
            int searchResult = cars.Find( needle );
            delete needle;

            // And return his count of cars
            return searchResult != cars.ITEM_NOT_FOUND ? *(cars.At( searchResult )->ownerHistory) : COwnerList();
        }

        /**
        * Returns the count of all owners, which owned specified car before
        * @param rz
        * @return owners count
        */
        int CountOwners( const char * rz ) const
        {
            // Find the car
            CCar * needle = new CCar( rz, nullptr );
            int searchResult = cars.Find( needle );
            delete needle;

            // And return his count of cars
            CCar * found = cars.At( searchResult );
            return searchResult != cars.ITEM_NOT_FOUND ? found->ownerHistory->length : 0;
        }

    private:
        CCarList cars;
        CSortedOwnerList owners;
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