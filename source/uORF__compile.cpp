//==============================================================================
// Project	   : uORF
// Name        : uORF__compile.cpp
// Author      : Garin Newcomb
// Email       : gpnewcomb@live.com   
// Version     : See "Revision History" below
// Copyright   : Copyright 2014 University of Nebraska-Lincoln
// Description : Source code to extract and compile uORF's
//==============================================================================
//
//  Revision History
//      v0.0.0 - 2014/06/05 - Garin Newcomb
//          Initial creation of file, pulling primarily from the previous "uORF_manip.cpp" used with this program
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
//      E. UNUSED Non-Member Function Definitions
//
////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//
// A. Include Statements, Preprocessor Directives, and Related
//
////////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>   	// For find()
#include <cmath>   		// For abs( double )

using namespace std;

// Project-specific header files:  definitions and related information
#include "defs__general.h"
#include "defs__appl_parameters.h"

// Project-specific header files:  support functions and related
#include "support__file_io.h"
#include "support__general.h"
#include "support__bioinformatics.h"
#include "uORF__process.h"

// Header file for this file
#include "uORF__compile.h"

//==============================================================================





////////////////////////////////////////////////////////////////////////////////
//
// B. Global Variable Declarations (including those in other files)
//
////////////////////////////////////////////////////////////////////////////////

extern TErrors Errors;						// Stores information about errors and responds to them in several ways

// Static Variable Definitions
vector <vector <double> > TCalculate_AUGCAI::AUGCAI_weight_matrix 	   = vector <vector <double> > ( NUM_AUGCAI_POSITIONS, vector <double> ( NUM_DIFF_NT, 0 ) );
bool 					  TCalculate_AUGCAI::AUGCAI_weight_matrix_init = FALSE;

// vector <string> GO_terms_not_found( 0, "" );

int max_upstream = 5080;
int min_downstream = -2323;

////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//
// C. Member Function Definitions
//
////////////////////////////////////////////////////////////////////////////////

//==============================================================================

string TData_Source::get_author( void ) const
{
	return author;
}
//------------------------------------------------------------------------------


string TData_Source::get_evidence( void ) const
{
	return evidence_type;
}
//------------------------------------------------------------------------------
		
		
TData_Source::TData_Source( const string & paper_author ) : author( paper_author )
{
	if	   ( author == DATA_SOURCE_MIURA			  ) { evidence_type = EVIDENCE_TYPE_MIURA; 				}
	else if( author == DATA_SOURCE_INGOLIA			  ) { evidence_type = EVIDENCE_TYPE_INGOLIA; 			}
	else if( author == DATA_SOURCE_ZHANG_DIETRICH_NAR ) { evidence_type = EVIDENCE_TYPE_ZHANG_DIETRICH_NAR; }
	else if( author == DATA_SOURCE_ZHANG_DIETRICH_CG  ) { evidence_type = EVIDENCE_TYPE_ZHANG_DIETRICH_CG;  }
	else if( author == DATA_SOURCE_NAGALAKSHMI		  ) { evidence_type = EVIDENCE_TYPE_NAGALAKSHMI; 		}
	else if( author == DATA_SOURCE_CVIJOVIC			  ) { evidence_type = EVIDENCE_TYPE_CVIJOVIC; 			}
	else if( author == DATA_SOURCE_GUAN				  ) { evidence_type = EVIDENCE_TYPE_GUAN;			 	}
	else if( author == DATA_SOURCE_LAWLESS			  ) { evidence_type = EVIDENCE_TYPE_LAWLESS; 			}
	else if( author == DATA_SOURCE_SELPI			  ) { evidence_type = EVIDENCE_TYPE_SELPI; 				}
	else 
	{
		Errors.handle_error( FATAL, "Error in 'TData_Source::TData_Source()': Paper author not recognized. ", __FILE__, __LINE__ );
	}
}
//------------------------------------------------------------------------------		
		
//==============================================================================


string TuORF_Effect::get_source( void ) const
{
	return source;
}
//------------------------------------------------------------------------------	


string TuORF_Effect::get_effect( void ) const
{
	return effect;
}
//------------------------------------------------------------------------------	


bool TuORF_Effect::get_hypoth( void ) const
{	
	return hypoth;
}
//------------------------------------------------------------------------------	


bool TuORF_Effect::get_specified( void ) const
{	
	return specified;
}
//------------------------------------------------------------------------------		
	

string TuORF_Effect::print_effect_to_str( void ) const
{
	string effect_str = "";
	
	if( effect != "" )
	{
		effect_str += effect;
		
		if( specified == FALSE )
		{
			effect_str += (string)", particular uORF not specified";
		}
		
		if( source != "" )
		{
			effect_str += (string)" (" + ( hypoth == TRUE ? "Hypothosized - " : "" ) + source + ")";
		}
	}
	
	return effect_str;
}
//------------------------------------------------------------------------------

	
//==============================================================================



TuORF_Data::TuORF_Data( const string uORF_content,                                  
						const string uORF_start_context,                            
						const string uORF_ext_start_context,                        
						const int relative_uORF_pos,                                     
						const unsigned int uORF_start_pos,                               
						const unsigned int uORF_len,                                     
						const unsigned int exp_uORF_len,                                                                      
						const string & data_source,  
						const bool uORF_in_frame, 						
						const bool start,                                                
						const unsigned int nt_start_moved,                               
						const bool end_past_gene ) :

	content( uORF_content ),
	start_context( uORF_start_context ),
	ext_start_context( uORF_ext_start_context ),
	context_profile( vector<unsigned int>( 0 ) ),
	rel_uORF_pos( relative_uORF_pos ),
	start_pos( uORF_start_pos ),
	len( uORF_len ),
	exp_len( exp_uORF_len ),
	AUGCAI( DEFAULT_AUGCAI ),
	source( vector <TData_Source>( 1, TData_Source( data_source ) ) ),
	num_ribosomes( DEFAULT_NUM_RIBOSOMES ),
	num_mRNA( DEFAULT_NUM_mRNA ),
	transl_effic_effect( TuORF_Effect() ),
	distance_to_tss( DEFAULT_DISTANCE_TO_TSS ),
	cap_distance_index( DEFAULT_CDI ),
	prob_of_translation( DEFAULT_PROBABILITY ), 
	cluster( DEFAULT_CLUSTER ), 
	distortion( DEFAULT_DISTORTION ),
	avg_cap_dist( DEFAULT_DISTANCE_TO_TSS ),
	prop_transcripts_incl( DEFAULT_PERCENTAGE ),
	in_frame( uORF_in_frame ),
	start_codon( start ),
	num_nt_start_moved( nt_start_moved ),
	end_past_gene_start( end_past_gene ),
	exp_transl_tss( NO_KNOWN_TSS )

{ 
	num_nt_len_changed = (int)len - (int)exp_len;
	
	if( start_codon == FALSE || end_past_gene_start == TRUE || num_nt_len_changed != 0 ) { problem = TRUE;  }
	else													  							 { problem = FALSE; }
}
//------------------------------------------------------------------------------		
		

// Dummy Constructor - used when "pretending" the gene is a uORF		
TuORF_Data::TuORF_Data( const unsigned int uORF_start_pos ) : 
	rel_uORF_pos( 0 ),
	start_pos( uORF_start_pos )

{ }
//------------------------------------------------------------------------------	

//==============================================================================		
		
		
				
vector <TuORF_Data> TORF_Data::get_uORFs( void ) const
{
	return uORFs;
}
//------------------------------------------------------------------------------


TuORF_Data TORF_Data::get_uORF( unsigned int uORF_it ) const
{
	if( uORF_it >= uORFs.size() )
	{
		Errors.handle_error( FATAL, "Error in 'TORF_Data::get_uORF()': The uORF that was attempted to be retrieved does not exist. ", __FILE__, __LINE__ );
	}
	
	return uORFs[uORF_it];
}
//------------------------------------------------------------------------------


void TORF_Data::delete_uORF( unsigned int uORF_it )
{
	if( uORF_it >= uORFs.size() )
	{
		Errors.handle_error( FATAL, "Error in 'TORF_Data::delete_uORF()': The uORF that was attempted to be modified does not exist. ", __FILE__, __LINE__ );
	}
	
	uORFs.erase( uORFs.begin() + uORF_it );
	
	return;
}
//------------------------------------------------------------------------------


void TORF_Data::add_uORF( const TuORF_Data uORF )
{
	uORFs.push_back( uORF );
	
	return;
}
//------------------------------------------------------------------------------


int TORF_Data::extract_uORF( const int exp_rel_uORF_pos,
							 const unsigned int exp_uORF_len,
							 const string & exp_start_codon,
							 const string & chrom_seq,
							 const string & data_source,
							 unsigned int * const num_realigned_uORFs,
							 unsigned int * const num_misaligned_uORFs,
							 const string & exp_start_context,
							 const unsigned int num_ribosomes,
							 const unsigned int num_mRNA )
{
	// ------------------------------
	// uORF parameter declarations
	string uORF_content;
	int rel_uORF_pos = exp_rel_uORF_pos;
	unsigned int uORF_len = exp_uORF_len;
	string uORF_start_context = "";
	string ext_uORF_start_context = "";
	unsigned int uORF_start_pos;
	bool uORF_in_frame = FALSE;
	
	bool start_codon_found = TRUE;
	int num_nt_start_moved = 0;
	bool end_past_gene_start = FALSE;
	// ------------------------------

	
	
	// ------------------------------
	// Extract the uORF, removing any untranslated regions if necessary
	int uORF_first_coord_in_chrom_it;
	unsigned int gene_start_it = gene_CDS.start_coord - 1;
	
	if( opposite_strand == FALSE )   
	{ 
		uORF_first_coord_in_chrom_it = gene_start_it + rel_uORF_pos;
	}
	else/*( opposite_strand == TRUE )*/ 
	{ 
		uORF_first_coord_in_chrom_it = gene_start_it - rel_uORF_pos - ( uORF_len - 1 );
	}
	
	int uORF_last_coord_in_chrom_it = uORF_first_coord_in_chrom_it + uORF_len - 1;

	
	if( uORF_first_coord_in_chrom_it < 0 )
	{
		return Errors.handle_error( PASS_UP_ONE_LEVEL, "Error in 'TORF_Data::extract_uORF()': uORF start position was less than 1. ", __FILE__, __LINE__ );
	}


	if( uORF_last_coord_in_chrom_it >= (int)chrom_seq.size() )
	{
		return Errors.handle_error( PASS_UP_ONE_LEVEL, "Error in 'TORF_Data::extract_uORF()': uORF end position was greater than the length of the genome. ", __FILE__, __LINE__ );
	}
		

	if	  ( opposite_strand == FALSE )  { uORF_start_pos = uORF_first_coord_in_chrom_it + 1; }
	else/*( opposite_strand == TRUE )*/ { uORF_start_pos = uORF_last_coord_in_chrom_it  + 1; }
	
	uORF_content = extract_DNA_seq( chrom_seq, uORF_start_pos, uORF_len,  opposite_strand, untransl_reg );

	if( exp_start_codon.size() != 3 )
	{
		Errors.handle_error( FATAL, "Error in 'TORF_Data::extract_uORF()': Expected start codon passed did not contain 3 nucleotides. ", __FILE__, __LINE__ );
	}
	
	// Check that the uORF extracted correctly starts with the intended start codon; if not, attempt to realign
	if( uORF_content.substr( 0, 3) != exp_start_codon  )
	{ 
		realign_uORF(  chrom_seq,
					   gene_name,
					   opposite_strand,
					   exp_start_codon,
					   uORF_len,
					   &rel_uORF_pos,
					   &uORF_first_coord_in_chrom_it,
					   &uORF_content,
					   &num_nt_start_moved,
					   &start_codon_found  );
					  
		if( start_codon_found == TRUE  )   
		{ 
			if( num_realigned_uORFs  != NULL ) { (*num_realigned_uORFs)++;  }
		}
		else/*( start_codon_found == FALSE )*/ 
		{ 
			if( num_misaligned_uORFs != NULL ) { (*num_misaligned_uORFs)++; }  
		}
	}
	// ------------------------------
	

	
	// ------------------------------
	// Extract the start codon context, find the stop codon, get uORF characteristics
	vector <TFeature> untransl_reg_all = untransl_reg;
	if( fpUTR_intron.start_coord != string::npos ) { untransl_reg_all.insert( untransl_reg_all.begin(), fpUTR_intron ); }
	
	if( uORF_analysis( chrom_seq,
					   gene_name,
					   gene_CDS.start_coord,
					   opposite_strand,
					   start_codon_found,
					   rel_uORF_pos,
					   uORF_first_coord_in_chrom_it,
					   untransl_reg_all,
					   &uORF_content,
					   &uORF_start_context,
					   &ext_uORF_start_context,
					   &uORF_start_pos,
					   &uORF_len,
					   &end_past_gene_start,
					   &uORF_in_frame ) )
	{
		return Errors.handle_error( PASS_UP_ONE_LEVEL, "Error in 'TORF_Data::extract_uORF()': ", __FILE__, __LINE__ );
	}
	// ------------------------------

	
	
	// ------------------------------
	// If the uORF was correctly found, and the expected start context is known, make sure the start context matches what was expected
	if( start_codon_found == TRUE && exp_start_context.empty() == FALSE )
	{
		// Allow for one substitution in the start codon context
		if( get_num_diff_chars( uORF_start_context, exp_start_context ) > 1 )
		{
			ostringstream error_oss;
			error_oss << "Error in 'TORF_Data::extract_uORF()': For uORF located at "
					  << rel_uORF_pos
					  << " relative to gene " 
					  << gene_name 
					  << " the start codon context ("
					  << uORF_start_context
					  << ") did not match what was expected ("
					  << exp_start_context
					  << "). ";
					  
			return Errors.handle_error( PASS_UP_ONE_LEVEL, error_oss.str(), __FILE__, __LINE__ );
		}
	}
	// ------------------------------
	
	
	
	// ------------------------------
	// Actually add the uORF to the data vector now that all information has been gathered	
	add_uORF( TuORF_Data( uORF_content,
						  uORF_start_context,
						  ext_uORF_start_context,
						  rel_uORF_pos,
						  uORF_start_pos,
						  uORF_len,
						  exp_uORF_len,
						  data_source,
						  
						  uORF_in_frame,
						  start_codon_found,
						  num_nt_start_moved,
						  end_past_gene_start ) );
						  
	update_num_rib( ( uORFs.size() - 1 ), num_ribosomes, num_mRNA );
	// ------------------------------
	
		
	return SUCCESSFUL;
}
//------------------------------------------------------------------------------


void TORF_Data::add_GO_Annotation( const string & name_space, const string & term, const string & evidence, const vector <TGO_Annotation > & All_GO_Annotations )
{
	unsigned int GO_namespace_it = 0;
	
	// Find the GO Annotation namespace
	while( GO_namespace_it < GO_Annotations.size() && GO_Annotations[GO_namespace_it].GO_namespace != name_space )
	{
		GO_namespace_it++;
	}
	
	if( GO_namespace_it >= GO_Annotations.size() )
	{
		Errors.handle_error( FATAL, (string)"Error in 'TORF_Data::add_GO_Annotation()': For gene " + gene_name + 
											", the GO Annotation namespace that was attempted to be accessed (" + name_space + 
											") does not exist. ", __FILE__, __LINE__ );
	}
	
	
	// For some annotations, if a curator was unable to find any biological data, the GO term is the same as the 
	//  namespace, with an evidence code of "ND" for "No Data."  These should accordingly not be added to the list
	if( term != name_space )
	{
		unsigned int GO_term_it = 0;
		
		// Find the GO Annotation term
		while( GO_term_it < All_GO_Annotations.at( GO_namespace_it).GO_Terms.size() &&  All_GO_Annotations.at( GO_namespace_it).GO_Terms[GO_term_it].name != term )
		{
			GO_term_it++;
		}
		
		
		if( GO_term_it >= All_GO_Annotations[ GO_namespace_it ].GO_Terms.size() )
		{
			Errors.handle_error( FATAL, (string)"Error in 'TORF_Data::add_GO_Annotation()': For gene " + gene_name + 
												", the GO Annotation term that was attempted to be accessed (" + term + 
												") does not exist. ", __FILE__, __LINE__ );
			
			
			// Add the term to the list of those that couldn't be found (in order to determine what terms need to be replaced with their canonical synonyms in the "gene_info.csv" file)
			/*
			unsigned int GO_term_not_found_it = 0;
			
			while( GO_term_not_found_it < GO_terms_not_found.size() &&  GO_terms_not_found[GO_term_not_found_it] != term )
			{
				GO_term_not_found_it++;
			}	

			if( GO_term_not_found_it >= GO_terms_not_found.size() )
			{
				GO_terms_not_found.push_back( term );
			}
			*/
			
		}
		else
		{
			vector <TGO_Term>::iterator curr_GO_it = GO_Annotations[GO_namespace_it].GO_Terms.begin();
			bool parent_term_already_added = FALSE;
			
			// Find where the GO annotation should be added to the list (in order of level, then alphabetical)
			while( ( curr_GO_it < GO_Annotations[GO_namespace_it].GO_Terms.end() ) && 
				   ( curr_GO_it->level <  All_GO_Annotations[ GO_namespace_it ].GO_Terms[GO_term_it].level || 
				   ( curr_GO_it->level == All_GO_Annotations[ GO_namespace_it ].GO_Terms[GO_term_it].level && curr_GO_it->name.compare( term ) < 0 ) ) )
			{
				if( curr_GO_it->name ==	All_GO_Annotations[ GO_namespace_it ].GO_Terms[GO_term_it].parent )
				{
					parent_term_already_added = TRUE;
				}
				
				curr_GO_it++;
			}
			
			
			vector <TGO_Term>::iterator new_GO_it = 
			GO_Annotations[GO_namespace_it].GO_Terms.insert( curr_GO_it, TGO_Term( term, 
																				   evidence, 
																				   All_GO_Annotations[ GO_namespace_it ].GO_Terms[GO_term_it].level,
																				   All_GO_Annotations[ GO_namespace_it ].GO_Terms[GO_term_it].parent ) );
																				   
			
			if( ( most_specific_GO_term[GO_namespace_it].level <  new_GO_it->level ) || 
				( most_specific_GO_term[GO_namespace_it].level == new_GO_it->level &&  most_specific_GO_term[GO_namespace_it].name.compare( new_GO_it->name ) > 0 ) )
			{
				most_specific_GO_term[GO_namespace_it] = *new_GO_it;
			}
			
			
			if( parent_term_already_added == FALSE && All_GO_Annotations[ GO_namespace_it ].GO_Terms[GO_term_it].level != 1 )
			{
				vector <TGO_Term> all_parent_terms = get_all_parents_of( term, All_GO_Annotations[ GO_namespace_it ] );
				unsigned int parent_term_it = 0;
				
				do
				{
					curr_GO_it = GO_Annotations[GO_namespace_it].GO_Terms.begin();
					
					// Find where the GO annotation should be added to the list (in order of level, then alphabetical)
					while( ( curr_GO_it < GO_Annotations[GO_namespace_it].GO_Terms.end() )    && 
						   ( curr_GO_it->level <  all_parent_terms[parent_term_it].level || 
						   ( curr_GO_it->level == all_parent_terms[parent_term_it].level && curr_GO_it->name.compare( all_parent_terms[parent_term_it].name ) < 0 ) ) )
					{
						if( curr_GO_it->name ==	all_parent_terms[parent_term_it].parent )
						{
							parent_term_already_added = TRUE;
						}
						
						curr_GO_it++;
					}
					
					
					GO_Annotations[GO_namespace_it].GO_Terms.insert( curr_GO_it, TGO_Term( all_parent_terms[parent_term_it].name, 
																						   "", 
																						   all_parent_terms[parent_term_it].level,
																						   all_parent_terms[parent_term_it].parent ) );
				
			
					parent_term_it++;
					
				} while( parent_term_already_added == FALSE && parent_term_it < all_parent_terms.size() );
			}
		}
	}
	
	return;
}
//------------------------------------------------------------------------------


void TORF_Data::sort_tss( void )
{
	if( tss_rel_pos.size() > 1 )
	{
		sort( tss_rel_pos.begin(), tss_rel_pos.end() );
	}
	
	return;
}
//------------------------------------------------------------------------------


void TORF_Data::add_tss( int new_tss )
{
	if( find( tss_rel_pos.begin(), tss_rel_pos.end(), new_tss ) == tss_rel_pos.end() )
	{
		tss_rel_pos.push_back( new_tss );
	}
	
	return;
}
//------------------------------------------------------------------------------


void TORF_Data::add_tss( vector <int> new_tss )
{
	for( unsigned int i = 0; i < new_tss.size(); i++ )
	{
		add_tss( new_tss[i] );
	}
	
	return;
}
//------------------------------------------------------------------------------

		
void TORF_Data::calculate_AUGCAI_values( void )
{
	TCalculate_AUGCAI Calculate_AUGCAI;
	
	gene_AUGCAI = Calculate_AUGCAI.calc( gene_start_context );
	
	for( unsigned int uORF_it = 0; uORF_it < uORFs.size(); uORF_it++ )
	{
		uORFs[uORF_it].AUGCAI = Calculate_AUGCAI.calc( uORFs[uORF_it].start_context );
	}
	
	return;
}
//------------------------------------------------------------------------------


void TORF_Data::update_AUGCAI( const unsigned int uORF_it, double new_AUGCAI )
{
	if( uORF_it >= uORFs.size() )
	{
		Errors.handle_error( FATAL, "Error in 'TORF_Data::update_AUGCAI()': The uORF that was attempted to be modified does not exist. ", __FILE__, __LINE__ );
	}
	
	uORFs[uORF_it].AUGCAI = new_AUGCAI;
	
	return;
}
//------------------------------------------------------------------------------


void TORF_Data::update_num_rib( const unsigned int uORF_it, const int new_num_rib, const int new_num_mRNA )
{
	if( uORF_it >= uORFs.size() )
	{
		Errors.handle_error( FATAL, "Error in 'TORF_Data::update_num_rib()': The uORF that was attempted to be modified does not exist. ", __FILE__, __LINE__ );
	}
	
	uORFs[uORF_it].num_ribosomes = new_num_rib;
	uORFs[uORF_it].num_mRNA 	 = new_num_mRNA;
	
	return;
}
//------------------------------------------------------------------------------


void TORF_Data::add_source( const unsigned int uORF_it, unsigned int pos_to_add, string added_source )
{
	if( uORF_it >= uORFs.size() )
	{
		Errors.handle_error( FATAL, "Error in 'TORF_Data::add_source()': The uORF that was attempted to be modified does not exist. ", __FILE__, __LINE__ );
	}
	
	if( pos_to_add > uORFs[uORF_it].source.size() )
	{
		Errors.handle_error( FATAL, "Error in 'TORF_Data::add_source()': The uORF source position that was attempted to have an insertion does not exist. ", __FILE__, __LINE__ );
	}
	
	uORFs[uORF_it].source.insert( ( uORFs[uORF_it].source.begin() + pos_to_add ), TData_Source( added_source ) );
	
	return;
}
//------------------------------------------------------------------------------


void TORF_Data::update_source( const unsigned int uORF_it, vector <TData_Source> data_source )
{
	if( uORF_it >= uORFs.size() )
	{
		Errors.handle_error( FATAL, "Error in 'TORF_Data::update_source()': The uORF that was attempted to be modified does not exist. ", __FILE__, __LINE__ );
	}
	
	uORFs[uORF_it].source = data_source;
	
	return;
}
//------------------------------------------------------------------------------


void TORF_Data::update_context_profile( const unsigned int uORF_it, vector<unsigned int> new_context_profile )
{
	if (uORF_it >= uORFs.size())
	{
		Errors.handle_error( FATAL, "Error in 'TORF_Data::update_context_profile()': The uORF that was attempted to be modified does not exist. ", __FILE__, __LINE__ );
	}

	uORFs[uORF_it].context_profile = new_context_profile;

	return;
}
//------------------------------------------------------------------------------


void TORF_Data::update_cluster( const unsigned int uORF_it, int new_cluster )
{
	if (uORF_it >= uORFs.size())
	{
		Errors.handle_error( FATAL, "Error in 'TORF_Data::update_cluster()': The uORF that was attempted to be modified does not exist. ", __FILE__, __LINE__ );
	}

	uORFs[uORF_it].cluster = new_cluster;

	return;
}
//------------------------------------------------------------------------------


void TORF_Data::update_distortion( const unsigned int uORF_it, double new_distortion )
{
	if (uORF_it >= uORFs.size())
	{
		Errors.handle_error( FATAL, "Error in 'TORF_Data::update_distortion()': The uORF that was attempted to be modified does not exist. ", __FILE__, __LINE__ );
	}

	uORFs[uORF_it].distortion = new_distortion;

	return;
}
//------------------------------------------------------------------------------


void TORF_Data::clear_start_pos_change( const unsigned int uORF_it )
{
	if( uORF_it >= uORFs.size() )
	{
		Errors.handle_error( FATAL, "Error in 'TORF_Data::clear_start_pos_change()': The uORF that was attempted to be modified does not exist. ", __FILE__, __LINE__ );
	}
	
	uORFs[uORF_it].num_nt_start_moved = 0;
	
	return;
}
//------------------------------------------------------------------------------


void TORF_Data::clear_length_change( const unsigned int uORF_it )
{
	if( uORF_it >= uORFs.size() )
	{
		Errors.handle_error( FATAL, "Error in 'TORF_Data::clear_length_change()': The uORF that was attempted to be modified does not exist. ", __FILE__, __LINE__ );
	}
	
	uORFs[uORF_it].num_nt_len_changed = 0;
	
	return;
}
//------------------------------------------------------------------------------
		
	
void TORF_Data::clear_problem( const unsigned int uORF_it )
{
	if( uORF_it >= uORFs.size() )
	{
		Errors.handle_error( FATAL, "Error in 'TORF_Data::clear_problem()': The uORF that was attempted to be modified does not exist. ", __FILE__, __LINE__ );
	}
	
	uORFs[uORF_it].problem = FALSE;
	
	return;
}
//------------------------------------------------------------------------------

	
int TORF_Data::add_uORF_transl_effect( const string & effect, const string & source, const bool hypthothesized, const int uORF_rel_pos )
{
	if( uORF_rel_pos != 0 )
	{
		unsigned int uORF_it = find_uORF( uORF_rel_pos ); 
		
		if( uORF_it == uORFs.size() )
		{
			return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'TORF_Data::add_uORF_transl_effect(): Specified uORF (at " + get_str( uORF_rel_pos ) + 
																   " relatieve to gene " + gene_name + ") not found. ", __FILE__, __LINE__ );
		}
		
		uORFs[uORF_it].transl_effic_effect = TuORF_Effect( effect, source, hypthothesized, SPECIFIED );
	}
	else/*( uORF_rel_pos == 0 )*/
	{
		for( unsigned int i = 0; i < uORFs.size(); i++ )
		{	
			uORFs[i].transl_effic_effect = TuORF_Effect( effect, source, hypthothesized, UNSPECIFIED );
		
		}
	}
	
	
	return SUCCESSFUL;
}
//------------------------------------------------------------------------------


void TORF_Data::check_uORF_upstream_of_tss( void )
{
	sort_tss();

	for( unsigned int i = 0; i < uORFs.size(); i++ )
	{	
		if( tss_rel_pos.empty() == FALSE )
		{
			if( uORFs[i].rel_uORF_pos < tss_rel_pos[0] )
			{
				uORFs[i].exp_transl_tss = uORF_UPSTREAM_OF_ALL_KNOWN_TSS;
				uORFs[i].problem 		= TRUE;
			}
			else/*( uORFs[i].rel_pos_uORF <= tss_rel_pos[0] )*/
			{
				uORFs[i].distance_to_tss = uORFs[i].rel_uORF_pos - tss_rel_pos[0];
				
				// uORFs[i].exp_transl_tss  = uORF_DOWNSTREAM_OF_KNOWN_TSS;
				
				if( uORFs[i].distance_to_tss < 12 )
				{
					uORFs[i].exp_transl_tss = uORF_TOO_CLOSE_TO_TSS;
				}
				else if( uORFs[i].distance_to_tss <= 19 )
				{
					uORFs[i].exp_transl_tss = uORF_INEFFICIENTLY_TRANSLATED;
				}
				else
				{
					uORFs[i].exp_transl_tss = uORF_EFFICIENTLY_TRANSLATED;
				}
			}
		}
		else/*( tss_rel_pos.empty() == TRUE )*/
		{
			uORFs[i].exp_transl_tss = NO_KNOWN_TSS;
		}
	}
	
	return;
}
//------------------------------------------------------------------------------


void TORF_Data::get_CDI( const std::vector <unsigned int> TL_read_positions, const std::vector <double> TL_read_densities )
{
	unsigned int intergen_start_it = 0;

	while( intergen_start_it < TL_read_positions.size() && 
		   TL_read_positions[intergen_start_it] < gene_intergen_start_coord && 
		   TL_read_positions[intergen_start_it] < gene_CDS.start_coord )
	{
		intergen_start_it++;
	}
	
	if( intergen_start_it == TL_read_positions.size() ) { Errors.handle_error( FATAL, (string)"No reads mapped to upstream intergenic region of gene " + gene_name, __FILE__, __LINE__ ); }
	
	
	unsigned int intergen_end_it = intergen_start_it;
	
	while( intergen_end_it < TL_read_positions.size() && 
		   ( TL_read_positions[intergen_end_it] < gene_intergen_start_coord || 
		     TL_read_positions[intergen_end_it] < gene_CDS.start_coord ) )
	{
		intergen_end_it++;
	}
	
	intergen_end_it--;
	
	
	double total_intergenic_reads = 0;
	double weighted_sum = 0;
	
	for( unsigned int i = intergen_start_it; i <= intergen_end_it; i++ )
	{
		if( ( opposite_strand == FALSE && TL_read_densities[i] > 0 ) || ( opposite_strand == TRUE && TL_read_densities[i] < 0 ) )
		{ 
			total_intergenic_reads += TL_read_densities[i]; 
			weighted_sum 		   += TL_read_positions[i] * TL_read_densities[i]; 
		}
	}

	
	
	// Calculate the weighting between the TL read data and TSS position data
	
	/*// V2 Method
	double TL_reads_weight = ( ( ( abs( total_intergenic_reads ) >= 30 ) ? log10( abs( total_intergenic_reads ) ) : 0 ) - ( ( tss_rel_pos.size() > 0 ) ? log10( tss_rel_pos.size() ) + .2 : 0 ) ) /
							   ( ( abs( total_intergenic_reads ) >= 30 ) ? log10( abs( total_intergenic_reads ) ) : 1 );

	if( TL_reads_weight < 0 ) { TL_reads_weight = 0; }
	*/
	
	// V3 Method
	/*
	double TL_reads_weight = 1 - ( tss_rel_pos.size() > 0 ? log2( tss_rel_pos.size() ) / 10 + .15 : 0 );
	
	if( abs( total_intergenic_reads ) < 30 ) { TL_reads_weight = 0; }
	*/
	
	
	/*// V4 Method
	double TL_reads_weight = 0;

	if	   ( tss_rel_pos.size() 		   == 0  ) { TL_reads_weight = 1; }
	else if( abs( total_intergenic_reads ) >= 30 ) 
	{ 
		TL_reads_weight = .7 * ( logb( abs( total_intergenic_reads ), 115557.0 ) - logb( (double)( tss_rel_pos.size() + 1 ), 25.0 ) + 1 );

		if( TL_reads_weight > 1 ) { TL_reads_weight = 1; }
	}
	*/

	// V5 Method
	double TL_reads_weight = 0;
	if	   ( abs( total_intergenic_reads ) >= 30 && tss_rel_pos.size() == 0 ) { TL_reads_weight = 1; }
	else if( abs( total_intergenic_reads ) >= 30 && tss_rel_pos.size() >  0 ) 
	{ 
		TL_reads_weight = .54 * ( ( ( abs( total_intergenic_reads ) <= 115557 ) ? logb( abs( total_intergenic_reads  ), 115557  ) : 1 ) - 
								  ( ( tss_rel_pos.size()            <= 24     ) ? logb(    ( tss_rel_pos.size() + .5 ), 24 + .5 ) : 1 ) + 1 );

		if	   ( TL_reads_weight > 1 ) { TL_reads_weight = 1; }
		else if( TL_reads_weight < 0 ) { TL_reads_weight = 0; } 
		
		if( abs( total_intergenic_reads ) > 700 && tss_rel_pos.size() >= 4 ) { TL_reads_weight = .75; } 
	}
	//  else if( abs( total_intergenic_reads ) < 30 && tss_rel_pos.size() >= 3 ) { TL_reads_weight = 0; }  
	
	
	
	// Add a dummy uORF in order to easily calculate the average TSS position relative to the ORF
	uORFs.push_back( TuORF_Data( gene_CDS.start_coord ) );
	
	for( unsigned int uORF_it = 0; uORF_it < uORFs.size(); uORF_it++ )
	{
		bool ingolia_uORF = FALSE;
		vector <double> TSS_distribution( max_upstream - min_downstream + 1, 0 );
		uORFs[uORF_it].prop_transcripts_incl = 0;

		for( unsigned int i = 0; i < uORFs[uORF_it].source.size(); i++ )
		{
			if( uORFs[uORF_it].source[i].get_author() == "Ingolia" ) { ingolia_uORF = TRUE; }
		}


		uORFs[uORF_it].cap_distance_index = 0;
		
		double TL_reads_component = 0;
		double TSS_pos_component  = 0;
		
		
		// Calculate the TL reads component for the CDI
		if( abs( total_intergenic_reads ) >= 30 )
		{
			for( unsigned int i = intergen_start_it; i <= intergen_end_it; i++ )
			{
				if( ( opposite_strand == FALSE && TL_read_densities[i] > 0 ) || ( opposite_strand == TRUE && TL_read_densities[i] < 0 ) )
				{
					int distance_to_cap = uORFs[uORF_it].start_pos - TL_read_positions[i];
					if( opposite_strand == TRUE ) { distance_to_cap *= (-1); }
					
					if     ( distance_to_cap > 19 						   ) { TL_reads_component += 							      ( TL_read_densities[i] / total_intergenic_reads ); }
					else if( distance_to_cap > 11 && distance_to_cap <= 19 ) { TL_reads_component += ( distance_to_cap - 11 ) / 9.0 * ( TL_read_densities[i] / total_intergenic_reads ); }
					
					//if( distance_to_cap > max_upstream   ) { max_upstream   = distance_to_cap; }
					//if( distance_to_cap < min_downstream ) { min_downstream = distance_to_cap; }
					
					TSS_distribution[max_upstream - distance_to_cap]     			  += TL_read_densities[i] / total_intergenic_reads * TL_reads_weight;
					if( distance_to_cap >= 0 ) { uORFs[uORF_it].prop_transcripts_incl += TL_read_densities[i] / total_intergenic_reads * TL_reads_weight; }
				}
			}
		}
		

		// Calculate the TSS position component for the CDI
		for( unsigned int i = 0; i < tss_rel_pos.size(); i++ )
		{
			int distance_to_cap = uORFs[uORF_it].rel_uORF_pos - tss_rel_pos[i];

			if     ( distance_to_cap > 19 						   ) { TSS_pos_component += 							     ( 1.0 / tss_rel_pos.size() ); }
			else if( distance_to_cap > 11 && distance_to_cap <= 19 ) { TSS_pos_component += ( distance_to_cap - 11 ) / 9.0 * ( 1.0 / tss_rel_pos.size() ); }
			
			//if( distance_to_cap > max_upstream   ) { max_upstream   = distance_to_cap; }
			//if( distance_to_cap < min_downstream ) { min_downstream = distance_to_cap; }
			
			TSS_distribution[max_upstream - distance_to_cap]      			  += 1.0 / tss_rel_pos.size() * ( 1 - TL_reads_weight );
			if( distance_to_cap >= 0 ) { uORFs[uORF_it].prop_transcripts_incl += 1.0 / tss_rel_pos.size() * ( 1 - TL_reads_weight ); }
		} 
		

		
		// Note: better correlation between CDI and Translation likelihood if these are included (presumably because of TSS's still contributing,
		// even if less than 3
		/*if( ( opposite_strand == FALSE && uORFs[uORF_it].start_pos < gene_intergen_start_coord ) ||
			( opposite_strand == TRUE  && uORFs[uORF_it].start_pos > gene_intergen_start_coord ) )
		{
			TL_reads_weight = 0;
		}*/
		
		//cout << "TL reads weight: " << TL_reads_weight << endl;

		uORFs[uORF_it].cap_distance_index = ( TL_reads_component * TL_reads_weight ) + ( TSS_pos_component * ( 1 - TL_reads_weight ) );
		
		if( TL_reads_weight == 0 && tss_rel_pos.size() < 3 ) { uORFs[uORF_it].cap_distance_index = -1; }
		
		
		// Also calculate the average TL length 
		uORFs[uORF_it].avg_cap_dist = 0;
		for( unsigned int i = 0; i < max_upstream /*TSS_distribution.size()*/; i++ )
		{
			uORFs[uORF_it].avg_cap_dist += TSS_distribution[i] * (max_upstream - i) / uORFs[uORF_it].prop_transcripts_incl;
		}

		
		if( /*abs( total_intergenic_reads ) < 30*/ TL_reads_weight == 0 && tss_rel_pos.size() < 3 ) 
		{ 
			uORFs[uORF_it].avg_cap_dist 		 = DEFAULT_DISTANCE_TO_TSS; 
			uORFs[uORF_it].prop_transcripts_incl = DEFAULT_PERCENTAGE;
		}
		
		if( uORFs[uORF_it].prop_transcripts_incl == 0 )
		{
			uORFs[uORF_it].avg_cap_dist = DEFAULT_DISTANCE_TO_TSS; 
		}
		

		if( /*gene_name == "YGR148C"*/ingolia_uORF == TRUE ) { cout /*<< "gene: " */<< gene_name << /*"	uORF " */"	" << uORFs[uORF_it].start_pos  << "	" << TL_reads_component << "	" << TSS_pos_component/*<< "	CDI: " << uORFs[uORF_it].cap_distance_index*/ << /*"	total_intergenic_reads: " */"	" << (double)abs( total_intergenic_reads ) << "	" << tss_rel_pos.size() << /*"	TL reads weight: " */"	"  << uORFs[uORF_it].avg_cap_dist << "	" << uORFs[uORF_it].prop_transcripts_incl << endl; }
		
		// Output all TSS/TL weights
		// if( /*gene_name == "YGR148C"*/ingolia_uORF == TRUE && uORFs[uORF_it].cap_distance_index != -1 /*abs( total_intergenic_reads ) >= 30*/ && proportion_TSS_downstream < .2 ) 
		// { 
			// ostringstream temp_oss;
			// temp_oss /*<< "gene: " */<< gene_name << /*"	uORF " */"	" << uORFs[uORF_it].start_pos  << "	";
			
			// for( int i = 3606; i >= -931; i-- )
			// {
				// temp_oss << TSS_distribution[max_upstream - i] << "	"; 
			// }
			
			// output_text_line( temp_oss.str() );
		// }
		
		//cout << "max: " << max_upstream << "	min: " << min_downstream << endl;
	} 

	avg_TSS_rel_pos = ( uORFs.back().avg_cap_dist != DEFAULT_DISTANCE_TO_TSS ) ? (-1) * uORFs.back().avg_cap_dist : DEFAULT_TSS_REL_POS;
	uORFs.pop_back();
		
	
	return;
}
//------------------------------------------------------------------------------


