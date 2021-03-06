//==============================================================================
// Project	   : uORF
// Name        : support__file_io.cpp
// Author      : Garin Newcomb
// Email       : gpnewcomb@live.com   
// Version     : See "Revision History" below
// Copyright   : Copyright 2014 University of Nebraska-Lincoln
// Description : Source code to read in, parse, and write to various file types
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
//     *E. UNUSED Non-Member Function Definitions
//
////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//
// A. Include Statements, Preprocessor Directives, and Related
//
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <windows.h>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>
#include <time.h>   	// For time() and related formatting functions
#include <typeinfo>

using namespace std;

// Project-specific header files:  definitions and related information
#include "defs__general.h"

// Project-specific header files:  support functions and related
#include "support__file_io.h"

// Header file for this file
#include "support__general.h"

////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//
// B. Global Variable Declarations (including those in other files)
//
////////////////////////////////////////////////////////////////////////////////

extern TErrors Errors;						// Stores information about errors and responds to them in several ways

////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//
// C. Member Function Definitions
//
////////////////////////////////////////////////////////////////////////////////

//==============================================================================

int TFasta_Content::read_file( void )
{
	if( read_entire_file_contents( file_folder_path, file_name, temp_contents ) )
	{
		return Errors.handle_error( PASS_UP_ONE_LEVEL, "Error reading FASTA file contents. ", __FILE__, __LINE__ );
	}
	
	file_read = TRUE;
	
	return SUCCESSFUL;
}
//------------------------------------------------------------------------------


int TFasta_Content::get_contents( void )
{
	if( file_read == FALSE )
	{
		if( read_file() )
		{
			return Errors.handle_error( PASS_UP_ONE_LEVEL, "", __FILE__, __LINE__ );
		}
	}
	
	size_t descr_end_pos = 0;
	size_t next_descr_start_pos = 0;
	bool end_of_file_found = FALSE;

	while( end_of_file_found == FALSE )
	{
		size_t descr_start_pos = next_descr_start_pos;
		descr_end_pos          = temp_contents->find( '\n', descr_start_pos + 1 );
		
		if( descr_end_pos == string::npos )
		{
			return Errors.handle_error( PASS_UP_ONE_LEVEL, "Error in 'TFasta_Content::get_contents()': End of FASTA description could not be found. ", __FILE__, __LINE__ );
		}

		descr.push_back( temp_contents->substr( descr_start_pos, ( descr_end_pos - descr_start_pos ) ) );

		next_descr_start_pos = temp_contents->find( FASTA_SEQ_START_ID, descr_end_pos + 1 );

		size_t seq_start_pos = descr_end_pos 		+ 1;
		size_t seq_end_pos   = next_descr_start_pos - 1;
		
		if( next_descr_start_pos == string::npos )
		{
			seq_end_pos = temp_contents->size() - 1;
			end_of_file_found = TRUE;
		}

		string temp_sequence = temp_contents->substr( seq_start_pos, ( seq_end_pos - seq_start_pos + 1 ) );
		remove_new_lines( &temp_sequence );

		sequence.push_back( temp_sequence );
	}

	// Now that the file has been parsed, 'temp_contents' can be deleted to free memory
	delete temp_contents;
	temp_contents = NULL;

	return SUCCESSFUL;
}
//------------------------------------------------------------------------------


int TFasta_Content::get_contents_from_gff( void )
{
	if( file_read == FALSE )
	{
		if( read_file() )
		{
			return Errors.handle_error( PASS_UP_ONE_LEVEL, "", __FILE__, __LINE__ );
		}
	}
	
	if( file_name.find( GFF_FILE_EXTENSION ) == string::npos )
	{
		return Errors.handle_error( PASS_UP_ONE_LEVEL, "Error in 'get_contents_from_gff()': File was not of .gff format. ", __FILE__, __LINE__ );
	}
	
	unsigned int fasta_id_pos = temp_contents->find( GFF_FILE_FASTA_ID );
	
	if( fasta_id_pos == string::npos )
	{
		return Errors.handle_error( PASS_UP_ONE_LEVEL, "Error in 'get_contents_from_gff()': .gff file did not contain the FASTA identifier. ", __FILE__, __LINE__ );
	}
	
	unsigned int fasta_start_pos = fasta_id_pos + ((string)GFF_FILE_FASTA_ID).size();
	
	
	size_t next_chrom_start_pos = temp_contents->find( "\nchr" );
	
	if( next_chrom_start_pos == string::npos )
	{
		return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'TFasta_Content::get_contents_from_gff()': Annotation data not found in specified file " +
															   "(chromosome identifiers not found). ", __FILE__, __LINE__ );
	}
	
	bool end_of_annotations_found = FALSE;

	while( end_of_annotations_found == FALSE )
	{
		size_t chrom_start_pos = next_chrom_start_pos;
		size_t tab_del_pos = temp_contents->find( "\t", chrom_start_pos );
		
		if( tab_del_pos == string::npos )
		{
			return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'TFasta_Content::get_contents_from_gff()': Annotation data not found in specified file " +
																   "(tab delimiter not found). ", __FILE__, __LINE__ );
		}
		
		string chrom_id 	 = temp_contents->substr( chrom_start_pos, ( tab_del_pos - chrom_start_pos + 1 ) );
		string next_chrom_id = chrom_id;
		
		size_t annotations_end_pos = fasta_id_pos - 1;
		
		do
		{
			next_chrom_start_pos = temp_contents->find( "\nchr", next_chrom_start_pos + 1 );

			if( next_chrom_start_pos == string::npos || next_chrom_start_pos > fasta_id_pos )
			{
				end_of_annotations_found = TRUE;
			}
			else
			{
				tab_del_pos = temp_contents->find( "\t", next_chrom_start_pos );
				
				if( tab_del_pos == string::npos )
				{
					return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'TFasta_Content::get_contents_from_gff()': Annotation data not found in specified file " +
																		   "(tab delimiter not found). ", __FILE__, __LINE__ );
				}
				
				next_chrom_id = temp_contents->substr( next_chrom_start_pos, ( tab_del_pos - next_chrom_start_pos + 1 ) );
			}

		} while( next_chrom_id == chrom_id && end_of_annotations_found == FALSE );
	

		size_t annotations_start_pos = chrom_start_pos;
		if( end_of_annotations_found == FALSE ) { annotations_end_pos = next_chrom_start_pos - 1; }


		string temp_annotations = temp_contents->substr( annotations_start_pos, ( annotations_end_pos - annotations_start_pos + 1 ) );
		gff_annotations.push_back( temp_annotations );
	}
	
	
	gff_annotations_init = INITIALIZED;
	
	
	*temp_contents = temp_contents->substr( fasta_start_pos );
	
	
	if( get_contents() )
	{
		return Errors.handle_error( PASS_UP_ONE_LEVEL, "", __FILE__, __LINE__ );
	}
	
	return SUCCESSFUL;
}
//------------------------------------------------------------------------------


