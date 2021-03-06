//==============================================================================
// Project	   : uORF
// Name        : uORF__process.cpp
// Author      : Garin Newcomb
// Email       : gpnewcomb@live.com    
// Version     : See "Revision History" below
// Copyright   : Copyright 2014 University of Nebraska-Lincoln
// Description : Source code to process and perform operations on uORF's
//==============================================================================
//
//  Revision History
//      v0.0.0 - 2014/07/16 - Garin Newcomb
//          Initial creation of file, pulling primarily from the previous "uORF_manip.cpp" used with this program
//
//    	Appl Version at Last File Update::  v0.0.x - 2014/07/16 - Garin Newcomb
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

#include <windows.h>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>   	// For find()
#include <cmath>   		// For log2()

using namespace std;

// Project-specific header files:  definitions and related information
#include "defs__general.h"
#include "defs__appl_parameters.h"

// Project-specific header files:  support functions and related
#include "support__file_io.h"
#include "support__general.h"
#include "support__bioinformatics.h"
#include "uORF__compile.h"

// External header files
#include "lbg_clustering.h"

// Header file for this file
#include "uORF__process.h"

//==============================================================================





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

////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//
// D. Non-Member Function Definitions
//
////////////////////////////////////////////////////////////////////////////////

//==============================================================================

void sort_and_consolidate( vector <TORF_Data> * const ORF_Data )
{
	if( ORF_SORTING_METHOD != DONT_SORT_ORFS )
	{
		sort( ORF_Data->begin(), ORF_Data->end(), compare_ORF_positions );

		combine_uORFs_by_gene( ORF_Data );

		for( unsigned int i = 0; i < ORF_Data->size(); i++ )
		{
			ORF_Data->at( i ).sort_uORFs();
		}

		delete_duplicate_uORFs( ORF_Data, SELECT_uORFS_TO_DELETE );
		
	
		vector <TGO_Annotation> All_GO_Annotations;
		parse_GO_hierarchy( &All_GO_Annotations );
		add_GO_info_to_ORFs( ORF_Data, All_GO_Annotations );
		
		
		if( uORFs_TO_PRINT == ORFS_MATCHING_GO_TERMS )
		{
			check_for_matching_GO_terms( ORF_Data );
		}
		

		if( ORF_SORTING_METHOD == SORT_ORFS_BY_GO_TERMS )
		{
			sort_ORFs_by_GO_terms( GO_NAMESPACE_TO_SORT_BY, ORF_Data, All_GO_Annotations );
		}
	}
	
	return;
}
//==============================================================================



void sort_ORFs_by_GO_terms( const string & GO_namespace_to_sort, vector <TORF_Data> * const ORF_Data, const vector <TGO_Annotation> & All_GO_Annotations )
{
	if( ORF_Data->size() == 0 )
	{
		Errors.handle_error( FATAL, (string)"Error in 'sort_ORFs_by_GO_terms()': 'ORF_Data' vector is empty. ", __FILE__, __LINE__ );
	}
	

	bool no_ORFs_w_GO_terms_this_level = TRUE;

	vector < vector<TORF_Data>::iterator > segment_begin_it( 1, ORF_Data->begin() );
	vector < vector<TORF_Data>::iterator > segment_end_it  ( 1, ORF_Data->end  () );
	
	unsigned int namespace_max_level = 1;
	unsigned int namespace_it = 0;
	
	if( GO_namespace_to_sort == GO_BIO_PROC_ID  )
	{
		namespace_max_level = GO_BIO_PROC_MAX_LEVEL;
		namespace_it 		= GO_BIO_PROC_IT;
	}
	else if( GO_namespace_to_sort == GO_CELL_COMP_ID )
	{
		namespace_max_level = GO_CELL_COMP_MAX_LEVEL;
		namespace_it 		= GO_CELL_COMP_IT;
	}
	else if( GO_namespace_to_sort == GO_MOL_FUNC_ID  )
	{
		namespace_max_level = GO_MOL_FUNC_MAX_LEVEL;
		namespace_it 		= GO_MOL_FUNC_IT;
	}
	else
	{
		Errors.handle_error( FATAL, (string)"Error in 'sort_ORFs_by_GO_terms()': GO namespace " + GO_namespace_to_sort + " not recognized. ", __FILE__, __LINE__ );
	}
	
	
	// Determine the ancestry of the most specific GO term for each ORF
	for( unsigned int ORF_it = 0; ORF_it < ORF_Data->size(); ORF_it++ )
	{
		string GO_term_ancestry = "";
		
		if( ORF_Data->at( ORF_it ).most_specific_GO_term[namespace_it].level != DUMMY_GO_LEVEL )
		{
			vector <TGO_Term> parents_of_most_specific = get_all_parents_of( ORF_Data->at( ORF_it ).most_specific_GO_term[namespace_it].name, 
																			 All_GO_Annotations[ namespace_it ] );

			for( unsigned int i = 0; i < parents_of_most_specific.size(); i++ )
			{
				GO_term_ancestry += ( parents_of_most_specific[ parents_of_most_specific.size() - i - 1 ].name + "->" );
			}
		}
		
		ORF_Data->at( ORF_it ).GO_term_sorted_by = ( GO_term_ancestry + ORF_Data->at( ORF_it ).most_specific_GO_term[namespace_it].name );
	}
	
	
	// Perform the sorting
	for( unsigned int curr_level = 1; curr_level <= namespace_max_level; curr_level++ )
	{
		ostringstream output_oss;
		output_oss << "Sorting by GO '" << GO_namespace_to_sort << "' level " << curr_level << " terms...";
		output_text_line( output_oss.str() );
		

		for( unsigned int ORF_it = 0; ORF_it < ORF_Data->size(); ORF_it++ )
		{
			if( ORF_Data->at( ORF_it ).most_specific_GO_term[namespace_it].level > curr_level && ORF_Data->at( ORF_it ).most_specific_GO_term[namespace_it].level != DUMMY_GO_LEVEL )
			{
				vector <TGO_Term> parents_of_most_specific = get_all_parents_of( ORF_Data->at( ORF_it ).most_specific_GO_term[namespace_it].name, 
																				 All_GO_Annotations[ namespace_it ] );
																				 
				ORF_Data->at( ORF_it ).GO_term_to_sort_curr_level = parents_of_most_specific[ ORF_Data->at( ORF_it ).most_specific_GO_term[namespace_it].level - curr_level - 1 ].name;
				
				no_ORFs_w_GO_terms_this_level = FALSE;
			}
			else if( ORF_Data->at( ORF_it ).most_specific_GO_term[namespace_it].level == curr_level )
			{
				ORF_Data->at( ORF_it ).GO_term_to_sort_curr_level = ORF_Data->at( ORF_it ).most_specific_GO_term[namespace_it].name;
				no_ORFs_w_GO_terms_this_level = FALSE;
			}
			else
			{
				ORF_Data->at( ORF_it ).GO_term_to_sort_curr_level = "";
			}
			

			// output_oss.str( "" );
			// output_oss << "term to sort ORF " << ORF_it << " (" << ORF_Data->at( ORF_it ).gene_name << ") by: " << ORF_Data->at( ORF_it ).GO_term_to_sort_curr_level;
			// output_text_line( output_oss.str() );
		}
		
		
		if( no_ORFs_w_GO_terms_this_level == FALSE )
		{
			unsigned int num_segments_to_sort = 0;
			
			if( segment_end_it.size() != segment_begin_it.size() )
			{ 
				Errors.handle_error( FATAL, (string)"Error in 'sort_ORFs_by_GO_terms()': Sizes of segment end and segment begin vectors don't match. ", __FILE__, __LINE__ ); 
			
			}
			for( unsigned int i = 0; i < segment_end_it.size(); i++ )
			{
				stable_sort( segment_begin_it[i], segment_end_it[i], compare_ORF_GO_terms );
			}
		
		
			segment_begin_it.clear();
			segment_end_it.clear();


			string prev_GO_term = ORF_Data->at( 0 ).GO_term_to_sort_curr_level;
			
			if( prev_GO_term != "" )
			{
				segment_begin_it.push_back( ORF_Data->begin() );
			}

			for( unsigned int curr_it = 1; curr_it < ORF_Data->size(); curr_it++ )
			{
				if( prev_GO_term != ORF_Data->at( curr_it ).GO_term_to_sort_curr_level )
				{
					if( prev_GO_term != "" )
					{
						segment_end_it.push_back  ( ORF_Data->begin() + curr_it );
						num_segments_to_sort++;
						
						// output_oss.str( "" );
						// output_oss << "Segment to sort " << num_segments_to_sort << ": " 
						//			  << "begin it: "  << (int)( segment_begin_it.back() - ORF_Data->begin() ) 
						//			  << "   end it: " << (int)( segment_end_it.back() - ORF_Data->begin() ) 
						//			  << "   prev go: " << prev_GO_term;
						// output_text_line( output_oss.str() );
					}
					
					
					if( ORF_Data->at( curr_it ).GO_term_to_sort_curr_level != "" )
					{
						segment_begin_it.push_back( ORF_Data->begin() + curr_it );
					}
					
					prev_GO_term = ORF_Data->at( curr_it ).GO_term_to_sort_curr_level;
				}
			}
			
			if( ORF_Data->size() > 1 && segment_end_it.size() != segment_begin_it.size() )
			{
				num_segments_to_sort++;
				segment_end_it.push_back( ORF_Data->end() );
				
				// output_oss.str( "" );
				// output_oss << "Segment to sort " << num_segments_to_sort << ": " 
				//			  << "begin it: "  << (int)( segment_begin_it.back() - ORF_Data->begin() ) 
				//			  << "   end it: " << (int)( segment_end_it.back() - ORF_Data->begin() ) 
				//			  << "   prev go: " << prev_GO_term;
				// output_text_line( output_oss.str() );
			}
		}
	}

	return;
}

/*
// Unused Code

	// Failed attempt at generating a GO hierarchy based solely on the GO annotations for the genes included in the uORF list
	
	vector <vector <TGO_Annotation> > All_Go_Annotations( NUM_GO_NAMESPACES, vector <TGO_Annotation>( 0, TGO_Annotation( "" ) ) );
	
	for( unsigned int i = 0; i < NUM_GO_NAMESPACES; i++ )
	{
		TCount_Term_Vect num_times_terms_appear;
		
		output_text_line( (string)"Namespace: " + ORF_Data->at( 0 ).GO_Annotations[i].GO_namespace );
		
		vector <string> all_terms;
		
		for( unsigned int j = 0; j < ORF_Data->size(); j++ )
		{
			All_Go_Annotations[i].push_back( TGO_Annotation( ORF_Data->at( j ).GO_Annotations[i].GO_namespace ) );
			
			
			for( unsigned int k = 0; k < ORF_Data->at( j ).GO_Annotations[i].GO_Terms.size(); k++ )
			{
				// Check if the term has already been added (there are multiples because the same term can have multiple evidence codes)
				unsigned int searched_term = 0;
				
				while( searched_term < All_Go_Annotations[i][j].GO_Terms.size() &&
					   All_Go_Annotations[i][j].GO_Terms[searched_term].name != ORF_Data->at( j ).GO_Annotations[i].GO_Terms[k].name )
				{
					searched_term++;	
				}

				if( searched_term == All_Go_Annotations[i][j].GO_Terms.size() )
				{
					All_Go_Annotations[i][j].GO_Terms.push_back( ORF_Data->at( j ).GO_Annotations[i].GO_Terms[k] );
					num_times_terms_appear.modify( ORF_Data->at( j ).GO_Annotations[i].GO_Terms[k].name, INCREMENT );
				}
				

				searched_term = 0;
				
				while( searched_term <all_terms.size() &&
					   all_terms[searched_term] != ORF_Data->at( j ).GO_Annotations[i].GO_Terms[k].name )
				{
					searched_term++;	
				}

				if( searched_term == all_terms.size() )
				{
					all_terms.push_back( ORF_Data->at( j ).GO_Annotations[i].GO_Terms[k].name );
				}
			}		
		}
		

		vector <vector <vector <string> > > terms_appear_with( all_terms.size(), vector <vector <string> >( 0 ) );
		
		for( unsigned int j = 0; j < All_Go_Annotations[i].size(); j++ )
		{
			for( unsigned int k = 0; k < ( All_Go_Annotations[i][j].GO_Terms.size() ); k++ )
			{
				unsigned int term_it = find( all_terms.begin(), all_terms.end(), All_Go_Annotations[i][j].GO_Terms[k].name ) - all_terms.begin();
				terms_appear_with[ term_it ].push_back( vector <string>( 0 ) );
			}
			
			
			for( unsigned int k = 0; (int)k < (int)( All_Go_Annotations[i][j].GO_Terms.size() - 1 ); k++ )
			{
				for( unsigned int l = k + 1; l < All_Go_Annotations[i][j].GO_Terms.size(); l++ )
				{
					unsigned int term_it = find( all_terms.begin(), all_terms.end(), All_Go_Annotations[i][j].GO_Terms[k].name ) - all_terms.begin();
					terms_appear_with[ term_it ].back().push_back( All_Go_Annotations[i][j].GO_Terms[l].name );
					
					term_it = find( all_terms.begin(), all_terms.end(), All_Go_Annotations[i][j].GO_Terms[l].name ) - all_terms.begin();
					terms_appear_with[ term_it ].back().push_back( All_Go_Annotations[i][j].GO_Terms[k].name );
				}
			}
		}
		

		vector < vector <string> > terms_common_to_all( all_terms.size() );
		
		for( unsigned int j = 0; j < terms_appear_with.size(); j++ )
		{
			for( unsigned int k = 0; k < all_terms.size(); k++ )
			{
				if( perc_lists_containing_element( all_terms[k], terms_appear_with[j] ) >= 90 )
				{
					terms_common_to_all[j].push_back( all_terms[k] );
				}
			}
		}
		
		
		TCount_Term_Vect term_num_parents;
		
		for( unsigned int j = 0; j < all_terms.size(); j++ )
		{
			term_num_parents.add_term( all_terms[j], terms_common_to_all[j].size() );
		}
		
		term_num_parents.sort_terms( BY_COUNT, INCREASING );

		vector <TCount_Term> term_num_parents_vect = term_num_parents.get_term_vect();
		
		vector <TCount_Term_Vect> num_times_terms_appear_by_level;
		vector <unsigned int> levels( 0 );
		
		
		unsigned int curr_it = 0;
		unsigned int level_num = term_num_parents_vect[curr_it].get_count();
		
		while( curr_it < term_num_parents_vect.size() )
		{
			level_num = term_num_parents_vect[curr_it].get_count();
			num_times_terms_appear_by_level.push_back( TCount_Term_Vect() );
			levels.push_back( level_num );
			
			while( curr_it < term_num_parents_vect.size() && level_num == term_num_parents_vect[curr_it].get_count() )
			{
				num_times_terms_appear_by_level.back().add_term( term_num_parents_vect[curr_it].get_term(), 
																 num_times_terms_appear.get_term_count( term_num_parents_vect[curr_it].get_term() ) );
				curr_it++;
			}
		}

		
		for( unsigned int j = 0; j < num_times_terms_appear_by_level.size(); j++ )
		{
			ostringstream output_oss;
			output_oss << "level: " << levels[j] << "\n";
			output_text_line( output_oss.str() );
			
			output_oss.str( "" );
			
			num_times_terms_appear_by_level[j].sort_terms( BY_COUNT, DECREASING );
						
			vector <TCount_Term> num_time_term_appears_this_level = num_times_terms_appear_by_level[j].get_term_vect();
						
			for( unsigned int k = 0; k < num_time_term_appears_this_level.size(); k++ )
			{
				output_oss << "  num appearances: " << num_time_term_appears_this_level[k].get_count() << "	term: " << num_time_term_appears_this_level[k].get_term() << "\n";
			}
			
			output_text_line( output_oss.str() );
		}
	}
	*/
