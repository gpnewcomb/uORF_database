//==============================================================================
// Project	   : uORF
// Name        : support__general.cpp
// Author      : Garin Newcomb
// Email       : gpnewcomb@live.com   
// Version     : See "Revision History" below
// Copyright   : Copyright 2014 University of Nebraska-Lincoln
// Description : Source code for defining bioinformatics support functions
//==============================================================================
//
//  Revision History
//      v0.0.0 - 2014/07/09 - Garin Newcomb
//          Initial creation of file, moving code from "support__general.cpp" and "uORF__compile.cpp", also
//			included in this program
//
//    	Appl Version at Last File Update::  v0.0.x - 2014/07/09 - Garin Newcomb
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

#include <windows.h>
#include <string>
#include <vector>
#include <algorithm>      // For 'reverse()'
#include <cmath>      	  // For 'pow()'

using namespace std;

// Project-specific header files:  definitions and related information
#include "defs__general.h"

// Project-specific header files:  support functions and related
#include "support__general.h"


// Header file for this file
#include "support__bioinformatics.h"

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



void TGO_Annotation::add_term( const string & GO_term_name, const string & GO_term_evidence, const unsigned int GO_term_level, const std::string GO_term_parent )
{
	unsigned int curr_it = 0;
	
	while( curr_it < GO_Terms.size() )
	{
		if( GO_term_name == GO_Terms[ curr_it ].name )
		{
			return;
		}
		 
		curr_it++;
	}
	
	
	GO_Terms.push_back( TGO_Term( GO_term_name, GO_term_evidence, GO_term_level, GO_term_parent ) );
	
	return;
}
//==============================================================================



string TGO_Annotation::format_terms_list( void ) const
{
	string terms_list = "";
	const string TERMS_DELIMITER = GO_TERM_LIST_DELIMITER;
	vector <string> terms_list_vect( 0 );
	
	
	// Only add the terms that were explicitly obtained from SGD (i.e. the terms that have evidence codes)
	for( unsigned int i = 0; i < GO_Terms.size(); i++ )
	{
		if( GO_Terms[i].evidence != "" )
		{
			terms_list_vect.push_back(  GO_Terms[i].name );
		}
	}
	
	
	
	for( unsigned int i = 0; i < terms_list_vect.size(); i++ )
	{	
		// Check if the term has already been added (there are multiples because the same term can have multiple evidence codes)
		bool curr_term_added = FALSE;
		
		unsigned int searched_term = 0;
		
		while( searched_term < i && curr_term_added == FALSE )
		{
			if( terms_list_vect[i] == terms_list_vect[searched_term] )
			{
				curr_term_added = TRUE;
			}
			
			searched_term++;	
		}
		
		
		// If not already added, add the term to the string
		if( curr_term_added == FALSE )
		{
			if( terms_list_vect[i].find( TERMS_DELIMITER.substr( 0, 1 ) ) != string::npos )
			{
				Errors.handle_error( FATAL, (string)"Error in 'TGO_Annotation::format_terms_list()': GO Term '" + terms_list_vect[i] + 
													"' contained the delimiter used to separate terms.", __FILE__, __LINE__ );
			}
			
			terms_list += terms_list_vect[i] + ( i + 1 != terms_list_vect.size() ? TERMS_DELIMITER : "" );
		}
		else
		{
			if( i + 1 == terms_list_vect.size() ) 
			{ 
				terms_list.erase( terms_list.size() - TERMS_DELIMITER.size(), TERMS_DELIMITER.size() );
			}
		}
	}
	
	
	return terms_list;
}
//==============================================================================



double TCalculate_AUGCAI::calc( const string & start_context ) const
{
	if( start_context.size() == 0 )
	{
		return DEFAULT_AUGCAI;
	}
	else if( start_context.size() != START_CONTEXT_LENGTH_NT )
	{
		Errors.handle_error( FATAL, "Error in 'TCalculate_AUGCAI::calc()': Passed start context was not of the correct length. ", __FILE__, __LINE__ );
	}
	

	string start_context_wo_start_codon = start_context.substr( 0, 								 START_CONTEXT_NT_BEFORE_AUG  ) + 
										  start_context.substr( START_CONTEXT_NT_BEFORE_AUG + 3, START_CONTEXT_NT_AFTER_AUG - NUM_NT_TO_COMPLETE_CODON );
										  
									  
	double AUGCAI_product = 1;
	for( unsigned int nt_pos_it = 0; nt_pos_it < NUM_AUGCAI_POSITIONS; nt_pos_it++ )
	{
		unsigned int nt_it = 0;
		
		switch( start_context_wo_start_codon[nt_pos_it] )
		{
			case 'A' : nt_it = NT_A;  break;
			case 'C' : nt_it = NT_C;  break;
			case 'G' : nt_it = NT_G;  break;
			case 'T' : nt_it = NT_T;  break;
			
			default: Errors.handle_error( FATAL, "Error in 'TCalculate_AUGCAI::calc()': Passed start context contains unrecognized characters. ", __FILE__, __LINE__ );
		}
		
		AUGCAI_product *= AUGCAI_weight_matrix[nt_pos_it][nt_it];
	}
	
	
	double AUGCAI_val = pow( AUGCAI_product, ( 1 / (double)NUM_AUGCAI_POSITIONS ) ) / MAX_AUGCAI;
	
	
	return AUGCAI_val;
}
//------------------------------------------------------------------------------


TCalculate_AUGCAI::TCalculate_AUGCAI( void )
{
	if( AUGCAI_weight_matrix_init == FALSE )
	{
		const double AUGCAI_weight_matrix_posn_neg6_array[] = AUGCAI_WEIGHT_MATRIX_POSN_NEG6;
		const double AUGCAI_weight_matrix_posn_neg5_array[] = AUGCAI_WEIGHT_MATRIX_POSN_NEG5;
		const double AUGCAI_weight_matrix_posn_neg4_array[] = AUGCAI_WEIGHT_MATRIX_POSN_NEG4;
		const double AUGCAI_weight_matrix_posn_neg3_array[] = AUGCAI_WEIGHT_MATRIX_POSN_NEG3;
		const double AUGCAI_weight_matrix_posn_neg2_array[] = AUGCAI_WEIGHT_MATRIX_POSN_NEG2;
		const double AUGCAI_weight_matrix_posn_neg1_array[] = AUGCAI_WEIGHT_MATRIX_POSN_NEG1;
		const double AUGCAI_weight_matrix_posn_pos4_array[] = AUGCAI_WEIGHT_MATRIX_POSN_POS4;
		const double AUGCAI_weight_matrix_posn_pos5_array[] = AUGCAI_WEIGHT_MATRIX_POSN_POS5;
		const double AUGCAI_weight_matrix_posn_pos6_array[] = AUGCAI_WEIGHT_MATRIX_POSN_POS6;


		for( unsigned int nt_it = 0; nt_it < NUM_DIFF_NT; nt_it++ )
		{
			AUGCAI_weight_matrix[ 0 ][ nt_it ] = AUGCAI_weight_matrix_posn_neg6_array[ nt_it ];
			AUGCAI_weight_matrix[ 1 ][ nt_it ] = AUGCAI_weight_matrix_posn_neg5_array[ nt_it ];
			AUGCAI_weight_matrix[ 2 ][ nt_it ] = AUGCAI_weight_matrix_posn_neg4_array[ nt_it ];
			AUGCAI_weight_matrix[ 3 ][ nt_it ] = AUGCAI_weight_matrix_posn_neg3_array[ nt_it ];
			AUGCAI_weight_matrix[ 4 ][ nt_it ] = AUGCAI_weight_matrix_posn_neg2_array[ nt_it ];
			AUGCAI_weight_matrix[ 5 ][ nt_it ] = AUGCAI_weight_matrix_posn_neg1_array[ nt_it ];
			AUGCAI_weight_matrix[ 6 ][ nt_it ] = AUGCAI_weight_matrix_posn_pos4_array[ nt_it ];
			AUGCAI_weight_matrix[ 7 ][ nt_it ] = AUGCAI_weight_matrix_posn_pos5_array[ nt_it ];
			AUGCAI_weight_matrix[ 8 ][ nt_it ] = AUGCAI_weight_matrix_posn_pos6_array[ nt_it ];
		}
		

		AUGCAI_weight_matrix_init = TRUE;
	}
}
//==============================================================================
	
////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//
// D. Non-Member Function Definitions
//
////////////////////////////////////////////////////////////////////////////////

void request_sel_sequence( const vector <string> & yeast_chromosomes )
{
	bool request_another_seq = FALSE;
	output_text_line( "" );
	ostringstream output_oss;
	
	do
	{
		unsigned int sel_chrom = 0;
		
		do
		{
			sel_chrom = prompt_for_input<unsigned int>( "Selected Chromosome (1-16):   ", PRINT_PROMPT_TO_LOG );
			//output_text_line( "" );
			
		} while( sel_chrom < 1 || sel_chrom > 16 );
		
		
		unsigned int start_nucleotide = 0;
		
		do
		{
			start_nucleotide = prompt_for_input<unsigned int>( "Selected Start Nucleotide:    ", PRINT_PROMPT_TO_LOG );
			//output_text_line( "" );
			
		} while( start_nucleotide < 1 || start_nucleotide > yeast_chromosomes[sel_chrom - 1].size() );
		

		
		unsigned int end_nucleotide = 0;
		
		do
		{
			end_nucleotide = prompt_for_input<unsigned int>( "Selected End Nucleotide:      ", PRINT_PROMPT_TO_LOG );
			//output_text_line( "" );
			
		} while( end_nucleotide < 1 || end_nucleotide > yeast_chromosomes[sel_chrom - 1].size() );
		
		string sequence;
		
		if( end_nucleotide >= start_nucleotide )
		{
			sequence = yeast_chromosomes[sel_chrom - 1].substr( start_nucleotide - 1, ( end_nucleotide - start_nucleotide + 1 ) );
		}
		else/*( end_nucleotide < start_nucleotide )*/
		{
			sequence = yeast_chromosomes[sel_chrom - 1].substr( end_nucleotide - 1, ( start_nucleotide - end_nucleotide + 1 ) );
			
			sequence = get_inverted_sequence( sequence );
			sequence = get_opposite_strand  ( sequence );
		}
		
		
		output_text_line( "" );
		output_text_line( (string)"Sequence:                     " + sequence );
		output_text_line( "" );
		
		string select_new;
		do
		{
			select_new = prompt_for_input<string>( "Select a new sequence? ('Y'/'N'): ", PRINT_PROMPT_TO_LOG );
			output_text_line( "" );
			
		} while( select_new != "Y" && select_new != "y" && select_new != "N" && select_new != "n" );
		
		output_text_line( "" );
		
		if    ( select_new == "Y" || select_new == "y" )  { request_another_seq = TRUE;  }
		else/*( select_new == "N" || select_new == "n" )*/{ request_another_seq = FALSE; }
		
	} while( request_another_seq == TRUE );
	
	return;
}
//==============================================================================



