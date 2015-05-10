#ifndef __PROGTEST__
#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <list>
#include <algorithm>
#include <memory>
using namespace std;
#endif /* __PROGTEST__ */

class WrongSizeSpecifiedException {};

class CCell
{
public:
    static const int ALIGN_LEFT = 0, ALIGN_RIGHT = 1, ALIGN_CENTER = 2;

    virtual ~CCell()
    {

    };
    virtual CCell *clone() const = 0;

    virtual string render( unsigned int line, unsigned int outWidth, unsigned int outHeight ) const = 0;

    unsigned int width, height;
    int align;
};

class CEmpty : public CCell
{
public:
    CEmpty()
    {
        width = height = 0;
        align = ALIGN_LEFT;
    }

    CEmpty( const CEmpty &obj )
    {
        align = obj.align;
        width = obj.width;
        height = obj.height;
    }

    CEmpty *clone() const
    {
        return new CEmpty( *this );
    }

    string render( unsigned int line, unsigned int outWidth, unsigned int outHeight ) const
    {
        // Make sure the size specified is correct
        if ( outWidth < width || outHeight < height )
            throw WrongSizeSpecifiedException();

        return string( outWidth, ' ' ) ;
    }

protected:
};

class CText : public CCell
{
public:
    CText( const string &content, const int align = ALIGN_LEFT )
    {
        stringstream ss;
        string buf;

        ss.str( content );

        width = height = 0;

        while ( ! ss.eof() )
        {
            getline( ss, buf );
            InsertRow( buf );
        }

        this->align = align;
    }

    CText( const CText &obj )
    {
        for ( string *line : obj.content )
        {
            content.insert( content.end( ), new string( *line ) );
        }
        align = obj.align;
        width = obj.width;
        height = obj.height;
    }

    CText *clone() const
    {
        return new CText( *this );
    }

    ~CText()
    {
        for ( string *line : content )
            delete line;
    }

    CText &SetText( const string &content )
    {
        stringstream ss;
        string buf;

        ss.str( content );

        width = height = 0;

        for ( string *line : this->content )
            delete line;

        this->content.clear();

        while ( ! ss.eof() )
        {
            getline( ss, buf );
            InsertRow( buf );
        }

        return *this;
    }

    string render( unsigned int line, unsigned int outWidth, unsigned int outHeight ) const
    {
        unsigned int marginTop, marginLeft;

        // Make sure the size specified is correct
        if ( outWidth < width || outHeight < height )
            throw WrongSizeSpecifiedException();

        // todo: this is prolly wrong, this is gonna be computed every render call

        // Compute top margin
        marginTop = align == ALIGN_CENTER ? ( outHeight - height ) / 2 : 0;

        // Check if this isn't just blank line
        if ( line < marginTop || line >= marginTop + height ) return string( outWidth, ' ' );

        int contentWidth = content.at( line - marginTop )->length();

        // Compute left margin
        marginLeft = ( align == ALIGN_CENTER ? ( outWidth - contentWidth ) / 2 : ( align == ALIGN_RIGHT ? outWidth - contentWidth : 0 ) );

        return string( marginLeft, ' ' ) + *(content.at( line - marginTop )) + string( outWidth - ( marginLeft + contentWidth ), ' ' );
    }

    vector<string*> content;

protected:
    void InsertRow( const string &row )
    {
        content.insert( content.end(), new string( row ) );

        unsigned int nwidth = row.length();

        if ( nwidth > width ) width = nwidth;
        height++;
    }
};

class CImage : public CCell
{
public:
    CImage()
    {
        align = ALIGN_CENTER;
        width = height = 0;
    }

    CImage( const CImage &obj )
    {
        for ( string *line : obj.content )
        {
            content.insert( content.end( ), new string( *line ) );
        }
        align = obj.align;
        width = obj.width;
        height = obj.height;
    }

    ~CImage()
    {
        for ( string *line : content )
            delete line;
    }

    CImage *clone() const
    {
        return new CImage( *this );
    }

    CImage &AddRow( const string &row )
    {
        InsertRow( row );
        return *this;
    }