void TORF_Data::calc_uORF_prob_of_transl( void )
{
	// Note: coefficients garnered from Matlab's 'cftool', with data obtained by running the 'AugCAI_and_CDI_clusters.m' script
	for( unsigned int i = 0; i < uORFs.size(); i++ )
	{
		if( uORFs[i].AUGCAI == DEFAULT_AUGCAI || uORFs[i].cap_distance_index == DEFAULT_CDI ) { uORFs[i].prob_of_translation = DEFAULT_PROBABILITY; }
		else
		{
			if( uORFs[i].AUGCAI < LOW_AUGCAI_CUTOFF )
			{
				uORFs[i].prob_of_translation = .2622 - 1.243 * LOW_AUGCAI_CUTOFF  + .07895 * uORFs[i].cap_distance_index + 2.15 * pow( LOW_AUGCAI_CUTOFF, 2 )  + .7051 * LOW_AUGCAI_CUTOFF  * uORFs[i].cap_distance_index;
			}
			else if( uORFs[i].AUGCAI > HIGH_AUGCAI_CUTOFF )
			{
				uORFs[i].prob_of_translation = .2622 - 1.243 * HIGH_AUGCAI_CUTOFF + .07895 * uORFs[i].cap_distance_index + 2.15 * pow( HIGH_AUGCAI_CUTOFF, 2 ) + .7051 * HIGH_AUGCAI_CUTOFF * uORFs[i].cap_distance_index;
			} 
			else
			{
				uORFs[i].prob_of_translation = .2622 - 1.243 * uORFs[i].AUGCAI    + .07895 * uORFs[i].cap_distance_index + 2.15 * pow( uORFs[i].AUGCAI, 2 )    + .7051 * uORFs[i].AUGCAI    * uORFs[i].cap_distance_index;
			} 
			
			
			if	   ( uORFs[i].prob_of_translation > .94 ) { uORFs[i].prob_of_translation = .94; }
			else if( uORFs[i].prob_of_translation < .1 )  { uORFs[i].prob_of_translation = .1;  }
		}
	}
	
	
	return;
}
//------------------------------------------------------------------------------


unsigned int TORF_Data::find_uORF( const int uORF_rel_pos ) const
{
	unsigned int r_uORF_it = 0;
	
	while( r_uORF_it < uORFs.size() && uORFs[r_uORF_it].rel_uORF_pos != uORF_rel_pos )
	{
		r_uORF_it++;
	}
	
	return r_uORF_it;
}
//------------------------------------------------------------------------------


unsigned int TORF_Data::find_GO_term( const int GO_namespace_it, const string & term, const bool search_parent_terms ) const
{
	unsigned int r_GO_term_it = 0;
	bool GO_term_found = FALSE;
	
	while( r_GO_term_it < GO_Annotations[GO_namespace_it].GO_Terms.size() && GO_term_found == FALSE )
	{
		if( GO_Annotations[GO_namespace_it].GO_Terms[r_GO_term_it].name == term )
		{
			if( search_parent_terms == TRUE || GO_Annotations[GO_namespace_it].GO_Terms[r_GO_term_it].evidence != "" )
			{
				GO_term_found = TRUE;
			}
			else
			{
				r_GO_term_it++;
			}
		}
		else
		{
			r_GO_term_it++;
		}
	}
	
	
	return r_GO_term_it;
}
//------------------------------------------------------------------------------


bool TORF_Data::is_GO_term_present( const int GO_namespace_it, const string & term, const bool search_parent_terms ) const
{
	vector <string> GO_terms_to_search( 0 );
	
	for( unsigned int i = 0; i < GO_Annotations[GO_namespace_it].GO_Terms.size(); i++ )
	{
		if( search_parent_terms == TRUE || GO_Annotations[GO_namespace_it].GO_Terms[i].evidence != "" )
		{
			GO_terms_to_search.push_back( GO_Annotations[GO_namespace_it].GO_Terms[i].name );
		}
	}
	
	return ( ( find( GO_terms_to_search.begin(), GO_terms_to_search.end(), term ) == GO_terms_to_search.end() ) ? FALSE : TRUE );
}
//------------------------------------------------------------------------------



vector <vector <string> > TORF_Data::form_vector_for_csv_rows( const vector <Tenum_uORF_CSV_columns> & col_to_write, const unsigned int select_uORFs ) const
{
	vector <vector <string> > csv_rows( 0, vector <string> ( col_to_write.size(), "" ) );

	unsigned int curr_col_it = 0;
	

	for( unsigned int i = 0; i < uORFs.size(); i++ )
	{
		bool dont_print_because_GO_term = FALSE;
		if( ( PRINT_GO_MATCHING_OR_NOT == GO_TERMS_DONT_MATCH && matches_GO_term == TRUE  ) ||
			( PRINT_GO_MATCHING_OR_NOT == GO_TERMS_MATCH 	  && matches_GO_term == FALSE ) )
		{
			dont_print_because_GO_term = TRUE;
		}
		
		
		if( !( select_uORFs == CANONICAL_uORFS_ONLY 	 &&     uORFs[i].problem == TRUE ) &&
		    !( select_uORFs == EXP_TRANSLATED_uORFs_ONLY && ( /*uORFs[i].problem == TRUE ||*/ uORFs[i].distance_to_tss <= 19 ) ) &&
			!( select_uORFs == ORFS_MATCHING_GO_TERMS 	 && ( /*uORFs[i].problem == TRUE ||*/ /*uORFs[i].distance_to_tss <= 19 ||*/ dont_print_because_GO_term == TRUE ) ) )
		{
			csv_rows.push_back( vector <string> ( col_to_write.size(), "" ) );
			curr_col_it = 0;

			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_CHROM_NUM )
			{
				csv_rows.back()[curr_col_it] = get_str( chrom_num );
				curr_col_it++;
			}
			// ------------------------------

			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_SYST_NAME )
			{
				csv_rows.back()[curr_col_it] = gene_name;		
				curr_col_it++;
			}
			// ------------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_GENE_CONTEXT )
			{
				csv_rows.back()[curr_col_it] = cap_start_codon_only( gene_start_context );
				curr_col_it++;
			}
			// ------------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_AVG_TSS_POS )
			{
				csv_rows.back()[curr_col_it] = ( avg_TSS_rel_pos != DEFAULT_TSS_REL_POS ) ? get_str( avg_TSS_rel_pos ) : "";
				curr_col_it++;
			}
			// ------------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_TSS_REL_POS )
			{
				csv_rows.back()[curr_col_it] = generate_delimited_list( tss_rel_pos, DEFAULT_DELIMITER );				curr_col_it++;
			}
			// ------------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_SINGLE_PEAK_TSS )
			{
				csv_rows.back()[curr_col_it] = ( single_peak_tss_rel_pos != DEFAULT_TSS_REL_POS ) ? get_str( single_peak_tss_rel_pos ) : "";
				curr_col_it++;
			}
			// ------------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_TL_SHAPE_INDEX )
			{
				csv_rows.back()[curr_col_it] = ( TL_shape_index != DEFAULT_SHAPE_INDEX ) ? get_str( TL_shape_index ) : "";
				curr_col_it++;
			}
			// ------------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_GENE_AUGCAI )
			{
				csv_rows.back()[curr_col_it] = format_with_rounding( gene_AUGCAI, 3 );
				curr_col_it++;
			}
			// ------------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_GENE_CHANGE )
			{
				csv_rows.back()[curr_col_it] = get_str( gene_pos_change );
				curr_col_it++;
			}
			// ------------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_CDS_RIB_RPKM )
			{
				csv_rows.back()[curr_col_it] = ( CDS_rib_rpkM != DEFAULT_CDS_RIB_RPKM ) ? get_str( CDS_rib_rpkM ) : "";
				curr_col_it++;
			}
			// ------------------------------
			

			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_CDS_mRNA_RPKM )
			{
				csv_rows.back()[curr_col_it] = ( CDS_mRNA_rpkM != DEFAULT_CDS_mRNA_RPKM ) ? get_str( CDS_mRNA_rpkM ) : "";
				curr_col_it++;
			}
			// ------------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_TRANSL_CORR )
			{
				csv_rows.back()[curr_col_it] = ( transl_corr != DEFAULT_CORRELATION ) ? get_str( transl_corr ) : "";
				curr_col_it++;
			}
			// -----------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_RPM_READS )
			{
				csv_rows.back()[curr_col_it] = ( RPF_reads != DEFAULT_RPF_READS ) ? get_str( RPF_reads ) : "";
				curr_col_it++;
			}
			// -----------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_TRANSL_EFF )
			{
				csv_rows.back()[curr_col_it] = ( transl_eff != DEFAULT_TRANSL_EFF ) ? get_str( transl_eff ) : "";
				curr_col_it++;
			}
			// -----------------------------
			
			
			// ------------------------------
			for( unsigned int col_it = uORF_CSV_COL_NUM_H_UPF1_RATIO; col_it <= uORF_CSV_COL_NUM_H_UPF3_RATIO; col_it++ )
			{
				if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == col_it )
				{
					unsigned int UPF_it = col_to_write[curr_col_it] - uORF_CSV_COL_NUM_H_UPF1_RATIO;
					csv_rows.back()[curr_col_it] = ( He_mRNA_changes[ UPF_it ].ratio != DEFAULT_RATIO ) ? get_str( He_mRNA_changes[ UPF_it ].ratio ) : "";
					curr_col_it++;
				}
			}
			// -----------------------------
			
			
			// ------------------------------
			for( unsigned int col_it = uORF_CSV_COL_NUM_H_UPF1_Q_VAL; col_it <= uORF_CSV_COL_NUM_H_UPF3_Q_VAL; col_it++ )
			{
				if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == col_it )
				{
					unsigned int UPF_it = col_to_write[curr_col_it] - uORF_CSV_COL_NUM_H_UPF1_Q_VAL;
					csv_rows.back()[curr_col_it] = ( He_mRNA_changes[ UPF_it ].q_val != DEFAULT_Q_VAL ) ? get_str( He_mRNA_changes[ UPF_it ].q_val ) : "";
					curr_col_it++;
				}
			}
			// -----------------------------
			
			
			// ------------------------------
			for( unsigned int col_it = uORF_CSV_COL_NUM_L_UPF1_RATIO; col_it <= uORF_CSV_COL_NUM_L_UPF123_RATIO; col_it++ )
			{
				if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == col_it )
				{
					unsigned int UPF_it = col_to_write[curr_col_it] - uORF_CSV_COL_NUM_L_UPF1_RATIO;
					csv_rows.back()[curr_col_it] = ( Lelivelt_mRNA_changes[ UPF_it ].ratio != DEFAULT_RATIO ) ? get_str( Lelivelt_mRNA_changes[ UPF_it ].ratio ) : "";
					curr_col_it++;
				}
			}
			// -----------------------------
			
			
			// ------------------------------
			for( unsigned int col_it = uORF_CSV_COL_NUM_L_UPF1_Q_VAL; col_it <= uORF_CSV_COL_NUM_L_UPF123_Q_VAL; col_it++ )
			{
				if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == col_it )
				{
					unsigned int UPF_it = col_to_write[curr_col_it] - uORF_CSV_COL_NUM_L_UPF1_Q_VAL;
					csv_rows.back()[curr_col_it] = ( Lelivelt_mRNA_changes[ UPF_it ].q_val != DEFAULT_Q_VAL ) ? get_str( Lelivelt_mRNA_changes[ UPF_it ].q_val ) : "";
					curr_col_it++;
				}
			}
			// -----------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_PUB1_ZSCORE )
			{
				csv_rows.back()[curr_col_it] = ( PUB1_binding_zscore != DEFAULT_Z_SCORE ) ? format_with_rounding( PUB1_binding_zscore, 3 ) : "";
				curr_col_it++;
			}
			// ------------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_W_PUB1_HL )
			{
				csv_rows.back()[curr_col_it] = ( w_PUB1_half_life != DEFAULT_HALF_LIFE ) ? format_with_rounding( w_PUB1_half_life, 3 ) : "";
				curr_col_it++;
			}
			// ------------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_WO_PUB1_HL )
			{
				csv_rows.back()[curr_col_it] = ( wo_PUB1_half_life != DEFAULT_HALF_LIFE ) ? format_with_rounding( wo_PUB1_half_life, 3 ) : "";
				curr_col_it++;
			}
			// ------------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_PUB1_STABILITY )
			{
				csv_rows.back()[curr_col_it] = stability_PUB1;
				curr_col_it++;
			}
			// ------------------------------
			
			
			/*
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_NO_NMD_mRNA )
			{
				csv_rows.back()[curr_col_it] = ( no_NMD_mRNA_abundance != DEFAULT_mRNA_CHANGE ) ? ( no_NMD_mRNA_abundance == LOWER ? "Lower" : "Higher" ) : "";
				curr_col_it++;
			}
			// ------------------------------
			*/
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_NMD_TARGET )
			{
				csv_rows.back()[curr_col_it] = ( direct_NMD_target != DEFAULT_NMD_TARGET ) ? ( direct_NMD_target == DIRECT ? "Direct" : "Indirect" ) : "";
				curr_col_it++;
			}
			// ------------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_GUAN_W_NMD_HL )
			{
				csv_rows.back()[curr_col_it] = ( guan_w_NMD_half_life != DEFAULT_HALF_LIFE ) ? format_with_rounding( guan_w_NMD_half_life, 3 ) : "";
				curr_col_it++;
			}
			// ------------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_GUAN_WO_NMD_HL )
			{
				csv_rows.back()[curr_col_it] = ( guan_wo_NMD_half_life != DEFAULT_HALF_LIFE ) ? format_with_rounding( guan_wo_NMD_half_life, 3 ) : "";
				curr_col_it++;
			}
			// ------------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_GUAN_FCR )
			{
				csv_rows.back()[curr_col_it] = ( guan_FCR != DEFAULT_RATIO ) ? format_with_rounding( guan_FCR, 3 ) : "";
				curr_col_it++;
			}
			// ------------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_GUAN_P_VAL )
			{
				csv_rows.back()[curr_col_it] = ( guan_pval != DEFAULT_P_VAL ) ? format_with_rounding( guan_pval, 3 ) : "";
				curr_col_it++;
			}
			// ------------------------------

			
			// ------------------------------
			for( unsigned int col_it = uORF_CSV_COL_NUM_HGN_UPF1_RATIO; col_it <= uORF_CSV_COL_NUM_HGN_PUB1_RATIO; col_it++ )
			{
				if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == col_it )
				{
					unsigned int RBP_it = col_to_write[curr_col_it] - uORF_CSV_COL_NUM_HGN_UPF1_RATIO;
					csv_rows.back()[curr_col_it] = ( Hogan_mRNA_changes[ RBP_it ].ratio != DEFAULT_RATIO ) ? get_str( Hogan_mRNA_changes[ RBP_it ].ratio ) : "";
					curr_col_it++;
				}
			}
			// -----------------------------
			
			
			// ------------------------------
			for( unsigned int col_it = uORF_CSV_COL_NUM_HGN_UPF1_Q_VAL; col_it <= uORF_CSV_COL_NUM_HGN_PUB1_Q_VAL; col_it++ )
			{
				if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == col_it )
				{
					unsigned int RBP_it = col_to_write[curr_col_it] - uORF_CSV_COL_NUM_HGN_UPF1_Q_VAL;
					csv_rows.back()[curr_col_it] = ( Hogan_mRNA_changes[ RBP_it ].q_val != DEFAULT_Q_VAL ) ? get_str( Hogan_mRNA_changes[ RBP_it ].q_val ) : "";
					curr_col_it++;
				}
			}
			// -----------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_JHNS_mRNA_BINDING )
			{
				csv_rows.back()[curr_col_it] = ( Johansson_mRNA_binding_fold_change != DEFAULT_RATIO ) ? format_with_rounding( Johansson_mRNA_binding_fold_change, 3 ) : "";
				curr_col_it++;
			}
			// ------------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_JHNS_mRNA_DECAY )
			{
				csv_rows.back()[curr_col_it] = ( Johansson_mRNA_decay_fold_change != DEFAULT_RATIO ) ? format_with_rounding( Johansson_mRNA_decay_fold_change, 3 ) : "";
				curr_col_it++;
			}
			// ------------------------------

			
			// ------------------------------
		  /*if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_RPF_PERC_CIS )
			{
				csv_rows.back()[curr_col_it] = ( RPF_perc_cis != DEFAULT_PERCENTAGE ) ? get_str( RPF_perc_cis ) : "";
				curr_col_it++;
			}
			// -----------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_mRNA_PERC_CIS )
			{
				csv_rows.back()[curr_col_it] = ( mRNA_perc_cis != DEFAULT_PERCENTAGE ) ? get_str( mRNA_perc_cis ) : "";
				curr_col_it++;
			}
			// -----------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_EFF_PERC_CIS )
			{
				csv_rows.back()[curr_col_it] = ( EFF_perc_cis != DEFAULT_PERCENTAGE ) ? get_str( EFF_perc_cis ) : "";
				curr_col_it++;
			}*/
			// -----------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_GO_TERM_SORTED )
			{
				csv_rows.back()[curr_col_it]  = GO_term_sorted_by;
				curr_col_it++;
			}
			// ------------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_GO_BIO_PROC )
			{
				csv_rows.back()[curr_col_it]  = GO_Annotations[GO_BIO_PROC_IT].format_terms_list();
				curr_col_it++;
			}
			// ------------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_GO_CELL_COMP )
			{
				csv_rows.back()[curr_col_it] = GO_Annotations[GO_CELL_COMP_IT].format_terms_list();
				curr_col_it++;
			}
			// ------------------------------		
				
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_GO_MOL_FUNC )
			{
				csv_rows.back()[curr_col_it] = GO_Annotations[GO_MOL_FUNC_IT].format_terms_list();
				curr_col_it++;
			}
			// ------------------------------	

			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_NUM_uORFs )
			{
				csv_rows.back()[curr_col_it] = get_str( uORFs.size() );
				curr_col_it++;
			}
			// ------------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_SOURCE )
			{
				string temp_source_str = "";
				for( unsigned int source_it = 0; source_it < uORFs[i].source.size(); source_it++ )
				{
					temp_source_str += uORFs[i].source[source_it].get_author();
					temp_source_str += ( ( source_it != uORFs[i].source.size() - 1 ) ? DEFAULT_DELIMITER : "" );
				}
				
				csv_rows.back()[curr_col_it] = temp_source_str;
				curr_col_it++;
			}
			// ------------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_EVIDENCE_TYPE )
			{
				string temp_evidence_str = "";
				for( unsigned int source_it = 0; source_it < uORFs[i].source.size(); source_it++ )
				{
					temp_evidence_str += uORFs[i].source[source_it].get_evidence();
					temp_evidence_str += ( ( source_it != uORFs[i].source.size() - 1 ) ? DEFAULT_DELIMITER : "" );
				}
				
				csv_rows.back()[curr_col_it] = temp_evidence_str;
				curr_col_it++;
			}
			// ------------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_uORF_POS )
			{
				csv_rows.back()[curr_col_it] = get_str( uORFs[i].start_pos );
				curr_col_it++;
			}
			// ------------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_uORF_LEN )
			{
				csv_rows.back()[curr_col_it] = get_str( uORFs[i].len );
				curr_col_it++;
			}
			// ------------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_uORF_REL_POS )
			{
				csv_rows.back()[curr_col_it] = get_str( uORFs[i].rel_uORF_pos );
				curr_col_it++;
			}
			// ------------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_uORF_CAP_DIST )
			{
				csv_rows.back()[curr_col_it] = ( uORFs[i].avg_cap_dist != DEFAULT_DISTANCE_TO_TSS ) ? get_str( uORFs[i].avg_cap_dist ) : "";
				curr_col_it++;
			}
			// ------------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_uORF_IN_TRANSCR )
			{
				csv_rows.back()[curr_col_it] = ( uORFs[i].prop_transcripts_incl != DEFAULT_PERCENTAGE ) ? get_str( uORFs[i].prop_transcripts_incl ) : "";
				curr_col_it++;
			}
			// ------------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_uORF_TO_TSS )
			{
				csv_rows.back()[curr_col_it] = ( uORFs[i].distance_to_tss != DEFAULT_DISTANCE_TO_TSS ) ? get_str( uORFs[i].distance_to_tss ) : "";
				curr_col_it++;
			}
			// ------------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_AUGCAI )
			{
				csv_rows.back()[curr_col_it] = ( uORFs[i].AUGCAI != DEFAULT_AUGCAI ) ? format_with_rounding( uORFs[i].AUGCAI, 3 ) : "";
				curr_col_it++;
			}
			// ------------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_CDI )
			{
				csv_rows.back()[curr_col_it] = ( uORFs[i].cap_distance_index != DEFAULT_CDI ) ? format_with_rounding( uORFs[i].cap_distance_index, 3 ) : "";
				curr_col_it++;
			}
			// ------------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_PROB_OF_TRANSL )
			{
				csv_rows.back()[curr_col_it] = ( uORFs[i].prob_of_translation != DEFAULT_PROBABILITY ) ? format_with_rounding( uORFs[i].prob_of_translation, 3 ) : "";
				curr_col_it++;
			}
			// ------------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_RIBOSOMES )
			{
				csv_rows.back()[curr_col_it] = ( uORFs[i].num_ribosomes != DEFAULT_NUM_RIBOSOMES ) ? get_str( uORFs[i].num_ribosomes ) : "";
				curr_col_it++;
			}
			// ------------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_mRNA )
			{
				csv_rows.back()[curr_col_it] = ( uORFs[i].num_mRNA != DEFAULT_NUM_mRNA ) ? get_str( uORFs[i].num_mRNA ) : "";
				curr_col_it++;
			}
			// ------------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_TRANSL_EFFECT )
			{
				csv_rows.back()[curr_col_it] =  uORFs[i].transl_effic_effect.print_effect_to_str();
				curr_col_it++;
			}
			// ------------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_CLUSTER )
			{
				csv_rows.back()[curr_col_it] =  get_str( uORFs[i].cluster );
				curr_col_it++;
			}
			// ------------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_CLUST_DIST )
			{
				csv_rows.back()[curr_col_it] =  get_str( uORFs[i].distortion );
				curr_col_it++;
			}
			// ------------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_uORF_CONTEXT )
			{
				csv_rows.back()[curr_col_it] = ( uORFs[i].start_context.size() != 0 ) ? cap_start_codon_only( uORFs[i].start_context ) : "";
				curr_col_it++;
			}
			// ------------------------------

			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_PROBLEM )
			{
				csv_rows.back()[curr_col_it] = ( uORFs[i].problem == TRUE ? "yes" : "" );
				curr_col_it++;
			}
			// ------------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_uORF_IN_FRAME )
			{
				csv_rows.back()[curr_col_it] = ( uORFs[i].in_frame == TRUE ? "yes" : "" );
				curr_col_it++;
			}
			// ------------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_START_CODON )
			{
				csv_rows.back()[curr_col_it] = ( uORFs[i].start_codon == TRUE ? "" : "yes" );
				curr_col_it++;
			}
			// ------------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_START_MOVED )
			{
				csv_rows.back()[curr_col_it] = ( uORFs[i].num_nt_start_moved != 0 ) ? get_str( uORFs[i].num_nt_start_moved ) : "";
				curr_col_it++;
			}
			// ------------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_PAST_GENE )
			{
				csv_rows.back()[curr_col_it] = ( uORFs[i].end_past_gene_start == TRUE ? "yes" : "" );
				curr_col_it++;
			}
			// ------------------------------

			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_LEN_CHANGE )
			{
				csv_rows.back()[curr_col_it] = ( uORFs[i].num_nt_len_changed != 0 ) ? get_str( uORFs[i].num_nt_len_changed ) : "";
				curr_col_it++;
			}
			// ------------------------------

			
			/*
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_TSS_REL )
			{
				if	   ( uORFs[i].exp_transl_tss == uORF_DOWNSTREAM_OF_KNOWN_TSS   ) { csv_rows.back()[curr_col_it] =  uORF_DOWNSTREAM_OF_KNOWN_TSS_MSG;   }
				else if( uORFs[i].exp_transl_tss == uORF_UPSTREAM_OF_ALL_KNOWN_TSS ) { csv_rows.back()[curr_col_it] =  uORF_UPSTREAM_OF_ALL_KNOWN_TSS_MSG; }
				else if( uORFs[i].exp_transl_tss == uORF_TOO_CLOSE_TO_TSS	  	   ) { csv_rows.back()[curr_col_it] =  uORF_TOO_CLOSE_TO_TSS_TSS_MSG;      }
				else if( uORFs[i].exp_transl_tss == uORF_INEFFICIENTLY_TRANSLATED  ) { csv_rows.back()[curr_col_it] =  uORF_INEFFICIENTLY_TRANSLATED_MSG;  }
				else if( uORFs[i].exp_transl_tss == uORF_EFFICIENTLY_TRANSLATED    ) { csv_rows.back()[curr_col_it] =  uORF_EFFICIENTLY_TRANSLATED_MSG;    }
				else if( uORFs[i].exp_transl_tss == NO_KNOWN_TSS		 		   ) { csv_rows.back()[curr_col_it] =  NO_KNOWN_TSS_MSG; 				   }
			
				curr_col_it++;
			}
			// ------------------------------
			*/
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_EXT_uORF_CONTEXT )
			{
				csv_rows.back()[curr_col_it] = uORFs[i].ext_start_context;
				curr_col_it++;
			}
			// ------------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_EXT_GENE_CONTEXT )
			{
				csv_rows.back()[curr_col_it] = ext_gene_start_context;
				curr_col_it++;
			}
			// ------------------------------
			
			
			// ------------------------------
			if( curr_col_it < col_to_write.size() && col_to_write[curr_col_it] == uORF_CSV_COL_NUM_uORF )
			{
				csv_rows.back()[curr_col_it] = uORFs[i].content;
				curr_col_it++;
			}
			// ------------------------------
		}
	}
	
	return csv_rows;
}
//------------------------------------------------------------------------------


void TORF_Data::sort_uORFs( void )
{
	sort( uORFs.begin(), uORFs.end(), compare_uORFs(*this) );
	
	return;
}
//------------------------------------------------------------------------------


bool TORF_Data::compare_uORFs::operator()( const TuORF_Data & uORF_1, const TuORF_Data & uORF_2 )
{ 
	if( uORF_1.start_pos < uORF_2.start_pos ) 
	{ 
		if    ( m_ORF_Data.opposite_strand == FALSE )   { return uORF_1_BEFORE_uORF_2; }
		else/*( m_ORF_Data.opposite_strand == TRUE  )*/ { return uORF_2_BEFORE_uORF_1; }
	}
	else if( uORF_1.start_pos > uORF_2.start_pos ) 
	{ 
		if    ( m_ORF_Data.opposite_strand == FALSE )   { return uORF_2_BEFORE_uORF_1; }
		else/*( m_ORF_Data.opposite_strand == TRUE  )*/ { return uORF_1_BEFORE_uORF_2; }
	}
	/*else
	{
		ostringstream output_oss;
		output_oss << "Note: Duplicate uORFs at position " << uORF_1.start_pos << endl;
		Errors.handle_error( NONFATAL, output_oss.str(), __FILE__, __LINE__ );
	}*/
	
	
	// If equivalent, indicate 2 is before 1 (to obey the 'sort()' weak ordering requirement)
	return uORF_2_BEFORE_uORF_1;
}
//------------------------------------------------------------------------------


int TORF_Data::get_gene_start_context( const string & chrom_seq )
{
	if( gene_CDS.start_coord > chrom_seq.size() )
	{ 
		ostringstream error_oss;
		error_oss << "Error in 'TORF_Data::get_gene_start_context()': UTR end position for gene " 
				  << gene_name
				  << " is greater than the length of chromosome " 
				  << chrom_num;
				  
		return Errors.handle_error( PASS_UP_ONE_LEVEL, error_oss.str(), __FILE__, __LINE__ );
	}
	
	
	
	// ------------------------------
	// Extract the biologically-defined start codon context (2 codons upstream, 1 downstream)
	//   Since this is relevant at the translation level, remove introns to obtain start codon context
	unsigned int gene_start_it = gene_CDS.start_coord - 1;
	
	unsigned int start_context_start_it;
	unsigned int start_context_end_it;
	
	vector <TFeature> untransl_reg_interfere;
	
	for( unsigned int i = 0; i < untransl_reg.size(); i++ )
	{
		if( abs( (int)untransl_reg[i].start_coord - (int)gene_CDS.start_coord ) <= START_CONTEXT_NT_AFTER_AUG )
		{
			untransl_reg_interfere.push_back( untransl_reg[i] );
		}
	}
	

	if( untransl_reg_interfere.size() == 0 )
	{	
		if( opposite_strand == FALSE )
		{
			start_context_start_it = gene_start_it - START_CONTEXT_NT_BEFORE_AUG;
		}
		else/*( opposite_strand == TRUE )*/
		{
			start_context_start_it = gene_start_it - START_CONTEXT_NT_AFTER_AUG;	
		}
		
		gene_start_context = chrom_seq.substr( start_context_start_it, START_CONTEXT_LENGTH_NT );
	}
	else/*( untransl_reg_interfere.size() != 0 )*/
	{
		// Extremely unlikely that there will be more than one intron within the first 2 codons of the gene
		if( untransl_reg_interfere.size() > 1 )
		{
			ostringstream error_oss;
			error_oss << "Error in 'TORF_Data::get_gene_start_context()': For gene " << gene_name
					  << ", there were " << untransl_reg_interfere.size()
					  << " introns within the first 2 codons of the gene (at coordinates ";
					  
			for( unsigned int i = 0; i < untransl_reg_interfere.size(); i++ )
			{
				error_oss << untransl_reg[i].start_coord;
				if( i != untransl_reg_interfere.size() - 1 ) { error_oss << ", "; }
				if( i == untransl_reg_interfere.size() - 2 ) { error_oss << "and "; }
			}
			
			error_oss << ").";
					  
			return Errors.handle_error( PASS_UP_ONE_LEVEL, error_oss.str(), __FILE__, __LINE__ );
		}
		
		
		unsigned int untransl_start_it = untransl_reg_interfere[0].start_coord - 1;
		unsigned int untransl_start_within_context_it;
		
		if( opposite_strand == FALSE )
		{
			start_context_start_it = gene_start_it - START_CONTEXT_NT_BEFORE_AUG;

			gene_start_context = chrom_seq.substr( start_context_start_it, ( START_CONTEXT_LENGTH_NT + untransl_reg_interfere[0].length ) );
			
			untransl_start_within_context_it = untransl_start_it - start_context_start_it;
		}
		else/*( opposite_strand == TRUE )*/
		{
			start_context_end_it   = gene_start_it + START_CONTEXT_NT_BEFORE_AUG;
			start_context_start_it = ( start_context_end_it - START_CONTEXT_LENGTH_NT + 1 ) - untransl_reg_interfere[0].length;
			
			gene_start_context = chrom_seq.substr( start_context_start_it, ( START_CONTEXT_LENGTH_NT + untransl_reg_interfere[0].length ) );
			
			untransl_start_within_context_it = ( untransl_start_it - untransl_reg_interfere[0].length + 1 ) - start_context_start_it;
		}
		
		gene_start_context.erase( untransl_start_within_context_it, untransl_reg_interfere[0].length );
	}
	// ------------------------------
	
	
	
	// ------------------------------
	// Extract the extended start codon context.  Since it's not known what level this might be relevant, don't remove introns	
	unsigned int ext_start_context_start_it;
	
	if( opposite_strand == FALSE )
	{
		ext_start_context_start_it = gene_start_it - EXT_START_CONTEXT_NT_BEFORE_AUG;
	}
	else/*( opposite_strand == TRUE )*/
	{
		ext_start_context_start_it = gene_start_it - EXT_START_CONTEXT_NT_AFTER_AUG;	
	}
	
	ext_gene_start_context = chrom_seq.substr( ext_start_context_start_it, EXT_START_CONTEXT_LENGTH_NT );
	// ------------------------------
	
	
	
	// ------------------------------
	// If on the Crick strand, obtain that strand from the known corresponding Watson strand
	if( opposite_strand == TRUE )
	{
		gene_start_context	   = get_inverted_sequence( gene_start_context 	   );
		gene_start_context	   = get_opposite_strand  ( gene_start_context 	   );
		
		ext_gene_start_context = get_inverted_sequence( ext_gene_start_context );
		ext_gene_start_context = get_opposite_strand  ( ext_gene_start_context );
	}
	// ------------------------------
	
	
	
	// ------------------------------
	// Check that the gene's start codon was found.  If so, add the two contexts to the TORF_Data element
	if( gene_start_context.substr( START_CONTEXT_NT_BEFORE_AUG, 3 ) != "ATG" )
	{ 
		gene_start_context     = "";
		ext_gene_start_context = "";
		
		return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'TORF_Data::get_gene_start_context()': Start codon was not found at start of gene  " +
																 gene_name + " (Start Codon Context: " + gene_start_context + "). ", 
																 __FILE__, __LINE__ );
	}
	// ------------------------------

	
	return SUCCESSFUL;
}
//------------------------------------------------------------------------------


TORF_Data::TORF_Data( const unsigned int chr_num, 
					  const string gene, 
					  const unsigned int start, 
					  const unsigned int length, 
					  const unsigned int intergen_start, 
					  const int change, 
					  const bool strand, 
					  const vector <TFeature> untrans,
					  const TFeature fpUTR_in,
					  const double CDS_rib ) :
					  
	gene_name( gene ),
	gene_start_context( "" ),
	ext_gene_start_context( "" ),
	gene_AUGCAI( DEFAULT_AUGCAI ),
	chrom_num( chr_num ),
	gene_CDS( start, length ),
	gene_intergen_start_coord( intergen_start ),
	gene_pos_change( change ),
	opposite_strand( strand ),
	avg_TSS_rel_pos( DEFAULT_TSS_REL_POS ),
	tss_rel_pos( vector <int>( 0 ) ),
	single_peak_tss_rel_pos( DEFAULT_TSS_REL_POS ),
	TL_shape_index( DEFAULT_SHAPE_INDEX ),
	untransl_reg( untrans ),
	fpUTR_intron( fpUTR_in ),
	CDS_rib_rpkM( CDS_rib ),
	CDS_mRNA_rpkM( DEFAULT_CDS_mRNA_RPKM ),
	transl_corr( DEFAULT_CORRELATION ),
	RPF_reads  ( DEFAULT_RPF_READS   ),
	transl_eff ( DEFAULT_TRANSL_EFF  ),
	He_mRNA_changes( 	   vector <TRatio_Stat>( HE_mRNA_DATA_NUM_STRAINS, 		 TRatio_Stat( "", DEFAULT_RATIO, DEFAULT_Q_VAL ) ) ),
	Lelivelt_mRNA_changes( vector <TRatio_Stat>( LELIVELT_mRNA_DATA_NUM_STRAINS, TRatio_Stat( "", DEFAULT_RATIO, DEFAULT_Q_VAL ) ) ),
	PUB1_binding_zscore( DEFAULT_Z_SCORE ),
	stability_PUB1( "" ), 
	w_PUB1_half_life( DEFAULT_HALF_LIFE ), 
	wo_PUB1_half_life( DEFAULT_HALF_LIFE ), 
	//no_NMD_mRNA_abundance( DEFAULT_mRNA_CHANGE ),
	direct_NMD_target( DEFAULT_NMD_TARGET ),
	guan_w_NMD_half_life( DEFAULT_HALF_LIFE ),
	guan_wo_NMD_half_life( DEFAULT_HALF_LIFE ),
	guan_FCR( DEFAULT_RATIO ),
	guan_pval( DEFAULT_P_VAL ),
	Hogan_mRNA_changes( vector <TRatio_Stat>( HOGAN_NUM_RBPs, TRatio_Stat( "", DEFAULT_RATIO, DEFAULT_Q_VAL ) ) ),
	Johansson_mRNA_binding_fold_change( DEFAULT_RATIO ),
	Johansson_mRNA_decay_fold_change( DEFAULT_RATIO ),
	GO_term_to_sort_curr_level( "" ),
	GO_term_sorted_by( "" ),
	most_specific_GO_term( vector <TGO_Term>( NUM_GO_NAMESPACES, TGO_Term( "", "", DUMMY_GO_LEVEL, "" ) ) ),
	matches_GO_term( FALSE )
	