// Note that this interface is 1-indexed to reflect coordinates in the chromosome, rather than iterators in the string
string extract_DNA_seq( const string & chrom_seq,
						unsigned int start_coord, 
						const unsigned int len, 
						const bool opposite_strand, 
						const vector <TFeature> untransl_reg, 
						const bool remove_untransl_reg_in_first_codon )
{
	if( len == 0 ) { return ""; }
	
	unsigned int first_coord_it;
	if	  ( opposite_strand == FALSE )  { first_coord_it = start_coord - 1; 			  }
	else/*( opposite_strand == TRUE )*/ { first_coord_it = start_coord - 1 - ( len - 1 ); }
	
	unsigned long last_coord_it = first_coord_it + len - 1;
	if( last_coord_it >= chrom_seq.size() )
	{
		Errors.handle_error( FATAL, "Error in 'extract_DNA_seq()': Sequence specified was out of range. ", __FILE__, __LINE__ );
	}
	
	
	string sequence = "";
	
	// If there aren't any untranslated regions, the sequence can be extracted as is
	if( untransl_reg.empty() == TRUE )
	{
		sequence = chrom_seq.substr( first_coord_it, len );
	}
	
	// If there are any untranslated regions, check if they interfere with the sequence, then remove them
	else
	{
		vector <TFeature> interfering_reg;
	
		unsigned int len_w_untrans = len;
		
		// An untranslated region interferes with the sequence if it starts between the sequence start and the place the sequence would end
		//  in the absence of any untranslated regions
		for( unsigned int i = 0; i < untransl_reg.size(); i++ )
		{
			
			bool untransl_reg_start_after_seq_start  = FALSE;
			bool untransl_reg_start_before_seq_end   = FALSE;
			
			int search_start_adj = ( remove_untransl_reg_in_first_codon == TRUE ) ? 0 : NUM_NT_TO_COMPLETE_CODON;
			
			if( ( opposite_strand == FALSE && ( (int)start_coord + search_start_adj - (int)untransl_reg[i].start_coord ) < 0 ) ||
				( opposite_strand == TRUE  && ( (int)start_coord - search_start_adj - (int)untransl_reg[i].start_coord ) > 0 ) ) 
			{
				untransl_reg_start_after_seq_start = TRUE;
			}
			
			if( ( opposite_strand == FALSE && ( (int)start_coord + (int)( len_w_untrans - 1 ) - (int)untransl_reg[i].start_coord ) >= 0 ) ||
				( opposite_strand == TRUE  && ( (int)start_coord - (int)( len_w_untrans - 1 ) - (int)untransl_reg[i].start_coord ) <= 0 ) )
			{
				untransl_reg_start_before_seq_end = TRUE;
			}
			

			if( untransl_reg_start_after_seq_start == TRUE && untransl_reg_start_before_seq_end == TRUE )
			{
				interfering_reg.push_back( untransl_reg[i] );
				len_w_untrans += untransl_reg[i].length;
			}
		}


		// Calculate the iterators for the first and last coordinates of the sequence once the untranslated regions are removed
		if	  ( opposite_strand == FALSE )  { last_coord_it  = first_coord_it + ( len_w_untrans - 1 ); }
		else/*( opposite_strand == TRUE  )*/{ first_coord_it = last_coord_it  - ( len_w_untrans - 1 ); }
	

		// Add the portions of the sequence iteratively by extracting the regions between/before/after the untranslated regions
		unsigned int section_first_coord_it = first_coord_it;
		
		unsigned int section_length = len;
		if( interfering_reg.empty() == FALSE )
		{
			if( opposite_strand == FALSE )
			{
				section_length = abs( ( (int)section_first_coord_it + 1 ) - (int)interfering_reg[0].start_coord );
			}
			else/*( opposite_strand == TRUE )*/
			{
				section_first_coord_it = (int)interfering_reg[0].start_coord;
				section_length 		   = last_coord_it - section_first_coord_it + 1;
			}
		}
			

		for( unsigned int i = 0; i < interfering_reg.size() + 1; i++ )
		{
			// First add the sequence between the start coordinate and the farthest upstream untranslated region (section length and start coordinate calculated above
			
			// Then add the sequences between the untranslated regions
			if( i > 0 )
			{
				if( i < interfering_reg.size() )
				{
					if( opposite_strand == FALSE )
					{
						section_first_coord_it = interfering_reg[i - 1].start_coord + interfering_reg[i - 1].length - 1;
					}
					else/*( opposite_strand == TRUE )*/
					{
						section_first_coord_it = interfering_reg[i].start_coord;
					}
						
					section_length = abs( (int)interfering_reg[i].start_coord - (int)interfering_reg[i - 1].start_coord ) - interfering_reg[i - 1].length;
				}
				
				// Finally add the sequence between the farthest downstream unstranslated region and the sequence end coordinate
				else/*( i == interfering_reg.size() )*/
				{
					if( opposite_strand == FALSE )
					{
						section_first_coord_it = interfering_reg[i - 1].start_coord + interfering_reg[i - 1].length - 1;
						section_length = ( last_coord_it + 1 ) - interfering_reg[i - 1].start_coord - interfering_reg[i - 1].length + 1;
					}
					else/*( opposite_strand == TRUE )*/
					{
						section_first_coord_it = first_coord_it;
						section_length = interfering_reg[i - 1].start_coord - interfering_reg[i - 1].length - ( first_coord_it + 1 ) + 1;
					}
				}
			}

			
			// Add the iterative sequence
			if	  ( opposite_strand == FALSE )  { sequence = sequence + chrom_seq.substr( section_first_coord_it, section_length ); }
			else/*( opposite_strand == FALSE )*/{ sequence = chrom_seq.substr( section_first_coord_it, section_length ) + sequence; }
		}
	}
	
	if( opposite_strand == TRUE )
	{
		sequence = get_inverted_sequence( sequence );
		sequence = get_opposite_strand  ( sequence );
	}
	
	
	return sequence;
}
//==============================================================================



string get_inverted_sequence( const string seq_to_invert )
{
	string inverted_seq = seq_to_invert;
	for( unsigned int i = 0; i < seq_to_invert.size(); i++ )
	{
		inverted_seq[i] = seq_to_invert[seq_to_invert.size() - 1 - i];
	}
	
	return inverted_seq;
}
//==============================================================================



string get_opposite_strand( const string strand )
{
	string opposite_strand = strand;
	
	for( unsigned int i = 0; i < strand.size(); i++ )
	{
		switch( strand[i] )
		{
			case 'A' : 	opposite_strand[i] = 'T';
						break;
						
			case 'T' : 	opposite_strand[i] = 'A';
						break;
						
			case 'C' : 	opposite_strand[i] = 'G';
						break;
						
			case 'G' : 	opposite_strand[i] = 'C';
						break;
						
			default  : 	break;
		}
	}
	
	return opposite_strand;
}
//==============================================================================



string convert_to_AA_seq( const string & DNA_seq )
{
	if( DNA_seq.size() % 3 != 0 )
	{
		Errors.handle_error( FATAL, "Error in 'convert_to_AA_seq()': DNA sequence to convert must have a multiple of three nucleotides. ", __FILE__, __LINE__ );
	}
	
	
	string AA_seq( DNA_seq.size() / 3, '-' ); 
	
	for( unsigned int i = 0; i < AA_seq.size(); i++ )
	{
		string codon = DNA_seq.substr( 3 * i, 3 );

		if     ( codon == "TTT" ) { AA_seq[i] = 'F'; }
		else if( codon == "TTC" ) { AA_seq[i] = 'F'; } 	
		else if( codon == "TTA" ) { AA_seq[i] = 'L'; }	
		else if( codon == "TTG" ) { AA_seq[i] = 'L'; }	
		
		else if( codon == "TCT" ) { AA_seq[i] = 'S'; }
		else if( codon == "TCC" ) { AA_seq[i] = 'S'; } 	
		else if( codon == "TCA" ) { AA_seq[i] = 'S'; }	
		else if( codon == "TCG" ) { AA_seq[i] = 'S'; }	
		
		else if( codon == "TAT" ) { AA_seq[i] = 'Y'; }
		else if( codon == "TAC" ) { AA_seq[i] = 'Y'; } 	
		else if( codon == "TAA" ) { AA_seq[i] = '*'; }	
		else if( codon == "TAG" ) { AA_seq[i] = '*'; }	
		
		else if( codon == "TGT" ) { AA_seq[i] = 'C'; }
		else if( codon == "TGC" ) { AA_seq[i] = 'C'; } 	
		else if( codon == "TGA" ) { AA_seq[i] = '*'; }	
		else if( codon == "TGG" ) { AA_seq[i] = 'W'; }	
		
		
		
		else if( codon == "CTT" ) { AA_seq[i] = 'L'; }
		else if( codon == "CTC" ) { AA_seq[i] = 'L'; } 	
		else if( codon == "CTA" ) { AA_seq[i] = 'L'; }	
		else if( codon == "CTG" ) { AA_seq[i] = 'L'; }	
													
		else if( codon == "CCT" ) { AA_seq[i] = 'P'; }
		else if( codon == "CCC" ) { AA_seq[i] = 'P'; } 	
		else if( codon == "CCA" ) { AA_seq[i] = 'P'; }	
		else if( codon == "CCG" ) { AA_seq[i] = 'P'; }	
													
		else if( codon == "CAT" ) { AA_seq[i] = 'H'; }
		else if( codon == "CAC" ) { AA_seq[i] = 'H'; } 	
		else if( codon == "CAA" ) { AA_seq[i] = 'Q'; }	
		else if( codon == "CAG" ) { AA_seq[i] = 'Q'; }	
													
		else if( codon == "CGT" ) { AA_seq[i] = 'R'; }
		else if( codon == "CGC" ) { AA_seq[i] = 'R'; } 	
		else if( codon == "CGA" ) { AA_seq[i] = 'R'; }	
		else if( codon == "CGG" ) { AA_seq[i] = 'R'; }	
		
		
		
		else if( codon == "ATT" ) { AA_seq[i] = 'I'; }
		else if( codon == "ATC" ) { AA_seq[i] = 'I'; } 	
		else if( codon == "ATA" ) { AA_seq[i] = 'I'; }	
		else if( codon == "ATG" ) { AA_seq[i] = 'M'; }	
													
		else if( codon == "ACT" ) { AA_seq[i] = 'T'; }
		else if( codon == "ACC" ) { AA_seq[i] = 'T'; } 	
		else if( codon == "ACA" ) { AA_seq[i] = 'T'; }	
		else if( codon == "ACG" ) { AA_seq[i] = 'T'; }	
													
		else if( codon == "AAT" ) { AA_seq[i] = 'N'; }
		else if( codon == "AAC" ) { AA_seq[i] = 'N'; } 	
		else if( codon == "AAA" ) { AA_seq[i] = 'K'; }	
		else if( codon == "AAG" ) { AA_seq[i] = 'K'; }	
													
		else if( codon == "AGT" ) { AA_seq[i] = 'S'; }
		else if( codon == "AGC" ) { AA_seq[i] = 'S'; } 	
		else if( codon == "AGA" ) { AA_seq[i] = 'R'; }	
		else if( codon == "AGG" ) { AA_seq[i] = 'R'; }	
		
	
		
		else if( codon == "GTT" ) { AA_seq[i] = 'V'; }
		else if( codon == "GTC" ) { AA_seq[i] = 'V'; } 	
		else if( codon == "GTA" ) { AA_seq[i] = 'V'; }	
		else if( codon == "GTG" ) { AA_seq[i] = 'V'; }	
													
		else if( codon == "GCT" ) { AA_seq[i] = 'A'; }
		else if( codon == "GCC" ) { AA_seq[i] = 'A'; } 	
		else if( codon == "GCA" ) { AA_seq[i] = 'A'; }	
		else if( codon == "GCG" ) { AA_seq[i] = 'A'; }	
													
		else if( codon == "GAT" ) { AA_seq[i] = 'D'; }
		else if( codon == "GAC" ) { AA_seq[i] = 'D'; } 	
		else if( codon == "GAA" ) { AA_seq[i] = 'E'; }	
		else if( codon == "GAG" ) { AA_seq[i] = 'E'; }	
													
		else if( codon == "GGT" ) { AA_seq[i] = 'G'; }
		else if( codon == "GGC" ) { AA_seq[i] = 'G'; } 	
		else if( codon == "GGA" ) { AA_seq[i] = 'G'; }	
		else if( codon == "GGG" ) { AA_seq[i] = 'G'; }	
		
		else
		{
			Errors.handle_error( FATAL, "Error in 'convert_to_AA_seq()': DNA sequence not recognized. ", __FILE__, __LINE__ );
		}
	}
	
	return AA_seq;
}
//==============================================================================



