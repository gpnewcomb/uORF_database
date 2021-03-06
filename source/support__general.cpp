//==============================================================================
// Project	   : uORF
// Name        : support__general.cpp
// Author      : Garin Newcomb
// Email       : gpnewcomb@live.com   
// Version     : See "Revision History" below
// Copyright   : Copyright 2014 University of Nebraska-Lincoln
// Description : Source code for defining general purpose support functions
//==============================================================================
//
//  Revision History
//      v0.0.0 - 2014/06/05 - Garin Newcomb
//          Initial creation of file, pulling primarily from the previous "fasta_in.cpp" and "csv_read.cpp" used with
//			this program
//
//    	Appl Version at Last File Update::  v0.0.x - 2014/06/05 - Garin Newcomb
//      	[Note:  until program released, all files tracking with program revision level -- see "version.h" file]
//
//==============================================================================


////////////////////////////////////////////////////////////////////////////////
//
//  Table of Contents -- Source (.cpp) File
//      (Note:  (*) indicates that the section is not present in this file)
//
//      A. Include Statements, Preprocessor Directives, and Related
//      B. Global Variable Declarations (including those in other files)
//      C. Member Function Definitions
//      D. Non-Member Function Definitions
//		E. Templated (Non-Member) Function Definitions
//      F. UNUSED Non-Member Function Definitions
//
////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//
// A. Include Statements, Preprocessor Directives, and Related
//
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <windows.h>
#include <string>
#include <vector>
#include <time.h>
#include <cmath>
#include <iomanip>      // For controlling how doubles are displayed using "setprecision()" and "fixed"/"scientific"
#include <algorithm>    // For "sort()"

using namespace std;

// Project-specific header files:  definitions and related information
#include "defs__general.h"
#include "version.h"

// Project-specific header files:  support functions and related
#include "support__file_io.h"
#include "uORF__compile.h"

// Header file for this file
#include "support__general.h"

////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//
// B. Global Variable Declarations (including those in other files)
//
////////////////////////////////////////////////////////////////////////////////

TErrors Errors;						// Stores information about errors and responds to them in several ways
extern TProgram_Log Program_Log;

////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//
// C. Member Function Definitions
//
////////////////////////////////////////////////////////////////////////////////

//==============================================================================

void TErrors::fatal_error_response( )
{
	// Ensure that this is the first error being responded to (to ensure that this function doesn't wind up creating an infinite loop of errors)
   if ( error_in_progress == ERROR_IN_PROGRESS )
	{
        return;  // Return to the calling function to ensure it can finish execution as best it can
    }
    
    // Indicate using the global variable that an error has occurred, and so future calls won't result in additional errors
	error_in_progress = ERROR_IN_PROGRESS;

    // Output a message to the debugging console to make it easiest to find where the fatal error occurred
	output_text_line( (string)"\n" +
							  "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n" +
							  "!! FATAL ERROR OCCURRED\n" +
							  "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n" );


	// Assemble the formatted error message to display to the user in a message box, first showing the source file and line number
	// While assembling the error message, also send the lines to the debug console and to the output window
	ostringstream error_message;
	string temp_string;

	const string error_line_separator = "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";

	output_text_line( error_line_separator );

    temp_string = "Encountered a fatal error in the file";
      output_text_line( temp_string );
    
    temp_string = "    " + file_name;
      error_message << temp_string << "\n";
      
    ostringstream temp_ostringstream;
    temp_ostringstream << line_number;
    temp_string = "at line number " + temp_ostringstream.str() + ".";
      error_message << temp_string << "\n";

    // If there's something in the error message, print it, then clear it
	if (!whole_error_msg.empty()) 
	{
		temp_string = "========================================";
		error_message << temp_string << "\n\n";

		error_message << "\n";
        
        temp_string = whole_error_msg;
		error_message << temp_string << "\n";
		whole_error_msg.clear();

		temp_string = "========================================";
		error_message << "\n" << temp_string << "\n\n";
	}
	
	output_text( error_message.str() );
	output_text_line( error_line_separator );
	output_text_line( "" );

	
	exit( Program_Log.print_log_to_file( PATH_PROGRAM_LOG_FOLDER, uORF_APPL_VERSION_NUMBER, NOT_SUCCESSFUL ) );
}
//------------------------------------------------------------------------------


void TErrors::nonfatal_error_response( )
{
	output_text_line( "" );
	output_text_line( "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" );
	
	// Output the argument error message (emphasizing it to show that an error occurred)
    output_text_line( whole_error_msg );
	output_text_line( "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" );
	output_text_line( "" );
		
	whole_error_msg.clear();

	num_nonfatal_errors++;

	return;
}
//------------------------------------------------------------------------------


int TErrors::handle_error( unsigned int error_type, string error_message, string file_nm, int line_num )
{
	// Update the file name and line number with the passed information
	file_name = file_nm;
	line_number = line_num;

	// Call the overloaded version of handle_error() and return 1 (in case the error is passed up one level)
	return handle_error(error_type, error_message);
}
//------------------------------------------------------------------------------


