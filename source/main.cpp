//============================================================================
// Project	   : uORF
// Name        : main.cpp
// Author      : Garin Newcomb
// Email       : gpnewcomb@live.com   
// Version     : See "Revision History" below
// Copyright   : Copyright 2014 University of Nebraska-Lincoln
// Description : uORF project 'Main' file, including 'main()'
//============================================================================
//
//  Revision History
//      v0.0.0 - 2014/02/06 - Garin Newcomb
//          Initial creation of file
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
//		C. Main
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
#include <string>
#include <vector>
#include <algorithm>   	// For sort()

using namespace std;

// Project-specific header files:  definitions and related information
#include "defs__general.h"
#include "defs__appl_parameters.h"
#include "version.h"

// Project-specific header files:  support functions and related
#include "support__file_io.h"
#include "uORF__compile.h"
#include "uORF__process.h"
#include "support__general.h"
#include "support__bioinformatics.h"

////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//
// B. Global Variable Declarations (including those in other files)
//
////////////////////////////////////////////////////////////////////////////////

extern TErrors Errors;						// Stores information about errors and responds to them in several ways
TProgram_Log Program_Log;

////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//
// C. Main
//
////////////////////////////////////////////////////////////////////////////////

//==============================================================================

int main()
{	
	////////////////////////////////////////////////////////////////////////////////
	// Alternative Program Flows - Before Reading in Yeast Genome
	
	// ------------------------------
	// Generate files containing a simplified hierarchy of GO terms
	#ifdef GENERATE_GO_HIERARCHY
		generate_simple_GO_hierarchy();
		return Program_Log.print_log_to_file( PATH_PROGRAM_LOG_FOLDER, uORF_APPL_VERSION_NUMBER, SUCCESSFUL );
	#endif
	// ------------------------------
	
	
	
	// ------------------------------
	// Generate files containing a list of children for each GO term
	#ifdef GENERATE_GO_CHILDREN_LIST
		generate_list_of_children();
		return Program_Log.print_log_to_file( PATH_PROGRAM_LOG_FOLDER, uORF_APPL_VERSION_NUMBER, SUCCESSFUL );
	#endif
	// ------------------------------
	
	
	
	// ------------------------------
	// Generate files containing a simplified hierarchy of GO terms
	#ifdef WRITE_MIURA_SGD_ANNOT_INFO
		write_Miura_SGD_annot_info_to_txt();
		return Program_Log.print_log_to_file( PATH_PROGRAM_LOG_FOLDER, uORF_APPL_VERSION_NUMBER, SUCCESSFUL );
	#endif
	// ------------------------------



	// ------------------------------
	// Generate files containing a simplified hierarchy of GO terms
	#ifdef WRITE_MIURA_TSS_DATA
		write_Miura_TSS_data_to_csv();
		return Program_Log.print_log_to_file( PATH_PROGRAM_LOG_FOLDER, uORF_APPL_VERSION_NUMBER, SUCCESSFUL );
	#endif
	// ------------------------------



	// ------------------------------
	// Convert a DNA sequence to its translated Amino Acid sequence
	#ifdef CONVERT_DNA_SEQ_TO_AA
		string DNA_seq = prompt_for_input<string>( "Input DNA sequence to convert to AA sequence: " );
		output_text_line( "" );

		output_text_line( (string)"AA seq: " + convert_to_AA_seq( DNA_seq ) );
		return Program_Log.print_log_to_file( PATH_PROGRAM_LOG_FOLDER, uORF_APPL_VERSION_NUMBER, SUCCESSFUL );
	#endif
	// ------------------------------
	
	////////////////////////////////////////////////////////////////////////////////
	
	
	
	// ------------------------------
	// Read in and parse the S. cerevisiae GFF
	TFasta_Content * S_Cerevisiae_Chrom = new TFasta_Content( PATH_S_CEREVISIA_FASTA_FOLDER, S_CEREVISIAE_GFF_FILE_NAME );
	
	if( S_Cerevisiae_Chrom->get_contents_from_gff() )
	{
		Errors.handle_error( FATAL, "", __FILE__, __LINE__ );
	}
	// ------------------------------



	////////////////////////////////////////////////////////////////////////////////
	// Alternative Program Flows - After Reading in Yeast Genome

	// ------------------------------
	// If desired, search for an Amino Acid sequence
	#ifdef SEARCH_FOR_AA_SEQ
		ostringstream AA_find_oss;
		string AA_seq = prompt_for_input<string>( "Input AA sequence to search for: " );
		output_text_line( "" );

		for( unsigned int i = 0; i < S_Cerevisiae_Chrom->sequence.size(); i++ )
		{
			unsigned int first_inst = find_AA_seq( S_Cerevisiae_Chrom->sequence[i], AA_seq, 1);

			AA_find_oss << "AA_seq_pos in chrom " << i << ": " << ( i < 10 ? " " : "" )
						<< ( first_inst == S_Cerevisiae_Chrom->sequence[i].size() ? 0 : first_inst ) << "\n";
		}
		output_text_line( AA_find_oss.str() );
		return Program_Log.print_log_to_file( PATH_PROGRAM_LOG_FOLDER, uORF_APPL_VERSION_NUMBER, SUCCESSFUL );
	#endif
	// ------------------------------
	
	
	
	// ------------------------------
	// Extract a specified DNA sequence
	#ifdef REQUEST_SEL_SEQUENCES
		request_sel_sequence( S_Cerevisiae_Chrom->sequence );
		return Program_Log.print_log_to_file( PATH_PROGRAM_LOG_FOLDER, uORF_APPL_VERSION_NUMBER, SUCCESSFUL );
	#endif
	// ------------------------------
	 
	////////////////////////////////////////////////////////////////////////////////



	// ------------------------------
	// Compile the list of uORFs and write to CSV
	vector <TORF_Data> * ORF_Data = new vector <TORF_Data>;

	compile_uORF_list( *S_Cerevisiae_Chrom, ORF_Data );
	sort_and_consolidate( ORF_Data );
	determine_ORF_and_uORF_characteristics( *S_Cerevisiae_Chrom, ORF_Data );
	write_uORFs_to_csv( *ORF_Data, uORF_COLUMN_SELECTION_TYPE, uORFs_TO_PRINT, OUTPUT_FILE_NAMING_METHOD );
	// ------------------------------
	

	
	////////////////////////////////////////////////////////////////////////////////
	// Alternative Program Flows - After uORF list compilation/processing
	/*
	vector <vector <double> > joint_prob_nt_coding;
	vector <vector <double> > joint_prob_nt_noncoding;
	vector <double> marginal_prob_nt_coding;
	vector <double> marginal_prob_nt_noncoding;
	
	vector <string> coding_DNA   ( 0 );
	vector <string> noncoding_DNA( 0 );
	
	get_coding_and_noncoding_DNA( *S_Cerevisiae_Chrom, &coding_DNA, &noncoding_DNA );
	

	for( unsigned int i = 1; i < coding_DNA.size(); i++ )
	{
		coding_DNA[0].append   ( coding_DNA[i]    );
		noncoding_DNA[0].append( noncoding_DNA[i] );
		
		coding_DNA.erase   ( coding_DNA.begin()    + 1 );
		noncoding_DNA.erase( noncoding_DNA.begin() + 1 );
		
		i--;
	}
	
	cout << "size of coding region: " << coding_DNA[0].size() << endl;
	cout << "size of noncoding region: " << noncoding_DNA[0].size() << endl;
	
	unsigned int genome_size = 0;
	
	for( unsigned int i = 0; i < NUM_S_CEREVISIAE_CHROM; i++ ) { genome_size += 1 * S_Cerevisiae_Chrom->sequence[i].size(); }
	cout << "genome size: " << genome_size << endl;
	
	calc_joint_prob_nt_sequence( coding_DNA[0], &joint_prob_nt_coding,    	 &marginal_prob_nt_coding    );
	calc_joint_prob_nt_sequence( noncoding_DNA[0], &joint_prob_nt_noncoding, &marginal_prob_nt_noncoding );
	//calc_joint_prob_nt_TL( S_Cerevisiae_Chrom, ORF_Data, &joint_prob_nt, &marginal_prob_nt );
	
	calculate_AMI_profile( *S_Cerevisiae_Chrom, *ORF_Data, joint_prob_nt_coding,    marginal_prob_nt_coding    );
	calculate_AMI_profile( *S_Cerevisiae_Chrom, *ORF_Data, joint_prob_nt_noncoding, marginal_prob_nt_noncoding );
	*/
	//calc_AMI_uORFs( *S_Cerevisiae_Chrom, *ORF_Data );
	//count_uORF_codons( *ORF_Data );



	// ------------------------------
	#ifdef WRITE_GENE_LIST_TO_FILE
		if( write_gene_list_to_file( *ORF_Data ) )
		{
			Errors.handle_error( FATAL, "", __FILE__, __LINE__ );
		}
	#endif
	// ------------------------------
	
	
	
	// ------------------------------
	#ifdef WRITE_GENE_AND_uORFs_TO_FASTA
		write_sequences_to_fasta( *ORF_Data );
	#endif
	// ------------------------------
	
	
	
	// ------------------------------
	#ifdef ADD_COL_TO_MCMANUS_TRANSL_DATA
		add_present_in_uORF_list_col_to_McManus_transl_data( *ORF_Data );
	#endif
	// ------------------------------
	
	////////////////////////////////////////////////////////////////////////////////


	
	// ------------------------------
	// Dynamically allocated memory deletion
	delete S_Cerevisiae_Chrom;
	delete ORF_Data;
	// ------------------------------


	return Program_Log.print_log_to_file( PATH_PROGRAM_LOG_FOLDER, uORF_APPL_VERSION_NUMBER, SUCCESSFUL );
}
//==============================================================================