vector <string> TFasta_Content::get_gff_annotations( void ) const
{
	if( gff_annotations_init == UNINITIALIZED )
	{
		Errors.handle_error( FATAL, "Error in 'TFasta_Content::get_gff_annotations()': .gff annotations haven't been initialized yet. ", __FILE__, __LINE__ );
	}

	return gff_annotations;
}
//------------------------------------------------------------------------------

//==============================================================================



void TCSV_Contents::parse_csv( void )
{
	if( file_name.find( CSV_FILE_EXTENSION ) == file_name.npos )
	{
		ostringstream error_oss;
		error_oss << "File \"" << file_name << "\" could not be parsed as a CSV, since it did not contain the CSV file extension.";
		Errors.handle_error( FATAL, error_oss.str(), __FILE__, __LINE__ );
	}

	string csv_buf;
	read_entire_file_contents( file_folder_path, file_name, &csv_buf );
	if( csv_buf.size() == 0 )
	{
		ostringstream error_oss;
		error_oss << "File \"" << file_name << "\" could not be parsed as a CSV, since it was empty. ";
		Errors.handle_error( FATAL, error_oss.str(), __FILE__, __LINE__ );
	}

	size_t cell_start_pos = 0;


	bool end_of_file = FALSE;
	unsigned int row_num = 0;
	size_t next_row_start_pos = 0;

	while( end_of_file == FALSE )
	{
		bool end_of_row = FALSE;
		cell_contents.push_back( vector <string> (0) );

		cell_start_pos = next_row_start_pos;
		next_row_start_pos = csv_buf.find( '\n', next_row_start_pos + 1) + 1;
		if( next_row_start_pos >= csv_buf.size() ) { end_of_file = TRUE; }


		while( end_of_row == FALSE )
		{
			size_t cell_end_delim_pos = csv_buf.find(",", cell_start_pos );
			bool cell_contains_spl_char = FALSE;
			bool non_esc_chars_in_field = FALSE;

			if( csv_buf[cell_start_pos] == '"')
			{
				cell_contains_spl_char = TRUE;
				size_t esc_char_pos = cell_start_pos;

				cell_start_pos++;
				bool end_of_field_found = FALSE;
				size_t num_consec_esc = 1;
				

				while( end_of_field_found == FALSE )
				{
					size_t last_esc_pos = esc_char_pos;
					esc_char_pos = csv_buf.find( '"', esc_char_pos + 1);
					if    ( last_esc_pos == esc_char_pos - 1) { num_consec_esc++;   }
					else/*( last_esc_pos != esc_char_pos - 1)*/ 
					{ 
						non_esc_chars_in_field = TRUE;
						num_consec_esc = 1;
					}

					if( num_consec_esc % 2 == 0 )
					{
						csv_buf.erase( esc_char_pos, 1 );
						esc_char_pos--;
						next_row_start_pos--;
					}

					// Should never happen
					if( esc_char_pos == csv_buf.size() - 1 )
					{
						end_of_field_found = TRUE;
						cell_end_delim_pos = esc_char_pos;
					}
					else
					{
						if( ( ( num_consec_esc % 2 != 0 && non_esc_chars_in_field == TRUE ) || ( num_consec_esc % 2 == 0 && non_esc_chars_in_field == FALSE ) ) &&
						    ( ( csv_buf[esc_char_pos + 1] == ',' ) || ( csv_buf[esc_char_pos + 1] == '\n' ) ) )
						{
							end_of_field_found = TRUE;
							
							if( esc_char_pos < cell_start_pos )
							{
								cell_end_delim_pos = cell_start_pos;
							}
							else/*( esc_char_pos >= cell_start_pos )*/
							{
								cell_end_delim_pos = esc_char_pos;
							}
						}
					}
				}
			}


			if( cell_end_delim_pos >= next_row_start_pos - 2 )
			{
				end_of_row = TRUE;
			}

			if( cell_end_delim_pos >= next_row_start_pos )
			{
				cell_end_delim_pos = next_row_start_pos - 1;
			}


			size_t cell_length = cell_end_delim_pos - cell_start_pos;
			cell_contents[row_num].push_back( csv_buf.substr( cell_start_pos, cell_length ) );

			cell_start_pos =  cell_end_delim_pos + 1;

			if( cell_contains_spl_char == TRUE && non_esc_chars_in_field == TRUE ) { cell_start_pos++; }
		}

		row_num++;
	}


	// Make sure all rows have the same number of columns
	unsigned int longest_row = 0;
	for( unsigned int row_num = 0; row_num < cell_contents.size(); row_num++)
	{
		if( cell_contents[row_num].size() > longest_row ) { longest_row = cell_contents[row_num].size(); }
	}

	for( unsigned int row_num = 0; row_num < cell_contents.size(); row_num++)
	{
		if( cell_contents[row_num].size() < longest_row ) { cell_contents[row_num].resize( longest_row, ""); }
	}

	return;
}
//------------------------------------------------------------------------------