{ 
	GO_Annotations.resize( NUM_GO_NAMESPACES, TGO_Annotation( "" ) );

	GO_Annotations[GO_BIO_PROC_IT]  = TGO_Annotation( GO_BIO_PROC_ID  );
	GO_Annotations[GO_CELL_COMP_IT] = TGO_Annotation( GO_CELL_COMP_ID );
	GO_Annotations[GO_MOL_FUNC_IT]  = TGO_Annotation( GO_MOL_FUNC_ID  );
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

void compile_uORF_list( const TFasta_Content & S_Cerevisiae_Chrom, vector <TORF_Data> * const ORF_Data )
{
	if( LIST_COMPILE_SOURCE == FROM_CURRENT_LIST )
	{
		vector <TGO_Annotation> All_GO_Annotations;
		parse_GO_hierarchy( &All_GO_Annotations );
		parse_uORFs_from_list( ORF_Data, S_Cerevisiae_Chrom.get_gff_annotations(), All_GO_Annotations, INPUT_FILE_NAMING_METHOD );
	}
	else/*( LIST_COMPILE_SOURCE == FROM_DATA_SOURCES )*/
	{
		compile_uORF_list_from_data( S_Cerevisiae_Chrom, ORF_Data );
	}
	
	return;
}	
//==============================================================================



void compile_uORF_list_from_data( const TFasta_Content & S_Cerevisiae_Chrom, vector <TORF_Data> * const ORF_Data )
{
	unsigned int uORF_data_start_it = 0;

	// ------------------------------
	// Extract the uORF list from the Miura data
	TCSV_Contents * uORF_table_Miura = extract_uORF_rows_from_Miura_table();

	if( extract_gene_coord_Miura( *uORF_table_Miura, S_Cerevisiae_Chrom.get_gff_annotations(), ORF_Data, &uORF_data_start_it ) )
	{
		Errors.handle_error( FATAL, "", __FILE__, __LINE__ );
	}

	if( extract_uORFs_Miura( *uORF_table_Miura, S_Cerevisiae_Chrom.sequence, ORF_Data, uORF_data_start_it ) )
	{
		Errors.handle_error( FATAL, "", __FILE__, __LINE__ );
	}

	delete uORF_table_Miura;
	// ------------------------------

	

	// ------------------------------
	// Extract the uORF list from the Ingolia data
	TCSV_Contents * uORF_table_Ingolia = new TCSV_Contents( PATH_DATA_FOLDER, uORF_ALL_FILE_NAME_INGOLIA );
	uORF_table_Ingolia->parse_csv();

	if( extract_gene_coord_Ingolia( *uORF_table_Ingolia, S_Cerevisiae_Chrom.get_gff_annotations(), S_Cerevisiae_Chrom.sequence, ORF_Data, &uORF_data_start_it ) )
	{
		Errors.handle_error( FATAL, "", __FILE__, __LINE__ );
	}

	if( extract_uORFs_Ingolia( *uORF_table_Ingolia, S_Cerevisiae_Chrom.sequence, ORF_Data, uORF_data_start_it ) )
	{
		Errors.handle_error( FATAL, "", __FILE__, __LINE__ );
	}

	delete uORF_table_Ingolia;
	// ------------------------------
	


	// ------------------------------
	// Extract the uORF list from the Zhang & Dietrich Current Genetics data
	TCSV_Contents * uORF_table_Zhang_Dietrich_NAR = new TCSV_Contents( PATH_DATA_FOLDER, uORF_FILE_NAME_ZHANG_DIETRICH_NAR  );
	uORF_table_Zhang_Dietrich_NAR->parse_csv();

	if( extract_gene_coord_Zhang_Dietrich( *uORF_table_Zhang_Dietrich_NAR, S_Cerevisiae_Chrom.get_gff_annotations(), ORF_Data, &uORF_data_start_it ) )
	{
		Errors.handle_error( FATAL, "", __FILE__, __LINE__ );
	}

	if( extract_uORFs_Zhang_Dietrich_NAR( *uORF_table_Zhang_Dietrich_NAR, S_Cerevisiae_Chrom.sequence, ORF_Data, uORF_data_start_it ) )
	{
		Errors.handle_error( FATAL, "", __FILE__, __LINE__ );
	}

	delete uORF_table_Zhang_Dietrich_NAR;
	// ------------------------------



	// ------------------------------
	// Extract the uORF list from the Zhang & Dietrich Nucleic Acids Research data
	TCSV_Contents * uORF_table_Zhang_Dietrich_CG = new TCSV_Contents( PATH_DATA_FOLDER, uORF_FILE_NAME_ZHANG_DIETRICH_CG );
	uORF_table_Zhang_Dietrich_CG->parse_csv();

	if( extract_gene_coord_Zhang_Dietrich( *uORF_table_Zhang_Dietrich_CG, S_Cerevisiae_Chrom.get_gff_annotations(), ORF_Data, &uORF_data_start_it ) )
	{
		Errors.handle_error( FATAL, "", __FILE__, __LINE__ );
	}

	if( extract_uORFs_Zhang_Dietrich_CG( *uORF_table_Zhang_Dietrich_CG, S_Cerevisiae_Chrom.sequence, ORF_Data, uORF_data_start_it ) )
	{
		Errors.handle_error( FATAL, "", __FILE__, __LINE__ );
	}

	delete uORF_table_Zhang_Dietrich_CG;
	// ------------------------------



	// ------------------------------
	// Extract the uORF list from the Nagalakshmi data
	TCSV_Contents * uORF_table_Nagalakshmi = new TCSV_Contents( PATH_DATA_FOLDER, uORF_FILE_NAME_NAGALAKSHMI );
	uORF_table_Nagalakshmi->parse_csv();

	if( extract_gene_coord_Nagalakshmi( *uORF_table_Nagalakshmi, S_Cerevisiae_Chrom.get_gff_annotations(), ORF_Data, &uORF_data_start_it ) )
	{
		Errors.handle_error( FATAL, "", __FILE__, __LINE__ );
	}

	if( extract_uORFs_Nagalakshmi( *uORF_table_Nagalakshmi, S_Cerevisiae_Chrom.sequence, ORF_Data, uORF_data_start_it ) )
	{
		Errors.handle_error( FATAL, "", __FILE__, __LINE__ );
	}

	delete uORF_table_Nagalakshmi;
	// ------------------------------



	// ------------------------------
	// Extract the uORF list from the Cjijovic data
	TCSV_Contents * uORF_table_Cvijovic = new TCSV_Contents( PATH_DATA_FOLDER, uORF_FILE_NAME_CVIJOVIC );
	uORF_table_Cvijovic->parse_csv();

	if( extract_gene_coord_general( *uORF_table_Cvijovic, S_Cerevisiae_Chrom.get_gff_annotations(), ORF_Data, &uORF_data_start_it ) )
	{
		Errors.handle_error( FATAL, "", __FILE__, __LINE__ );
	}

	if( extract_uORFs_Cvijovic( *uORF_table_Cvijovic, S_Cerevisiae_Chrom.sequence, ORF_Data, uORF_data_start_it ) )
	{
		Errors.handle_error( FATAL, "", __FILE__, __LINE__ );
	}

	delete uORF_table_Cvijovic;
	// ------------------------------



	// ------------------------------
	// Extract the uORF list from the Guan data
	TCSV_Contents * uORF_table_Guan = new TCSV_Contents( PATH_DATA_FOLDER, uORF_FILE_NAME_GUAN );
	uORF_table_Guan->parse_csv();

	if( extract_gene_coord_general( *uORF_table_Guan, S_Cerevisiae_Chrom.get_gff_annotations(), ORF_Data, &uORF_data_start_it ) )
	{
		Errors.handle_error( FATAL, "", __FILE__, __LINE__ );
	}

	if( extract_uORFs_Guan( *uORF_table_Guan, S_Cerevisiae_Chrom.sequence, ORF_Data, uORF_data_start_it ) )
	{
		Errors.handle_error( FATAL, "", __FILE__, __LINE__ );
	}

	delete uORF_table_Guan;
	// ------------------------------

	
	
	// ------------------------------
	// Extract the uORF list from the Lawless data
	TCSV_Contents * uORF_table_Lawless = new TCSV_Contents( PATH_DATA_FOLDER, uORF_FILE_NAME_LAWLESS );
	uORF_table_Lawless->parse_csv();

	if( extract_gene_coord_general( *uORF_table_Lawless, S_Cerevisiae_Chrom.get_gff_annotations(), ORF_Data, &uORF_data_start_it ) )
	{
		Errors.handle_error( FATAL, "", __FILE__, __LINE__ );
	}

	if( extract_uORFs_Lawless( *uORF_table_Lawless, S_Cerevisiae_Chrom.sequence, ORF_Data, uORF_data_start_it ) )
	{
		Errors.handle_error( FATAL, "", __FILE__, __LINE__ );
	}

	delete uORF_table_Lawless;
	// ------------------------------
	
	
	
	// ------------------------------
	// Extract the uORF list from the Selpi data
	TCSV_Contents * uORF_table_Selpi = new TCSV_Contents( PATH_DATA_FOLDER, uORF_FILE_NAME_SELPI );
	uORF_table_Selpi->parse_csv();

	if( extract_gene_coord_general( *uORF_table_Selpi, S_Cerevisiae_Chrom.get_gff_annotations(), ORF_Data, &uORF_data_start_it ) )
	{
		Errors.handle_error( FATAL, "", __FILE__, __LINE__ );
	}

	if( extract_uORFs_Selpi( *uORF_table_Selpi, S_Cerevisiae_Chrom.sequence, ORF_Data, uORF_data_start_it ) )
	{
		Errors.handle_error( FATAL, "", __FILE__, __LINE__ );
	}

	delete uORF_table_Selpi;
	// ------------------------------
	

	return;
}
//==============================================================================



void add_GO_info_to_ORFs( vector <TORF_Data> * const ORF_Data, const vector <TGO_Annotation> & All_GO_Annotations )
{
	TCSV_Contents GO_Info_Table( PATH_DATA_FOLDER, GENE_INFO_FILE_NAME );
	GO_Info_Table.parse_csv();


	vector <string> GO_namespace = GO_Info_Table.get_csv_column<string> ( GENE_INFO_GO_NAMESPACE_HEADER );
	vector <string> GO_name    	 = GO_Info_Table.get_csv_column<string> ( GENE_INFO_GO_NAME_HEADER      );
	vector <string> GO_evidence  = GO_Info_Table.get_csv_column<string> ( GENE_INFO_GO_EVIDENCE_HEADER 	);
	vector <string> GO_gene_name = GO_Info_Table.get_csv_column<string> ( GENE_INFO_SYST_NAME_HEADER 	);
	
	unsigned int prev_gene_end_pos = -1;
	unsigned int num_genes_wo_GO_annotations  = 0;
	unsigned int num_genes_not_in_correct_pos = 0;

	for( unsigned int i = 0; i < ORF_Data->size(); i++ )
	{
		// First, attempt to locate the gene in the table
		int curr_gene_start_pos = -1;
		unsigned int search_pos = prev_gene_end_pos + 1;
		bool rollover_occurred = FALSE;
		
		
		if( search_pos >= GO_gene_name.size() ) 
		{ 
			search_pos = 0;
			rollover_occurred = TRUE;
		}
		
		
		do
		{
			if( ORF_Data->at( i ).gene_name == GO_gene_name[search_pos] )
			{
				curr_gene_start_pos = search_pos;
			}
			else
			{
				search_pos++;
				
				if( search_pos >= GO_gene_name.size() ) 
				{ 
					search_pos = 0; 
					rollover_occurred = TRUE;
				}
			}
			
		} while( curr_gene_start_pos == -1 && ( search_pos <= prev_gene_end_pos || rollover_occurred == FALSE ) );
		
		
		if( curr_gene_start_pos == -1 )
		{
			num_genes_wo_GO_annotations++;
		}
		else
		{
			if( (unsigned int)curr_gene_start_pos != prev_gene_end_pos + 1 )
			{
				num_genes_not_in_correct_pos++;
			}
			
			unsigned int curr_gene_end_pos;
			
			do 
			{ 
				search_pos++; 
				
			} while( search_pos < GO_gene_name.size() && ORF_Data->at( i ).gene_name == GO_gene_name[search_pos] );
			
			curr_gene_end_pos = search_pos - 1;
			
			
			for( unsigned int GO_it = curr_gene_start_pos; GO_it <= curr_gene_end_pos; GO_it++ )
			{
				ORF_Data->at( i ).add_GO_Annotation( GO_namespace[GO_it], GO_name[GO_it], GO_evidence[GO_it], All_GO_Annotations );
			}
			
			prev_gene_end_pos = curr_gene_end_pos;
		}
	}

	
	ostringstream output_oss;
	output_oss << "num_genes_wo_GO_annotations:  " << num_genes_wo_GO_annotations  << "\n"
			   << "num_genes_not_in_correct_pos: " << num_genes_not_in_correct_pos << "\n\n";
		
	
	// Print a list of the terms not found
	/*
	output_oss << "list of GO terms not founD: \n";
			   
	for( unsigned int i = 0; i < GO_terms_not_found.size(); i++ )
	{
		output_oss << "    " << i+ 1 << ". " << GO_terms_not_found[i] << "\n";
	}
	
	if( GO_terms_not_found.size() != 0 )
	{
		output_text_line( output_oss.str() );
		Errors.handle_error( FATAL, (string)"Error in 'add_GO_info_to_ORFs()': The GO terms listed above were not found in the hierarchy. ", __FILE__, __LINE__ );
	}
	*/
	
	output_text_line( output_oss.str() );

	
	return;
}
//==============================================================================



void determine_ORF_and_uORF_characteristics( const TFasta_Content & S_Cerevisiae_Chrom, vector <TORF_Data> * const ORF_Data )
{
	if( LIST_COMPILE_SOURCE == FROM_DATA_SOURCES )
	{
		// Extract the start contexts for each of the genes (done after duplicate genes have been removed)
		for( unsigned int i = 0; i < ORF_Data->size(); i++ )
		{
			if( ORF_Data->at( i ).get_gene_start_context( S_Cerevisiae_Chrom.sequence[ ORF_Data->at( i ).chrom_num - 1 ] ) )
			{
				Errors.handle_error( FATAL, "", __FILE__, __LINE__ );
			}
		}


		for( unsigned int i = 0; i < ORF_Data->size(); i++ )
		{
			ORF_Data->at( i ).calculate_AUGCAI_values();
		}
		
		if( get_uORF_effects_on_gene_translation( ORF_Data ) )
		{
			Errors.handle_error( FATAL, "", __FILE__, __LINE__ );
		}
		
		get_Brar_translation_data( ORF_Data );
		
		#ifndef ADD_COL_TO_MCMANUS_TRANSL_DATA
			get_McManus_translation_data( ORF_Data );
		#endif
		
		
		get_Miura_TSS_data		     ( ORF_Data );
		get_Zhang_Dietrich_TSS_data  ( ORF_Data );
		get_Xu_TSS_data			     ( ORF_Data );
		get_Yassour_TSS_data	     ( ORF_Data );
		get_Nagalakshmi_TSS_data     ( ORF_Data );
		get_David_TSS_data   	     ( ORF_Data );
                                     
		                             
		get_Arribere_TL_data   	     ( ORF_Data );
		get_Arribere_SI_data   	     ( ORF_Data );
		                             
		get_He_mRNA_change_data      ( ORF_Data );
		get_Lelivelt_mRNA_change_data( ORF_Data );
		
		get_Duttagupta_PUB1_binding_data		( ORF_Data );
		get_Duttagupta_PUB1_effects_data		( ORF_Data );
		get_Duttagupta_PUB1_mRNA_half_lives_data( ORF_Data );
		get_Guan_NMD_sensitive_transcripts_data	( ORF_Data );
	  //get_Guan_uORFs_in_NMD_path_data			( ORF_Data );
		get_Hogan_PUB1_data						( ORF_Data );
		get_Hogan_UPF1_data						( ORF_Data );
		get_Johansson_mRNA_binding_data			( ORF_Data );
		get_Johansson_mRNA_decay_data			( ORF_Data );

		
		get_Arribere_Cap_Distance_index( ORF_Data );
		
		unsigned int tss_sum = 0;
		unsigned int num_tss = 0;
		
		for( unsigned int i = 0; i < ORF_Data->size(); i++ )
		{
			ORF_Data->at( i ).calc_uORF_prob_of_transl();
			ORF_Data->at( i ).sort_tss();
			ORF_Data->at( i ).check_uORF_upstream_of_tss();
			
			for( unsigned int j = 0; j < ORF_Data->at( i ).tss_rel_pos.size(); j++ )
			{
				tss_sum -= ORF_Data->at( i ).tss_rel_pos[j];
				num_tss++;
			}
		}
		
		/*
		output_text_line( (string)"Average TSS rel pos: "  + get_str( (double)tss_sum / num_tss ) );
		output_text_line( (string)"Number TSS's:        "  + get_str( num_tss 					) );
		output_text_line( "" );
		*/
		
		
		// Cluster the uORFs based on kmer vectors for uORF context (-50 to +50 relative to start codon)
		calc_mod_AMI_uORF_context( S_Cerevisiae_Chrom, ORF_Data );
		cluster_uORF_context_profiles( ORF_Data );
	}

	return;
}
//==============================================================================



int get_uORF_effects_on_gene_translation( vector <TORF_Data> * const ORF_Data )
{
	TCSV_Contents uORF_transl_effects( PATH_DATA_FOLDER, uORFs_TRANSL_EFFIC_FILE_NAME );
	uORF_transl_effects.parse_csv();
	
	
	vector <string> gene_name    = uORF_transl_effects.get_csv_column<string>( uORF_TRANSL_SYST_NAME_HEADER    );
	vector <string> effect	     = uORF_transl_effects.get_csv_column<string>( uORF_TRANSL_EFFECT_HEADER 	   );
	vector <string> source	     = uORF_transl_effects.get_csv_column<string>( uORF_TRANSL_SOURCE_HEADER 	   );
	vector <string> hypothesized = uORF_transl_effects.get_csv_column<string>( uORF_TRANSL_HYPOTHESIZED_HEADER );
	vector <int> 	uORF_rel_pos = uORF_transl_effects.get_csv_column<int>	 ( uORF_TRANSL_uORF_REL_POS_HEADER );
	
	
	for( unsigned int i = 0; i < gene_name.size(); i++ )
	{
		unsigned int ORF_it = find_ORF( gene_name[i], *ORF_Data );
		
		if( ORF_it != ORF_Data->size() )
		{
			if( ORF_Data->at( ORF_it ).add_uORF_transl_effect( effect[i], source[i], ( hypothesized[i] == "no" ? FALSE : TRUE ), uORF_rel_pos[i] ) )
			{
				Errors.handle_error( FATAL, "Error in 'get_uORF_effects_on_gene_translation()': ", __FILE__, __LINE__ );
			}
		}
	}
	
	return SUCCESSFUL;
}
//==============================================================================



void get_Miura_TSS_data( vector <TORF_Data> * const ORF_Data )
{
	TCSV_Contents Miura_TSS_Data( PATH_DATA_FOLDER, MIURA_TSS_DATA_FILE_NAME );
	Miura_TSS_Data.parse_csv();
	
	
	vector <string> 	  gene_name = Miura_TSS_Data.get_csv_column<string>		 ( MIURA_TSS_SYST_NAME_HEADER );
	vector <unsigned int> hit_start = Miura_TSS_Data.get_csv_column<unsigned int>( MIURA_TSS_HIT_START_HEADER );
	vector <unsigned int> hit_end   = Miura_TSS_Data.get_csv_column<unsigned int>( MIURA_TSS_HIT_END_HEADER   );
	
	vector <unsigned int> tss_coord( hit_start.size(), 0 );
	
	for( unsigned int i = 0; i < gene_name.size(); i++ )
	{
		tss_coord[i] = ( gene_name[i].find( 'W' ) != string::npos ) ? hit_start[i] : hit_end[i];
	}
	

	if( add_TSS_list( tss_coord, gene_name, ORF_Data ) )
	{
		Errors.handle_error( FATAL, "Error in 'get_Miura_TSS_data()': ", __FILE__, __LINE__ );
	}
			
	
	return;
}
//==============================================================================



void get_Zhang_Dietrich_TSS_data( vector <TORF_Data> * const ORF_Data )
{
	// ------------------------------
	// Read in the GFF file containing the Zhang and Dietrich TSS annotations used in SGD's GBrowse
	string Zhang_Dietrich_TSS_info;
	if( read_entire_file_contents( PATH_DATA_FOLDER, ZHANG_DIETRICH_TSS_DATA_FILE_NAME, &Zhang_Dietrich_TSS_info ) )
	{
		Errors.handle_error( FATAL, "Error reading Zhang and Dietrich TSS file contents. ", __FILE__, __LINE__ );
	}
	
	string Zhang_Dietrich_TSS_info_file_body = Zhang_Dietrich_TSS_info.substr( Zhang_Dietrich_TSS_info.find( "\nchrI" ) );
	// ------------------------------
	

	
	// ------------------------------
	// Extract the needed information (chromosome number, clone ID, and hit start/end)
	vector <string> tss_coord_str = extract_fields( Zhang_Dietrich_TSS_info_file_body, "TSS\t", 			 "\t" 			);
	vector <string> tss_freq_str  = extract_fields( Zhang_Dietrich_TSS_info_file_body, ";frequency=", 	 ";associated_ORF=" );
	vector <string> gene_name     = extract_fields( Zhang_Dietrich_TSS_info_file_body, ";associated_ORF=", "\n" 			);
	
	if( tss_coord_str.size() != gene_name.size() || tss_coord_str.size() != gene_name.size() )
	{
		Errors.handle_error( FATAL, (string)"Error in 'get_Zhang_Dietrich_TSS_data()': Size of data vectors obtained from '" + 
											ZHANG_DIETRICH_TSS_DATA_FILE_NAME + "' don't match. ", __FILE__, __LINE__ );
	}
	
	
	vector <unsigned int> tss_coord( tss_coord_str.size(), 0 );
	vector <unsigned int> tss_freq ( tss_freq_str.size(),  0 );
	
	for( unsigned int i = 0; i < tss_coord.size(); i++ )
	{
		tss_coord[i] = atoi( tss_coord_str[i].c_str() );
		tss_freq[i]  = atoi( tss_freq_str[i].c_str()  );
	}
	// ------------------------------
	
	
	
	// ------------------------------
	// Add the TSS coordinates to the ORF Data set
	if( add_TSS_list( tss_coord, gene_name, ORF_Data ) )
	{
		Errors.handle_error( FATAL, "Error in 'get_Zhang_Dietrich_TSS_data()': ", __FILE__, __LINE__ );
	}
	// ------------------------------
	
	
	return;
}
//==============================================================================



void get_Xu_TSS_data( vector <TORF_Data> * const ORF_Data )
{
	// ------------------------------
	// Read in the GFF file containing the Zhang and Dietrich TSS annotations used in SGD's GBrowse
	string Xu_TSS_info;
	if( read_entire_file_contents( PATH_DATA_FOLDER, XU_TSS_DATA_FILE_NAME, &Xu_TSS_info ) )
	{
		Errors.handle_error( FATAL, "Error reading Xu TSS file contents. ", __FILE__, __LINE__ );
	}
	
	string Xu_TSS_info_file_body = Xu_TSS_info.substr( Xu_TSS_info.find( "\nchrI" ) );
	// ------------------------------
	

	
	// ------------------------------
	// Extract the needed information (chromosome number, clone ID, and hit start/end)
	vector <string> transcr_first_coord_str = extract_fields( Xu_TSS_info_file_body, "Xu_2009_ORFs\ttranscript\t", "\t"  			  );
	vector <string> transcr_last_coord_str  = extract_fields( Xu_TSS_info_file_body, "Xu_2009_ORFs\ttranscript\t", "\t." 			  );
	vector <string> gene_name     			= extract_fields( Xu_TSS_info_file_body, ";Name=", 					   ";end_confidence=" );
	
	
	if( transcr_first_coord_str.size() != gene_name.size() || transcr_last_coord_str.size() != gene_name.size() )
	{
		Errors.handle_error( FATAL, (string)"Error in 'get_Xu_TSS_data()': Size of data vectors obtained from '" + 
											XU_TSS_DATA_FILE_NAME + "' don't match. ", __FILE__, __LINE__ );
	}
	
	
	vector <unsigned int> tss_coord( transcr_first_coord_str.size(), 0 );
	
	for( unsigned int i = 0; i < gene_name.size(); i++ )
	{
		unsigned int transcr_first_coord  = atoi( transcr_first_coord_str[i].c_str() );
		
		unsigned int last_coord_start_pos = transcr_last_coord_str[i].find( '\t' ) + 1; 
		unsigned int transcr_last_coord   = atoi( transcr_last_coord_str[i].substr( last_coord_start_pos ).c_str() );
	
		tss_coord[i] = ( gene_name[i].find( 'W' ) != string::npos ) ? transcr_first_coord : transcr_last_coord;
	}
	// ------------------------------
	
	
	
	// ------------------------------
	// Add the TSS coordinates to the ORF Data set
	if( add_TSS_list( tss_coord, gene_name, ORF_Data ) )
	{
		Errors.handle_error( FATAL, "Error in 'get_Xu_TSS_data()': ", __FILE__, __LINE__ );
	}
	// ------------------------------
	
	
	return;
}
//==============================================================================



void get_Yassour_TSS_data( vector <TORF_Data> * const ORF_Data )
{
	// ------------------------------
	// Read in the GFF file containing the Zhang and Dietrich TSS annotations used in SGD's GBrowse
	string Yassour_TSS_info;
	if( read_entire_file_contents( PATH_DATA_FOLDER, YASSOUR_TSS_DATA_FILE_NAME, &Yassour_TSS_info ) )
	{
		Errors.handle_error( FATAL, "Error reading Yassour TSS file contents. ", __FILE__, __LINE__ );
	}
	
	string Yassour_TSS_info_file_body = Yassour_TSS_info.substr( Yassour_TSS_info.find( "\nchrI" ) );
	// ------------------------------
	

	
	// ------------------------------
	// Extract the needed information (chromosome number, clone ID, and hit start/end)
	vector <string> transcr_first_coord_str = extract_fields( Yassour_TSS_info_file_body, "Yassour_2009\tfive_prime_UTR\t", "\t" 		  );
	vector <string> transcr_last_coord_str  = extract_fields( Yassour_TSS_info_file_body, "Yassour_2009\tfive_prime_UTR\t", "\t." 		  );
	vector <string> gene_name     			= extract_fields( Yassour_TSS_info_file_body, ";Name=", 					   "_5UTR;Alias=" );
	

	if( transcr_first_coord_str.size() != gene_name.size() || transcr_last_coord_str.size() != gene_name.size() )
	{
		Errors.handle_error( FATAL, (string)"Error in 'get_Yassour_TSS_data()': Size of data vectors obtained from '" + 
											YASSOUR_TSS_DATA_FILE_NAME + "' don't match. ", __FILE__, __LINE__ );
	}
	
	
	vector <unsigned int> tss_coord( transcr_first_coord_str.size(), 0 );
	
	for( unsigned int i = 0; i < gene_name.size(); i++ )
	{
		unsigned int transcr_first_coord  = atoi( transcr_first_coord_str[i].c_str() );
		
		unsigned int last_coord_start_pos = transcr_last_coord_str[i].find( '\t' ) + 1; 
		unsigned int transcr_last_coord   = atoi( transcr_last_coord_str[i].substr( last_coord_start_pos ).c_str() );
	
		tss_coord[i] = ( gene_name[i].find( 'W' ) != string::npos ) ? transcr_first_coord : transcr_last_coord;
	}
	// ------------------------------
	
	
	
	// ------------------------------
	// Add the TSS coordinates to the ORF Data set
	if( add_TSS_list( tss_coord, gene_name, ORF_Data ) )
	{
		Errors.handle_error( FATAL, "Error in 'get_Yassour_TSS_data()': ", __FILE__, __LINE__ );
	}
	// ------------------------------
	
	
	return;
}
//==============================================================================



void get_Nagalakshmi_TSS_data( vector <TORF_Data> * const ORF_Data )
{
	// ------------------------------
	// Read in the GFF file containing the Zhang and Dietrich TSS annotations used in SGD's GBrowse
	string Nagalakshmi_TSS_info;
	if( read_entire_file_contents( PATH_DATA_FOLDER, NAGALAKSHMI_TSS_DATA_FILE_NAME, &Nagalakshmi_TSS_info ) )
	{
		Errors.handle_error( FATAL, "Error reading Nagalakshmi TSS file contents. ", __FILE__, __LINE__ );
	}
	
	string Nagalakshmi_TSS_info_file_body = Nagalakshmi_TSS_info.substr( Nagalakshmi_TSS_info.find( "\nchrI" ) );
	// ------------------------------
	

	
	// ------------------------------
	// Extract the needed information (chromosome number, clone ID, and hit start/end)
	vector <string> transcr_first_coord_str = extract_fields( Nagalakshmi_TSS_info_file_body, "Nagalakshmi_2008\tfive_prime_UTR\t", "\t" 		  				  	 );
	vector <string> transcr_last_coord_str  = extract_fields( Nagalakshmi_TSS_info_file_body, "Nagalakshmi_2008\tfive_prime_UTR\t", "\t." 		  				   	 );
	vector <string> gene_name     			= extract_fields( Nagalakshmi_TSS_info_file_body, ";Name=", 					   	   "_5UTR;log2_transcription_level=" );
	
	
	if( transcr_first_coord_str.size() != gene_name.size() || transcr_last_coord_str.size() != gene_name.size() )
	{
		Errors.handle_error( FATAL, (string)"Error in 'get_Nagalakshmi_TSS_data()': Size of data vectors obtained from '" +
											NAGALAKSHMI_TSS_DATA_FILE_NAME + "' don't match. ", __FILE__, __LINE__ );
	}

	
	vector <unsigned int> tss_coord( transcr_first_coord_str.size(), 0 );
	
	for( unsigned int i = 0; i < gene_name.size(); i++ )
	{
		unsigned int transcr_first_coord  = atoi( transcr_first_coord_str[i].c_str() );
		
		unsigned int last_coord_start_pos = transcr_last_coord_str[i].find( '\t' ) + 1; 
		unsigned int transcr_last_coord   = atoi( transcr_last_coord_str[i].substr( last_coord_start_pos ).c_str() );
	
		tss_coord[i] = ( gene_name[i].find( 'W' ) != string::npos ) ? transcr_first_coord : transcr_last_coord;
	}
	// ------------------------------
	
	
	
	// ------------------------------
	// Add the TSS coordinates to the ORF Data set
	if( add_TSS_list( tss_coord, gene_name, ORF_Data ) )
	{
		Errors.handle_error( FATAL, "Error in 'get_Nagalakshmi_TSS_data()': ", __FILE__, __LINE__ );
	}
	// ------------------------------
	
	
	return;
}
//==============================================================================



void get_David_TSS_data( vector <TORF_Data> * const ORF_Data )
{
	// ------------------------------
	// Read in the GFF file containing the Zhang and Dietrich TSS annotations used in SGD's GBrowse
	string David_TSS_info;
	if( read_entire_file_contents( PATH_DATA_FOLDER, DAVID_TSS_DATA_FILE_NAME, &David_TSS_info ) )
	{
		Errors.handle_error( FATAL, "Error reading David TSS file contents. ", __FILE__, __LINE__ );
	}
	
	string David_TSS_info_file_body = David_TSS_info.substr( David_TSS_info.find( "\nchrI" ) );
	// ------------------------------
	

	
	// ------------------------------
	// Extract the needed information (chromosome number, clone ID, and hit start/end)
	vector <string> transcr_first_coord_str = extract_fields( David_TSS_info_file_body, "David_2006_total\ttranscript\t", "\t" 	 										   );
	vector <string> transcr_last_coord_str  = extract_fields( David_TSS_info_file_body, "David_2006_total\ttranscript\t", "\t.\t" 										   );
	vector <string> strand_str  			= extract_fields( David_TSS_info_file_body, "David_2006_total\ttranscript\t", "\t.\t" 										   );
	vector <string> UTR_len_str  			= extract_fields( David_TSS_info_file_body, ";utr5=", 						 ";utr3=", 		   "David_2006_total\ttranscript\t" );
	vector <string> overlapping_features    = extract_fields( David_TSS_info_file_body, ";overlapFeatAll=", 			 ";oppositeExpr=", "David_2006_total\ttranscript\t" );
	
	
	if( transcr_first_coord_str.size() != overlapping_features.size() || 
		transcr_last_coord_str.size()  != overlapping_features.size() || 
		strand_str.size()  			   != overlapping_features.size() || 
		UTR_len_str.size() 			   != overlapping_features.size() )
	{
		Errors.handle_error( FATAL, (string)"Error in 'get_David_TSS_data()': Size of data vectors obtained from '" +
											DAVID_TSS_DATA_FILE_NAME + "' don't match. ", __FILE__, __LINE__ );
	}
	

	vector <unsigned int> tss_coord( 0 );
	vector <string> 	  gene_name( 0 );
	

	for( unsigned int i = 0; i < UTR_len_str.size(); i++ )
	{
		if( UTR_len_str[i] != "N/A" )
		{	
			char strand_indicator = 'W';
			if( strand_str[i][ strand_str[i].size() - 1 ] == '-' ) { strand_indicator = 'C'; }
			
			
			vector <string> feature_list = parse_delimited_list<string>( overlapping_features[i], DEFAULT_DELIMITER );
			
			unsigned int assigned_feature_it = feature_list.size();
			for( unsigned int j = 0; j < feature_list.size(); j++ )
			{
				if( STRAND_INDICATOR_POS < feature_list[j].size() && feature_list[j][STRAND_INDICATOR_POS] == strand_indicator )
				{
					if( assigned_feature_it != feature_list.size() )
					{
						Errors.handle_error( FATAL, (string)"Error in 'get_David_TSS_data()': Found multiple overlapping features on the correct strand (overlapping features: " + 
															overlapping_features[i] + "). ", __FILE__, __LINE__ );
					}
					
					assigned_feature_it = j;
				}
			}
			
			if( assigned_feature_it == feature_list.size() )
			{
				Errors.handle_error( FATAL, "Error in 'get_David_TSS_data()': Could not find an overlapping feature on the correct strand. ", __FILE__, __LINE__ );
			}
			
			gene_name.push_back( feature_list[ assigned_feature_it ] );
			
			
			unsigned int transcr_first_coord  = atoi( transcr_first_coord_str[i].c_str() );
			
			unsigned int last_coord_start_pos = transcr_last_coord_str[i].find( '\t' ) + 1; 
			unsigned int last_coord_end_pos   = find_last_digit_or_char( DIGIT, transcr_last_coord_str[i], last_coord_start_pos );
			unsigned int transcr_last_coord   = atoi( transcr_last_coord_str[i].substr( last_coord_start_pos, ( last_coord_end_pos - last_coord_start_pos + 1 ) ).c_str() );
		
			tss_coord.push_back( ( strand_indicator == 'W' ) ? transcr_first_coord : transcr_last_coord );
		}
	}
	// ------------------------------
	
	
	
	// ------------------------------
	// Add the TSS coordinates to the ORF Data set
	if( add_TSS_list( tss_coord, gene_name, ORF_Data ) )
	{
		Errors.handle_error( FATAL, "Error in 'get_David_TSS_data()': ", __FILE__, __LINE__ );
	}
	// ------------------------------
	
	
	return;
}
//==============================================================================



int add_TSS_list( const vector <unsigned int> & tss_coord_list, const vector <string> & gene_name_list, vector <TORF_Data> * const ORF_Data )
{
	if( gene_name_list.size() != tss_coord_list.size() )
	{
		return Errors.handle_error( PASS_UP_ONE_LEVEL, "Error in 'add_TSS_list()': Size of list of TSS coordinates did not match size of list of genes. ", __FILE__, __LINE__ );
	}
	
	
	// Add the TSS coordinates to the ORF Data set
	for( unsigned int i = 0; i < gene_name_list.size(); i++ )
	{
		unsigned int ORF_it = find_ORF( gene_name_list[i], *ORF_Data );
		
		if( ORF_it != ORF_Data->size() )
		{
			int tss_rel_pos = 0;
			
			if( ORF_Data->at( ORF_it ).opposite_strand == FALSE )
			{
				if( tss_coord_list[i] < ORF_Data->at( ORF_it ).gene_CDS.start_coord )
				{
					tss_rel_pos = (int)tss_coord_list[i] - (int)ORF_Data->at( ORF_it ).gene_CDS.start_coord;
				}
			}
			else/*( ORF_Data->at( ORF_it ).opposite_strand == TRUE )*/
			{
				if( tss_coord_list[i] > ORF_Data->at( ORF_it ).gene_CDS.start_coord )
				{
					tss_rel_pos = (int)ORF_Data->at( ORF_it ).gene_CDS.start_coord - (int)tss_coord_list[i];
				}
			}
			
			
			if( tss_rel_pos != 0 )
			{
				ORF_Data->at( ORF_it ).add_tss( tss_rel_pos );
			}
		}
	}
	
	return SUCCESSFUL;
}
//==============================================================================



void get_Brar_translation_data( vector <TORF_Data> * const ORF_Data )
{
	unsigned int num_uORFs_found_none_indicated = 0;
	unsigned int num_no_uORFs_found_some_indicated = 0;
	
	// ------------------------------
	// Read in the CSV file containing the Brar translation data and extract the relevant columns
	TCSV_Contents Brar_Translation_Info( PATH_DATA_FOLDER, BRAR_TRANSL_DATA_FILE_NAME );
	Brar_Translation_Info.parse_csv();
	
	vector <string> gene_name 	  = Brar_Translation_Info.get_csv_column<string>( BRAR_TRANSL_SYST_NAME_HEADER    );
	vector <double> correlation   = Brar_Translation_Info.get_csv_column<double>( BRAR_TRANSL_CORRELATION_HEADER  );
	vector <string> uORFs_present = Brar_Translation_Info.get_csv_column<string>( BRAR_TRANSL_uORFs_PRESET_HEADER );
	// ------------------------------
	
	
	
	// ------------------------------
	// Add the translation data for each uORF/ORF to the uORF list
	for( unsigned int i = 0; i < gene_name.size(); i++ )
	{
		unsigned int ORF_it = find_ORF( gene_name[i], *ORF_Data );
		
		if( ORF_it == ORF_Data->size() )
		{
			if( uORFs_present[i] == "Y" )
			{
				num_no_uORFs_found_some_indicated++;
			}
		}
		else/*( ORF_it != ORF_Data->size() )*/
		{
			ORF_Data->at( ORF_it ).transl_corr = correlation[i];
			
			if( uORFs_present[i] != "Y" )
			{
				num_uORFs_found_none_indicated++;
			}
		}
	}
	// ------------------------------
	
	
	
	// ------------------------------
	// Output the diagnostic measurements
	output_text_line( (string)"Num times uORFs found when none were indicated (Brar):     " + get_str( num_uORFs_found_none_indicated    ) );
	output_text_line( (string)"Num times uORFs not found when some were indicated (Brar): " + get_str( num_no_uORFs_found_some_indicated ) );
	output_text_line( "" );
	// ------------------------------
	
	
	return;
}
//==============================================================================



void get_McManus_translation_data( vector <TORF_Data> * const ORF_Data )
{
	// ------------------------------
	// Read in the CSV file containing the Brar translation data and extract the relevant columns
	TCSV_Contents McManus_Translation_Info( PATH_DATA_FOLDER, McMANUS_TRANSL_DATA_FILE_NAME );
	McManus_Translation_Info.parse_csv();
	
	vector <string> gene_name 			 = McManus_Translation_Info.get_csv_column<string>( McMANUS_TRANSL_SYST_NAME_HEADER, 	 McMANUS_TRANSL_HEADER_ROW );
	vector <string> present_in_uORF_list = McManus_Translation_Info.get_csv_column<string>( McMANUS_TRANSL_IN_uORF_LIST_HEADER,	 McMANUS_TRANSL_HEADER_ROW );
    vector <double> RPF_A 				 = McManus_Translation_Info.get_csv_column<double>( McMANUS_TRANSL_RPF_REP_A_HEADER,	 McMANUS_TRANSL_HEADER_ROW );
	vector <double> RPF_B 				 = McManus_Translation_Info.get_csv_column<double>( McMANUS_TRANSL_RPF_REP_B_HEADER,	 McMANUS_TRANSL_HEADER_ROW );
    vector <double> mRNA_A 				 = McManus_Translation_Info.get_csv_column<double>( McMANUS_TRANSL_mRNA_REP_A_HEADER,	 McMANUS_TRANSL_HEADER_ROW );
	vector <double> mRNA_B 				 = McManus_Translation_Info.get_csv_column<double>( McMANUS_TRANSL_mRNA_REP_B_HEADER,	 McMANUS_TRANSL_HEADER_ROW );
 // vector <double> RPF_perc_cis 		 = McManus_Translation_Info.get_csv_column<double>( McMANUS_TRANSL_PERC_CIS_RPF_HEADER,	 McMANUS_TRANSL_HEADER_ROW );
 // vector <double> mRNA_perc_cis 		 = McManus_Translation_Info.get_csv_column<double>( McMANUS_TRANSL_PERC_CIS_mRNA_HEADER, McMANUS_TRANSL_HEADER_ROW );
 // vector <double> transl_eff_perc_cis  = McManus_Translation_Info.get_csv_column<double>( McMANUS_TRANSL_PERC_CIS_EFF_HEADER,	 McMANUS_TRANSL_HEADER_ROW );
	// ------------------------------
	
	
	
	// ------------------------------
	// Add the translation data for each uORF/ORF to the uORF list
	for( unsigned int i = 0; i < gene_name.size(); i++ )
	{
		if( present_in_uORF_list[i] == "yes" )
		{
			unsigned int ORF_it = find_ORF( gene_name[i], *ORF_Data );
			
			if( ORF_it == ORF_Data->size() )
			{
				Errors.handle_error( FATAL, "Error in 'get_McManus_translation_data()': Unable to find gene '" + gene_name[i] + "' in the uORF list. ", __FILE__, __LINE__ );
			}
			
			
		    double transl_eff_A = RPF_A[i] / mRNA_A[i];
			double transl_eff_B = RPF_B[i] / mRNA_B[i];
			
			double RPF_avg		  = ( RPF_A[i] 	   + RPF_B[i] 	  ) / 2;
			double transl_eff_avg = ( transl_eff_A + transl_eff_B ) / 2;
			
			
			ORF_Data->at( ORF_it ).RPF_reads 		   = RPF_B[i];
			ORF_Data->at( ORF_it ).transl_eff 		   = /*transl_eff_B*/ transl_eff_avg;
		 // ORF_Data->at( ORF_it ).RPF_perc_cis 	   = RPF_perc_cis[i];
		 // ORF_Data->at( ORF_it ).mRNA_perc_cis 	   = mRNA_perc_cis[i];
		 // ORF_Data->at( ORF_it ).transl_eff_perc_cis = transl_eff_perc_cis[i];
		}
	}
	// ------------------------------
	
	
	return;
}
//==============================================================================



void add_present_in_uORF_list_col_to_McManus_transl_data( const vector <TORF_Data> & ORF_Data )
{
	// ------------------------------
	// Read in the CSV file containing the Brar translation data and extract the relevant columns
	TCSV_Contents McManus_Translation_Info( PATH_DATA_FOLDER, McMANUS_TRANSL_DATA_FILE_NAME );
	McManus_Translation_Info.parse_csv();
	
	vector <string> gene_name = McManus_Translation_Info.get_csv_column<string>( McMANUS_TRANSL_SYST_NAME_HEADER, McMANUS_TRANSL_HEADER_ROW );
	// ------------------------------
	
	
	
	// ------------------------------
	// Assemble and add the column (as a vector)
	vector <string> present_in_uORF_list_col( McMANUS_TRANSL_HEADER_ROW + gene_name.size(), "" );
	
	present_in_uORF_list_col[McMANUS_TRANSL_HEADER_ROW - 1] = McMANUS_TRANSL_IN_uORF_LIST_HEADER;
	
	for( unsigned int i = 0; i < gene_name.size(); i++ )
	{
		if( find_ORF( gene_name[i], ORF_Data ) != ORF_Data.size() )
		{
			present_in_uORF_list_col[McMANUS_TRANSL_HEADER_ROW + i] = "yes";
		}
	}
	
	if( McManus_Translation_Info.does_column_exist( McMANUS_TRANSL_IN_uORF_LIST_HEADER ) == FALSE )
	{
		McManus_Translation_Info.add_column( McManus_Translation_Info.get_col_num( McMANUS_TRANSL_SYST_NAME_HEADER, McMANUS_TRANSL_HEADER_ROW ) + 1, present_in_uORF_list_col );
	}
	else
	{
		McManus_Translation_Info.replace_column( McManus_Translation_Info.get_col_num( McMANUS_TRANSL_IN_uORF_LIST_HEADER, McMANUS_TRANSL_HEADER_ROW ), present_in_uORF_list_col );
	}
	
	McManus_Translation_Info.write_back_to_csv();
	// ------------------------------
	
	
	return;
}
//==============================================================================



void get_Arribere_TL_data( vector <TORF_Data> * const ORF_Data )
{
	TCSV_Contents Arribere_TL_Data( PATH_DATA_FOLDER, ARRIBERE_TL_LENGTHS_FILE_NAME );
	Arribere_TL_Data.parse_csv();
	
	
	vector <string> gene_name    		  = Arribere_TL_Data.get_csv_column<string>( ARRIBERE_TL_SYST_NAME_HEADER );
	vector <int>    single_peak_TL_length = Arribere_TL_Data.get_csv_column<int>   ( ARRIBERE_TL_LENGTH_HEADER    );

	
	// Add the TL Length to the ORF Data set (both as the single peak TSS, and as a general TSS)
	for( unsigned int i = 0; i < gene_name.size(); i++ )
	{
		unsigned int ORF_it = find_ORF( gene_name[i], *ORF_Data );
		
		if( ORF_it != ORF_Data->size() )
		{
			ORF_Data->at( ORF_it ).single_peak_tss_rel_pos = (-1) * single_peak_TL_length[i];
			
			ORF_Data->at( ORF_it ).add_tss( ORF_Data->at( ORF_it ).single_peak_tss_rel_pos );
		}
	}
	

	return;
}
//==============================================================================



void get_Arribere_SI_data( vector <TORF_Data> * const ORF_Data )  
{
	TCSV_Contents Arribere_Shape_Index_Data( PATH_DATA_FOLDER, ARRIBERE_SI_SCORES_FILE_NAME );
	Arribere_Shape_Index_Data.parse_csv();
	
	vector <string> gene_name   = Arribere_Shape_Index_Data.get_csv_column<string>( ARRIBERE_SI_SYST_NAME_HEADER );
	vector <double> shape_index = Arribere_Shape_Index_Data.get_csv_column<double>( ARRIBERE_SI_SCORE_HEADER 	 );    
	
	
	// Add the shape indexes to the ORF Data set
	for( unsigned int i = 0; i < gene_name.size(); i++ )
	{
		unsigned int ORF_it = find_ORF( gene_name[i], *ORF_Data );
		
		if( ORF_it != ORF_Data->size() )
		{
			ORF_Data->at( ORF_it ).TL_shape_index = shape_index[i];
		}
	}
			
	
	return;
}
//==============================================================================



void get_Arribere_Cap_Distance_index( vector <TORF_Data> * const ORF_Data )  
{
	TCSV_Contents Arribere_TATL_seq_reads( PATH_DATA_FOLDER, ARRIBERE_TATL_SEQ_FILE_NAME );
	Arribere_TATL_seq_reads.parse_csv();
	
	
	vector <vector <unsigned int> > read_pos	( NUM_S_CEREVISIAE_CHROM, vector <unsigned int>( 0 ) );
	vector <vector <double	    > > read_density( NUM_S_CEREVISIAE_CHROM, vector <double      >( 0 ) );
	
	for( unsigned int chrom_it = 0; chrom_it < read_pos.size(); chrom_it++ )
	{
		read_pos[chrom_it] 	   = Arribere_TATL_seq_reads.get_csv_column<unsigned int>( (string)"Chr" + convert_uint_to_roman_numeral( chrom_it + 1 ) + " Positions" 	 );
		read_density[chrom_it] = Arribere_TATL_seq_reads.get_csv_column<double		>( (string)"Chr" + convert_uint_to_roman_numeral( chrom_it + 1 ) + " Read Densities" );
		
		vector <unsigned int>::iterator it = find( read_pos[chrom_it].begin(), read_pos[chrom_it].end(), DEFAULT_NAN );
		if( it == read_pos[chrom_it].end() ) { Errors.handle_error( FATAL, "Error in 'get_Arribere_Cap_Distance_index()': Could not find end of data for chromosome " + get_str( chrom_it + 1 ), __FILE__, __LINE__ ); }
		
		read_pos[chrom_it].erase	( it, 												  				    read_pos[chrom_it].end() 	 );
		read_density[chrom_it].erase( read_density[chrom_it].begin() + ( it - read_pos[chrom_it].begin() ), read_density[chrom_it].end() );
	}
	
	
	for( unsigned int ORF_it = 0; ORF_it < ORF_Data->size(); ORF_it++ )
	{
		unsigned int chrom_it = ORF_Data->at( ORF_it ).chrom_num - 1;
		ORF_Data->at( ORF_it ).get_CDI( read_pos[chrom_it], read_density[chrom_it] ); 
	}
	

	return;
}
//==============================================================================

  
  
void get_He_mRNA_change_data( vector <TORF_Data> * const ORF_Data )
{
	TCSV_Contents He_mRNA_Data( PATH_DATA_FOLDER, HE_mRNA_ABUNDANCE_FILE_NAME );
	He_mRNA_Data.parse_csv();

	vector <string> gene_name  = He_mRNA_Data.get_csv_column<string> ( HE_mRNA_SYST_NAME_HEADER  );
	vector <double> ratio_UPF1 = He_mRNA_Data.get_csv_column<double> ( HE_mRNA_RATIO_UPF1_HEADER );
	vector <double> ratio_UPF2 = He_mRNA_Data.get_csv_column<double> ( HE_mRNA_RATIO_UPF2_HEADER );
	vector <double> ratio_UPF3 = He_mRNA_Data.get_csv_column<double> ( HE_mRNA_RATIO_UPF3_HEADER );
	vector <double> q_val_UPF1 = He_mRNA_Data.get_csv_column<double> ( HE_mRNA_Q_VAL_UPF1_HEADER );
	vector <double> q_val_UPF2 = He_mRNA_Data.get_csv_column<double> ( HE_mRNA_Q_VAL_UPF2_HEADER );
	vector <double> q_val_UPF3 = He_mRNA_Data.get_csv_column<double> ( HE_mRNA_Q_VAL_UPF3_HEADER );

	
	// Add the shape indexes to the ORF Data set
	for( unsigned int i = 0; i < gene_name.size(); i++ )
	{
		unsigned int ORF_it = find_ORF( gene_name[i], *ORF_Data );
		
		if( ORF_it != ORF_Data->size() )
		{
			if    ( q_val_UPF1[i] != 0 )   { ORF_Data->at( ORF_it ).He_mRNA_changes[ UPF1 ] = TRatio_Stat( "", ratio_UPF1[i], q_val_UPF1[i] ); }
			else/*( q_val_UPF1[i] == 0 )*/ { ORF_Data->at( ORF_it ).He_mRNA_changes[ UPF1 ] = TRatio_Stat( "", ratio_UPF1[i], DEFAULT_Q_VAL ); }
			
			if    ( q_val_UPF2[i] != 0 )   { ORF_Data->at( ORF_it ).He_mRNA_changes[ UPF2 ] = TRatio_Stat( "", ratio_UPF2[i], q_val_UPF2[i] ); }
			else/*( q_val_UPF2[i] == 0 )*/ { ORF_Data->at( ORF_it ).He_mRNA_changes[ UPF2 ] = TRatio_Stat( "", ratio_UPF2[i], DEFAULT_Q_VAL ); }
			
			if    ( q_val_UPF3[i] != 0 )   { ORF_Data->at( ORF_it ).He_mRNA_changes[ UPF3 ] = TRatio_Stat( "", ratio_UPF3[i], q_val_UPF3[i] ); }
			else/*( q_val_UPF3[i] == 0 )*/ { ORF_Data->at( ORF_it ).He_mRNA_changes[ UPF3 ] = TRatio_Stat( "", ratio_UPF3[i], DEFAULT_Q_VAL ); }
		}
	}
			
	
	return;
}
//==============================================================================



void get_Lelivelt_mRNA_change_data( vector <TORF_Data> * const ORF_Data )
{
	TCSV_Contents Lelivelt_mRNA_Data( PATH_DATA_FOLDER, LELIVELT_mRNA_ABUNDANCE_FILE_NAME );
	Lelivelt_mRNA_Data.parse_csv();

	vector <string> gene_name 	 = Lelivelt_mRNA_Data.get_csv_column<string> ( LELIVELT_mRNA_SYST_NAME_HEADER	 );
	vector <double> ratio_UPF1 	 = Lelivelt_mRNA_Data.get_csv_column<double> ( LELIVELT_mRNA_RATIO_UPF1_HEADER	 );
	vector <double> ratio_UPF2 	 = Lelivelt_mRNA_Data.get_csv_column<double> ( LELIVELT_mRNA_RATIO_UPF2_HEADER	 );
	vector <double> ratio_UPF3 	 = Lelivelt_mRNA_Data.get_csv_column<double> ( LELIVELT_mRNA_RATIO_UPF3_HEADER	 );
	vector <double> ratio_UPF123 = Lelivelt_mRNA_Data.get_csv_column<double> ( LELIVELT_mRNA_RATIO_UPF123_HEADER );
	vector <double> q_val_UPF1 	 = Lelivelt_mRNA_Data.get_csv_column<double> ( LELIVELT_mRNA_Q_VAL_UPF1_HEADER	 );
	vector <double> q_val_UPF2 	 = Lelivelt_mRNA_Data.get_csv_column<double> ( LELIVELT_mRNA_Q_VAL_UPF2_HEADER	 );
	vector <double> q_val_UPF3 	 = Lelivelt_mRNA_Data.get_csv_column<double> ( LELIVELT_mRNA_Q_VAL_UPF3_HEADER	 );
	vector <double> q_val_UPF123 = Lelivelt_mRNA_Data.get_csv_column<double> ( LELIVELT_mRNA_Q_VAL_UPF123_HEADER );

	
	// Add the ratios and q-values to the ORF Data set
	for( unsigned int i = 0; i < gene_name.size(); i++ )
	{
		unsigned int ORF_it = find_ORF( gene_name[i], *ORF_Data );

		if( ORF_it != ORF_Data->size() )
		{
			if    ( q_val_UPF1[i]   != 0 )   { ORF_Data->at( ORF_it ).Lelivelt_mRNA_changes[ UPF1 ]   = TRatio_Stat( "", ratio_UPF1[i],   q_val_UPF1[i]   ); }
			else/*( q_val_UPF1[i]   == 0 )*/ { ORF_Data->at( ORF_it ).Lelivelt_mRNA_changes[ UPF1 ]   = TRatio_Stat( "", ratio_UPF1[i],   DEFAULT_Q_VAL   ); }
			
			if    ( q_val_UPF2[i]   != 0 )   { ORF_Data->at( ORF_it ).Lelivelt_mRNA_changes[ UPF2 ]   = TRatio_Stat( "", ratio_UPF2[i],   q_val_UPF2[i]   ); }
			else/*( q_val_UPF2[i]   == 0 )*/ { ORF_Data->at( ORF_it ).Lelivelt_mRNA_changes[ UPF2 ]   = TRatio_Stat( "", ratio_UPF2[i],   DEFAULT_Q_VAL   ); }
			
			if    ( q_val_UPF3[i]   != 0 )   { ORF_Data->at( ORF_it ).Lelivelt_mRNA_changes[ UPF3 ]   = TRatio_Stat( "", ratio_UPF3[i],   q_val_UPF3[i]   ); }
			else/*( q_val_UPF3[i]   == 0 )*/ { ORF_Data->at( ORF_it ).Lelivelt_mRNA_changes[ UPF3 ]   = TRatio_Stat( "", ratio_UPF3[i],   DEFAULT_Q_VAL   ); }
			
			if    ( q_val_UPF123[i] != 0 )   { ORF_Data->at( ORF_it ).Lelivelt_mRNA_changes[ UPF123 ] = TRatio_Stat( "", ratio_UPF123[i], q_val_UPF123[i] ); }
			else/*( q_val_UPF123[i] == 0 )*/ { ORF_Data->at( ORF_it ).Lelivelt_mRNA_changes[ UPF123 ] = TRatio_Stat( "", ratio_UPF123[i], DEFAULT_Q_VAL   ); }
		}
	}
			
	
	return;
}
//==============================================================================



void get_Duttagupta_PUB1_binding_data( vector <TORF_Data> * const ORF_Data )
{
	TCSV_Contents Duttagupta_PUB1_binding( PATH_DATA_FOLDER, DUTTAGUPTA_PUB1_BINDING_FILE_NAME );
	Duttagupta_PUB1_binding.parse_csv();

	vector <string> gene_name  = Duttagupta_PUB1_binding.get_csv_column<string> ( DUTTAGUPTA_SYST_NAME_HEADER,  DUTTAGUPTA_PUB1_BINDING_HEADER_ROW );
	vector <double> med_zscore = Duttagupta_PUB1_binding.get_csv_column<double> ( DUTTAGUPTA_MED_ZSCORE_HEADER, DUTTAGUPTA_PUB1_BINDING_HEADER_ROW );
	
	
	for( unsigned int i = 0; i < gene_name.size(); i++ )
	{
		unsigned int ORF_it = find_ORF( gene_name[i], *ORF_Data );

		if( ORF_it != ORF_Data->size() )
		{
			ORF_Data->at( ORF_it ).PUB1_binding_zscore = med_zscore[i]; 
		}
	}
	
	
	return;
}

//==============================================================================


void get_Duttagupta_PUB1_effects_data( vector <TORF_Data> * const ORF_Data )
{
	TCSV_Contents Duttagupta_PUB1_effects( PATH_DATA_FOLDER, DUTTAGUPTA_PUB1_EFFECTS_FILE_NAME );
	Duttagupta_PUB1_effects.parse_csv();

	vector <string> genes_stabilized_by_PUB1   = Duttagupta_PUB1_effects.get_csv_column<string> ( DUTTAGUPTA_STABILIZED_HEADER,   DUTTAGUPTA_PUB1_EFFECTS_HEADER_ROW );
	vector <string> genes_destabilized_by_PUB1 = Duttagupta_PUB1_effects.get_csv_column<string> ( DUTTAGUPTA_DESTABILIZED_HEADER, DUTTAGUPTA_PUB1_EFFECTS_HEADER_ROW );
    vector <string> genes_no_change_by_PUB1    = Duttagupta_PUB1_effects.get_csv_column<string> ( DUTTAGUPTA_NO_CHANGE_HEADER,    DUTTAGUPTA_PUB1_EFFECTS_HEADER_ROW );
	
	unsigned int max_num_genes = genes_stabilized_by_PUB1.size();
	if( genes_destabilized_by_PUB1.size() > max_num_genes ) { max_num_genes = genes_destabilized_by_PUB1.size(); }
	if( genes_no_change_by_PUB1.size()    > max_num_genes ) { max_num_genes = genes_no_change_by_PUB1.size();    }
	
	
	for( unsigned int i = 0; i < max_num_genes; i++ )
	{
		unsigned int ORF_it = ( genes_stabilized_by_PUB1[i] == "" ) ? ORF_Data->size() : find_ORF( genes_stabilized_by_PUB1[i], *ORF_Data );

		if( ORF_it != ORF_Data->size() )
		{
			ORF_Data->at( ORF_it ).stability_PUB1 = STABILIZED; 
		}
		
		
		ORF_it = ( genes_destabilized_by_PUB1[i] == "" ) ? ORF_Data->size() : find_ORF( genes_destabilized_by_PUB1[i], *ORF_Data );

		if( ORF_it != ORF_Data->size() )
		{
			ORF_Data->at( ORF_it ).stability_PUB1 = DESTABILIZED; 
		}
		
		
		ORF_it = ( genes_no_change_by_PUB1[i] == "" ) ? ORF_Data->size() : find_ORF( genes_no_change_by_PUB1[i], *ORF_Data );

		if( ORF_it != ORF_Data->size() )
		{
			ORF_Data->at( ORF_it ).stability_PUB1 = SAME_STABILITY; 
		}
	}
	

	return;
}
//==============================================================================



void get_Duttagupta_PUB1_mRNA_half_lives_data( vector <TORF_Data> * const ORF_Data )
{
	TCSV_Contents Duttagupta_PUB1_mRNA_half_lives( PATH_DATA_FOLDER, DUTTAGUPTA_PUB1_mRNA_HL_FILE_NAME );
	Duttagupta_PUB1_mRNA_half_lives.parse_csv();

	vector <string> gene_name = Duttagupta_PUB1_mRNA_half_lives.get_csv_column<string> ( DUTTAGUPTA_SYST_NAME_HEADER, DUTTAGUPTA_PUB1_HL_HEADER_ROW );
	
	vector < vector <double> > wild_type_half_lives( DUTTAGUPTA_NUM_HL_EXPERIMENTS, vector <double>( 0 ) );
	vector < vector <double> > wild_type_p_values  ( DUTTAGUPTA_NUM_HL_EXPERIMENTS, vector <double>( 0 ) );
	
	vector < vector <double> > mutant_half_lives   ( DUTTAGUPTA_NUM_HL_EXPERIMENTS, vector <double>( 0 ) );
	vector < vector <double> > mutant_p_values     ( DUTTAGUPTA_NUM_HL_EXPERIMENTS, vector <double>( 0 ) );
	

	for( unsigned int j = 0; j < DUTTAGUPTA_NUM_HL_EXPERIMENTS; j++ )
	{
		wild_type_half_lives[j] = Duttagupta_PUB1_mRNA_half_lives.get_csv_column<double> ( DUTTAGUPTA_WT_PREFIX_HEADER     + get_str( j + 1 ) + DUTTAGUPTA_HL_SUFFIX_HEADER,   DUTTAGUPTA_PUB1_HL_HEADER_ROW );
		wild_type_p_values[j]   = Duttagupta_PUB1_mRNA_half_lives.get_csv_column<double> ( DUTTAGUPTA_WT_PREFIX_HEADER     + get_str( j + 1 ) + DUTTAGUPTA_PVAL_SUFFIX_HEADER, DUTTAGUPTA_PUB1_HL_HEADER_ROW );
		mutant_half_lives[j]    = Duttagupta_PUB1_mRNA_half_lives.get_csv_column<double> ( DUTTAGUPTA_MUTANT_PREFIX_HEADER + get_str( j + 1 ) + DUTTAGUPTA_HL_SUFFIX_HEADER,   DUTTAGUPTA_PUB1_HL_HEADER_ROW );
		mutant_p_values[j]      = Duttagupta_PUB1_mRNA_half_lives.get_csv_column<double> ( DUTTAGUPTA_MUTANT_PREFIX_HEADER + get_str( j + 1 ) + DUTTAGUPTA_PVAL_SUFFIX_HEADER, DUTTAGUPTA_PUB1_HL_HEADER_ROW );
	}
	
	
	for( unsigned int i = 0; i < gene_name.size(); i++ )
	{
		unsigned int ORF_it = find_ORF( gene_name[i], *ORF_Data );

		if( ORF_it != ORF_Data->size() )
		{
			// There are some negative half lives (perhaps indicating growth, though this would seem spurious) in the data.
			//   Their analysis simply ignores those values; do the same
			vector <double> wild_type_half_lives_filt( 0 );
			vector <double> mutant_half_lives_filt( 0 );
			 
			for( unsigned int k = 0; k < DUTTAGUPTA_NUM_HL_EXPERIMENTS; k++ )
			{
				if( wild_type_half_lives[k][i] > 0 ) { wild_type_half_lives_filt.push_back( wild_type_half_lives[k][i] ); }
				if( mutant_half_lives[k][i]    > 0 ) { mutant_half_lives_filt.push_back(    mutant_half_lives[k][i]    ); }
			}
			
			// Calculate the median (iff there are nonnegative half lives)
			if( wild_type_half_lives_filt.size() != 0 ) { ORF_Data->at( ORF_it ).w_PUB1_half_life  = mean( wild_type_half_lives_filt ); }
			if( mutant_half_lives_filt.size()    != 0 ) { ORF_Data->at( ORF_it ).wo_PUB1_half_life = mean( mutant_half_lives_filt    ); }
		}
	}
	
	
	return;
}
//==============================================================================



// Note: This data seems suspect, as their analysis was based on a monophasic decay model, which showed that decay rates
// increased for 91% of NMD- mutants (opposing their hypothesis).  However, for a small subset of transcripts that were further
// examined, decay was found to often be biphasic, with decay rates decreased for NMD- mutants in the initial phase.  More analysis
// on this biphasic decay was apparently not done.  
//
// Each of the transcripts in this file were selected by SAM as having >1.5 fold changes in abundance in the NMD- strain.  As expected, 
// the vast majority of these transcripts increased in abundance.  Exact abundance changes weren't published

void get_Guan_NMD_sensitive_transcripts_data( vector <TORF_Data> * const ORF_Data )
{
	TCSV_Contents Guan_NMD_sensitive_transcripts( PATH_DATA_FOLDER, GUAN_NMD_TRANSCRIPTS_FILE_NAME );
	Guan_NMD_sensitive_transcripts.parse_csv();
	
	vector <string> gene_name 	  = Guan_NMD_sensitive_transcripts.get_csv_column<string> ( GUAN_NMD_SYST_NAME_HEADER, 		   GUAN_NMD_GENES_HEADER_ROW );
	vector <string> direct_str    = Guan_NMD_sensitive_transcripts.get_csv_column<string> ( GUAN_NMD_DIRECT_TARGET_HEADER, 	   GUAN_NMD_GENES_HEADER_ROW );
	vector <string> abundance_str = Guan_NMD_sensitive_transcripts.get_csv_column<string> ( GUAN_NMD_ABUNDANCE_CHANGE_HEADER,  GUAN_NMD_GENES_HEADER_ROW );
    vector <double> mut_half_life = Guan_NMD_sensitive_transcripts.get_csv_column<double> ( GUAN_NMD_MUT_HL_HEADER, 	       GUAN_NMD_GENES_HEADER_ROW );
    vector <double> wt_half_life  = Guan_NMD_sensitive_transcripts.get_csv_column<double> ( GUAN_NMD_WT_HL_HEADER, 			   GUAN_NMD_GENES_HEADER_ROW );
    vector <double> fold_change   = Guan_NMD_sensitive_transcripts.get_csv_column<double> ( GUAN_NMD_FCR_HEADER, 			   GUAN_NMD_GENES_HEADER_ROW );
  //vector <double> testing_stat  = Guan_NMD_sensitive_transcripts.get_csv_column<double> ( GUAN_NMD_TESTING_STATISTIC_HEADER, GUAN_NMD_GENES_HEADER_ROW );
    vector <double> p_value 	  = Guan_NMD_sensitive_transcripts.get_csv_column<double> ( GUAN_NMD_PVAL_HEADER, 			   GUAN_NMD_GENES_HEADER_ROW );
	

	for( unsigned int i = 0; i < gene_name.size(); i++ )
	{
		// Include only mRNA's that increased in abundance in the absence of NMD
		if( abundance_str[i] == (string)GUAN_NMD_GENE_HIGHER_ABUNDANCE )
		{
			unsigned int ORF_it = find_ORF( gene_name[i], *ORF_Data );

			if( ORF_it != ORF_Data->size() )
			{
				//ORF_Data->at( ORF_it ).no_NMD_mRNA_abundance = ( abundance_str[i] == (string)GUAN_NMD_GENE_HIGHER_ABUNDANCE ) ? HIGHER : LOWER;

				ORF_Data->at( ORF_it ).direct_NMD_target 	 = ( direct_str[i] == (string)GUAN_NMD_GENE_DIRECT_TARGET ) ? DIRECT : INDIRECT;
				ORF_Data->at( ORF_it ).guan_w_NMD_half_life  =   wt_half_life[i]; 
				ORF_Data->at( ORF_it ).guan_wo_NMD_half_life =   mut_half_life[i];
				ORF_Data->at( ORF_it ).guan_FCR 	 		 =   fold_change[i];  
				ORF_Data->at( ORF_it ).guan_pval 	 		 =   p_value[i];
			}
		}
	}
	
	
	return;
}
//==============================================================================



/*
// Note: This data isn't particularly useful, since we have technically already documented all transcripts sensitive
// to NMD that have uORFs (in 'get_Guan_NMD_sensitive_transcripts_data()')
void get_Guan_uORFs_in_NMD_path_data( vector <TORF_Data> * const ORF_Data )
{
	TCSV_Contents Guan_uORFs_in_NMD_path_data( PATH_DATA_FOLDER, GUAN_UORFS_IN_NMD_PATH_FILE_NAME );
	Guan_uORFs_in_NMD_path_data.parse_csv();

	vector <string> gene_name = Guan_uORFs_in_NMD_path_data.get_csv_column<string> ( GUAN_NMD_uORF_SYST_NAME_HEADER );
	
	
	for( unsigned int i = 0; i < gene_name.size(); i++ )
	{
		unsigned int ORF_it = find_ORF( gene_name[i], *ORF_Data );

		if( ORF_it != ORF_Data->size() )
		{
		
		}
	}
	
	
	return;
}
*/
//==============================================================================



void get_Hogan_PUB1_data( vector <TORF_Data> * const ORF_Data )
{
	TCSV_Contents Hogan_PUB1_data( PATH_DATA_FOLDER, HOGAN_PUB1_DATA_FILE_NAME );
	Hogan_PUB1_data.parse_csv();

	vector <string> gene_name   = Hogan_PUB1_data.get_csv_column<string> ( HOGAN_SYST_NAME_HEADER   );
	vector <double> q_value	    = Hogan_PUB1_data.get_csv_column<double> ( HOGAN_QVAL_HEADER 		);
	vector <double> fold_change = Hogan_PUB1_data.get_csv_column<double> ( HOGAN_FOLD_CHANGE_HEADER );
	
	
	for( unsigned int i = 0; i < gene_name.size(); i++ )
	{
		unsigned int ORF_it = find_ORF( gene_name[i], *ORF_Data );

		if( ORF_it != ORF_Data->size() )
		{
			ORF_Data->at( ORF_it ).Hogan_mRNA_changes[HOGAN_PUB1] = TRatio_Stat( "", fold_change[i], q_value[i] );
		}
	}
	
	
	return;
}
//==============================================================================



void get_Hogan_UPF1_data( vector <TORF_Data> * const ORF_Data )
{
	TCSV_Contents Hogan_UPF1_data( PATH_DATA_FOLDER, HOGAN_UPF1_DATA_FILE_NAME );
	Hogan_UPF1_data.parse_csv();

	vector <string> gene_name   = Hogan_UPF1_data.get_csv_column<string> ( HOGAN_SYST_NAME_HEADER   );
	vector <double> q_value	    = Hogan_UPF1_data.get_csv_column<double> ( HOGAN_QVAL_HEADER 		);
	vector <double> fold_change = Hogan_UPF1_data.get_csv_column<double> ( HOGAN_FOLD_CHANGE_HEADER );
	
	
	for( unsigned int i = 0; i < gene_name.size(); i++ )
	{
		unsigned int ORF_it = find_ORF( gene_name[i], *ORF_Data );
		
		if( ORF_it != ORF_Data->size() )
		{
			ORF_Data->at( ORF_it ).Hogan_mRNA_changes[HOGAN_UPF1] = TRatio_Stat( "", fold_change[i], q_value[i] );
		}
	}
	
	
	return;
}
//==============================================================================



void get_Johansson_mRNA_binding_data( vector <TORF_Data> * const ORF_Data )
{
	TCSV_Contents Johansson_mRNA_change_data( PATH_DATA_FOLDER, JOHANSSON_mRNA_UPF1_BIND_FILE_NAME );
	Johansson_mRNA_change_data.parse_csv();

	vector <string> gene_name 	= Johansson_mRNA_change_data.get_csv_column<string> ( JOHANSSON_SYST_NAME_HEADER     );
	vector <double> fold_change = Johansson_mRNA_change_data.get_csv_column<double> ( JOHANSSON_FOLD_INCREASE_HEADER );
	
	
	for( unsigned int i = 0; i < gene_name.size(); i++ )
	{
		unsigned int ORF_it = find_ORF( gene_name[i], *ORF_Data );

		if( ORF_it != ORF_Data->size() )
		{
			ORF_Data->at( ORF_it ).Johansson_mRNA_binding_fold_change = fold_change[i];
		}
	}
	
	
	return;
}
//==============================================================================



void get_Johansson_mRNA_decay_data( vector <TORF_Data> * const ORF_Data )
{
	TCSV_Contents Johansson_mRNA_change_data( PATH_DATA_FOLDER, JOHANSSON_mRNA_DECAY_FILE_NAME );
	Johansson_mRNA_change_data.parse_csv();

	vector <string> gene_name 	 = Johansson_mRNA_change_data.get_csv_column<string> ( JOHANSSON_SYST_NAME_HEADER      );
	vector <double> NMD_POS_mRNA = Johansson_mRNA_change_data.get_csv_column<double> ( JOHANSSON_60MIN_NMD2_POS_HEADER );
	vector <double> NMD_NEG_mRNA = Johansson_mRNA_change_data.get_csv_column<double> ( JOHANSSON_60MIN_NMD2_NEG_HEADER );
	
	
	for( unsigned int i = 0; i < gene_name.size(); i++ )
	{
		unsigned int ORF_it = find_ORF( gene_name[i], *ORF_Data );

		if( ORF_it != ORF_Data->size() )
		{
			ORF_Data->at( ORF_it ).Johansson_mRNA_decay_fold_change = NMD_NEG_mRNA[i] / NMD_POS_mRNA[i];
		}
	}
	
	
	return;
}
//==============================================================================



unsigned int find_ORF( const string & gene_name, const vector <TORF_Data> & ORF_Data )
{
	unsigned int r_ORF_it = 0;
	
	while( r_ORF_it < ORF_Data.size() && ORF_Data[r_ORF_it].gene_name != gene_name )
	{
		r_ORF_it++;
	}
	
	return r_ORF_it;
}
//==============================================================================



TCSV_Contents * extract_uORF_rows_from_Miura_table( void )
{
	TCSV_Contents uORF_table_all( PATH_DATA_FOLDER, uORF_FILE_NAME_MIURA );

	uORF_table_all.parse_csv();

	
	TCSV_Contents * uORF_table = new TCSV_Contents( PATH_DATA_FOLDER, uORF_FILE_NAME_MIURA );

	vector <string> uORF_info = uORF_table_all.get_csv_column<string>( MIURA_uORFS_INFO_HEADER );

	vector <vector <string> > uORF_table_all_contents = uORF_table_all.get_cell_contents();
	

	uORF_table->append_row( uORF_table_all_contents[0] );

	for( unsigned int i = 0; i < uORF_info.size(); i++ )
	{
		if( uORF_info[i].empty() == FALSE )
		{
			uORF_table->append_row( uORF_table_all_contents[i + 1] );
		}
	}

	return uORF_table;
}
//==============================================================================



int extract_gene_coord_Miura( const TCSV_Contents & uORF_table, const vector <string> & gff_annotations, vector < TORF_Data > * const ORF_Data, unsigned int * const uORF_data_start_it )
{
	vector <string>       UTR_coord_raw = uORF_table.get_csv_column<string>	     ( MIURA_UTR_COORD_HEADER  );
	vector <string>       gene_name     = uORF_table.get_csv_column<string> 	 ( MIURA_SYST_NAME_HEADER  );
	vector <unsigned int> UTR_lengths   = uORF_table.get_csv_column<unsigned int>( MIURA_UTR_LENGTH_HEADER );
	
	if( UTR_coord_raw.size() != gene_name.size() || UTR_coord_raw.size() != UTR_lengths.size() )
	{
		return Errors.handle_error( PASS_UP_ONE_LEVEL, "Error in 'extract_gene_coord_Miura()': Size of uORF data vectors didn't match. ", __FILE__, __LINE__ );
	}
	

	*uORF_data_start_it = ORF_Data->size();
	ORF_Data->reserve( ORF_Data->size() + UTR_coord_raw.size() );

	unsigned int num_genes_not_found = 0;
	bool prev_gene_not_found = FALSE;

	for( unsigned int i = 0; i < UTR_coord_raw.size(); i++ )
	{
		// For some files, only the first row of a gene's uORF list contains the gene name
		if( i > 0 && gene_name[i] == gene_name[i - 1] && UTR_coord_raw[i] == UTR_coord_raw[i - 1] )
		{ 
			if( prev_gene_not_found == FALSE )
			{
				ORF_Data->push_back( TORF_Data( ORF_Data->back().chrom_num,
												ORF_Data->back().gene_name,  
												ORF_Data->back().gene_CDS.start_coord,
												ORF_Data->back().gene_CDS.length,
												ORF_Data->back().gene_intergen_start_coord,
												ORF_Data->back().gene_pos_change, 	
												ORF_Data->back().opposite_strand,
												ORF_Data->back().untransl_reg,
												ORF_Data->back().fpUTR_intron ) );
			}
		}
		else
		{
			unsigned int UTR_chrom_num;
			unsigned int UTR_start_pos;
			unsigned int UTR_end_pos;
			unsigned int gene_pos_change;
			unsigned int act_UTR_length;
			bool opposite_strand;
			
			if( extract_UTR_coord_from_Miura_table_row( UTR_coord_raw[i], 
														gene_name[i], 
														UTR_lengths[i], 
														&UTR_chrom_num,    
														&UTR_start_pos,  
														&UTR_end_pos,  
														&opposite_strand,
														&act_UTR_length  ) )
			{
				return Errors.handle_error( PASS_UP_ONE_LEVEL, "", __FILE__, __LINE__ );
			}
			
			
			unsigned int gene_start_coord;
			unsigned int gene_end_coord;
			unsigned int gene_intergen_start_coord;
			vector <TFeature> untransl_reg;
			TFeature fpUTR_intron( string::npos, string::npos );
				
			if( get_gene_coord_from_gff( gene_name[i], 
										 gff_annotations[UTR_chrom_num - 1], 
										 opposite_strand, 
										 &gene_start_coord, 
										 &gene_end_coord,
										 &gene_intergen_start_coord,
										 &untransl_reg, 
										 &fpUTR_intron ) )
			{
				prev_gene_not_found = TRUE;
				num_genes_not_found++;
				Errors.handle_error( NONFATAL, "", __FILE__, __LINE__ );
			}
			else
			{	
				prev_gene_not_found = FALSE;

				// Find the change in the gene's position
				if( opposite_strand == FALSE )
				{
					gene_pos_change = gene_start_coord - ( UTR_end_pos + 1 );
				}
				else/*( opposite_strand == TRUE )*/
				{
					gene_pos_change = ( UTR_end_pos - 1 ) - gene_start_coord;
				}
				

				ORF_Data->push_back( TORF_Data( UTR_chrom_num, 
												gene_name[i], 
												gene_start_coord,
												abs( (int)gene_end_coord - (int)gene_start_coord + 1 ),
												gene_intergen_start_coord,
												gene_pos_change, 
												opposite_strand,
												untransl_reg, 
											    fpUTR_intron  ) );
				
				// Since the Miura data provides UTR coordinates, add the transcription start site for the gene
				// ORF_Data->back().add_tss( (-1) * act_UTR_length );
			}
		}
	}
	
	if( num_genes_not_found != 0 ) 
	{
		ostringstream error_oss;
		error_oss << "Error in 'extract_gene_coord_Miura()': " << num_genes_not_found << " genes were not found.";
		return Errors.handle_error( PASS_UP_ONE_LEVEL, error_oss.str(), __FILE__, __LINE__ );
	}

	return SUCCESSFUL;
}
//==============================================================================



int extract_UTR_coord_from_Miura_table_row( const string & UTR_coord_raw, 
											const string & gene_name,
											const unsigned int UTR_exp_length, 
											unsigned int * const UTR_chrom_num,    
											unsigned int * const UTR_start_pos,  
											unsigned int * const UTR_end_pos,  
											bool 		 * const opposite_strand,
											unsigned int * const UTR_length )
{
	unsigned int chom_num_start_pos = UTR_coord_raw.find( "chr" ) + 3;
	if( UTR_coord_raw.find( "chr" ) == string::npos )
	{
		return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'extract_UTR_coord_from_Miura_table_row()': " +
															   "Could not find the start of the chromosome identifier in the UTR coordinate column of Table 4 for gene " + 
															   gene_name, __FILE__, __LINE__ );
	}
	
	unsigned int chrom_num_end_pos = UTR_coord_raw.find( '(' ) - 1;
	if( UTR_coord_raw.find( '(' ) == string::npos )
	{
		return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'extract_UTR_coord_from_Miura_table_row()': " +
															   "Could not find the end of the chromosome identifier in the UTR coordinate column of Table 4 for gene " + 
															   gene_name, __FILE__, __LINE__ );
	}
	
	unsigned int chrom_num_len = chrom_num_end_pos - chom_num_start_pos + 1;

	*UTR_chrom_num = atoi( UTR_coord_raw.substr( chom_num_start_pos, chrom_num_len ).c_str() );


	
	unsigned int first_coord_start_pos = UTR_coord_raw.find( '(' ) + 1;
	if( UTR_coord_raw.find( '(' ) == string::npos )
	{
		return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'extract_UTR_coord_from_Miura_table_row()': " +
															   "Could not find the start of the first coordinate in the UTR coordinate column of Table 4 for gene " + 
															   gene_name, __FILE__, __LINE__ );
	}
	
	unsigned int first_coord_end_pos = UTR_coord_raw.find( '-' ) - 1;
	if( UTR_coord_raw.find( '-' ) == string::npos )
	{
		return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'extract_UTR_coord_from_Miura_table_row()': " +
															   "Could not find the end of the first coordinate in the UTR coordinate column of Table 4 for gene " + 
															   gene_name, __FILE__, __LINE__ );
	}
	
	unsigned int first_coord_len = first_coord_end_pos - first_coord_start_pos + 1;

	*UTR_start_pos = atoi( UTR_coord_raw.substr( first_coord_start_pos, first_coord_len ).c_str() );


	
	unsigned int second_coord_start_pos = UTR_coord_raw.rfind( '-' ) + 1;
	if( UTR_coord_raw.rfind( '-' ) == string::npos )
	{
		return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'extract_UTR_coord_from_Miura_table_row()': " +
															   "Could not find the start of the second coordinate in the UTR coordinate column of Table 4 for gene " + 
															   gene_name, __FILE__, __LINE__ );
	}
	
	unsigned int second_coord_end_pos = UTR_coord_raw.find( ')' ) - 1;
	if( UTR_coord_raw.find( ')' ) == string::npos )
	{
		return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'extract_UTR_coord_from_Miura_table_row()': " +
															   "Could not find the end of the second coordinate in the UTR coordinate column of Table 4 for gene " + 
															   gene_name, __FILE__, __LINE__ );
	}

	unsigned int second_coord_len = second_coord_end_pos - second_coord_start_pos + 1;

	*UTR_end_pos = atoi( UTR_coord_raw.substr( second_coord_start_pos, second_coord_len ).c_str() );

	
	
	*opposite_strand = FALSE;
	if( *UTR_end_pos < *UTR_start_pos )
	{
		*opposite_strand = TRUE;
	}
	
	unsigned int UTR_len = abs( (int)*UTR_end_pos - (int)*UTR_start_pos ) + 1;

	/*
	// Note: Decided this check was unnecessary because it will just spit out a bunch of errors that don't actually affect the uORF data
	if( UTR_len != UTR_exp_length )
	{
		ostringstream error_oss;
		error_oss << "Error in 'extract_UTR_coord_from_Miura_table_row()': Size of five prime UTR does not match that specified in Table 4 for gene " << gene_name 
				  << " (calculated: " << UTR_len << "  specified: " << UTR_exp_length << "). ";
			 
		Errors.handle_error( NONFATAL, error_oss.str(), __FILE__, __LINE__ );
	}
	*/
	
	if( UTR_length != NULL ) { *UTR_length = UTR_len; }


	return SUCCESSFUL;
}
//==============================================================================