//==============================================================================



void check_for_matching_GO_terms( vector <TORF_Data> * const ORF_Data )
{
	string GO_database_contents;
	
	if( read_entire_file_contents( PATH_GO_DATABASE_FOLDER, GO_DATABASE_OBO_FILE_NAME, &GO_database_contents ) )
	{
		Errors.handle_error( FATAL, "Error reading GO database file contents. ", __FILE__, __LINE__ );
	}
	
	
	vector <string> searched_GO_terms = parse_delimited_list<string>( SEARCHED_GO_TERMS, GO_TERM_LIST_DELIMITER );
	
	vector <TCSV_Contents> GO_children;
	GO_children.push_back( TCSV_Contents( PATH_DATA_FOLDER, GO_CHILDREN_LIST_BP_FILE_NAME ) );
	GO_children.push_back( TCSV_Contents( PATH_DATA_FOLDER, GO_CHILDREN_LIST_CC_FILE_NAME ) );
	GO_children.push_back( TCSV_Contents( PATH_DATA_FOLDER, GO_CHILDREN_LIST_MF_FILE_NAME ) );
	
	vector < vector <string> > parents		( NUM_GO_NAMESPACES, vector<string>( 0 ) );
	vector < vector <string> > children_list( NUM_GO_NAMESPACES, vector<string>( 0 ) );
	
	
	for( unsigned int i = 0; i < GO_children.size(); i++ ) 
	{ 
		GO_children[i].parse_csv(); 
		
		parents[i] 		 = GO_children[i].get_csv_column<string>( GO_CHILDREN_LIST_PARENT_HEADER   );
		children_list[i] = GO_children[i].get_csv_column<string>( GO_CHILDREN_LIST_CHILDREN_HEADER );
	}
	
	
	vector <TGO_Annotation> searched_GO_Annotations( NUM_GO_NAMESPACES, TGO_Annotation( "" ) );

	searched_GO_Annotations[GO_BIO_PROC_IT]  = TGO_Annotation( GO_BIO_PROC_ID  );
	searched_GO_Annotations[GO_CELL_COMP_IT] = TGO_Annotation( GO_CELL_COMP_ID );
	searched_GO_Annotations[GO_MOL_FUNC_IT]  = TGO_Annotation( GO_MOL_FUNC_ID  );
	
	
	
	for( unsigned int i = 0; i < searched_GO_terms.size(); i++ )
	{
		/*unsigned int GO_namespace_it = 0;
		unsigned int GO_parent_it 	 = string::npos;
	
		// Find the GO Annotation namespace
		while( GO_namespace_it < searched_GO_Annotations.size() && GO_parent_it == string::npos )
		{
			GO_parent_it = (unsigned int)( find( parents[GO_namespace_it].begin(), parents[GO_namespace_it].end(), searched_GO_terms[i] ) - parents[GO_namespace_it].begin() );
			if( GO_parent_it == parents[GO_namespace_it].size() ) { GO_parent_it = string::npos; }
			
			GO_namespace_it++;
		}
		
		if( GO_namespace_it >= searched_GO_Annotations.size() )
		{
			Errors.handle_error( FATAL, (string)"Error in 'TORF_Data::check_for_matching_GO_terms()': Term " + searched_GO_terms[i] + 
												" was not found in the list of GO term parents. ", __FILE__, __LINE__ );
		}
		
		vector <string> children_terms = parse_delimited_list<string>( children_list[GO_namespace_it][GO_parent_it], DEFAULT_DELIMITER );
		*/
		
		
		const string TERM_ID 	  = "\nname: ";
		const string NAMESPACE_ID = "\nnamespace: ";

		unsigned int term_start_pos = GO_database_contents.find( TERM_ID + searched_GO_terms[i] );
	
		unsigned int namespace_start_pos = GO_database_contents.find( NAMESPACE_ID, term_start_pos ) + NAMESPACE_ID.size();
		unsigned int namespace_end_pos   = GO_database_contents.find( '\n', namespace_start_pos ) -1;

		if( namespace_start_pos == string::npos )
		{
			Errors.handle_error( FATAL, "Error in 'check_for_matching_GO_terms()': Failed to find the end of the specified term. ", __FILE__, __LINE__ );
		}
		
		if( namespace_start_pos - NAMESPACE_ID.size() == string::npos )
		{
			Errors.handle_error( FATAL, "Error in 'check_for_matching_GO_terms()': Failed to find the end of the namespace. ", __FILE__, __LINE__ );
		}
		
		if( namespace_end_pos + 1 == string::npos )
		{
			Errors.handle_error( FATAL, "Error in 'check_for_matching_GO_terms()': Failed to find the end of the namespace. ", __FILE__, __LINE__ );
		}
		
		string GO_namespace = GO_database_contents.substr( namespace_start_pos, ( namespace_end_pos - namespace_start_pos + 1 ) );
		
		vector <string> children_terms = get_all_children_terms( GO_database_contents, searched_GO_terms[i] );
		

		unsigned int GO_namespace_it = 0;
				
		if	   ( GO_namespace == GO_BIO_PROC_ID  ) { GO_namespace_it = GO_BIO_PROC_IT;  }
		else if( GO_namespace == GO_CELL_COMP_ID ) { GO_namespace_it = GO_CELL_COMP_IT; }
		else if( GO_namespace == GO_MOL_FUNC_ID  ) { GO_namespace_it = GO_MOL_FUNC_IT;  }
		else
		{
			Errors.handle_error( FATAL, (string)"Error in 'check_for_matching_GO_terms()': GO namespace found (" + GO_namespace + ") is not recognized. ", __FILE__, __LINE__ );
		}
		
		
		
		
		
		
		
		searched_GO_Annotations[GO_namespace_it].GO_Terms.push_back( TGO_Term( searched_GO_terms[i], "", DUMMY_GO_LEVEL, "" ) );
		
		
		for( unsigned int j = 0; j < children_terms.size(); j++ )
		{
			output_text_line( (string)"Child term " + get_str( j ) + " before: " + children_terms[j] );
			searched_GO_Annotations[GO_namespace_it].GO_Terms.push_back( TGO_Term( children_terms[j], "", DUMMY_GO_LEVEL, "" ) );
			output_text_line( (string)"Child term " + get_str( j ) + " after: " + searched_GO_Annotations[GO_namespace_it].GO_Terms.back().name );
		}
	}
	
	
	
	for( unsigned int i = 0; i < ORF_Data->size(); i++ )
	{
		for( unsigned int j = 0; j < NUM_GO_NAMESPACES; j++ )
		{
			for( unsigned int k = 0; k < searched_GO_Annotations[j].GO_Terms.size(); k++ )
			{
				if( ORF_Data->at( i ).is_GO_term_present( j, searched_GO_Annotations[j].GO_Terms[k].name ) == TRUE )
				{
					ORF_Data->at( i ).matches_GO_term = TRUE;
				}
			}
		}
	}
	

	
	return;
}
//==============================================================================



bool compare_ORF_positions( const TORF_Data & ORF_1, const TORF_Data & ORF_2 )
{
	if     ( ORF_1.chrom_num < ORF_2.chrom_num ) { return ORF_1_BEFORE_ORF_2; }
	else if( ORF_1.chrom_num > ORF_2.chrom_num ) { return ORF_2_BEFORE_ORF_1; }
	else
	{
		if    ( ORF_1.gene_CDS.start_coord <  ORF_2.gene_CDS.start_coord )   { return ORF_1_BEFORE_ORF_2; }
		else/*( ORF_1.gene_CDS.start_coord >= ORF_2.gene_CDS.start_coord )*/ { return ORF_2_BEFORE_ORF_1; }
	}
	
	// Should never get here
	return ORF_2_BEFORE_ORF_1;
}
//==============================================================================



bool compare_ORF_GO_terms( const TORF_Data & ORF_1, const TORF_Data & ORF_2 )
{
	if( ORF_1.GO_term_to_sort_curr_level.compare( ORF_2.GO_term_to_sort_curr_level ) > 0 ) 
	{ 
		if	  ( ORF_2.GO_term_to_sort_curr_level == "" )  { return ORF_1_BEFORE_ORF_2; }
		else/*( ORF_2.GO_term_to_sort_curr_level != "" )*/{ return ORF_2_BEFORE_ORF_1; } 
	}
	else if( ORF_1.GO_term_to_sort_curr_level.compare( ORF_2.GO_term_to_sort_curr_level ) < 0 )
	{
		if	  ( ORF_1.GO_term_to_sort_curr_level == "" )  { return ORF_2_BEFORE_ORF_1; }
		else/*( ORF_1.GO_term_to_sort_curr_level != "" )*/{ return ORF_1_BEFORE_ORF_2; } 
	}
	else /*if( ORF_1.GO_term_to_sort_curr_level != "" )*/
	{
		return compare_ORF_positions( ORF_1, ORF_2 );
	}

	// If equivalent, indicate 2 is before 1 (to obey the 'std::sort()' weak ordering requirement)
	return ORF_2_BEFORE_ORF_1;
}
//==============================================================================