int TErrors::handle_error( unsigned int error_type, string error_message, int error_code )
{
	// Update the error message string by adding the passed error message to the front
	whole_error_msg = error_message + " " + whole_error_msg;
	error_type_local = error_type;

	// Check what kind of error occurred and call the corresponding function
	if ( error_type == FATAL ) { fatal_error_response();}
	else if ( error_type == NONFATAL ) 
	{ 
		nonfatal_error_response();
		
	}
	// If the error is to be passed up, simply return NOT_SUCCESSFUL so the next level is aware of an error and change to the error message
	else if ( error_type == PASS_UP_ONE_LEVEL ) { return error_code;}
	
	// If an invalid error type is passed, error out (fatally)
	else
	{
		// Doesn't call handle_error() to avoid (vaguely possible) infinite loop
		whole_error_msg = "Invalid error type passed."  + whole_error_msg;
		fatal_error_response();
	}
	
	return SUCCESSFUL;		// Success (Sort of.  This is the result of an error, after all)
}
//------------------------------------------------------------------------------


void TErrors::update_error_msg_global( const string text_to_add )
{
	// If the argument specifies to clear the error message instead of setting or appending to it, do so
	if ( text_to_add == CLEAR_ERROR_MSG_GLOBAL )
	{
		whole_error_msg = "";
	}

	// If the global error message string is currently blank, just set it to be the argument string
	if ( whole_error_msg.length() == 0 )
	{
		whole_error_msg = text_to_add;
	}
	// If there is already text in the global error message from a "lower-level" function call, push the new text in front of the old text
	else // ( whole_error_msg.length() != 0 )
	{
		whole_error_msg = text_to_add + "\n\n" + whole_error_msg;
	}
}
//------------------------------------------------------------------------------

//==============================================================================



void TProgram_Log::add_to_log( const string & text_to_add )
{
	log += text_to_add;
	
	return;
}
//------------------------------------------------------------------------------


int TProgram_Log::print_log_to_file( const string & path_program_log_file, const string & version_number, bool program_result )
{
	output_text_line( "" );
	output_text_line( "Program Duration: " + format_time_elapsed( 0, get_system_time_ms(), MILLISECONDS ) );
	
	output_text_line( "" );

	const string USER_COMMENT_PROMPT = "User Comment (Press Enter to Exit): ";
	string user_comment = "";
	string add_comment_to_file_name = "";
	
	do
	{
		user_comment = prompt_for_input<string>( USER_COMMENT_PROMPT, DONT_PRINT_PROMPT_TO_LOG );

		
		if( user_comment != "" ) 
		{ 
			add_comment_to_file_name = prompt_for_input<string>(  "Add Comment to File Name? ('Y'/Enter or ['C' to Change Comment]): ", DONT_PRINT_PROMPT_TO_LOG );
			if( add_comment_to_file_name == "y" ) { add_comment_to_file_name = "Y"; }
		}
		
	} while( add_comment_to_file_name == "C" );
	
	log += USER_COMMENT_PROMPT + ( user_comment == "" ? "NA" : user_comment ) + "\n"; 

	
	string file_name = format_system_date_time_filesystem_safe( time( NULL ) ) + 
					   PROGRAM_LOG_FILE_NAME_SUFFIX + 
					   ( add_comment_to_file_name == "Y" ? ( (string)"__'" + replace_spaces_w_underscores( &user_comment ) + "'" ) : "" ) +
					   TXT_FILE_EXTENSION;
	
	const string PATH_PROGRAM_LOG_VERSION_FOLDER = path_program_log_file 		   + PATH_FOLDER_SEPARATOR + "v" + version_number;
	const string PATH_PROGRAM_LOG_ALL_FOLDER     = PATH_PROGRAM_LOG_VERSION_FOLDER + PATH_FOLDER_SEPARATOR + "all";
	const string PATH_PROGRAM_LOG_RESULT_FOLDER  = PATH_PROGRAM_LOG_VERSION_FOLDER + PATH_FOLDER_SEPARATOR + ( program_result == SUCCESSFUL ? "no_error" : "error" );

	
	if( write_file_contents( PATH_PROGRAM_LOG_ALL_FOLDER, file_name, log, TRUE ) )
	{
		Errors.handle_error( NONFATAL, (string)"Error in 'TProgram_Log::print_program_log_to_file()': " );
	}
	
	if( write_file_contents( PATH_PROGRAM_LOG_RESULT_FOLDER, file_name, log, TRUE ) )
	{
		Errors.handle_error( NONFATAL, (string)"Error in 'TProgram_Log::print_program_log_to_file()': " );
	}

	return program_result;
}
//==============================================================================



void TCount_Term::increment( void )
{
	count++;
	
	return;
}
//------------------------------------------------------------------------------


void TCount_Term::decrement( void )
{
	count--;
	
	return;
}
//------------------------------------------------------------------------------


string TCount_Term::get_term( void ) const
{
	return term;
}
//------------------------------------------------------------------------------


unsigned int TCount_Term::get_count( void ) const
{
	return count;
}
//------------------------------------------------------------------------------

//==============================================================================
	


void TCount_Term_Vect::modify( const string & modified_term, const bool increment_or_decrement )
{	
	bool term_found = FALSE;
	unsigned int curr_it = 0;
	
	while( term_found == FALSE && curr_it < term_vect.size() )
	{
		if( modified_term == term_vect[ curr_it ].term )
		{
			if	  ( increment_or_decrement == INCREMENT )  { term_vect[ curr_it ].increment(); }
			else/*( increment_or_decrement == DECREMENT )*/{ term_vect[ curr_it ].decrement(); }
			
			term_found = TRUE;
		}
		
		curr_it++;	
	} 
	
	
	if( term_found == FALSE )
	{
		if( increment_or_decrement == INCREMENT )
		{
			term_vect.push_back( TCount_Term( modified_term, 1 ) );
		}
		else
		{
			Errors.handle_error( FATAL, "Error in 'TCount_Term_Vect::modify()': Term (" + modified_term + ") attempted to modify does not exist. ", __FILE__, __LINE__ );
		}
	}
	
	return;
}
//------------------------------------------------------------------------------