vector <vector <string> > TCSV_Contents::get_cell_contents( void )
{
	return cell_contents;
}
//------------------------------------------------------------------------------


// Note that this is 1-indexed
void TCSV_Contents::add_row( const unsigned int row_num, const vector <string> & cell_row )
{	
	if( cell_contents.size() > 0 )
	{
		if( cell_row.size() != cell_contents[0].size() )
		{
			Errors.handle_error( FATAL, "Row to add in 'TCSV_Contents::add_row()' does not have the same number of columns as the spreadsheet. ", __FILE__, __LINE__ );
		}
	}
	
	if( ( row_num - 1 ) > cell_contents.size() )
	{
		Errors.handle_error( FATAL, "Row number specified in 'TCSV_Contents::add_row()' exceeds the maximum allowable row. ", __FILE__, __LINE__ );
	}
	
	
	cell_contents.insert( ( cell_contents.begin() + row_num - 1 ), cell_row );
	
	return;
}
//------------------------------------------------------------------------------


void TCSV_Contents::append_row( const vector <string> & cell_row )
{	
	add_row( cell_contents.size() + 1, cell_row );
	
	return;
}
//------------------------------------------------------------------------------


// Note that this is 1-indexed
void TCSV_Contents::replace_row( const unsigned int row_num, const vector <string> & new_cell_row )
{
	if( new_cell_row.size() != cell_contents[0].size() )
	{
		Errors.handle_error( FATAL, "Row to replace in 'TCSV_Contents::replace_row()' did not have the same number of columns as the spreadsheet. ", __FILE__, __LINE__ );
	}
	
	if( ( row_num - 1 ) >= cell_contents.size() )
	{
		Errors.handle_error( FATAL, "Row to replace in 'TCSV_Contents::replace_row()' appears not to exist. ", __FILE__, __LINE__ );
	}
	
		
	cell_contents[row_num - 1] = new_cell_row;
	
	return;
}
//------------------------------------------------------------------------------


void TCSV_Contents::resize_rows( const unsigned int num_rows )
{
	cell_contents.resize( num_rows );
	
	return;
}
//------------------------------------------------------------------------------


// Note that this is 1-indexed	
void TCSV_Contents::add_column( const unsigned int col_num, const vector <string> & cell_column )
{
	unsigned int num_rows = cell_contents.size();
	
	if( num_rows > 0 )
	{
		if( cell_contents.size() != cell_column.size() )
		{
			Errors.handle_error( FATAL, "Column to add in 'TCSV_Contents::add_column()' does not have the same number of rows as the spreadsheet. ", __FILE__, __LINE__ );
		}
		
		if( ( col_num - 1 ) > cell_contents[0].size() )
		{
			Errors.handle_error( FATAL, "Column number specified 'TCSV_Contents::add_column()' exceeds the maximum allowable column. ", __FILE__, __LINE__ );
		}
	}
	else
	{
		if( col_num != 1 )
		{
			Errors.handle_error( FATAL, "Row number specified 'TCSV_Contents::add_column()' exceeds the maximum allowable column. ", __FILE__, __LINE__ );
		}
		
		num_rows = cell_column.size();
		cell_contents.resize( num_rows, vector <string>( 0 ) );
	}
	
	

	for( unsigned int i = 0; i < num_rows; i++ )
	{
		cell_contents[i].insert( ( cell_contents[i].begin() + col_num - 1 ), cell_column[i] );
	}
	
	return;
}
//------------------------------------------------------------------------------

	
void TCSV_Contents::append_column( const vector <string> & cell_column )
{	
	if( cell_contents.empty() == TRUE )
	{
		add_column( 1, cell_column );
	}
	else/*( cell_contents.empty == FALSE )*/
	{
		add_column( cell_contents[0].size() + 1, cell_column );
	}

	return;
}
//------------------------------------------------------------------------------