bool check_if_AA_seq_matches( const string & DNA_seq, const string & AA_seq )
{	
	if( DNA_seq.size() != 3 * AA_seq.size() )
	{
		Errors.handle_error( FATAL, "Error in 'check_if_AA_seq_matches()': DNA sequence length did not correspond to the length of the AA sequence. ", __FILE__, __LINE__ );
	}
	
	return ( convert_to_AA_seq( DNA_seq ) == AA_seq );
}
//==============================================================================



// Note that this is 1-indexed to reflect the convention for DNA sequence coordinates 
unsigned int find_AA_seq( const string & DNA_seq, const string & AA_seq, unsigned int start_coord )
{
	unsigned int AA_seq_start_coord = DNA_seq.size() + 1;
	
	
	while( start_coord + ( AA_seq.size() * 3 ) - 1 <= DNA_seq.size() && AA_seq_start_coord == DNA_seq.size() + 1 )
	{
		if( AA_seq == convert_to_AA_seq( DNA_seq.substr( start_coord - 1, AA_seq.size() * 3 ) ) )
		{
			AA_seq_start_coord = start_coord;
		}
		
		start_coord++;
	}

	return AA_seq_start_coord;
}
//==============================================================================



string cap_start_codon_only( const string & start_codon_context )
{
	string start_context_mod = start_codon_context;
	
	cap_or_uncap_str( &start_context_mod, UNCAPITALIZE );
	cap_or_uncap_str( &start_context_mod, CAPITALIZE, START_CONTEXT_NT_BEFORE_AUG, 3 );
	
	return start_context_mod;
}
//==============================================================================



