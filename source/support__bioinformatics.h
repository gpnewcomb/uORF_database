//==============================================================================
// Project	   : uORF
// Name        : support__bioinformatics.h
// Author      : Garin Newcomb
// Email       : gpnewcomb@live.com   
// Version     : See "Revision History" below
// Copyright   : Copyright 2014 University of Nebraska-Lincoln
// Description : Header file declaring or defining bioinformatics support 
//				 functions and classes
//==============================================================================
//
//  Revision History
//      v0.0.0 - 2014/07/09 - Garin Newcomb
//          Initial creation of file, moving code from "support__general.h" and "uORF__compile.h", also
//			included in this program
//
//    	Appl Version at Last File Update::  v0.0.x - 2014/07/09 - Garin Newcomb
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
//     *E. Templated (Non-Member) Function Declarations
//     *F. Inline (Non-Member) Function Declarations and Definitions
//     *G. UNUSED Non-Member Function Declarations
//
////////////////////////////////////////////////////////////////////////////////



#ifndef _SUPPORT__BIOINFORMATICS_H_
#define _SUPPORT__BIOINFORMATICS_H_



////////////////////////////////////////////////////////////////////////////////
//
// A. Include Statements, Preprocessor Directives, and Related
//
////////////////////////////////////////////////////////////////////////////////

// Standard libraries and related
#include <sstream>
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

//==============================================================================

enum Tenum_GO_children_CSV_columns
{
	GO_CHILD_CSV_COL_NUM_PARENT,	
	GO_CHILD_CSV_COL_NUM_CHILDREN,	

	NUM_CSV_GO_CHILD_COLUMNS
};		
//==============================================================================



enum Tenum_nucleotides
{
	NT_A,	
	NT_C,
	NT_G,
	NT_T,	

	NUM_DIFF_NT
};		
//==============================================================================



enum Tenum_nucleotide_pairs
{
	A_AND_A,	
	A_AND_T,	
	A_AND_C,	
	A_AND_G,	
	T_AND_A,	
	T_AND_T,	
	T_AND_C,	
	T_AND_G,	
	C_AND_A,	
	C_AND_T,	
	C_AND_C,	
	C_AND_G,	
	G_AND_A,	
	G_AND_T,	
	G_AND_C,	
	G_AND_G,	

	NUM_NUCLEOTIDE_PAIRS
};		
//==============================================================================



class TFeature
{
	public:
		unsigned int start_coord;
		unsigned int length;
	
		TFeature( const unsigned int start_coordinate, const unsigned int feature_length ) :
			start_coord( start_coordinate ),
			length	   ( feature_length   )
			{ }
			
		TFeature( void ) :
			start_coord( std::string::npos ),
			length	   ( std::string::npos )
			{ }
};
//==============================================================================



class TGO_Term
{
	public:
		string name;
		string evidence;
		unsigned int level;
		string parent;
		
		TGO_Term( const std::string & GO_term_name, const std::string & GO_term_evidence, const unsigned int GO_term_level = 0, const std::string GO_term_parent = "" ) :
			name	( GO_term_name     ),
			evidence( GO_term_evidence ),
			level	( GO_term_level	   ),
			parent	( GO_term_parent   )
		{ }

};
//==============================================================================



class TGO_Annotation
{
	public:
		string GO_namespace;
		vector <TGO_Term> GO_Terms;

		void add_term( const std::string & GO_term_name, const std::string & GO_term_evidence, const unsigned int GO_term_level = 0, const std::string GO_term_parent = "" );
		string format_terms_list( void ) const;
		
		TGO_Annotation( const std::string & name_space ) : GO_namespace( name_space )
		{ }
};
//==============================================================================



class TCalculate_AUGCAI
{
	private:
		static std::vector <vector <double> > AUGCAI_weight_matrix;
		static bool AUGCAI_weight_matrix_init;
		
	public:
		double calc( const std::string & start_context ) const;
		
		TCalculate_AUGCAI( void );
};
//==============================================================================

////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//
// D. Non-Member Function Declarations
//
////////////////////////////////////////////////////////////////////////////////

void request_sel_sequence( const std::vector <std::string> & yeast_chromosomes );

std::string extract_DNA_seq( const std::string & chrom_seq, 
							 unsigned int start_coord, 
							 const unsigned int len, 
							 const bool opposite_strand, 
							 const std::vector <TFeature> untransl_reg = std::vector <TFeature> ( 0 ), 
							 const bool remove_untransl_reg_in_first_codon = FALSE );
							 
std::string get_inverted_sequence( const std::string seq_to_invert );
std::string get_opposite_strand  ( const std::string strand );

std::string convert_to_AA_seq( const std::string & DNA_seq );
bool check_if_AA_seq_matches( const std::string & DNA_seq, const std::string & AA_seq );
unsigned int find_AA_seq( const std::string & DNA_seq, const std::string & AA_seq, unsigned int start_coord = 1 );

std::string cap_start_codon_only( const std::string & start_codon_context );

int get_gene_coord_from_gff( const std::string & gene_name, 
							 const std::string & gff_annotation,
							 const bool opposite_strand,
							 unsigned int * const gene_start_coord, 
							 unsigned int * const gene_end_coord,
							 unsigned int * const  gene_intergen_start_coord,
							 std::vector <TFeature> * const untransl_reg,
							 TFeature * const fpUTR_intron );
							 
unsigned int find_first_stop_codon( const std::string & chomosome, 
									unsigned int start_pos, 
									bool opposite_strand, 
									unsigned int * const len = NULL, 
									const std::vector <TFeature> removed_reg = std::vector <TFeature> ( 0 ) 
									/*bool search_in_frame = TRUE*/ );
									
void generate_simple_GO_hierarchy( void );
void parse_GO_hierarchy( std::vector <TGO_Annotation> * const All_GO_Annotations );
void generate_list_of_children( void );
unsigned int find_GO_parent_term( const std::string & GO_database_contents, const std::string & parent_term, const unsigned int prev_parent_id_pos = 0 );
unsigned int find_GO_parent_term( const std::string & GO_database_contents, const int parent_term_num, 		 const unsigned int prev_parent_id_pos = 0 );

std::vector <std::string> get_all_children_terms( const std::string & GO_database_contents, 
												  const std::string & parent_term, 
												  const std::vector <std::string> & terms_already_found = std::vector <std::string>( 0 ), 
												  const std::vector <std::vector <std::string> > & terms_and_children = vector <std::vector <std::string> >( 0 ) );
												  
std::vector <std::string> get_all_children_terms( const std::string & GO_database_contents, const unsigned int parent_term_num );
std::string get_child_term( const std::string & GO_database_contents, const unsigned int parent_id_pos );
bool is_child_descendent_of( const std::string & child_term, const std::string & parent_term, const TGO_Annotation & GO_Annotations );
std::vector <TGO_Term> get_all_parents_of( const std::string & child_term, const TGO_Annotation & GO_Annotations );

std::vector <unsigned int> convert_DNA_string_to_num( const std::string & DNA_string  );
std::string convert_DNA_num_to_string( const std::vector <unsigned int> & DNA_numeric );

void get_coding_and_noncoding_DNA( const TFasta_Content & S_Cerevisiae_Chrom,  std::vector <std::string> * const coding_DNA, std::vector <std::string> * const noncoding_DNA );

////////////////////////////////////////////////////////////////////////////////



#endif  // _SUPPORT__BIOINFORMATICS_H_