void write_uORFs_to_csv( const vector <TORF_Data> & ORF_Data, const vector <Tenum_uORF_CSV_columns> & col_to_write, const unsigned int select_uORFs, const bool & file_naming_method )
{
	vector <vector <string> > vector_to_write( 1, vector <string> ( 0, "" ) );
	
	vector <Tenum_uORF_CSV_columns> col_to_write_cpy = col_to_write;
	
	// Add all the desired column headers
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_CHROM_NUM 		   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_CHROM_NUM_HEADER		    ); }											
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_SYST_NAME 		   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_SYST_NAME_HEADER		    ); }                                    
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_GENE_CONTEXT 	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_GENE_CONTEXT_HEADER	    ); }                         
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_AVG_TSS_POS 	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_AVG_CAP_DIST_HEADER	    ); }                         
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_TSS_REL_POS 	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_TSS_REL_POS_HEADER	    ); }                         
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_SINGLE_PEAK_TSS   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_SINGLE_PEAK_TSS_HEADER   ); }                         
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_TL_SHAPE_INDEX    ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_TL_SHAPE_INDEX_HEADER    ); }                         
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_GENE_AUGCAI 	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_GENE_AUGCAI_HEADER	    ); }                                
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_GENE_CHANGE 	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_GENE_CHANGE_HEADER	    ); }                             
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_CDS_RIB_RPKM 	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_CDS_RIB_RPKM_HEADER	    ); }                        
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_CDS_mRNA_RPKM 	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_CDS_mRNA_RPKM_HEADER	    ); }                        
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_TRANSL_CORR 	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_TRANSL_CORR_HEADER	    ); }                        
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_RPM_READS 	  	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_RPM_READS_HEADER	  	    ); }                        
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_TRANSL_EFF 	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_TRANSL_EFF_HEADER	    ); }                        
	
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_H_UPF1_RATIO	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_H_UPF1_RATIO_HEADER	    ); }                        
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_H_UPF2_RATIO	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_H_UPF2_RATIO_HEADER	    ); }                        
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_H_UPF3_RATIO	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_H_UPF3_RATIO_HEADER	    ); }                        
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_H_UPF1_Q_VAL	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_H_UPF1_Q_VAL_HEADER	    ); }                        
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_H_UPF2_Q_VAL	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_H_UPF2_Q_VAL_HEADER	    ); }                        
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_H_UPF3_Q_VAL	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_H_UPF3_Q_VAL_HEADER	    ); }                        
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_L_UPF1_RATIO	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_L_UPF1_RATIO_HEADER	    ); }                        
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_L_UPF2_RATIO	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_L_UPF2_RATIO_HEADER	    ); }                        
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_L_UPF3_RATIO	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_L_UPF3_RATIO_HEADER	    ); }                        
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_L_UPF123_RATIO	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_L_UPF123_RATIO_HEADER    ); }                        
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_L_UPF1_Q_VAL	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_L_UPF1_Q_VAL_HEADER	    ); }                        
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_L_UPF2_Q_VAL	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_L_UPF2_Q_VAL_HEADER	    ); }                        
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_L_UPF3_Q_VAL	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_L_UPF3_Q_VAL_HEADER	    ); }                        
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_L_UPF123_Q_VAL	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_L_UPF123_Q_VAL_HEADER    ); }                                                                                                                                                                                  
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_PUB1_ZSCORE	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_PUB1_ZSCORE_HEADER	    ); } 
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_W_PUB1_HL	   	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_W_PUB1_HL_HEADER	    	); } 
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_WO_PUB1_HL	   	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_WO_PUB1_HL_HEADER	    ); } 
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_PUB1_STABILITY	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_PUB1_STABILITY_HEADER    ); } 
  //if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_NO_NMD_mRNA	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_NO_NMD_mRNA_HEADER	    ); }
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_NMD_TARGET	  	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_NMD_TARGET_HEADER	    ); }
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_GUAN_W_NMD_HL     ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_GUAN_W_NMD_HL_HEADER	    ); }
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_GUAN_WO_NMD_HL    ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_GUAN_WO_NMD_HL_HEADER	); }
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_GUAN_FCR  	 	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_GUAN_FCR_HEADER	    	); }
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_GUAN_P_VAL   	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_GUAN_P_VAL_HEADER	    ); }
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_HGN_UPF1_RATIO	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_HGN_UPF1_RATIO_HEADER    ); } 
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_HGN_PUB1_RATIO	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_HGN_PUB1_RATIO_HEADER    ); } 
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_HGN_UPF1_Q_VAL	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_HGN_UPF1_Q_VAL_HEADER    ); } 
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_HGN_PUB1_Q_VAL	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_HGN_PUB1_Q_VAL_HEADER    ); } 
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_JHNS_mRNA_BINDING ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_JHNS_mRNA_BINDING_HEADER ); } 
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_JHNS_mRNA_DECAY   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_JHNS_mRNA_DECAY_HEADER   ); } 
                                                                                                                                                                                                                                                                                                                                                                         
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_GO_TERM_SORTED    ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_GO_TERM_SORTED_HEADER    ); }                        
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_GO_BIO_PROC 	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_GO_BIO_PROC_HEADER	    ); }        
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_GO_CELL_COMP 	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_GO_CELL_COMP_HEADER	    ); }      
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_GO_MOL_FUNC 	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_GO_MOL_FUNC_HEADER	    ); }       
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_NUM_uORFs 	  	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_NUM_uORFs_HEADER	  	    ); }       
	                                                                                                                                                                                     
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_SOURCE		  	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_SOURCE_HEADER			); }                                           
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_EVIDENCE_TYPE	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_EVIDENCE_TYPE_HEADER	    ); }                                           
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_uORF_POS		   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_uORF_POS_HEADER		    ); }                        
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_uORF_LEN		   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_uORF_LEN_HEADER		    ); }                        
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_uORF_REL_POS	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_uORF_REL_POS_HEADER	    ); }                       
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_uORF_CAP_DIST	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_uORF_CAP_DIST_HEADER	    ); }                       
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_uORF_IN_TRANSCR   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_uORF_IN_TRANSCR_HEADER	); }                       
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_uORF_TO_TSS	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_uORF_TO_TSS_HEADER	    ); }                       
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_AUGCAI	 	  	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_AUGCAI_HEADER			); }                                   
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_CDI	 	  	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_CDI_HEADER			    ); }                                   
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_PROB_OF_TRANSL	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_PROB_OF_TRANSL_HEADER 	); }                                   
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_RIBOSOMES	 	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_RIBOSOMES_HEADER		    ); }                              
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_mRNA	 	  	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_mRNA_HEADER		  	    ); }                              
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_TRANSL_EFFECT 	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_TRANSL_EFFECTS_HEADER	); }          
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_CLUSTER 	   	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_CLUSTER_HEADER		    ); }          
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_CLUST_DIST 	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_CLUST_DIST_HEADER		); }          
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_uORF_CONTEXT 	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_uORF_CONTEXT_HEADER	    ); }          
	                                                                                                                
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_PROBLEM 		   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_PROBLEM_HEADER		    ); }                                   
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_uORF_IN_FRAME	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_uORF_IN_FRAME_HEADER	    ); }                  
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_START_CODON 	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_START_CODON_HEADER	    ); }                         
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_START_MOVED 	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_START_MOVED_HEADER	    ); }                         
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_PAST_GENE 		   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_PAST_GENE_HEADER		    ); }                             
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_LEN_CHANGE	 	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_LEN_CHANGE_HEADER		); }          
  //if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_TSS_REL	 	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_TSS_REL_HEADER		    ); }          
	                                                                                                                                                                                     
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_EXT_uORF_CONTEXT  ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_EXT_uORF_CONTEXT_HEADER  ); }                       
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_EXT_GENE_CONTEXT  ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_EXT_GENE_CONTEXT_HEADER  ); }                        
	if( find( col_to_write.begin(), col_to_write.end(), uORF_CSV_COL_NUM_uORF		 	   ) != col_to_write.end() ) { vector_to_write[0].push_back( uORF_LIST_uORF_HEADER			    ); }                                        
	

	// Make sure the list is in ascending order (necessary for 'TORF_Data::form_vector_for_csv_rows()' to work properly)
	sort( col_to_write_cpy.begin(), col_to_write_cpy.end() );
	

	for( unsigned int i = 0; i < ORF_Data.size(); i++ )
	{
		vector <vector <string> > temp_vect = ORF_Data[i].form_vector_for_csv_rows( col_to_write_cpy, select_uORFs );
		vector_to_write.insert( vector_to_write.end(), temp_vect.begin(), temp_vect.end() );
	}


	if( write_2d_vector_to_csv( PATH_OUTPUT_FOLDER, get_uORF_list_file_name( file_naming_method, WRITE ), vector_to_write ) )
	{
		Errors.handle_error( FATAL, "Error in 'write_uORFs_to_csv()': Failed to write uORF list to CSV file. ", __FILE__, __LINE__ );
	}
	
	
	return;
}
//==============================================================================



void write_uORFs_to_csv( const vector <TORF_Data> & ORF_Data, const unsigned int col_selection_type, const unsigned int select_uORFs, const bool & file_naming_method )
{
	vector <Tenum_uORF_CSV_columns> col_to_write( 0, uORF_CSV_COL_NUM_CHROM_NUM );

	if( col_selection_type == ALL_COLUMNS || col_selection_type == ALL_PERTINENT_COLUMNS || col_selection_type == CANONICAL_uORF_COLUMNS )
	{
			col_to_write.push_back( uORF_CSV_COL_NUM_CHROM_NUM 		   );
			col_to_write.push_back( uORF_CSV_COL_NUM_SYST_NAME 		   );
			col_to_write.push_back( uORF_CSV_COL_NUM_GENE_CONTEXT 	   );
			col_to_write.push_back( uORF_CSV_COL_NUM_AVG_TSS_POS 	   );
			col_to_write.push_back( uORF_CSV_COL_NUM_TSS_REL_POS 	   );
			col_to_write.push_back( uORF_CSV_COL_NUM_SINGLE_PEAK_TSS   );
			col_to_write.push_back( uORF_CSV_COL_NUM_TL_SHAPE_INDEX    );
			col_to_write.push_back( uORF_CSV_COL_NUM_GENE_AUGCAI 	   );
		                                                               
		                                                               
		if( col_selection_type == ALL_COLUMNS )                        
		{                                                              
			col_to_write.push_back( uORF_CSV_COL_NUM_GENE_CHANGE 	   ); 
		}                                                              
		                                                               
		                                                               
			col_to_write.push_back( uORF_CSV_COL_NUM_CDS_RIB_RPKM 	   );
			col_to_write.push_back( uORF_CSV_COL_NUM_CDS_mRNA_RPKM 	   );
			col_to_write.push_back( uORF_CSV_COL_NUM_TRANSL_CORR 	   );
			col_to_write.push_back( uORF_CSV_COL_NUM_RPM_READS	 	   );
			col_to_write.push_back( uORF_CSV_COL_NUM_TRANSL_EFF 	   );
			col_to_write.push_back( uORF_CSV_COL_NUM_H_UPF1_RATIO	   );
			col_to_write.push_back( uORF_CSV_COL_NUM_H_UPF2_RATIO	   );
			col_to_write.push_back( uORF_CSV_COL_NUM_H_UPF3_RATIO	   );
			col_to_write.push_back( uORF_CSV_COL_NUM_H_UPF1_Q_VAL	   );
			col_to_write.push_back( uORF_CSV_COL_NUM_H_UPF2_Q_VAL	   );
			col_to_write.push_back( uORF_CSV_COL_NUM_H_UPF3_Q_VAL	   );
			col_to_write.push_back( uORF_CSV_COL_NUM_L_UPF1_RATIO	   );
			col_to_write.push_back( uORF_CSV_COL_NUM_L_UPF2_RATIO	   );
			col_to_write.push_back( uORF_CSV_COL_NUM_L_UPF3_RATIO	   );
			col_to_write.push_back( uORF_CSV_COL_NUM_L_UPF123_RATIO	   );
			col_to_write.push_back( uORF_CSV_COL_NUM_L_UPF1_Q_VAL	   );
			col_to_write.push_back( uORF_CSV_COL_NUM_L_UPF2_Q_VAL	   );
			col_to_write.push_back( uORF_CSV_COL_NUM_L_UPF3_Q_VAL	   );
			col_to_write.push_back( uORF_CSV_COL_NUM_L_UPF123_Q_VAL	   );
			
			col_to_write.push_back( uORF_CSV_COL_NUM_PUB1_ZSCORE	   );
			col_to_write.push_back( uORF_CSV_COL_NUM_W_PUB1_HL	   	   );
			col_to_write.push_back( uORF_CSV_COL_NUM_WO_PUB1_HL	   	   );
			col_to_write.push_back( uORF_CSV_COL_NUM_PUB1_STABILITY	   );
		  //col_to_write.push_back( uORF_CSV_COL_NUM_NO_NMD_mRNA	   );
			col_to_write.push_back( uORF_CSV_COL_NUM_NMD_TARGET	  	   );
			col_to_write.push_back( uORF_CSV_COL_NUM_GUAN_W_NMD_HL     );
			col_to_write.push_back( uORF_CSV_COL_NUM_GUAN_WO_NMD_HL    );
			col_to_write.push_back( uORF_CSV_COL_NUM_GUAN_FCR  	 	   );
			col_to_write.push_back( uORF_CSV_COL_NUM_GUAN_P_VAL   	   );
			col_to_write.push_back( uORF_CSV_COL_NUM_HGN_UPF1_RATIO	   );
			col_to_write.push_back( uORF_CSV_COL_NUM_HGN_PUB1_RATIO	   );
			col_to_write.push_back( uORF_CSV_COL_NUM_HGN_UPF1_Q_VAL	   );
			col_to_write.push_back( uORF_CSV_COL_NUM_HGN_PUB1_Q_VAL	   );
			col_to_write.push_back( uORF_CSV_COL_NUM_JHNS_mRNA_BINDING );
			col_to_write.push_back( uORF_CSV_COL_NUM_JHNS_mRNA_DECAY   );
			

		if( ORF_SORTING_METHOD == SORT_ORFS_BY_GO_TERMS )                       
		{  	
			col_to_write.push_back( uORF_CSV_COL_NUM_GO_TERM_SORTED   );
		}	
			
			
			col_to_write.push_back( uORF_CSV_COL_NUM_GO_BIO_PROC 	  );
			col_to_write.push_back( uORF_CSV_COL_NUM_GO_CELL_COMP 	  );
			col_to_write.push_back( uORF_CSV_COL_NUM_GO_MOL_FUNC 	  );
			col_to_write.push_back( uORF_CSV_COL_NUM_NUM_uORFs 	  	  );
																	   
			col_to_write.push_back( uORF_CSV_COL_NUM_SOURCE		  	  );
			col_to_write.push_back( uORF_CSV_COL_NUM_EVIDENCE_TYPE	  );
			col_to_write.push_back( uORF_CSV_COL_NUM_uORF_POS		  );
			col_to_write.push_back( uORF_CSV_COL_NUM_uORF_LEN		  );
			col_to_write.push_back( uORF_CSV_COL_NUM_uORF_REL_POS	  );
			col_to_write.push_back( uORF_CSV_COL_NUM_uORF_CAP_DIST	  );
			col_to_write.push_back( uORF_CSV_COL_NUM_uORF_IN_TRANSCR  );
			col_to_write.push_back( uORF_CSV_COL_NUM_uORF_TO_TSS	  );
			col_to_write.push_back( uORF_CSV_COL_NUM_AUGCAI	 	  	  );
			col_to_write.push_back( uORF_CSV_COL_NUM_CDI	 	  	  );
			col_to_write.push_back( uORF_CSV_COL_NUM_PROB_OF_TRANSL	  );
			col_to_write.push_back( uORF_CSV_COL_NUM_RIBOSOMES	 	  );
			col_to_write.push_back( uORF_CSV_COL_NUM_mRNA	 	 	  );
			col_to_write.push_back( uORF_CSV_COL_NUM_TRANSL_EFFECT 	  );
			col_to_write.push_back( uORF_CSV_COL_NUM_CLUSTER 	   	  );
			col_to_write.push_back( uORF_CSV_COL_NUM_CLUST_DIST 	  );
			col_to_write.push_back( uORF_CSV_COL_NUM_uORF_CONTEXT 	  );
			
                                                            
		if( col_selection_type != CANONICAL_uORF_COLUMNS )            
		{                                                             
			col_to_write.push_back( uORF_CSV_COL_NUM_PROBLEM 		  );
			col_to_write.push_back( uORF_CSV_COL_NUM_uORF_IN_FRAME	  );
			col_to_write.push_back( uORF_CSV_COL_NUM_START_CODON 	  );
			col_to_write.push_back( uORF_CSV_COL_NUM_START_MOVED 	  );
			col_to_write.push_back( uORF_CSV_COL_NUM_PAST_GENE 		  );
			col_to_write.push_back( uORF_CSV_COL_NUM_LEN_CHANGE	 	  );	
		}
		     
		  //col_to_write.push_back( uORF_CSV_COL_NUM_TSS_REL	 	  );
			   
			col_to_write.push_back( uORF_CSV_COL_NUM_EXT_uORF_CONTEXT );
			col_to_write.push_back( uORF_CSV_COL_NUM_EXT_GENE_CONTEXT );
			col_to_write.push_back( uORF_CSV_COL_NUM_uORF		 	  );
	}
	else 
	{
		Errors.handle_error( FATAL, "Error in 'write_uORFs_to_csv()': Column selection type passed is unrecognized. ", __FILE__, __LINE__ );
	}
	
	
	write_uORFs_to_csv( ORF_Data, col_to_write, select_uORFs, file_naming_method );
	
	return;
}
//==============================================================================



