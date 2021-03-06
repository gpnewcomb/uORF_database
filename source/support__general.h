//==============================================================================
// Project	   : uORF
// Name        : support__general.h
// Author      : Garin Newcomb
// Email       : gpnewcomb@live.com   
// Version     : See "Revision History" below
// Copyright   : Copyright 2014 University of Nebraska-Lincoln
// Description : Header file declaring or defining general support functions and 
//				 classes
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
//      B. Type (and Member Function) Declarations and Definitions
//      C. Global Variable Declarations (including those in other files)
//      D. Non-Member Function Declarations
//      E. Templated (Non-Member) Function Declarations
//      F. Inline (Non-Member) Function Declarations and Definitions
//      G. UNUSED Non-Member Function Declarations
//
////////////////////////////////////////////////////////////////////////////////



#ifndef _SUPPORT__GENERAL_H_
#define _SUPPORT__GENERAL_H_



////////////////////////////////////////////////////////////////////////////////
//
// A. Include Statements, Preprocessor Directives, and Related
//
////////////////////////////////////////////////////////////////////////////////

// Standard libraries and related
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// Project-specific header files:  definitions and related information
#include "defs__general.h"

// Project-specific header files:  support functions and related
#include "support__file_io.h"

////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//
// B. Type (and Member Function) Declarations and Definitions
//
////////////////////////////////////////////////////////////////////////////////

//===============================================================================

// Define a class that holds error information and functions to respond to errors
//   on a case by case basis.  Declaration must follow ATE_System_Disable() function prototype
class TErrors
{
	private:
	// User calls handle_error(), which subsequently calls one of these three private functions
		void fatal_error_response();
		void nonfatal_error_response();

	public:
		unsigned int num_nonfatal_errors;
		std::string file_name;
		int line_number;
		std::string whole_error_msg;
		int error_code;
		unsigned int error_type_local;
		bool error_in_progress;

		int handle_error( unsigned int error_type, std::string error_message, std::string file_nm, int line_num );
		int handle_error( unsigned int error_type, std::string error_message, int error_code = NOT_SUCCESSFUL);
		void update_error_msg_global( const std::string text_to_add );

		TErrors( ) :
			num_nonfatal_errors( 0 ),
			line_number( 0 ),
			error_code( NOT_SUCCESSFUL ),
			error_type_local( NOT_SUCCESSFUL ),
			error_in_progress( ERROR_NOT_IN_PROGRESS )
		{ }
}; 
//===============================================================================



class TProgram_Log
{
	private:
		std::string log;
		
	public:
		void add_to_log( const std::string & text_to_add );
		int print_log_to_file( const std::string & path_log, const std::string & version, bool program_result );
};
//===============================================================================



class TCount_Term
{
	friend class TCount_Term_Vect;

	protected:
		std::string term;
		unsigned int count;
		
	public:
		void increment( void );
		void decrement( void );
		
		std::string get_term ( void ) const;
		unsigned int  get_count( void ) const;
		
		TCount_Term( std::string term_name, const unsigned int init_count = 0 ) :
			term ( term_name  ),
			count( init_count )
		{ }	
};
//===============================================================================



class TCount_Term_Vect
{
	private:
		std::vector <TCount_Term> term_vect;

	public:
		void modify( const std::string & modified_term, const bool increment_or_decrement );
		void add_term( const std::string & added_term, const unsigned int init_count = 0 );
		
		std::vector <TCount_Term> get_term_vect( void ) const;
		unsigned int get_term_count( const std::string & term ) const;
		void sort_terms( const bool by_count_or_alphabet, const bool increasing_or_decreasing );
};
//===============================================================================

////////////////////////////////////////////////////////////////////////////////





// Global Variable Declaration occur after class declarations because TProgram_Log
//  is defined there

////////////////////////////////////////////////////////////////////////////////
//
// C. Global Variable Declarations (including those in other files)
//
////////////////////////////////////////////////////////////////////////////////

extern TProgram_Log Program_Log;

////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//
// D. Non-Member Function Declarations
//
////////////////////////////////////////////////////////////////////////////////