// Note that this is 1-indexed
void TCSV_Contents::replace_column( const unsigned int column_num, const vector <string> & new_cell_column )
{
	if( cell_contents.size() == 0 )
	{
		Errors.handle_error( FATAL, "Column to replace in 'TCSV_Contents::replace_column()' does not exist (as there are no columns). ", __FILE__, __LINE__ );
	}
	else
	{
		if( cell_contents.size() != new_cell_column.size() )
		{
			Errors.handle_error( FATAL, "Column to add in 'TCSV_Contents::replace_column()' does not have the same number of rows as the spreadsheet. ", __FILE__, __LINE__ );
		}
		
		if( ( column_num - 1 ) >= cell_contents[0].size() )
		{
			Errors.handle_error( FATAL, "Column number specified in 'TCSV_Contents::replace_column()' exceeds the maximum allowable column. ", __FILE__, __LINE__ );
		}
	}
	

	for( unsigned int i = 0; i < cell_contents.size(); i++ )
	{
		cell_contents[i][column_num - 1] = new_cell_column[i];
	}
	
	return;
}
//------------------------------------------------------------------------------


void TCSV_Contents::replace_column( const string & column_header, const unsigned int row_num, vector <string> & new_cell_column )
{
	unsigned int col_num = get_col_num( column_header, row_num );
	
	if( col_num == 0 )
	{
		Errors.handle_error( FATAL, (string)"Column to retrieve ('" + get_str( column_header ) + 
											"') in 'TCSV_Contents::replace_column()' does not appear to exist", __FILE__, __LINE__ );
	}
	
	if( cell_contents.size() != new_cell_column.size() + row_num )
	{
		Errors.handle_error( FATAL, (string)"Column to add in 'TCSV_Contents::replace_column()' does not have the same number of rows " +
											"as the spreadsheet has below the specified header row. ", __FILE__, __LINE__ );
	}
	

	for( unsigned int i = row_num; i < cell_contents.size(); i++ )
	{
		cell_contents[i][col_num - 1] = new_cell_column[i - row_num];
	}
	
	return;
}
//------------------------------------------------------------------------------

		
// Note that this is 1-indexed (to match excel format)
template < class r_type >
vector <r_type> TCSV_Contents::get_csv_row( const unsigned int row_num, const bool remove_spaces  ) const
{
	if( row_num == 0 )
	{
		Errors.handle_error( FATAL, "Row number to retrieve in 'get_csv_row()' cannot be 0 (since 1-indexed)", __FILE__, __LINE__ );
	}

	if( row_num > cell_contents.size() )
	{
		ostringstream error_oss;
		error_oss << "Row number to retrieve (" 
				  << row_num 
				  << ") in 'get_csv_row()' cannot be greater than the number of rows ("
				  <<  cell_contents.size() << ")";

		Errors.handle_error( FATAL, error_oss.str(), __FILE__, __LINE__ );
	}
	
	
	vector <r_type> csv_row( cell_contents[0].size() );

	for( unsigned int i = 0; i < cell_contents[0].size(); i++ )
	{	
		string temp_field = cell_contents[row_num - 1][i];

		if( remove_spaces == TRUE )
		{
			remove_white_space( &temp_field );
		}

		
		csv_row[i] = get_type< r_type >( temp_field );
	}

	return csv_row;
}

template vector <int> 		   TCSV_Contents::get_csv_row<int>		   ( const unsigned int, const bool ) const;
template vector <unsigned int> TCSV_Contents::get_csv_row<unsigned int>( const unsigned int, const bool ) const;
template vector <double> 	   TCSV_Contents::get_csv_row<double>	   ( const unsigned int, const bool ) const;
template vector <string> 	   TCSV_Contents::get_csv_row<string>	   ( const unsigned int, const bool ) const;
//------------------------------------------------------------------------------


// Note that this is 1-indexed (to match excel format)
template < class r_type >
vector <r_type> TCSV_Contents::get_csv_column( const unsigned int col_num, const bool remove_header, const bool remove_spaces ) const
{
	if( col_num == 0 )
	{
		Errors.handle_error( FATAL, "Column number to retrieve in 'get_csv_column()' cannot be 0 (since 1-indexed)", __FILE__, __LINE__ );
	}

	if( col_num > cell_contents[0].size() )
	{
		ostringstream error_oss;
		error_oss << "Column number to retrieve (" 
				  << col_num 
				  << ") in 'get_csv_column()' cannot be greater than the number of columns ("
				  <<  cell_contents[0].size() << ")";

		Errors.handle_error( FATAL, error_oss.str(), __FILE__, __LINE__ );
	}


	vector <r_type> csv_column( cell_contents.size() - ( remove_header == FALSE ? 0 : 1 ) );

	for( unsigned int i = ( remove_header == FALSE ? 0 : 1 ); i < cell_contents.size(); i++ )
	{	
		string temp_field = cell_contents[i][col_num - 1];

		if( remove_spaces == TRUE )
		{
			remove_white_space( &temp_field );
		}

		
		csv_column[i] = get_type< r_type >( temp_field );
	}

	return csv_column;
}