int get_gene_coord_from_gff( const string & gene_name, 
							 const string & gff_annotation,
							 const bool opposite_strand,
							 unsigned int * const gene_start_coord, 
							 unsigned int * const gene_end_coord, 
							 unsigned int * const  gene_intergen_start_coord,
							 vector <TFeature> * const untransl_reg,
							 TFeature * const fpUTR_intron )
{
	// ------------------------------
	// Find the start coordinate of the gene's first CDS
	unsigned int first_gene_cds_pos = gff_annotation.find ( (string)"Name=" + gene_name + "_CDS;" );
	

	if( first_gene_cds_pos == string::npos ) 
	{
		ostringstream error_oss;
		error_oss << "Error in 'get_gene_coord_from_gff()': Gene name " << gene_name << " not found";
		return Errors.handle_error( PASS_UP_ONE_LEVEL, error_oss.str(), __FILE__, __LINE__ );
	}
	

	unsigned int first_CDS_id_pos = gff_annotation.rfind( "SGD	CDS", first_gene_cds_pos );
	
	
	if( first_CDS_id_pos == string::npos || first_CDS_id_pos < gff_annotation.rfind( "\n", first_gene_cds_pos ) ) 
	{
		ostringstream error_oss;
		error_oss << "Error in 'get_gene_coord_from_gff()': First Gene CDS identifier for " << gene_name << " not found\n"
				  << "gene cds pos: " << first_gene_cds_pos << "  cds id pos: " << first_CDS_id_pos << "\n"
				  << "gff annotation: \n" << gff_annotation.substr( first_CDS_id_pos, (first_gene_cds_pos - first_CDS_id_pos + 1 ) );
		
		return Errors.handle_error( PASS_UP_ONE_LEVEL, error_oss.str(), __FILE__, __LINE__ );
	}
	
		
	string first_cds_info = gff_annotation.substr( first_CDS_id_pos, ( first_gene_cds_pos - first_CDS_id_pos + 1 ) );
	
	
	unsigned int gene_first_coord_start_pos = find_first_digit_or_char( DIGIT, first_cds_info );
	
	if( gene_first_coord_start_pos == string::npos ) 
	{
		ostringstream error_oss;
		error_oss << "Error in 'get_gene_coord_from_gff()': First coordinate of gene " << gene_name << " not found";
		return Errors.handle_error( PASS_UP_ONE_LEVEL, error_oss.str(), __FILE__, __LINE__ );
	}
	
	unsigned int gene_first_coord_end_pos = find_last_digit_or_char( DIGIT, first_cds_info, gene_first_coord_start_pos );
	unsigned int gene_first_coord_len = gene_first_coord_end_pos - gene_first_coord_start_pos + 1;
	// ------------------------------
	
	
	
	// ------------------------------
	// Find the end coordinate of the gene's last CDS
	unsigned int last_gene_cds_pos  = gff_annotation.rfind( (string)"Name=" + gene_name + "_CDS;" );
	if( last_gene_cds_pos == string::npos ) 
	{
		ostringstream error_oss;
		error_oss << "Error in 'get_gene_coord_from_gff()': Gene name " << gene_name << " not found";
		return Errors.handle_error( PASS_UP_ONE_LEVEL, error_oss.str(), __FILE__, __LINE__ );
	}
	
	unsigned int last_CDS_id_pos  = gff_annotation.rfind( "SGD	CDS", last_gene_cds_pos );
	
	if( last_CDS_id_pos == string::npos || last_CDS_id_pos < gff_annotation.rfind( "\n", last_gene_cds_pos ) )
	{
		ostringstream error_oss;
		error_oss << "Error in 'get_gene_coord_from_gff()': Last Gene CDS identifier for " << gene_name << " not found in chromosome\n"
				  << "gene cds pos: " << last_gene_cds_pos << "  cds id pos: " << last_CDS_id_pos << "\n"
				  << "gff annotation: \n" << gff_annotation.substr( last_CDS_id_pos, ( last_gene_cds_pos - last_CDS_id_pos + 1 ) );
		
		return Errors.handle_error( PASS_UP_ONE_LEVEL, error_oss.str(), __FILE__, __LINE__ );
	}
	
	string last_cds_info  = gff_annotation.substr( last_CDS_id_pos,  ( last_gene_cds_pos  - last_CDS_id_pos  + 1 ) );
	
	unsigned int last_cds_first_coord_start_pos = find_first_digit_or_char( DIGIT, last_cds_info );
	unsigned int last_cds_first_coord_end_pos   = find_last_digit_or_char( DIGIT, last_cds_info, last_cds_first_coord_start_pos );
	
	
	unsigned int gene_second_coord_start_pos = find_first_digit_or_char( DIGIT, last_cds_info, ( last_cds_first_coord_end_pos + 1 ) );
	
	if( gene_second_coord_start_pos == string::npos ) 
	{
		ostringstream error_oss;
		error_oss << "Error in 'get_gene_coord_from_gff()': Last coordinate of gene " << gene_name << " not found";
		return Errors.handle_error( PASS_UP_ONE_LEVEL, error_oss.str(), __FILE__, __LINE__ );
	}
	
	
	unsigned int gene_second_coord_end_pos = find_last_digit_or_char ( DIGIT, last_cds_info, gene_second_coord_start_pos );
	unsigned int gene_second_coord_len 	   = gene_second_coord_end_pos - gene_second_coord_start_pos + 1;
	// ------------------------------
	
	
	
	// ------------------------------
	// Based on the strand, determine the gene's start and end coordinates
	if( opposite_strand == FALSE )
	{
		// If the gene is on the Watson strand, the first coordinate of the first CDS is the start of the translated gene
		*gene_start_coord = atoi( first_cds_info.substr( gene_first_coord_start_pos, gene_first_coord_len   ).c_str() );
		*gene_end_coord   = atoi( last_cds_info.substr ( gene_second_coord_start_pos, gene_second_coord_len ).c_str() );	
	}
	else/*( opposite_strand == TRUE )*/
	{
		// If the gene is on the Crick strand, the second coordinate of the last CDS is the start of the gene
		*gene_start_coord = atoi( last_cds_info.substr ( gene_second_coord_start_pos, gene_second_coord_len ).c_str() );	
		*gene_end_coord   = atoi( first_cds_info.substr( gene_first_coord_start_pos,  gene_first_coord_len  ).c_str() );
	}
	// ------------------------------
	
	
	
	// ------------------------------
	// Now extract the upper bound of the upstream intergenic region
	//  If the first CDS on the chromosome (either C or W strand), take the start of the intergenic region to be the start of the chromosome.
	//  If the previous CDS overlaps with the CDS of interest, iteratively search for previous CDS's until one is found that doesn't overlap,
	//  considering this to be the previous CDS.
	//  Once the previous CDS is found, determine the start of the intergenic region by bisecting the intergenic region between the previous
	//  CDS and the CDS of interest.
	*gene_intergen_start_coord = *gene_start_coord;
	unsigned int prev_cds_id_pos = first_CDS_id_pos;
	
	while( *gene_intergen_start_coord == *gene_start_coord )
	{
		string prev_cds_info;
		
		if( opposite_strand == FALSE )
		{
			prev_cds_id_pos = gff_annotation.rfind( "SGD	CDS", prev_cds_id_pos - 1 );
			if    ( prev_cds_id_pos == string::npos )   { *gene_intergen_start_coord = 1; 								}
			else/*( prev_cds_id_pos == string::npos )*/ { prev_cds_info = gff_annotation.substr( prev_cds_id_pos, 50 ); }
		}
		else/*( opposite_strand == TRUE )*/
		{
			prev_cds_id_pos = gff_annotation.find(  "SGD	CDS", prev_cds_id_pos + 1 );
			if( prev_cds_id_pos == string::npos ) 
			{ 
				prev_cds_id_pos = gff_annotation.rfind( "SGD	chromosome", prev_cds_id_pos - 1 ); 
				
				// Flag that the start of the intergic region should be the last coordinate of the chromosome by marking the value as 1
				*gene_intergen_start_coord = 1;
			}
			
			prev_cds_info = gff_annotation.substr( prev_cds_id_pos, 50 );
		}

		
		if( prev_cds_info.size() != 0 )
		{
			unsigned int prev_cds_first_coord_start_pos  = find_first_digit_or_char( DIGIT, prev_cds_info 			    	 				);
			unsigned int prev_cds_first_coord_end_pos    = find_last_digit_or_char ( DIGIT, prev_cds_info, prev_cds_first_coord_start_pos   );
			unsigned int prev_cds_second_coord_start_pos = find_first_digit_or_char( DIGIT, prev_cds_info, prev_cds_first_coord_end_pos + 1 );
			unsigned int prev_cds_second_coord_end_pos   = find_last_digit_or_char ( DIGIT, prev_cds_info, prev_cds_second_coord_start_pos  );
			
			unsigned int prev_cds_second_coord = atoi( prev_cds_info.substr ( prev_cds_second_coord_start_pos, prev_cds_second_coord_end_pos - prev_cds_second_coord_start_pos + 1 ).c_str() );
			unsigned int prev_cds_first_coord  = atoi( prev_cds_info.substr ( prev_cds_first_coord_start_pos,  prev_cds_first_coord_end_pos  - prev_cds_first_coord_start_pos  + 1 ).c_str() );
			
			if( opposite_strand == FALSE )
			{
				if	  ( prev_cds_first_coord > *gene_start_coord )   { *gene_intergen_start_coord = *gene_start_coord; }
				else/*( prev_cds_first_coord > *gene_start_coord )*/ { *gene_intergen_start_coord = ( prev_cds_second_coord + *gene_start_coord ) / 2 + ( ( prev_cds_second_coord + *gene_start_coord ) % 2 == 0 ? 0 : 1 ) ; }
			}
			else/*( opposite_strand == TRUE )*/
			{
				if	   ( *gene_intergen_start_coord == 1 				) { *gene_intergen_start_coord = prev_cds_second_coord; 						   }
				else if( *gene_intergen_start_coord < *gene_start_coord ) { *gene_intergen_start_coord = *gene_start_coord;    							   }
				else 													  { *gene_intergen_start_coord = ( prev_cds_first_coord + *gene_start_coord ) / 2; }
			}
		}
	}
	// ------------------------------

	
	
	// ------------------------------
	// Now extract the intron information, if it exists and is relevant
	untransl_reg->clear();
	
	// If there is more than one CDS, there must be at least one intron
	if( last_gene_cds_pos != first_gene_cds_pos )
	{
		string untransl_reg_info;
		bool all_untransl_reg_extracted = FALSE;
		unsigned int prev_untransl_pos = 0;
		
		do
		{
			// Find the appropriate identifiers for the intron in the .gff and extract a substring containing the intron coordinates
			unsigned int gene_intron_pos      = gff_annotation.find(  (string)"Name=" + gene_name + "_intron;", 						 prev_untransl_pos + 1 );
			unsigned int gene_frameshift_pos  = gff_annotation.find(  (string)"Name=" + gene_name + "_plus_1_translational_frameshift;", prev_untransl_pos + 1 );
			
			unsigned int gene_untransl_pos;
			
			if	   ( gene_intron_pos     == string::npos ) { gene_untransl_pos = gene_frameshift_pos; }
			else if( gene_frameshift_pos == string::npos ) { gene_untransl_pos = gene_intron_pos;     }
			else
			{
				gene_untransl_pos = ( gene_intron_pos < gene_frameshift_pos ? gene_intron_pos : gene_frameshift_pos );
			}

			
			if( gene_untransl_pos == string::npos ) 
			{
				if( untransl_reg->size() == 0 )
				{
					ostringstream error_oss;
					error_oss << "Error in 'get_gene_coord_from_gff()': No introns or translational frameshifts found in gene " << gene_name 
							  << " (though there were indicated by a split CDS). ";
							  
					return Errors.handle_error( PASS_UP_ONE_LEVEL, error_oss.str(), __FILE__, __LINE__ );
				}
				else
				{
					all_untransl_reg_extracted = TRUE;
				}
			}
			else
			{
				unsigned int nontran_id_pos = 0;
				
				if( gene_intron_pos < gene_frameshift_pos )
				{
					nontran_id_pos = gff_annotation.rfind( "SGD	intron", gene_untransl_pos );
				}
				else/*( gene_intron_pos >= gene_frameshift_pos )*/
				{
					nontran_id_pos = gff_annotation.rfind( "SGD	plus_1_translational_frameshift", gene_untransl_pos );
				}
				
				
				if( nontran_id_pos == string::npos || nontran_id_pos < gff_annotation.rfind( "\n", gene_untransl_pos ) ) 
				{
					ostringstream error_oss;
					error_oss << "Error in 'get_gene_coord_from_gff()': Gene non-translated region identifier for " << gene_name << " not found. ";
					return Errors.handle_error( PASS_UP_ONE_LEVEL, error_oss.str(), __FILE__, __LINE__ );
				}
				
					
				untransl_reg_info = gff_annotation.substr( nontran_id_pos, ( gene_untransl_pos - nontran_id_pos + 1 ) );
				
				
				// Extract the start and end coordinates of the non-translated region
				unsigned int untransl_first_coord_start_pos = find_first_digit_or_char( DIGIT, untransl_reg_info );
				
				if( untransl_first_coord_start_pos == string::npos ) 
				{
					ostringstream error_oss;
					error_oss << "Error in 'get_gene_coord_from_gff()': First coordinate of non-translated region for gene " << gene_name << " not found. ";
					return Errors.handle_error( PASS_UP_ONE_LEVEL, error_oss.str(), __FILE__, __LINE__ );
				}
				
				unsigned int untransl_first_coord_end_pos = find_last_digit_or_char( DIGIT, untransl_reg_info, untransl_first_coord_start_pos );
				
				unsigned int untransl_first_coord_len = untransl_first_coord_end_pos - untransl_first_coord_start_pos + 1;
				unsigned int untransl_first_coord     = atoi( untransl_reg_info.substr( untransl_first_coord_start_pos, untransl_first_coord_len ).c_str() );
				
				
				// Extract the second coordinate
				unsigned int untransl_second_coord_start_pos = find_first_digit_or_char( DIGIT, untransl_reg_info, untransl_first_coord_end_pos + 1 );
				
				if( untransl_second_coord_start_pos == string::npos ) 
				{
					ostringstream error_oss;
					error_oss << "Error in 'get_gene_coord_from_gff()': Second coordinate of non-translated region for gene " << gene_name << " not found. ";
					return Errors.handle_error( PASS_UP_ONE_LEVEL, error_oss.str(), __FILE__, __LINE__ );
				}
				
				unsigned int untransl_second_coord_end_pos = find_last_digit_or_char( DIGIT, untransl_reg_info, untransl_second_coord_start_pos );
				
				unsigned int untransl_second_coord_len = untransl_second_coord_end_pos - untransl_second_coord_start_pos + 1;
				unsigned int untransl_second_coord     = atoi( untransl_reg_info.substr( untransl_second_coord_start_pos, untransl_second_coord_len ).c_str() );
				
				
				// Pass the non-translated region start and length by reference (only if the non-translated region interferes with the gene's start codon context
				if( opposite_strand == FALSE )  
				{ 
					untransl_reg->push_back( TFeature( untransl_first_coord, untransl_second_coord - untransl_first_coord + 1 ) );  
				}
				else/*( opposite_strand == TRUE )*/ 
				{ 
					untransl_reg->push_back( TFeature( untransl_second_coord, untransl_second_coord - untransl_first_coord + 1 ) );  
				}
			}	
				
			prev_untransl_pos = gene_untransl_pos;
			
		} while( all_untransl_reg_extracted == FALSE );
		
		
		// In order to put in upstream-downstream order, if the gene is on the Crick strand, reverse the order of the vectors
		if( opposite_strand == TRUE )
		{
			reverse( untransl_reg->begin(), untransl_reg->end() );
		}
	}
	// ------------------------------
	
	
	
	// ------------------------------
	// Now extract the 5' UTR intron information, if it exists (assume there's only one)
	*fpUTR_intron = TFeature( string::npos, string::npos );
	

	// Look for the appropriate identifier for the 5' UTR intron in the .gff
	unsigned int gene_fpUTR_intron_pos = gff_annotation.find( (string)"Name=" + gene_name + "_five_prime_UTR_intron;" );
		
	if( gene_fpUTR_intron_pos != string::npos ) 
	{
		// As of 2014-06-02, the most recent GFF file did not have more than one 5' UTR intron annotation for any gene
		if( gff_annotation.find( (string)"Name=" + gene_name + "_five_prime_UTR_intron;", gene_fpUTR_intron_pos + 1 ) != string::npos )
		{
			ostringstream error_oss;
			error_oss << "Error in 'get_gene_coord_from_gff()': More than one 5' UTR intron found for " << gene_name << ". ";
			return Errors.handle_error( PASS_UP_ONE_LEVEL, error_oss.str(), __FILE__, __LINE__ );
		}
		
		
		unsigned int fpUTR_intron_id_pos = gff_annotation.rfind( "SGD	five_prime_UTR_intron", gene_fpUTR_intron_pos );
		
		if( fpUTR_intron_id_pos == string::npos || fpUTR_intron_id_pos < gff_annotation.rfind( "\n", gene_fpUTR_intron_pos ) ) 
		{
			ostringstream error_oss;
			error_oss << "Error in 'get_gene_coord_from_gff()': Gene 5' UTR intron identifier for " << gene_name << " not found. ";
			return Errors.handle_error( PASS_UP_ONE_LEVEL, error_oss.str(), __FILE__, __LINE__ );
		}
		
			
		string fpUTR_intron_info = gff_annotation.substr( fpUTR_intron_id_pos, ( gene_fpUTR_intron_pos - fpUTR_intron_id_pos + 1 ) );
	
		
		
		// Extract the start and end coordinates of the non-translated region
		unsigned int fpUTR_intron_first_coord_start_pos = find_first_digit_or_char( DIGIT, fpUTR_intron_info );
		
		if( fpUTR_intron_first_coord_start_pos == string::npos ) 
		{
			ostringstream error_oss;
			error_oss << "Error in 'get_gene_coord_from_gff()': First coordinate of 5' UTR intron for gene " << gene_name << " not found. ";
			return Errors.handle_error( PASS_UP_ONE_LEVEL, error_oss.str(), __FILE__, __LINE__ );
		}
		
		unsigned int fpUTR_intron_first_coord_end_pos = find_last_digit_or_char( DIGIT, fpUTR_intron_info, fpUTR_intron_first_coord_start_pos );
		
		unsigned int fpUTR_intron_first_coord_len = fpUTR_intron_first_coord_end_pos - fpUTR_intron_first_coord_start_pos + 1;
		unsigned int fpUTR_intron_first_coord     = atoi( fpUTR_intron_info.substr( fpUTR_intron_first_coord_start_pos, fpUTR_intron_first_coord_len ).c_str() );
		
		
		// Extract the second coordinate
		unsigned int fpUTR_intron_second_coord_start_pos = find_first_digit_or_char( DIGIT, fpUTR_intron_info, fpUTR_intron_first_coord_end_pos + 1 );
		
		if( fpUTR_intron_second_coord_start_pos == string::npos ) 
		{
			ostringstream error_oss;
			error_oss << "Error in 'get_gene_coord_from_gff()': Second coordinate of 5' UTR intron for gene " << gene_name << " not found. ";
			return Errors.handle_error( PASS_UP_ONE_LEVEL, error_oss.str(), __FILE__, __LINE__ );
		}
		
		unsigned int fpUTR_intron_second_coord_end_pos = find_last_digit_or_char( DIGIT, fpUTR_intron_info, fpUTR_intron_second_coord_start_pos );
		
		unsigned int fpUTR_intron_second_coord_len = fpUTR_intron_second_coord_end_pos - fpUTR_intron_second_coord_start_pos + 1;
		unsigned int fpUTR_intron_second_coord     = atoi( fpUTR_intron_info.substr( fpUTR_intron_second_coord_start_pos, fpUTR_intron_second_coord_len ).c_str() );
		
		
		// Pass the non-translated region start and length by reference (only if the non-translated region interferes with the gene's start codon context
		if( opposite_strand == FALSE )  
		{ 
			*fpUTR_intron = TFeature( fpUTR_intron_first_coord, fpUTR_intron_second_coord - fpUTR_intron_first_coord + 1 );  
		}
		else/*( opposite_strand == TRUE )*/ 
		{ 
			*fpUTR_intron = TFeature( fpUTR_intron_second_coord, fpUTR_intron_second_coord - fpUTR_intron_first_coord + 1 );  
		}
	}
	// ------------------------------

	
	return SUCCESSFUL;
}
//==============================================================================