void TCount_Term_Vect::add_term( const string & added_term, const unsigned int init_count )
{
	unsigned int curr_it = 0;
	
	while( curr_it < term_vect.size() )
	{
		if( added_term == term_vect[ curr_it ].term )
		{
			Errors.handle_error( FATAL, (string)"Error in 'TCount_Term_Vect::add_term()': Term (" + added_term + ") attempted to add already exists. ", __FILE__, __LINE__ );
		}
		 
		curr_it++;
	}
	
	
	term_vect.push_back( TCount_Term( added_term, init_count ) );
	
	return;
}
//------------------------------------------------------------------------------


vector <TCount_Term> TCount_Term_Vect::get_term_vect( void ) const
{
	return term_vect;
}
//------------------------------------------------------------------------------


unsigned int TCount_Term_Vect::get_term_count( const string & term ) const
{
	unsigned int curr_it = 0;
	
	while( curr_it < term_vect.size() )
	{
		if( term == term_vect[ curr_it ].term )
		{
			return term_vect[ curr_it ].count;
		}	
		
		curr_it++;
	} 
	
	
	Errors.handle_error( FATAL, (string)"Error in 'TCount_Term_Vect::get_term_count()': Term (" + term + ") attempted to get count for does not exist. ", __FILE__, __LINE__ );
	
	// Should never get here
	return NOT_SUCCESSFUL;
}	
//------------------------------------------------------------------------------


void TCount_Term_Vect::sort_terms( const bool by_count_or_alphabet, const bool increasing_or_decreasing )
{
	if	  ( by_count_or_alphabet == BY_COUNT 	   )   { sort( term_vect.begin(), term_vect.end(), compare_terms_count	  ); }
	else/*( by_count_or_alphabet == ALPHABETICALLY )*/ { sort( term_vect.begin(), term_vect.end(), compare_terms_alphabet ); }
	
	if( increasing_or_decreasing == DECREASING )
	{
		reverse( term_vect.begin(), term_vect.end() );
	}
	
	return;
}
//------------------------------------------------------------------------------

//==============================================================================
		
////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//
// D. Non-Member Function Definitions
//
////////////////////////////////////////////////////////////////////////////////

// Time-related functions
//==============================================================================

// Return the current time in milliseconds elapsed since the program was opened
double get_system_time_ms( void )
{
	return (double)clock() / CLOCKS_PER_SEC * 1000.0;
}
//------------------------------------------------------------------------------


// Record and return the current absolute time, formatted in a readable form
string get_abs_time_formatted( const time_t current_time )
{
	// Get the formatted time/date text
	char * temp_buffer;  // Temporary storage for the formatted string

	temp_buffer = ctime( &current_time );

    if ( temp_buffer == NULL )
    {
		Errors.handle_error( FATAL, "Error in 'get_abs_time_formatted()':  call to 'ctime()' failed with a return value of 'NULL'." , __FILE__, __LINE__ );
    }
    
    
    // Convert the formatted text into a string, 
	string abs_time_unformatted = temp_buffer;
    
    output_text_line_debug_only( (string)"~~~ abs_time_unformatted = \"" + abs_time_unformatted + "\"" );
    
    // ensure it's of the expected length (otherwise text formatting will fail),
    if ( abs_time_unformatted.length() != 25 )
    {
        ostringstream error_ostringstream;
		error_ostringstream << "Error in 'get_abs_time_formatted()':  call to 'ctime()' resulted in text of length " << abs_time_unformatted.length()
                            << ", not 25 as expected:\n"
                            << "    \"" << abs_time_unformatted << "\"";
							
        Errors.handle_error( FATAL, error_ostringstream.str() , __FILE__, __LINE__ );
    }
    
    // remove the last character (a newline),
    abs_time_unformatted = abs_time_unformatted.substr( 0, 24 );
    
    // and perform text formatting to make the results look nicer
    string time_formatted;
    
    // Original format:
    //   "<Day of the week, 3 letters> <Month, 3 letters> <Day of the month, 2 numbers> <Hours/Minutes/Seconds, 8 numbers/colons> <Year, 4 numbers>"
    //   Example    "Fri Feb 22 09:37:42 2013"
    //   Positions:  012345678901234567890123
    // Reformatted:
    //   "<Year, 4 numbers>/<Month, 2 numbers>/<Day of the month, 2 numbers> <Day of the week, 3 letters> <Hours/Minutes/Seconds, 8 numbers/colons>"    
    //   Example -- "2013/02/22 Fri 09:37:42"
    //   Positions:  01234567890123456789012
    
    string month_text = abs_time_unformatted.substr( 4, 3 );
    string month_num;
    
    if      ( month_text == "Jan" ) { month_num = "01"; }
    else if ( month_text == "Feb" ) { month_num = "02"; }
    else if ( month_text == "Mar" ) { month_num = "03"; }
    else if ( month_text == "Apr" ) { month_num = "04"; }
    else if ( month_text == "May" ) { month_num = "05"; }
    else if ( month_text == "Jun" ) { month_num = "06"; }
    else if ( month_text == "Jul" ) { month_num = "07"; }
    else if ( month_text == "Aug" ) { month_num = "08"; }
    else if ( month_text == "Sep" ) { month_num = "09"; }
    else if ( month_text == "Oct" ) { month_num = "10"; }
    else if ( month_text == "Nov" ) { month_num = "11"; }
    else if ( month_text == "Dec" ) { month_num = "12"; }
    else
    {
		Errors.handle_error( FATAL, (string)"Error in 'get_abs_time_formatted()':  unknown month text of \"" + month_text + "\"" , __FILE__, __LINE__ );
    }
    
	time_formatted = abs_time_unformatted.substr( 20, 4 ) + "/" +  // Year
					 month_num                            + "/" +  // Month
					 abs_time_unformatted.substr(  8, 2 ) + " " +  // Day of the month
				  // abs_time_unformatted.substr(  0, 3 ) + " " +  // Day of the week
					 abs_time_unformatted.substr( 11, 8 );         // Hours:Minutes:Seconds
    
    
    output_text_line_debug_only( (string)"~~~ time_formatted = \"" + time_formatted + "\"" );
    
    
    // Return the formatted string
    return time_formatted;
}
//------------------------------------------------------------------------------