int extract_uORFs_Miura( const TCSV_Contents & uORF_table, const vector <string> & yeast_chromosomes, vector <TORF_Data> * const ORF_Data, const unsigned int uORF_data_start_it )
{
	unsigned int num_misaligned_uORFs = 0;
	unsigned int num_realigned_uORFs  = 0;
	
	unsigned int num_uORFs = 0;

	unsigned int num_perfect_uORFs = 0;
	unsigned int num_perfect_uORFs_bc_realigned = 0;
	
	
	vector <string> uORF_info_all = uORF_table.get_csv_column<string>( MIURA_uORFS_INFO_HEADER, DEFAULT_HEADER_ROW, REMOVE_SPACES );
	vector <string> UTR_comments  = uORF_table.get_csv_column<string>( MIURA_COMMENTS_HEADER   );
	
	if( uORF_info_all.size()!= ( ORF_Data->size() - uORF_data_start_it ) )
	{
		return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'extract_uORFs_Miura()': Size of uORF data vectors " +
															   "didn't match size of allotted increase in 'ORF_Data' capacity. ", __FILE__, __LINE__ );
	}


	// First, delete all duplicate gene entries (after adding their uORF information to the remaining entry)
	//  Note: This is no longer (easily) feasible, since some rows contain the "UTR-intron" flag, while others from the
	//  same gene do not.  To preserve this flag for the uORFs it applies to, all rows are processed individually
	/*
	string prev_gene_name = "";
	for( unsigned int i = 0; i < uORF_info_all.size(); i++ )
	{
		unsigned int ORF_it = i + uORF_data_start_it;
		
		remove_white_space( &uORF_info_all[i] );
		
		if( ORF_Data->at( ORF_it ).gene_name == prev_gene_name )
		{
			uORF_info_all[i-1].append( uORF_info_all[i] );

			uORF_info_all.erase( uORF_info_all.begin() + i );
			ORF_Data->erase    ( ORF_Data->begin()     + ORF_it );
			
			i--;
		}
		else
		{
			prev_gene_name = ORF_Data->at( ORF_it ).gene_name;
		}	
	}
	*/
	
	

	// Extract the uORFs
	for( unsigned int i = 0; i < uORF_info_all.size(); i++ )
	{	
		unsigned int ORF_it = i + uORF_data_start_it;
		string chrom_seq = yeast_chromosomes[ORF_Data->at( ORF_it ).chrom_num - 1];

		
		// ------------------------------
		// Separate the uORF info field into a vector, with each element corresponding to the info for one uORF
		vector <string> uORF_info_indiv( 0 );
		unsigned int uORF_info_start_pos = 0;
		unsigned int uORF_info_end_pos   = 0;
		
		bool uORF_info_end_reached = FALSE;		

		do
		{	
			uORF_info_end_pos   = uORF_info_all[i].find( ",", uORF_info_start_pos ) - 1;
				
			string new_uORF_info = uORF_info_all[i].substr( uORF_info_start_pos, uORF_info_end_pos - uORF_info_start_pos + 1 );
			bool uORF_info_already_added = FALSE;
			
			for( unsigned int j = 0; j < uORF_info_indiv.size(); j++ )
			{
				if( new_uORF_info == uORF_info_indiv[j] ) { uORF_info_already_added = TRUE; }
			}
			
			if( uORF_info_already_added == FALSE )
			{
				uORF_info_indiv.push_back( new_uORF_info );
			}
			
			uORF_info_start_pos = uORF_info_all[i].find( ",", uORF_info_start_pos ) + 1;
			
			// The field ends with a comma, so the end of the field is reached if no more
			//   commas can be found
			if( uORF_info_all[i].find( ",", uORF_info_start_pos ) == string::npos )
			{
				uORF_info_end_reached = TRUE;
			}
			
		} while( uORF_info_end_reached == FALSE );
		// ------------------------------
		
		
			
		for( unsigned int uORF_it = 0; uORF_it < uORF_info_indiv.size(); uORF_it++ )
		{
			int rel_uORF_pos;
			unsigned int exp_uORF_len;
			double AUGCAI_val;
			
			
			// ------------------------------
			// Extract the relevant information from the Miura Table
			
			// Most entries are of the form [relative_position/length/AUGCAI]
			if( uORF_info_indiv[uORF_it][0] == '-' )
			{
				unsigned int rel_pos_start_pos = 0;
				unsigned int rel_pos_end_pos   = uORF_info_indiv[uORF_it].find( '/', rel_pos_start_pos ) - 1;
				unsigned int rel_pos_len       = rel_pos_end_pos - rel_pos_start_pos + 1;

				rel_uORF_pos = atoi( uORF_info_indiv[uORF_it].substr( rel_pos_start_pos, rel_pos_len ).c_str() );	

				

				unsigned int uORF_len_start_pos = uORF_info_indiv[uORF_it].find( '/' ) + 1;
				unsigned int uORF_len_end_pos   = uORF_info_indiv[uORF_it].find( '/', uORF_len_start_pos ) - 1;
				unsigned int uORF_len_len       = uORF_len_end_pos - uORF_len_start_pos + 1;

				exp_uORF_len = atoi( uORF_info_indiv[uORF_it].substr( uORF_len_start_pos, uORF_len_len ).c_str() ) * 3;		
				
				
				
				unsigned int uORF_AUGCAI_start_pos = uORF_info_indiv[uORF_it].find( '/', uORF_len_start_pos ) + 1;
				unsigned int uORF_AUGCAI_end_pos   = uORF_info_indiv[uORF_it].size() - 1;
				unsigned int uORF_AUGCAI_len       = uORF_AUGCAI_end_pos - uORF_AUGCAI_start_pos + 1;

				AUGCAI_val = atof( uORF_info_indiv[uORF_it].substr( uORF_AUGCAI_start_pos, uORF_AUGCAI_len ).c_str() );
			}
			
			// A few other entries are of the form [M-AA_sequence-*(length/AUGCAI)]
			else if( uORF_info_indiv[uORF_it][0] == 'M' )
			{
				unsigned int uORF_aa_seq_start_pos = 0;
				unsigned int uORF_aa_seqn_end_pos  = uORF_info_indiv[uORF_it].find( '(', uORF_aa_seq_start_pos ) - 1;
				unsigned int uORF_aa_seq_len       = uORF_aa_seqn_end_pos - uORF_aa_seq_start_pos + 1;

				exp_uORF_len = uORF_aa_seq_len * 3;
				
					
					
				unsigned int rel_pos_start_pos = uORF_info_indiv[uORF_it].find( '(' ) + 1;
				unsigned int rel_pos_end_pos;
				if( uORF_info_indiv[uORF_it][rel_pos_start_pos] == '-' ) 
				{ 
					rel_pos_end_pos = find_last_digit_or_char( DIGIT, uORF_info_indiv[uORF_it], rel_pos_start_pos + 1 );
				}
				else
				{
					return Errors.handle_error( PASS_UP_ONE_LEVEL, "Error in 'extract_uORFs_Miura()': uORF information not recognized. ", __FILE__, __LINE__ );
				}
				unsigned int rel_pos_len = rel_pos_end_pos - rel_pos_start_pos + 1;

				rel_uORF_pos = atoi( uORF_info_indiv[uORF_it].substr( rel_pos_start_pos, rel_pos_len ).c_str() );		


				
				unsigned int uORF_AUGCAI_start_pos = uORF_info_indiv[uORF_it].find( '/', rel_pos_end_pos )       + 1;
				unsigned int uORF_AUGCAI_end_pos   = uORF_info_indiv[uORF_it].find( ')', uORF_AUGCAI_start_pos ) - 1;
				unsigned int uORF_AUGCAI_len       = uORF_AUGCAI_end_pos - uORF_AUGCAI_start_pos + 1;

				AUGCAI_val = atof( uORF_info_indiv[uORF_it].substr( uORF_AUGCAI_start_pos, uORF_AUGCAI_len ).c_str() );
			}
			else
			{
				return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'extract_uORFs_Miura()': Gene " + ORF_Data->at( ORF_it ).gene_name + 
																	   " contained uORF information that could not be identified ('" + 
																		uORF_info_indiv[uORF_it] + "').", __FILE__, __LINE__ );
			}
			
			if( UTR_comments[i].find( MIURA_UTR_INTRON_ID ) != string::npos ) 
			{ 
				if( ORF_Data->at( ORF_it ).fpUTR_intron.start_coord == string::npos )
				{
					return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'extract_uORFs_Miura()': Gene " + ORF_Data->at( ORF_it ).gene_name + 
																	       " contained uORFs documented to be affected by a 5' UTR intron," +
																		   " but no such intron was found in the GFF. ", __FILE__, __LINE__ );
				}
				
				rel_uORF_pos -= ORF_Data->at( ORF_it ).fpUTR_intron.length;
			}
			// ------------------------------
			
			

			// ------------------------------
			// Make sure the uORF hasn't already been added (if it has, make sure the lengths match, and replace the AUGCAI if the new entry is higher
			//  Note that although duplicate uORF info entries weren't added, two uORF info entries might refer to the same uORF, but have different
			//  AUGCAI's, so this step is still necessary
			int uORF_prev_it = -1;
			
			for( unsigned int j = 0; j < ORF_Data->at( ORF_it ).get_uORFs().size(); j++ )
			{
				if( rel_uORF_pos == ORF_Data->at( ORF_it ).get_uORF( j ).rel_uORF_pos )
				{
					uORF_prev_it = j;
				}
			}
			
			if( uORF_prev_it != -1 )
			{
				if( exp_uORF_len != ORF_Data->at( ORF_it ).get_uORF( uORF_prev_it ).exp_len )
				{
					ostringstream error_oss;
					error_oss << "Error in 'extract_uORFs_Miura()': Gene " 
							  << ORF_Data->at( ORF_it ).gene_name 
							  << " contained duplicate uORF information for which the uORF length didn't match (Previous Length: "
							  << ORF_Data->at( ORF_it ).get_uORF( uORF_prev_it ).exp_len
							  << "  New Length: "
							  << exp_uORF_len
							  << "). ";
							  
					return Errors.handle_error( PASS_UP_ONE_LEVEL, error_oss.str(), __FILE__, __LINE__ );
				}
				
				if( AUGCAI_val > ORF_Data->at( ORF_it ).get_uORF( uORF_prev_it ).AUGCAI )
				{
					ORF_Data->at( ORF_it ).update_AUGCAI( uORF_prev_it, AUGCAI_val );
					
					if( ORF_Data->at( ORF_it ).get_uORF( uORF_prev_it ).AUGCAI != DEFAULT_AUGCAI )
					{
						ostringstream error_oss;
						error_oss << "Error in 'extract_uORFs_Miura()': Two AUGCAI values (" 
								  << AUGCAI_val 
								  << " and " 
								  << ORF_Data->at( ORF_it ).get_uORF( uORF_prev_it ).AUGCAI 
								  << ") were documented for the uORF located at position " 
								  << rel_uORF_pos
								  << " relative to the gene "
								  << ORF_Data->at( ORF_it ).gene_name;
								  
						return Errors.handle_error( PASS_UP_ONE_LEVEL, error_oss.str(), __FILE__, __LINE__ );
					}
				}
			}
			// ------------------------------
			
			
			
			// ------------------------------
			// If it hasn't already been added, add it
			else
			{	
				if( ORF_Data->at( ORF_it ).extract_uORF( rel_uORF_pos,
														 exp_uORF_len,
														 "ATG",
														 chrom_seq,
														 DATA_SOURCE_MIURA,
														 &num_realigned_uORFs,
														 &num_misaligned_uORFs  ) )
				{
					return Errors.handle_error( PASS_UP_ONE_LEVEL, "Error in 'extract_uORFs_Miura()': ", __FILE__, __LINE__ );
				}
				
				
				
				// Define a "perfect uORF" as one that was found as documented in the data table (except small alignment changes),
				//   and doesn't extend past the gene's start
				if( ORF_Data->at( ORF_it ).get_uORFs().back().start_codon 		  == TRUE && 
					ORF_Data->at( ORF_it ).get_uORFs().back().num_nt_len_changed  == 0    && 
					ORF_Data->at( ORF_it ).get_uORFs().back().end_past_gene_start == FALSE )
				{
					num_perfect_uORFs++;
					if( ORF_Data->at( ORF_it ).get_uORFs().back().num_nt_start_moved != 0 ) { num_perfect_uORFs_bc_realigned++; }
				}
							 
				num_uORFs++;
			}
			// ------------------------------
		}

	}
	
	
	// Output a summary of the collected uORFs
	ostringstream output_oss;
	output_oss << "num uORFs:                 " << num_uORFs                      << "\n"
		       
			   << "num_misaligned_uORFs:      " << num_misaligned_uORFs           << "\n"
			   << "num_realigned_uORFs:       " << num_realigned_uORFs            << "\n"
			   
			   << "num_perfect_uORFs:         " << num_perfect_uORFs              << "\n"
			   << "num_perfects_bc_realigned: " << num_perfect_uORFs_bc_realigned << "\n";
	
	output_text_line( output_oss.str() );
	
	return SUCCESSFUL;
}
//==============================================================================


	
int extract_gene_coord_Ingolia( const TCSV_Contents & uORF_table, 
							    const vector <string> & gff_annotations, 
							    const vector <string> & yeast_chromosomes, 
							    vector <TORF_Data> * const ORF_Data, 
							    unsigned int * const uORF_data_start_it )
{
	// ------------------------------
	// Read in (from the Ingolia data table) the gene names and expected uORF coordinates and distances from uORF end to the start of the CDS
	vector <string> gene_name  		= uORF_table.get_csv_column<string>( INGOLIA_SYST_NAME_HEADER 	);
	vector <string> uORF_coord 		= uORF_table.get_csv_column<string>( INGOLIA_uORF_COORD_HEADER 	);
	vector <int>    uORF_end_to_CDS = uORF_table.get_csv_column<int>   ( INGOLIA_uORF_TO_CDS_HEADER );
	vector <double> CDS_Rib_rpkM    = uORF_table.get_csv_column<double>( INGOLIA_CDS_RIB_HEADER 	);
	vector <double> CDS_mRNA_rpkM   = uORF_table.get_csv_column<double>( INGOLIA_CDS_mRNA_HEADER 	);
	
	if( gene_name.size() != uORF_coord.size() || gene_name.size() != uORF_end_to_CDS.size() || gene_name.size() != CDS_Rib_rpkM.size() || gene_name.size() != CDS_mRNA_rpkM.size() )
	{
		return Errors.handle_error( PASS_UP_ONE_LEVEL, "Error in 'extract_gene_coord_Ingolia()': Size of uORF data vectors didn't match. ", __FILE__, __LINE__ );
	}
	// ------------------------------
	
	
	unsigned int num_genes_not_found = 0;
	bool prev_gene_not_found = FALSE;

	*uORF_data_start_it = ORF_Data->size();
	ORF_Data->reserve( ORF_Data->size() + gene_name.size() );
	

	for( unsigned int i = 0; i < gene_name.size(); i++ )
	{
		// For some files, only the first row of a gene's uORF list contains the gene name
		if( i > 0 && gene_name[i] == gene_name[i - 1] )
		{ 
			if( prev_gene_not_found == FALSE )
			{
				ORF_Data->push_back( TORF_Data( ORF_Data->back().chrom_num,
												ORF_Data->back().gene_name,  
												ORF_Data->back().gene_CDS.start_coord,
												ORF_Data->back().gene_CDS.length,
												ORF_Data->back().gene_intergen_start_coord,
												ORF_Data->back().gene_pos_change, 	
												ORF_Data->back().opposite_strand,
												ORF_Data->back().untransl_reg, 
												ORF_Data->back().fpUTR_intron ) );
			}
		}
		else
		{
			// ------------------------------
			// Find the uORF's chromosome number
			unsigned int chom_num_start_pos = uORF_coord[i].find( "chr" ) + 3;
			if( uORF_coord[i].find( "chr" ) == string::npos )
			{
				return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'extract_gene_start_coord_Ingolia()': " +
																	   "Could not find the start of the chromosome identifier in the UTR coordinate column of the Ingolia table for gene " + 
																	   gene_name[i], __FILE__, __LINE__ );
			}
			
			unsigned int chrom_num_end_pos = uORF_coord[i].find( '@' ) - 1;
			if( uORF_coord[i].find( '@' ) == string::npos )
			{
				return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'extract_gene_start_coord_Ingolia()': " +
																	   "Could not find the end of the chromosome identifier in the uORF coordinate column of the Ingolia table for gene " + 
																	   gene_name[i], __FILE__, __LINE__ );
			}
			
			unsigned int chrom_num_len = chrom_num_end_pos - chom_num_start_pos + 1;
					
			unsigned int chrom_num = convert_roman_numeral_to_uint( uORF_coord[i].substr( chom_num_start_pos, chrom_num_len ) );
			// ------------------------------
		
		
		
			// ------------------------------
			// Determine what DNA strand the uORF is on
			bool opposite_strand;
			if	   ( gene_name[i].find( 'W' ) != string::npos ) { opposite_strand = FALSE; }
			else if( gene_name[i].find( 'C' ) != string::npos ) { opposite_strand = TRUE;  }
			else
			{
				return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'extract_gene_start_coord_Ingolia()': Gene " + gene_name[i] + 
																	   " could not be recognized as either residing on the Watson or Crick strand. ", __FILE__, __LINE__ );
			}
			// ------------------------------
			
			
			
			// ------------------------------
			// Calculate the expected gene start coordinate based on the data in the Ingolia table
			unsigned int gene_start_coord_OLD;
			
			// Note that regardless of what strand the uORF is on, the second coordinate listed in the Ingolia table entry corresponds to the end of the uORF
			unsigned int uORF_end_coord_start_pos = uORF_coord[i].find( INGOLIA_uORF_COORD_SEP ) + ( (string)INGOLIA_uORF_COORD_SEP ).size();

			if( uORF_coord[i].find( INGOLIA_uORF_COORD_SEP ) == string::npos )
			{
				return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'extract_gene_start_coord_Ingolia()': " +
																	   "Could not find the end of the coordinate separator in the uORF coordinate column of the Ingolia table for gene " +
																	   gene_name[i], __FILE__, __LINE__ );
			}

			unsigned int uORF_end_coord_end_pos   = find_last_digit_or_char( DIGIT, uORF_coord[i], uORF_end_coord_start_pos );
			unsigned int uORF_end_coord_len       = uORF_end_coord_end_pos - uORF_end_coord_start_pos + 1;	

			unsigned int uORF_end_coord = atoi( uORF_coord[i].substr( uORF_end_coord_start_pos, uORF_end_coord_len ).c_str() );
			
		
			if( opposite_strand == FALSE )
			{
				gene_start_coord_OLD = uORF_end_coord + uORF_end_to_CDS[i];
			}
			else/*( opposite_strand == TRUE )*/
			{
				gene_start_coord_OLD = uORF_end_coord - uORF_end_to_CDS[i];
			}	
			// ------------------------------
			

			
			// ------------------------------
			// Retrieve the gene coordinates from the most recent annotations
			unsigned int gene_start_coord;
			unsigned int gene_end_coord;
			unsigned int gene_intergen_start_coord;
			vector <TFeature> untransl_reg;
			TFeature fpUTR_intron( string::npos, string::npos );
				
			if( get_gene_coord_from_gff( gene_name[i], 
										 gff_annotations[chrom_num - 1], 
										 opposite_strand, 
										 &gene_start_coord, 
										 &gene_end_coord, 
										 &gene_intergen_start_coord,
										 &untransl_reg, 
										 &fpUTR_intron ) )
			{
				prev_gene_not_found = TRUE;
				num_genes_not_found++;
				Errors.handle_error( NONFATAL, "", __FILE__, __LINE__ );
			}
			else
			{
				prev_gene_not_found = FALSE;

				int gene_pos_change = gene_start_coord - gene_start_coord_OLD;		
				if( opposite_strand == TRUE ) { gene_pos_change *= (-1); }
				
				// If the gene was successfully found in the .gff, add an entry to the ORF_Data vector
				ORF_Data->push_back( TORF_Data( chrom_num, 
												gene_name[i], 
												gene_start_coord,
												abs( (int)gene_end_coord - (int)gene_start_coord + 1 ),
												gene_intergen_start_coord,
												gene_pos_change, 
												opposite_strand,
												untransl_reg, 
												fpUTR_intron,
												CDS_Rib_rpkM[i] ) );
			}
			
			
			ORF_Data->back().CDS_mRNA_rpkM = CDS_mRNA_rpkM[i];
			// ------------------------------
		}
	}

	
	if( num_genes_not_found != 0 ) 
	{
		ostringstream error_oss;
		error_oss << "Error in 'extract_gene_start_coord_Ingolia()': " << num_genes_not_found << " genes were not found.";
		return Errors.handle_error( PASS_UP_ONE_LEVEL, error_oss.str(), __FILE__, __LINE__ );
	}
	
	return SUCCESSFUL;
}
//==============================================================================



