//==============================================================================
// Project	   : uORF
// Name        : support__file_io.h
// Author      : Garin Newcomb
// Email       : gpnewcomb@live.com   
// Version     : See "Revision History" below
// Copyright   : Copyright 2014 University of Nebraska-Lincoln
// Description : Header file including  function prototypes and object
//				 definitions used in reading in, parsing, and writing various 
//				 file types
//==============================================================================
//
//  Revision History
//      v0.0.0 - 2014/06/05 - Garin Newcomb
//          Initial creation of file, pulling primarily from the previous "fasta_in.h" and "csv_read.h" used with
//			this program
//
//    	Appl Version at Last File Update::  v0.0.x - 2014/06/05 - Garin Newcomb
//      	[Note:  until program released, all files tracking with program revision level -- see "version.h" file]
//
//==============================================================================


////////////////////////////////////////////////////////////////////////////////
//
//  Table of Contents -- Header (.h) File
//      (Note: (*) indicates that the section is not present in this file)
//
//      A. Include Statements, Preprocessor Directives, and Related
//     *B. Global Variable Declarations (including those in other files)
//      C. Type (and Member Function) Declarations and Definitions
//      D. Non-Member Function Declarations
//     *E. Templated (Non-Member) Function Declarations and Definitions
//     *F. Inline (Non-Member) Function Declarations and Definitions
//     *G. UNUSED Non-Member Function Declarations
//
////////////////////////////////////////////////////////////////////////////////



#ifndef _SUPPORT__FILE_IO_H_
#define _SUPPORT__FILE_IO_H_



////////////////////////////////////////////////////////////////////////////////
//
// A. Include Statements, Preprocessor Directives, and Related
//
////////////////////////////////////////////////////////////////////////////////

// Standard libraries and related
#include <string>
#include <vector>

// Project-specific header files:  definitions and related information
#include "defs__general.h"

////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//
// C. Type (and Member Function) Declarations and Definitions
//
////////////////////////////////////////////////////////////////////////////////

//===============================================================================

class TFasta_Content
{
	private:
		std::string file_folder_path;
		std::string file_name;
		
		std::string * temp_contents;
		bool file_read;
		
		std::vector <std::string> gff_annotations;
		bool gff_annotations_init;

		int read_file( void );

	public:
		std::vector <std::string> descr;
		std::vector <std::string> sequence;

		int get_contents();
		int get_contents_from_gff();
		
		std::vector <std::string> get_gff_annotations( void ) const;
		

		TFasta_Content( std::string fpath, std::string fname ) :
			file_folder_path( fpath ),
			file_name( fname ),
			temp_contents( new std::string( "" ) ),
			file_read( FALSE ),
			gff_annotations_init( UNINITIALIZED ),
			descr( std::vector <std::string>(0) ),
			sequence( std::vector <std::string>(0) )
		{ }
		
		~TFasta_Content( void )
		{
			delete temp_contents;
		}
};
//===============================================================================



class TCSV_Contents
{
	private:
		std::string file_folder_path;
		std::string file_name;
		
		std::vector <std::vector <std::string> > cell_contents;

	public:
		
		void parse_csv( void );
		
		std::vector <std::vector <std::string> > get_cell_contents( void );
		
		void add_row( const unsigned int row_num, const std::vector <std::string> & cell_row );
		void append_row( const std::vector <std::string> & cell_row );
		void replace_row( const unsigned int row_num, const std::vector <std::string> & new_cell_row );
		void resize_rows( const unsigned int num_rows );
		
		void add_column( const unsigned int col_num, const std::vector <std::string> & cell_column );
		void append_column( const std::vector <std::string> & cell_column );
		void replace_column( const unsigned int column_num, const std::vector <std::string> & new_cell_column );
		void replace_column( const std::string & column_header, const unsigned int row_num, std::vector <std::string> & new_cell_column );
		
		
		template < class r_type >
		std::vector <r_type> get_csv_row( const unsigned int row_num, const bool remove_spaces = REMOVE_SPACES ) const;
		
		template < class r_type >
		std::vector <r_type> get_csv_column( const unsigned int col_num, const bool remove_header, const bool remove_spaces = DONT_REMOVE_SPACES ) const;
		
		template < class r_type >
		std::vector <r_type> get_csv_column( const std::string & column_header, const unsigned int row_num = DEFAULT_HEADER_ROW, const bool remove_spaces = DONT_REMOVE_SPACES ) const;
		
		
		bool does_column_exist( const std::string & column_header, const unsigned int row_num = DEFAULT_HEADER_ROW ) const;
		unsigned int get_col_num( const std::string & column_header, const unsigned int row_num ) const;
		
		
		void write_back_to_csv( const std::string & fpath = "", const std::string & fname = "" ) const;
		
		
		TCSV_Contents( const std::string & fpath, const std::string & fname ) :
			file_folder_path( fpath ),
			file_name( fname ),
			cell_contents( 0 )
		{ }
};
//===============================================================================

////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//
// D. Non-Member Function Declarations
//
////////////////////////////////////////////////////////////////////////////////

int read_entire_file_contents( std::string file_folder_path, std::string file_name, std::string * file_contents );
int write_file_contents( const std::string file_folder_path, const std::string file_name, const std::string file_contents, const bool overwrite_file = YES );
int create_file( const std::string file_folder_path, const std::string file_name, bool * const did_file_already_exist  = NULL );
int check_if_file_exists( const std::string file_folder_path, const std::string file_name, bool * const did_file_already_exist );
bool create_folder( const std::string & arg_folder_abs_path );
std::string trim_trailing_separator( const std::string & str_to_trim );

int write_2d_vector_to_csv( const std::string file_folder_path, const std::string file_name, const std::vector <std::vector <std::string> > vector_to_write, const bool overwrite_file = YES );
std::string format_csv_field_with_quotes( std::string * const str_to_modify );

////////////////////////////////////////////////////////////////////////////////



#endif  // _SUPPORT__FILE_IO_H_