string format_time_elapsed( const double time_begin, const double time_end, unsigned int resolution )
{
    ostringstream ret_time_elapsed;

    // Calculate the total number of seconds elapsed
    double diff_in_ms = time_end - time_begin;
 
    // If the test takes longer than 24 hours to perform, something is wrong
    if ( diff_in_ms > 86400000 ) // 60 s/min * 60 min/hr * 24 hr/day = 86400 s/day
    {
        Errors.handle_error( FATAL, "Error in format_time_elapsed():  More than 1 day has elapsed" );
    }
	
	
    // Otherwise, calculate the number of hours, minutes, and seconds, and format them into the desired format
	unsigned int rounded_diff_in_ms = diff_in_ms;
	if( (diff_in_ms - rounded_diff_in_ms ) >= .5 ) { rounded_diff_in_ms += 1; }

	unsigned int num_hours = rounded_diff_in_ms / ( 60 * 60 * 1000 );
	unsigned int num_minutes = ( rounded_diff_in_ms - num_hours * ( 60 * 60 * 1000 ) ) / ( 60 * 1000 );
	double num_seconds = ( rounded_diff_in_ms - num_hours * ( 60 * 60 * 1000 ) - num_minutes * ( 60 * 1000 ) ) / 1000.0;
	
	

	// Assemble the formatted string, extending any single-digit integers to be two digits long (for consistency)
	ret_time_elapsed << ( num_hours < 10 ? "0" : "" ) << num_hours << "h:";
	
	if( resolution != HOURS )
	{
		ret_time_elapsed << ( num_minutes < 10 ? "0" : "" ) << num_minutes << "m:";
		
		if( resolution != MINUTES )
		{
			if( resolution == SECONDS ) 
			{ 
				ret_time_elapsed << ( num_seconds < 9.5 ? "0" : "" ) << format_with_rounding( num_seconds, 0 ) << "s";
			}
			else if( resolution == MILLISECONDS ) 
			{ 
				ret_time_elapsed << ( num_seconds < 9.9995 ? "0" : "" ) << format_with_rounding( num_seconds, 3 ) << "s";
			}
			else
			{
				Errors.handle_error( FATAL, "Error in format_time_elapsed(): An invalid resolution was requested" );
			}
		}
	}                                   

    // Return the formatted string
    return ret_time_elapsed.str();
}
//------------------------------------------------------------------------------


string format_system_date( const time_t current_time )
{
	return get_abs_time_formatted( current_time ).substr( 0, 10 );
}
//------------------------------------------------------------------------------


string format_system_date_filesystem_safe( const time_t current_time )
{
    return format_system_date_time_filesystem_safe( current_time ).substr( 0, 10 );
}
//------------------------------------------------------------------------------


string format_system_date_time_filesystem_safe( const time_t current_time )
{
    // Change all instances of '/' to '-'
    string ret_string = get_abs_time_formatted( current_time );
    
    ret_string = format_string_replace_all( ret_string, "/", "-" );
	
	
	// Change all instances of ':' to '-'
	ret_string = format_string_replace_all( ret_string, ":", "-" );
	
	
	// Change the date/time separating space to an underscore
	ret_string = format_string_replace_all( ret_string, " ", "_" );
    
    return ret_string;
}
//------------------------------------------------------------------------------

//==============================================================================



void remove_new_lines( string * const str_to_modify )
{
	for( unsigned int i = 0; i < (*str_to_modify).size(); i++ )
	{
		if( (*str_to_modify)[i] == '\n' )
		{
			(*str_to_modify).erase( i, 1 );
		}
	}
	return;
}
//==============================================================================



void remove_white_space( string * const str_to_modify, const size_t start_pos, const size_t end_pos )
{
	if( str_to_modify->size() == 0 ) { return; }
	
	size_t end_pos_cpy = end_pos;
	
	// Make sure start position is within the length of the string, and end position is within or just past the lenth of the string 
	//   (since the character at end position is not altered)
	if( start_pos   >= (*str_to_modify).size() ) { Errors.handle_error( FATAL, "Error in remove_white_space(): Start position specified does not exist in the passed string" ); }
	if( end_pos_cpy == string::npos 		   ) { end_pos_cpy = (*str_to_modify).size() - 1; }
	if( end_pos_cpy >= (*str_to_modify).size() ) { Errors.handle_error( FATAL, "Error in remove_white_space(): End position specified does not exist in the passed string"   ); }
	
	// For each character in the specified interval, check if it's a white space; if it is, erase it
	for( unsigned int i = start_pos; i <= end_pos_cpy; i++ )
	{
		if( is_char_white_space( (*str_to_modify), i ) == TRUE ) 
		{
			(*str_to_modify).erase( i, 1 );
			i--;
		}
	}
	
	return;
}
//==============================================================================