int extract_uORFs_Ingolia( const TCSV_Contents & uORF_table, const vector <string> & yeast_chromosomes, vector <TORF_Data> * const ORF_Data, const unsigned int uORF_data_start_it )
{
	unsigned int num_uORFs = 0;
	
	unsigned int num_perfect_uORFs = 0;
	unsigned int num_perfect_uORFs_bc_realigned = 0;
	
	unsigned int num_misaligned_uORFs = 0;
	unsigned int num_realigned_uORFs  = 0;
	
	
	// ------------------------------
	// Read in (from the Ingolia data table) the expected uORF lengths and distances from uORF end to the start of the CDS
	vector <unsigned int> exp_uORF_len  		 = uORF_table.get_csv_column<unsigned int>( GENERAL_uORF_LENGTH_HEADER );
	vector <int>          uORF_end_to_CDS        = uORF_table.get_csv_column<int>		  ( INGOLIA_uORF_TO_CDS_HEADER );
	vector <int>    	  num_ribosomes          = uORF_table.get_csv_column<int>		  ( INGOLIA_NUM_RIB_HEADER 	   );
	vector <int>    	  num_mRNA          	 = uORF_table.get_csv_column<int>		  ( INGOLIA_NUM_mRNA_HEADER    );
	vector <string> 	  exp_uORF_start_context = uORF_table.get_csv_column<string>	  ( INGOLIA_INIT_SITE_HEADER   );
	
	vector <string> uORF_start_codon( exp_uORF_start_context.size() );
	
	if( exp_uORF_len.size()!= ( ORF_Data->size() - uORF_data_start_it ) )
	{
		return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'extract_uORFs_Ingolia()': Size of uORF data vectors didn't match size of "
															   "allotted increase in 'ORF_Data' capacity. ", __FILE__, __LINE__ );
	}
	
	
	for( unsigned int i = 0; i < exp_uORF_len.size(); i++ )
	{
		cap_or_uncap_str( &exp_uORF_start_context[i], CAPITALIZE );
		uORF_start_codon[i] = exp_uORF_start_context[i].substr( START_CONTEXT_NT_BEFORE_AUG, 3 );
	}
	// ------------------------------
	
	
	
	// ------------------------------
	// Extract the uORFs from the chromosomes using the Ingolia data
	for( unsigned int i = 0; i < exp_uORF_len.size(); i++ )
	{
		unsigned int ORF_it = i + uORF_data_start_it;
		string chrom_seq = yeast_chromosomes[ORF_Data->at( ORF_it ).chrom_num - 1];
		
		
		// Relative position is found by adding the lenth to he distance from the end of the uORF to the CDS
		int rel_uORF_pos = (-1) * ( exp_uORF_len[i] + uORF_end_to_CDS[i] - 1 );


		if( ORF_Data->at( ORF_it ).extract_uORF( rel_uORF_pos,
												 exp_uORF_len[i],
												 uORF_start_codon[i],
												 chrom_seq,
												 DATA_SOURCE_INGOLIA,
												 &num_realigned_uORFs,
												 &num_misaligned_uORFs,
												 exp_uORF_start_context[i],
												 num_ribosomes[i],
												 num_mRNA[i] ) )
		{
			return Errors.handle_error( PASS_UP_ONE_LEVEL, "Error in 'extract_uORFs_Ingolia()': ", __FILE__, __LINE__ );
		}
		
		
		
		// Define a "perfect uORF" as one that was found as documented in the data table (except small alignment changes),
		//   and doesn't extend past the gene's start
		if( ORF_Data->at( ORF_it ).get_uORFs().back().start_codon 		  == TRUE && 
			ORF_Data->at( ORF_it ).get_uORFs().back().num_nt_len_changed  == 0    && 
			ORF_Data->at( ORF_it ).get_uORFs().back().end_past_gene_start == FALSE )
		{
			num_perfect_uORFs++;
			if( ORF_Data->at( ORF_it ).get_uORFs().back().num_nt_start_moved != 0 ) { num_perfect_uORFs_bc_realigned++; }
		}
					 
		num_uORFs++;
	}
	// ------------------------------
	
	
	
	// ------------------------------
	// Output a summary of the collected uORFs
	ostringstream output_oss;
	output_oss << "num uORFs:                 " << num_uORFs                      << "\n"
		       
			   << "num_misaligned_uORFs:      " << num_misaligned_uORFs           << "\n"
			   << "num_realigned_uORFs:       " << num_realigned_uORFs            << "\n"
			   
			   << "num_perfect_uORFs:         " << num_perfect_uORFs              << "\n"
			   << "num_perfects_bc_realigned: " << num_perfect_uORFs_bc_realigned << "\n";
	
	output_text_line( output_oss.str() );
	// ------------------------------

	return SUCCESSFUL;
}
//==============================================================================



int extract_gene_coord_Zhang_Dietrich( const TCSV_Contents & uORF_table, 
									   const vector <string> & gff_annotations, 
									   vector <TORF_Data> * const ORF_Data, 
									   unsigned int * const uORF_data_start_it )
{
	// ------------------------------
	// Read in (from the Zhang & Dietrich data table) the gene names and chromosomes
	vector <string>       gene_name = uORF_table.get_csv_column<string>      ( GENERAL_SYST_NAME_HEADER  );
	vector <unsigned int> chrom_num = uORF_table.get_csv_column<unsigned int>( GENERAL_CHROMOSOME_HEADER );

	if( gene_name.size() != chrom_num.size() )
	{
		return Errors.handle_error( PASS_UP_ONE_LEVEL, "Error in 'extract_gene_coord_Zhang_Dietrich()': Size of uORF data vectors didn't match. ", __FILE__, __LINE__ );
	}
	// ------------------------------
	
	
	
	unsigned int num_genes_not_found = 0;

	*uORF_data_start_it = ORF_Data->size();
	ORF_Data->reserve( ORF_Data->size() + gene_name.size() );
	

	for( unsigned int i = 0; i < gene_name.size(); i++ )
	{
		// ------------------------------
		// Determine what DNA strand the uORF is on
		bool opposite_strand;
		if	   ( gene_name[i].find( 'W' ) != string::npos ) { opposite_strand = FALSE; }
		else if( gene_name[i].find( 'C' ) != string::npos ) { opposite_strand = TRUE;  }
		else
		{
			return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'extract_gene_coord_Zhang_Dietrich()': Gene " + gene_name[i] +
																   " could not be recognized as either residing on the Watson or Crick strand. ", __FILE__, __LINE__ );
		}
		// ------------------------------
		
		
		
		// ------------------------------
		// Retrieve the gene coordinates from the most recent annotations
		unsigned int gene_start_coord;
		unsigned int gene_end_coord;
		unsigned int gene_intergen_start_coord;
		vector <TFeature> untransl_reg;
		TFeature fpUTR_intron( string::npos, string::npos );
			
		if( get_gene_coord_from_gff( gene_name[i], 
									 gff_annotations[chrom_num[i] - 1], 
									 opposite_strand, 
									 &gene_start_coord, 
									 &gene_end_coord, 
									 &gene_intergen_start_coord,
									 &untransl_reg, 
									 &fpUTR_intron ) )
		{
			num_genes_not_found++;
			Errors.handle_error( NONFATAL, "", __FILE__, __LINE__ );
		}
		else
		{
			// If the gene was successfully found in the .gff, add an entry to the ORF_Data vector
			ORF_Data->push_back( TORF_Data( chrom_num[i],
											gene_name[i], 
											gene_start_coord,
											abs( (int)gene_end_coord - (int)gene_start_coord + 1 ),
											gene_intergen_start_coord,
											0, 						// Old gene start position unknown, so gene position change assumed to be 0
											opposite_strand, 
											untransl_reg,
											fpUTR_intron ) );
		}
		// ------------------------------
	}

	
	if( num_genes_not_found != 0 ) 
	{
		ostringstream error_oss;
		error_oss << "Error in 'extract_gene_coord_Zhang_Dietrich()': " << num_genes_not_found << " genes were not found.";
		return Errors.handle_error( PASS_UP_ONE_LEVEL, error_oss.str(), __FILE__, __LINE__ );
	}
	
	return SUCCESSFUL;
}
//==============================================================================



int extract_uORFs_Zhang_Dietrich_NAR( const TCSV_Contents & uORF_table, const vector <string> & yeast_chromosomes, vector <TORF_Data> * const ORF_Data, const unsigned int uORF_data_start_it )
{
	unsigned int num_uORFs = 0;
	
	unsigned int num_perfect_uORFs = 0;
	unsigned int num_perfect_uORFs_bc_realigned = 0;
	
	unsigned int num_misaligned_uORFs = 0;
	unsigned int num_realigned_uORFs  = 0;
	
	
	// ------------------------------
	// Read in (from the Zhang Dietrich Nucleic Acid Research data table) the expected uORF lengths and distances from uORF end to the start of the CDS
	vector <unsigned int> exp_uORF_len    = uORF_table.get_csv_column<unsigned int>( GENERAL_uORF_LENGTH_HEADER    );
	vector <int> 		  TSS_rel_pos     = uORF_table.get_csv_column<int>		   ( ZHANG_DIET_NAR_TSS_POS_HEADER );
	vector <string> 	  exp_uORF_AA_seq = uORF_table.get_csv_column<string>	   ( ZHANG_DIET_NAR_uORF_HEADER    );
	
	if( exp_uORF_len.size() != ( ORF_Data->size() - uORF_data_start_it ) )
	{
		return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'extract_uORFs_Zhang_Dietrich_NAR()': Size of uORF data vectors " +
															   "didn't match size of allotted increase in 'ORF_Data' capacity. ", __FILE__, __LINE__ );
	}
	// ------------------------------
	
	
	
	// ------------------------------
	// Extract the uORFs from the chromosomes using the Zhang Dietrich Nucleic Acid Research data
	for( unsigned int i = 0; i < exp_uORF_len.size(); i++ )
	{
		unsigned int ORF_it = i + uORF_data_start_it;
		string chrom_seq = yeast_chromosomes[ORF_Data->at( ORF_it ).chrom_num - 1];
		
		
		
		// ------------------------------
		// Extract the 5' UTR
		unsigned int UTR_first_coord = ( ORF_Data->at( ORF_it ).gene_CDS.start_coord - 1 ) + TSS_rel_pos[i];
		if( ORF_Data->at( ORF_it ).opposite_strand == TRUE ) { UTR_first_coord = ( ORF_Data->at( ORF_it ).gene_CDS.start_coord - 1 ) + 1; }
		
		unsigned int UTR_len = abs( TSS_rel_pos[i] );
		
		unsigned int start_coord;
		if( ORF_Data->at( ORF_it ).opposite_strand == FALSE )
		{
			start_coord = UTR_first_coord + 1;
		}
		else/*( ORF_Data->at( ORF_it ).opposite_strand == TRUE )*/
		{
			start_coord = UTR_first_coord + 1 + ( UTR_len - 1 );
		}
			
		string five_prime_UTR = extract_DNA_seq( chrom_seq, start_coord, UTR_len,  ORF_Data->at( ORF_it ).opposite_strand );
		// ------------------------------
		
		
		
		// ------------------------------
		// Find the expected uORF AA sequence in the 5' UTR by starting with the start codon and incrementing
		vector <unsigned int> prev_poss_uORF_start_pos_in_UTR( 0 );
		vector <unsigned int> poss_uORF_start_pos_in_UTR( 0 );
		unsigned int num_AA_to_search = 1;
		
		do
		{
			unsigned int search_pos_start = 1;
			unsigned int AA_seq_start_pos = 0;
			
			if( poss_uORF_start_pos_in_UTR.size() != 1 )
			{
				prev_poss_uORF_start_pos_in_UTR = poss_uORF_start_pos_in_UTR;
				poss_uORF_start_pos_in_UTR.clear();
			}
			
			do
			{
				AA_seq_start_pos = find_AA_seq( five_prime_UTR, exp_uORF_AA_seq[i].substr( 0, num_AA_to_search ), search_pos_start );
				
				if( AA_seq_start_pos <= five_prime_UTR.size() )
				{
					poss_uORF_start_pos_in_UTR.push_back( AA_seq_start_pos );
					search_pos_start = AA_seq_start_pos + 1;
				}
				
			} while( AA_seq_start_pos <= five_prime_UTR.size() );

			
			num_AA_to_search++;
			
		
		} while( num_AA_to_search <= exp_uORF_AA_seq[i].size() && poss_uORF_start_pos_in_UTR.size() > 1 );
		
		
		
		if( ( poss_uORF_start_pos_in_UTR.size() == 0 && prev_poss_uORF_start_pos_in_UTR.size() > 1 ) ||
			( poss_uORF_start_pos_in_UTR.size() > 1 ) )
		{
			ostringstream error_oss;
			error_oss << "Error in 'extract_uORFs_Zhang_Dietrich_NAR()': Multiple uORF substrings of the same length were found in the 5' UTR for gene " 
					  << ORF_Data->at( ORF_it ).gene_name
					  << ". The correct uORF cannot be resolved. ";
					  
			return Errors.handle_error( PASS_UP_ONE_LEVEL, error_oss.str(), __FILE__, __LINE__ );
		}
		else if( poss_uORF_start_pos_in_UTR.size() == 0 )
		{
			return Errors.handle_error( PASS_UP_ONE_LEVEL, "Error in 'extract_uORFs_Zhang_Dietrich_NAR()': No upstream ATG's were found. ", __FILE__, __LINE__ );
		}
		// ------------------------------
		
		
		
		// ------------------------------
		// Extract the relevant information from the Zhang Dietrich Nucleic Acid Research table; Then extract the uORF and manipulate it as necessary
		int rel_uORF_pos = (-1) * ( five_prime_UTR.size() - poss_uORF_start_pos_in_UTR[0] + 1 );

		if( ORF_Data->at( ORF_it ).extract_uORF( rel_uORF_pos,
												 exp_uORF_len[i],
												 "ATG",
												 chrom_seq,
												 DATA_SOURCE_ZHANG_DIETRICH_NAR,
												 &num_realigned_uORFs,
												 &num_misaligned_uORFs ) )
		{
			return Errors.handle_error( PASS_UP_ONE_LEVEL, "Error in 'extract_uORFs_Zhang_Dietrich_NAR()': ", __FILE__, __LINE__ );
		}
		
		
		// Make sure that the uORF amino acid sequence matches what was expected
		string uORF_AA_seq = convert_to_AA_seq( ORF_Data->at( ORF_it ).get_uORFs().back().content );
		if( uORF_AA_seq != exp_uORF_AA_seq[i] )
		{
			return Errors.handle_error( PASS_UP_ONE_LEVEL, "Error in 'extract_uORFs_Zhang_Dietrich_NAR()': uORF did not match what was expected. ", __FILE__, __LINE__ );
		}
		
		
		// Define a "perfect uORF" as one that was found as documented in the data table (except small alignment changes),
		//   and doesn't extend past the gene's start
		if( ORF_Data->at( ORF_it ).get_uORFs().back().start_codon 		  == TRUE && 
			ORF_Data->at( ORF_it ).get_uORFs().back().num_nt_len_changed  == 0    && 
			ORF_Data->at( ORF_it ).get_uORFs().back().end_past_gene_start == FALSE )
		{
			num_perfect_uORFs++;
			if( ORF_Data->at( ORF_it ).get_uORFs().back().num_nt_start_moved != 0 ) { num_perfect_uORFs_bc_realigned++; }
		}
		
		num_uORFs++;
	}
	// ------------------------------
	
	
	
	// ------------------------------
	// Output a summary of the collected uORFs
	ostringstream output_oss;
	output_oss << "num uORFs:                 " << num_uORFs                      << "\n"
		       
			   << "num_misaligned_uORFs:      " << num_misaligned_uORFs           << "\n"
			   << "num_realigned_uORFs:       " << num_realigned_uORFs            << "\n"
			   
			   << "num_perfect_uORFs:         " << num_perfect_uORFs              << "\n"
			   << "num_perfects_bc_realigned: " << num_perfect_uORFs_bc_realigned << "\n";
	
	output_text_line( output_oss.str() );
	// ------------------------------

	return SUCCESSFUL;
}
//==============================================================================



