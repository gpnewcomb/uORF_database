//==============================================================================
// Project	   : uORF
// Name        : version.h
// Author      : Garin Newcomb
// Email       : gpnewcomb@live.com   
// Version     : See "Revision History" below
// Copyright   : Copyright 2014 University of Nebraska-Lincoln
// Description : Header file to keep track of version history for the entire 
//				 program
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


//==============================================================================
//
//  Application Revision History
//    v0.0.0 - 2014/06/06 - Garin Newcomb
//		- Initial creation of all files
//		- Wrote library to read in and parse .fasta, .gff, and .csv files, as well as write .csv files
//		- Wrote library to extract uORFs from the most recent S. cerevisiae genome based on the data in Miura Table 4
//		- Wrote functions to output uORFs and associated info to a .csv file
//
//    v0.0.1 - 2014/06/11 - Garin Newcomb
//		- Wrote library to extract uORFs from the most recent S. cerevisiae genome based on the data in Ingolia Table S3
//		- Implemented sorting (using custom compare function) of uORFs based on what gene they're associated with
//		  (note that uORFS within a single gene association are not yet sorted)
//		- Added functionality to extract uORFs based on the data in Miura Table 4 when the uORF is listed as the amino
//		  acid sequence, followed by relative position and AUGCAI in parenthesis (e.g. 'MMVIL*(-254 / 0.412)')
//		- Added functionality to extract uORFs based on the data in Miura Table 4 when the uORF isn't listed in the 
//		  associated gene's first row 
//		- Added functionality to extract the uORF's associated gene's start codon context (note that this function also
//		  checks that the start codon is present, removing relevent introns if necessary)
//		- Added columns for data source, uORF start codon context, and gene start codon context to the output CSV
//
//    v0.0.2 - 2014/06/19 - Garin Newcomb
//		- Added 'cap_or_uncap_str()' utility function (capitalizes or uncapitalizes a specified portion of a passed string)
//		- Added 'get_num_diff_chars()' utility function (Returns how many characters differ between two strings.  No alignment)
//		- Added utility functions to convert a DNA sequence to an amino sequence, or find an amino acid sequence in a DNA sequence
//		- Added columns to the uORF list CSV for the assorted problems with the given uORF (e.g. the start codon not found)
//		- Separated uORF Data class into 'TuORF_Data', which contains all data on a single uORF, and 'TORF_Data', 
//		  which contains all data on an ORF (including a vector of 'TuORF_Data' elements for all the associated uORFs)
//		- Extracted code from 'extract_uORFs...()' functions to created 'realign_uORF()'
//			- Fixed bugs for the uORFs on the Crick strand
//			- Generalized to search for any passed start codon desired
//		- Manually fixed some uORF entries in the data CSV's.  Accordingly, the '..._modified.csv' versions of the files are now used
//		- Added 'sort_uORFs()' function to 'TORF_Data' class, which uses a member comparator functor to sort uORFs 
//		  from upstream to downstream
//		- Switched from a scheme in which the Miura UTR's were extracted first, and the uORFs in turn extracted from them, to
//		  one nearly identical to the scheme used for the Ingolia data
//		- Cleaned up and commented much of the existing code
//		- Added ability to remove introns from a gene prior to extracting the start codon context
//		- Added 'combine_uORFs_by_gene()' function to combine all of an ORFs associated uORFs into a single instance of 'TuORF_Data'
//		- Added 'delete_duplicate_uORFs()' function to delete duplicate uORFs (i.e. a uORF documented in multiple data sets)
//		- If a uORFs stop codon is not in the specified place, now locate the first in frame stop codon and adjust the uORFs length
//	
//    v0.0.3 - 2014/06/23 - Garin Newcomb
//		- Modified ' TCSV_Contents::get_csv_colum()' to add option to remove white spaces from the CSV fields
//		- Added capacity to pull the translation data from the Ingolia tables ('# Rib' and 'CDS Rib'), and added them to the uORF list
//		- Switched to using relative file paths to make more portable
//		- Changed Miura uORF extraction scheme to first parse the uORF info fields into vectors for each gene, then parse each vector
//		  element to find the corresponding uORF (done to avoid duplicate uORFs when the relative positions were modified slightly
//		  due to realignment
//		- Wrote library to extract uORFs from the most recent S. cerevisiae genome based on the data in Zhang & Dietrich  
//		  supplemental data from the paper in Current Genetics
//		- Wrote library to extract uORFs from the most recent S. cerevisiae genome based on the data in Zhang & Dietrich  
//		  supplemental data from the paper in Nucleic Acid Research
//		- Wrote library to extract uORFs from the most recent S. cerevisiae genome based on the data in Nagalakshmi 
//		  supplemental data Table S6 from the paper in Nucleic Acid Research
//		- Compiled the code that processed and modified uORFs in the Ingolia and Miura uORF extraction functions into the
//		  'uORF_analysis()' function
//
//    v0.0.4 - 2014/06/27 - Garin Newcomb
//		- Added all uORF extraction functions to 'compile_uORFs()' in order to clean the main
//		- Made application more memory efficient by dynamically allocating classes, deleting when finished, passing by const reference
//		- Wrote library to extract predicted uORFs from the most recent S. cerevisiae genome based on the data in Cvijovic 
//		  supplemental file 2
//		- Wrote library to extract predicted uORFs from the most recent S. cerevisiae genome based on the data in Guan 
//		  supplemental data Table S10
//		- Fixed bugs in comparator functions used with 'std::sort()'- converted for compatibility with weak ordering
//		- Improved 'TCSV_Contents' functionality
//			- Made 'TCSV_Contents::get_column()/get_row()' into templated functions that can return csv columns/rows as
//		  	  vectors of strings, unsigned ints, ints, or doubles
//			- Made 'TCSV_Contents::cell_contents' private, added a getter and various modifier functions
//		- Improved algorithm in 'realign_uORF()' to check for the correct length in a potential uORF before defaulting
//		  to choosing the uORF closest to the expected start
//		- Modified 'extract_gene_coord...()' functions to only search for a gene's coordinates if not already found
//		- Added extended start contexts for both uORFs and associated genes to the uORF list CSV 
//		  (+/- 50 nucleotides, introns not removed)
//		- Fixed bug in 'extract_uORFs_Nagalakshmi()' in which some uORFs could not be identified because their expected length
//		  caused multiple 5' UTR sequences to start with an ATG and end with a stop codon (even though it was not the 
//		  first in-frame stop codon)
//		- Added 'write_gene_list_to_file()' to write list of all genes associated with uORFs to a .txt file (using '\n' delimiters)
//
//    v0.0.5 - 2014/07/01 - Garin Newcomb
//		- Added a Program Log ('TProgram_Log') that stores all messages printed and writes them to a log file when the program concludes
//		- Added various time functions to keep track of, calculate, and format times
//		- Fixed bug in 'TCSV_Contents::parse_csv()' preventing the correct parsing of fields that contained only empty quotations
//		  Note that this is not Excel's standard, since quotes only enclose fields containing the delimiter.  However, files may be 
//		  written in this format by external programs (e.g. the SGD tools)
//		- Wrote library to extract GO annotations from the file obtained from SGD, add them as a vector to elements of the 
//		  'TORF_Data' class
//		- Added functionality to calculate AugCAI values for both uORFs and their associated genes, according to the weight matrices
//		  obtained from the Guan paper
//		- Modified the Nagalakshmi library to add any potential uORFs found that matched the specified length to the uORF list
//
//    v0.0.6 - 2014/07/09 - Garin Newcomb
//		- Wrote library to extract predicted uORFs from the most recent S. cerevisiae genome based on the data in Selpi 
//		  supplemental data Table S-11
//		- Wrote library to extract predicted uORFs from the most recent S. cerevisiae genome based on the data in Lawless 
//		  additional file 9
//		- Modified 'extract_DNA_seq()' to automatically remove a passed list of untranslated regions from the extracted sequence
//		- Modified 'uORF_analysis()' to remove both 5' UTR introns and gene introns/frameshifts from the uORF sequence, and the
//		  associate start codon context
//		- Modified Miura uORF extraction scheme to add the length of the 5' UTR intron to the relative uORF position if the data
//		  file specifies that the uORF relative position was given with the intron removed
//		- Added member function 'TORF_Data::extract_uORF()', called in each 'extract_uORFs...()' function, to reduce duplicate code
//		  and limit member operations outside of the class
//		- Modified 'get_gene_coord_from_gff()' to extract all untranslated regions (gene introns and frameshifts, 5' UTR introns), 
//		  not just the farthest upstream gene intron
//
//    v0.0.7 - 2014/07/16 - Garin Newcomb
//		- Fixed bug in 'TProgram_Log::print_log_to_file()' in which request for user comment at end of application would be skipped
//		  because input was taken from earlier in the program ('get_input_line<>()' is used to remedy this)
//		- Added functionality to 'delete_duplicate_uORFs()' to only delete duplicates from the same source (e.g. Miura), so that
//		  a list of uORFs with a line for each source that includes the uORF can be made
//		- Created 'support__bioinformatics...' files (contents consist of code taken from 'support__general..." and 
//		  "uORF__compile..." files)
//		- Created 'defs__appl_parameters.h' file to store macros that define the program's flow (in lieu of an .ini file)
//			- Converted flow of 'main.cpp' to utilize 'defs__appl_parameters.h' macros to define program flow rather than commenting out
//			  unused sections
//		- Added 'TCSV_Contents::does_column_exist()' to check if a column exists rather than assuming that it does, risking application 
//		  failure
//		- Wrote 'generate_delimited_list()/parse_delimited_list()' to convert between vectors of strings and string lists with fields
//		  separated by a specified delimiter
//		- Wrote 'get_input_line()' templated function to be able to read a line of input as various variable types
//		- Created 'TCount_Term' and 'TCount_Term_Vect' classes to be able to count (and sort) terms based on some arbitrary count
//		- Created 'TData_Source', which automatically associates a paper's author with the evidence they used to determine uORFs
//		- Implemented sorting of genes based on their assigned GO terms
//			- Wrote 'generate_simple_GO_hierarchy()', which reads in GO relationships from the 'go-basic.obo' file (obtained from 
//		      http://www.geneontology.org/page/download-ontology ) and simplifies them into a hierarchy with each term having one parent
//			- Modified 'TORF_Data::add_GO_Annotation()' to add all parent terms of each term listed in SGD (to allow for easy sorting)
//			- Wrote 'sort_ORFs_by_GO_terms()', which sorts genes based on the specified namespace (sorts hierarchically, from most
//			  general term to most specific term), with the most specific GO term in SGD for each gene used to sort them
//		- Wrote 'parse_uORFs_from_list()' to read in a uORF list file and pass all the gathered data into a TORF_Data vector (thus
//		  eliminating the need to compile and sort from scratch every time the program runs)
//		- Fixed bug in which duplicate uORFs for which the kept uORF had changed in length indicated a problem, even if the deleted
//		  uORF's length didn't change
//		- Allowed for a selection of which uORF list columns and which uORFs are shown
//		- Made uORF list file naming more specific (indicates which set of uORFs is printed, and how genes were sorted)
//		- Fixed bug in 'add_GO_info_to_ORFs()' in which the first GO term would not be added
//		- Modified 'delete_duplicate_uORFs()' to delete either all duplicate uORFs, or only duplicate uORFs from the same source
//		
//    v0.0.8 - 2014/07/16 - Garin Newcomb
//		- Wrote 'prompt_for_input<type>()' to simplify prompting the user for input
//		- Modified "write_file_contents()' to request that the user close the specified file upon failure to open the file stream,
//		  then attempt to open it again before erroring out
//		- Improved alternative program flow sections (code and structure) in the 'main()' in order for each alternative flow to be 
//		  able to generate a stand-alone utility app (found in '../utility_apps' folder)
//		- Updated working directory of application to be '../microORF_project' folder so that application would be on the same
//		  file hierarchy level as the files it accesses (e.g. '../data/Cvijovic_predicted_uORF_list.csv')
//
//    v0.1.0 - 2014/07/17 - Garin Newcomb
//		- Created the 'uORF__process.cpp/h' files and moved code from the 'uORF__compile.cpp/h' files that was relevant after the
//		  uORF list had been compiled
//		- Cleaned 'main()' by moving most code into 'uORF__compile.cpp' and 'uORF__process.cpp'
//		- Unless/until the Waern & Snyder group gets back to Dr. Atkin with their missing uORF table, the uORF list is now fully
//		  compiled (i.e. all still-valid uORFs from the known data sources have been correctly added to the list)
//		
//	v0.1.1 - 2014/07/21 - Garin Newcomb
//		- Added the uORF effects on gene translational efficiency from the "uORF_effects_on_translational_efficiency.csv" file
//		  to the uORF list ('get_uORF_effects_on_gene_translation()')
//		- Fixed bug in 'TCSV_Contents::parse_csv()' in which a field that started with an odd number of escape characters followed
//		  by a field-inclusive comma would be interpreted as only including the escape characters
//		- Added/made more robust functionality to modify the cell contents of a 'TCSV_Contents" object
//			- Also wrote 'TCSV_Contents::write_back_to_csv()' to write the modified cell contents to a .csv file
//		- Wrote 'write_Miura_SGD_annot_info_to_txt()' to print lists of the associated features and clone ID's included in the 
//		  Miura SGD annotation GFF file to respective text files
//		- Wrote 'write_Miura_TSS_data_to_csv()' to construct an updated, concatenated version of Miura Table 4 by pulling
//		  the clone coordinates from the Miura SGD annotation GFF file and matching them with their respective features
//		  in Miura Table 1
//		- Added TSS's from the Miura data to the 'TORF_Data' object (and documented this data in the uORF list .csv files)
//			- New column “ORF TSS Relative Positions” notes the positions of all known TSS’s relative to the gene start coordinate
//			- New column “uORF Distance to Farthest Upstream TSS” contains the number of nucleotides separating the farthest 
//			  upstream TSS from the given uORF
//			- New column “uORF Relative to TSS's” documents if there is a potential problem with the uORF’s validity due to 
//			  TSS positions
//		- Added/modified various utility functions
//			- 'extract_fields()' extracts all instances of fields in a string that are between the specified delimiters
//			- 'count_instances_of()' counts all instances of a string within another string
//			- 'get_str<>()' takes a numeric type input and returns it as a string
//			- 'generate_delimited_list<>()' and 'parse_delimited_list<>()' are now templated and accept/return vectors
//			  of strings or numeric types
//
//	v0.1.2 - 2014/08/08 - Garin Newcomb
//		- Added ability to print only uORFs that match (or don't match) a set of GO terms and their children 
//			- Wrote 'check_for_matching_GO_terms()' to flag all ORFs that match one or more GO terms in the specified set
//			- Wrote 'ORF_Data::find_GO_term()' searches a given ORF for a specified GO term
//			- Wrote 'generate_list_of_children()' to streamline finding all children of a given GO Term
//		- Fixed bugs in 'extract_fields()'
//		- Fixed bug in 'delete_duplicate_uORFs()'
//		- Expanded TSS functionality
//			- Added TSSs from Zhang & Dietrich, Xu, Yassour, Nagalakshmi, and David data sets
//			- Expanded range of options for 'ORF_Data::exp_transl_tss'
//		- Added fields to output CSV (CDS mRNA, translation correlation, RPF reads, translational efficiency)
//			- Added ability to extract them to 'parse_uORFs_from_list()' 
//		- Added translation data from Brar and McManus data sets
//
//	v0.1.3 - 2014/09/19 - Garin Newcomb
//		- Added fields to output CSV (number of uORFs per ORF, GO term sorted by, with ancestry)
//		- Modified 'get_gene_coord_from_gff()' to be able to extract a gene's end coordinate
//		- Fixed bugs/improved efficiency in 'generate_list_of_children()' and 'get_all_children_terms()'
//		- Added 'convert_DNA_num_to_string()'/'convert_DNA_string_to_num()' utility functions
//		- Added several functions for calculating AMI profiles (and similar profiles).  Note: very rough right now.
//			- 'get_coding_and_noncoding_DNA()' extracts all coding and noncoding DNA from the genome
//			- 'count_nucleotides()' gives nt distribution (presently for just entire genome)
//			- 'calc_joint_prob_nt_genes()' - calculates joint probabilities for nt pairs in genes
//			- 'calc_joint_prob_nt_TL()' - calculates joint probabilities for nt pairs in DNA upstream of a gene
//			- 'calc_AMI_uORFs()' - calculates aggregate AMI value for each uORF based only on that sequence (ineffective)
//			- 'calc_mod_AMI_uORF_context()' - returns 256 dimension vector for each uORF context 
//			  (1 entry for each nt pair, 1-16 nt's apart)
//			- 'calc_joint_prob_nt_genome()' - calculates joint probabilities for nt pairs in a passed sequence
//			- 'calc_joint_prob_nt_sequence()' - calculates joint probabilities for nt pairs in a passed sequence
//			- 'calculate_AMI_profile()' - calculates AMI profile based on passed joint probabilities/nt probabilities
//			- 'count_uORF_codons()' - gets codon distribution of uORFs
//		- Added gene TL shape indexes from the Arribere data set
//		- Added single peak TSS's from the Arribere data set 
//		- Added UPF[1-3]-associated mRNA abundance data from the He and Lelivelt data sets
//
//	v0.1.4 - 2014/11/10 - Garin Newcomb
//		- Added ability to extract intergenic region of a gene in 'get_gene_coord_from_gff()'
//		- Added 'logb()', 'median()', 'mean()' utility functions
//		- Fixed bug in 'convert_uint_to_roman_numeral()' and removed from 'UNUSED Non-Member Function Definitions' section
//		- Added calculation of "Cap Distance Index" (CDI), which uses TL reads from the Arribere data as well as TSS 
//		  positions from several data sets to calculate an index of how efficiently a uORF is translated based on its
//		  position relative to the TSS in a given (heterogeneous) transcript
//			- Calculated in 'TORF_Data::get_CDI()'
//			- 'get_Arribere_Cap_Distance_index()' extracts TL reads from the Arribere study and calls the function above
//		- Added estimate of the probability a given uORF is translated based on the AugCAI and CDI (model derived from 
//		  empirical data)
//		- Added 'get_Duttagupta_PUB1_binding_data()' to extract PUB1 binding z-scores from Duttagupta Table S2a
//		- Added 'get_Duttagupta_PUB1_effects_data()' to extract mRNA stability changes due to PUB1 interaction from 
//		  Duttagupta Table S1b
//		- Added 'get_Duttagupta_PUB1_mRNA_half_lives_data()' to extract mRNA half lives with and without PUB1 from 
//		  Duttagupta Table S1a
//		- Added 'get_Guan_NMD_sensitive_transcripts_data()' to extract mRNA half life changes with and without NMD, 
///		  and related statistics from Guan Table S2
//		- Added 'get_Guan_uORFs_in_NMD_path_data()' to extract uORFs on transcripts degraded via NMD from Guan table S10
//		  Note: Listed as Table S5 in document "Guan-Supporting_Material.doc"
//		- Added 'get_Hogan_PUB1_data()' to extract mRNA half life changes with and without PUB1, and related statistics
//		  from Hogan Dataset_S3, sheet "Pub1_sam"
//		- Added 'get_Hogan_UPF1_data()' to extract mRNA half life changes with and without UPF1, and related statistics
//		  from Hogan Dataset_S3, sheet "Upf1_sam"
//		- Added 'get_Johansson_mRNA_binding_data()' to extract mRNA half life changes with and without UPF1
//		  from Johansson Supporting Information Table 1
//		- Added 'get_Johansson_mRNA_decay_data()' to extract mRNA quantity with and without UPF1 from Johansson 
//		  Supporting Information Table 2
//
//	v0.1.5 - 2014/01/11 - Garin Newcomb
//		-  Made improvements to 'TORF_Data::get_CDI()'
//			- Added calculation for average TL length
//			- Added calculation for proportion of transcripts that include a uORF
//			- Calculated proportion of transcripts starting at each position
//		- Modified McManus data output to average data sets rather than just taking set B
//		- Added 'cluster_uORF_context_profiles()' to cluster uORFs based on their context profile vectors
//		  (uses externals function 'lbg_clustering()' to perform LBG clustering
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


#ifndef _uORF_VERSION_H_
#define _uORF_VERSION_H_



////////////////////////////////////////////////////////////////////////////////
//
// A. Include Statements, Preprocessor Directives, and Related
//
////////////////////////////////////////////////////////////////////////////////

#define uORF_APPLICATION_NAME         	"uORF_project.exe"

#define uORF_APPL_VERSION_NUMBER		"0.1.5"

////////////////////////////////////////////////////////////////////////////////



#endif  // _uORF_VERSION_H_