// Development 'main()'
/*int main( )
{
	TCSV_Contents Prac_CSV( PATH_DATA_FOLDER, "prac_csv.csv" );
	Prac_CSV.parse_csv();


	vector <string> header_row = Prac_CSV.get_csv_row<string>( 1 );

	for( unsigned int i = 0; i < header_row.size(); i++ )
	{
		cout << "header row " << i << ": " << header_row[i] << endl;
	}

	vector <string> new_header_row( 0 );

	new_header_row.push_back( "Column_1" );
	new_header_row.push_back( "Column_2" );
	new_header_row.push_back( "Column_3" );
	new_header_row.push_back( "Column_4" );
	new_header_row.push_back( "Column_5" );

	Prac_CSV.replace_row( 1, new_header_row );


	vector <string> new_last_row( 0 );

	new_last_row.push_back( "last_row_1" );
	new_last_row.push_back( "last_row_2" );
	new_last_row.push_back( "last_row_3" );
	new_last_row.push_back( "last_row_4" );
	new_last_row.push_back( "last_row_5" );

	Prac_CSV.add_row( Prac_CSV.get_cell_contents().size() + 1, new_last_row );
	Prac_CSV.add_row( 2, new_last_row );


	vector <string> new_column( 0 );

	new_column.push_back( "Column_6" );
	new_column.push_back( "contents_2b" );
	new_column.push_back( "contents_3b" );
	new_column.push_back( "contents_4b" );
	new_column.push_back( "contents_5b" );

	Prac_CSV.append_column( new_column );

	new_column[0] = "Column b2.0";
	Prac_CSV.add_column( 2, new_column );


	vector <string> new_column2( 0 );

	//new_column2.push_back( "Column_6" );
	new_column2.push_back( "contents_2" );
	new_column2.push_back( "contents_3" );
	new_column2.push_back( "contents_4" );
	new_column2.push_back( "contents_5" );

	Prac_CSV.replace_column( "Column_2", DEFAULT_HEADER_ROW, new_column2 );


	Prac_CSV.write_back_to_csv( PATH_DATA_FOLDER, "prac_csv_modified.csv" );


	return Program_Log.print_log_to_file( PATH_PROGRAM_LOG_FOLDER, uORF_APPL_VERSION_NUMBER, SUCCESSFUL );
}*/
//==============================================================================

////////////////////////////////////////////////////////////////////////////////