int extract_uORFs_Zhang_Dietrich_CG( const TCSV_Contents & uORF_table, const vector <string> & yeast_chromosomes, vector <TORF_Data> * const ORF_Data, const unsigned int uORF_data_start_it )
{
	unsigned int num_uORFs = 0;
	
	unsigned int num_perfect_uORFs = 0;
	unsigned int num_perfect_uORFs_bc_realigned = 0;
	
	unsigned int num_misaligned_uORFs = 0;
	unsigned int num_realigned_uORFs  = 0;
	
	
	// ------------------------------
	// Read in (from the Zhang Dietrich Current Genetics data table) the expected uORF lengths and distances from uORF end to the start of the CDS
	vector <unsigned int> exp_uORF_len  	 	 = uORF_table.get_csv_column<unsigned int>( GENERAL_uORF_LENGTH_HEADER 	 );
	vector <int> 		  rel_uORF_pos 	   		 = uORF_table.get_csv_column<int>		  ( ZHANG_DIET_CG_REL_POS_HEADER );
	vector <string> 	  exp_uORF_AA_seq 	     = uORF_table.get_csv_column<string>	  ( ZHANG_DIET_CG_uORF_HEADER 	 );
	vector <string> 	  exp_uORF_start_context = uORF_table.get_csv_column<string>	  ( ZHANG_DIET_CG_CONTEXT_HEADER );
	
	if( exp_uORF_len.size() != ( ORF_Data->size() - uORF_data_start_it ) )
	{
		return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'extract_uORFs_Zhang_Dietrich_CG()': Size of uORF data vectors " +
															   "didn't match size of allotted increase in 'ORF_Data' capacity. ", __FILE__, __LINE__ );
	}
	// ------------------------------
	
	
	
	// ------------------------------
	// Extract the uORFs from the chromosomes using the Zhang Dietrich Current Genetics data
	for( unsigned int i = 0; i < exp_uORF_len.size(); i++ )
	{
		unsigned int ORF_it = i + uORF_data_start_it;
		string chrom_seq = yeast_chromosomes[ORF_Data->at( ORF_it ).chrom_num - 1];
		

		// Extract the uORF and manipulate it as necessary
		if( ORF_Data->at( ORF_it ).extract_uORF( rel_uORF_pos[i],
												 exp_uORF_len[i],
												 "ATG",
												 chrom_seq,
												 DATA_SOURCE_ZHANG_DIETRICH_CG,
												 &num_realigned_uORFs,
												 &num_misaligned_uORFs,
												 exp_uORF_start_context[i] ) )
		{
			return Errors.handle_error( PASS_UP_ONE_LEVEL, "Error in 'extract_uORFs_Zhang_Dietrich_CG()': ", __FILE__, __LINE__ );
		}
		
		
		// Make sure that the uORF amino acid sequence matches what was expected
		string uORF_AA_seq = convert_to_AA_seq( ORF_Data->at( ORF_it ).get_uORFs().back().content );
		if( uORF_AA_seq != exp_uORF_AA_seq[i] )
		{
			return Errors.handle_error( PASS_UP_ONE_LEVEL, "Error in 'extract_uORFs_Zhang_Dietrich_CG()': uORF did not match what was expected. ", __FILE__, __LINE__ );
		}
		
		
		// Define a "perfect uORF" as one that was found as documented in the data table (except small alignment changes),
		//   and doesn't extend past the gene's start
		if( ORF_Data->at( ORF_it ).get_uORFs().back().start_codon 		  == TRUE && 
			ORF_Data->at( ORF_it ).get_uORFs().back().num_nt_len_changed  == 0    && 
			ORF_Data->at( ORF_it ).get_uORFs().back().end_past_gene_start == FALSE )
		{
			num_perfect_uORFs++;
			if( ORF_Data->at( ORF_it ).get_uORFs().back().num_nt_start_moved != 0 ) { num_perfect_uORFs_bc_realigned++; }
		}
		
		num_uORFs++;
	}
	// ------------------------------
	
	
	
	// ------------------------------
	// Output a summary of the collected uORFs
	ostringstream output_oss;
	output_oss << "num uORFs:                 " << num_uORFs                      << "\n"
		       
			   << "num_misaligned_uORFs:      " << num_misaligned_uORFs           << "\n"
			   << "num_realigned_uORFs:       " << num_realigned_uORFs            << "\n"
			   
			   << "num_perfect_uORFs:         " << num_perfect_uORFs              << "\n"
			   << "num_perfects_bc_realigned: " << num_perfect_uORFs_bc_realigned << "\n";
	
	output_text_line( output_oss.str() );
	// ------------------------------

	return SUCCESSFUL;
}
//==============================================================================



int extract_gene_coord_Nagalakshmi( const TCSV_Contents & uORF_table, const vector <string> & gff_annotations, vector <TORF_Data> * const ORF_Data, unsigned int * const uORF_data_start_it )
{
	// ------------------------------
	// Read in (from the Nagalakshmi data table) the gene names and chromosomes
	vector <string> gene_name     = uORF_table.get_csv_column<string>( NAGALAKSHMI_SYST_NAME_HEADER, NAGALAKSHMI_HEADER_ROW );
	vector <string> chrom_num_str = uORF_table.get_csv_column<string>( NAGALAKSHMI_CHROM_NUM_HEADER, NAGALAKSHMI_HEADER_ROW );


	if( gene_name.size() != chrom_num_str.size() )
	{
		return Errors.handle_error( PASS_UP_ONE_LEVEL, "Error in 'extract_gene_coord_Nagalakshmi()': Size of uORF data vectors didn't match. ", __FILE__, __LINE__ );
	}
	// ------------------------------
	
	
	
	unsigned int num_genes_not_found = 0;

	*uORF_data_start_it = ORF_Data->size();
	ORF_Data->reserve( ORF_Data->size() + gene_name.size() );
	

	for( unsigned int i = 0; i < gene_name.size(); i++ )
	{
		// ------------------------------
		// Find the uORF's chromosome number
		unsigned int chom_num_start_pos = chrom_num_str[i].find( "chr" ) + ( (string)"chr" ).size();
		if( chrom_num_str[i].find( "chr" ) == string::npos )
		{
			return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'extract_gene_coord_Nagalakshmi()': " +
																   "Could not find the start of the chromosome identifier in the chromosome number column of the Nagalakshmi table for gene " + 
																   gene_name[i], __FILE__, __LINE__ );
		}
		
		unsigned int chrom_num_end_pos = chrom_num_str[i].size() - 1;
		unsigned int chrom_num_len = chrom_num_end_pos - chom_num_start_pos + 1;
				
		unsigned int chrom_num = convert_roman_numeral_to_uint( chrom_num_str[i].substr( chom_num_start_pos, chrom_num_len ) );
		// ------------------------------
		
		
		
		// ------------------------------
		// Determine what DNA strand the uORF is on
		bool opposite_strand;
		if	   ( gene_name[i].find( 'W' ) != string::npos ) { opposite_strand = FALSE; }
		else if( gene_name[i].find( 'C' ) != string::npos ) { opposite_strand = TRUE;  }
		else
		{
			return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'extract_gene_coord_Nagalakshmi()': Gene " + gene_name[i] +
																   " could not be recognized as either residing on the Watson or Crick strand. ", __FILE__, __LINE__ );
		}
		// ------------------------------
		
		
		
		// ------------------------------
		// Retrieve the gene coordinates from the most recent annotations
		unsigned int gene_start_coord;
		unsigned int gene_end_coord;
		unsigned int gene_intergen_start_coord;
		vector <TFeature> untransl_reg;
		TFeature fpUTR_intron( string::npos, string::npos );
			
		if( get_gene_coord_from_gff( gene_name[i], 
									 gff_annotations[chrom_num - 1], 
									 opposite_strand, 
									 &gene_start_coord, 
									 &gene_end_coord, 
									 &gene_intergen_start_coord,
									 &untransl_reg, 
									 &fpUTR_intron ) )
		{
			num_genes_not_found++;
			Errors.handle_error( NONFATAL, "", __FILE__, __LINE__ );
		}
		else
		{
			// If the gene was successfully found in the .gff, add an entry to the ORF_Data vector
			ORF_Data->push_back( TORF_Data( chrom_num,
											gene_name[i], 
											gene_start_coord,
											abs( (int)gene_end_coord - (int)gene_start_coord + 1 ),
											gene_intergen_start_coord,
											0, 						// Old gene start position unknown, so gene position change assumed to be 0
											opposite_strand,
											untransl_reg,
											fpUTR_intron ) );
		}
		// ------------------------------
	}

	
	if( num_genes_not_found != 0 ) 
	{
		ostringstream error_oss;
		error_oss << "Error in 'extract_gene_coord_Nagalakshmi()': " << num_genes_not_found << " genes were not found.";
		return Errors.handle_error( PASS_UP_ONE_LEVEL, error_oss.str(), __FILE__, __LINE__ );
	}
	
	return SUCCESSFUL;
}
//==============================================================================



int extract_uORFs_Nagalakshmi( const TCSV_Contents & uORF_table, const vector <string> & yeast_chromosomes, vector <TORF_Data> *  const ORF_Data, const unsigned int   uORF_data_start_it )
{
	unsigned int num_uORFs = 0;
	
	unsigned int num_perfect_uORFs = 0;
	unsigned int num_perfect_uORFs_bc_realigned = 0;
	
	unsigned int num_misaligned_uORFs = 0;
	unsigned int num_realigned_uORFs  = 0;
	
	
	// ------------------------------
	// Read in (from the Nagalakshmi data table) the expected uORF lengths and the 5' UTR's
	vector <unsigned int> exp_uORF_len      	   = uORF_table.get_csv_column<unsigned int>( NAGALAKSHMI_uORF_LEN_HEADER,   	   NAGALAKSHMI_HEADER_ROW );
	vector <string> 	  five_prime_UTR_OLD 	   = uORF_table.get_csv_column<string>	    ( NAGALAKSHMI_5PRIME_UTR_HEADER, 	   NAGALAKSHMI_HEADER_ROW );
	vector <unsigned int> five_prime_UTR_start_OLD = uORF_table.get_csv_column<unsigned int>( NAGALAKSHMI_5PRIME_UTR_START_HEADER, NAGALAKSHMI_HEADER_ROW );
	
	vector <int> TSS_rel_pos( five_prime_UTR_OLD.size() );
	
	if( exp_uORF_len.size() != ( ORF_Data->size() - uORF_data_start_it ) )
	{
		return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'extract_uORFs_Nagalakshmi()': Size of uORF data vectors " +
															   "didn't match size of allotted increase in 'ORF_Data' capacity. ", __FILE__, __LINE__ );
	}
	
	
	for( unsigned int i = 0; i < exp_uORF_len.size(); i++ )
	{	
		unsigned int ORF_it = i + uORF_data_start_it;
		
		// Remove the gene's start codon from the end of the 5' UTR
		five_prime_UTR_OLD[i] = five_prime_UTR_OLD[i].substr( 0, five_prime_UTR_OLD[i].size() - 3 );
		
		// The data presents length as the number of codons excluding the start codon.  Therefore, to find the number of nucleotides
		//   in the uORF, add one and multiply by three
		exp_uORF_len[i] = 3 * ( exp_uORF_len[i] + 1 );
		
		
		// Get the relative position of the TSS by taking the negative of the 5' UTR length
		unsigned int five_prime_UTR_size_ALT_OLD = abs( (int)five_prime_UTR_start_OLD[i] - (int)ORF_Data->at( ORF_it ).gene_CDS.start_coord ) + 1;
		
		if( five_prime_UTR_size_ALT_OLD <= five_prime_UTR_OLD[i].size() )
		{
			TSS_rel_pos[i] = (-1) * five_prime_UTR_OLD[i].size();
		}
		else/*( five_prime_UTR_size_ALT_OLD > five_prime_UTR_OLD[i].size() )*/
		{
			TSS_rel_pos[i] = (-1) * five_prime_UTR_size_ALT_OLD;
			
			if( five_prime_UTR_size_ALT_OLD - five_prime_UTR_OLD[i].size() > 10 )
			{
				ostringstream error_oss;
				error_oss << "Error in 'extract_uORFs_Nagalakshmi()': Gene " 
						  << ORF_Data->at( ORF_it ).gene_name
						  << " shifted by " << five_prime_UTR_size_ALT_OLD - five_prime_UTR_OLD[i].size() 
						  << " since the time the data was published. ";
						  
				Errors.handle_error( NONFATAL, error_oss.str(), __FILE__, __LINE__ );
			}
		}
	}
	// ------------------------------
	
	
	
	// ------------------------------
	// Extract the uORFs from the chromosomes using the Nagalakshmi data
	for( unsigned int i = 0; i < exp_uORF_len.size(); i++ )
	{
		int rel_uORF_pos;
		unsigned int uORF_start_pos;

		unsigned int ORF_it = i + uORF_data_start_it;
		string chrom_seq = yeast_chromosomes[ORF_Data->at( ORF_it ).chrom_num - 1];
		
		
		
		// ------------------------------
		// Extract the current 5' UTR (assuming the same length as when the data was published), extended into the gene
		//  in order to account for uORFs that extend into the gene
		unsigned int gene_start_pos_it = ORF_Data->at( ORF_it ).gene_CDS.start_coord - 1;
		
		unsigned int UTR_first_coord = gene_start_pos_it + TSS_rel_pos[i];
		if( ORF_Data->at( ORF_it ).opposite_strand == TRUE ) { UTR_first_coord = gene_start_pos_it + 1 /*- NUM_NT_PAST_GENE_START*/; }
		
		unsigned int UTR_length = (-1) * TSS_rel_pos[i] /*+ NUM_NT_PAST_GENE_START*/;
		
		if( UTR_first_coord + UTR_length - 1 > chrom_seq.size() )
		{
			UTR_length = chrom_seq.size() - UTR_first_coord + 1;
		}
		
		unsigned int start_coord;
		if( ORF_Data->at( ORF_it ).opposite_strand == FALSE )
		{
			start_coord = UTR_first_coord + 1;
		}
		else/*( ORF_Data->at( ORF_it ).opposite_strand == TRUE )*/
		{
			start_coord = UTR_first_coord + 1 + ( UTR_length - 1 );
		}
			
		string five_prime_UTR = extract_DNA_seq( chrom_seq, start_coord, UTR_length,  ORF_Data->at( ORF_it ).opposite_strand );
		// ------------------------------
		
		
		
		// ------------------------------
		// Find the expected uORF sequence in the 5' UTR by finding all upstream ATG's and checking for the correct length
		vector <unsigned int> poss_uORF_start_pos_in_UTR( 0 );
		unsigned int star_pos_to_search = 0;
		
		
		do
		{
			unsigned int poss_uORF_start_pos = five_prime_UTR.find( "ATG", star_pos_to_search );
			
			if( poss_uORF_start_pos < five_prime_UTR.size() )
			{
				poss_uORF_start_pos_in_UTR.push_back( poss_uORF_start_pos );
				star_pos_to_search = poss_uORF_start_pos + 1;
			}
			else
			{
				star_pos_to_search = five_prime_UTR.size();
			}
		
		} while( star_pos_to_search <= five_prime_UTR.size() - 3 );

		
		
		// Attempt to find the true uORF
		vector <unsigned int> poss_uORF_len( poss_uORF_start_pos_in_UTR.size(), FALSE );
		unsigned int num_poss_length_matches = 0;

		
		for( unsigned int j = 0; j < poss_uORF_start_pos_in_UTR.size(); j++ )
		{
			/* Previous Method of determining if the correct upstream ATG was found
			if( ( poss_uORF_start_pos_in_UTR[j] + ( exp_uORF_len[i] - 1 ) ) >= five_prime_UTR.size() )
			{
				ostringstream error_oss;
				error_oss << "Error in 'extract_uORFs_Nagalakshmi()': Expected uORF length causes the potential uORF located at relative position " 
						  << (-1) * ( (int)five_prime_UTR.size() - (int)poss_uORF_start_pos_in_UTR[j] )
						  << " to the gene "
						  << ORF_Data->at( ORF_it ).gene_name
						  << " to exceed the length of the searched string. ";
						  
				return Errors.handle_error( PASS_UP_ONE_LEVEL, error_oss.str(), __FILE__, __LINE__ );
			}
			*/
			
			rel_uORF_pos = (-1) * ( five_prime_UTR.size() - poss_uORF_start_pos_in_UTR[j] );
		
			uORF_start_pos = ORF_Data->at( ORF_it ).gene_CDS.start_coord - ( five_prime_UTR.size() - poss_uORF_start_pos_in_UTR[j] );
			if( ORF_Data->at( ORF_it ).opposite_strand == TRUE ) 
			{ 
				uORF_start_pos = ORF_Data->at( ORF_it ).gene_CDS.start_coord + ( five_prime_UTR.size() - poss_uORF_start_pos_in_UTR[j] );
			}
		
			unsigned int uORF_end_pos = find_first_stop_codon( chrom_seq, uORF_start_pos, ORF_Data->at( ORF_it ).opposite_strand );
			poss_uORF_len[j] = (unsigned int)( abs( (int)uORF_end_pos - (int)uORF_start_pos ) + 1 );
			
			if( poss_uORF_len[j] == exp_uORF_len[i] )
			{
				num_poss_length_matches++;
			}
			
			
			/* Previous Method of determining if the correct upstream ATG was found
			string poss_stop_codon = five_prime_UTR_ext.substr( ( poss_uORF_start_pos_in_UTR[j] + ( exp_uORF_len[i] - 1 ) - NUM_NT_TO_COMPLETE_CODON ), 3 );
			if( poss_stop_codon == "TAA" || poss_stop_codon == "TGA" || poss_stop_codon == "TAG" )
			{
				poss_uORF_len_matches[j] = TRUE;
				num_poss_length_matches++;
			}
			*/
		}
		// ------------------------------
		
		
		
		// ------------------------------
		// Check for errors
		if( poss_uORF_start_pos_in_UTR.size() == 0 )
		{
			Errors.handle_error( NONFATAL, "Error in 'extract_uORFs_Nagalakshmi()': No upstream ATG's were found. ", __FILE__, __LINE__ );
		}
		/*else if( num_poss_length_matches > 1 )
		{
			ostringstream error_oss;
			error_oss << "Error in 'extract_uORFs_Nagalakshmi()': Multiple uORFs of the expected length were found in the 5' UTR for gene " 
					  << ORF_Data->at( ORF_it ).gene_name
					  << ". The correct uORF cannot be resolved. ";
					  
			Errors.handle_error( NONFATAL, error_oss.str(), __FILE__, __LINE__ );
		}*/
		else if( poss_uORF_start_pos_in_UTR.size() > 1 && num_poss_length_matches == 0 )
		{
			ostringstream error_oss;
			error_oss << "Error in 'extract_uORFs_Nagalakshmi()': Multiple possible uORFs were found in the 5' UTR for gene " 
					  << ORF_Data->at( ORF_it ).gene_name
					  << ", but none were of the expected length.  The correct uORF cannot be resolved. ";
					  
			Errors.handle_error( NONFATAL, error_oss.str(), __FILE__, __LINE__ );
		}
		// ------------------------------
		
		else
		{	
			// ------------------------------
			// If there was only one upstream ATG found, and it wasn't part of a uORF of the expected length, output a warning
			//  message, but still add it to the list
			if( poss_uORF_start_pos_in_UTR.size() == 1 && num_poss_length_matches == 0 ) 
			{
				ostringstream error_oss;
				error_oss << "Error in 'extract_uORFs_Nagalakshmi()': One possible uORF was found in the 5' UTR for gene " 
						  << ORF_Data->at( ORF_it ).gene_name
						  << ", but it's length (" << poss_uORF_len[0]
						  << " nt) did not match what was expected (" << exp_uORF_len[i] 
						  << " nt).  It is assumed that this was nonetheless the uORF referred to. ";
					  
				Errors.handle_error( NONFATAL, error_oss.str(), __FILE__, __LINE__ );
			}
			else
			{
				for( unsigned int poss_uORF_it = 0; poss_uORF_it <  poss_uORF_start_pos_in_UTR.size(); poss_uORF_it++ )
				{
					if( poss_uORF_len[poss_uORF_it] != exp_uORF_len[i] )
					{
						poss_uORF_len.erase				( poss_uORF_len.begin() 			 + poss_uORF_it );
						poss_uORF_start_pos_in_UTR.erase( poss_uORF_start_pos_in_UTR.begin() + poss_uORF_it );
						
						poss_uORF_it--;
					}
				}
			}
			// ------------------------------
			
			

			for( unsigned int uORF_it = 0; uORF_it < poss_uORF_start_pos_in_UTR.size(); uORF_it++ )
			{
				// ------------------------------
				// Extract the relevant information from the Zhang Dietrich Nucleic Acid Research table; Then, Extract the uORF and manipulate it as necessary
				rel_uORF_pos = (-1) * ( (-1) * TSS_rel_pos[i] - poss_uORF_start_pos_in_UTR[uORF_it] );
				
				if( ORF_Data->at( ORF_it ).extract_uORF( rel_uORF_pos,
														 exp_uORF_len[i],
														 "ATG",
														 chrom_seq,
														 DATA_SOURCE_NAGALAKSHMI,
														 &num_realigned_uORFs,
														 &num_misaligned_uORFs ) )
				{
					return Errors.handle_error( PASS_UP_ONE_LEVEL, "Error in 'extract_uORFs_Nagalakshmi()': ", __FILE__, __LINE__ );
				}
				

				
				// Define a "perfect uORF" as one that was found as documented in the data table (except small alignment changes),
				//   and doesn't extend past the gene's start
				if( ORF_Data->at( ORF_it ).get_uORFs().back().start_codon 		  == TRUE && 
					ORF_Data->at( ORF_it ).get_uORFs().back().num_nt_len_changed  == 0    && 
					ORF_Data->at( ORF_it ).get_uORFs().back().end_past_gene_start == FALSE )
				{
					num_perfect_uORFs++;
					if( ORF_Data->at( ORF_it ).get_uORFs().back().num_nt_start_moved != 0 ) { num_perfect_uORFs_bc_realigned++; }
				}
				
				num_uORFs++;
			}
		}
	}
	// ------------------------------
	
	
	
	// ------------------------------
	// Output a summary of the collected uORFs
	ostringstream output_oss;
	output_oss << "num uORFs:                 " << num_uORFs                      << "\n"
		       
			   << "num_misaligned_uORFs:      " << num_misaligned_uORFs           << "\n"
			   << "num_realigned_uORFs:       " << num_realigned_uORFs            << "\n"
			   
			   << "num_perfect_uORFs:         " << num_perfect_uORFs              << "\n"
			   << "num_perfects_bc_realigned: " << num_perfect_uORFs_bc_realigned << "\n";
	
	output_text_line( output_oss.str() );
	// ------------------------------

	return SUCCESSFUL;
}
//==============================================================================



int extract_gene_coord_general( const TCSV_Contents & uORF_table, 
								const vector <string> & gff_annotations, 
								vector < TORF_Data > * const ORF_Data, 
								unsigned int * const uORF_data_start_it )
{
	// ------------------------------
	// Read in (from the passed data table) the gene names (note that chromosome numbers are assumed to not be provided
	vector <string> gene_name = uORF_table.get_csv_column<string> ( GENERAL_SYST_NAME_HEADER );
	// ------------------------------
	
	
	unsigned int num_genes_not_found = 0;

	*uORF_data_start_it = ORF_Data->size();
	ORF_Data->reserve( ORF_Data->size() + gene_name.size() );
	
	bool prev_gene_not_found = FALSE;

	for( unsigned int i = 0; i < gene_name.size(); i++ )
	{	
		// For some files, only the first row of a gene's uORF list contains the gene name
		if( gene_name[i] == "" || ( i > 0 && gene_name[i] == gene_name[i - 1] ) )
		{ 
			if( i == 0 )
			{	  
				return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'extract_gene_coord_general()': " +
																	   "First non-header row of the data table did not contain a gene name", __FILE__, __LINE__ );
			}
			
			
			if( prev_gene_not_found == FALSE )
			{
				ORF_Data->push_back( TORF_Data( ORF_Data->back().chrom_num,
												ORF_Data->back().gene_name,  
												ORF_Data->back().gene_CDS.start_coord,
												ORF_Data->back().gene_CDS.length,
												ORF_Data->back().gene_intergen_start_coord,
												ORF_Data->back().gene_pos_change, 	
												ORF_Data->back().opposite_strand,
												ORF_Data->back().untransl_reg, 
												ORF_Data->back().fpUTR_intron ) );
			}
		}
		else
		{
			// ------------------------------
			// Find the uORF's chromosome number (The second letter of the systematic name indicates the chromosome number, 
			//  with 'A' corresponding to chr01, 'B' to chr02, and so on
			unsigned char chrom_letter = gene_name[i][1];
			unsigned int chrom_num 	   = chrom_letter - 'A' + 1;
			
			/*
			unsigned int chrom_num_it = 0;
			
			do
			{
				if( gff_annotations[chrom_num_it].find( (string)"ID=" + gene_name[i] ) != string::npos )
				{
					chrom_num = chrom_num_it + 1;
				}
				
				chrom_num_it++;
				
			} while( chrom_num_it < gff_annotations.size() && chrom_num == 0 );
			
			
			// This yielded no errors, so it was determined to be safe to stop searching once the gene
			//    is found in one chromosome
			for( unsigned int chrom_num_it = 0; chrom_num_it < gff_annotations.size(); chrom_num_it++ )
			{
				if( gff_annotations[chrom_num_it].find( (string)"ID=" + gene_name[i] ) != string::npos )
				{
					if( chrom_num != 0 )
					{
						ostringstream error_oss;
						error_oss << "Error in 'extract_gene_coord_general()': Gene " << gene_name[i]
								  << " was found in both chromosome "
								  << chrom_num << " and " << chrom_num_it + 1
								  << ". The correct chromosome number cannot be resolved. ";

						return Errors.handle_error( PASS_UP_ONE_LEVEL, error_oss.str(), __FILE__, __LINE__ );
					}
					else
					{
						chrom_num = chrom_num_it + 1;
					}
				}
			}
			
			
			
			if( chrom_num == 0 )
			{
				ostringstream error_oss;
				error_oss << "Error in 'extract_gene_coord_general()': Gene " << gene_name[i] 
						  << " was not found in any chromosome in the GFF. ";
						  
				return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'extract_gene_coord_general()': Gene " + gene_name[i] +
																	   " was not found in any chromosome in the GFF. ", __FILE__, __LINE__ );
			}*/
			// ------------------------------
			
			
			
			// ------------------------------
			// Determine what DNA strand the uORF is on
			bool opposite_strand;
			if	   ( gene_name[i].find( 'W' ) != string::npos ) { opposite_strand = FALSE; }
			else if( gene_name[i].find( 'C' ) != string::npos ) { opposite_strand = TRUE;  }
			else
			{
				return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'extract_gene_coord_general()': Gene " + gene_name[i] +
																	   " could not be recognized as either residing on the Watson or Crick strand. ", __FILE__, __LINE__ );
			}
			// ------------------------------
			
			
			
			// ------------------------------
			// Retrieve the gene coordinates from the most recent annotations
			unsigned int gene_start_coord;
			unsigned int gene_end_coord;
			unsigned int gene_intergen_start_coord;
			vector <TFeature> untransl_reg;
			TFeature fpUTR_intron( string::npos, string::npos );
				
			if( get_gene_coord_from_gff( gene_name[i], 
										 gff_annotations[chrom_num - 1], 
										 opposite_strand, 
										 &gene_start_coord, 
										 &gene_end_coord, 
										 &gene_intergen_start_coord,
										 &untransl_reg, 
										 &fpUTR_intron ) )
			{
				num_genes_not_found++;
				prev_gene_not_found = TRUE;
				Errors.handle_error( NONFATAL, "", __FILE__, __LINE__ );
			}
			else
			{
				// If the gene was successfully found in the .gff, add an entry to the ORF_Data vector
				ORF_Data->push_back( TORF_Data( chrom_num,
												gene_name[i], 
												gene_start_coord,
												abs( (int)gene_end_coord - (int)gene_start_coord + 1 ),
												gene_intergen_start_coord,
												0, 						// Old gene start position unknown, so gene position change assumed to be 0
												opposite_strand,
												untransl_reg,
												fpUTR_intron ) );
												
				prev_gene_not_found = FALSE;
			}
			// ------------------------------
		}
	}

	
	if( num_genes_not_found != 0 ) 
	{
		ostringstream error_oss;
		error_oss << "Error in 'extract_gene_coord_general()': " << num_genes_not_found << " genes were not found.";
		return Errors.handle_error( PASS_UP_ONE_LEVEL, error_oss.str(), __FILE__, __LINE__ );
	}
	
	return SUCCESSFUL;
}
//==============================================================================



int extract_uORFs_Cvijovic( const TCSV_Contents & uORF_table, const vector <string> & yeast_chromosomes, vector <TORF_Data> * const ORF_Data, const unsigned int uORF_data_start_it )
{
	unsigned int num_uORFs = 0;
	
	unsigned int num_perfect_uORFs = 0;
	unsigned int num_perfect_uORFs_bc_realigned = 0;
	
	unsigned int num_misaligned_uORFs = 0;
	unsigned int num_realigned_uORFs  = 0;
	
	
	// ------------------------------
	// Read in (from the Cvijovic data table) the expected uORF lengths and distances from uORF end to the start of the CDS
	vector <unsigned int> exp_uORF_len = uORF_table.get_csv_column<unsigned int>( GENERAL_uORF_LENGTH_HEADER   );
	vector <int> 		  rel_uORF_pos = uORF_table.get_csv_column<int>		    ( CVIJOVIC_RELATIVE_POS_HEADER );
	
	if( exp_uORF_len.size()!= ( ORF_Data->size() - uORF_data_start_it ) )
	{
		return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'extract_uORFs_Cvijovic()': Size of uORF data vectors " +
															   "didn't match size of allotted increase in 'ORF_Data' capacity. ", __FILE__, __LINE__ );
	}
	// ------------------------------
	
	
	
	// ------------------------------
	// Extract the uORFs from the chromosomes using the Cvijovic data
	for( unsigned int i = 0; i < exp_uORF_len.size(); i++ )
	{	
		// Length is listed in codons (excluding the stop codon); convert to nucleotides
		exp_uORF_len[i] = 3 * ( exp_uORF_len[i] + 1);
		
		// For some reason, relative position is nonstandard ( [Standard Relative Position] = [Cvijovic Relative Position] + 1 )
		rel_uORF_pos[i] += 1;
		
		
		unsigned int ORF_it = i + uORF_data_start_it;
		string chrom_seq = yeast_chromosomes[ORF_Data->at( ORF_it ).chrom_num - 1];
		

		// Extract the uORF and manipulate it as necessary
		if( ORF_Data->at( ORF_it ).extract_uORF( rel_uORF_pos[i],
												 exp_uORF_len[i],
												 "ATG",
												 chrom_seq,
												 DATA_SOURCE_CVIJOVIC,
												 &num_realigned_uORFs,
												 &num_misaligned_uORFs ) )
		{
			return Errors.handle_error( PASS_UP_ONE_LEVEL, "Error in 'extract_uORFs_Cvijovic()': ", __FILE__, __LINE__ );
		}
		

		
		// Define a "perfect uORF" as one that was found as documented in the data table (except small alignment changes),
		//   and doesn't extend past the gene's start
		if( ORF_Data->at( ORF_it ).get_uORFs().back().start_codon 		  == TRUE && 
			ORF_Data->at( ORF_it ).get_uORFs().back().num_nt_len_changed  == 0    && 
			ORF_Data->at( ORF_it ).get_uORFs().back().end_past_gene_start == FALSE )
		{
			num_perfect_uORFs++;
			if( ORF_Data->at( ORF_it ).get_uORFs().back().num_nt_start_moved != 0 ) { num_perfect_uORFs_bc_realigned++; }
		}
		
		num_uORFs++;
	}
	// ------------------------------
	
	
	
	// ------------------------------
	// Output a summary of the collected uORFs
	ostringstream output_oss;
	output_oss << "num uORFs:                 " << num_uORFs                      << "\n"
		       
			   << "num_misaligned_uORFs:      " << num_misaligned_uORFs           << "\n"
			   << "num_realigned_uORFs:       " << num_realigned_uORFs            << "\n"
			   
			   << "num_perfect_uORFs:         " << num_perfect_uORFs              << "\n"
			   << "num_perfects_bc_realigned: " << num_perfect_uORFs_bc_realigned << "\n";
	
	output_text_line( output_oss.str() );
	// ------------------------------

	return SUCCESSFUL;
}
//==============================================================================



int extract_uORFs_Guan( const TCSV_Contents & uORF_table, const vector <string> & yeast_chromosomes, vector <TORF_Data> * const ORF_Data, const unsigned int uORF_data_start_it )
{
	unsigned int num_uORFs = 0;
	
	unsigned int num_perfect_uORFs = 0;
	unsigned int num_perfect_uORFs_bc_realigned = 0;
	
	unsigned int num_misaligned_uORFs = 0;
	unsigned int num_realigned_uORFs  = 0;
	
	
	// ------------------------------
	// Read in (from the Guan data table) the expected uORF lengths, relative positions, expected start contexts, and AUGCAI's
	vector <unsigned int> exp_uORF_len 			 = uORF_table.get_csv_column<unsigned int>( GUAN_uORF_LENGTH_HEADER   );
	vector <int> 		  rel_uORF_pos 			 = uORF_table.get_csv_column<int>		  ( GUAN_uORF_START_HEADER 	  );
	vector <string> 	  exp_uORF_start_context = uORF_table.get_csv_column<string>	  ( GUAN_START_CONTEXT_HEADER );
	vector <double> 	  AUGCAI_val 			 = uORF_table.get_csv_column<double>	  ( GUAN_AUGCAI_HEADER 		  );
	
	if( exp_uORF_len.size()!= ( ORF_Data->size() - uORF_data_start_it ) )
	{
		return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'extract_uORFs_Guan()': Size of uORF data vectors " +
															   "didn't match size of allotted increase in 'ORF_Data' capacity. ", __FILE__, __LINE__ );
	}
	// ------------------------------
	
	
	
	// ------------------------------
	// Extract the uORFs from the chromosomes using the Guan data
	for( unsigned int i = 0; i < exp_uORF_len.size(); i++ )
	{	
		unsigned int ORF_it = i + uORF_data_start_it;
		string chrom_seq = yeast_chromosomes[ORF_Data->at( ORF_it ).chrom_num - 1];
		

		// Extract the uORF and manipulate it as necessary
		if( ORF_Data->at( ORF_it ).extract_uORF( rel_uORF_pos[i],
												 exp_uORF_len[i],
												 "ATG",
												 chrom_seq,
												 DATA_SOURCE_GUAN,
												 &num_realigned_uORFs,
												 &num_misaligned_uORFs,
												 exp_uORF_start_context[i] ) )
		{
			return Errors.handle_error( PASS_UP_ONE_LEVEL, "Error in 'extract_uORFs_Guan()': ", __FILE__, __LINE__ );
		}
		

		
		// Define a "perfect uORF" as one that was found as documented in the data table (except small alignment changes),
		//   and doesn't extend past the gene's start
		if( ORF_Data->at( ORF_it ).get_uORFs().back().start_codon 		  == TRUE && 
			ORF_Data->at( ORF_it ).get_uORFs().back().num_nt_len_changed  == 0    && 
			ORF_Data->at( ORF_it ).get_uORFs().back().end_past_gene_start == FALSE )
		{
			num_perfect_uORFs++;
			if( ORF_Data->at( ORF_it ).get_uORFs().back().num_nt_start_moved != 0 ) { num_perfect_uORFs_bc_realigned++; }
		}
		
		num_uORFs++;
	}
	// ------------------------------
	
	
	
	// ------------------------------
	// Output a summary of the collected uORFs
	ostringstream output_oss;
	output_oss << "num uORFs:                 " << num_uORFs                      << "\n"
		       
			   << "num_misaligned_uORFs:      " << num_misaligned_uORFs           << "\n"
			   << "num_realigned_uORFs:       " << num_realigned_uORFs            << "\n"
			   
			   << "num_perfect_uORFs:         " << num_perfect_uORFs              << "\n"
			   << "num_perfects_bc_realigned: " << num_perfect_uORFs_bc_realigned << "\n";
	
	output_text_line( output_oss.str() );
	// ------------------------------

	return SUCCESSFUL;
}
//==============================================================================



int extract_uORFs_Lawless( const TCSV_Contents & uORF_table, const vector <string> & yeast_chromosomes, vector <TORF_Data> * const ORF_Data, const unsigned int uORF_data_start_it )
{
	unsigned int num_uORFs = 0;
	
	unsigned int num_perfect_uORFs = 0;
	unsigned int num_perfect_uORFs_bc_realigned = 0;
	
	unsigned int num_misaligned_uORFs = 0;
	unsigned int num_realigned_uORFs  = 0;

	
	// ------------------------------
	// Read in (from the Lawless data table) the expected uORF lengths, relative positions, expected start contexts, and AUGCAI's
	vector <unsigned int> exp_uORF_len 		= uORF_table.get_csv_column<unsigned int>( LAWLESS_uORF_LENGTH_HEADER   );
	vector <int> 		  rel_uORF_pos 		= uORF_table.get_csv_column<int>		 ( LAWLESS_uORF_START_HEADER    );
	vector <string> 	  exp_uORF_sequence = uORF_table.get_csv_column<string>	  	 ( LAWLESS_uORF_SEQUENCE_HEADER );
	
	if( exp_uORF_len.size() != ( ORF_Data->size() - uORF_data_start_it ) )
	{
		return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'extract_uORFs_Lawless()': Size of uORF data vectors " + 
															   "didn't match size of allotted increase in 'ORF_Data' capacity. ", __FILE__, __LINE__ );
	}
	// ------------------------------
	

	
	// ------------------------------
	// Extract the uORFs from the chromosomes using the Lawless data
	for( unsigned int i = 0; i < exp_uORF_len.size(); i++ )
	{	
		unsigned int ORF_it = i + uORF_data_start_it;
		string chrom_seq = yeast_chromosomes[ORF_Data->at( ORF_it ).chrom_num - 1];
		

		// Extract the uORF and manipulate it as necessary
		if( ORF_Data->at( ORF_it ).extract_uORF( rel_uORF_pos[i],
												 exp_uORF_len[i],
												 "ATG",
												 chrom_seq,
												 DATA_SOURCE_LAWLESS,
												 &num_realigned_uORFs,
												 &num_misaligned_uORFs ) )
		{
			return Errors.handle_error( PASS_UP_ONE_LEVEL, "Error in 'extract_uORFs_Lawless()': ", __FILE__, __LINE__ );
		}
		
		
		
		// If the uORF was correctly found, make sure the start context matches what was expected
		if( ORF_Data->at( ORF_it ).get_uORFs().back().start_codon == TRUE )
		{
			// Allow for one substitution in the start codon context
			if( ORF_Data->at( ORF_it ).get_uORFs().back().content.substr( 0, NUM_NT_MATCH_uORF_SEQUENCE ) != 
											 exp_uORF_sequence[i].substr( 0, NUM_NT_MATCH_uORF_SEQUENCE ) )
			{
				ostringstream error_oss;
				error_oss << "Error in 'extract_uORFs_Lawless()': For uORF located at "
						  << rel_uORF_pos[i] 
						  << " relative to gene " 
						  << ORF_Data->at( ORF_it ).gene_name 
						  << " the first two codons ("
						  << ORF_Data->at( ORF_it ).get_uORFs().back().content.substr( 0, START_CONTEXT_NT_AFTER_AUG + 1 )
						  << ") did not match what was expected ("
						  << exp_uORF_sequence[i].substr( 0, START_CONTEXT_NT_AFTER_AUG + 1 )
						  << "). ";
						  
				/*return*/ Errors.handle_error( NONFATAL/*PASS_UP_ONE_LEVEL*/, error_oss.str(), __FILE__, __LINE__ );
			}
		}
		

		
		// Define a "perfect uORF" as one that was found as documented in the data table (except small alignment changes),
		//   and doesn't extend past the gene's start
		if( ORF_Data->at( ORF_it ).get_uORFs().back().start_codon 		  == TRUE && 
			ORF_Data->at( ORF_it ).get_uORFs().back().num_nt_len_changed  == 0    && 
			ORF_Data->at( ORF_it ).get_uORFs().back().end_past_gene_start == FALSE )
		{
			num_perfect_uORFs++;
			if( ORF_Data->at( ORF_it ).get_uORFs().back().num_nt_start_moved != 0 ) { num_perfect_uORFs_bc_realigned++; }
		}
		
		num_uORFs++;
	}
	// ------------------------------
	
	
	
	// ------------------------------
	// Output a summary of the collected uORFs
	ostringstream output_oss;
	output_oss << "num uORFs:                 " << num_uORFs                      << "\n"
		       
			   << "num_misaligned_uORFs:      " << num_misaligned_uORFs           << "\n"
			   << "num_realigned_uORFs:       " << num_realigned_uORFs            << "\n"
			   
			   << "num_perfect_uORFs:         " << num_perfect_uORFs              << "\n"
			   << "num_perfects_bc_realigned: " << num_perfect_uORFs_bc_realigned << "\n";
	
	output_text_line( output_oss.str() );
	// ------------------------------

	return SUCCESSFUL;
}
//==============================================================================