    string render( unsigned int line, unsigned int outWidth, unsigned int outHeight ) const
    {
        unsigned int marginTop, marginLeft;

        // Make sure the size specified is correct
        if ( outWidth < width || outHeight < height )
            throw WrongSizeSpecifiedException();

        // todo: this is prolly wrong, this is gonna be computed every render call

        // Compute top margin
        marginTop = align == ALIGN_CENTER ? ( outHeight - height ) / 2 : 0;

        // Check if this isn't just blank line
        if ( line < marginTop || line >= marginTop + height ) return string( outWidth, ' ' );

        int contentWidth = content.at( line - marginTop )->length();

        // Compute left margin
        marginLeft = ( align == ALIGN_CENTER ? ( outWidth - contentWidth ) / 2 : ( align == ALIGN_RIGHT ? outWidth - contentWidth : 0 ) );

        return string( marginLeft, ' ' ) + *(content.at( line - marginTop )) + string( outWidth - ( marginLeft + contentWidth ), ' ' );
    }

    vector<string*> content;

protected:
    void InsertRow( const string &row )
    {
        content.insert( content.end(), new string( row ) );

        unsigned int nwidth = row.length();

        if ( nwidth > width ) width = nwidth;
        height++;
    }
};

class CTable
{
public:
    CTable( unsigned int rows, unsigned int cols ) : rows( rows ), cols ( cols )
    {
        cells = new CCell*[ rows * cols ];
        rowsHeight = new unsigned int[ rows ];
        colsWidth = new unsigned int[ cols ];

        for( unsigned int i = 0; i < rows * cols; i++ ) cells[ i ] = new CEmpty();
    }

    CTable ( const CTable &obj ) : rows( obj.rows ), cols ( obj.cols ), width( obj.width ), height( obj.height )
    {
        rowsHeight = new unsigned int[ rows ];
        for ( int i = 0; i < rows; i++ ) rowsHeight[ i ] = obj.rowsHeight[ i ];
        colsWidth = new unsigned int[ cols ];
        for ( int i = 0; i < cols; i++ ) colsWidth[ i ] = obj.colsWidth[ i ];

        cells = new CCell*[ rows*cols ];
        for ( int i = 0; i < rows*cols; i++ )
            cells[ i ] = obj.cells[ i ]->clone();
    }

    ~CTable()
    {
        for ( int i = 0; i < rows*cols; i++ )
            delete cells[ i ];

        delete[] cells;

        delete[] rowsHeight;
        delete[] colsWidth;
    }

    CTable &operator=( CTable rhs )
    {
        this->swap( *this, rhs );
        return *this;
    }

    CTable &SetCell( unsigned int row, unsigned int col, const CCell &newContent )
    {
        CCell *clone = newContent.clone();
        delete cells[ row * cols + col ];
        cells[ row * cols + col ] = clone;
        return *this;
    }

    CCell &GetCell( unsigned int row, unsigned int col ) const
    {
        return *(cells[ row * cols + col ]);
    }

    void refreshRowColsSizes()
    {
        int tablewidth = 0, tableheight = 0;

        for ( int i = 0; i < rows; i++ ) rowsHeight[ i ] = 0;
        for ( int i = 0; i < cols; i++ ) colsWidth[ i ] = 0;

        for ( int i = 0; i < rows * cols; i++ ) {
            if ( cells[ i ]->width > colsWidth[ i % cols ] )
            {
                colsWidth[ i % cols ] = cells[ i ]->width;

            }
            if ( cells[ i ]->height > rowsHeight[ i / cols ] )
            {
                rowsHeight[ i / cols ] = cells[ i ]->height;
            }
        }

        for ( int i = 0; i < cols; i++ ) tablewidth += colsWidth[ i ];
        for ( int i = 0; i < rows; i++ ) tableheight += rowsHeight[ i ];

        width = tablewidth + cols + 1;
        height = tableheight + rows + 1;
    }

    /*

    void beforeRender()
    {
        pre-renders into lines
    }

    */

    string render( unsigned int line )
    {
        // todo: this will return single line of table
        // if ( demand ) beforeRender();
        return ""; // return line
    }