template vector <int> 		   TCSV_Contents::get_csv_column<int>		  ( const unsigned int, const bool, const bool ) const;
template vector <unsigned int> TCSV_Contents::get_csv_column<unsigned int>( const unsigned int, const bool, const bool ) const;
template vector <double> 	   TCSV_Contents::get_csv_column<double>	  ( const unsigned int, const bool, const bool ) const;
template vector <string> 	   TCSV_Contents::get_csv_column<string>	  ( const unsigned int, const bool, const bool ) const;
//------------------------------------------------------------------------------


// Note that this is 1-indexed (to match excel format)
template < class r_type >
vector <r_type> TCSV_Contents::get_csv_column( const string & column_header, const unsigned int row_num, const bool remove_spaces ) const
{
	unsigned int col_num = get_col_num( column_header, row_num );
	
	if( col_num == 0 )
	{
		Errors.handle_error( FATAL, (string)"Column to retrieve ('" + get_str( column_header ) + 
											"') in 'TCSV_Contents::get_csv_column()' does not appear to exist", __FILE__, __LINE__ );
	}

	vector <r_type> csv_column( 0 );

	for( unsigned int i = row_num; i < cell_contents.size(); i++ )
	{	
		string temp_field = cell_contents[i][col_num - 1];

		if( remove_spaces == REMOVE_SPACES )
		{
			remove_white_space( &temp_field );
		}

		
		csv_column.push_back( get_type< r_type >( temp_field ) );
	}
	
	return csv_column;
}

template vector <int> 		   TCSV_Contents::get_csv_column<int>		  ( const string&, const unsigned int, const bool ) const;
template vector <unsigned int> TCSV_Contents::get_csv_column<unsigned int>( const string&, const unsigned int, const bool ) const;
template vector <double> 	   TCSV_Contents::get_csv_column<double>	  ( const string&, const unsigned int, const bool ) const;
template vector <string> 	   TCSV_Contents::get_csv_column<string>	  ( const string&, const unsigned int, const bool ) const;
//------------------------------------------------------------------------------


bool TCSV_Contents::does_column_exist( const string & column_header, const unsigned int row_num ) const
{
	if( row_num == 0 )
	{
		Errors.handle_error( FATAL, "Row number to search in 'does_column_exist()' cannot be 0 (since 1-indexed)", __FILE__, __LINE__ );
	}

	if( row_num > cell_contents.size() )
	{
		ostringstream error_oss;
		error_oss << "Column header ('" 
				  << column_header 
				  << "') to search for in 'does_column_exist()' could not be found, because the row number to search for the header (" 
				  << row_num 
				  << ") cannot be greater than the number of rows ("
				  <<  cell_contents.size() << ")";

		Errors.handle_error( FATAL, error_oss.str(), __FILE__, __LINE__ );
	}


	size_t col_pos = cell_contents[row_num - 1].size();
	for(unsigned int col_num = 0; col_num < cell_contents[row_num - 1].size(); col_num++ )
	{
		if( cell_contents[row_num - 1][col_num] == column_header ) 
		{ 
			col_pos = col_num; 
		}
	} 

	if    ( col_pos == cell_contents[row_num - 1].size() )  { return FALSE; }
	else/*( col_pos != cell_contents[row_num - 1].size() )*/{ return TRUE;  }
	
	// Should never get here
	return FALSE;
}
//------------------------------------------------------------------------------


// Note that this is 1-indexed
unsigned int TCSV_Contents::get_col_num( const string & column_header, const unsigned int row_num ) const
{
	if( row_num == 0 )
	{
		Errors.handle_error( FATAL, "Row number to search in 'TCSV_Contents::get_col_pos()' cannot be 0 (since 1-indexed)", __FILE__, __LINE__ );
	}

	if( ( row_num - 1 ) >= cell_contents.size() )
	{
		ostringstream error_oss;
		error_oss << "Column header ('" 
				  << column_header 
				  << "') to search for in 'TCSV_Contents::get_col_pos()' could not be found, because the row number to search for the header (" 
				  << row_num 
				  << ") cannot be greater than the number of rows ("
				  <<  cell_contents.size() << "), and cannot be 0.";

		Errors.handle_error( FATAL, error_oss.str(), __FILE__, __LINE__ );
	}


	unsigned int r_col_num = 0;
	for(unsigned int col_num = 1; col_num <= cell_contents[0].size(); col_num++ )
	{
		if( cell_contents[row_num - 1][col_num - 1] == column_header ) 
		{ 
			if( r_col_num != 0 )
			{	
				ostringstream error_oss;
				error_oss << "Column header ('" 
						  << column_header 
						  << "') to search for in 'TCSV_Contents::get_col_pos()' appears more than once. No way to determine which column is wanted.";
						  
				Errors.handle_error( FATAL, error_oss.str(), __FILE__, __LINE__ );
			}
			r_col_num = col_num; 
		}
	}

	
	return r_col_num;
}
//------------------------------------------------------------------------------


