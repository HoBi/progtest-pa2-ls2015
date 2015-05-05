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

    }

    string render( unsigned int line, int outWidth, int outHeight ) const
    {
        int marginTop, marginLeft;

        // Make sure the size specified is correct
        if ( outWidth < width || outHeight < height )
            throw WrongSizeSpecifiedException();

        // todo: this is prolly wrong, this is gonna be computed every render call

        // Compute top margin
        marginTop = align == ALIGN_CENTER ? ( outHeight - height ) / 2 : 0;

        // Check if this isn't just blank line
        if ( line < marginTop || line >= marginTop + height ) return string( outWidth, ' ' );

        // Compute left margin
        marginLeft = ( align == ALIGN_CENTER ? ( outWidth - width ) / 2 : ( align == ALIGN_RIGHT ? outWidth - width : 0 ) );

        return string( marginLeft, ' ' ) + content.at( line ) + string( outWidth - ( marginLeft + content.at( line ).length() ), ' ' );
    }

    int width, height, align;
    vector<string> content;

protected:
    void InsertRow( const string &row )
    {
        content.insert( content.end(), row );

        int nwidth = row.length();

        if ( nwidth > width ) width = nwidth;
        height++;
    }
};

class CEmpty : public CCell
{
public:
    CEmpty()
    {
        width = height = 0;
        align = ALIGN_LEFT;
        content.insert( content.end(), "" );
    }

    ~CEmpty()
    {

    }
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

    ~CText()
    {

    }

    CText &SetText( const string &content )
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

        return *this;
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

    ~CImage()
    {

    }

    CImage &AddRow( const string &row )
    {
        InsertRow( row );
        return *this;
    }
};

class CTable
{
public:
    CTable( unsigned int rows, unsigned int cols ) : rows( rows ), cols ( cols )
    {
        cells = new CCell*[ rows * cols ];
        rowsHeight = new int[ rows ];
        colsWidth = new int[ cols ];

        for( int i = 0; i < rows * cols; i++ ) cells[ i ] = new CEmpty();
    }

    CTable &SetCell( unsigned int row, unsigned int col, CCell newContent )
    {
        // todo: value escapes local scope
        cells[ row * cols + col ] = new CCell( newContent );
        return *this;
    }

    CTable &SetCell( unsigned int row, unsigned int col, CCell *newContent )
    {
        cells[ row * cols + col ] = newContent;
        return *this;
    }

    CCell &GetCell( unsigned int row, unsigned int col )
    {
        return *cells[ row * cols + col ];
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
            if ( cells[ i ]->height > rowsHeight[ i / rows ] )
            {
                rowsHeight[ i / rows ] = cells[ i ]->height;
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
            os << divider << endl << "|";

            // Iterate through lines in this row
            for ( int j = 0; j < obj.rowsHeight[ i ]; j++ )
            {
                // Iterate through each column line of this row
                for ( int k = 0; k < obj.cols; k++ )
                {
                    os << obj.cells[ i * obj.cols + k ]->render( j, obj.colsWidth[ k ], obj.rowsHeight[ i ] );

                    // Col divider
                    os << "|";
                }
                os << endl;
                if ( j < obj.rowsHeight[ i ] - 1 ) os << "|";
            }
        }

        // Last divider
        os << divider;

        return os;
    }

private:
    CCell **cells;
    int *rowsHeight, *colsWidth;
    int rows, cols, width, height;
};

int main ( void )
{

    CTable table( 5, 4 );

    table.SetCell( 0, 0, CText( "Lorem" ) )
            .SetCell( 0, 1, CEmpty() )
            .SetCell( 0, 2, CText( "dolor sit" ) )
            .SetCell( 0, 3, CText( "amet" ) )
            .SetCell( 1, 0, CText( "Lorem\nipsum\ndolor\nsit" ) )
            .SetCell( 1, 1, CText( "consectetur\nadipiscing" ) )
            .SetCell( 1, 2, CText( "Lorem\nipsum", CText::ALIGN_RIGHT ) )
            .SetCell( 1, 3, CText( "Blah", CText::ALIGN_RIGHT ) )
            .SetCell( 2, 0, CText( "Lorem" ) )
            .SetCell( 2, 1, CText( "Lorem\nipsum", CText::ALIGN_RIGHT ) )
            .SetCell( 2, 2, CText( "Ipsum\nlorem" ) )
            .SetCell( 2, 3, CText( "Ipsum", CText::ALIGN_RIGHT ) )
            .SetCell( 3, 0, CText( "OOOOMMMMMGGGGGAAAA", CText::ALIGN_RIGHT ) )
            .SetCell( 3, 1, CText( "AAAAAAAAAAAAAAAAAA", CText::ALIGN_RIGHT ) )
            .SetCell( 3, 2, CText( "BBBBBBBBBBBBBBBBBB" ) )
            .SetCell( 3, 3, CText( "EEEEEEEEEEEEEEEEEE", CText::ALIGN_RIGHT ) )
            .SetCell (4, 1, CImage ()
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

    cout << table;


    return 69;

    // --------------------

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

    return 0;
}