int extract_uORFs_Selpi( const TCSV_Contents & uORF_table, const vector <string> & yeast_chromosomes, vector <TORF_Data> * const ORF_Data, const unsigned int uORF_data_start_it )
{
	unsigned int num_uORFs = 0;
	
	unsigned int num_perfect_uORFs = 0;
	unsigned int num_perfect_uORFs_bc_realigned = 0;
	
	unsigned int num_misaligned_uORFs = 0;
	unsigned int num_realigned_uORFs  = 0;
	
	
	// ------------------------------
	// Read in (from the Selpi data table) the expected uORF lengths, relative positions, expected start contexts, and AUGCAI's
	vector <unsigned int> exp_uORF_len 			 = uORF_table.get_csv_column<unsigned int>( SELPI_uORF_LENGTH_HEADER     );
	vector <int> 		  rel_uORF_pos 			 = uORF_table.get_csv_column<int>		  ( SELPI_DISTANCE_TO_CDS_HEADER );
	
	if( exp_uORF_len.size()!= ( ORF_Data->size() - uORF_data_start_it ) )
	{
		return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'extract_uORFs_Selpi()': Size of uORF data vectors " +
															   "didn't match size of allotted increase in 'ORF_Data' capacity. ", __FILE__, __LINE__ );
	}
	// ------------------------------
	
	
	
	// ------------------------------
	// Extract the uORFs from the chromosomes using the Selpi data
	for( unsigned int i = 0; i < exp_uORF_len.size(); i++ )
	{	
		// uORF length listed as number of codons, including start and stop
		exp_uORF_len[i] *= 3;
		
		// "Distance to CDS" is the number of codons between uORF start and CDS start.  Therefore, the uORF's relative position
		//   is the multiplicative inverse of this
		rel_uORF_pos[i] *= (-1);
		
		
		unsigned int ORF_it = i + uORF_data_start_it;
		string chrom_seq = yeast_chromosomes[ORF_Data->at( ORF_it ).chrom_num - 1];
		

		// Extract the uORF and manipulate it as necessary
		if( ORF_Data->at( ORF_it ).extract_uORF( rel_uORF_pos[i],
												 exp_uORF_len[i],
												 "ATG",
												 chrom_seq,
												 DATA_SOURCE_SELPI,
												 &num_realigned_uORFs,
												 &num_misaligned_uORFs ) )
		{
			return Errors.handle_error( PASS_UP_ONE_LEVEL, "Error in 'extract_uORFs_Selpi()': ", __FILE__, __LINE__ );
		}
		
		
		
		// Define a "perfect uORF" as one that was found as documented in the data table (except small alignment changes),
		//   and doesn't extend past the gene's start
		if( ORF_Data->at( ORF_it ).get_uORFs().back().start_codon 		  == TRUE && 
			ORF_Data->at( ORF_it ).get_uORFs().back().num_nt_len_changed  == 0    && 
			ORF_Data->at( ORF_it ).get_uORFs().back().end_past_gene_start == FALSE )
		{
			num_perfect_uORFs++;
			if( ORF_Data->at( ORF_it ).get_uORFs().back().num_nt_start_moved != 0 ) { num_perfect_uORFs_bc_realigned++; }
		}	

		num_uORFs++;		
	}
	// ------------------------------
	
	
	
	// ------------------------------
	// Output a summary of the collected uORFs
	ostringstream output_oss;
	output_oss << "num uORFs:                 " << num_uORFs                      << "\n"
		       
			   << "num_misaligned_uORFs:      " << num_misaligned_uORFs           << "\n"
			   << "num_realigned_uORFs:       " << num_realigned_uORFs            << "\n"
			   
			   << "num_perfect_uORFs:         " << num_perfect_uORFs              << "\n"
			   << "num_perfects_bc_realigned: " << num_perfect_uORFs_bc_realigned << "\n";
	
	output_text_line( output_oss.str() );
	// ------------------------------

	return SUCCESSFUL;
}
//==============================================================================



void realign_uORF( const string & chrom_seq,
				   const string & gene_name,
				   const bool opposite_strand,
				   const string & exp_start_codon,
				   const unsigned int uORF_len,
				   int * const rel_uORF_pos,
				   int * const uORF_first_coord_in_chrom_it,
				   string * const uORF_content,
				   int * const num_nt_start_moved,
				   bool * const start_codon_found  )
{
	// ------------------------------
	// Prepare for the search by adding the uORF's info to the error message
	ostringstream error_oss;
	error_oss << "uORF not properly found or aligned\n"
			  << "gene: "     << gene_name     << "\n"
			  << "uORF pos: " << *rel_uORF_pos  << "\n"
			  << "uORF len: " << uORF_len      << "\n"
			  << "uORF    : " << *uORF_content << "\n\n";
				  

	error_oss << "Attempting to manually align by searching +/- "<< NUM_NT_TO_SEARCH_AROUND_EXP << " nucleotides from the expected start codon..." << "\n";
	// ------------------------------


	
	// ------------------------------	
	// Obtain the search window by extracting a DNA sequence a few basepairs up/downstream of the expected start coordinate
	int chrom_start_pos_to_search;
	int chrom_end_pos_to_search;  


	string search_window;
	unsigned int chrom_it_search_window_it_diff;
	unsigned int search_window_it = 0;

	if( opposite_strand == FALSE )
	{
		chrom_start_pos_to_search = *uORF_first_coord_in_chrom_it - NUM_NT_TO_SEARCH_AROUND_EXP;
		chrom_end_pos_to_search   = *uORF_first_coord_in_chrom_it + NUM_NT_TO_SEARCH_AROUND_EXP + NUM_NT_TO_COMPLETE_CODON;
	}
	else/* opposite_strand == TRUE )*/
	{
		int uORF_last_coord_in_chrom_it = *uORF_first_coord_in_chrom_it + uORF_len - 1;

		chrom_start_pos_to_search = uORF_last_coord_in_chrom_it - NUM_NT_TO_SEARCH_AROUND_EXP - NUM_NT_TO_COMPLETE_CODON;							  
		chrom_end_pos_to_search   = uORF_last_coord_in_chrom_it + NUM_NT_TO_SEARCH_AROUND_EXP;
	}

	
	// If the search window extends beyond the chromosome, reduce its size accordingly
	if( chrom_start_pos_to_search < 0 )
	{
		error_oss << "Chromosome too small to search " 
				  << NUM_NT_TO_SEARCH_AROUND_EXP 
				  << " nucleotides before expected uORF start.  Instead starting search at chromosome start (" 
				  << *uORF_first_coord_in_chrom_it 
				  << " base pairs before the expected start)\n";
		
		chrom_start_pos_to_search = 0;
	}

	if( chrom_end_pos_to_search >= (int)chrom_seq.size() )
	{
		error_oss << "Chromosome too small to search " 
				  << NUM_NT_TO_SEARCH_AROUND_EXP 
				  << " nucleotides after expected uORF start.  Instead ending search at chromosome end (" 
				  << ( chrom_seq.size() - *uORF_first_coord_in_chrom_it - 1 ) 
				  << " base pairs after the expected end)\n";
		
		chrom_end_pos_to_search = chrom_seq.size() - 1;
	}

	search_window = chrom_seq.substr( chrom_start_pos_to_search, ( chrom_end_pos_to_search - chrom_start_pos_to_search + 1 ) );


	if( opposite_strand == FALSE )
	{
		chrom_it_search_window_it_diff = chrom_start_pos_to_search;
	}
	else
	{
		chrom_it_search_window_it_diff = ( chrom_start_pos_to_search - uORF_len + 1 ) + NUM_NT_TO_COMPLETE_CODON;
		
		search_window = get_inverted_sequence( search_window );
		search_window = get_opposite_strand  ( search_window );
	}
		

	error_oss << "Search Window: " << search_window << "\n";
	// ------------------------------	
	
	
	
	// ------------------------------	
	// Find all start codons in the search window
	vector <unsigned int> start_codon_pos_in_window( 0 );
	bool end_of_window_reached = FALSE;

	do
	{
		if( search_window.find( exp_start_codon, search_window_it ) != string::npos )
		{
			start_codon_pos_in_window.push_back( search_window.find( exp_start_codon, search_window_it ) );
			search_window_it = start_codon_pos_in_window.back() + 1;
		}
		else
		{
			end_of_window_reached = TRUE;
		}

	} while( end_of_window_reached == FALSE );
	// ------------------------------	
	
	
	
	// ------------------------------	
	// If any start codons were found, adjust the uORFs start position accordingly.  Otherwise, do nothing
	if( start_codon_pos_in_window.size() >= 1 )
	{
		unsigned int new_uORF_start_pos_it = start_codon_pos_in_window[0] + chrom_it_search_window_it_diff;
		
		
		if( start_codon_pos_in_window.size() > 1 )
		{
			// In case more than one start codons were found in the window, first check to see if any of the start codons produce a uORF
			//  of the expected length
			unsigned int num_poss_length_matches = 0;
			unsigned int temp_new_uORF_start_pos_it = 0;
			
			for( unsigned int i = 0; i < start_codon_pos_in_window.size(); i++ )
			{
				unsigned int temp_first_coord_in_chrom_it = *uORF_first_coord_in_chrom_it;
				int temp_num_nt_start_moved = (int)( start_codon_pos_in_window[i] + chrom_it_search_window_it_diff ) - (int)temp_first_coord_in_chrom_it;
				

				if( opposite_strand == FALSE )
				{
					temp_first_coord_in_chrom_it = start_codon_pos_in_window[i] + chrom_it_search_window_it_diff;
				}
				else/*( opposite_strand == TRUE )*/
				{
					// Since 'new_uORF_start_pos_it' moved in the opposite direction, need to compensate by adding
					//   twice the number of nucleotides the start position moved
					temp_first_coord_in_chrom_it = ( start_codon_pos_in_window[i] + chrom_it_search_window_it_diff ) - ( 2 * temp_num_nt_start_moved );	
				}
		
				unsigned int start_coord;
				if( opposite_strand == FALSE )
				{
					start_coord = temp_first_coord_in_chrom_it + 1;
				}
				else/*( opposite_strand == TRUE )*/
				{
					start_coord = temp_first_coord_in_chrom_it + 1 + ( uORF_len - 1 );
				}
		
				*uORF_content = extract_DNA_seq( chrom_seq, start_coord, uORF_len, opposite_strand );
				
				string poss_stop_codon = uORF_content->substr( uORF_content->size() - 3, 3 );
				
				
				// Check if a stop codon occurs at the end of the expected length
				if( poss_stop_codon == "TAA" || poss_stop_codon == "TGA" || poss_stop_codon == "TAG" )
				{
					num_poss_length_matches++;
					
					temp_new_uORF_start_pos_it = start_codon_pos_in_window[i] + chrom_it_search_window_it_diff;
				}
			}

			
			// If none (or more than one) of the start codons produce a uORF of the expected length, default to choosing the uATG closest
			//  to the expected start position
			if( num_poss_length_matches != 1 )
			{
				error_oss << "Error in extract_UTRs()': Attempt to manually align uORF could not be resolved, " 
						  << "because multiple start codons were found in the searched window that yielded a uORF of the expected length. "
						  << "Choosing the closest to the expected start codon.\n";
					  
				for( unsigned int i = 1; i < start_codon_pos_in_window.size(); i++ )
				{
					if( abs( (int)start_codon_pos_in_window[i] + (int)chrom_it_search_window_it_diff - (int)*uORF_first_coord_in_chrom_it ) <
						abs( (int)new_uORF_start_pos_it - (int)*uORF_first_coord_in_chrom_it ) )
					{
						new_uORF_start_pos_it = start_codon_pos_in_window[i] + chrom_it_search_window_it_diff;
					}
				}
			}
			else
			{
				new_uORF_start_pos_it = temp_new_uORF_start_pos_it;
			}
		}

		
		
		*num_nt_start_moved = (int)new_uORF_start_pos_it - (int)*uORF_first_coord_in_chrom_it;

		if( opposite_strand == FALSE )
		{
			*uORF_first_coord_in_chrom_it = new_uORF_start_pos_it;
		}
		else/*( opposite_strand == TRUE )*/
		{
			// Since 'new_uORF_start_pos_it' moved in the opposite direction, need to compensate by adding
			//   twice the number of nucleotides the start position moved
			*uORF_first_coord_in_chrom_it = new_uORF_start_pos_it - ( 2 * ( *num_nt_start_moved ) );	
		}
		
		*rel_uORF_pos += *num_nt_start_moved;
		
		error_oss << "Manual alignment was successful (shifting uORF start position by " << *num_nt_start_moved << " base pairs)";
		
		
		// Extract the new uORF contents
		unsigned int start_coord;
		if( opposite_strand == FALSE )
		{
			start_coord = *uORF_first_coord_in_chrom_it + 1;
		}
		else/*( opposite_strand == TRUE )*/
		{
			start_coord = *uORF_first_coord_in_chrom_it + 1 + ( uORF_len - 1 );
		}
		
		*uORF_content = extract_DNA_seq( chrom_seq, start_coord, uORF_len, opposite_strand );
		*start_codon_found = TRUE;
	}
	else
	{
		error_oss << "Manual alignment was unsuccessful";
		*start_codon_found = FALSE;
	}

	Errors.handle_error( NONFATAL, error_oss.str(), __FILE__, __LINE__ );
	// ------------------------------
	
	return;
}
//==============================================================================					


	

int uORF_analysis( const string & chrom_seq,
				   const string & gene_name,
				   const unsigned int gene_start_pos,
				   const bool opposite_strand,
				   const bool start_codon_found,
				   const int rel_uORF_pos,
				   int uORF_first_coord_in_chrom_it,
				   const vector <TFeature> untransl_reg,
				   string * const uORF_content,
				   string * const uORF_start_context,
				   string * const ext_uORF_start_context,
				   unsigned int * const uORF_start_pos,
				   unsigned int * const uORF_len,
				   bool * const end_past_gene_start,
				   bool * const uORF_in_frame )
{
	// ------------------------------
	// Calculate the start and end positions of the uORFs (not string iterators, 1-indexed)
	unsigned int uORF_end_pos;
	
	if( opposite_strand == FALSE )
	{
		*uORF_start_pos = uORF_first_coord_in_chrom_it + 1;
		uORF_end_pos    = *uORF_start_pos + *uORF_len - 1;
	}
	else/*( opposite_strand == TRUE )*/
	{
		*uORF_start_pos = ( uORF_first_coord_in_chrom_it + 1 ) + *uORF_len - 1;
		uORF_end_pos    =   uORF_first_coord_in_chrom_it + 1;
	}
	// ------------------------------
	

	
	// ------------------------------
	// If the uORF was correctly found, extract the start codon context (biologically-defined and extended)
	if( start_codon_found == TRUE )
	{
		// Extract the biologically-defined start context (remove introns)
		int uORF_context_start_coord = *uORF_start_pos - ( START_CONTEXT_NT_BEFORE_AUG * ( opposite_strand == FALSE ? 1 : -1 ) );

		unsigned int uORF_context_len_w_untrans = START_CONTEXT_LENGTH_NT;
		
		vector <TFeature> untransl_context_interfere;
		
		// If any introns exist that don't interfere with the uORF, but do interfere with the two codons prior to the uAUG,
		//   the start coordinate of the start context will need to be adjusted accordingly.
		//	 If any introns start within the uAUG, they should not be removed, since for the uORF to exist, such an 
		//   intron must sometimes not be removed from the transcript
		for( unsigned int i = 0; i < untransl_reg.size(); i++ )
		{
			int untransl_reg_end_coord = (int)untransl_reg[i].start_coord + ( ( (int)untransl_reg[i].length - 1 ) * ( opposite_strand == FALSE ? 1 : -1 ) );

			
			if( ( opposite_strand == FALSE && ( (int)*uORF_start_pos + NUM_NT_TO_COMPLETE_CODON - (int)untransl_reg[i].start_coord ) < 0 ) ||
				( opposite_strand == TRUE  && ( (int)*uORF_start_pos - NUM_NT_TO_COMPLETE_CODON - (int)untransl_reg[i].start_coord ) > 0 ) ) 
			{
				untransl_context_interfere.push_back( untransl_reg[i] );
			}
		

			if( ( opposite_strand == FALSE && ( (int)*uORF_start_pos - untransl_reg_end_coord > 0 ) && ( (int)*uORF_start_pos - untransl_reg_end_coord <= START_CONTEXT_NT_BEFORE_AUG ) ) ||
				( opposite_strand == TRUE  && ( (int)*uORF_start_pos - untransl_reg_end_coord < 0 ) && ( (int)*uORF_start_pos - untransl_reg_end_coord >= START_CONTEXT_NT_BEFORE_AUG ) ) )
			{
				uORF_context_len_w_untrans += untransl_reg[i].length;
				untransl_context_interfere.push_back( untransl_reg[i] );
				
				uORF_context_start_coord -= ( untransl_reg[i].length * ( opposite_strand == FALSE ? 1 : -1 ) );
			}
		}
		

		*uORF_start_context = extract_DNA_seq( chrom_seq, uORF_context_start_coord, START_CONTEXT_LENGTH_NT, opposite_strand, untransl_context_interfere );
		
		
		// Extract the extended start context (don't remove introns)
		unsigned int ext_start_context_start_pos = *uORF_start_pos - ( EXT_START_CONTEXT_NT_BEFORE_AUG * ( opposite_strand == FALSE ? 1 : -1 ) );
		*ext_uORF_start_context = extract_DNA_seq( chrom_seq, ext_start_context_start_pos, EXT_START_CONTEXT_LENGTH_NT, opposite_strand );
	}
	// ------------------------------

	
	
	// ------------------------------
	// Determine if the uORF correctly ends with a stop codon
	bool stop_codon_at_end = FALSE;
	string last_codon = uORF_content->substr( uORF_content->size() - 3, 3 );
	if( last_codon == "TAG" || last_codon == "TAA" || last_codon == "TGA" ) { stop_codon_at_end = TRUE; }
	// ------------------------------
	
	
	
	// ------------------------------
	// If the start codon was found, locate the uORF's first in frame stop codon (use this as the uORF's end if
	//   the annotated last codon was not a stop codon; if it was, error out)
	if( start_codon_found == TRUE )
	{
		// Find the true end position (altering the uORF's length accordingly)
		unsigned int true_end_pos = find_first_stop_codon( chrom_seq, *uORF_start_pos, opposite_strand, uORF_len, untransl_reg );
		
		
		if( ( opposite_strand == FALSE && true_end_pos < uORF_end_pos && stop_codon_at_end == TRUE ) ||
			( opposite_strand == TRUE  && true_end_pos > uORF_end_pos && stop_codon_at_end == TRUE ) )
		{
			ostringstream error_oss;
			error_oss << "Error in 'uORF_analysis()': For gene " << gene_name 
					  << ", annotated uORF last codon was a stop codon, but it was not the first stop codon in frame with the start codon. \n"
					  << "     uORF Start Position:         " << *uORF_start_pos << "\n"
					  << "     uORF Annotated End Position: " << uORF_end_pos    << "\n"
					  << "     uORF True End Position:      " << true_end_pos    << "\n"
					  << "     uORF Content:                " << *uORF_content;
		
			Errors.handle_error( NONFATAL, error_oss.str(), __FILE__, __LINE__ );
		}
		
		
		if( true_end_pos != uORF_end_pos )
		{
			uORF_end_pos = true_end_pos;
			*uORF_content = extract_DNA_seq( chrom_seq, *uORF_start_pos, *uORF_len, opposite_strand, untransl_reg );
		}
	}
	// ------------------------------
	
	
	
	// ------------------------------
	// Determine if the (alleged) uORF extends past the gene's annotated start coordinate (and if so, if it is in frame with the gene )
	if( ( opposite_strand == FALSE && uORF_end_pos >= gene_start_pos ) ||
		( opposite_strand == TRUE  && uORF_end_pos <= gene_start_pos ) )
	{
		*end_past_gene_start = TRUE;
		
		if	  ( rel_uORF_pos % 3 == 0 )  { *uORF_in_frame = TRUE;  }
		else/*( rel_uORF_pos % 3 != 0 )*/{ *uORF_in_frame = FALSE; }
	}
	else
	{
		*end_past_gene_start = FALSE;
	}
	// ------------------------------
	

	return SUCCESSFUL;
}
//==============================================================================



void combine_uORFs_by_gene( vector <TORF_Data> * const ORF_Data )
{
	for( unsigned int i = 1; i < ORF_Data->size(); i++ )
	{
		if( ORF_Data->at( i ).gene_name == ORF_Data->at( i - 1 ).gene_name )
		{
			// output_text_line( (string)"Note: Combining uORFs from multiple data sources for gene " + ORF_Data->at( i ).gene_name );
			
			vector <TuORF_Data> genes_uORFS = ORF_Data->at( i ).get_uORFs();
			for( unsigned int j = 0; j < genes_uORFS.size(); j++ )
			{
				ORF_Data->at( i - 1 ).add_uORF( genes_uORFS[j] );
			}
			
			if( ORF_Data->at( i ).CDS_rib_rpkM > ORF_Data->at( i - 1 ).CDS_rib_rpkM )
			{
				ORF_Data->at( i - 1 ).CDS_rib_rpkM = ORF_Data->at( i ).CDS_rib_rpkM;
			}
			
			if( ORF_Data->at( i ).CDS_mRNA_rpkM > ORF_Data->at( i - 1 ).CDS_mRNA_rpkM )
			{
				ORF_Data->at( i - 1 ).CDS_mRNA_rpkM = ORF_Data->at( i ).CDS_mRNA_rpkM;
			}
			
			ORF_Data->at( i - 1 ).add_tss( ORF_Data->at( i ).tss_rel_pos );
			
			ORF_Data->erase( ORF_Data->begin() + i );
			i--;
		}
	}
	
	return;
}
//==============================================================================



void delete_duplicate_uORFs( vector <TORF_Data> * const ORF_Data, const bool uORFs_to_delete )
{
	unsigned int num_deleted 			= 0;
	unsigned int num_deleted_w_diff_len = 0;
	
	for( unsigned int i = 0; i < ORF_Data->size(); i++ )
	{
		vector <TuORF_Data> ORF_uORF_list = ORF_Data->at( i ).get_uORFs();
		

		for( unsigned int j = 1; j < ORF_uORF_list.size(); ++j )
		{
			unsigned int prev_uORF_it = ORF_uORF_list.size();
			unsigned int searched_uORF_it = 0;
			
			do
			{
				if( ORF_uORF_list[j].start_pos == ORF_uORF_list[searched_uORF_it].start_pos )
				{
					prev_uORF_it = searched_uORF_it;
				}
				
				searched_uORF_it++;
			
			}while( prev_uORF_it == ORF_uORF_list.size() && searched_uORF_it < j );
			
			
			
			if( prev_uORF_it != ORF_uORF_list.size() )
			{
				unsigned int uORF_to_delete = j;
				unsigned int uORF_to_keep   = prev_uORF_it;
				
				if( ORF_uORF_list[uORF_to_delete].len != ORF_uORF_list[uORF_to_keep].len )
				{
					num_deleted_w_diff_len++;
					Errors.handle_error( FATAL, "Error in 'delete_duplicate_uORFs()': Duplicate uORFs with different lengths for gene " + ORF_Data->at( i ).gene_name + ".", __FILE__, __LINE__ );

					// Now that all uORFs have been correctly found, there should be none at the same position with different lengths.
					/*
					if( ORF_uORF_list[uORF_to_delete].start_codon == TRUE && ORF_uORF_list[uORF_to_keep].start_codon == FALSE )
					{
						uORF_to_delete = prev_uORF_it;
					    uORF_to_keep   = j;
					}
					else if( ORF_uORF_list[uORF_to_delete].start_codon == TRUE && ORF_uORF_list[uORF_to_keep].start_codon == TRUE )
					{	
						Errors.handle_error( FATAL, "Error in 'delete_duplicate_uORFs()': Duplicate uORFs with different lengths for gene " + ORF_Data->at( i ).gene_name + ".", __FILE__, __LINE__ );
					}
					else if( ORF_uORF_list[uORF_to_delete].start_codon == FALSE && ORF_uORF_list[uORF_to_keep].start_codon == FALSE )
					{
						if( ORF_uORF_list[uORF_to_delete].num_nt_len_changed == 0 && ORF_uORF_list[uORF_to_keep].num_nt_len_changed != 0 )
						{
							uORF_to_delete = prev_uORF_it;
							uORF_to_keep   = j;
						}
						else
						{
							if( ORF_uORF_list[uORF_to_delete].num_nt_start_moved == 0 && ORF_uORF_list[uORF_to_keep].num_nt_start_moved != 0 )
							{
								uORF_to_delete = prev_uORF_it;
								uORF_to_keep   = j;
							}
						}
					}*/
				}
				
				
				// Check if the data source of the deleted uORF is already among the sources of the kept uORF
				vector <TData_Source>::iterator curr_source_it = ORF_uORF_list[uORF_to_keep].source.begin();
				
				// Find where the source should be added to the list (alphabetical)
				while( ( curr_source_it < ORF_uORF_list[uORF_to_keep].source.end() ) && 
					   ( curr_source_it->get_author().compare( ORF_uORF_list[uORF_to_delete].source[0].get_author() ) < 0 ) )
				{
					curr_source_it++;
				}
				
				bool delete_uORF = TRUE;
				
				if( uORFs_to_delete == DELETE_SAME_SOURCE_ONLY )
				{
					delete_uORF = FALSE;
					
					for( unsigned int k = 0; k < uORF_to_delete; k++ )
					{
						if( ( ORF_uORF_list[uORF_to_delete].start_pos 			   == ORF_uORF_list[k].start_pos 			  ) &&
							( ORF_uORF_list[uORF_to_delete].source[0].get_author() == ORF_uORF_list[k].source[0].get_author() ) )
						{
							delete_uORF = TRUE;
						}
					}
				}
				
				
				bool is_source_already_in_list = FALSE;
				if( curr_source_it < ORF_uORF_list[uORF_to_keep].source.end() && curr_source_it->get_author() == ORF_uORF_list[uORF_to_delete].source[0].get_author() )
				{
					is_source_already_in_list = TRUE;
				}
				
				
				// If deleting only duplicate uORFs from the same source, and the source vector of the kept uORF doesn't contain the 
				//   source of the duplicate uORF, do nothing. Otherwise, delete the duplicate uORF
				if( delete_uORF )
				{
					if( is_source_already_in_list == FALSE )
					{
						ORF_Data->at( i ).add_source( uORF_to_keep, 
													  (unsigned int)( curr_source_it - ORF_uORF_list[uORF_to_keep].source.begin() ), 
													  ORF_uORF_list[uORF_to_delete].source[0].get_author() );
					}
					
					
					
					if( ORF_uORF_list[uORF_to_delete].AUGCAI > ORF_uORF_list[uORF_to_keep].AUGCAI )
					{
						ORF_Data->at( i ).update_AUGCAI( uORF_to_keep, ORF_uORF_list[uORF_to_delete].AUGCAI );
					}
					
					if( ORF_uORF_list[uORF_to_delete].num_ribosomes > ORF_uORF_list[uORF_to_keep].num_ribosomes )
					{
						ORF_Data->at( i ).update_num_rib( uORF_to_keep, ORF_uORF_list[uORF_to_delete].num_ribosomes, ORF_uORF_list[uORF_to_delete].num_mRNA );
					}
					
					if( ORF_uORF_list[uORF_to_delete].num_nt_start_moved == 0 )
					{
						ORF_Data->at( i ).clear_start_pos_change( uORF_to_keep );
					}
					
					if( ORF_uORF_list[uORF_to_delete].num_nt_len_changed == 0 )
					{
						ORF_Data->at( i ).clear_length_change( uORF_to_keep );
						if( ORF_uORF_list[uORF_to_delete].problem == FALSE )
						{
							ORF_Data->at( i ).clear_problem( uORF_to_keep );
						}
					}
					

					ORF_Data->at( i ).delete_uORF( uORF_to_delete );
					ORF_uORF_list = ORF_Data->at( i ).get_uORFs();
					j--;
					
					num_deleted++;
				}
				else 
				{
					if( uORFs_to_delete == DELETE_SAME_SOURCE_ONLY )
					{
						// Even if not deleting the same uORF from different sources, if identical uORFs are found, some indicating
						//   a problem and at least one not, assume there's no problem
						
						bool all_instances_have_problems = TRUE;
						unsigned int problem_uORF_it = 0;
						
						do
						{
							if( ORF_uORF_list[uORF_to_delete].start_pos == ORF_uORF_list[problem_uORF_it].start_pos &&
								ORF_uORF_list[problem_uORF_it].problem  == FALSE )
							{
								all_instances_have_problems = FALSE;
							}
							
							problem_uORF_it++;
						
						}while( all_instances_have_problems == TRUE && problem_uORF_it < ORF_uORF_list.size() );
			
			
						if( all_instances_have_problems == FALSE )
						{
							if( ORF_uORF_list[uORF_to_delete].problem == TRUE )
							{
								ORF_Data->at( i ).clear_problem( uORF_to_delete );
							}
							
							if( ORF_uORF_list[uORF_to_keep].problem == TRUE )
							{
								ORF_Data->at( i ).clear_problem( uORF_to_keep );
							}
						}
					}
				}
			}
		}
	}
	

	ostringstream output_oss;
	output_oss << "num duplicate uORFs: 	   " << num_deleted 		   << "\n"
			   << "num with different lengths: " << num_deleted_w_diff_len << "\n";
	
	output_text_line( output_oss.str() );
	
	return;

}
//==============================================================================