double get_system_time_ms( void );
std::string get_abs_time_formatted(  const time_t current_time );
std::string format_time_elapsed( const double time_begin, const double time_end, unsigned int resolution );
std::string format_system_date( const time_t current_time );
std::string format_system_date_filesystem_safe( const time_t current_time );
std::string format_system_date_time_filesystem_safe( const time_t current_time );
		
void remove_new_lines( std::string * const str_to_modify );
void remove_white_space( std::string * const str_to_modify, const size_t start_pos = 0, const size_t end_pos = std::string::npos );
std::string replace_spaces_w_underscores( std::string * const str_to_modify );
std::string format_string_replace_all( const std::string & string_to_be_formatted, const std::string & string_to_replace, const std::string & replacement_string = "" );
bool is_char_white_space( const std::string searched_str, const size_t pos );
void cap_or_uncap_str( std::string * const str_to_modify, const bool cap_or_uncap, const unsigned int start_pos = 0, const unsigned int length = std::string::npos );
size_t find_first_digit_or_char( int digit_or_char, std::string searched_str, size_t start_pos = 0 ); 
size_t find_last_digit_or_char( int digit_or_char, std::string searched_str, size_t start_pos = 0 ); 
unsigned int is_char_num_lower_upper( unsigned char c );
unsigned int get_num_diff_chars( const std::string & string_1, const std::string & string_2 );
double logb( double num, double base );
double median( const std::vector <double> & set, bool presorted = FALSE );
double mean  ( const std::vector <double> & set );

std::vector <std::string> extract_fields( const std::string & str, 
										  const std::string & before_delimiter, 
										  const std::string & after_delimiter, 
										  const std::string & line_indicator = "", 
										  const bool add_repeats = TRUE, 
										  const bool exclude_newlines = TRUE );
										  
unsigned int count_instances_of( const std::string & str, const std::string & txt_to_search );
std::string format_with_rounding( double number_to_format, unsigned int num_places );

unsigned int convert_roman_numeral_to_uint( const std::string roman_numeral );
std::string convert_uint_to_roman_numeral( const unsigned int uint );

bool compare_terms_alphabet( const TCount_Term & term_1, const TCount_Term & term_2 );
bool compare_terms_count   ( const TCount_Term & term_1, const TCount_Term & term_2 );
bool compare_strings	   ( const std::string & str_1,  const std::string & str_2  );

////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//
// E. Templated (Non-Member) Function Declarations
//
////////////////////////////////////////////////////////////////////////////////

template < class type >
type get_type( const std::string & field );

template < class type >
std::string get_str( const type & field );

template < class type >
type get_input_line( void );

template < class type >
type prompt_for_input( const std::string & prompt, const bool print_prompt_to_log = PRINT_PROMPT_TO_LOG );

template < class type >
double perc_lists_containing_element( const type & element, const std::vector <vector <type> > & lists );

template < class type >
std::string generate_delimited_list ( const std::vector <type> & vect_list, const std::string & delimiter );

template < class type >
std::vector <type> parse_delimited_list( const std::string & str_list, const std::string & delimiter );

////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//
// F. Inline (Non-Member) Function Declarations and Definitions
//
////////////////////////////////////////////////////////////////////////////////

//==============================================================================

inline
void output_text_line_debug_only( const std::string & arg_line )
{
	// Output the argument text to the console for debugging
	cout << arg_line << endl;
	cout.flush();
}



//==============================================================================
inline
void output_text_debug_only( const std::string & arg_text )
{
	// Output the argument text to the console for debugging without including a newline (but still flushing to actually show output)
	cout << arg_text;
	cout.flush();
}
//==============================================================================



inline
void output_text_line( const std::string & arg_line )
{
    // Output the argument text to the console for debugging
    output_text_line_debug_only( arg_line );
	
	Program_Log.add_to_log( arg_line + (string)"\n" );
}
//==============================================================================



inline
void output_text( const std::string & arg_line )
{
    // Output the argument text to the console for debugging
    output_text_debug_only( arg_line );
	
	Program_Log.add_to_log( arg_line );
}
//==============================================================================

////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//
// G. UNUSED Non-Member Function Declarations
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////



#endif  // _SUPPORT__GENERAL_H_