void TCSV_Contents::write_back_to_csv( const string & fpath, const string & fname ) const
{
	string fpath_local = fpath;
	string fname_local = fname;

	if( fpath_local == "" ) { fpath_local = file_folder_path; }
	if( fname_local == "" ) { fname_local = file_name; 		  }
	
	if( write_2d_vector_to_csv( fpath_local, fname_local, cell_contents ) )
	{
		Errors.handle_error( FATAL, "Error in 'TCSV_Contents::write_back_to_csv()': Failed to write cell contents back to CSV file. ", __FILE__, __LINE__ );
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

//==============================================================================

int read_entire_file_contents( string file_folder_path, string file_name, string * file_contents )
{
	// Open the ini file
	ifstream ifstream_file;
	ifstream_file.open( (file_folder_path + PATH_FOLDER_SEPARATOR + file_name).c_str(), ifstream::in );
	
    // If the attempt to open the file fails, the file very likely does not already exist
	if ( ifstream_file.fail() )
    {
		ostringstream error_oss;
		error_oss << ((string)"In 'read_entire_file_contents()', unable to open (for reading) file \"" + file_name + "\" at path \n" +
						 "              \"" + file_folder_path + "\".\nFile likely does not exist or is currently open" );
		return Errors.handle_error( PASS_UP_ONE_LEVEL, error_oss.str(), __FILE__, __LINE__ );
    }
	
	// Read all of the contents of the file into a string, one line at a time
	*file_contents = "";
    string temp_string;

    getline( ifstream_file, temp_string ); // Perform a preliminary read, entering the while loop if the file has at least one line
    
    while ( ifstream_file.fail() == 0 )   // 'getline()' will flag an error/failure when it runs out of lines to read in the file
    {
        *file_contents += temp_string + '\n';  // 'getline()' ends after each newline, but the newline isn't extracted, and so must be manually appended

        getline( ifstream_file, temp_string );     // Attempt to read the next line of the file, exiting from the while loop during the next iteration if no add'l lines are present
	}
    
	// Close the file stream, checking for any errors, and return its contents in string form
    ifstream_file.clear();  // Reset status of error bits, which were flagged after the last line failed to be read above
    ifstream_file.close();
	if ( ifstream_file.fail() )
	{
		ostringstream error_oss;
		error_oss << ((string)"In 'read_entire_file_contents()', unable to close file \"" + file_name + "\" at path \n" + 
							  "              \"" + file_folder_path + "\".\n" );
		return Errors.handle_error( PASS_UP_ONE_LEVEL, error_oss.str(), __FILE__, __LINE__ );
	}
	

	return SUCCESSFUL;
}
//==============================================================================



int write_file_contents( const string file_folder_path, const string file_name, const string file_contents, const bool overwrite_file )
{
	const string full_file_path = file_folder_path + PATH_FOLDER_SEPARATOR + file_name;
	
	bool did_file_exist;
	// Create the folder specified by the file path (or check that it already exists)
	if( create_folder( file_folder_path ) ) 
	{ 
		return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'write_file_contents()': Unable to create/verify path \" for file \"" + file_name + "\"."); 
	}
	
	if( check_if_file_exists( file_folder_path, file_name, &did_file_exist ) )
	{
		return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'write_file_contents()': ", __FILE__, __LINE__ );
	}
	
	if( did_file_exist == TRUE && overwrite_file == NO )
	{
		if( overwrite_file == NO )
		{
			return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'write_file_contents()': Unable to write file '" + full_file_path + 
																   "' because the file already existed and overwriting was disallowed. ", __FILE__, __LINE__ );
		}
		else
		{
			if( remove( full_file_path.c_str() ) )
			{
				return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'write_file_contents()':\n" \
																	   "'remove( \"" + full_file_path + "\" )' failed to delete the file. ", __FILE__, __LINE__ );
			}
		}
	}
	
	
	// Open an output file stream to the specified file
	ofstream ofstream_file;
	
	
	ofstream_file.open( full_file_path.c_str(), ofstream::out );
	if ( ofstream_file.fail() != 0 )  // 'ofstream.open()' will cause 'fail()' to return false if the open operation is not successful
	{
		// If opening the file stream fails, request that the user closes the file and try again
		output_text_line( (string)"Unable to open file '" + full_file_path + 
								  "'.  Check that file is closed, then press any key to continue." );
								  
		cin.ignore( 1 );
	
	
		ofstream_file.open( full_file_path.c_str(), ofstream::out );
		if ( ofstream_file.fail() != 0 ) 
		{
			return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'write_file_contents()': Still unable to open file '" + full_file_path + 
																   "' using 'ofstream.open()'. ", __FILE__, __LINE__ );
		}
	}

	// Output the contents of the argument string to the file
	ofstream_file << file_contents;
	
	// Close the file stream, checking for any errors
	ofstream_file.close();
	
	if ( ofstream_file.fail() != 0 )  // 'ofstream.close()' will cause 'fail()' to return false if the close operation is not successful
	{
		return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'write_file_contents()': Unable to close file '" + full_file_path + "' using 'ofstream.close()'",__FILE__, __LINE__ );
	}
		
	
	return SUCCESSFUL;
}
//==============================================================================