int write_gene_list_to_file( const vector <TORF_Data> & ORF_Data )
{
	string file_contents_to_write_str = "";
	
	for( unsigned int i = 0; i < ORF_Data.size(); i++ )
	{
		file_contents_to_write_str.append( ORF_Data[i].gene_name + (string)"\n" );
	}
	

	if( write_file_contents( PATH_OUTPUT_FOLDER, GENE_LIST_FILE_NAME, file_contents_to_write_str, TRUE ) )
	{
		return Errors.handle_error( PASS_UP_ONE_LEVEL, (string)"Error in 'write_gene_list_to_file()': " );
	}
	
	output_text_line( (string)"- Successfully wrote gene list to file '" + PATH_OUTPUT_FOLDER + PATH_FOLDER_SEPARATOR + GENE_LIST_FILE_NAME + "'" );
	
	return SUCCESSFUL;
}
//==============================================================================



string get_uORF_list_file_name( const bool & file_naming_method, const bool read_or_write )
{
	string uORF_list_file_name = "";
	
	if( file_naming_method == REQUEST_FILE_NAME )
	{
		do
		{
			const string FILE_NAME_PROMPT = (string)"Input uORF list file name " + ( read_or_write == READ ? "for reading" : "for writing" ) + " (must have '.csv' extension): ";
			uORF_list_file_name = prompt_for_input<string>( FILE_NAME_PROMPT, PRINT_PROMPT_TO_LOG );
			
		} while( uORF_list_file_name.find( CSV_FILE_EXTENSION ) == string::npos );
	}
	else
	{
		int sorting_method;

		if( read_or_write == READ )
		{ 
			sorting_method = PREV_ORF_SORTING_METHOD;
		}
		else/*(  read_or_write == WRITE )*/
		{
			if	  ( ORF_SORTING_METHOD == DONT_SORT_ORFS )  { sorting_method = PREV_ORF_SORTING_METHOD; }
			else/*( ORF_SORTING_METHOD == DONT_SORT_ORFS )*/{ sorting_method = ORF_SORTING_METHOD; 		}
		}
		
		
		// Add the uORF selection
		uORF_list_file_name = (string)uORF_LIST_FILE_NAME_PREFIX + ( uORFs_TO_PRINT == CANONICAL_uORFS_ONLY ? "__canonical_only" : "__all" );
		
		
		// Add the sorting method
		if( sorting_method == SORT_ORFS_BY_GO_TERMS )
		{
			uORF_list_file_name += 	"__sorted_by_" + (string)GO_NAMESPACE_TO_SORT_BY;	 		
		}
		else if( sorting_method == SORT_ORFS_BY_POSITION )
		{
			uORF_list_file_name += 	"__sorted_by_position";	
		}
		else if( sorting_method == DONT_SORT_ORFS )
		{
			uORF_list_file_name += "__not_sorted";	
		}
		else
		{
			Errors.handle_error( FATAL, "Error in 'generate_uORF_list_file_name()': ORF sorting method is not recognized. ", __FILE__, __LINE__ );
		}
		
		uORF_list_file_name += CSV_FILE_EXTENSION;
	}
	
	
	return uORF_list_file_name;
}
//==============================================================================



void write_sequences_to_fasta( const std::vector <TORF_Data> & ORF_Data )
{
	string ORF_context_fasta   = "";
	string uORF_sequence_fasta = "";
	string uORF_context_fasta  = "";
	
	vector <string> uORF_clusters_sequence_fasta( NUM_CLUSTERS, "" );
	vector <string> uORF_clusters_context_fasta ( NUM_CLUSTERS, "" );
	
	
	for( unsigned int i = 0; i < ORF_Data.size(); i++ )
	{
		ORF_context_fasta.append( (string)FASTA_SEQ_START_ID + ORF_Data[i].gene_name + "\n" + 
												ORF_Data[i].ext_gene_start_context   + "\n" );
												
		vector <TuORF_Data> uORFs = ORF_Data[i].get_uORFs();
		
		for( unsigned int j = 0; j < uORFs.size(); j++ )
		{
			uORF_sequence_fasta.append( (string)FASTA_SEQ_START_ID + ORF_Data[i].gene_name + "_uORF" + get_str( j ) + "\n" + 
												uORFs[j].content 									  				+ "\n" );
												
			uORF_context_fasta.append ( (string)FASTA_SEQ_START_ID + ORF_Data[i].gene_name + "_uORF" + get_str( j ) + "\n" + 
												uORFs[j].ext_start_context 							  				+ "\n" );
				
				
			uORF_clusters_sequence_fasta[ uORFs[j].cluster ].append( (string)FASTA_SEQ_START_ID + ORF_Data[i].gene_name + "_uORF" + get_str( j ) + "\n" + 
																			 uORFs[j].content 									  				 + "\n" );
												
			uORF_clusters_context_fasta[ uORFs[j].cluster ].append ( (string)FASTA_SEQ_START_ID + ORF_Data[i].gene_name + "_uORF" + get_str( j ) + "\n" + 
																			 uORFs[j].ext_start_context 							  			 + "\n" );									
												
												
		}
	}
	

	if( write_file_contents( PATH_OUTPUT_FOLDER, ORF_CONTEXT_FASTA_FILE_NAME, ORF_context_fasta ) )
	{
		Errors.handle_error( FATAL, (string)"Error in 'write_sequences_to_fasta()': Failed to write list of ORF contexts. ", __FILE__, __LINE__ );
	}
	
	if( write_file_contents( PATH_OUTPUT_FOLDER, uORF_SEQUENCE_FASTA_FILE_NAME, uORF_sequence_fasta ) )
	{
		Errors.handle_error( FATAL, (string)"Error in 'write_sequences_to_fasta()': Failed to write list of uORF sequences. ", __FILE__, __LINE__  );
	}
	
	if( write_file_contents( PATH_OUTPUT_FOLDER, uORF_CONTEXT_FASTA_FILE_NAME, uORF_context_fasta ) )
	{
		Errors.handle_error( FATAL, (string)"Error in 'write_sequences_to_fasta()': Failed to write list of uORF contexts. ", __FILE__, __LINE__ );
	}
	
	
	for( unsigned int i = 0; i < NUM_CLUSTERS; i++ )
	{
		if( write_file_contents( PATH_CLUSTER_OUTPUT_FOLDER, (string)uORF_SEQUENCE_CLUSTER + get_str( i ) + FASTA_FILE_EXTENSION, uORF_clusters_sequence_fasta[i] ) )
		{
			Errors.handle_error( FATAL, (string)"Error in 'write_sequences_to_fasta()': Failed to write list of " +
											    "uORF sequences for cluster " + get_str( i ) + ". ", __FILE__, __LINE__  );
		}
		
		if( write_file_contents( PATH_CLUSTER_OUTPUT_FOLDER, (string)uORF_CONTEXT_CLUSTER + get_str( i ) + FASTA_FILE_EXTENSION, uORF_clusters_context_fasta[i]   ) )
		{
			Errors.handle_error( FATAL, (string)"Error in 'write_sequences_to_fasta()': Failed to write list of " +
												"uORF contexts for cluster "  + get_str( i ) + ". ", __FILE__, __LINE__ );
		}
	}
	

	return;
}
//==============================================================================



void count_nucleotides( const TFasta_Content & S_Cerevisiae_Chrom )
{
	unsigned int num_As = 0;
	unsigned int num_Ts = 0;
	unsigned int num_Cs = 0;
	unsigned int num_Gs = 0;
	
	
	for( unsigned int i = 0; i < S_Cerevisiae_Chrom.sequence.size(); i++ )
	{
		for( unsigned int j = 0; j < S_Cerevisiae_Chrom.sequence[i].size(); j++ )
		{
			switch( S_Cerevisiae_Chrom.sequence[i][j] )
			{
				case 'A' : 	num_As++;
							break;
							
				case 'T' : 	num_Ts++;
							break;
							
				case 'C' : 	num_Cs++;
							break;
							
				case 'G' : 	num_Gs++;
							break;
							
				default  : 	break;
			}
		}
	}
	
	
	output_text_line( (string)"Num A's: " + get_str( num_As ) );
	output_text_line( (string)"Num T's: " + get_str( num_Ts ) );
	output_text_line( (string)"Num C's: " + get_str( num_Cs ) );
	output_text_line( (string)"Num G's: " + get_str( num_Gs ) );
	
	return;
}
//==============================================================================



void calc_joint_prob_nt_genes( const TFasta_Content & S_Cerevisiae_Chrom, const vector <TORF_Data> & ORF_Data, vector <vector <double> > * const joint_prob_nt, vector <double> * const marginal_prob_nt )
{
	vector <vector <unsigned int> > num_instances_each_pair( MAX_NUM_BASES_APART - MIN_NUM_BASES_APART + 1, vector <unsigned int>( NUM_NUCLEOTIDE_PAIRS, 0 ) );
	*joint_prob_nt = vector <vector <double> >   		   ( MAX_NUM_BASES_APART - MIN_NUM_BASES_APART + 1, vector <double		>( NUM_NUCLEOTIDE_PAIRS, 0 ) );
	
	vector <unsigned int> num_instances_each_distance( MAX_NUM_BASES_APART - MIN_NUM_BASES_APART + 1, 0 );
	
	
	vector <unsigned int> num_instances_each_nt( NUM_DIFF_NT, 0 );
	*marginal_prob_nt = vector <double>		   ( NUM_DIFF_NT, 0 );
	
	unsigned int num_nt= 0;
	
	
	vector <vector <string> > chrom_seq( 2, vector <string>( S_Cerevisiae_Chrom.sequence.size(), "" ) );
	
	for( unsigned int chrom_it = 0; chrom_it < S_Cerevisiae_Chrom.sequence.size(); chrom_it++ )
	{
		chrom_seq[STRAND_W][chrom_it] = S_Cerevisiae_Chrom.sequence[ chrom_it ];
		
		chrom_seq[STRAND_C][chrom_it] = get_inverted_sequence( chrom_seq[STRAND_W][chrom_it] );
		chrom_seq[STRAND_C][chrom_it] = get_opposite_strand  ( chrom_seq[STRAND_C][chrom_it] );
	}


	for( unsigned int i = 0; i < ORF_Data.size(); i++ )
	{
		for( unsigned int k = MIN_NUM_BASES_APART; k <= MAX_NUM_BASES_APART; k++ )
		{
			bool strand;
			unsigned int gene_CDS_start;
			unsigned int gene_CDS_end;
			unsigned int chrom_it = ORF_Data[i].chrom_num - 1;

			if( ORF_Data[i].opposite_strand == FALSE )  
			{ 
				strand = STRAND_W;
				
				gene_CDS_start = ORF_Data[i].gene_CDS.start_coord;
				gene_CDS_end   = ORF_Data[i].gene_CDS.start_coord + ORF_Data[i].gene_CDS.length - 1;
			}
			else/*( ORF_Data[i].opposite_strand == TRUE )*/ 
			{ 
				strand = STRAND_C;
				
				gene_CDS_start = chrom_seq[strand][chrom_it].size() - ORF_Data[i].gene_CDS.start_coord + 1;
				gene_CDS_end   = gene_CDS_start   				 	+ ORF_Data[i].gene_CDS.length 	   - 1;
			}
			

			for( unsigned int j = gene_CDS_start - 1; j < gene_CDS_end; j++ )
			{	
				num_nt++;
				
				switch( chrom_seq[strand][chrom_it][j] )
				{
					case 'A' : 	num_instances_each_nt[ NT_A ]++;
								break;
								
					case 'T' : 	num_instances_each_nt[ NT_T ]++;
								break;
								
					case 'C' : 	num_instances_each_nt[ NT_C ]++;
								break;
								
					case 'G' : 	num_instances_each_nt[ NT_G ]++;
								break;
								
					default  : 	break;
				}
			}	
		

			for( unsigned int j = gene_CDS_start - 1; j < ( gene_CDS_end - k ); j++ )
			{	
				num_instances_each_distance[ k - MIN_NUM_BASES_APART ]++;
				
				if	   ( chrom_seq[strand][chrom_it][j] == 'A' && chrom_seq[strand][chrom_it][j + k] == 'A' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ A_AND_A ]++; }
				else if( chrom_seq[strand][chrom_it][j] == 'A' && chrom_seq[strand][chrom_it][j + k] == 'T' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ A_AND_T ]++; }
				else if( chrom_seq[strand][chrom_it][j] == 'A' && chrom_seq[strand][chrom_it][j + k] == 'C' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ A_AND_C ]++; }
				else if( chrom_seq[strand][chrom_it][j] == 'A' && chrom_seq[strand][chrom_it][j + k] == 'G' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ A_AND_G ]++; }
				else if( chrom_seq[strand][chrom_it][j] == 'T' && chrom_seq[strand][chrom_it][j + k] == 'A' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ T_AND_A ]++; }
				else if( chrom_seq[strand][chrom_it][j] == 'T' && chrom_seq[strand][chrom_it][j + k] == 'T' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ T_AND_T ]++; }
				else if( chrom_seq[strand][chrom_it][j] == 'T' && chrom_seq[strand][chrom_it][j + k] == 'C' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ T_AND_C ]++; }
				else if( chrom_seq[strand][chrom_it][j] == 'T' && chrom_seq[strand][chrom_it][j + k] == 'G' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ T_AND_G ]++; }
				else if( chrom_seq[strand][chrom_it][j] == 'C' && chrom_seq[strand][chrom_it][j + k] == 'A' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ C_AND_A ]++; }
				else if( chrom_seq[strand][chrom_it][j] == 'C' && chrom_seq[strand][chrom_it][j + k] == 'T' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ C_AND_T ]++; }
				else if( chrom_seq[strand][chrom_it][j] == 'C' && chrom_seq[strand][chrom_it][j + k] == 'C' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ C_AND_C ]++; }
				else if( chrom_seq[strand][chrom_it][j] == 'C' && chrom_seq[strand][chrom_it][j + k] == 'G' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ C_AND_G ]++; }
				else if( chrom_seq[strand][chrom_it][j] == 'G' && chrom_seq[strand][chrom_it][j + k] == 'A' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ G_AND_A ]++; }
				else if( chrom_seq[strand][chrom_it][j] == 'G' && chrom_seq[strand][chrom_it][j + k] == 'T' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ G_AND_T ]++; }
				else if( chrom_seq[strand][chrom_it][j] == 'G' && chrom_seq[strand][chrom_it][j + k] == 'C' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ G_AND_C ]++; }
				else if( chrom_seq[strand][chrom_it][j] == 'G' && chrom_seq[strand][chrom_it][j + k] == 'G' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ G_AND_G ]++; }
				else
				{
					Errors.handle_error( FATAL, "Error in 'calc_joint_prob_nt_genes_occurrence()': Pair of nucleotides not recognized. ", __FILE__, __LINE__ );
				}
			}
		}
	}
	


	for( unsigned int i = 0; i < num_instances_each_pair.size(); i++ )
	{
		for( unsigned int j = 0; j < num_instances_each_pair[i].size(); j++ )
		{
			(*joint_prob_nt)[i][j] = (double)num_instances_each_pair[i][j] / num_instances_each_distance[i];
			
			output_text_line( (string)"Prob of case " + get_str( j ) + " for k=" + get_str( i + MIN_NUM_BASES_APART ) + ": " + get_str( (*joint_prob_nt)[i][j] ) );
		}
	}
	
	for( unsigned int i = 0; i < num_instances_each_nt.size(); i++ )
	{
		(*marginal_prob_nt)[i] = (double)num_instances_each_nt[i] / num_nt;
			
		output_text_line( (string)"Prob of case " + get_str( i )  + ": " + get_str( (*marginal_prob_nt)[i] ) );
	}
			
	return;
}
//==============================================================================