int parse_uORFs_from_list( vector <TORF_Data> * const ORF_Data, 
						   const vector <string> & gff_annotations, 
						   const vector <TGO_Annotation> & All_GO_Annotations, 
						   const bool & file_naming_method )
{
	// First make sure the vector is empty
	ORF_Data->clear();
	
	// Parse the desired .CSV uORF list
	TCSV_Contents * uORF_list = new TCSV_Contents( PATH_OUTPUT_FOLDER, get_uORF_list_file_name( file_naming_method, READ ) );
	uORF_list->parse_csv();
	
	
	// Read in all the data columns (check for the existence of those that are optional)
	vector <unsigned int> gene_chrom_nums 	      = uORF_list->get_csv_column<unsigned int>	( uORF_LIST_CHROM_NUM_HEADER		 );
	vector <string> 	  gene_names 		      = uORF_list->get_csv_column<string>	    ( uORF_LIST_SYST_NAME_HEADER		 );
	vector <string> 	  gene_start_contexts     = uORF_list->get_csv_column<string>	    ( uORF_LIST_GENE_CONTEXT_HEADER		 );
	vector <string> 	  TSS_rel_pos_list		  = uORF_list->get_csv_column<string>	    ( uORF_LIST_TSS_REL_POS_HEADER		 );
	vector <double> 	  gene_AUGCAIs 		      = uORF_list->get_csv_column<double>	    ( uORF_LIST_GENE_AUGCAI_HEADER		 );
	vector <int> 	  	  gene_pos_changes;    // = uORF_list->get_csv_column<int>	     	( uORF_LIST_GENE_CHANGE_HEADER		 );
	vector <double> 	  CDS_ribs 			      = uORF_list->get_csv_column<double>	    ( uORF_LIST_CDS_RIB_RPKM_HEADER		 );
	vector <double> 	  CDS_mRNA 			      = uORF_list->get_csv_column<double>	    ( uORF_LIST_CDS_mRNA_RPKM_HEADER	 );
	vector <double> 	  CDS_transl_corr		  = uORF_list->get_csv_column<double>	    ( uORF_LIST_TRANSL_CORR_HEADER	 	 );
	vector <int> 	  	  CDS_RPM_reads			  = uORF_list->get_csv_column<int>	    	( uORF_LIST_RPM_READS_HEADER		 );
	vector <double> 	  CDS_transl_eff		  = uORF_list->get_csv_column<double>	    ( uORF_LIST_TRANSL_EFF_HEADER		 );
	vector <string> 	  GO_terms_sorted_by;  // = uORF_list->get_csv_column<string>	    ( uORF_LIST_GO_TERM_SORTED_HEADER	 );
	vector <string> 	  GO_bio_procs 		      = uORF_list->get_csv_column<string>	    ( uORF_LIST_GO_BIO_PROC_HEADER   	 );
	vector <string> 	  GO_cell_comps 	      = uORF_list->get_csv_column<string>	    ( uORF_LIST_GO_CELL_COMP_HEADER  	 );
	vector <string> 	  GO_mol_funcs 		      = uORF_list->get_csv_column<string>	    ( uORF_LIST_GO_MOL_FUNC_HEADER   	 );
 // vector <unsigned int> num_uORFs 		      = uORF_list->get_csv_column<unsigned int> ( uORF_LIST_NUM_uORFs_HEADER   		 );		// Easily Calculated
	vector <string> 	  source_authors 	      = uORF_list->get_csv_column<string>	    ( uORF_LIST_SOURCE_HEADER       	 );
	vector <string> 	  source_evidence 	      = uORF_list->get_csv_column<string>	    ( uORF_LIST_EVIDENCE_TYPE_HEADER 	 );
	vector <unsigned int> uORF_start_pos 	      = uORF_list->get_csv_column<unsigned int> ( uORF_LIST_uORF_POS_HEADER			 );
	vector <unsigned int> uORF_lens 		      = uORF_list->get_csv_column<unsigned int>	( uORF_LIST_uORF_LEN_HEADER			 );
	vector <int> 	  	  uORF_rel_pos 		      = uORF_list->get_csv_column<int>	     	( uORF_LIST_uORF_REL_POS_HEADER		 );
 // vector <int> 	  	  uORF_distance_to_TSS 	  = uORF_list->get_csv_column<int>	     	( uORF_LIST_uORF_TO_TSS_HEADER		 );		// Easily Calculated
	vector <double> 	  uORF_AUGCAIs	  	      = uORF_list->get_csv_column<double>	    ( uORF_LIST_AUGCAI_HEADER			 );
	vector <int> 	  	  uORF_ribosomes 	      = uORF_list->get_csv_column<int>	     	( uORF_LIST_RIBOSOMES_HEADER		 );
	vector <int> 	  	  uORF_mRNA 	      	  = uORF_list->get_csv_column<int>	     	( uORF_LIST_mRNA_HEADER				 );
 // vector <string> 	  uORF_transl_effects	  = uORF_list->get_csv_column<string>	    ( uORF_LIST_uORF_CONTEXT_HEADER	 	 );		// The 'uORF_transl_effects' was deemed to difficult to parse to merit the time spent
	vector <string> 	  uORF_start_contexts     = uORF_list->get_csv_column<string>	    ( uORF_LIST_uORF_CONTEXT_HEADER		 );
 // vector <string> 	  uORF_problems; 	   // = uORF_list->get_csv_column<string>	    ( uORF_LIST_PROBLEM_HEADER			 );
	vector <string> 	  uORF_in_frame; 	   // = uORF_list->get_csv_column<string>	    ( uORF_LIST_uORF_IN_FRAME_HEADER	 );
	vector <string> 	  uORF_start_codon_abs;// = uORF_list->get_csv_column<string>	    ( uORF_LIST_START_CODON_HEADER		 );
	vector <int> 	  	  uORF_start_moved;    // = uORF_list->get_csv_column<int>	     	( uORF_LIST_START_MOVED_HEADER		 );
	vector <string> 	  uORF_past_gene; 	   // = uORF_list->get_csv_column<string>	    ( uORF_LIST_PAST_GENE_HEADER		 );
	vector <int> 	  	  uORF_len_change; 	   // = uORF_list->get_csv_column<int>	     	( uORF_LIST_LEN_CHANGE_HEADER		 );
	vector <string> 	  uORF_relation_to_TSS;// = uORF_list->get_csv_column<string> 		( uORF_LIST_TSS_REL_HEADER			 );
	vector <string> 	  uORF_ext_contexts 	  = uORF_list->get_csv_column<string>	    ( uORF_LIST_EXT_uORF_CONTEXT_HEADER	 );
	vector <string> 	  gene_ext_contexts 	  = uORF_list->get_csv_column<string>	    ( uORF_LIST_EXT_GENE_CONTEXT_HEADER	 );
	vector <string> 	  uORF_sequences 	      = uORF_list->get_csv_column<string>	    ( uORF_LIST_uORF_HEADER				 );
	
	
	
	// Optional Columns
	if( uORF_list->does_column_exist( uORF_LIST_GENE_CHANGE_HEADER    ) == TRUE ) { gene_pos_changes     = uORF_list->get_csv_column<int>	( uORF_LIST_GENE_CHANGE_HEADER	  ); }
	if( uORF_list->does_column_exist( uORF_LIST_GO_TERM_SORTED_HEADER ) == TRUE ) { GO_terms_sorted_by   = uORF_list->get_csv_column<string>( uORF_LIST_GO_TERM_SORTED_HEADER ); }
 // if( uORF_list->does_column_exist( uORF_LIST_PROBLEM_HEADER		  ) == TRUE ) { uORF_problems        = uORF_list->get_csv_column<string>( uORF_LIST_PROBLEM_HEADER		  ); }		// The 'Problem' field is automatically determined in the uORF constructor
	if( uORF_list->does_column_exist( uORF_LIST_uORF_IN_FRAME_HEADER  ) == TRUE ) { uORF_in_frame        = uORF_list->get_csv_column<string>( uORF_LIST_uORF_IN_FRAME_HEADER  ); }
	if( uORF_list->does_column_exist( uORF_LIST_START_CODON_HEADER	  ) == TRUE ) { uORF_start_codon_abs = uORF_list->get_csv_column<string>( uORF_LIST_START_CODON_HEADER	  ); }
	if( uORF_list->does_column_exist( uORF_LIST_START_MOVED_HEADER	  ) == TRUE ) { uORF_start_moved     = uORF_list->get_csv_column<int>	( uORF_LIST_START_MOVED_HEADER	  ); }
	if( uORF_list->does_column_exist( uORF_LIST_PAST_GENE_HEADER	  ) == TRUE ) { uORF_past_gene       = uORF_list->get_csv_column<string>( uORF_LIST_PAST_GENE_HEADER	  ); }
	if( uORF_list->does_column_exist( uORF_LIST_LEN_CHANGE_HEADER	  ) == TRUE ) { uORF_len_change      = uORF_list->get_csv_column<int>	( uORF_LIST_LEN_CHANGE_HEADER	  ); }
 //	if( uORF_list->does_column_exist( uORF_CSV_COL_NUM_TSS_REL	 	  ) == TRUE ) { uORF_relation_to_TSS = uORF_list->get_csv_column<string>( uORF_CSV_COL_NUM_TSS_REL	 	  ); }		// The 'uORF_relation_to_TSS' field is automatically determined when 'check_uORF_upstream_of_tss()' is called
	
	
	
	string prev_gene_name = "";
	
	
	for( unsigned int i = 0; i < gene_names.size(); i++ )
	{
		if( gene_names[i] != prev_gene_name )
		{
			// ------------------------------
			// Determine what DNA strand the uORF is on
			bool opposite_strand;
			if	   ( gene_names[i].find( 'W' ) != string::npos ) { opposite_strand = FALSE; }
			else if( gene_names[i].find( 'C' ) != string::npos ) { opposite_strand = TRUE;  }
			else
			{
				return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'parse_uORFs_from_list()': Gene " + gene_names[i] + 
																	   " could not be recognized as either residing on the Watson or Crick strand. ", __FILE__, __LINE__ );
			}
			// ------------------------------
			
			
			
			// ------------------------------
			// Get the gene's start coordinate, as well as its associated untranslated regions
			unsigned int gene_start_coord;
			unsigned int gene_end_coord;
			unsigned int gene_intergen_start_coord;
			vector <TFeature> untransl_reg;
			TFeature fpUTR_intron( string::npos, string::npos );
				
			if( get_gene_coord_from_gff( gene_names[i], 
										 gff_annotations[ gene_chrom_nums[i] - 1 ], 
										 opposite_strand, 
										 &gene_start_coord, 
										 &gene_end_coord, 
										 &gene_intergen_start_coord,
										 &untransl_reg, 
										 &fpUTR_intron ) )
			{
				return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in parse_uORFs_from_list(): Could not find gene " + gene_names[i] + 
																	   " in the GFF. ", __FILE__, __LINE__ );
			}
			// ------------------------------
			
			
			
			// ------------------------------
			// If in the table, add the gene position change
			int gene_pos_change = 0;
			if( gene_pos_changes.size() != 0 ) { gene_pos_change = gene_pos_changes[i]; }
			// ------------------------------
			
			
			
			// ------------------------------
			// Add the ORF
			ORF_Data->push_back( TORF_Data( gene_chrom_nums[i],
											gene_names[i],  
											gene_start_coord,
											abs( (int)gene_end_coord - (int)gene_start_coord + 1 ),
											gene_intergen_start_coord,
											gene_pos_change, 	
											opposite_strand,
											untransl_reg, 
											fpUTR_intron ) );
			// ------------------------------
			
			
			
			// ------------------------------
			// Add the remaining ORF parameters
			ORF_Data->back().gene_start_context     = gene_start_contexts[i];
			ORF_Data->back().ext_gene_start_context = gene_ext_contexts[i];
			
			vector <int> TSS_rel_pos = parse_delimited_list<int>( TSS_rel_pos_list[i], DEFAULT_DELIMITER );
			
			for( unsigned int j = 0; j < TSS_rel_pos.size(); j++ )
			{
				ORF_Data->back().add_tss( TSS_rel_pos[j] );
			}
			
			ORF_Data->back().gene_AUGCAI  	   = gene_AUGCAIs[i];
			ORF_Data->back().CDS_rib_rpkM 	   = CDS_ribs[i];
			ORF_Data->back().CDS_mRNA_rpkM 	   = CDS_mRNA[i];
			ORF_Data->back().transl_corr 	   = CDS_transl_corr[i];
			ORF_Data->back().RPF_reads 		   = CDS_RPM_reads[i];
			ORF_Data->back().transl_eff 	   = CDS_transl_eff[i];
			
			if( GO_terms_sorted_by.size() != 0 )
			{
				ORF_Data->back().GO_term_sorted_by = GO_terms_sorted_by[i];
			}
			
			
			
			// Add the GO terms
			vector < vector <string> > GO_terms( NUM_GO_NAMESPACES, vector <string>( 0 ) );
			GO_terms[GO_BIO_PROC_IT]  = parse_delimited_list<string>( GO_bio_procs[i],  GO_TERM_LIST_DELIMITER );
			GO_terms[GO_CELL_COMP_IT] = parse_delimited_list<string>( GO_cell_comps[i], GO_TERM_LIST_DELIMITER );
			GO_terms[GO_MOL_FUNC_IT]  = parse_delimited_list<string>( GO_mol_funcs[i],  GO_TERM_LIST_DELIMITER );
			
			vector <string> GO_namespaces( NUM_GO_NAMESPACES, "" );
			GO_namespaces[GO_BIO_PROC_IT]  = GO_BIO_PROC_ID;
			GO_namespaces[GO_CELL_COMP_IT] = GO_CELL_COMP_ID;
			GO_namespaces[GO_MOL_FUNC_IT]  = GO_MOL_FUNC_ID;
			
			
			for( unsigned int GO_namespace_it = 0; GO_namespace_it < GO_namespaces.size(); GO_namespace_it++ )
			{
				for( unsigned int term_it = 0; term_it < GO_terms[GO_namespace_it].size(); term_it++ )
				{
					ORF_Data->back().add_GO_Annotation( GO_namespaces[GO_namespace_it], GO_terms[GO_namespace_it][term_it], "NA", All_GO_Annotations );
				}
			}
			// ------------------------------
			
			
			prev_gene_name = ORF_Data->back().gene_name;
		}
	
	
		// ------------------------------
		// Read in the uORF Data Sources
		vector <string> paper_authors = parse_delimited_list<string>( source_authors[i], DEFAULT_DELIMITER );
		vector <TData_Source> data_sources;
		
		for( unsigned int j = 0; j < paper_authors.size(); j++ )
		{
			data_sources.push_back( TData_Source( paper_authors[j] ) );
		}
		
		if( data_sources.size() == 0 )
		{
			return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'parse_uORFs_from_list()': uORF associated with gene " + gene_names[i] + 
																   " did not have any specified data sources. ", __FILE__, __LINE__ );
		}
		// ------------------------------
		
		
		
		// ------------------------------
		// Since the "problem" fields will only be present if non-canonical uORFs were printed,
		//   these fields should be defaulted, then updated if the the columns are found to exist
		int exp_uORF_len = uORF_lens[i];
		if( uORF_len_change.size() != 0 ) { exp_uORF_len = (int)uORF_lens[i] - uORF_len_change[i]; }
		
		bool is_uORF_in_frame = FALSE;
		if( uORF_in_frame.size() != 0 ) { is_uORF_in_frame = ( uORF_in_frame[i] == "yes" ) ? TRUE : FALSE;  }
		
		bool start_codon_found = TRUE;
		if( uORF_start_codon_abs.size() != 0 ) { start_codon_found = ( uORF_start_codon_abs[i] == "yes" ) ? FALSE : TRUE;  }
		
		int uORF_start_pos_change = FALSE;
		if( uORF_start_moved.size() != 0 ) { uORF_start_pos_change = uORF_start_moved[i]; }
		
		bool end_past_gene_start = FALSE;
		if( uORF_past_gene.size() != 0 ) { end_past_gene_start = ( uORF_past_gene[i] == "yes" ) ? TRUE : FALSE; }
		// ------------------------------
		

		
		// ------------------------------
		// Add the uORF
		ORF_Data->back().add_uORF( TuORF_Data( uORF_sequences[i],
											   uORF_start_contexts[i],
											   uORF_ext_contexts[i],
											   uORF_rel_pos[i],
											   uORF_start_pos[i],
											   uORF_lens[i],
											   exp_uORF_len,
											   data_sources[0].get_author(),
											   
											   is_uORF_in_frame,
											   start_codon_found,
											   uORF_start_pos_change,
											   end_past_gene_start ) );
		// ------------------------------
											   
			

		// ------------------------------
		// Update the uORF parameters not included in the constructor
		if( data_sources.size() > 1 ) 
		{ 
			ORF_Data->back().update_source( ( ORF_Data->back().get_uORFs().size() - 1 ), data_sources ); 
		}
		
		unsigned int curr_uORF_it = ORF_Data->back().get_uORFs().size() - 1;
		ORF_Data->back().update_num_rib( curr_uORF_it, uORF_ribosomes[i], uORF_mRNA[i] );
		ORF_Data->back().update_AUGCAI ( curr_uORF_it, uORF_AUGCAIs[i] );
		// ------------------------------
	}
	
	
	// Done outside of the loop to avoid repeating calculations
	for( unsigned int i = 0; i < ORF_Data->size(); i++ )
	{
		ORF_Data->at( i ).check_uORF_upstream_of_tss();
	}
	
	
	if( get_uORF_effects_on_gene_translation( ORF_Data ) )
	{
		Errors.handle_error( FATAL, "", __FILE__, __LINE__ );
	}
	

	return SUCCESSFUL;
}
//==============================================================================



void write_Miura_SGD_annot_info_to_txt( void )
{
	// ------------------------------
	// Read in the GFF file containing the Miura annotations used in SGD's GBrowse
	string Miura_SGD_Annotations;
	if( read_entire_file_contents( PATH_DATA_FOLDER, MIURA_SGD_ANNOTATIONS_FILE_NAME, &Miura_SGD_Annotations ) )
	{
		Errors.handle_error( FATAL, "Error reading Miura SGD Annotations file contents. ", __FILE__, __LINE__ );
	}
	// ------------------------------
	
	
	
	// ------------------------------
	// Extract the list of associated features and their clones
	vector <string> associated_features = extract_fields( Miura_SGD_Annotations, "associated_feature=", ";" );
	vector <string> clone_IDs 			= extract_fields( Miura_SGD_Annotations, "ID=", ";" );
	
	vector <string> annotation_lines 	= extract_fields( Miura_SGD_Annotations.substr( Miura_SGD_Annotations.find( "\nchrI" ) ), "\n", "\n" );
	
	
	if( annotation_lines.size() != clone_IDs.size() )
	{
		Errors.handle_error( FATAL, (string)"Error in 'write_Miura_SGD_annot_info_to_txt()': Number of extracted clone ID's (" + get_str( clone_IDs.size() ) + 
											") didn't match the number of lines (" + get_str( annotation_lines.size() ) + "). ", __FILE__, __LINE__ );
	}
	// ------------------------------
	
	
	
	// ------------------------------
	// Write the list of features to a text file
	string features_file_contents = "";
	
	for( unsigned int i = 0; i < associated_features.size(); i++ )
	{
		features_file_contents.append( associated_features[i] + (string)"\n" );
	}
	

	if( write_file_contents( PATH_OUTPUT_FOLDER, MIURA_SGD_FEATURES_LIST_FILE_NAME, features_file_contents, TRUE ) )
	{
		Errors.handle_error( FATAL, (string)"Error in 'write_Miura_SGD_annot_info_to_txt()': " );
	}
	
	output_text_line( (string)"- Successfully wrote Miura features list to file '" + PATH_OUTPUT_FOLDER + PATH_FOLDER_SEPARATOR + MIURA_SGD_FEATURES_LIST_FILE_NAME + "'" );
	// ------------------------------
	
	
	
	// ------------------------------
	// Write the list of clone ID's to a text file, 
	string clone_id_file_contents = "";
	
	for( unsigned int i = 0; i < clone_IDs.size(); i++ )
	{
		// Print the clone ID once for each of the clone's associated features
		//   (note that this is presently commented out because the GFF lists of associated features don't match
		//    the clone ID's they're assigned to)
		
		unsigned int num_features_this_id = 1; // count_instances_of( annotation_lines[i], "associated_feature=" );
		
		for( unsigned int j = 0; j < num_features_this_id; j++ )
		{
			clone_id_file_contents.append( clone_IDs[i] + (string)"\n" );
		}
	}

	
	if( write_file_contents( PATH_OUTPUT_FOLDER, MIURA_SGD_CLONE_ID_LIST_FILE_NAME, clone_id_file_contents, TRUE ) )
	{
		Errors.handle_error( FATAL, (string)"Error in 'write_Miura_SGD_annot_info_to_txt()': " );
	}
	
	output_text_line( (string)"- Successfully wrote Miura clone ID list to file '" + PATH_OUTPUT_FOLDER + PATH_FOLDER_SEPARATOR + MIURA_SGD_CLONE_ID_LIST_FILE_NAME + "'" );
	// ------------------------------
	
	
	return;
}
//==============================================================================



void write_Miura_TSS_data_to_csv( void )
{
	// ------------------------------
	// Read in the GFF file containing the Miura annotations used in SGD's GBrowse
	string Miura_SGD_Annotations;
	if( read_entire_file_contents( PATH_DATA_FOLDER, MIURA_SGD_ANNOTATIONS_FILE_NAME, &Miura_SGD_Annotations ) )
	{
		Errors.handle_error( FATAL, "Error reading Miura SGD Annotations file contents. ", __FILE__, __LINE__ );
	}

	string Miura_SGD_Annotations_file_body = Miura_SGD_Annotations.substr( 	Miura_SGD_Annotations.find( "\nchrI" ) );
	// ------------------------------
	
	
	
	// ------------------------------
	// Extract the needed information (chromosome number, clone ID, and hit start/end)
	vector <string> chrom_num_rn  = extract_fields( Miura_SGD_Annotations_file_body, "\nchr", 		 "\t" );
	vector <string> clone_ids 	  = extract_fields( Miura_SGD_Annotations_file_body, "ID=",   		 ";"  );
	vector <string> hit_start_str = extract_fields( Miura_SGD_Annotations_file_body, "cDNA_clone\t", "\t" );
	vector <string> hit_end_str   = extract_fields( Miura_SGD_Annotations_file_body, "cDNA_clone\t", "."  );
	
	
	vector <unsigned int> chrom_num( chrom_num_rn.size() );
	vector <string> clone_ids_no_suffix = clone_ids;
	vector <unsigned int> hit_start( hit_start_str.size(), 0 );
	vector <unsigned int> hit_end  ( hit_end_str.size(),   0 );
	
	for( unsigned int i = 0; i < clone_ids_no_suffix.size(); i++ )
	{
		if    ( chrom_num_rn[i] != "mt" )  { chrom_num[i] = convert_roman_numeral_to_uint( chrom_num_rn[i] ); }
		else/*( chrom_num_rn[i] == "mt" )*/{ chrom_num[i] = 0; 												  }
		
		clone_ids_no_suffix[i] = clone_ids_no_suffix[i].substr( 0, clone_ids_no_suffix[i].rfind( "." ) );
		
		hit_start[i] = atoi( hit_start_str[i].c_str() );
		
		unsigned int hit_end_start_pos = hit_end_str[i].find( "\t" ) + 1;
		unsigned int hit_end_end_pos   = find_last_digit_or_char( DIGIT, hit_end_str[i], hit_end_start_pos );
		hit_end[i] = atoi( hit_end_str[i].substr( hit_end_start_pos, ( hit_end_end_pos - hit_end_start_pos + 1 ) ).c_str() );
	}
	// ------------------------------


	
	// ------------------------------
	// Read in the needed information from Miura Table 1, which was used to (incorrectly) construct the GFF)
	TCSV_Contents Miura_Table_1( PATH_DATA_SOURCE_FOLDER, MIURA_TABLE_1_FILE_NAME );
	Miura_Table_1.parse_csv();
	
	vector <string> 	  clone_ids_table     = Miura_Table_1.get_csv_column<string>	  ( MIURA_TABLE1_CLONE_ID_HEADER  	 );
	vector <string> 	  chrom_num_table_str = Miura_Table_1.get_csv_column<string>	  ( MIURA_TABLE1_CHROMOSOME_HEADER 	 );
	vector <unsigned int> hit_start_table     = Miura_Table_1.get_csv_column<unsigned int>( MIURA_TABLE1_HIT_START_HEADER  	 );
	vector <unsigned int> hit_end_table 	  = Miura_Table_1.get_csv_column<unsigned int>( MIURA_TABLE1_HIT_END_HEADER 	 );
	vector <string> 	  gene_name_table     = Miura_Table_1.get_csv_column<string>	  ( MIURA_TABLE1_SYST_NAME_HEADER  	 );
	                                                                                                                        
	vector <unsigned int> Gcap_qual_table     = Miura_Table_1.get_csv_column<unsigned int>( MIURA_TABLE1_GCAP_QUAL_HEADER 	 );
	vector <string> 	  Gcap_judge_table    = Miura_Table_1.get_csv_column<string>	  ( MIURA_TABLE1_GCAP_JUDGE_HEADER 	 );
	vector <string> 	  assignemnt_table    = Miura_Table_1.get_csv_column<string>	  ( MIURA_TABLE1_ASSIGNMENT_HEADER 	 );
	vector <string> 	  feature_type_table  = Miura_Table_1.get_csv_column<string>	  ( MIURA_TABLE1_FEATURE_TYPE_HEADER );
	vector <int> 		  hit_rel_pos_table   = Miura_Table_1.get_csv_column<int>		  ( MIURA_TABLE1_HIT_REL_POS_HEADER  );
	
	vector <unsigned int> chrom_num_table( chrom_num_table_str.size() );
	
	for( unsigned int i = 0; i < chrom_num_table_str.size(); i++ )
	{
		unsigned int chr_start_pos = find_first_digit_or_char( DIGIT, chrom_num_table_str[i] );
		
		if( chr_start_pos != string::npos )
		{
			chrom_num_table[i] = atoi( chrom_num_table_str[i].substr( chr_start_pos ).c_str() );
		}
		else
		{
			chrom_num_table[i] = 0; 
		}
	}
	// ------------------------------

	
	
	// ------------------------------
	// Build the header row of the Miura TSS .csv
	vector <vector <string > > TSS_csv( 1, vector <string>( NUM_CSV_TSS_COLUMNS, "" ) );
	
	TSS_csv[0][TSS_CSV_COL_NUM_CLONE_ID   ] = MIURA_TSS_CLONE_ID_HEADER;
	TSS_csv[0][TSS_CSV_COL_NUM_SYST_NAME  ] = MIURA_TSS_SYST_NAME_HEADER;
	TSS_csv[0][TSS_CSV_COL_NUM_START_COORD] = MIURA_TSS_HIT_START_HEADER;
	TSS_csv[0][TSS_CSV_COL_NUM_END_COORD  ] = MIURA_TSS_HIT_END_HEADER;
	// ------------------------------
	
	
	
	// ------------------------------
	// Match the clones from the GFF with the clones in Miura Table 1 to construct a list of clones that
	//   include a feature's 5' UTR (thus allowing for the determination of the TSS's)
	
	for( unsigned int i = 0; i < clone_ids_table.size(); i++ )
	{
		// Only add the clones that fit the criteria the Miura group used to construct Table 4 (5' UTR Info) from Table 1
		if( ( Gcap_qual_table[i] 	>= MIN_GCAP_QUALITY 												) && 
			( Gcap_judge_table[i]   == PERFECT_GCAP														) && 
			( assignemnt_table[i]   == SINGLE_FEATURE || assignemnt_table[i]   == SINGLE_MEATURE_MANUAL ) && 
			( feature_type_table[i] == ORF 			  || feature_type_table[i] == PSEUDOGENE 			) && 
			( hit_rel_pos_table[i]  <  0 																) )
		{
			// For each clone that fits the criteria in Table 1, compile a list of all the matching clone ID's on the same chromosome in the GFF 
			vector <unsigned int> matching_clones_it( 0 );
			unsigned int curr_it = (unsigned int)( find( clone_ids_no_suffix.begin(), clone_ids_no_suffix.end(), clone_ids_table[i] ) - clone_ids_no_suffix.begin() );
			
			while( curr_it != clone_ids_no_suffix.size() )
			{
				if( chrom_num_table[i] == chrom_num[curr_it] )
				{
					matching_clones_it.push_back( curr_it );
				}
				
				curr_it = (unsigned int)( find( clone_ids_no_suffix.begin() + curr_it + 1, clone_ids_no_suffix.end(), clone_ids_table[i] ) - clone_ids_no_suffix.begin() );
			}
			
			unsigned int correct_match_it = 0;
			
			
			// Error out if no matches were found.  If one was found, assume it is correct.
			if( matching_clones_it.size() == 0 )
			{
				Errors.handle_error( FATAL, (string)"Error in 'write_Miura_TSS_data_to_csv()': Failed to find clone " + clone_ids_table[i] + 
													" in the Miura SGD annotations GFF. " , __FILE__, __LINE__ );
			}
			else if( matching_clones_it.size() == 1 )
			{
				correct_match_it = matching_clones_it[0];
			}
			else/*( matching_clones_it.size() > 1 )*/
			{
				output_text_line( "Multiple clone matches found for clone " + clone_ids_table[i] );
				
				vector <unsigned int> length_diff( matching_clones_it.size() );
				vector <unsigned int> start_diff ( matching_clones_it.size() );
				
				unsigned int min_len_diff 	   = DEFAULT_LARGE_VAL;
				unsigned int min_len_diff_it   = 0;
				
				unsigned int min_start_diff    = DEFAULT_LARGE_VAL;
				unsigned int min_start_diff_it = 0;
				
				
				// For each of the matching clones, calculate the difference (in nucleotides) between the GFF and the Table 1 annotations for:
				//		(a) Sequence Length
				//		(b) Sequence Start Coordinate
				for( unsigned int j = 0; j < matching_clones_it.size(); j++ )
				{
					length_diff[j] = abs( (int)abs( (int)hit_start[ matching_clones_it[j] ] - (int)hit_end[ matching_clones_it[j] ] ) - 
										  (int)abs( (int)hit_start_table[i] 				- (int)hit_end_table[i] 				) );
										  
					start_diff[j]  = abs( (int)hit_start[ matching_clones_it[j] ] - (int)hit_start_table[i] );
					
					output_text_line( "   clone match " + get_str( j ) + " length differs by " + get_str( length_diff[j] ) );
					output_text_line( "   clone match " + get_str( j ) + " start  differs by " + get_str( start_diff[j]  ) );
					

					if( length_diff[j] < min_len_diff )
					{
						min_len_diff = length_diff[j];
						min_len_diff_it = j;
					}
					
					if( start_diff[j] < min_start_diff )
					{
						min_start_diff = start_diff[j];
						min_start_diff_it = j;
					}
				}
				
				
				// If the minimum differences for length and start coordinate correspond to different clones, not enough is known
				//   to allow for accurate selection
				if( min_len_diff_it != min_start_diff_it )
				{
					Errors.handle_error( FATAL, (string)"Error in 'write_Miura_TSS_data_to_csv()': For clone " + clone_ids_table[i] + 
														", the matching clone with the closest length was not " +
														"the same as the clone with the closest start position. " , __FILE__, __LINE__ );
				}
				
				
				// If the minimum differences for length and start coordinate correspond to the same clone, assume it is the correct clone 
				correct_match_it = matching_clones_it[min_len_diff_it];
				
				output_text_line( "" );
				output_text_line( "   selecting match " + get_str( min_len_diff_it ) );
				output_text_line( "" );
				output_text_line( "" );
			}
			
			
			// Add a row to the .csv for the clone's annotation
			TSS_csv.push_back( vector <string>( NUM_CSV_TSS_COLUMNS, "" ) );
			
			TSS_csv.back()[TSS_CSV_COL_NUM_CLONE_ID   ] = clone_ids_table[i];
			TSS_csv.back()[TSS_CSV_COL_NUM_SYST_NAME  ] = gene_name_table[i];
			TSS_csv.back()[TSS_CSV_COL_NUM_START_COORD] = get_str( hit_start[correct_match_it] );
			TSS_csv.back()[TSS_CSV_COL_NUM_END_COORD  ] = get_str( hit_end  [correct_match_it] );
		}
	}
	// ------------------------------
	
	
	
	// ------------------------------
	// Write the compiled 5' UTR annotations to .csv
	if( write_2d_vector_to_csv( PATH_DATA_FOLDER, "Miura_TSS_Data.csv", TSS_csv ) )
	{
		Errors.handle_error( FATAL, "Error in 'write_Miura_TSS_data_to_csv()': Failed to write TSS Data to CSV file. ", __FILE__, __LINE__ );
	}
	// ------------------------------
		
		
	return;
}
//==============================================================================

////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//
// E. UNUSED Non-Member Function Definitions
//
////////////////////////////////////////////////////////////////////////////////

//==============================================================================
/*
int extract_gene_coord_Miura( const TCSV_Contents uORF_table, vector <TORF_Data> * const ORF_Data, const vector <string> yeast_genbank, unsigned int * const uORF_data_start_it )
{
	vector <string> 	  UTR_coord_raw = uORF_table.get_csv_column<string>	     ( MIURA_UTR_COORD_HEADER  );
	vector <string> 	  gene_name     = uORF_table.get_csv_column<string>	     ( MIURA_SYST_NAME_HEADER  );
	vector <unsigned int> UTR_lengths   = uORF_table.get_csv_column<unsigned int>( MIURA_UTR_LENGTH_HEADER );
	
	if( UTR_coord_raw.size() != gene_name.size() || UTR_coord_raw.size() != UTR_lengths.size() )
	{
		return Errors.handle_error( PASS_UP_ONE_LEVEL, "Error in 'extract_gene_coord_Miura()': Size of uORF data vectors didn't match. ", __FILE__, __LINE__ );
	}

	
	*uORF_data_start_it = ORF_Data->size();

	ORF_Data->reserve( ORF_Data->size() + UTR_coord_raw.size() );
	
	unsigned int num_genes_not_found = 0;

	for( unsigned int i = 0; i < UTR_coord_raw.size(); i++ )
	{
		unsigned int UTR_chrom_num;
		unsigned int UTR_start_pos;
		unsigned int UTR_end_pos;
		unsigned int gene_pos_change;
		unsigned int UTR_length;
		unsigned int gene_start_pos;
		bool opposite_strand;
		
		if( extract_UTR_coord_from_Miura_table_row( UTR_coord_raw[i], 
													gene_name[i], 
													UTR_lengths[i], 
													&UTR_chrom_num,    
													&UTR_start_pos,  
													&UTR_end_pos,  
													&opposite_strand,
													&UTR_length  ) )
		{
			return Errors.handle_error( PASS_UP_ONE_LEVEL, "", __FILE__, __LINE__ );
		}
		
		
		unsigned int gene_pos = yeast_genbank[UTR_chrom_num - 1].find( (string)"/locus_tag=\"" + gene_name[i] );
		
		if( gene_pos == string::npos ) 
		{
			ostringstream error_oss;
			error_oss << "Gene name " << gene_name[i] << " not found in chromosome " << UTR_chrom_num;
			Errors.handle_error( NONFATAL, error_oss.str(), __FILE__, __LINE__ );
			
			num_genes_not_found++;
		}
		else
		{
			unsigned int CDS_id_pos = yeast_genbank[UTR_chrom_num - 1].find( "CDS", gene_pos );
			
			if( CDS_id_pos == string::npos || CDS_id_pos > yeast_genbank[UTR_chrom_num - 1].find( "gene ", gene_pos ) ) 
			{
				ostringstream error_oss;
				error_oss << "Complementary strand of gene " << gene_name[i] << " not found in chromosome " << UTR_chrom_num << "\n"
						  << "gene pos: " << gene_pos << "  CDS pos: " << CDS_id_pos << "  next gene id pos: " << yeast_genbank[UTR_chrom_num - 1].find( "gene ", gene_pos ) << "\n"
						  << "genbank: \n"  << yeast_genbank[UTR_chrom_num - 1].substr( gene_pos, (CDS_id_pos - gene_pos + 1 ) );
							
				return Errors.handle_error( PASS_UP_ONE_LEVEL, error_oss.str(), __FILE__, __LINE__ );
			}
			
			unsigned int gene_first_coord_start_pos = find_first_digit_or_char( DIGIT, yeast_genbank[UTR_chrom_num - 1], CDS_id_pos );
			
			if( gene_first_coord_start_pos == string::npos || gene_first_coord_start_pos > yeast_genbank[UTR_chrom_num - 1].find( "gene ", gene_pos ) ) 
			{
				ostringstream error_oss;
				error_oss << "Coordinates of gene " << gene_name[i] << " not found in chromosome " << UTR_chrom_num;
				return Errors.handle_error( PASS_UP_ONE_LEVEL, error_oss.str(), __FILE__, __LINE__ );
			}
			
			unsigned int gene_first_coord_end_pos = find_last_digit_or_char( DIGIT, yeast_genbank[UTR_chrom_num - 1], gene_first_coord_start_pos );
			
			unsigned int gene_second_coord_start_pos = find_first_digit_or_char( DIGIT, yeast_genbank[UTR_chrom_num - 1], ( gene_first_coord_end_pos + 1 ) );
			unsigned int gene_second_coord_end_pos   = find_last_digit_or_char ( DIGIT, yeast_genbank[UTR_chrom_num - 1], gene_second_coord_start_pos );
			
			unsigned int gene_first_coord_len = gene_first_coord_end_pos - gene_first_coord_start_pos + 1;
			unsigned int gene_first_coord = atoi( yeast_genbank[UTR_chrom_num - 1].substr( gene_first_coord_start_pos, gene_first_coord_len ).c_str() );
			
			unsigned int gene_second_coord_len = gene_second_coord_end_pos - gene_second_coord_start_pos + 1;
			unsigned int gene_second_coord = atoi( yeast_genbank[UTR_chrom_num - 1].substr( gene_second_coord_start_pos, gene_second_coord_len ).c_str() );
			
			// Find the real coordinates of the UTR
			if( opposite_strand == FALSE )
			{
				gene_pos_change = (gene_first_coord - 1) - UTR_end_pos;
				
				gene_start_pos = gene_first_coord;
			}
			else*//*( opposite_strand == TRUE )*//*
			{
				gene_pos_change = UTR_start_pos - (gene_second_coord + 1);
				
				gene_start_pos = gene_second_coord;
			}
			
			vector <TFeature> no_untransl_reg;
			TFeature no_fpUTR_introns( string::npos, string::npos );

			ORF_Data->push_back( TORF_Data( UTR_chrom_num, gene_name[i], gene_start_pos, gene_pos_change, opposite_strand, "Miura", no_untransl_reg, no_fpUTR_introns ) );
		}
	}
	
	if( num_genes_not_found != 0 ) 
	{
		ostringstream error_oss;
		error_oss << "Error in 'extract_gene_coord_Miura()': " << num_genes_not_found << " genes were not found.";
		return Errors.handle_error( PASS_UP_ONE_LEVEL, error_oss.str(), __FILE__, __LINE__ );
	}

	return SUCCESSFUL;
}
//==============================================================================



int extract_gene_coord_Miura( const TCSV_Contents uORF_table, vector < TORF_Data > * const ORF_Data, unsigned int * const uORF_data_start_it )
{
	vector <string> UTR_coord_raw     = uORF_table.get_csv_column<string>	   ( MIURA_UTR_COORD_HEADER  );
	vector <string> gene_name         = uORF_table.get_csv_column<string>	   ( MIURA_SYST_NAME_HEADER  );
	vector <unsigned int> UTR_lengths = uORF_table.get_csv_column<unsigned int>( MIURA_UTR_LENGTH_HEADER );
	
	if( UTR_coord_raw.size() != gene_name.size() || UTR_coord_raw.size() != UTR_lengths.size() )
	{
		return Errors.handle_error( PASS_UP_ONE_LEVEL, "Error in 'extract_gene_coord_Miura()': Size of uORF data vectors didn't match. ", __FILE__, __LINE__ );
	}
	

	*uORF_data_start_it = ORF_Data->size();
	ORF_Data->reserve( ORF_Data->size() + UTR_coord_raw.size() );
	

	for( unsigned int i = 0; i < UTR_coord_raw.size(); i++ )
	{
		unsigned int UTR_chrom_num;
		unsigned int UTR_start_pos;
		unsigned int UTR_end_pos;
		unsigned int gene_start_pos;
		bool opposite_strand;
		
		if( extract_UTR_coord_from_Miura_table_row( UTR_coord_raw[i], 
													gene_name[i], 
													UTR_lengths[i], 
													&UTR_chrom_num,    
													&UTR_start_pos,  
													&UTR_end_pos,  
													&opposite_strand ) )
		{
			return Errors.handle_error( PASS_UP_ONE_LEVEL, "", __FILE__, __LINE__ );
		}
		
		
		if    ( opposite_strand == FALSE )  { gene_start_pos = UTR_end_pos   + 1; }
		else*//*( opposite_strand == TRUE )*//* { gene_start_pos = UTR_start_pos - 1; }
		
		vector <TFeature> no_untransl_reg;
		TFeature no_fpUTR_introns( string::npos, string::npos );

		ORF_Data->push_back( TORF_Data( UTR_chrom_num, gene_name[i], gene_start_pos, 0, opposite_strand, "Miura", no_untransl_reg, no_fpUTR_introns ) );
	}
	
	return SUCCESSFUL;
}
//==============================================================================



// Used with previous means of extracting uORFs based on Miura data (UTRs were extracted first, uORFs extracted from the UTRs)

int extract_gene_coord_Miura( const TCSV_Contents uORF_table, vector < TuORF_Data > * const uORF_Data, const vector <string> yeast_genbank, unsigned int * const uORF_data_start_it )
{
	vector <string> 	  UTR_coord_raw = uORF_table.get_csv_column<string>		 ( MIURA_UTR_COORD_HEADER  );
	vector <string> 	  gene_name     = uORF_table.get_csv_column<string>		 ( MIURA_SYST_NAME_HEADER  );
	vector <unsigned int> UTR_lengths   = uORF_table.get_csv_column<unsigned int>( MIURA_UTR_LENGTH_HEADER );
	

	*uORF_data_start_it = uORF_Data->size();

	uORF_Data->reserve( uORF_Data->size() + UTR_coord_raw.size() );
	
	unsigned int num_genes_not_found = 0;

	for( unsigned int i = 0; i < UTR_coord_raw.size(); i++ )
	{
		unsigned int UTR_chrom_num;
		unsigned int UTR_start_pos;
		unsigned int UTR_end_pos;
		unsigned int UTR_pos_change;
		unsigned int UTR_length;
		bool opposite_strand;
		
		if( extract_UTR_coord_from_Miura_table_row( UTR_coord_raw[i], 
													gene_name[i], 
													UTR_lengths[i], 
													&UTR_chrom_num,    
													&UTR_start_pos,  
													&UTR_end_pos,  
													&opposite_strand,
													&UTR_length  ) )
		{
			return Errors.handle_error( PASS_UP_ONE_LEVEL, "", __FILE__, __LINE__ );
		}
		
		
		unsigned int gene_pos = yeast_genbank[UTR_chrom_num - 1].find( (string)"/locus_tag=\"" + gene_name[i] );
		
		if( gene_pos == string::npos ) 
		{
			ostringstream error_oss;
			error_oss << "Gene name " << gene_name[i] << " not found in chromosome " << UTR_chrom_num;
			Errors.handle_error( NONFATAL, error_oss.str(), __FILE__, __LINE__ );
			
			num_genes_not_found++;
		}
		else
		{
			unsigned int CDS_id_pos = yeast_genbank[UTR_chrom_num - 1].find( "CDS", gene_pos );
			
			if( CDS_id_pos == string::npos || CDS_id_pos > yeast_genbank[UTR_chrom_num - 1].find( "gene ", gene_pos ) ) 
			{
				ostringstream error_oss;
				error_oss << "Complementary strand of gene " << gene_name[i] << " not found in chromosome " << UTR_chrom_num << "\n"
						  << "gene pos: " << gene_pos << "  CDS pos: " << CDS_id_pos << "  next gene id pos: " << yeast_genbank[UTR_chrom_num - 1].find( "gene ", gene_pos ) << "\n"
						  << "genbank: \n"  << yeast_genbank[UTR_chrom_num - 1].substr( gene_pos, (CDS_id_pos - gene_pos + 1 ) );
							
				return Errors.handle_error( PASS_UP_ONE_LEVEL, error_oss.str(), __FILE__, __LINE__ );
			}
			
			unsigned int gene_first_coord_start_pos = find_first_digit_or_char( DIGIT, yeast_genbank[UTR_chrom_num - 1], CDS_id_pos );
			
			if( gene_first_coord_start_pos == string::npos || gene_first_coord_start_pos > yeast_genbank[UTR_chrom_num - 1].find( "gene ", gene_pos ) ) 
			{
				ostringstream error_oss;
				error_oss << "Coordinates of gene " << gene_name[i] << " not found in chromosome " << UTR_chrom_num;
				return Errors.handle_error( PASS_UP_ONE_LEVEL, error_oss.str(), __FILE__, __LINE__ );
			}
			
			unsigned int gene_first_coord_end_pos = find_last_digit_or_char( DIGIT, yeast_genbank[UTR_chrom_num - 1], gene_first_coord_start_pos );
			
			unsigned int gene_second_coord_start_pos = find_first_digit_or_char( DIGIT, yeast_genbank[UTR_chrom_num - 1], ( gene_first_coord_end_pos + 1 ) );
			unsigned int gene_second_coord_end_pos   = find_last_digit_or_char ( DIGIT, yeast_genbank[UTR_chrom_num - 1], gene_second_coord_start_pos );
			
			unsigned int gene_first_coord_len = gene_first_coord_end_pos - gene_first_coord_start_pos + 1;
			unsigned int gene_first_coord = atoi( yeast_genbank[UTR_chrom_num - 1].substr( gene_first_coord_start_pos, gene_first_coord_len ).c_str() );
			
			unsigned int gene_second_coord_len = gene_second_coord_end_pos - gene_second_coord_start_pos + 1;
			unsigned int gene_second_coord = atoi( yeast_genbank[UTR_chrom_num - 1].substr( gene_second_coord_start_pos, gene_second_coord_len ).c_str() );
			
			// Find the real coordinates of the UTR
			if( opposite_strand == FALSE )
			{
				UTR_pos_change = UTR_end_pos - (gene_first_coord - 1);
				UTR_end_pos = gene_first_coord - 1;
				UTR_start_pos = UTR_end_pos - UTR_length + 1;
			}
			else //( opposite_strand == TRUE )
			{
				UTR_pos_change = (gene_second_coord + 1) - UTR_start_pos;
				UTR_start_pos = gene_second_coord + 1;
				UTR_end_pos = UTR_start_pos + UTR_length - 1;
			}
			
			uORF_Data->push_back( TuORF_Data(UTR_chrom_num, gene_name[i], UTR_start_pos, UTR_end_pos, UTR_pos_change, opposite_strand, "Miura" ) );
		}
	}
	
	if( num_genes_not_found != 0 ) 
	{
		ostringstream error_oss;
		error_oss << "Error in 'extract_gene_coord_Miura()': " << num_genes_not_found << " genes were not found.";
		return Errors.handle_error( PASS_UP_ONE_LEVEL, error_oss.str(), __FILE__, __LINE__ );
	}

	return SUCCESSFUL;
}*/
//==============================================================================

////////////////////////////////////////////////////////////////////////////////


