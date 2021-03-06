//==============================================================================
// Project	   : uORF
// Name        : uORF__process.h
// Author      : Garin Newcomb
// Email       : gpnewcomb@live.com   
// Version     : See "Revision History" below
// Copyright   : Copyright 2014 University of Nebraska-Lincoln
// Description : Header file including function prototypes and object
//				 definitions used in processing and performing operations on 
//				 uORF's
//==============================================================================
//
//  Revision History
//      v0.0.0 - 2014/07/16 - Garin Newcomb
//          Initial creation of file, pulling primarily from "uORF__compile.h", also used with
//			this program
//
//    	Appl Version at Last File Update::  v0.0.x - 2014/07/16 - Garin Newcomb
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



#ifndef uORF__PROCESS_H
#define uORF__PROCESS_H



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

// Project-specific header files:  support functions and related
#include "uORF__compile.h"

////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//
// C. Type (and Member Function) Declarations and Definitions
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//
// D. Non-Member Function Declarations
//
////////////////////////////////////////////////////////////////////////////////

void sort_and_consolidate( std::vector <TORF_Data> * const ORF_Data );
void sort_ORFs_by_GO_terms( const std::string &  GO_namespace_to_sort, std::vector <TORF_Data> * const ORF_Data, const std::vector <TGO_Annotation> & All_GO_Annotations );
void check_for_matching_GO_terms( std::vector <TORF_Data> * const ORF_Data );

bool compare_ORF_positions( const TORF_Data & ORF_1, const TORF_Data & ORF_2 );
bool compare_ORF_GO_terms ( const TORF_Data & ORF_1, const TORF_Data & ORF_2 );

void write_uORFs_to_csv( const std::vector <TORF_Data> & ORF_Data, 
						 const std::vector <Tenum_uORF_CSV_columns> & col_to_write, 
						 const unsigned int select_uORFs, 
						 const bool & file_naming_method = GENERATE_FILE_NAME );
						 
void write_uORFs_to_csv( const std::vector <TORF_Data> & ORF_Data, const unsigned int col_selection_type, const unsigned int select_uORFs, const bool & file_naming_method = GENERATE_FILE_NAME );
int  write_gene_list_to_file( const std::vector <TORF_Data> & ORF_Data );
std::string get_uORF_list_file_name( const bool & file_naming_method, const bool read_or_write );
void write_sequences_to_fasta( const std::vector <TORF_Data> & ORF_Data );

void count_nucleotides				 ( const TFasta_Content & S_Cerevisiae_Chrom );
void calc_joint_prob_nt_genes 		 ( const TFasta_Content & S_Cerevisiae_Chrom, const std::vector <TORF_Data> & ORF_Data, std::vector <std::vector <double> > * const joint_prob_nt, std::vector <double> * const marginal_prob_nt );
void calc_joint_prob_nt_TL    		 ( const TFasta_Content & S_Cerevisiae_Chrom, const std::vector <TORF_Data> & ORF_Data, std::vector <std::vector <double> > * const joint_prob_nt, std::vector <double> * const marginal_prob_nt );
void calc_AMI_uORFs			  		 ( const TFasta_Content & S_Cerevisiae_Chrom, const std::vector <TORF_Data> & ORF_Data );
void calc_mod_AMI_uORF_context	  	 ( const TFasta_Content & S_Cerevisiae_Chrom, std::vector <TORF_Data> * const ORF_Data );
void calc_joint_prob_nt_genome		 ( const TFasta_Content & S_Cerevisiae_Chrom, 											std::vector <std::vector <double> > * const joint_prob_nt, std::vector <double> * const marginal_prob_nt );
void calc_joint_prob_nt_sequence	 ( const string & sequence, 															std::vector <std::vector <double> > * const joint_prob_nt, std::vector <double> * const marginal_prob_nt );
vector <double> calculate_AMI_profile( const TFasta_Content & S_Cerevisiae_Chrom, const std::vector <TORF_Data> & ORF_Data, const std::vector <std::vector <double> > & joint_prob_nt, const std::vector <double> & marginal_prob_nt );

void count_uORF_codons( const std::vector <TORF_Data> & ORF_Data );
void cluster_uORF_context_profiles(std::vector<TORF_Data> * const ORF_Data);

////////////////////////////////////////////////////////////////////////////////



#endif // uORF__PROCESS_H