string replace_spaces_w_underscores( string * const str_to_modify )
{
	for( unsigned int i = 0; i < str_to_modify->size(); i++ )
	{
		if( str_to_modify->at( i ) == ' ' || str_to_modify->at( i ) == '\t' )
		{
			str_to_modify->at( i ) = '_'; 
		}
	}

	return *str_to_modify;
}
//==============================================================================



string format_string_replace_all( const string & string_to_be_formatted, const string & string_to_replace, const string & replacement_string )
{
    if ( string_to_replace == "" )
    {
        Errors.handle_error( FATAL, (string)"Error in 'format_string_replace_all()':  'string_to_replace' can't be empty." , __FILE__, __LINE__ );
    }
    
    // Search 'string_to_be_formatted' for all occurrences of 'string_to_replace' and replace them with 'replacement_string'
    string ret_string = string_to_be_formatted;
    
    size_t pos_next_replacement = ret_string.find( string_to_replace, 0 );
    
    /*  Function doesn't assume that any occurrences of 'string_to_replace' exist in 'string_to_be_formatted'
    if ( pos_next_replacement == ret_string.npos )
    {
        Program_Errors.error_occurred( FATAL, (string)"Error in 'format_string_replace_all()':  Can't find any occurrences of\n" + "  \"" + 
													   string_to_replace + "\"\n" + "in the argument string." , __FILE__, __LINE__ );
    }
    */
    
    while ( pos_next_replacement != ret_string.npos )
    {
        ret_string.replace( pos_next_replacement, string_to_replace.length(), replacement_string );
        pos_next_replacement = ret_string.find( string_to_replace, pos_next_replacement + replacement_string.length() );
    }
    
   return ret_string;
}
//==============================================================================

bool is_char_white_space( const string searched_str, const size_t pos )
{
	// Determine if the specified position in the string is a space or tab
	if    ( searched_str[pos] == ' ' || searched_str[pos] == '\t' )  { return TRUE;  }
	else/*( searched_str[pos] == ' ' || searched_str[pos] == '\t' )*/{ return FALSE; }
}
//==============================================================================



void cap_or_uncap_str( string * const str_to_modify, const bool cap_or_uncap, const unsigned int start_pos, const unsigned int length )
{
	unsigned int char_type;
	unsigned int end_pos = start_pos + length - 1;
	
	if( length == string::npos ) { end_pos = str_to_modify->size() - 1; }

	if( start_pos >= str_to_modify->size() )
	{
		Errors.handle_error( FATAL, "Error in 'cap_or_uncap_str()': Start position exceeds the length of the string passed. ", __FILE__, __LINE__ );
	}
	
	if( end_pos >= str_to_modify->size() )
	{
		Errors.handle_error( FATAL, "Error in 'cap_or_uncap_str()': End position exceeds the length of the string passed. ", __FILE__, __LINE__ );
	}
	
	
	for( unsigned int i = start_pos; i <= end_pos; i++ )
	{
		char_type = is_char_num_lower_upper( (*str_to_modify)[i] );
		if     ( cap_or_uncap == CAPITALIZE   && char_type == LOWER_CASE ){ (*str_to_modify)[i] -= ASCII_UPPER_LOWER_DIFF; }
		else if( cap_or_uncap == UNCAPITALIZE && char_type == UPPER_CASE ){ (*str_to_modify)[i] += ASCII_UPPER_LOWER_DIFF; }
	}

	return;
}
//==============================================================================



size_t find_first_digit_or_char( int digit_or_char, string searched_str, size_t start_pos ) 
{
	size_t i = start_pos;
	
	if( digit_or_char == DIGIT)
	{
		// Search the string for numbers, starting with start_pos and stopping when a number is found (returning its position)
		do
		{
			if( is_char_num_lower_upper( searched_str[i] ) == NUMBER) { return i; }
			i++;
		}
		while(i < searched_str.size() );
	}
	else if( digit_or_char == CHARACTER)
	{
		// Search the string for characters, starting with start_pos and stopping when a number is found (returning its position)
		do
		{
			unsigned int char_type = is_char_num_lower_upper( searched_str[i] );
			if( char_type == LOWER_CASE || char_type == UPPER_CASE) { return i; }
			i++;
		}
		while(i < searched_str.size() );
	}
	// If searching for either character or number, use recursion to find the first of each and return the lesser one
	else if( digit_or_char == DIGIT_OR_CHARACTER)
	{
		size_t first_num = find_first_digit_or_char( DIGIT, searched_str, start_pos );
		size_t first_char = find_first_digit_or_char( CHARACTER, searched_str, start_pos );
		return first_num <= first_char ? first_num : first_char;
	}
	// If neither DIGIT nor CHARACTER is passed, something went seriously wrong
	else
	{ 
		Errors.handle_error( FATAL, "Error in find_first_digit_or_char(): Must be searching for either a digit or character. ", __FILE__, __LINE__ );
	}
	
	// If no number is found, return npos
	return searched_str.npos; 
}
//==============================================================================