// Note that this interface is 1-indexed to reflect coordinates in the chromosome, rather than iterators in the string
unsigned int find_first_stop_codon( const string & chomosome, 
									unsigned int start_pos, 
									bool opposite_strand, 
									unsigned int * const len, 
									const vector <TFeature> removed_reg
									/*bool search_in_frame = TRUE*/ )
{
	if( start_pos > chomosome.size() )
	{
		Errors.handle_error( FATAL, (string)"Error in 'find_first_stop_codon()': Specified start position exceeded the length of the passed chromosome", __FILE__, __LINE__ );
	}

	unsigned int end_pos 			= string::npos;
	unsigned int pos_to_search 		= start_pos;
	unsigned int prev_pos_to_search = start_pos;
	unsigned int removed_reg_it		= removed_reg.size();
	int seq_length = 0;
	
	const unsigned int NUM_NT_TO_INCREMENT = 3 /*( search_in_frame == TRUE ? 3 : 1 )*/;
	

	if( removed_reg.empty() == TRUE )
	{
		// Note: the algorithm below is used in favor of the shorter version commented out because of the additional time costs of 'extract_DNA_seq()' 
		//   (1-2 seconds over the course of the program)
		if( opposite_strand == FALSE )
		{
			// Start with the codon after the start codon
			pos_to_search += NUM_NT_TO_INCREMENT; 
			
			do 
			{
				// See if the current codon is a stop codon (subtract 1 to 0-index)
				if( ( chomosome.substr( pos_to_search - 1, 3 ) == "TAA" ) ||
					( chomosome.substr( pos_to_search - 1, 3 ) == "TAG" ) ||
					( chomosome.substr( pos_to_search - 1, 3 ) == "TGA" ) )
				{
					// End position is 2 past the start of the stop codon
					end_pos = pos_to_search + NUM_NT_TO_COMPLETE_CODON;
				}
			
				pos_to_search += NUM_NT_TO_INCREMENT;
				
			} while( pos_to_search <= chomosome.size() && end_pos == string::npos );
		}
		else/*( opposite_strand == TRUE )*/
		{
			// Start with the codon after the start codon
			pos_to_search -= ( NUM_NT_TO_INCREMENT + NUM_NT_TO_COMPLETE_CODON );
			
			do 
			{
				// See if the current codon is a stop codon (subtract 1 to 0-index)
				if( ( chomosome.substr( pos_to_search - 1, 3 ) == "TTA" ) ||
					( chomosome.substr( pos_to_search - 1, 3 ) == "CTA" ) ||
					( chomosome.substr( pos_to_search - 1, 3 ) == "TCA" ) )
				{
					end_pos = pos_to_search;
				}
			
				pos_to_search -= NUM_NT_TO_INCREMENT;
				
			} while( pos_to_search - 1 < chomosome.size() && end_pos == string::npos );
		}
		
		/*
		do
		{
			pos_to_search += ( NUM_NT_TO_INCREMENT * ( opposite_strand == FALSE ? 1 : -1 ) );
			
			string searched_codon = extract_DNA_seq( chomosome, pos_to_search, 3, opposite_strand );

			if( ( searched_codon == "TAA" ) ||
				( searched_codon == "TAG" ) ||
				( searched_codon == "TGA" ) )
			{
				end_pos = pos_to_search + ( NUM_NT_TO_COMPLETE_CODON * ( opposite_strand == FALSE ? 1 : -1 ) );
			}
		} while( pos_to_search - 1 < chomosome.size() && end_pos == string::npos );
		*/
		
		seq_length = abs( (int)end_pos - (int)start_pos ) + 1;
	}
	
	// If untranslated regions may interfere, use 'extract_DNA_seq()' to simplify their removal
	else
	{
		prev_pos_to_search = pos_to_search + ( NUM_NT_TO_INCREMENT * ( opposite_strand == FALSE ? 1 : -1 ) );
		
		
		do
		{
			if( opposite_strand == FALSE )  
			{
				pos_to_search += NUM_NT_TO_INCREMENT; 
				
				// If the current search position and the previous one straddle an intron start coordinate, add the intron
				//   length to the start position
				for( unsigned int i = 0; i < removed_reg.size(); i++ )
				{
					if( pos_to_search >= removed_reg[i].start_coord && prev_pos_to_search < removed_reg[i].start_coord )
					{
						seq_length    -= removed_reg[i].length;
						pos_to_search += removed_reg[i].length;
					}
				}
					
				
				removed_reg_it = removed_reg.size();
				unsigned int curr_it = 0;
				
				// If the searched codon is split by an intron, record the intron to pass to 'extract_DNA_seq()'
				do
				{				
					if( ( pos_to_search + NUM_NT_TO_COMPLETE_CODON ) >= removed_reg[curr_it].start_coord && pos_to_search < removed_reg[curr_it].start_coord )
					{
						removed_reg_it = curr_it;
					}
					else
					{
						removed_reg_it = removed_reg.size();
					}
					
					curr_it++;
					
				} while( curr_it < removed_reg.size() && removed_reg_it == removed_reg.size() );
			}
			else/*( opposite_strand == TRUE )*/
			{ 
				pos_to_search -= NUM_NT_TO_INCREMENT; 
				for( unsigned int i = 0; i < removed_reg.size(); i++ )
				{
					if( pos_to_search <= removed_reg[i].start_coord && prev_pos_to_search > removed_reg[i].start_coord )
					{
						seq_length    -= removed_reg[i].length;
						pos_to_search -= removed_reg[i].length;
					}
				}
				
				removed_reg_it = removed_reg.size();
				unsigned int curr_it = 0;
				
				do
				{				
					if( ( pos_to_search - NUM_NT_TO_COMPLETE_CODON ) <= removed_reg[curr_it].start_coord && pos_to_search > removed_reg[curr_it].start_coord )
					{
						removed_reg_it = curr_it;
					}
					else
					{
						removed_reg_it = removed_reg.size();
					}
					
					curr_it++;
					
				} while( curr_it < removed_reg.size() && removed_reg_it == removed_reg.size() );
			}	
			
			// To save time, pass only the interfering inton to 'extract_DNA_seq()'
			vector <TFeature> interfering_reg( ( removed_reg_it == removed_reg.size() ? 0 : 1 ), 
											   ( removed_reg_it == removed_reg.size() ? TFeature() : removed_reg[ removed_reg_it ] ) );
											   
			string searched_codon = extract_DNA_seq( chomosome, pos_to_search, 3, opposite_strand, interfering_reg );

			
			// Check if the searched codon is a stopped codon
			if( ( searched_codon == "TAA" ) ||
				( searched_codon == "TAG" ) ||
				( searched_codon == "TGA" ) )
			{
				end_pos = pos_to_search + ( NUM_NT_TO_COMPLETE_CODON * ( opposite_strand == FALSE ? 1 : -1 ) );
				
				if( removed_reg_it != removed_reg.size() ) { seq_length -= removed_reg[removed_reg_it].length; }
			}
			
			prev_pos_to_search = pos_to_search;

		} while( pos_to_search - 1 < chomosome.size() && end_pos == string::npos );
		
		seq_length += ( abs( (int)end_pos - (int)start_pos ) + 1 );
	}
	
	
	if( end_pos == string::npos )
	{
		Errors.handle_error( FATAL, (string)"Error in 'find_first_stop_codon()': No stop codon was found. ", __FILE__, __LINE__ );
	}
	
	if( len != NULL ) { *len = seq_length; }
	
	return end_pos;
}
//==============================================================================