void calc_joint_prob_nt_TL( const TFasta_Content & S_Cerevisiae_Chrom, const vector <TORF_Data> & ORF_Data, vector <vector <double> > * const joint_prob_nt, vector <double> * const marginal_prob_nt )
{
	vector <vector <unsigned int> > num_instances_each_pair( MAX_NUM_BASES_APART - MIN_NUM_BASES_APART + 1, vector <unsigned int>( NUM_NUCLEOTIDE_PAIRS, 0 ) );
	*joint_prob_nt = vector <vector <double> >   		   ( MAX_NUM_BASES_APART - MIN_NUM_BASES_APART + 1, vector <double		>( NUM_NUCLEOTIDE_PAIRS, 0 ) );
	
	vector <unsigned int> num_instances_each_distance( MAX_NUM_BASES_APART - MIN_NUM_BASES_APART + 1, 0 );
	
	
	vector <unsigned int> num_instances_each_nt( NUM_DIFF_NT, 0 );
	*marginal_prob_nt = vector <double>		   ( NUM_DIFF_NT, 0 );
	
	unsigned int num_nt= 0;
	
	
	vector <vector <string> > chrom_seq( 2, vector <string>( S_Cerevisiae_Chrom.sequence.size(), "" ) );
	
	for( unsigned int chrom_it = 0; chrom_it < S_Cerevisiae_Chrom.sequence.size(); chrom_it++ )
	{
		chrom_seq[STRAND_W][chrom_it] = S_Cerevisiae_Chrom.sequence[ chrom_it ];
		
		chrom_seq[STRAND_C][chrom_it] = get_inverted_sequence( chrom_seq[STRAND_W][chrom_it] );
		chrom_seq[STRAND_C][chrom_it] = get_opposite_strand  ( chrom_seq[STRAND_C][chrom_it] );
	}


	for( unsigned int i = 0; i < ORF_Data.size(); i++ )
	{
		for( unsigned int k = MIN_NUM_BASES_APART; k <= MAX_NUM_BASES_APART; k++ )
		{
			bool strand;
			unsigned int gene_CDS_start;
			unsigned int gene_CDS_end;
			unsigned int chrom_it = ORF_Data[i].chrom_num - 1;

			if( ORF_Data[i].opposite_strand == FALSE )  
			{ 
				strand = STRAND_C;
				
				gene_CDS_start = ORF_Data[i].gene_CDS.start_coord;
				gene_CDS_end   = ORF_Data[i].gene_CDS.start_coord + ORF_Data[i].gene_CDS.length - 1;
			}
			else/*( ORF_Data[i].opposite_strand == TRUE )*/ 
			{ 
				strand = STRAND_W;
				
				gene_CDS_start = chrom_seq[strand][chrom_it].size() - ORF_Data[i].gene_CDS.start_coord + 1;
				gene_CDS_end   = gene_CDS_start   				 	+ ORF_Data[i].gene_CDS.length 	   - 1;
			}
			

			for( unsigned int j = gene_CDS_start - 401; j < gene_CDS_start - 389 + 500; j++ )
			{	
				num_nt++;
				
				switch( chrom_seq[strand][chrom_it][j] )
				{
					case 'A' : 	num_instances_each_nt[ NT_A ]++;
								break;
								
					case 'T' : 	num_instances_each_nt[ NT_T ]++;
								break;
								
					case 'C' : 	num_instances_each_nt[ NT_C ]++;
								break;
								
					case 'G' : 	num_instances_each_nt[ NT_G ]++;
								break;
								
					default  : 	break;
				}
			}	
		

			for( unsigned int j = gene_CDS_start - 401; j < gene_CDS_start - 389; j++ )
			{	
				
				num_instances_each_distance[ k - MIN_NUM_BASES_APART ]++;
				
				
				if	   ( chrom_seq[strand][chrom_it][j] == 'A' && chrom_seq[strand][chrom_it][j + k] == 'A' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ A_AND_A ]++; }
				else if( chrom_seq[strand][chrom_it][j] == 'A' && chrom_seq[strand][chrom_it][j + k] == 'T' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ A_AND_T ]++; }
				else if( chrom_seq[strand][chrom_it][j] == 'A' && chrom_seq[strand][chrom_it][j + k] == 'C' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ A_AND_C ]++; }
				else if( chrom_seq[strand][chrom_it][j] == 'A' && chrom_seq[strand][chrom_it][j + k] == 'G' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ A_AND_G ]++; }
				else if( chrom_seq[strand][chrom_it][j] == 'T' && chrom_seq[strand][chrom_it][j + k] == 'A' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ T_AND_A ]++; }
				else if( chrom_seq[strand][chrom_it][j] == 'T' && chrom_seq[strand][chrom_it][j + k] == 'T' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ T_AND_T ]++; }
				else if( chrom_seq[strand][chrom_it][j] == 'T' && chrom_seq[strand][chrom_it][j + k] == 'C' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ T_AND_C ]++; }
				else if( chrom_seq[strand][chrom_it][j] == 'T' && chrom_seq[strand][chrom_it][j + k] == 'G' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ T_AND_G ]++; }
				else if( chrom_seq[strand][chrom_it][j] == 'C' && chrom_seq[strand][chrom_it][j + k] == 'A' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ C_AND_A ]++; }
				else if( chrom_seq[strand][chrom_it][j] == 'C' && chrom_seq[strand][chrom_it][j + k] == 'T' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ C_AND_T ]++; }
				else if( chrom_seq[strand][chrom_it][j] == 'C' && chrom_seq[strand][chrom_it][j + k] == 'C' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ C_AND_C ]++; }
				else if( chrom_seq[strand][chrom_it][j] == 'C' && chrom_seq[strand][chrom_it][j + k] == 'G' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ C_AND_G ]++; }
				else if( chrom_seq[strand][chrom_it][j] == 'G' && chrom_seq[strand][chrom_it][j + k] == 'A' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ G_AND_A ]++; }
				else if( chrom_seq[strand][chrom_it][j] == 'G' && chrom_seq[strand][chrom_it][j + k] == 'T' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ G_AND_T ]++; }
				else if( chrom_seq[strand][chrom_it][j] == 'G' && chrom_seq[strand][chrom_it][j + k] == 'C' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ G_AND_C ]++; }
				else if( chrom_seq[strand][chrom_it][j] == 'G' && chrom_seq[strand][chrom_it][j + k] == 'G' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ G_AND_G ]++; }
				else
				{
					Errors.handle_error( FATAL, "Error in 'calc_joint_prob_nt_genes_occurrence()': Pair of nucleotides not recognized. ", __FILE__, __LINE__ );
				}
			}
		}
	}
	


	for( unsigned int i = 0; i < num_instances_each_pair.size(); i++ )
	{
		for( unsigned int j = 0; j < num_instances_each_pair[i].size(); j++ )
		{
			(*joint_prob_nt)[i][j] = (double)num_instances_each_pair[i][j] / num_instances_each_distance[i];
			
			//output_text_line( (string)"Prob of case " + get_str( j ) + " for k=" + get_str( i + MIN_NUM_BASES_APART ) + ": " + get_str( (*joint_prob_nt)[i][j] ) );
			//output_text_line( (string)"num  of case " + get_str( j ) + " for k=" + get_str( i + MIN_NUM_BASES_APART ) + ": " + get_str( num_instances_each_pair[i][j] ) );
		}
	}
	
	for( unsigned int i = 0; i < num_instances_each_nt.size(); i++ )
	{
		(*marginal_prob_nt)[i] = (double)num_instances_each_nt[i] / num_nt;
			
		//output_text_line( (string)"Prob of case " + get_str( i )  + ": " + get_str( (*marginal_prob_nt)[i] ) );
	}
			
	return;
}
//==============================================================================