size_t find_last_digit_or_char( int digit_or_char, string searched_str, size_t start_pos ) 
{
	size_t i = start_pos;

	if( digit_or_char == DIGIT)
	{
		// Search the string for non-numbers, starting with start_pos and stopping when a non-number is found (returning the previous position)
		do
		{
			if( is_char_num_lower_upper( searched_str[i] ) != NUMBER) { return (i - 1); }
			i++;
		}
		while(i < searched_str.size() );
	}
	else if( digit_or_char == CHARACTER)
	{
		// Search the string for characters, starting with start_pos and stopping when a non-character is found (returning the previous position)
		do
		{
			unsigned int char_type = is_char_num_lower_upper( searched_str[i] );
			if( char_type != LOWER_CASE && char_type != UPPER_CASE ) { return (i - 1); }
			i++;
		}
		while(i < searched_str.size() );
	}
	// If searching for either non-character or non-number, check
	else if( digit_or_char == DIGIT_OR_CHARACTER)
	{
		// Search the string for characters, starting with start_pos and stopping when a non-number/non-character is found (returning the previous position)
		do
		{
			unsigned int char_type = is_char_num_lower_upper( searched_str[i] );
			if( char_type != LOWER_CASE && char_type != UPPER_CASE && char_type != NUMBER) { return (i - 1); }
			i++;
		}
		while(i < searched_str.size() );
	}
	// If neither DIGIT nor CHARACTER is passed, something went seriously wrong
	else
	{ 
		Errors.handle_error( FATAL, "Error in find_last_digit_or_char(): Must be searching for either a digit or character. ", __FILE__, __LINE__ );
	}
	
	
	// If no nonnumber is found, return npos
	return searched_str.npos; 
}
//==============================================================================



unsigned int is_char_num_lower_upper( unsigned char c )
{
	// Determine what kind of character c is using ASCII text table values
	if( c >= ASCII_NUMBER_START     && c <= ASCII_NUMBER_END) 	  { return NUMBER; }   		// The character passed is a number
	if( c >= ASCII_LOWER_CASE_START && c <= ASCII_LOWER_CASE_END) { return LOWER_CASE; }	// The character passed is a lower case letter
	if( c >= ASCII_UPPER_CASE_START && c <= ASCII_UPPER_CASE_END) { return UPPER_CASE; }	// The character passed is an upper case letter
	else { return SYMBOL;}
}
//==============================================================================



unsigned int get_num_diff_chars( const string & string_1, const string & string_2 )
{
	string ref_string  =  ( string_1.size() <= string_2.size() ?  string_1 : string_2 );
	string comp_string =  ( string_1.size() >  string_2.size() ?  string_1 : string_2 );
	
	unsigned int num_diff_chars = 0;
	
	for( unsigned int i = 0; i < ref_string.size(); i++ )
	{
		if( ref_string[i] != comp_string[i] )
		{
			num_diff_chars++;
		}
	}
	
	// If the strings differ in size, consider all the excess characters of the longer string to be "different"
	num_diff_chars += ( comp_string.size() - ref_string.size() );
	
	return num_diff_chars;
}
//==============================================================================



double logb( double num, double base )
{
	return log( num ) / log ( base );
}
//==============================================================================



double median( const std::vector <double> & set, bool presorted )
{
	if( presorted == FALSE )
	{
		vector <double> set_cpy = set;
		sort( set_cpy.begin(), set_cpy.end() );
		
		if	   ( set_cpy.size()     == 0 ) { return 0; }
		else if( set_cpy.size() % 2 != 0 ) { return   set_cpy[ ( set_cpy.size() - 1 ) / 2 ]; }
		else 					  	  	   { return ( set_cpy[ set_cpy.size() / 2 - 1 ] + set_cpy[ set_cpy.size() / 2 ] ) / 2; }
	}
	else
	{
		if	   ( set.size()     == 0 ) { return 0; }
		else if( set.size() % 2 != 0 ) { return set[ ( set.size() - 1 ) / 2 ]; }
		else 					  	   { return ( set[ set.size() / 2 - 1 ] + set[ set.size() / 2 ] ) / 2; }
	}
}
//==============================================================================



double mean  ( const std::vector <double> & set )
{
	double sum = 0;
	
	for( unsigned int i = 0; i < set.size(); i++ ) { sum += set[i]; }
	
	return sum / set.size();
}
//==============================================================================



vector <string> extract_fields( const string & str, const string & before_delimiter, const string & after_delimiter, const string & line_indicator, const bool add_repeats, const bool exclude_newlines )
{
	vector <string> field_contents( 0 );
	unsigned int field_start_pos = -1;
	unsigned int field_end_pos   = -1;
	
	bool str_end_reached = FALSE;		

	do
	{	
		if( str.find( before_delimiter, field_end_pos + 1 ) == string::npos )
		{
			str_end_reached = TRUE;
		}
		else
		{
			field_start_pos = str.find( before_delimiter, field_end_pos + 1 ) + before_delimiter.size();
			
			if( field_start_pos >= str.size() )
			{
				str_end_reached = TRUE;
			}
			else
			{
				field_end_pos = str.find( after_delimiter, field_start_pos + 1 ) - 1;
				
				bool line_indicator_present = TRUE;
				
				if( line_indicator != "" )
				{
					unsigned int prev_new_line = str.rfind( '\n', field_start_pos );
					if( prev_new_line == string::npos ) { prev_new_line = 0; }
					
					if( str.substr( prev_new_line, ( str.find( '\n', prev_new_line + 1 ) - prev_new_line ) ).find( line_indicator ) == string::npos )
					{
						line_indicator_present = FALSE;
					}
				}
				
				
				// If no end of field character is found after the start field character, all fields have been extracted
				if( field_end_pos == string::npos ) 
				{ 
					str_end_reached = TRUE; 			 
				}
				else if( exclude_newlines == TRUE && field_end_pos >= str.find( '\n', field_start_pos + 1 ) )
				{ 
					field_end_pos = field_start_pos; 
				}
				else if( line_indicator_present == FALSE )
				{
					field_end_pos = field_start_pos; 
				}
				else
				{
					string indiv_field_content = str.substr( field_start_pos, field_end_pos - field_start_pos + 1 );
					
					if( add_repeats == FALSE )
					{
						bool field_already_added = FALSE;
						
						for( unsigned int j = 0; j < field_contents.size(); j++ )
						{
							if( indiv_field_content == field_contents[j] ) { field_already_added = TRUE; }
						}
						
						if( field_already_added == FALSE )
						{
							field_contents.push_back( indiv_field_content );
						}
					}
					else/*( add_repeats == TRUE )*/
					{
						field_contents.push_back( indiv_field_content );
					}
				}
			}
		}

	} while( str_end_reached == FALSE );
	
	
	return field_contents;	
}
//==============================================================================



