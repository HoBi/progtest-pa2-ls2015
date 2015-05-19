#ifndef __PROGTEST__
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <iomanip>
using namespace std;
#endif /* __PROGTEST__ */


/**
 * Exception: Index is out of boundary
 */
class IndexOutOfBoundaryException {};

/**
 * Exception: Item has not been found
 */
class ItemNotFoundException {};

/**
 * Exception: Owner hasn't been correctly copied
 */
class OwnerNotCopiedException {};

/**
 * Shared pointer references counter
 */
struct SharedPtrRefCounter
{
public:
    /** No. of references */
    int references;

    /**
     * Constructor, will initiate with ref count 1.
     */
    SharedPtrRefCounter() { references = 1; }

    /**
     * Constructor which can be used to start from other point than 1. Specified as parameter.
     * @param start ref count starting point
     */
    SharedPtrRefCounter( int start ) { references = start; }
};

/**
 * Custom class for shared pointer
 */
template <typename T> class SharedPtr
{
public:
    /**
     * Implicit constructor
     * */
    SharedPtr() : ptr( 0 ), refCounter( 0 )
    {
        refCounter = new SharedPtrRefCounter();
        isZero = true;
    }

    /**
     * Constructs shared pointer from regular pointer
     * @param ptr source pointer
     */
    SharedPtr( T * ptr ) : ptr( ptr ), refCounter( 0 )
    {
        refCounter = new SharedPtrRefCounter();
        isZero = false;
    }

    /** Destructor */
    virtual ~SharedPtr()
    {
        if ( (--refCounter->references) == 0 )
        {
            delete ptr;
            delete refCounter;
        }
    }

    /** Copy constructor */
    SharedPtr( const SharedPtr<T> & sptr ) : ptr( sptr.ptr ), refCounter( sptr.refCounter )
    {
        refCounter->references++;
    }

    /** Assigment operator */
    SharedPtr & operator=( const SharedPtr & ptr )
    {
        if ( this != &ptr )
        {
            if ( (--refCounter->references) == 0 )
            {
                delete this->ptr;
                delete refCounter;
            }

            this->ptr = ptr.ptr;
            refCounter = ptr.refCounter;
            refCounter->references++;
        }
        return *this;
    }

    /** Equality operator */
    const SharedPtr & operator=( const SharedPtr & ptr ) const
    {
        if ( this != &ptr )
        {
            if ( (--refCounter->references) == 0 ) { delete ptr; delete refCounter; }

            ptr = ptr.ptr;
            refCounter = ptr.refCounter;
            refCounter->references++;
        }
        return *this;
    }

    /** Dereferencing -> operator */
    T* operator->()
    {
        return ptr;
    }

    /** Constant dereferencing operator */
    const T* operator->() const
    {
        return ptr;
    }

    /** Dereferencing * operator */
    T& operator*()
    {
        return *ptr;
    }

    /**
     * Make detached copy of content
     * @return new shared pointer
     */
    SharedPtr & Detach()
    {
        T * nPtr = new T( *ptr );
        if ( (--refCounter->references) == 0 ) { delete ptr; delete refCounter; }
        refCounter = new SharedPtrRefCounter();
        ptr = nPtr;

        return *this;
    }

    /**
     * Make detached copy of content only if this pointer is shared
     * @return new or old shared pointer
     */
    SharedPtr & DetachIfShared()
    {
        if ( refCounter->references > 1 )
        {
            refCounter->references--;
            ptr = new T( *ptr );
            refCounter = new SharedPtrRefCounter();
        }

        return *this;
    }

    /** Comparison operator */
    bool operator==( const SharedPtr & rhs ) const
    {
        return ( isZero == rhs.isZero || ptr == rhs.ptr );
    }

    /** Comparison operator */
    bool operator<( const SharedPtr & rhs ) const { return ptr < rhs.ptr; }
    /** Comparison operator */
    bool operator>( const SharedPtr & rhs ) const { return ptr > rhs.ptr; }

private:
    T* ptr;
    SharedPtrRefCounter* refCounter;
    bool isZero;
};

/**
 * Shared pointer factory
 */
class SharedPtrFactory
{
public:
    /**
     * Create new shared pointer
     * @param object object as content
     */
    template <class T> static SharedPtr<T> create( T* object )
    {
        return SharedPtr<T>( object );
    }
};