void generate_simple_GO_hierarchy( void )
{
	string GO_database_contents;
	
	if( read_entire_file_contents( PATH_GO_DATABASE_FOLDER, GO_DATABASE_OBO_FILE_NAME, &GO_database_contents ) )
	{
		Errors.handle_error( FATAL, "Error reading GO database file contents. ", __FILE__, __LINE__ );
	}
	
	vector <TGO_Annotation> All_GO_Annotations( NUM_GO_NAMESPACES, TGO_Annotation( "" ) );
		
	All_GO_Annotations[GO_BIO_PROC_IT]  = TGO_Annotation( GO_BIO_PROC_ID  );
	All_GO_Annotations[GO_CELL_COMP_IT] = TGO_Annotation( GO_CELL_COMP_ID );
	All_GO_Annotations[GO_MOL_FUNC_IT]  = TGO_Annotation( GO_MOL_FUNC_ID  );
	
	vector <vector < vector <string> > > GO_Annotations_to_csv( NUM_GO_NAMESPACES, vector < vector <string> > ( 1, vector <string>( 3, "" ) ) );
	
	vector <string> GO_hierarchy_file_names( NUM_GO_NAMESPACES, "" );
	
	GO_hierarchy_file_names[GO_BIO_PROC_IT]  = GO_HIERARCHY_BP_FILE_NAME;
	GO_hierarchy_file_names[GO_CELL_COMP_IT] = GO_HIERARCHY_CC_FILE_NAME;
	GO_hierarchy_file_names[GO_MOL_FUNC_IT]  = GO_HIERARCHY_MF_FILE_NAME;
	
	
	for( unsigned int i = 0; i < NUM_GO_NAMESPACES; i++ )
	{
		string current_parent_term = All_GO_Annotations[i].GO_namespace;
		ostringstream output_oss;
		output_oss << "Namespace: " << current_parent_term;
		output_text_line( output_oss.str() );
		
		unsigned int curr_level = 1;
		int j = -1;
		bool end_of_hierarchy_reached = FALSE;
		
		do
		{
			do
			{
				j++;
				unsigned int prev_parent_id_pos = 0;
				
				do
				{
					unsigned int parent_id_pos = find_GO_parent_term( GO_database_contents, current_parent_term, prev_parent_id_pos );
					
					if( parent_id_pos != string::npos )
					{
						unsigned int prev_size = All_GO_Annotations[i].GO_Terms.size();
						All_GO_Annotations[i].add_term( get_child_term( GO_database_contents, parent_id_pos ), "", curr_level, current_parent_term );

						output_oss.str( "" );
						if( prev_size != All_GO_Annotations[i].GO_Terms.size() )
						{
							output_oss << "  Level: " << curr_level << "   Term: " << All_GO_Annotations[i].GO_Terms.back().name << "     Parent: " << current_parent_term;
						}
						else
						{
							output_oss << "     repeat term " << get_child_term( GO_database_contents, parent_id_pos ) << " not added";
						}
						
						output_text_line( output_oss.str() );
					}
					
					prev_parent_id_pos = parent_id_pos;
					
				} while( prev_parent_id_pos != string::npos );
				
			
				if( All_GO_Annotations[i].GO_Terms.size() == 0 ) 
				{ 
					Errors.handle_error( FATAL, "Error in 'generate_simple_GO_hierarchy()': No terms found under namespace. ", __FILE__, __LINE__ ); 
				}
				
				if( (unsigned int)j == All_GO_Annotations[i].GO_Terms.size() ) { end_of_hierarchy_reached = TRUE; }
				else
				{
					current_parent_term = All_GO_Annotations[i].GO_Terms[j].name;
				}
				
			} while( end_of_hierarchy_reached == FALSE && All_GO_Annotations[i].GO_Terms[j].level == ( curr_level - 1 ) );
			
			curr_level++;
			
		} while( end_of_hierarchy_reached == FALSE );
		
		
		GO_Annotations_to_csv[i][0][0] = GO_HIERARCHY_NAME_HEADER;
		GO_Annotations_to_csv[i][0][1] = GO_HIERARCHY_LEVEL_HEADER;
		GO_Annotations_to_csv[i][0][2] = GO_HIERARCHY_PARENT_HEADER;
		
		for( unsigned int j = 0; j < All_GO_Annotations[i].GO_Terms.size(); j++ )
		{
			GO_Annotations_to_csv[i].push_back( vector <string>( 3, "" ) );
			
			ostringstream GO_level_oss;
			GO_level_oss << All_GO_Annotations[i].GO_Terms[j].level;

			GO_Annotations_to_csv[i][j + 1][0] = All_GO_Annotations[i].GO_Terms[j].name;
			GO_Annotations_to_csv[i][j + 1][1] = GO_level_oss.str();
			GO_Annotations_to_csv[i][j + 1][2] = All_GO_Annotations[i].GO_Terms[j].parent;
		}
		

		if( write_2d_vector_to_csv( PATH_DATA_FOLDER, GO_hierarchy_file_names[i], GO_Annotations_to_csv[i] ) )
		{
			Errors.handle_error( FATAL, "Error in 'generate_simple_GO_hierarchy()': Failed to write GO hierarchy to CSV file. ", __FILE__, __LINE__ );
		}
	}
	
	return;
}
//==============================================================================



void parse_GO_hierarchy( vector <TGO_Annotation> * const All_GO_Annotations )
{
	All_GO_Annotations->resize( NUM_GO_NAMESPACES, TGO_Annotation( "" ) );
	
	All_GO_Annotations->at( GO_BIO_PROC_IT ) = TGO_Annotation( GO_BIO_PROC_ID  );
	All_GO_Annotations->at( GO_CELL_COMP_IT) = TGO_Annotation( GO_CELL_COMP_ID );
	All_GO_Annotations->at( GO_MOL_FUNC_IT ) = TGO_Annotation( GO_MOL_FUNC_ID  );
	
	
	vector <string> GO_hierarchy_file_names( NUM_GO_NAMESPACES, "" );
	
	GO_hierarchy_file_names[GO_BIO_PROC_IT]  = GO_HIERARCHY_BP_FILE_NAME;
	GO_hierarchy_file_names[GO_CELL_COMP_IT] = GO_HIERARCHY_CC_FILE_NAME;
	GO_hierarchy_file_names[GO_MOL_FUNC_IT]  = GO_HIERARCHY_MF_FILE_NAME;
	
	
	for( unsigned int i = 0; i < NUM_GO_NAMESPACES; i++ )
	{
		TCSV_Contents * GO_Hierarchy_Table = new TCSV_Contents( PATH_DATA_FOLDER, GO_hierarchy_file_names[i] );
		GO_Hierarchy_Table->parse_csv();
		
		vector <string> 	  GO_term_names   = GO_Hierarchy_Table->get_csv_column<string>		( GO_HIERARCHY_NAME_HEADER   );
		vector <string> 	  GO_parent_names = GO_Hierarchy_Table->get_csv_column<string>	  	( GO_HIERARCHY_PARENT_HEADER );
		vector <unsigned int> GO_term_levels  = GO_Hierarchy_Table->get_csv_column<unsigned int>( GO_HIERARCHY_LEVEL_HEADER  );
		
		for( unsigned int j = 0; j < GO_term_names.size(); j++ )
		{
			All_GO_Annotations->at( i ).add_term( GO_term_names[j], "", GO_term_levels[j], GO_parent_names[j] );
		}
	}
	
	return;
}
//==============================================================================



void generate_list_of_children( void )
{
	string GO_database_contents;
	
	if( read_entire_file_contents( PATH_GO_DATABASE_FOLDER, GO_DATABASE_OBO_FILE_NAME, &GO_database_contents ) )
	{
		Errors.handle_error( FATAL, "Error reading GO database file contents. ", __FILE__, __LINE__ );
	}
	
	vector <TCSV_Contents> GO_Hierarchy;
	
	GO_Hierarchy.push_back( TCSV_Contents( PATH_DATA_FOLDER, GO_HIERARCHY_BP_FILE_NAME ) );
	GO_Hierarchy.push_back( TCSV_Contents( PATH_DATA_FOLDER, GO_HIERARCHY_CC_FILE_NAME ) );
	GO_Hierarchy.push_back( TCSV_Contents( PATH_DATA_FOLDER, GO_HIERARCHY_MF_FILE_NAME ) );
	
	
	vector <vector <vector <string> > > children_list_to_csv( NUM_GO_NAMESPACES, vector <vector <string> >( 1, vector <string>( NUM_CSV_GO_CHILD_COLUMNS, "" ) ) );

	vector <string> GO_children_file_names( NUM_GO_NAMESPACES, "" );
	
	GO_children_file_names[GO_BIO_PROC_IT]  = GO_CHILDREN_LIST_BP_FILE_NAME;
	GO_children_file_names[GO_CELL_COMP_IT] = GO_CHILDREN_LIST_CC_FILE_NAME;
	GO_children_file_names[GO_MOL_FUNC_IT]  = GO_CHILDREN_LIST_MF_FILE_NAME;
	
	
	for( unsigned int i = 0; i < NUM_GO_NAMESPACES; i++ )
	{
		children_list_to_csv[i][0][GO_CHILD_CSV_COL_NUM_PARENT]   = GO_CHILDREN_LIST_PARENT_HEADER;
		children_list_to_csv[i][0][GO_CHILD_CSV_COL_NUM_CHILDREN] = GO_CHILDREN_LIST_CHILDREN_HEADER;
	}	
		
		
	const string TERM_ID 	  = "\nname: ";
	const string NAMESPACE_ID = "\nnamespace: ";

	unsigned int term_start_pos = GO_database_contents.find( TERM_ID ) + TERM_ID.size();
	unsigned int term_num = 0;
	
	
	double init_time = get_system_time_ms();
	
	vector <vector <string> > terms_already_found( NUM_GO_NAMESPACES, vector <string>( 0 ) );
	
	
	for( unsigned int namespace_it = 0; namespace_it < NUM_GO_NAMESPACES; namespace_it++ )
	{
		GO_Hierarchy[namespace_it].parse_csv();
		
		vector <string> term_list = GO_Hierarchy[namespace_it].get_csv_column<string>( GO_HIERARCHY_NAME_HEADER );

		for( int term_it = term_list.size() - 1; term_it >= 0; term_it-- )
		{
			output_text_line( (string)"Term " + get_str( term_num ) + ": " + term_list[term_it] );
			
			vector <string> term_children = get_all_children_terms( GO_database_contents, term_list[term_it], terms_already_found[namespace_it], children_list_to_csv[namespace_it] );
			
			children_list_to_csv[namespace_it].push_back( vector <string>( NUM_CSV_GO_CHILD_COLUMNS, "" ) );
			children_list_to_csv[namespace_it].back()[GO_CHILD_CSV_COL_NUM_PARENT]   = term_list[term_it]; 
			children_list_to_csv[namespace_it].back()[GO_CHILD_CSV_COL_NUM_CHILDREN] = generate_delimited_list( term_children, GO_TERM_LIST_DELIMITER ); 	

			terms_already_found[namespace_it].push_back( term_list[term_it] );
			
			term_num++;
		}
	}
	
	/*
	while( ( term_start_pos - TERM_ID.size() ) != string::npos )
	{
		unsigned int term_end_pos = GO_database_contents.find( '\n', term_start_pos ) - 1;
		
		unsigned int namespace_start_pos = GO_database_contents.find( NAMESPACE_ID, term_end_pos ) + NAMESPACE_ID.size();
		unsigned int namespace_end_pos   = GO_database_contents.find( '\n', namespace_start_pos ) -1;

		if( term_end_pos + 1 == string::npos )
		{
			Errors.handle_error( FATAL, "Error in 'generate_list_of_children()': Failed to find the end of the term. ", __FILE__, __LINE__ );
		}
		
		if( namespace_end_pos + 1 == string::npos )
		{
			Errors.handle_error( FATAL, "Error in 'generate_list_of_children()': Failed to find the end of the namespace. ", __FILE__, __LINE__ );
		}
		
		
		string term = GO_database_contents.substr( term_start_pos, ( term_end_pos - term_start_pos + 1 ) );
		
		output_text_line( (string)"Term " + get_str( term_num ) + ": " + term );
		
		string GO_namespace = GO_database_contents.substr( namespace_start_pos, ( namespace_end_pos - namespace_start_pos + 1 ) );
		
		
		if( term != GO_namespace )
		{
			vector <string> term_children = get_all_children_terms( GO_database_contents, term, terms_already_found[GO_namespace_it], children_list_to_csv[GO_namespace_it] );
			
			
			unsigned int GO_namespace_it = 0;
			
			if	   ( GO_namespace == GO_BIO_PROC_ID  ) { GO_namespace_it = GO_BIO_PROC_IT;  }
			else if( GO_namespace == GO_CELL_COMP_ID ) { GO_namespace_it = GO_CELL_COMP_IT; }
			else if( GO_namespace == GO_MOL_FUNC_ID  ) { GO_namespace_it = GO_MOL_FUNC_IT;  }
			else
			{
				Errors.handle_error( FATAL, (string)"Error in 'generate_list_of_children()': GO namespace found (" + GO_namespace + ") is not recognized. ", __FILE__, __LINE__ );
			}
			
			
			children_list_to_csv[GO_namespace_it].push_back( vector <string>( NUM_CSV_GO_CHILD_COLUMNS, "" ) );
			children_list_to_csv[GO_namespace_it].back()[GO_CHILD_CSV_COL_NUM_PARENT]   = term; 
			children_list_to_csv[GO_namespace_it].back()[GO_CHILD_CSV_COL_NUM_CHILDREN] = generate_delimited_list( term_children, DEFAULT_DELIMITER ); 	

			terms_already_found[GO_namespace_it].push_back( term );
		}
		
		term_start_pos = GO_database_contents.find( TERM_ID, term_start_pos ) + TERM_ID.size();
		term_num++;
	}*/
	
	output_text_line( (string)"Total Time:    " + get_str( get_system_time_ms() - init_time ) );
	output_text_line( (string)"Time Per Term: " + get_str( ( get_system_time_ms() - init_time ) / term_num ) );
	
	
	for( unsigned int i = 0; i < NUM_GO_NAMESPACES; i++ )
	{
		if( write_2d_vector_to_csv( PATH_DATA_FOLDER, GO_children_file_names[i], children_list_to_csv[i] ) )
		{
			Errors.handle_error( FATAL, "Error in 'generate_list_of_children()': Failed to write GO children to CSV file. ", __FILE__, __LINE__ );
		}
	}
		
		
	return;
}
//==============================================================================