unsigned int count_instances_of( const string & str, const string & txt_to_search )
{
	int count = 0;
	
	unsigned int txt_start_pos = -1;
	
	while( str.find( txt_to_search, txt_start_pos + 1 ) != string::npos )
	{	
		txt_start_pos = str.find( txt_to_search, txt_start_pos + 1 );
		count++;
	}
	
	
	return count;
}
//==============================================================================



string format_with_rounding( double number_to_format, unsigned int num_places )
{
	ostringstream number_oss;
	
	double rounded_number = round( number_to_format * pow( 10, num_places ) ) / pow( 10, num_places );
	number_oss << FLOAT_OUTPUT_METHOD << setprecision( num_places ) << rounded_number;
	
	return number_oss.str();
}
//==============================================================================



unsigned int convert_roman_numeral_to_uint( const string roman_numeral )
{
	unsigned int converted_roman_num = 0;

	for( unsigned int curr_char = 0; curr_char < roman_numeral.size(); curr_char++ )
	{
		if	   ( roman_numeral[curr_char] == 'X' ) { converted_roman_num += 10; }
		else if( roman_numeral[curr_char] == 'V' ) { converted_roman_num += 5;  }
		else if( roman_numeral[curr_char] == 'I' )
		{
			if	   ( curr_char ==  ( roman_numeral.size() - 1 ) )
			{ 
				converted_roman_num += 1; 
			}
			else if( roman_numeral[curr_char + 1] == 'X' )		 
			{ 
				converted_roman_num += 9;
				curr_char++;				
			}
			else if( roman_numeral[curr_char + 1] == 'V' )		 
			{ 
				converted_roman_num += 4; 
				curr_char++;
			}
			else												 
			{ 
				converted_roman_num += 1; 
			}
		}
		else
		{
			Errors.handle_error( FATAL, (string)"Error in convert_roman_numeral_to_uint()': Cannot convert the roman numeral '" + roman_numeral + 
												"' as it exceeds 39 or contains disallowed characters. ", __FILE__, __LINE__ );
		}
		
	}
	
	return converted_roman_num;
}
//==============================================================================



string convert_uint_to_roman_numeral( const unsigned int uint )
{
	if( uint < 1 || uint > 39 )
	{
		Errors.handle_error( FATAL, "Error in 'convert_uint_to_roman_numeral()': Unsigned integer to convert must be in the range [1,39]. ", __FILE__, __LINE__ );
	}
	
	string converted_uint = "";
	
	
	unsigned int num_tens = uint / 10;
	
	for( unsigned int i = 0; i < num_tens; i++ )
	{
		converted_uint.push_back( 'X' );
	}
	
	
	unsigned int mod10 = uint % 10;
	unsigned int mod5  = uint % 5;
	
	if( mod10 >= 5 && mod10 < 9 )
	{
		converted_uint.push_back( 'V' );
	}

	if( mod5 <= 3 )
	{
		for( unsigned int i = 0; i < mod5; i++ )
		{
			converted_uint.push_back( 'I' );
		}
	}
	else if(  mod5 == 4 )
	{
		converted_uint.push_back( 'I' );
		converted_uint.push_back( mod10 == 9 ? 'X' : 'V' );
	}
	
	return converted_uint;
}
//==============================================================================



bool compare_terms_alphabet( const TCount_Term & term_1, const TCount_Term & term_2 )
{
	if	   ( term_1.get_term().compare( term_2.get_term() ) > 0 ) { return TERM_1_BEFORE_TERM_2; }
	else if( term_1.get_term().compare( term_2.get_term() ) < 0 ) { return TERM_2_BEFORE_TERM_1; }

	// If equivalent, indicate 2 is before 1 (to obey the 'std::sort()' weak ordering requirement)
	return TERM_2_BEFORE_TERM_1;
}
//==============================================================================



bool compare_terms_count( const TCount_Term & term_1, const TCount_Term & term_2 )
{	
	// Rank from highest count to lowest
	if	   ( term_1.get_count() > term_2.get_count() ) { return TERM_2_BEFORE_TERM_1; }
	else if( term_1.get_count() < term_2.get_count() ) { return TERM_1_BEFORE_TERM_2; }

	// If equivalent, indicate 2 is before 1 (to obey the 'std::sort()' weak ordering requirement)
	return TERM_2_BEFORE_TERM_1;
}
//==============================================================================