void calc_AMI_uORFs( const TFasta_Content & S_Cerevisiae_Chrom, const vector <TORF_Data> & ORF_Data )
{
	vector <vector <string> > chrom_seq( 2, vector <string>( S_Cerevisiae_Chrom.sequence.size(), "" ) );
	
	for( unsigned int chrom_it = 0; chrom_it < S_Cerevisiae_Chrom.sequence.size(); chrom_it++ )
	{
		chrom_seq[STRAND_W][chrom_it] = S_Cerevisiae_Chrom.sequence[ chrom_it ];
		
		chrom_seq[STRAND_C][chrom_it] = get_inverted_sequence( chrom_seq[STRAND_W][chrom_it] );
		chrom_seq[STRAND_C][chrom_it] = get_opposite_strand  ( chrom_seq[STRAND_C][chrom_it] );
	}


	for( unsigned int i = 0; i < ORF_Data.size(); i++ )
	{
		vector <TuORF_Data> uORFs = ORF_Data[i].get_uORFs();
		
		
		bool strand;
		unsigned int gene_CDS_start;
		unsigned int gene_CDS_end;
		unsigned int chrom_it = ORF_Data[i].chrom_num - 1;

		if( ORF_Data[i].opposite_strand == FALSE )  
		{ 
			strand = STRAND_C;
			
			gene_CDS_start = ORF_Data[i].gene_CDS.start_coord;
			gene_CDS_end   = ORF_Data[i].gene_CDS.start_coord + ORF_Data[i].gene_CDS.length - 1;
		}
		else/*( ORF_Data[i].opposite_strand == TRUE )*/ 
		{ 
			strand = STRAND_W;
			
			gene_CDS_start = chrom_seq[strand][chrom_it].size() - ORF_Data[i].gene_CDS.start_coord + 1;
			gene_CDS_end   = gene_CDS_start   				 	+ ORF_Data[i].gene_CDS.length 	   - 1;
		}
		
		

		for( unsigned int m = 0; m < uORFs.size(); m++ )
		{
			vector <unsigned int> num_instances_each_nt_x( NUM_DIFF_NT, 0 );
			vector <unsigned int> num_instances_each_nt_y( NUM_DIFF_NT, 0 );
			vector <double 		> marginal_prob_nt_x     ( NUM_DIFF_NT, 0 );
			vector <double 		> marginal_prob_nt_y     ( NUM_DIFF_NT, 0 );
			unsigned int num_nt_x = 0;
			unsigned int num_nt_y = 0;
			
			
			for( unsigned int j = gene_CDS_start - 1 + ( uORFs[m].content.size() - 3 - 1 ); j < gene_CDS_end - ( uORFs[m].content.size() - 3 - 1 ); j++ )
			{	
				num_nt_y++;
				
				switch( chrom_seq[strand][chrom_it][j] )
				{
					case 'A' : 	num_instances_each_nt_y[ NT_A ]++;
								break;
								
					case 'T' : 	num_instances_each_nt_y[ NT_T ]++;
								break;
								
					case 'C' : 	num_instances_each_nt_y[ NT_C ]++;
								break;
								
					case 'G' : 	num_instances_each_nt_y[ NT_G ]++;
								break;
								
					default  : 	break;
				}
			}	
			
			
			int num_k_values = gene_CDS_end - gene_CDS_start - ( 2 * uORFs[m].len ) + 3;	
			if( num_k_values < 0 ) { num_k_values = 0; }
			unsigned int y_pos_start  = gene_CDS_start - uORFs[m].start_pos - 3 + uORFs[m].len - 1;	
	
				
			
			for( unsigned int j = 3; j < uORFs[m].content.size(); j++ )
			{	
				num_nt_x++;
				
				switch( uORFs[m].content[j] )
				{
					case 'A' : 	num_instances_each_nt_x[ NT_A ]++;
								break;
								
					case 'T' : 	num_instances_each_nt_x[ NT_T ]++;
								break;
								
					case 'C' : 	num_instances_each_nt_x[ NT_C ]++;
								break;
								
					case 'G' : 	num_instances_each_nt_x[ NT_G ]++;
								break;
								
					default  : 	break;
				}
			}	
				
			vector <double> AMI_profiles( num_k_values, 0 ); 	
			double AMI_profile_sum = 0; 	
			
			for( unsigned int k = 0; k < (unsigned int)num_k_values; k++ )
			{	
				vector <unsigned int> num_instances_each_pair( NUM_NUCLEOTIDE_PAIRS, 0 );		
				vector <double      > joint_prob_nt		     ( NUM_NUCLEOTIDE_PAIRS, 0 );
				
				unsigned int num_pairs = 0;
			
			
				for( unsigned int j = 3; j < uORFs[m].len; j++ )
				{
					num_pairs++;
					
					unsigned int y_pos_it = ( uORFs[m].start_pos - 1 ) + j + y_pos_start + k;
					
					if( y_pos_it >= chrom_seq[strand][chrom_it].size() )
					{
						Errors.handle_error( FATAL, "Error in 'calc_joint_prob_nt_genes_occurrence()': Pair of nucleotides not recognized. ", __FILE__, __LINE__ );
					}

					if	   ( uORFs[m].content[j] == 'A' && chrom_seq[strand][chrom_it][y_pos_it] == 'A' ) { num_instances_each_pair[ A_AND_A ]++; }
					else if( uORFs[m].content[j] == 'A' && chrom_seq[strand][chrom_it][y_pos_it] == 'T' ) { num_instances_each_pair[ A_AND_T ]++; }
					else if( uORFs[m].content[j] == 'A' && chrom_seq[strand][chrom_it][y_pos_it] == 'C' ) { num_instances_each_pair[ A_AND_C ]++; }
					else if( uORFs[m].content[j] == 'A' && chrom_seq[strand][chrom_it][y_pos_it] == 'G' ) { num_instances_each_pair[ A_AND_G ]++; }
					else if( uORFs[m].content[j] == 'T' && chrom_seq[strand][chrom_it][y_pos_it] == 'A' ) { num_instances_each_pair[ T_AND_A ]++; }
					else if( uORFs[m].content[j] == 'T' && chrom_seq[strand][chrom_it][y_pos_it] == 'T' ) { num_instances_each_pair[ T_AND_T ]++; }
					else if( uORFs[m].content[j] == 'T' && chrom_seq[strand][chrom_it][y_pos_it] == 'C' ) { num_instances_each_pair[ T_AND_C ]++; }
					else if( uORFs[m].content[j] == 'T' && chrom_seq[strand][chrom_it][y_pos_it] == 'G' ) { num_instances_each_pair[ T_AND_G ]++; }
					else if( uORFs[m].content[j] == 'C' && chrom_seq[strand][chrom_it][y_pos_it] == 'A' ) { num_instances_each_pair[ C_AND_A ]++; }
					else if( uORFs[m].content[j] == 'C' && chrom_seq[strand][chrom_it][y_pos_it] == 'T' ) { num_instances_each_pair[ C_AND_T ]++; }
					else if( uORFs[m].content[j] == 'C' && chrom_seq[strand][chrom_it][y_pos_it] == 'C' ) { num_instances_each_pair[ C_AND_C ]++; }
					else if( uORFs[m].content[j] == 'C' && chrom_seq[strand][chrom_it][y_pos_it] == 'G' ) { num_instances_each_pair[ C_AND_G ]++; }
					else if( uORFs[m].content[j] == 'G' && chrom_seq[strand][chrom_it][y_pos_it] == 'A' ) { num_instances_each_pair[ G_AND_A ]++; }
					else if( uORFs[m].content[j] == 'G' && chrom_seq[strand][chrom_it][y_pos_it] == 'T' ) { num_instances_each_pair[ G_AND_T ]++; }
					else if( uORFs[m].content[j] == 'G' && chrom_seq[strand][chrom_it][y_pos_it] == 'C' ) { num_instances_each_pair[ G_AND_C ]++; }
					else if( uORFs[m].content[j] == 'G' && chrom_seq[strand][chrom_it][y_pos_it] == 'G' ) { num_instances_each_pair[ G_AND_G ]++; }
					else
					{
						Errors.handle_error( FATAL, "Error in 'calc_joint_prob_nt_genes_occurrence()': Pair of nucleotides not recognized. ", __FILE__, __LINE__ );
					}
				}
				
				
				for( unsigned int j = 0; j < num_instances_each_pair.size(); j++ )
				{
					joint_prob_nt[j] = (double)num_instances_each_pair[j] / num_pairs;
						
					//output_text_line( (string)"Prob of case " + get_str( j ) + " for gene " + ORF_Data[i].gene_name + " uORF " + get_str( m ) + ": " + get_str( joint_prob_nt[j] ) );
					//output_text_line( (string)"num of case "  + get_str( j ) + " for gene " + ORF_Data[i].gene_name + " uORF " + get_str( m ) + ": " + get_str( num_instances_each_pair[j] ) );
				}
				
				for( unsigned int j = 0; j < num_instances_each_nt_x.size(); j++ )
				{
					marginal_prob_nt_x[j] = (double)num_instances_each_nt_x[j] / num_nt_x;
					marginal_prob_nt_y[j] = (double)num_instances_each_nt_y[j] / num_nt_y;
						
					//output_text_line( (string)"Prob of case x" + get_str( j ) + " for gene " + ORF_Data[i].gene_name + " uORF " + get_str( m ) + ": " + get_str( marginal_prob_nt_x[j] ) );
					//output_text_line( (string)"Prob of case y" + get_str( j ) + " for gene " + ORF_Data[i].gene_name + " uORF " + get_str( m ) + ": " + get_str( marginal_prob_nt_y[j] ) );
				}
				
				
				
				if( joint_prob_nt[ A_AND_A ] != 0 ) { AMI_profiles[k] += ( joint_prob_nt[ A_AND_A ] * log10( joint_prob_nt[ A_AND_A ] / marginal_prob_nt_x[ NT_A ] / marginal_prob_nt_y[ NT_A ] ) ); }
				if( joint_prob_nt[ A_AND_T ] != 0 ) { AMI_profiles[k] += ( joint_prob_nt[ A_AND_T ] * log10( joint_prob_nt[ A_AND_T ] / marginal_prob_nt_x[ NT_A ] / marginal_prob_nt_y[ NT_T ] ) ); }
				if( joint_prob_nt[ A_AND_C ] != 0 ) { AMI_profiles[k] += ( joint_prob_nt[ A_AND_C ] * log10( joint_prob_nt[ A_AND_C ] / marginal_prob_nt_x[ NT_A ] / marginal_prob_nt_y[ NT_C ] ) ); }
				if( joint_prob_nt[ A_AND_G ] != 0 ) { AMI_profiles[k] += ( joint_prob_nt[ A_AND_G ] * log10( joint_prob_nt[ A_AND_G ] / marginal_prob_nt_x[ NT_A ] / marginal_prob_nt_y[ NT_G ] ) ); }
				if( joint_prob_nt[ T_AND_A ] != 0 ) { AMI_profiles[k] += ( joint_prob_nt[ T_AND_A ] * log10( joint_prob_nt[ T_AND_A ] / marginal_prob_nt_x[ NT_T ] / marginal_prob_nt_y[ NT_A ] ) ); }
				if( joint_prob_nt[ T_AND_T ] != 0 ) { AMI_profiles[k] += ( joint_prob_nt[ T_AND_T ] * log10( joint_prob_nt[ T_AND_T ] / marginal_prob_nt_x[ NT_T ] / marginal_prob_nt_y[ NT_T ] ) ); }
				if( joint_prob_nt[ T_AND_C ] != 0 ) { AMI_profiles[k] += ( joint_prob_nt[ T_AND_C ] * log10( joint_prob_nt[ T_AND_C ] / marginal_prob_nt_x[ NT_T ] / marginal_prob_nt_y[ NT_C ] ) ); }
				if( joint_prob_nt[ T_AND_G ] != 0 ) { AMI_profiles[k] += ( joint_prob_nt[ T_AND_G ] * log10( joint_prob_nt[ T_AND_G ] / marginal_prob_nt_x[ NT_T ] / marginal_prob_nt_y[ NT_G ] ) ); }
				if( joint_prob_nt[ C_AND_A ] != 0 ) { AMI_profiles[k] += ( joint_prob_nt[ C_AND_A ] * log10( joint_prob_nt[ C_AND_A ] / marginal_prob_nt_x[ NT_C ] / marginal_prob_nt_y[ NT_A ] ) ); }
				if( joint_prob_nt[ C_AND_T ] != 0 ) { AMI_profiles[k] += ( joint_prob_nt[ C_AND_T ] * log10( joint_prob_nt[ C_AND_T ] / marginal_prob_nt_x[ NT_C ] / marginal_prob_nt_y[ NT_T ] ) ); }
				if( joint_prob_nt[ C_AND_C ] != 0 ) { AMI_profiles[k] += ( joint_prob_nt[ C_AND_C ] * log10( joint_prob_nt[ C_AND_C ] / marginal_prob_nt_x[ NT_C ] / marginal_prob_nt_y[ NT_C ] ) ); }
				if( joint_prob_nt[ C_AND_G ] != 0 ) { AMI_profiles[k] += ( joint_prob_nt[ C_AND_G ] * log10( joint_prob_nt[ C_AND_G ] / marginal_prob_nt_x[ NT_C ] / marginal_prob_nt_y[ NT_G ] ) ); }
				if( joint_prob_nt[ G_AND_A ] != 0 ) { AMI_profiles[k] += ( joint_prob_nt[ G_AND_A ] * log10( joint_prob_nt[ G_AND_A ] / marginal_prob_nt_x[ NT_G ] / marginal_prob_nt_y[ NT_A ] ) ); }
				if( joint_prob_nt[ G_AND_T ] != 0 ) { AMI_profiles[k] += ( joint_prob_nt[ G_AND_T ] * log10( joint_prob_nt[ G_AND_T ] / marginal_prob_nt_x[ NT_G ] / marginal_prob_nt_y[ NT_T ] ) ); }
				if( joint_prob_nt[ G_AND_C ] != 0 ) { AMI_profiles[k] += ( joint_prob_nt[ G_AND_C ] * log10( joint_prob_nt[ G_AND_C ] / marginal_prob_nt_x[ NT_G ] / marginal_prob_nt_y[ NT_C ] ) ); }
				if( joint_prob_nt[ G_AND_G ] != 0 ) { AMI_profiles[k] += ( joint_prob_nt[ G_AND_G ] * log10( joint_prob_nt[ G_AND_G ] / marginal_prob_nt_x[ NT_G ] / marginal_prob_nt_y[ NT_G ] ) ); }
			

				AMI_profile_sum += AMI_profiles[k];
				
				//output_text_line( "    " + /*(string)"AMI for k=" + get_str( k ) + ": " +*/ get_str( AMI_profiles[k] ) );
			}


			
			if	  ( num_k_values != 0 )   { output_text_line( /*(string)"AMI for k=" + get_str( k ) + ": " +*/ get_str( AMI_profile_sum / AMI_profiles.size() ) ); }
			else/*( num_k_values != 0 )*/ { output_text_line( /*(string)"AMI for k=" + get_str( k ) + ": " +*/ get_str( -1 									  ) ); }
		}
	}
	

	return;
}
//==============================================================================