unsigned int find_GO_parent_term( const string & GO_database_contents, const string & parent_term, const unsigned int prev_parent_id_pos )
{
	unsigned int parent_id_pos;
	unsigned int search_start_pos = prev_parent_id_pos + 1;
	
	do
	{
		parent_id_pos = GO_database_contents.find( ( (string)"! " + parent_term + "\n" ), search_start_pos );
		search_start_pos = parent_id_pos + 1;
	
	} while( parent_id_pos != string::npos && GO_database_contents.rfind( "is_a", parent_id_pos ) < GO_database_contents.rfind( "\n", parent_id_pos ) );
	
	return parent_id_pos;
}
//==============================================================================



unsigned int find_GO_parent_term( const string & GO_database_contents, const int parent_term_num, const unsigned int prev_parent_id_pos )
{
	string search_str = get_str( parent_term_num );
	
	while( search_str.size() < 7 ) { search_str.insert( search_str.begin(), '0' ); }
	
	search_str = "\nis_a: GO:" + search_str;
	
	return GO_database_contents.find( search_str, prev_parent_id_pos + 1 );
}
//==============================================================================



vector <string> get_all_children_terms( const string & GO_database_contents, 
										const string & parent_term, 
										const vector <string> & terms_already_found, 
										const vector <vector <string> > & terms_and_children )
{
	vector <string> children( 0 );
	vector <string> terms_searched_for( 0 );
	unsigned int children_it = 0;
	string current_term = parent_term;

	do
	{
		if( children_it != 0 )
		{
			output_text_line( "   Searching for children of child " + get_str( children_it ) + ": " + current_term );
		}
		
		

		/*if( find( terms_searched_for.begin(), terms_searched_for.end(), current_term ) != terms_searched_for.end() )
		{
			Errors.handle_error( FATAL, (string)"Error in 'get_all_children_terms()': Repeat child term " + current_term + " searched for. ", __FILE__, __LINE__ );
		}*/


		unsigned int prev_parent_id_pos = 0;
		vector <string> children_this_term( 0 );
		
		unsigned int curr_term_it = (unsigned int)( find( terms_already_found.begin(), terms_already_found.end(), current_term ) - terms_already_found.begin() ) + 1;
		
		if( curr_term_it == terms_already_found.size() + 1 )
		{
			do
			{
				unsigned int parent_id_pos = find_GO_parent_term( GO_database_contents, current_term, prev_parent_id_pos );
				
				if( parent_id_pos != string::npos )
				{
					children_this_term.push_back( get_child_term( GO_database_contents, parent_id_pos ) );
				}
				
				prev_parent_id_pos = parent_id_pos;
				
			} while( prev_parent_id_pos != string::npos );
		}
		else
		{
			output_text_line( "   Already added children of child " + get_str( children_it ) + ": " + current_term + "(" +  terms_and_children[curr_term_it][GO_CHILD_CSV_COL_NUM_CHILDREN] + ")" );
			children_this_term = parse_delimited_list<string>( terms_and_children[curr_term_it][GO_CHILD_CSV_COL_NUM_CHILDREN], GO_TERM_LIST_DELIMITER );
		}
		

		for( unsigned int i = 0; i < children_this_term.size(); i++ )
		{
			if( find( children.begin(), children.end(), children_this_term[i] ) == children.end() )
			{
				children.push_back( children_this_term[i] );
			}
		}
		
		if( children_it < children.size() )
		{
			current_term = children[ children_it ];
		}
		
		children_it++;
		
	} while( children_it <= children.size() );
	
	
	sort( children.begin(), children.end(), compare_strings );
	
	/*
	for( unsigned int i = 1; i < children.size(); i++ )
	{
		if( children[i] == children[i - 1] )
		{
			children.erase( children.begin() + i );
			i--;
		}
	}*/
	
	/*for( unsigned int i = 1; i < children.size(); i++ )
	{
		cout << "    child term " << i << ": " << children[i] << endl;
	}*/
	
	
	return children;
}
//==============================================================================



vector <string> get_all_children_terms( const string & GO_database_contents, const unsigned int parent_term_num )
{
	vector <string> children( 0 );
	unsigned int prev_parent_id_pos = 0;
	
	do
	{
		unsigned int parent_id_pos = find_GO_parent_term( GO_database_contents, parent_term_num, prev_parent_id_pos );
		
		if( parent_id_pos != string::npos )
		{
			children.push_back( get_child_term( GO_database_contents, parent_id_pos ) );
		}
		
		prev_parent_id_pos = parent_id_pos;
		
	} while( prev_parent_id_pos != string::npos );
	
	
	return children;
}
//==============================================================================



string get_child_term( const string & GO_database_contents, const unsigned int parent_id_pos )
{
	const string GO_NAME_ID = "name: ";
	
	unsigned int term_id_pos = GO_database_contents.rfind( GO_NAME_ID, parent_id_pos );
	
	if( term_id_pos == string::npos) { Errors.handle_error( FATAL, "Error in 'get_child_term()': Child name identifier not found. ", __FILE__, __LINE__ ); }
	
	unsigned int term_start_pos = term_id_pos + GO_NAME_ID.size();
	
	return GO_database_contents.substr( term_start_pos, ( GO_database_contents.find( "\n", term_start_pos ) - term_start_pos ) );
}
//==============================================================================



bool is_child_descendent_of( const string & child_term, const string & parent_term, const TGO_Annotation & GO_Annotations )
{
	vector <TGO_Term> all_parent_terms = get_all_parents_of( child_term, GO_Annotations );
	
	unsigned int parent_term_it = 0;
		
	// Find the GO Annotation term
	while( parent_term_it < all_parent_terms.size() && all_parent_terms[parent_term_it].name != parent_term )
	{
		parent_term_it++;
	}
	
	return ( parent_term_it < all_parent_terms.size() ? TRUE : FALSE );
}
//==============================================================================



vector <TGO_Term> get_all_parents_of( const string & child_term, const TGO_Annotation & GO_Annotations )
{
	unsigned int child_term_it = 0;
		
	// Find the GO Annotation term
	while( child_term_it < GO_Annotations.GO_Terms.size() && GO_Annotations.GO_Terms[child_term_it].name != child_term )
	{
		child_term_it++;
	}
	
	if( child_term_it >= GO_Annotations.GO_Terms.size() )
	{
		Errors.handle_error( FATAL, (string)"Error in 'get_all_parents_of()': The child GO Annotation term that was attempted to be accessed (" + 
											child_term + ") does not exist. ", __FILE__, __LINE__ );
	}
	
	
	vector <TGO_Term> parent_terms( 0, TGO_Term( "", "" ) );
	unsigned int curr_level = GO_Annotations.GO_Terms[child_term_it].level - 1;
	string prev_parent_term = child_term;
	unsigned int parent_term_it = child_term_it;
	
	while( curr_level >= 1 )
	{
		string parent_term_name = GO_Annotations.GO_Terms[parent_term_it].parent;
		
		while( parent_term_it < GO_Annotations.GO_Terms.size() && GO_Annotations.GO_Terms[parent_term_it].name != parent_term_name )
		{
			parent_term_it--;
		}
	
		if( parent_term_it >= GO_Annotations.GO_Terms.size() )
		{
			Errors.handle_error( FATAL, (string)"Error in 'get_all_parents_of()': The parent GO Annotation term that was attempted to be accessed (" + 
												parent_term_name + ") does not exist. ", __FILE__, __LINE__ );
		}
		
		if( GO_Annotations.GO_Terms[parent_term_it].level != curr_level )
		{
			ostringstream error_oss;
			error_oss << "Error in 'get_all_parents_of()': The GO Annotation parent term level (" << GO_Annotations.GO_Terms[parent_term_it].level
					  << ") did not match what was expected (" 									  << curr_level 
					  << ") for parent term " 													  << GO_Annotations.GO_Terms[parent_term_it].name << ". ";
			
			Errors.handle_error( FATAL, error_oss.str(), __FILE__, __LINE__ );
		}
		
		
		parent_terms.push_back( GO_Annotations.GO_Terms[parent_term_it] );
		prev_parent_term = parent_term_name;
		
		curr_level--;
	}
	
	
	// One term should have been found for each level below the level of the child term
	if( parent_terms.size() != GO_Annotations.GO_Terms[child_term_it].level - 1 )
	{
		Errors.handle_error( FATAL, (string)"Error in 'get_all_parents_of()': An insufficient number of parents was found for child GO Annotation term " + 
											child_term + ". ", __FILE__, __LINE__ );
	}
	
	
	return parent_terms;
} 
//==============================================================================