    friend ostream &operator<<( ostream &os, CTable &obj )
    {
        // Refresh sizes
        obj.refreshRowColsSizes();

        // Create row divider
        string divider = string( obj.width, '-' );
        divider.replace( 0, 1, "+" );
        divider.replace( obj.width - 1, 1, "+" );
        int setat = 0;
        for ( int i = 0; i < obj.cols - 1; i++ ) { setat += obj.colsWidth[ i ] + 1; divider.replace( setat, 1, "+" ); }

        // Iterate through all rows in table
        for ( int i = 0; i < obj.rows; i++ )
        {
            os << divider << endl;

            // Iterate through lines in this row
            for ( unsigned int j = 0; j < obj.rowsHeight[ i ]; j++ )
            {
                os << "|";

                // Iterate through each column line of this row
                for ( int k = 0; k < obj.cols; k++ )
                {
                    os << obj.cells[ i * obj.cols + k ]->render( j, obj.colsWidth[ k ], obj.rowsHeight[ i ] );

                    // Col divider
                    os << "|";
                }
                os << endl;
            }
        }

        // Last divider
        os << divider << endl;

        return os;
    }

    void swap( CTable& first, CTable& second )
    {
        using std::swap;

        swap( first.rows, second.rows );
        swap( first.rowsHeight, second.rowsHeight );
        swap( first.cols, second.cols );
        swap( first.colsWidth, second.colsWidth );
        swap( first.width, second.width );
        swap( first.height, second.height );
        swap( first.cells, second.cells );
    }

private:
    CCell **cells;
    unsigned int *rowsHeight, *colsWidth;
    int rows, cols, width, height;
};