void calc_mod_AMI_uORF_context( const TFasta_Content & S_Cerevisiae_Chrom, vector <TORF_Data> * const ORF_Data )
{
	vector <vector <unsigned int> > uORFs_mutual_info_vect( 0 );
	string vectors_to_print = "";
	
	
	for( unsigned int i = 0; i < ORF_Data->size(); i++ )
	{
		vector <TuORF_Data> uORFs = ORF_Data->at(i).get_uORFs();
	
		for( unsigned int m = 0; m < uORFs.size(); m++ )
		{
			vector <vector <unsigned int> > num_instances_each_pair( MAX_NUM_BASES_APART_CONTEXT - MIN_NUM_BASES_APART_CONTEXT + 1, 
																	 vector <unsigned int>( NUM_NUCLEOTIDE_PAIRS, 0 ) );

			vector <unsigned int> num_instances_each_nt( NUM_DIFF_NT, 0 );
			
			vector <unsigned int> this_uORF_mutual_info_vect( 0 );
	
	
			for( unsigned int k = MIN_NUM_BASES_APART_CONTEXT; k <= MAX_NUM_BASES_APART_CONTEXT; k++ )
			{
				for( unsigned int j = 0; j < uORFs[m].ext_start_context.size(); j++ )
				{	
					switch( uORFs[m].ext_start_context[j] )
					{
						case 'A' : 	num_instances_each_nt[ NT_A ]++;
									break;
									
						case 'T' : 	num_instances_each_nt[ NT_T ]++;
									break;
									
						case 'C' : 	num_instances_each_nt[ NT_C ]++;
									break;
									
						case 'G' : 	num_instances_each_nt[ NT_G ]++;
									break;
									
						default  : 	break;
					}
				}	

				
				
				for( unsigned int j = 0; j < ( uORFs[m].ext_start_context.size() - k ); j++ )
				{
					if	   ( uORFs[m].ext_start_context[j] == 'A' && uORFs[m].ext_start_context[j + k] == 'A' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART_CONTEXT ][ A_AND_A ]++; }
					else if( uORFs[m].ext_start_context[j] == 'A' && uORFs[m].ext_start_context[j + k] == 'T' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART_CONTEXT ][ A_AND_T ]++; }
					else if( uORFs[m].ext_start_context[j] == 'A' && uORFs[m].ext_start_context[j + k] == 'C' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART_CONTEXT ][ A_AND_C ]++; }
					else if( uORFs[m].ext_start_context[j] == 'A' && uORFs[m].ext_start_context[j + k] == 'G' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART_CONTEXT ][ A_AND_G ]++; }
					else if( uORFs[m].ext_start_context[j] == 'T' && uORFs[m].ext_start_context[j + k] == 'A' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART_CONTEXT ][ T_AND_A ]++; }
					else if( uORFs[m].ext_start_context[j] == 'T' && uORFs[m].ext_start_context[j + k] == 'T' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART_CONTEXT ][ T_AND_T ]++; }
					else if( uORFs[m].ext_start_context[j] == 'T' && uORFs[m].ext_start_context[j + k] == 'C' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART_CONTEXT ][ T_AND_C ]++; }
					else if( uORFs[m].ext_start_context[j] == 'T' && uORFs[m].ext_start_context[j + k] == 'G' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART_CONTEXT ][ T_AND_G ]++; }
					else if( uORFs[m].ext_start_context[j] == 'C' && uORFs[m].ext_start_context[j + k] == 'A' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART_CONTEXT ][ C_AND_A ]++; }
					else if( uORFs[m].ext_start_context[j] == 'C' && uORFs[m].ext_start_context[j + k] == 'T' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART_CONTEXT ][ C_AND_T ]++; }
					else if( uORFs[m].ext_start_context[j] == 'C' && uORFs[m].ext_start_context[j + k] == 'C' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART_CONTEXT ][ C_AND_C ]++; }
					else if( uORFs[m].ext_start_context[j] == 'C' && uORFs[m].ext_start_context[j + k] == 'G' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART_CONTEXT ][ C_AND_G ]++; }
					else if( uORFs[m].ext_start_context[j] == 'G' && uORFs[m].ext_start_context[j + k] == 'A' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART_CONTEXT ][ G_AND_A ]++; }
					else if( uORFs[m].ext_start_context[j] == 'G' && uORFs[m].ext_start_context[j + k] == 'T' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART_CONTEXT ][ G_AND_T ]++; }
					else if( uORFs[m].ext_start_context[j] == 'G' && uORFs[m].ext_start_context[j + k] == 'C' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART_CONTEXT ][ G_AND_C ]++; }
					else if( uORFs[m].ext_start_context[j] == 'G' && uORFs[m].ext_start_context[j + k] == 'G' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART_CONTEXT ][ G_AND_G ]++; }
					else
					{
						Errors.handle_error( FATAL, (string)"Error in 'calc_joint_prob_nt_genes_occurrence()': Pair of nucleotides not recognized (" + uORFs[m].ext_start_context[j] + 
															" and " + uORFs[m].ext_start_context[j + k] + ").", __FILE__, __LINE__ );
					}
				}
				
				this_uORF_mutual_info_vect.insert( this_uORF_mutual_info_vect.end(),
												   num_instances_each_pair[ k - MIN_NUM_BASES_APART_CONTEXT ].begin(),
												   num_instances_each_pair[ k - MIN_NUM_BASES_APART_CONTEXT ].end() );
			}

			ORF_Data->at(i).update_context_profile(m, this_uORF_mutual_info_vect);
		
			/*
			uORFs_mutual_info_vect.push_back( this_uORF_mutual_info_vect );
			
			for( unsigned int j = 0; j < uORFs_mutual_info_vect.back().size(); j++ )
			{
				vectors_to_print += ( get_str( uORFs_mutual_info_vect.back()[j] ) + '\t');
			}
			
			vectors_to_print += '\n';
			*/
		}
	}
	
	// write_file_contents( PATH_OUTPUT_FOLDER, "uORF_context_profiles.txt", vectors_to_print );

		
	return;
}
//==============================================================================



void calc_joint_prob_nt_genome( const TFasta_Content & S_Cerevisiae_Chrom, vector <vector <double> > * const joint_prob_nt, vector <double> * const marginal_prob_nt )
{
	vector <vector <unsigned int> > num_instances_each_pair( MAX_NUM_BASES_APART - MIN_NUM_BASES_APART + 1, vector <unsigned int>( NUM_NUCLEOTIDE_PAIRS, 0 ) );
	*joint_prob_nt = vector <vector <double> >   		   ( MAX_NUM_BASES_APART - MIN_NUM_BASES_APART + 1, vector <double		>( NUM_NUCLEOTIDE_PAIRS, 0 ) );
	
	vector <unsigned int> num_instances_each_distance( MAX_NUM_BASES_APART - MIN_NUM_BASES_APART + 1, 0 );
	
	
	vector <unsigned int> num_instances_each_nt( NUM_DIFF_NT, 0 );
	*marginal_prob_nt = vector <double>		   ( NUM_DIFF_NT, 0 );
	
	unsigned int num_nt= 0;


	for( unsigned int k = MIN_NUM_BASES_APART; k <= MAX_NUM_BASES_APART; k++ )
	{	
		for( unsigned int i = 0; i < S_Cerevisiae_Chrom.sequence.size(); i++ )
		{
			for( unsigned int j = 0; j < ( S_Cerevisiae_Chrom.sequence[i].size() - k ); j++ )
			{	
				num_instances_each_distance[ k - MIN_NUM_BASES_APART ]++;
				
				if	   ( S_Cerevisiae_Chrom.sequence[i][j] == 'A' && S_Cerevisiae_Chrom.sequence[i][j + k] == 'A' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ A_AND_A ]++; }
				else if( S_Cerevisiae_Chrom.sequence[i][j] == 'A' && S_Cerevisiae_Chrom.sequence[i][j + k] == 'T' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ A_AND_T ]++; }
				else if( S_Cerevisiae_Chrom.sequence[i][j] == 'A' && S_Cerevisiae_Chrom.sequence[i][j + k] == 'C' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ A_AND_C ]++; }
				else if( S_Cerevisiae_Chrom.sequence[i][j] == 'A' && S_Cerevisiae_Chrom.sequence[i][j + k] == 'G' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ A_AND_G ]++; }
				else if( S_Cerevisiae_Chrom.sequence[i][j] == 'T' && S_Cerevisiae_Chrom.sequence[i][j + k] == 'A' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ T_AND_A ]++; }
				else if( S_Cerevisiae_Chrom.sequence[i][j] == 'T' && S_Cerevisiae_Chrom.sequence[i][j + k] == 'T' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ T_AND_T ]++; }
				else if( S_Cerevisiae_Chrom.sequence[i][j] == 'T' && S_Cerevisiae_Chrom.sequence[i][j + k] == 'C' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ T_AND_C ]++; }
				else if( S_Cerevisiae_Chrom.sequence[i][j] == 'T' && S_Cerevisiae_Chrom.sequence[i][j + k] == 'G' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ T_AND_G ]++; }
				else if( S_Cerevisiae_Chrom.sequence[i][j] == 'C' && S_Cerevisiae_Chrom.sequence[i][j + k] == 'A' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ C_AND_A ]++; }
				else if( S_Cerevisiae_Chrom.sequence[i][j] == 'C' && S_Cerevisiae_Chrom.sequence[i][j + k] == 'T' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ C_AND_T ]++; }
				else if( S_Cerevisiae_Chrom.sequence[i][j] == 'C' && S_Cerevisiae_Chrom.sequence[i][j + k] == 'C' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ C_AND_C ]++; }
				else if( S_Cerevisiae_Chrom.sequence[i][j] == 'C' && S_Cerevisiae_Chrom.sequence[i][j + k] == 'G' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ C_AND_G ]++; }
				else if( S_Cerevisiae_Chrom.sequence[i][j] == 'G' && S_Cerevisiae_Chrom.sequence[i][j + k] == 'A' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ G_AND_A ]++; }
				else if( S_Cerevisiae_Chrom.sequence[i][j] == 'G' && S_Cerevisiae_Chrom.sequence[i][j + k] == 'T' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ G_AND_T ]++; }
				else if( S_Cerevisiae_Chrom.sequence[i][j] == 'G' && S_Cerevisiae_Chrom.sequence[i][j + k] == 'C' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ G_AND_C ]++; }
				else if( S_Cerevisiae_Chrom.sequence[i][j] == 'G' && S_Cerevisiae_Chrom.sequence[i][j + k] == 'G' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ G_AND_G ]++; }
				else
				{
					Errors.handle_error( FATAL, "Error in 'calc_joint_prob_nt_genes_occurrence()': Pair of nucleotides not recognized. ", __FILE__, __LINE__ );
				}
			}
		}
	}
	

	// Calculate the marginal probabilities
	for( unsigned int i = 0; i < S_Cerevisiae_Chrom.sequence.size(); i++ )
	{
		for( unsigned int j = 0; j < S_Cerevisiae_Chrom.sequence[i].size(); j++ )
		{	
			num_nt++;
			
			switch( S_Cerevisiae_Chrom.sequence[i][j] )
			{
				case 'A' : 	num_instances_each_nt[ NT_A ]++;
							break;
							
				case 'T' : 	num_instances_each_nt[ NT_T ]++;
							break;
							
				case 'C' : 	num_instances_each_nt[ NT_C ]++;
							break;
							
				case 'G' : 	num_instances_each_nt[ NT_G ]++;
							break;
							
				default  : 	break;
			}
		}	
	}
		
			


	for( unsigned int i = 0; i < num_instances_each_pair.size(); i++ )
	{
		for( unsigned int j = 0; j < num_instances_each_pair[i].size(); j++ )
		{
			(*joint_prob_nt)[i][j] = (double)num_instances_each_pair[i][j] / num_instances_each_distance[i];
			
			output_text_line( (string)"Prob of case " + get_str( j ) + " for k=" + get_str( i + MIN_NUM_BASES_APART ) + ": " + get_str( (*joint_prob_nt)[i][j] ) );
		}
	}
	
	for( unsigned int i = 0; i < num_instances_each_nt.size(); i++ )
	{
		(*marginal_prob_nt)[i] = (double)num_instances_each_nt[i] / num_nt;
			
		output_text_line( (string)"Prob of case " + get_str( i )  + ": " + get_str( (*marginal_prob_nt)[i] ) );
	}
			
	return;
}
//==============================================================================



void calc_joint_prob_nt_sequence( const string & sequence, vector <vector <double> > * const joint_prob_nt, vector <double> * const marginal_prob_nt )
{
	vector <vector <unsigned int> > num_instances_each_pair( MAX_NUM_BASES_APART - MIN_NUM_BASES_APART + 1, vector <unsigned int>( NUM_NUCLEOTIDE_PAIRS, 0 ) );
	*joint_prob_nt = vector <vector <double> >   		   ( MAX_NUM_BASES_APART - MIN_NUM_BASES_APART + 1, vector <double		>( NUM_NUCLEOTIDE_PAIRS, 0 ) );
	
	vector <unsigned int> num_instances_each_distance( MAX_NUM_BASES_APART - MIN_NUM_BASES_APART + 1, 0 );
	
	
	vector <unsigned int> num_instances_each_nt( NUM_DIFF_NT, 0 );
	*marginal_prob_nt = vector <double>		   ( NUM_DIFF_NT, 0 );
	
	unsigned int num_nt= 0;


	for( unsigned int k = MIN_NUM_BASES_APART; k <= MAX_NUM_BASES_APART; k++ )
	{	
		for( unsigned int j = 0; j < ( sequence.size() - k ); j++ )
		{	
			num_instances_each_distance[ k - MIN_NUM_BASES_APART ]++;
			
			if	   ( sequence[j] == 'A' && sequence[j + k] == 'A' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ A_AND_A ]++; }
			else if( sequence[j] == 'A' && sequence[j + k] == 'T' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ A_AND_T ]++; }
			else if( sequence[j] == 'A' && sequence[j + k] == 'C' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ A_AND_C ]++; }
			else if( sequence[j] == 'A' && sequence[j + k] == 'G' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ A_AND_G ]++; }
			else if( sequence[j] == 'T' && sequence[j + k] == 'A' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ T_AND_A ]++; }
			else if( sequence[j] == 'T' && sequence[j + k] == 'T' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ T_AND_T ]++; }
			else if( sequence[j] == 'T' && sequence[j + k] == 'C' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ T_AND_C ]++; }
			else if( sequence[j] == 'T' && sequence[j + k] == 'G' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ T_AND_G ]++; }
			else if( sequence[j] == 'C' && sequence[j + k] == 'A' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ C_AND_A ]++; }
			else if( sequence[j] == 'C' && sequence[j + k] == 'T' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ C_AND_T ]++; }
			else if( sequence[j] == 'C' && sequence[j + k] == 'C' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ C_AND_C ]++; }
			else if( sequence[j] == 'C' && sequence[j + k] == 'G' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ C_AND_G ]++; }
			else if( sequence[j] == 'G' && sequence[j + k] == 'A' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ G_AND_A ]++; }
			else if( sequence[j] == 'G' && sequence[j + k] == 'T' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ G_AND_T ]++; }
			else if( sequence[j] == 'G' && sequence[j + k] == 'C' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ G_AND_C ]++; }
			else if( sequence[j] == 'G' && sequence[j + k] == 'G' ) { num_instances_each_pair[ k - MIN_NUM_BASES_APART ][ G_AND_G ]++; }
			else
			{
				Errors.handle_error( FATAL, "Error in 'calc_joint_prob_nt_genes_occurrence()': Pair of nucleotides not recognized. ", __FILE__, __LINE__ );
			}
		}
	}
	

	// Calculate the marginal probabilities
	for( unsigned int j = 0; j < sequence.size(); j++ )
	{	
		num_nt++;
		
		switch( sequence[j] )
		{
			case 'A' : 	num_instances_each_nt[ NT_A ]++;
						break;
						
			case 'T' : 	num_instances_each_nt[ NT_T ]++;
						break;
						
			case 'C' : 	num_instances_each_nt[ NT_C ]++;
						break;
						
			case 'G' : 	num_instances_each_nt[ NT_G ]++;
						break;
						
			default  : 	break;
		}
	}	
		
			

	for( unsigned int i = 0; i < num_instances_each_pair.size(); i++ )
	{
		for( unsigned int j = 0; j < num_instances_each_pair[i].size(); j++ )
		{
			(*joint_prob_nt)[i][j] = (double)num_instances_each_pair[i][j] / num_instances_each_distance[i];
			
			output_text_line( (string)"Prob of case " + get_str( j ) + " for k=" + get_str( i + MIN_NUM_BASES_APART ) + ": " + get_str( (*joint_prob_nt)[i][j] ) );
		}
	}
	
	for( unsigned int i = 0; i < num_instances_each_nt.size(); i++ )
	{
		(*marginal_prob_nt)[i] = (double)num_instances_each_nt[i] / num_nt;
			
		output_text_line( (string)"Prob of case " + get_str( i )  + ": " + get_str( (*marginal_prob_nt)[i] ) );
	}
			
	return;
}
//==============================================================================