bool compare_strings( const string & str_1, const string & str_2 )
{
	if	   ( str_1.compare( str_2 ) > 0 ) { return TERM_1_BEFORE_TERM_2; }
	else if( str_1.compare( str_2 ) < 0 ) { return TERM_2_BEFORE_TERM_1; }

	// If equivalent, indicate 2 is before 1 (to obey the 'std::sort()' weak ordering requirement)
	return TERM_2_BEFORE_TERM_1;
}
//==============================================================================

////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//
// E. Templated (Non-Member) Function Definitions
//
////////////////////////////////////////////////////////////////////////////////

//==============================================================================

template < class type >
type get_type( const string & field )
{
	return atof( field.c_str() );
}

template int 		  get_type<int>			( const string & );
template unsigned int get_type<unsigned int>( const string & );
template double 	  get_type<double>		( const string & );
//==============================================================================



template <>
string get_type <string> ( const string & field )
{
	return field;
}

template string get_type<string>( const string & );
//==============================================================================



template < class type >
string get_str( const type & field )
{
	ostringstream r_oss;
	r_oss << field;
	
	return r_oss.str();
}

template string get_str<int>		 ( const int & );
template string get_str<unsigned int>( const unsigned int & );
template string get_str<double>		 ( const double & );
template string get_str<string>		 ( const string & );
//==============================================================================



template < class type >
type get_input_line( void )
{
	string temp_str;
	getline( cin, temp_str );
	
	return get_type<type>( temp_str );
}

template int 		  get_input_line<int>		  ( void );
template unsigned int get_input_line<unsigned int>( void );
template double 	  get_input_line<double>	  ( void );
template string 	  get_input_line<string>	  ( void );
//==============================================================================



template < class type >
type prompt_for_input( const string & prompt, const bool print_prompt_to_log = PRINT_PROMPT_TO_LOG )
{
	type input;
	
	if( print_prompt_to_log == PRINT_PROMPT_TO_LOG )
	{
		output_text( prompt );
		input = get_input_line<type>();
		
		ostringstream temp_oss;
		temp_oss << input;
		Program_Log.add_to_log( temp_oss.str() + (string)"\n" );
	}
	else/*( print_prompt_to_log == DONT_PRINT_PROMPT_TO_LOG )*/
	{
		output_text_debug_only( prompt );
		input = get_input_line<type>();
	}
	
	
	return input;
}

template int 		  prompt_for_input<int>		    ( const string &, const bool );
template unsigned int prompt_for_input<unsigned int>( const string &, const bool );
template double 	  prompt_for_input<double>	    ( const string &, const bool );
template string 	  prompt_for_input<string>	    ( const string &, const bool );
//==============================================================================



template < class type >
double perc_lists_containing_element( const type & element, const vector <vector <type> > & lists )
{
	unsigned int num_lists_include = 0;
	
	for( unsigned int i = 0; i < lists.size(); i++ )
	{
		if( find( lists[i].begin(), lists[i].end(), element ) !=  lists[i].end() )
		{
			num_lists_include++;
		}
	}
	
	return ( 100 * (double)num_lists_include / lists.size() );
}

template double perc_lists_containing_element<int>		   ( const int &, 		   const vector <vector <int> > & 	   );
template double perc_lists_containing_element<unsigned int>( const unsigned int &, const vector <vector <unsigned int> > & );
template double perc_lists_containing_element<double>	   ( const double &, 	   const vector <vector <double> > & 	   );
template double perc_lists_containing_element<string>	   ( const string &, 	   const vector <vector <string> > & 	   );
//==============================================================================



template < class type >
string generate_delimited_list( const vector <type> & vect_list, const string & delimiter )
{
	string r_str = "";
	for( unsigned int i = 0; i < vect_list.size(); i++ )
	{
		r_str += get_str( vect_list[i] );
		r_str += ( ( i != vect_list.size() - 1 ) ? delimiter : "" );
	}
	
	return r_str;
}

template string generate_delimited_list<int>		 ( const vector <int> &, 		  const string & );
template string generate_delimited_list<unsigned int>( const vector <unsigned int> &, const string & );
template string generate_delimited_list<double>	   	 ( const vector <double> &, 	  const string & );
template string generate_delimited_list<string>	   	 ( const vector <string> &, 	  const string & );
//==============================================================================



template < class type >
vector <type> parse_delimited_list( const string & str_list, const string & delimiter )
{
	vector <type> r_vect( 0 );

	if( str_list.size() != 0 )
	{
		unsigned int field_start_pos = 0;
		unsigned int delim_pos = 0;
		
		if( str_list.find( delimiter ) == 0 )
		{
			r_vect.push_back( get_type<type>( "" ) );
		}
		
		
		while( delim_pos != string::npos )
		{
			delim_pos = str_list.find( delimiter, ( delim_pos + 1 ) );
			
			string temp_str = str_list.substr( field_start_pos, ( delim_pos - field_start_pos ) );
			r_vect.push_back( get_type<type>( temp_str ) );

			field_start_pos = delim_pos + delimiter.size();
		}
	}
	
	
	return r_vect;
}

template vector <int> 		   parse_delimited_list<int>		 ( const string &, const string & );
template vector <unsigned int> parse_delimited_list<unsigned int>( const string &, const string & );
template vector <double> 	   parse_delimited_list<double>	     ( const string &, const string & );
template vector <string> 	   parse_delimited_list<string>	     ( const string &, const string & );
//==============================================================================

////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//
// F. UNUSED Non-Member Function Definitions
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////