std::vector <unsigned int> convert_DNA_string_to_num( const std::string & DNA_string  )
{
	vector <unsigned int> DNA_numeric( DNA_string.size(), NT_A );
			
	for(unsigned int i = 0; i < DNA_string.size(); i++ )
	{
		switch( DNA_string[i] )
		{
			case 'A' :						 	break;
			case 'C' : DNA_numeric[i] = NT_C; 	break;
			case 'G' : DNA_numeric[i] = NT_G; 	break;
			case 'T' : DNA_numeric[i] = NT_T; 	break;
			
			default: Errors.handle_error( FATAL, "Error in 'convert_DNA_string_to_num()': Passed DNA string contains unrecognized characters. ", __FILE__, __LINE__ );		
		}
	}
		

	return DNA_numeric;
}
//==============================================================================



std::string convert_DNA_num_to_string( const std::vector <unsigned int> & DNA_numeric )
{
	string DNA_string( DNA_numeric.size(), 'A' );
			
	for(unsigned int i = 0; i < DNA_numeric.size(); i++ )
	{
		switch( DNA_numeric[i] )
		{
			case NT_A :						 	break; 
			case NT_C : DNA_string[i] = 'C'; 	break; 
			case NT_G : DNA_string[i] = 'G'; 	break; 
			case NT_T : DNA_string[i] = 'T'; 	break; 
			
			default: Errors.handle_error( FATAL, "Error in 'convert_DNA_num_to_string()': Passed DNA string contains unrecognized characters. ", __FILE__, __LINE__ );		
		}
	}
		

	return DNA_string;
}
//==============================================================================



void get_coding_and_noncoding_DNA( const TFasta_Content & S_Cerevisiae_Chrom,  vector <string> * const coding_DNA, vector <string> * const noncoding_DNA )
{
	vector <string> gff_annotations = S_Cerevisiae_Chrom.get_gff_annotations();
	
	*coding_DNA    = vector <string>( gff_annotations.size(), "" );
	*noncoding_DNA = vector <string>( gff_annotations.size(), "" );
	
	
	
	for( unsigned int i = 0; i < NUM_S_CEREVISIAE_CHROM; i++ )
	{
		// ------------------------------
		// Extract the needed information (CDS start and end, and strand)
		vector <string> CDS_first_coord_str = extract_fields( gff_annotations[i], "SGD\tCDS\t", "\t" 		);
		vector <string> CDS_last_coord_str  = extract_fields( gff_annotations[i], "SGD\tCDS\t", "\t." 		);
		vector <string> strand_str     		= extract_fields( gff_annotations[i], "SGD\tCDS\t", "\tParent=" );
		
		
		if( CDS_first_coord_str.size() != strand_str.size() || CDS_last_coord_str.size() != strand_str.size() )
		{
			Errors.handle_error( FATAL, (string)"Error in 'get_coding_and_noncoding_DNA()': Size of data vectors obtained from SGD GFF don't match. ", __FILE__, __LINE__ );
		}

		
		vector <unsigned int> CDS_start_coord_sense	   ( 0 );
		vector <unsigned int> CDS_start_coord_antisense( 0 );
		vector < 		 int> CDS_length_sense         ( 0 );
		vector < 		 int> CDS_length_antisense     ( 0 );
		// ------------------------------
		
		
		
		// ------------------------------
		// Extract all Coding DNA Segment annotations from the GFF file (don't add CDS's fully enveloped by another)
		for( unsigned int j = 0; j < CDS_first_coord_str.size(); j++ )
		{
			unsigned int strand_id_pos = strand_str[j].find( ".\t" ) + ((string)".\t").size() ;

			unsigned int CDS_first_coord = atoi( CDS_first_coord_str[j].c_str() );
			
			unsigned int last_coord_start_pos = CDS_last_coord_str[j].find( '\t' ) + 1; 
			unsigned int CDS_last_coord = atoi( CDS_last_coord_str[j].substr( last_coord_start_pos ).c_str() );
			
			if( strand_str[j][strand_id_pos] == '+' )
			{	
				// Only add the CDS if not enveloped by the next CDS
				if( CDS_start_coord_sense.size() == 0 || ( CDS_start_coord_sense.back() + CDS_length_sense.back() - 1 ) < CDS_last_coord )
				{
					CDS_start_coord_sense.push_back( CDS_first_coord ); 
					CDS_length_sense.push_back( CDS_last_coord - CDS_first_coord + 1 ); 
					
					// If the newly-added CDS envelops the previous CDS, delete the previous CDS
					if( ( CDS_start_coord_sense.size() != 0 ) && 
						( CDS_start_coord_sense.back() <= CDS_start_coord_sense[ CDS_start_coord_sense.size() - 2 ] ) &&
						( CDS_start_coord_sense.back() + CDS_length_sense.back() >= CDS_start_coord_sense[ CDS_start_coord_sense.size() - 2 ] + CDS_length_sense[ CDS_start_coord_sense.size() - 2 ] ) )
					{
						CDS_start_coord_sense.erase( CDS_start_coord_sense.begin() + CDS_start_coord_sense.size() - 2 );
						CDS_length_sense.erase     ( CDS_length_sense.begin()      + CDS_length_sense.size() - 2 );
					}
					
				}
			}
			else/*( strand_str[strand_id_pos] == '-' )*/ 
			{ 	
				// Only add the CDS if not enveloped by the next CDS
				if( CDS_start_coord_antisense.size() == 0 || CDS_start_coord_antisense.back() < CDS_last_coord )
				{
					CDS_start_coord_antisense.push_back( CDS_last_coord );  
					CDS_length_antisense.push_back( CDS_last_coord - CDS_first_coord + 1 ); 
					
					// If the newly-added CDS envelops the previous CDS, delete the previous CDS
					if( ( CDS_start_coord_antisense.size() != 0 ) && 
						( CDS_start_coord_antisense.back() >= CDS_start_coord_antisense[ CDS_start_coord_antisense.size() - 2 ] ) &&
						( CDS_start_coord_antisense.back() - CDS_length_antisense.back() <= CDS_start_coord_antisense[ CDS_start_coord_antisense.size() - 2 ] - CDS_length_antisense[ CDS_start_coord_antisense.size() - 2 ] ) )
					{
						CDS_start_coord_antisense.erase( CDS_start_coord_antisense.begin() + CDS_start_coord_antisense.size() - 2 );
						CDS_length_antisense.erase     ( CDS_length_antisense.begin()      + CDS_length_antisense.size() - 2 );
					}
				}
			}
		}
		// ------------------------------
		
		
		
		// ------------------------------
		// Add all the coding and noncoding segments on the sense strand of the current chromosome
		noncoding_DNA->at( i ).append( extract_DNA_seq( S_Cerevisiae_Chrom.sequence[i], 1, CDS_start_coord_sense[0] - 1,  FALSE ) );
		
		for( unsigned int j = 0; j < CDS_start_coord_sense.size(); j++ )
		{
			int intergenic_reg_len = (int)S_Cerevisiae_Chrom.sequence[i].size() - ( (int)CDS_start_coord_sense[j] + (int)CDS_length_sense[j] ) + 1;
			if( j < CDS_start_coord_sense.size() - 1 ) { intergenic_reg_len = (int)CDS_start_coord_sense[j + 1] - ( (int)CDS_start_coord_sense[j] + (int)CDS_length_sense[j] ); }
			
			if( intergenic_reg_len < 0 )
			{
				if( ( CDS_start_coord_sense[j] + (int)CDS_length_sense[j] ) > ( CDS_start_coord_sense[j + 1] + (int)CDS_length_sense[j + 1] ) ) 
				{  
					Errors.handle_error( FATAL, (string)"Envoloped + CDS. " + get_str(i) + ":" + get_str( CDS_start_coord_sense[j + 1] ), __FILE__, __LINE__ ); 
				}
				
				CDS_length_sense[j] += intergenic_reg_len;
				intergenic_reg_len = 0;
			}

			coding_DNA->at( i ).append   ( extract_DNA_seq( S_Cerevisiae_Chrom.sequence[i], CDS_start_coord_sense[j], 					    CDS_length_sense[j], FALSE ) );
			noncoding_DNA->at( i ).append( extract_DNA_seq( S_Cerevisiae_Chrom.sequence[i], CDS_start_coord_sense[j] + CDS_length_sense[j], intergenic_reg_len,  FALSE ) );
		}
		// ------------------------------
		
		
		
		// ------------------------------
		// Add all the coding and noncoding segments on the antisense strand of the current chromosome
		noncoding_DNA->at( i ).append( extract_DNA_seq( S_Cerevisiae_Chrom.sequence[i], CDS_start_coord_antisense[0] - CDS_length_antisense[0], CDS_start_coord_antisense[0] - CDS_length_antisense[0], TRUE ) );
		
		for( unsigned int j = 0; j < CDS_start_coord_antisense.size(); j++ )
		{
			if( j < CDS_start_coord_antisense.size() - 1 ) 
			{ 
				int intergenic_reg_len = (int)CDS_start_coord_antisense[j + 1] - (int)CDS_length_antisense[j + 1]  - (int)CDS_start_coord_antisense[j];
				if( intergenic_reg_len < 0 )
				{
					if( CDS_start_coord_antisense[j] > CDS_start_coord_antisense[j + 1] ) 
					{  
						Errors.handle_error( FATAL, (string)"Envoloped - CDS. " + get_str(i) + ":" + get_str( CDS_start_coord_sense[j + 1] ), __FILE__, __LINE__ ); 
					}
					
					CDS_start_coord_antisense[j] += intergenic_reg_len;
					CDS_length_antisense[j]      += intergenic_reg_len;
					intergenic_reg_len = 0;
				}
			
				noncoding_DNA->at( i ).append( extract_DNA_seq( S_Cerevisiae_Chrom.sequence[i], CDS_start_coord_antisense[j + 1] - CDS_length_antisense[j + 1], intergenic_reg_len, TRUE ) );
			}
			else
			{
				int intergenic_reg_len = (int)S_Cerevisiae_Chrom.sequence[i].size() - (int)CDS_start_coord_antisense[j];
				noncoding_DNA->at( i ).append( extract_DNA_seq( S_Cerevisiae_Chrom.sequence[i], S_Cerevisiae_Chrom.sequence[i].size(), intergenic_reg_len, TRUE ) );
			}
			
			
			coding_DNA->at( i ).append( extract_DNA_seq( S_Cerevisiae_Chrom.sequence[i], CDS_start_coord_antisense[j], CDS_length_antisense[j], TRUE ) );
		}
		// ------------------------------
	}
	
	
	return;
}
//==============================================================================

////////////////////////////////////////////////////////////////////////////////