vector <double> calculate_AMI_profile( const TFasta_Content & S_Cerevisiae_Chrom, const vector <TORF_Data> & ORF_Data, const vector <vector <double> > & joint_prob_nt, const vector <double> & marginal_prob_nt )
{
	//vector <vector <double> > joint_prob_nt;
	//vector <double> marginal_prob_nt;
	
	//calc_joint_prob_nt_TL( S_Cerevisiae_Chrom, ORF_Data, &joint_prob_nt, &marginal_prob_nt );
	
	vector <double> AMI_profile( MAX_NUM_BASES_APART - MIN_NUM_BASES_APART + 1, 0 );
	
	
	for( unsigned int k = MIN_NUM_BASES_APART; k <= MAX_NUM_BASES_APART; k++ )
	{
		vector <double> joint_probs = joint_prob_nt[ k - MIN_NUM_BASES_APART ];
		
		AMI_profile[ k - MIN_NUM_BASES_APART ] += ( joint_probs[ A_AND_A ] * log10( joint_probs[ A_AND_A ] / marginal_prob_nt[ NT_A ] / marginal_prob_nt[ NT_A ] ) );
		AMI_profile[ k - MIN_NUM_BASES_APART ] += ( joint_probs[ A_AND_T ] * log10( joint_probs[ A_AND_T ] / marginal_prob_nt[ NT_A ] / marginal_prob_nt[ NT_T ] ) );
		AMI_profile[ k - MIN_NUM_BASES_APART ] += ( joint_probs[ A_AND_C ] * log10( joint_probs[ A_AND_C ] / marginal_prob_nt[ NT_A ] / marginal_prob_nt[ NT_C ] ) );
		AMI_profile[ k - MIN_NUM_BASES_APART ] += ( joint_probs[ A_AND_G ] * log10( joint_probs[ A_AND_G ] / marginal_prob_nt[ NT_A ] / marginal_prob_nt[ NT_G ] ) );
		AMI_profile[ k - MIN_NUM_BASES_APART ] += ( joint_probs[ T_AND_A ] * log10( joint_probs[ T_AND_A ] / marginal_prob_nt[ NT_T ] / marginal_prob_nt[ NT_A ] ) );
		AMI_profile[ k - MIN_NUM_BASES_APART ] += ( joint_probs[ T_AND_T ] * log10( joint_probs[ T_AND_T ] / marginal_prob_nt[ NT_T ] / marginal_prob_nt[ NT_T ] ) );
		AMI_profile[ k - MIN_NUM_BASES_APART ] += ( joint_probs[ T_AND_C ] * log10( joint_probs[ T_AND_C ] / marginal_prob_nt[ NT_T ] / marginal_prob_nt[ NT_C ] ) );
		AMI_profile[ k - MIN_NUM_BASES_APART ] += ( joint_probs[ T_AND_G ] * log10( joint_probs[ T_AND_G ] / marginal_prob_nt[ NT_T ] / marginal_prob_nt[ NT_G ] ) );
		AMI_profile[ k - MIN_NUM_BASES_APART ] += ( joint_probs[ C_AND_A ] * log10( joint_probs[ C_AND_A ] / marginal_prob_nt[ NT_C ] / marginal_prob_nt[ NT_A ] ) );
		AMI_profile[ k - MIN_NUM_BASES_APART ] += ( joint_probs[ C_AND_T ] * log10( joint_probs[ C_AND_T ] / marginal_prob_nt[ NT_C ] / marginal_prob_nt[ NT_T ] ) );
		AMI_profile[ k - MIN_NUM_BASES_APART ] += ( joint_probs[ C_AND_C ] * log10( joint_probs[ C_AND_C ] / marginal_prob_nt[ NT_C ] / marginal_prob_nt[ NT_C ] ) );
		AMI_profile[ k - MIN_NUM_BASES_APART ] += ( joint_probs[ C_AND_G ] * log10( joint_probs[ C_AND_G ] / marginal_prob_nt[ NT_C ] / marginal_prob_nt[ NT_G ] ) );
		AMI_profile[ k - MIN_NUM_BASES_APART ] += ( joint_probs[ G_AND_A ] * log10( joint_probs[ G_AND_A ] / marginal_prob_nt[ NT_G ] / marginal_prob_nt[ NT_A ] ) );
		AMI_profile[ k - MIN_NUM_BASES_APART ] += ( joint_probs[ G_AND_T ] * log10( joint_probs[ G_AND_T ] / marginal_prob_nt[ NT_G ] / marginal_prob_nt[ NT_T ] ) );
		AMI_profile[ k - MIN_NUM_BASES_APART ] += ( joint_probs[ G_AND_C ] * log10( joint_probs[ G_AND_C ] / marginal_prob_nt[ NT_G ] / marginal_prob_nt[ NT_C ] ) );
		AMI_profile[ k - MIN_NUM_BASES_APART ] += ( joint_probs[ G_AND_G ] * log10( joint_probs[ G_AND_G ] / marginal_prob_nt[ NT_G ] / marginal_prob_nt[ NT_G ] ) );
		
		output_text_line( /*(string)"AMI for k=" + get_str( k ) + ": " +*/ get_str( AMI_profile[ k - MIN_NUM_BASES_APART ] ) );
	}
	
	
	return AMI_profile;
}
//==============================================================================



void count_uORF_codons( const std::vector <TORF_Data> & ORF_Data )
{
	vector <unsigned int> num_codons( pow( (unsigned int)NUM_DIFF_NT, 3 ), 0 );

	for( unsigned int ORF_it = 0; ORF_it < ORF_Data.size(); ORF_it++ )
	{
		vector <TuORF_Data> uORFs = ORF_Data[ORF_it].get_uORFs();
		

		for( unsigned int uORF_it = 0; uORF_it < uORFs.size(); uORF_it++ )
		{
			if( uORFs[uORF_it].problem == FALSE &&  uORFs[uORF_it].num_ribosomes >= 5 )
			{
				vector <unsigned int> numeric_DNA_uORF = convert_DNA_string_to_num( uORFs[uORF_it].content );
				
				for(unsigned int start_it = 3; start_it < numeric_DNA_uORF.size(); start_it += 3 )
				{
					unsigned int codon_it = 0;
					
					codon_it += ( NUM_DIFF_NT * NUM_DIFF_NT * numeric_DNA_uORF[start_it    ] ) +
								( 				NUM_DIFF_NT * numeric_DNA_uORF[start_it + 1] ) +
								( 							  numeric_DNA_uORF[start_it + 2] );
					
					num_codons[codon_it]++;
				}
			}
		}
	}
	
	
	for( unsigned int codon_it = 0; codon_it < num_codons.size(); codon_it++ )
	{
		vector <unsigned int> codon_numeric( 3, NT_A );
		
		
		codon_numeric[0] = ( codon_it % ( NUM_DIFF_NT * NUM_DIFF_NT * NUM_DIFF_NT ) ) / ( NUM_DIFF_NT * NUM_DIFF_NT );
		codon_numeric[1] = ( codon_it % ( NUM_DIFF_NT * NUM_DIFF_NT 			  ) ) /   NUM_DIFF_NT;
		codon_numeric[2] =   codon_it %   NUM_DIFF_NT;
		
		
		output_text_line( (string)"Codon " + convert_DNA_num_to_string( codon_numeric ) + ": " + get_str( num_codons[codon_it] ) );
	}
	
	
	return;
}
//==============================================================================



void cluster_uORF_context_profiles( vector <TORF_Data> * const ORF_Data )
{
	// Make sure the number of clusters is a power of 2
	double quotient = NUM_CLUSTERS;
	while( ( (int)quotient % 2 == 0 ) && quotient > 1 ) { quotient /= 2; }
	
	if( quotient != 1 ) { Errors.handle_error( FATAL, "Error in 'cluster_uORF_context_profiles()': Number of clusters specified is not a power of 2.", __FILE__, __LINE__ ); }

	
	float ** training_set;
	float ** clusters;
	unsigned int vector_dimension = NUM_NUCLEOTIDE_PAIRS * ( MAX_NUM_BASES_APART_CONTEXT - MIN_NUM_BASES_APART_CONTEXT + 1 );
	
	unsigned int num_uORFs = 0;
	for( unsigned int i = 0; i < ORF_Data->size(); i++ )
	{
		num_uORFs += ORF_Data->at(i).get_uORFs().size();
	}
	
	training_set = (float **) calloc( num_uORFs, sizeof( float * ) );
	for( unsigned int i = 0; i < num_uORFs; i++ )
	{
		training_set[i] = (float *)calloc( vector_dimension, sizeof( float ) );
	}
	
	unsigned int ts_it = 0;
	for( unsigned int i = 0; i < ORF_Data->size(); i++ )
	{
		vector <TuORF_Data> uORFs = ORF_Data->at(i).get_uORFs();

		for( unsigned int j = 0; j < uORFs.size(); j++ )
		{
			copy( uORFs[j].context_profile.begin(), uORFs[j].context_profile.end(), training_set[ts_it] );

			ts_it++;
		}
	}
	
	lbg_clustering( vector_dimension, NUM_CLUSTERS, num_uORFs, training_set, &clusters );
	
	ts_it = 0;
	for( unsigned int i = 0; i < ORF_Data->size(); i++ )
	{
		vector <TuORF_Data> uORFs = ORF_Data->at(i).get_uORFs();

		for( unsigned int j = 0; j < uORFs.size(); j++ )
		{
			float distortion;
			float * input;
			input = (float *) calloc( vector_dimension,sizeof( float ) );
			
			for( unsigned int k = 0; k < vector_dimension; k++ )
			{
				input[k] = training_set[ts_it][k];
			}


			ORF_Data->at(i).update_cluster   ( j, fvqe( training_set[ts_it], clusters, NUM_CLUSTERS, vector_dimension, &distortion ) );
			ORF_Data->at(i).update_distortion( j, distortion );

			ts_it++;
		}
	}
	
	/*
	// Output distances
	for( unsigned int i = 0; i < NUM_CLUSTERS; i++ )
	{
		string cluster_string = "Cluster " + get_str( i ) + ": ";
		
		for( unsigned int j = 0; j < vector_dimension; j++ )
		{
			cluster_string += get_str( (double)clusters[i][j] ) + "	";
		}
		
		output_text_line( cluster_string );
	}
	
	output_text_line( "" );
	output_text_line( "Distance Matrix" );
	
	for( unsigned int i = 0; i < NUM_CLUSTERS; i++ )
	{
		vector <double> distances( NUM_CLUSTERS, 0 );
		string distances_string = "";
		
		for( unsigned int j = 0; j < NUM_CLUSTERS; j++ )
		{
			if( i == j ) { distances[j] = 0; }
			else
			{
				for( unsigned int k = 0; k < vector_dimension; k++ )
				{
					distances[j] += pow( ( clusters[i][k] - clusters[j][k] ), 2 );
				}
				
				distances[j] = sqrt( distances[j] );
			}
			
			distances_string += get_str( distances[j] ) + "	";
		}
		
		output_text_line( distances_string );

	}
	*/
	
	// Free the memory!
	for( unsigned int i = 0; i < NUM_CLUSTERS; i++ )
	{
		free( clusters[i] );
	}

	free(clusters);

	for( unsigned int i = 0; i < num_uORFs; i++ )
	{
		free( training_set[i] );
	}

	free(training_set);
	
	return;
}
//==============================================================================

////////////////////////////////////////////////////////////////////////////////


