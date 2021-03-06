//==============================================================================
// Project	   : uORF
// Name        : version.h
// Author      : Garin Newcomb
// Email       : gpnewcomb@live.com   
// Version     : See "Revision History" below
// Copyright   : Copyright 2014 University of Nebraska-Lincoln
// Description : Header file to define macros that will control program flow
//==============================================================================
//
//  Revision History
//      v0.0.0 - 2014/07/14 - Garin Newcomb
//          Initial creation of file
//
//    	Appl Version at Last File Update::  v0.0.x - 2014/07/14 - Garin Newcomb
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
//     *C. Type (and Member Function) Declarations and Definitions
//     *D. Non-Member Function Declarations
//     *E. Templated (Non-Member) Function Declarations and Definitions
//     *F. Inline (Non-Member) Function Declarations and Definitions
//     *G. UNUSED Non-Member Function Declarations
//
////////////////////////////////////////////////////////////////////////////////



#ifndef DEFS__APPL_PARAMETERS_H
#define DEFS__APPL_PARAMETERS_H



////////////////////////////////////////////////////////////////////////////////
//
// A. Include Statements, Preprocessor Directives, and Related
//
////////////////////////////////////////////////////////////////////////////////

#include "defs__general.h"



// 'Main' Program Flow
// #define GENERATE_GO_HIERARCHY
// #define GENERATE_GO_CHILDREN_LIST
// #define WRITE_MIURA_SGD_ANNOT_INFO
// #define WRITE_MIURA_TSS_DATA
// #define SEARCH_FOR_AA_SEQ				
// #define CONVERT_DNA_SEQ_TO_AA			
// #define REQUEST_SEL_SEQUENCES
// #define WRITE_GENE_LIST_TO_FILE
   #define WRITE_GENE_AND_uORFs_TO_FASTA
// #define ADD_COL_TO_MCMANUS_TRANSL_DATA



// List Compile
#define LIST_COMPILE_SOURCE					FROM_DATA_SOURCES 			// FROM_CURRENT_LIST / FROM_DATA_SOURCES



// Sorting
#define GO_NAMESPACE_TO_SORT_BY				GO_BIO_PROC_ID				// GO_BIO_PROC_ID /	GO_CELL_COMP_ID / GO_MOL_FUNC_ID

#define PREV_ORF_SORTING_METHOD				SORT_ORFS_BY_POSITION		// SORT_ORFS_BY_POSITION / SORT_ORFS_BY_GO_TERMS / DONT_SORT_ORFS
#define ORF_SORTING_METHOD					SORT_ORFS_BY_POSITION		// SORT_ORFS_BY_POSITION / SORT_ORFS_BY_GO_TERMS / DONT_SORT_ORFS


// Duplicates
#define SELECT_uORFS_TO_DELETE				DELETE_ALL_DUPLICATES		// DELETE_SAME_SOURCE_ONLY / DELETE_ALL_DUPLICATES



// uORF and uORF columns to print to csv
#define uORF_COLUMN_SELECTION_TYPE			ALL_PERTINENT_COLUMNS		// CANONICAL_uORF_COLUMNS / ALL_COLUMNS / ALL_PERTINENT_COLUMNS
#define uORFs_TO_PRINT						ALL_uORFS					// CANONICAL_uORFS_ONLY / EXP_TRANSLATED_uORFs_ONLY / ORFS_MATCHING_GO_TERMS / ALL_uORFS

#define SEARCHED_GO_TERMS	\
"protein binding transcription factor activity; \
nucleic acid binding transcription factor activity; \
protein kinase activity; \
DNA binding; \
nucleotide binding; \
signal transducer activity"
/*anatomical structure development"*/


#define PRINT_GO_MATCHING_OR_NOT			GO_TERMS_MATCH				// GO_TERMS_DONT_MATCH / GO_TERMS_MATCH

// File Naming
#define INPUT_FILE_NAMING_METHOD			GENERATE_FILE_NAME			// REQUEST_FILE_NAME / GENERATE_FILE_NAME
#define OUTPUT_FILE_NAMING_METHOD			GENERATE_FILE_NAME			// REQUEST_FILE_NAME / GENERATE_FILE_NAME

////////////////////////////////////////////////////////////////////////////////



#endif // DEFS__APPL_PARAMETERS_H