int main ( void )
{
    ostringstream oss;
    CTable t0 ( 3, 2 );
    t0 . SetCell ( 0, 0, CText ( "Hello,\n"
                                         "Hello Kitty", CText::ALIGN_LEFT ) );
    t0 . SetCell ( 1, 0, CText ( "Lorem ipsum dolor sit amet", CText::ALIGN_LEFT ) );
    t0 . SetCell ( 2, 0, CText ( "Bye,\n"
                                         "Hello Kitty", CText::ALIGN_RIGHT ) );
    t0 . SetCell ( 1, 1, CImage ()
            . AddRow ( "###                   " )
            . AddRow ( "#  #                  " )
            . AddRow ( "#  # # ##   ###    ###" )
            . AddRow ( "###  ##    #   #  #  #" )
            . AddRow ( "#    #     #   #  #  #" )
            . AddRow ( "#    #     #   #  #  #" )
            . AddRow ( "#    #      ###    ###" )
            . AddRow ( "                     #" )
            . AddRow ( "                   ## " )
            . AddRow ( "                      " )
            . AddRow ( " #    ###   ###   #   " )
            . AddRow ( "###  #   # #     ###  " )
            . AddRow ( " #   #####  ###   #   " )
            . AddRow ( " #   #         #  #   " )
            . AddRow ( "  ##  ###   ###    ## " ) );
    t0 . SetCell ( 2, 1, CEmpty () );
    oss . str ("");
    oss . clear ();
    oss << t0;
    assert ( oss . str () ==
             "+--------------------------+----------------------+\n"
                     "|Hello,                    |                      |\n"
                     "|Hello Kitty               |                      |\n"
                     "+--------------------------+----------------------+\n"
                     "|Lorem ipsum dolor sit amet|###                   |\n"
                     "|                          |#  #                  |\n"
                     "|                          |#  # # ##   ###    ###|\n"
                     "|                          |###  ##    #   #  #  #|\n"
                     "|                          |#    #     #   #  #  #|\n"
                     "|                          |#    #     #   #  #  #|\n"
                     "|                          |#    #      ###    ###|\n"
                     "|                          |                     #|\n"
                     "|                          |                   ## |\n"
                     "|                          |                      |\n"
                     "|                          | #    ###   ###   #   |\n"
                     "|                          |###  #   # #     ###  |\n"
                     "|                          | #   #####  ###   #   |\n"
                     "|                          | #   #         #  #   |\n"
                     "|                          |  ##  ###   ###    ## |\n"
                     "+--------------------------+----------------------+\n"
                     "|                      Bye,|                      |\n"
                     "|               Hello Kitty|                      |\n"
                     "+--------------------------+----------------------+\n" );
    t0 . SetCell ( 0, 1, t0 . GetCell ( 1, 1 ) );
    t0 . SetCell ( 2, 1, CImage ()
            . AddRow ( "*****   *      *  *      ******* ******  *" )
            . AddRow ( "*    *  *      *  *      *            *  *" )
            . AddRow ( "*    *  *      *  *      *           *   *" )
            . AddRow ( "*    *  *      *  *      *****      *    *" )
            . AddRow ( "****    *      *  *      *         *     *" )
            . AddRow ( "*  *    *      *  *      *        *       " )
            . AddRow ( "*   *   *      *  *      *       *       *" )
            . AddRow ( "*    *    *****   ****** ******* ******  *" ) );
    dynamic_cast<CText &> ( t0 . GetCell ( 1, 0 ) ) . SetText ( "Lorem ipsum dolor sit amet,\n"
                                                                        "consectetur adipiscing\n"
                                                                        "elit. Curabitur scelerisque\n"
                                                                        "lorem vitae lectus cursus,\n"
                                                                        "vitae porta ante placerat. Class aptent taciti\n"
                                                                        "sociosqu ad litora\n"
                                                                        "torquent per\n"
                                                                        "conubia nostra,\n"
                                                                        "per inceptos himenaeos.\n"
                                                                        "\n"
                                                                        "Donec tincidunt augue\n"
                                                                        "sit amet metus\n"
                                                                        "pretium volutpat.\n"
                                                                        "Donec faucibus,\n"
                                                                        "ante sit amet\n"
                                                                        "luctus posuere,\n"
                                                                        "mauris tellus" );
    oss . str ("");
    oss . clear ();
    oss << t0;
    assert ( oss . str () ==
             "+----------------------------------------------+------------------------------------------+\n"
                     "|Hello,                                        |          ###                             |\n"
                     "|Hello Kitty                                   |          #  #                            |\n"
                     "|                                              |          #  # # ##   ###    ###          |\n"
                     "|                                              |          ###  ##    #   #  #  #          |\n"
                     "|                                              |          #    #     #   #  #  #          |\n"
                     "|                                              |          #    #     #   #  #  #          |\n"
                     "|                                              |          #    #      ###    ###          |\n"
                     "|                                              |                               #          |\n"
                     "|                                              |                             ##           |\n"
                     "|                                              |                                          |\n"
                     "|                                              |           #    ###   ###   #             |\n"
                     "|                                              |          ###  #   # #     ###            |\n"
                     "|                                              |           #   #####  ###   #             |\n"
                     "|                                              |           #   #         #  #             |\n"
                     "|                                              |            ##  ###   ###    ##           |\n"
                     "+----------------------------------------------+------------------------------------------+\n"
                     "|Lorem ipsum dolor sit amet,                   |                                          |\n"
                     "|consectetur adipiscing                        |          ###                             |\n"
                     "|elit. Curabitur scelerisque                   |          #  #                            |\n"
                     "|lorem vitae lectus cursus,                    |          #  # # ##   ###    ###          |\n"
                     "|vitae porta ante placerat. Class aptent taciti|          ###  ##    #   #  #  #          |\n"
                     "|sociosqu ad litora                            |          #    #     #   #  #  #          |\n"
                     "|torquent per                                  |          #    #     #   #  #  #          |\n"
                     "|conubia nostra,                               |          #    #      ###    ###          |\n"
                     "|per inceptos himenaeos.                       |                               #          |\n"
                     "|                                              |                             ##           |\n"
                     "|Donec tincidunt augue                         |                                          |\n"
                     "|sit amet metus                                |           #    ###   ###   #             |\n"
                     "|pretium volutpat.                             |          ###  #   # #     ###            |\n"
                     "|Donec faucibus,                               |           #   #####  ###   #             |\n"
                     "|ante sit amet                                 |           #   #         #  #             |\n"
                     "|luctus posuere,                               |            ##  ###   ###    ##           |\n"
                     "|mauris tellus                                 |                                          |\n"
                     "+----------------------------------------------+------------------------------------------+\n"
                     "|                                          Bye,|*****   *      *  *      ******* ******  *|\n"
                     "|                                   Hello Kitty|*    *  *      *  *      *            *  *|\n"
                     "|                                              |*    *  *      *  *      *           *   *|\n"
                     "|                                              |*    *  *      *  *      *****      *    *|\n"
                     "|                                              |****    *      *  *      *         *     *|\n"
                     "|                                              |*  *    *      *  *      *        *       |\n"
                     "|                                              |*   *   *      *  *      *       *       *|\n"
                     "|                                              |*    *    *****   ****** ******* ******  *|\n"
                     "+----------------------------------------------+------------------------------------------+\n" );
    CTable t1 ( t0 );
    t1 . SetCell ( 1, 0, CEmpty () );
    t1 . SetCell ( 1, 1, CEmpty () );
    oss . str ("");
    oss . clear ();
    oss << t0;
    assert ( oss . str () ==
             "+----------------------------------------------+------------------------------------------+\n"
                     "|Hello,                                        |          ###                             |\n"
                     "|Hello Kitty                                   |          #  #                            |\n"
                     "|                                              |          #  # # ##   ###    ###          |\n"
                     "|                                              |          ###  ##    #   #  #  #          |\n"
                     "|                                              |          #    #     #   #  #  #          |\n"
                     "|                                              |          #    #     #   #  #  #          |\n"
                     "|                                              |          #    #      ###    ###          |\n"
                     "|                                              |                               #          |\n"
                     "|                                              |                             ##           |\n"
                     "|                                              |                                          |\n"
                     "|                                              |           #    ###   ###   #             |\n"
                     "|                                              |          ###  #   # #     ###            |\n"
                     "|                                              |           #   #####  ###   #             |\n"
                     "|                                              |           #   #         #  #             |\n"
                     "|                                              |            ##  ###   ###    ##           |\n"
                     "+----------------------------------------------+------------------------------------------+\n"
                     "|Lorem ipsum dolor sit amet,                   |                                          |\n"
                     "|consectetur adipiscing                        |          ###                             |\n"
                     "|elit. Curabitur scelerisque                   |          #  #                            |\n"
                     "|lorem vitae lectus cursus,                    |          #  # # ##   ###    ###          |\n"
                     "|vitae porta ante placerat. Class aptent taciti|          ###  ##    #   #  #  #          |\n"
                     "|sociosqu ad litora                            |          #    #     #   #  #  #          |\n"
                     "|torquent per                                  |          #    #     #   #  #  #          |\n"
                     "|conubia nostra,                               |          #    #      ###    ###          |\n"
                     "|per inceptos himenaeos.                       |                               #          |\n"
                     "|                                              |                             ##           |\n"
                     "|Donec tincidunt augue                         |                                          |\n"
                     "|sit amet metus                                |           #    ###   ###   #             |\n"
                     "|pretium volutpat.                             |          ###  #   # #     ###            |\n"
                     "|Donec faucibus,                               |           #   #####  ###   #             |\n"
                     "|ante sit amet                                 |           #   #         #  #             |\n"
                     "|luctus posuere,                               |            ##  ###   ###    ##           |\n"
                     "|mauris tellus                                 |                                          |\n"
                     "+----------------------------------------------+------------------------------------------+\n"
                     "|                                          Bye,|*****   *      *  *      ******* ******  *|\n"
                     "|                                   Hello Kitty|*    *  *      *  *      *            *  *|\n"
                     "|                                              |*    *  *      *  *      *           *   *|\n"
                     "|                                              |*    *  *      *  *      *****      *    *|\n"
                     "|                                              |****    *      *  *      *         *     *|\n"
                     "|                                              |*  *    *      *  *      *        *       |\n"
                     "|                                              |*   *   *      *  *      *       *       *|\n"
                     "|                                              |*    *    *****   ****** ******* ******  *|\n"
                     "+----------------------------------------------+------------------------------------------+\n" );
    oss . str ("");
    oss . clear ();
    oss << t1;
    assert ( oss . str () ==
             "+-----------+------------------------------------------+\n"
                     "|Hello,     |          ###                             |\n"
                     "|Hello Kitty|          #  #                            |\n"
                     "|           |          #  # # ##   ###    ###          |\n"
                     "|           |          ###  ##    #   #  #  #          |\n"
                     "|           |          #    #     #   #  #  #          |\n"
                     "|           |          #    #     #   #  #  #          |\n"
                     "|           |          #    #      ###    ###          |\n"
                     "|           |                               #          |\n"
                     "|           |                             ##           |\n"
                     "|           |                                          |\n"
                     "|           |           #    ###   ###   #             |\n"
                     "|           |          ###  #   # #     ###            |\n"
                     "|           |           #   #####  ###   #             |\n"
                     "|           |           #   #         #  #             |\n"
                     "|           |            ##  ###   ###    ##           |\n"
                     "+-----------+------------------------------------------+\n"
                     "+-----------+------------------------------------------+\n"
                     "|       Bye,|*****   *      *  *      ******* ******  *|\n"
                     "|Hello Kitty|*    *  *      *  *      *            *  *|\n"
                     "|           |*    *  *      *  *      *           *   *|\n"
                     "|           |*    *  *      *  *      *****      *    *|\n"
                     "|           |****    *      *  *      *         *     *|\n"
                     "|           |*  *    *      *  *      *        *       |\n"
                     "|           |*   *   *      *  *      *       *       *|\n"
                     "|           |*    *    *****   ****** ******* ******  *|\n"
                     "+-----------+------------------------------------------+\n" );
    t1 = t0;
    t1 . SetCell ( 0, 0, CEmpty () );
    t1 . SetCell ( 1, 1, CImage ()
            . AddRow ( "  ********                    " )
            . AddRow ( " **********                   " )
            . AddRow ( "**        **                  " )
            . AddRow ( "**             **        **   " )
            . AddRow ( "**             **        **   " )
            . AddRow ( "***         ********  ********" )
            . AddRow ( "****        ********  ********" )
            . AddRow ( "****           **        **   " )
            . AddRow ( "****           **        **   " )
            . AddRow ( "****      **                  " )
            . AddRow ( " **********                   " )
            . AddRow ( "  ********                    " ) );
    oss . str ("");
    oss . clear ();
    oss << t0;
    assert ( oss . str () ==
             "+----------------------------------------------+------------------------------------------+\n"
                     "|Hello,                                        |          ###                             |\n"
                     "|Hello Kitty                                   |          #  #                            |\n"
                     "|                                              |          #  # # ##   ###    ###          |\n"
                     "|                                              |          ###  ##    #   #  #  #          |\n"
                     "|                                              |          #    #     #   #  #  #          |\n"
                     "|                                              |          #    #     #   #  #  #          |\n"
                     "|                                              |          #    #      ###    ###          |\n"
                     "|                                              |                               #          |\n"
                     "|                                              |                             ##           |\n"
                     "|                                              |                                          |\n"
                     "|                                              |           #    ###   ###   #             |\n"
                     "|                                              |          ###  #   # #     ###            |\n"
                     "|                                              |           #   #####  ###   #             |\n"
                     "|                                              |           #   #         #  #             |\n"
                     "|                                              |            ##  ###   ###    ##           |\n"
                     "+----------------------------------------------+------------------------------------------+\n"
                     "|Lorem ipsum dolor sit amet,                   |                                          |\n"
                     "|consectetur adipiscing                        |          ###                             |\n"
                     "|elit. Curabitur scelerisque                   |          #  #                            |\n"
                     "|lorem vitae lectus cursus,                    |          #  # # ##   ###    ###          |\n"
                     "|vitae porta ante placerat. Class aptent taciti|          ###  ##    #   #  #  #          |\n"
                     "|sociosqu ad litora                            |          #    #     #   #  #  #          |\n"
                     "|torquent per                                  |          #    #     #   #  #  #          |\n"
                     "|conubia nostra,                               |          #    #      ###    ###          |\n"
                     "|per inceptos himenaeos.                       |                               #          |\n"
                     "|                                              |                             ##           |\n"
                     "|Donec tincidunt augue                         |                                          |\n"
                     "|sit amet metus                                |           #    ###   ###   #             |\n"
                     "|pretium volutpat.                             |          ###  #   # #     ###            |\n"
                     "|Donec faucibus,                               |           #   #####  ###   #             |\n"
                     "|ante sit amet                                 |           #   #         #  #             |\n"
                     "|luctus posuere,                               |            ##  ###   ###    ##           |\n"
                     "|mauris tellus                                 |                                          |\n"
                     "+----------------------------------------------+------------------------------------------+\n"
                     "|                                          Bye,|*****   *      *  *      ******* ******  *|\n"
                     "|                                   Hello Kitty|*    *  *      *  *      *            *  *|\n"
                     "|                                              |*    *  *      *  *      *           *   *|\n"
                     "|                                              |*    *  *      *  *      *****      *    *|\n"
                     "|                                              |****    *      *  *      *         *     *|\n"
                     "|                                              |*  *    *      *  *      *        *       |\n"
                     "|                                              |*   *   *      *  *      *       *       *|\n"
                     "|                                              |*    *    *****   ****** ******* ******  *|\n"
                     "+----------------------------------------------+------------------------------------------+\n" );
    oss . str ("");
    oss . clear ();
    oss << t1;
    assert ( oss . str () ==
             "+----------------------------------------------+------------------------------------------+\n"
                     "|                                              |          ###                             |\n"
                     "|                                              |          #  #                            |\n"
                     "|                                              |          #  # # ##   ###    ###          |\n"
                     "|                                              |          ###  ##    #   #  #  #          |\n"
                     "|                                              |          #    #     #   #  #  #          |\n"
                     "|                                              |          #    #     #   #  #  #          |\n"
                     "|                                              |          #    #      ###    ###          |\n"
                     "|                                              |                               #          |\n"
                     "|                                              |                             ##           |\n"
                     "|                                              |                                          |\n"
                     "|                                              |           #    ###   ###   #             |\n"
                     "|                                              |          ###  #   # #     ###            |\n"
                     "|                                              |           #   #####  ###   #             |\n"
                     "|                                              |           #   #         #  #             |\n"
                     "|                                              |            ##  ###   ###    ##           |\n"
                     "+----------------------------------------------+------------------------------------------+\n"
                     "|Lorem ipsum dolor sit amet,                   |                                          |\n"
                     "|consectetur adipiscing                        |                                          |\n"
                     "|elit. Curabitur scelerisque                   |        ********                          |\n"
                     "|lorem vitae lectus cursus,                    |       **********                         |\n"
                     "|vitae porta ante placerat. Class aptent taciti|      **        **                        |\n"
                     "|sociosqu ad litora                            |      **             **        **         |\n"
                     "|torquent per                                  |      **             **        **         |\n"
                     "|conubia nostra,                               |      ***         ********  ********      |\n"
                     "|per inceptos himenaeos.                       |      ****        ********  ********      |\n"
                     "|                                              |      ****           **        **         |\n"
                     "|Donec tincidunt augue                         |      ****           **        **         |\n"
                     "|sit amet metus                                |      ****      **                        |\n"
                     "|pretium volutpat.                             |       **********                         |\n"
                     "|Donec faucibus,                               |        ********                          |\n"
                     "|ante sit amet                                 |                                          |\n"
                     "|luctus posuere,                               |                                          |\n"
                     "|mauris tellus                                 |                                          |\n"
                     "+----------------------------------------------+------------------------------------------+\n"
                     "|                                          Bye,|*****   *      *  *      ******* ******  *|\n"
                     "|                                   Hello Kitty|*    *  *      *  *      *            *  *|\n"
                     "|                                              |*    *  *      *  *      *           *   *|\n"
                     "|                                              |*    *  *      *  *      *****      *    *|\n"
                     "|                                              |****    *      *  *      *         *     *|\n"
                     "|                                              |*  *    *      *  *      *        *       |\n"
                     "|                                              |*   *   *      *  *      *       *       *|\n"
                     "|                                              |*    *    *****   ****** ******* ******  *|\n"
                     "+----------------------------------------------+------------------------------------------+\n" );

    t1.SetCell( 0, 0, t1.GetCell( 0, 0 ) );

    return 0;
}