/**
 * Utilities for string manipulation
 */
class CStringFuncs
{
public:
    /**
     * Checks if lhs is smaller than rhs
     * @param lhs left side
     * @param rhs right side
     */
    static bool smallerThan( const char * lhs, const char * rhs )
    {
        // todo: může to bejt na progtestu zakázaný
        return strcmp( lhs, rhs ) < 0;
    }

    /**
     * Checks if the specified strings are equal
     * @param lhs left side
     * @param rhs right side
     */
    static bool areEqual( const char * lhs, const char * rhs )
    {
        // todo: může to bejt na progtestu zakázaný
        return strcmp( lhs, rhs ) == 0;
    }

    /**
     * Make deep copy of string
     * @param from source
     */
    static char * makeCopy( const char * from )
    {
        int length = strlen( from ) + 1;
        char * nStr = new char[ length ];
        strcpy( nStr, from );
        return nStr;
    }
};

/**
 * Simple list
 */
template <class T> class CList
{
public:
    /** Item not found constant */
    static const int ITEM_NOT_FOUND = -1;
    /** List length */
    int length;

    /** Implicit constructor */
    CList()
    {
        length = 0;
        max = 64;
        isUnique = false;
        position = 0;
    }

    /**
     * Constructor
     * @param size list size
     */
    CList( int size )
    {
        length = size;
        max = size + 8;
        isUnique = false;
        position = 0;
    }

    /** Copy constructor */
    CList( const CList & list )
    {
        this->length = list.length;
        this->max = list.max;
        this->isUnique = false;
        this->position = 0;
    }

    /** Destructor */
    virtual ~CList()
    {
        delete[] list;
    }

    /** Assignment operator */
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
     * Set value of list item on specified index
     * @param index index
     * @param item new value
     */
    void Set( int index, T item )
    {
        if ( index < 0 || index >= length )
            throw IndexOutOfBoundaryException();

        list[ index ] = item;
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

    /**
     * Reset iterator to starting position
     */
    void Reset( void )
    {
        position = 0;
    }

protected:
    /** List of items */
    T* list;
    /** Unique flag */
    bool isUnique;
    /** Actual list maximum size (dynamic) */
    int max;
    /** Iterator actual position */
    int position;

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
     * @return midpoint
     */
    int getmidpoint( int from, int to ) const
    {
        return ( to - from ) / 2 + from;
    }
};

/**
 * Sorted list
 */
template <class T> class CSortedList : public CList<T>
{
public:

    /**
     * Constructor
     * @param unique Flag to forcing all items to be unique in the list
     * @param pointers Are values in list pointers?
     */
    CSortedList( bool unique = true, bool pointers = false )
    {
        // First allocation of dynamic array
        this->list = new T[ this->max ];
        this->isUnique = unique;
        this->pointers = pointers;
    }

    /** Copy constructor */
    CSortedList( const CSortedList & list )
    {
        this->list = new T[ list.max ];
        this->isUnique = list.isUnique;
        this->pointers = list.pointers;
        this->length = list.length;

        for ( int i = 0; i < list.length; i++ )
            this->list[ i ] = list.At( i );
    }

    /** Assigment operator */
    virtual CSortedList & operator=( const CSortedList & cslist )
    {
        if ( &cslist != this )
        {
            this->length = cslist.length;
            this->max = cslist.max;
            this->list = new T[ this->max ];
            this->isUnique = cslist.isUnique;
            this->pointers = cslist.pointers;
            this->length = cslist.length;

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
            T mpItem = this->list[ midpoint ];

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
    /** Unique flag */
    bool isUnique;

    /** Pointers flag */
    bool pointers;

};

class CCar;
typedef SharedPtr<CCar> CCarSPtr;

/**
 * List of cars with iterator
 */
class CCarList : public CSortedList<CCarSPtr>
{
public:

    /** Implicit constructor */
    CCarList() : CSortedList<CCarSPtr>( true, true )
    {

    }

    virtual CCarList & operator=( const CCarList & cslist );

    /**
     * @return Iterated car's RZ
     */
    const char * RZ( void );
};
typedef SharedPtr<CCarList> CCarListSPtr;

/**
 * @class COwner
 * @brief Instance of owner
 */
class COwner
{
public:
    /** Implicit constructor */
    COwner()
    {
        this->name = "";
        this->surname = "";
        this->cars = CCarListSPtr( new CCarList() );
    }

    /**
     * Constructor
     * @param name owner name
     * @param surname owner surname
     */
    COwner( const char * name, const char * surname )
    {
        this->name = name;
        this->surname = surname;
        this->cars = CCarListSPtr( new CCarList() );
    }

    /** Copy constructor */
    COwner( const COwner & own )
    {
        this->name = CStringFuncs::makeCopy( own.name );
        this->surname = CStringFuncs::makeCopy( own.surname );
        this->cars = CCarListSPtr( new CCarList() );

        for ( int i = 0; i < own.cars->length; i++ )
            this->cars->Insert( own.cars->At( i ) );
    }

    /** Destructor */
    ~COwner()
    {
        delete[] name;
        delete[] surname;
    }

    /** Assignment operator */
    COwner & operator=( const COwner & own )
    {
        if ( &own != this )
        {
            this->name = CStringFuncs::makeCopy( own.name );
            this->surname = CStringFuncs::makeCopy( own.surname );
            this->cars = CCarListSPtr( new CCarList() );

            for ( int i = 0; i < own.cars->length; i++ )
                this->cars->Insert( own.cars->At( i ) );
        }
            return *this;
    }

    /** Equality operator */
    friend bool operator==( const COwner & lhs, const COwner & rhs) { return CStringFuncs::areEqual( lhs.name, rhs.name ) && CStringFuncs::areEqual( lhs.surname, rhs.surname); }
    /** Comparison operator */
    friend bool operator!=( const COwner & lhs, const COwner & rhs) { return !operator==( lhs, rhs ); }
    /** Comparison operator */
    friend bool operator<( const COwner & lhs, const COwner & rhs)
    {
        return CStringFuncs::areEqual( lhs.name, rhs.name ) ? CStringFuncs::smallerThan( lhs.surname, rhs.surname ) : CStringFuncs::smallerThan( lhs.name, rhs.name );
    }
    /** Comparison operator */
    friend bool operator>( const COwner & lhs, const COwner & rhs ) { return operator<( rhs, lhs ); }
    /** Comparison operator */
    friend bool operator<=( const COwner & lhs, const COwner & rhs ) { return !operator>( lhs, rhs ); }
    /** Comparison operator */
    friend bool operator>=( const COwner & lhs, const COwner & rhs ) { return !operator<( lhs, rhs ); }

    /** Output operator */
    friend ostream & operator<<( ostream & os, const COwner & obj )
    {
        os << "COwner " << &obj << ": ( " << obj.name << ", " << obj.surname << " ) )";
        return os;
    }

    /** Owner's name */
    const char * name;
    /** Owner's surname */
    const char * surname;
    /** List of owner's cars */
    CCarListSPtr cars;
};
typedef SharedPtr<COwner> COwnerSPtr;

/**
* List of car owners (in correct historical sorting)
*/
class COwnerList : public CList<COwnerSPtr>
{
public:
    /** Implicit constructor */
    COwnerList() : CList<COwnerSPtr>()
    {
        this->list = new COwnerSPtr[ this->max ];
    }

    /** Copy constructor */
    COwnerList( const COwnerList & clist ) : CList<COwnerSPtr>( clist )
    {
        this->list = new COwnerSPtr[ this->length ];

        for ( int i = 0; i < this->length; i++ )
            this->list[ i ] = clist.At( i );
    }

    /**
     * Constructor with specified list size
     * @param size list size
     */
    COwnerList( int size ) : CList<COwnerSPtr>( size )
    {
        this->list = new COwnerSPtr[ size + 8 ];
    }

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
typedef SharedPtr<COwnerList> COwnerListSPtr;

/**
* List of car owners (in correct historical sorting)
*/
class CSortedOwnerList : public CSortedList<COwnerSPtr>
{
public:
    /** Implicit constructor */
    CSortedOwnerList() : CSortedList<COwnerSPtr>( true, true )
    {
        // empty corr
    }

    /** Copy constructor */
    CSortedOwnerList( const CSortedOwnerList & cslist) : CSortedList<COwnerSPtr>( cslist )
    {
        // empty corr
    }

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

    /** Assignment operator */
    CSortedOwnerList & operator=( const CSortedOwnerList & cslist )
    {
        if ( &cslist != this )
        {
            this->length = cslist.length;
            this->max = cslist.max;
            this->list = new COwnerSPtr[ this->max ];
            this->isUnique = cslist.isUnique;
            this->pointers = cslist.pointers;

            for ( int i = 0; i < this->length; i++ )
                this->list[ i ] = COwnerSPtr( cslist.list[ i ] );
        }
        return *this;
    }
};
typedef SharedPtr<CSortedOwnerList> CSortedOwnerListSPtr;

/**
 * Instance of car
 */
class CCar
{
    public:
        /** Implicit constructor */
        CCar()
        {
            this->rz = "";
            this->owner = nullptr;
            this->ownerHistory = COwnerListSPtr( new COwnerList() );
        }

        /**
         * Creates new car with specified RZ and owner
         * @param rz registration identification
         * @param owner car's owner
         */
        CCar( const char * rz, COwnerSPtr owner )
        {
            this->rz = rz;
            this->owner = owner;
            this->ownerHistory = COwnerListSPtr( new COwnerList() );
            ownerHistory->Insert( owner, 0 );
        }

        /**
         * @param rz registration idenfitication
         */
        CCar( const char * rz )
        {
            this->rz = rz;
        }

        /** Copy constructor */
        CCar( const CCar & car )
        {
            this->rz = CStringFuncs::makeCopy( car.rz );
            this->owner = car.owner;
            this->ownerHistory = COwnerListSPtr( new COwnerList( car.ownerHistory->length ) );

            for ( int i = 0; i < car.ownerHistory->length; i++ )
                this->ownerHistory->Set( i, car.ownerHistory->At( i ) );
        }

        /** Assignment operator */
        CCar & operator=( const CCar & car )
        {
            if ( &car != this )
            {
                rz = CStringFuncs::makeCopy( car.rz );
                owner = car.owner;
                ownerHistory = COwnerListSPtr( new COwnerList() );

                for ( int i = 0; i < car.ownerHistory->length; i++ )
                    this->ownerHistory->Set( i, car.ownerHistory->At( i ) );
            }
            return *this;
        }

        /**
         * Transfers the car to another owner
         * @param newOwner new car owner
         */
        void TransferTo( COwnerSPtr newOwner )
        {
            ownerHistory->Insert( newOwner, 0 );
            owner = newOwner;
        }

        /** Equality operator */
        friend bool operator==( const CCar & lhs, const CCar & rhs) { return CStringFuncs::areEqual(lhs.rz, rhs.rz); }
        /** Comparison operator */
        friend bool operator!=( const CCar & lhs, const CCar & rhs) { return !operator==( lhs, rhs ); }
        /** Comparison operator */
        friend bool operator<( const CCar & lhs, const CCar & rhs)
        {
            return CStringFuncs::smallerThan( lhs.rz, rhs.rz );
        }
        /** Comparison operator */
        friend bool operator>( const CCar & lhs, const CCar & rhs ) { return operator<( rhs, lhs ); }
        /** Comparison operator */
        friend bool operator<=( const CCar & lhs, const CCar & rhs ) { return !operator>( lhs, rhs ); }
        /** Comparison operator */
        friend bool operator>=( const CCar & lhs, const CCar & rhs ) { return !operator<( lhs, rhs ); }

        /** Registration idenfiticator */
        const char * rz;

        /** Car owner */
        COwnerSPtr owner;

        /** History of car ownership */
        COwnerListSPtr ownerHistory;
};

/**
 * Prints car's RZ
 */
const char * CCarList::RZ( void )
{
    return list[ position ]->rz;
}

/** Assignment operator */
CCarList & CCarList::operator=( const CCarList & cslist )
{
    if ( &cslist != this )
    {
        this->length = cslist.length;
        this->max = cslist.max;
        this->list = new CCarSPtr[ this->max ];
        this->isUnique = cslist.isUnique;
        this->pointers = cslist.pointers;

        for ( int i = 0; i < this->length; i++ )
            this->list[ i ] = CCarSPtr( cslist.list[ i ] );
    }
    return *this;
}

/**
* Main class of the car register
*/
class CRegister
{
    public:
        /** Implicit constructor */
        CRegister()
        {
            this->cars = CCarListSPtr( new CCarList() );
            this->owners = CSortedOwnerListSPtr( new CSortedOwnerList() );
            sharedCarList = sharedOwnerList = false;
        }

        /** Copy constructor */
        CRegister( CRegister & reg )
        {
            this->owners = reg.owners;
            this->cars = reg.cars;

            sharedCarList = sharedOwnerList = true;
            reg.sharedCarList = reg.sharedOwnerList = true;
        }

        /** Assignment operator */
        CRegister & operator=( CRegister & rhs )
        {
            if ( &rhs != this )
            {
                this->owners = rhs.owners;
                this->cars = rhs.cars;

                sharedCarList = sharedOwnerList = true;
                rhs.sharedCarList = rhs.sharedOwnerList = true;
            }
            return *this;
        }

        /**
         * Search for owner in database. If found - returns pointer to it's instance.
         * Otherwise it will create a new owner, put it into database and return appropiate pointer to instance
         * @return pointer to owner
         */
        COwnerSPtr GetOrCreateOwner( COwnerSPtr owner ) {
            // Search owner
            int search = owners->Find( owner );

            // If found, return existing owner
            if ( search != -1 )
            {
                COwnerSPtr rOwner = owners->At( search );
                COwnerSPtr newOwner = rOwner.Detach();

                // Recover links
                owners->Remove( owners->Find( rOwner ) );
                for ( int i = 0; i < rOwner->cars->length; i++ )
                    rOwner->cars->At( i )->owner = newOwner;

                owners->Insert( newOwner );
                return newOwner;
            }

            // If not, clone strings and create new owner
            char *nName = CStringFuncs::makeCopy( owner->name ),
                    *nSurname = CStringFuncs::makeCopy( owner->surname );

            COwnerSPtr newOwner = COwnerSPtr( new COwner( nName, nSurname ) );
            if ( ! owners->Insert( newOwner ) ) throw OwnerNotCopiedException();

            return newOwner;
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
            // Check uniqueness
            char *nRZ = CStringFuncs::makeCopy( rz );
            char *nName = CStringFuncs::makeCopy( name );
            char *nSurname = CStringFuncs::makeCopy( surname );
            CCarSPtr car = CCarSPtr( new CCar( nRZ, COwnerSPtr( new COwner( nName, nSurname ) ) ) );
            if ( cars->Find( car ) != -1 ) { delete[] nRZ; return false; }

            // Copy register cars and owners lists if shared
            CloneRegisterListsIfNeeded();

            // Get or create new owner (already saves to this->owners, copy strings etc.)
            COwnerSPtr owner = this->GetOrCreateOwner( COwnerSPtr( new COwner( name, surname ) ) );

            car->owner = owner;

            // Create the car
            cars->Insert( car );

            // Add to owner's list
            owner->cars->Insert( car );

            return true;
        }

        /**
        * Removes a car from the register by RZ
        * @param rz registration identification of the car to remove
        * @return success
        */
        bool DelCar( const char * rz )
        {
            int carOwnerIdx;

            // Copy register cars and owners lists if shared
            CloneRegisterListsIfNeeded();

            // Search for the car
            CCarSPtr needle = CCarSPtr( new CCar( rz, COwnerSPtr() ) );
            int searchResult = cars->Find( needle );

            // If found, get owner
            if ( searchResult != -1 )
            {
                carOwnerIdx = owners->Find( cars->At( searchResult )->owner );
            }
            else return false;

            // Make car owner copy
            if ( carOwnerIdx != COwnerList::ITEM_NOT_FOUND )
                owners->Set( carOwnerIdx, owners->At( carOwnerIdx ).Detach() );
            else throw ItemNotFoundException();

            // Remove the car from owner's carlist
            int carIdx = owners->At( carOwnerIdx )->cars->Find( cars->At( searchResult ) );
            owners->At( carOwnerIdx )->cars->Remove( carIdx );

            // Remove the car from the register carlist
            cars->Remove( searchResult );

            return true;
        }

        /**
        * Transfers a car to other owner by RZ
        * @param rz registration identification of the car to transfer
        * @param nName name of the new owner
        * @param nSurname surname of the new owner
        * @return success
        */
        bool Transfer( const char * rz, const char * nName, const char * nSurname )
        {
            // Copy register cars and owners lists if shared
            CloneRegisterListsIfNeeded();

            // Find the car
            CCarSPtr needle = CCarSPtr( new CCar( rz ) );
            int searchResult = cars->Find( needle );

            // If car doesn't exists
            if ( searchResult == CCarList::ITEM_NOT_FOUND ) return false;

            // Or trying to transfer to the same owner
            if ( *(cars->At( searchResult )->owner) == COwner( nName, nSurname ) ) return false;

            // Get or create new owner (already saves to this->owners, copy strings etc.)
            COwnerSPtr oldOwner = this->GetOrCreateOwner( cars->At( searchResult )->owner );
            COwnerSPtr newOwner = this->GetOrCreateOwner( new COwner( nName, nSurname ) );

            // Delete from old owner's carlist
            int carToRemove = oldOwner->cars->Find( cars->At( searchResult ) );
            if ( carToRemove != -1 ) oldOwner->cars->Remove( carToRemove );

            // Detach the car ( make copy )
            if ( searchResult != CCarList::ITEM_NOT_FOUND )
                cars->Set( searchResult, cars->At( searchResult ).Detach() );
            else return false;

            // Add to new owner's carlist
            newOwner->cars->Insert( cars->At( searchResult ) );

            cars->At( searchResult )->TransferTo( newOwner );

            newOwner->cars->Insert( cars->At( searchResult ) );

            return true;
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
            COwnerSPtr needle = COwnerSPtr( new COwner( name, surname ) );
            int searchResult = owners->Find( needle );

            // And return his cars
            return ( (searchResult != -1 ) ? *(owners->At( searchResult )->cars) : CCarList() );
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
            COwnerSPtr needle = COwnerSPtr( new COwner( name, surname ) );
            int searchResult = owners->Find( needle );

            // And return his count of cars
            return searchResult != -1 ? owners->At( searchResult )->cars->length : 0;
        }

        /**
        * List all owners, which owned specified car before
        * @param rz
        * @return list of owners
        */
        COwnerList ListOwners( const char * rz ) const
        {
            // Find the car
            CCarSPtr needle = CCarSPtr( new CCar( rz ) );
            int searchResult = cars->Find( needle );

            // Reset list counter and return the owner list
            if ( searchResult != -1 )
            {
                cars->At( searchResult )->ownerHistory->Reset();
                return *(cars->At( searchResult )->ownerHistory);
            }
            else
                return COwnerList();
        }

        /**
        * Returns the count of all owners, which owned specified car before
        * @param rz
        * @return owners count
        */
        int CountOwners( const char * rz ) const
        {
            // Find the car
            CCarSPtr needle = CCarSPtr( new CCar( rz ) );
            int searchResult = cars->Find( needle );

            // And return his count of cars
            if ( searchResult != -1 )
            {
                CCarSPtr found = cars->At( searchResult );
                return found->ownerHistory->length;
            } else return 0;
        }

    private:
        CCarListSPtr cars;
        CSortedOwnerListSPtr owners;
        bool sharedCarList, sharedOwnerList;

        void CloneRegisterListsIfNeeded()
        {
            if ( sharedOwnerList )
                owners.Detach();
            if ( sharedCarList )
                cars.Detach();

            sharedOwnerList = sharedCarList = false;
        }
};

#ifndef __PROGTEST__
int main ( int argc, char *argv[] )
{
    // Dummy args for BI-TED
    if ( argc > 1 ) return 1;
    else if ( argc == 1 )
    {
        if ( argv[0] == "--wow" ) cout << "such command";
        if ( argv[0] == "--woof" ) cout << "woof woof!";
        if ( argv[0] == "--nothing" ) cout << "nothing (that was tricky, right?)";
        if ( argv[0] == "--abcd" ) cout << "efgh";
    }

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

    assert ( b2 . CountOwners ( "AAA-AA-AA" ) == 0 );                         // <<<<<<<< record :(((
    for ( COwnerList l = b2 . ListOwners ( "AAA-AA-AA" ); ! l . AtEnd (); l . Next () )
        cout << l . Surname () << ", " << l . Name () << endl;
    // the following 0 owners in that order:
    return 0;
}
#endif /* __PROGTEST__ */