int create_file( const string file_folder_path, const string file_name, bool * const did_file_already_exist )
{
	// Create the folder specified by the file path (or check that it already exists)
	if( create_folder( file_folder_path ) ) 
	{ 
		return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'create_file()': Unable to create/verify path \" for file \"" + file_name + "\"."); 
	}
	
	if( check_if_file_exists( file_folder_path, file_name, did_file_already_exist ) )
	{ 
		return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'create_file()': " ); 
	}
	
	if( *did_file_already_exist == TRUE )
	{
		return SUCCESSFUL;
	}
	
	
	// If the file does not exist, create it; error out if the creation fails
	ofstream ofstream_file_to_create;
	
	ofstream_file_to_create.open( (file_folder_path + PATH_FOLDER_SEPARATOR + file_name).c_str(), ofstream::out );
	if ( ofstream_file_to_create.fail() == true )
	{
		return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Unable to create new file \n" + 
															   "    \"" + file_name + "\"\n" +
															   "at path\n" +
															   "    \"" + file_folder_path + "\"\n" +
															   "for writing in 'create_file()'." );
	}
	
	ofstream_file_to_create.close();  
	if ( ofstream_file_to_create.fail() == true )
	{
		return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error closing ofstream for new file \n" + 
															   "    \"" + file_name + "\"\n" +
															   "at path\n" +
															   "    \"" + file_folder_path + "\"\n" +
															   "in 'create_file()'." );
	}
	
	return SUCCESSFUL;
}
//==============================================================================



int check_if_file_exists( const string file_folder_path, const string file_name, bool * const did_file_already_exist )
{
	ifstream ifstream_target_file;
	ifstream_target_file.open( (file_folder_path + PATH_FOLDER_SEPARATOR + file_name).c_str(), ifstream::in );
    
    // If the attempt to open the file was successful, the file already exists - update the tracking flag accordingly
    if( ifstream_target_file.fail() == false )
    {                      
        ifstream_target_file.close();
        if ( ifstream_target_file.fail() == true )
        {
            return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'check_if_file_exists()': Error closing ifstream for existing file\n" + 
																   "    \"" + file_name + "\"\n" +
																   "at path\n" +
																   "    \"" + file_folder_path + "\"\n" +
																   "in 'create_file()'." );
        }
        

		if( did_file_already_exist != NULL ) { *did_file_already_exist = TRUE; }
        return SUCCESSFUL;
    }
	else/*( ifstream_target_file.fail() == true )*/
	{
		if( did_file_already_exist != NULL ) { *did_file_already_exist = FALSE; }
	}
	
	return SUCCESSFUL;
}
//==============================================================================



bool create_folder( const string & arg_folder_abs_path )
{
    output_text_debug_only( (string)" ~~ Creating/verifying folder:\n" +
                                    "      \"" + arg_folder_abs_path + "\"\n" );


    // If there's a trailing 'PATH_FOLDER_SEPARATOR', remove it
    string folder_abs_path = trim_trailing_separator( arg_folder_abs_path );


    // Since an absolute path must start with the drive letter, make sure that there's at least the 'PATH_FOLDER_SEPARATOR' between the
    // drive letter and the rest of the path, and then skip past it during the first loop iteration
    size_t curr_path_sep = folder_abs_path.find_first_of( PATH_FOLDER_SEPARATOR, 0 );
    if ( curr_path_sep == folder_abs_path.npos )
    {
        return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'create_folder()':  can't find \"" + PATH_FOLDER_SEPARATOR + "\" following drive letter (after\n" +
															   "  trimming trailing \"" + PATH_FOLDER_SEPARATOR + "\", if present) -- folder_abs_path =\n"
															   "    \"" + arg_folder_abs_path + "\"" );
    }

    // Until there are no more levels (each separated by 'PATH_FOLDER_SEPARATOR') left, create each level's folder
    const size_t len_PATH_FOLDER_SEPARATOR = ( (string)PATH_FOLDER_SEPARATOR ).length();

    do
    {
        size_t next_path_pos = folder_abs_path.find_first_of( PATH_FOLDER_SEPARATOR, curr_path_sep + len_PATH_FOLDER_SEPARATOR );
        const string curr_folder_path = folder_abs_path.substr( 0, curr_path_sep + len_PATH_FOLDER_SEPARATOR );
                                                                // ^^^ Includes trailing 'PATH_FOLDER_SEPARATOR' at 'curr_path_sep'
        const string curr_folder_name = folder_abs_path.substr( curr_path_sep + len_PATH_FOLDER_SEPARATOR,
                                                                ( next_path_pos == folder_abs_path.npos ?
                                                                  folder_abs_path.npos : next_path_pos - (curr_path_sep + len_PATH_FOLDER_SEPARATOR) ) );

        // If the current folder name is blank, issue an error
        if ( curr_folder_name.length() == 0 )
        {
            return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'create_folder()':  can't have a blank folder name at level\n" +
																   "    \"" + curr_folder_path + "\"\n" +
																   "  with folder_abs_path =\n"
																   "    \"" + arg_folder_abs_path + "\"" );
        }

        // Create or verify this level of the absolute path
        if ( CreateDirectory( (curr_folder_path + curr_folder_name).c_str(), NULL ) != 0 )
        {
            output_text_debug_only( (string)"  ~ Created folder \"" + curr_folder_name + "\" at path\n"
                                            "      \"" + curr_folder_path + "\"\n" );
        }
        else // ( CreateDirectory( path_root_folder, NULL ) == 0 )
        {
            DWORD error_no = GetLastError();
            if ( error_no == ERROR_ALREADY_EXISTS )
            {
                output_text_debug_only( (string)"  ~ Folder \"" + curr_folder_name + "\" already exists at path\n" +
                                                "      \"" + curr_folder_path + "\"\n" );
            }
            else // ( error_no == ERROR_PATH_NOT_FOUND )
            {
                return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'create_folder()':  error_no != 0, error_no != ERROR_ALREADY_EXISTS -- can't create folder\n" +
																	   "  \"" + curr_folder_name + "\"\n" +
																	   "at path\n" +
																	   "  \"" + curr_folder_path + "\"" );
            }
        }

        curr_path_sep = next_path_pos;
    }
    while ( curr_path_sep != folder_abs_path.npos );

    output_text_debug_only( "\n" );

    return SUCCESSFUL;
}
//==============================================================================



string trim_trailing_separator( const string & str_to_trim )
{
    // If there's a trailing 'PATH_FOLDER_SEPARATOR', remove it
    const size_t len_PATH_FOLDER_SEPARATOR = ( (string)PATH_FOLDER_SEPARATOR ).length();

    string trimmed_string;

    if ( str_to_trim.substr( str_to_trim.length() - len_PATH_FOLDER_SEPARATOR, len_PATH_FOLDER_SEPARATOR ) == PATH_FOLDER_SEPARATOR )
    {
        output_text_line_debug_only( (string)"  ~ Removing trailing '" + PATH_FOLDER_SEPARATOR + "' from \"" + str_to_trim + "\"" );
        return str_to_trim.substr( 0, str_to_trim.length() - len_PATH_FOLDER_SEPARATOR );
    }
    else  // No 'PATH_FOLDER_SEPARATOR' to trim
    {
        return str_to_trim;
    }
}
//==============================================================================



int write_2d_vector_to_csv( const string file_folder_path, const string file_name, const vector <vector <string> > vector_to_write, const bool overwrite_file )
{
	if( vector_to_write.size() == 0 )
	{
		return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'write_2d_vector_to_csv()': Vector passed was empty. " );
	}
	
	if( vector_to_write[0].size() == 0 )
	{
		return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'write_2d_vector_to_csv()': Vector passed did not contain any columns. " );
	}
	
	const unsigned int num_rows 	= vector_to_write.size();
	const unsigned int num_columns  = vector_to_write[0].size();
	
	for( unsigned int i = 1; i < num_rows; i++ )
	{
		if( vector_to_write[i].size() != num_columns )
		{
			return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'write_2d_vector_to_csv()': Vector passed did not have an equal number of columns in each row. " );
		}
	}
	
	
	
	string file_contents_to_write_str = "";
	
	for( unsigned int row_num = 0; row_num < num_rows; row_num++ )
	{
		for ( unsigned int col_num = 0; col_num < num_columns; col_num++ )
		{
			string modified_field_value = vector_to_write[row_num][col_num];
			format_csv_field_with_quotes( &modified_field_value );
			
			file_contents_to_write_str += modified_field_value + ( col_num != num_columns - 1 ? "," : "" );  // Don't output a comma for the last item in the vector
		}
		
		file_contents_to_write_str += "\n";
	}
	
	
	if( write_file_contents( file_folder_path, file_name, file_contents_to_write_str, overwrite_file ) )
	{
		return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'write_2d_vector_to_csv()': " );
	}
	
	return SUCCESSFUL;
}
//==============================================================================



string format_csv_field_with_quotes( string * const str_to_modify )
{
	// If the characters '=', '-', or '+' begin the string, the CSV will interpret the field as a formula, limiting the field to 255 characters and otherwise causing headaches
	if( ( (*str_to_modify)[0] == '=' ) ||
		( (*str_to_modify)[0] == '-' ) ||
		( (*str_to_modify)[0] == '+' )    )
	{
		(*str_to_modify).insert( 0, " " );
	}


	bool does_string_have_comma_or_new_line = FALSE;
	unsigned int i = 0;
	
	while( i < (*str_to_modify).size() && does_string_have_comma_or_new_line == FALSE )
	{
		if( (*str_to_modify)[i] == ',' || (*str_to_modify)[i] == '\n' ) { does_string_have_comma_or_new_line = TRUE; } 
		i++;
	}
	
	if( does_string_have_comma_or_new_line == TRUE )
	{
		size_t last_quote_pos = (*str_to_modify).find( '\"' );
		while( last_quote_pos != (*str_to_modify).npos )
		{
			(*str_to_modify).insert( last_quote_pos, (string)"\"" );
			last_quote_pos = (*str_to_modify).find( '\"', ( last_quote_pos + 2 ) );
		}
	
		(*str_to_modify).append( "\"" );
		(*str_to_modify).insert( 0, "\"" );
	}
	
	return *str_to_modify;
}
//==============================================================================

////////////////////////////////////////////////////////////////////////////////


