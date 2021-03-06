//==============================================================================
// Project	   : uORF
// Name        : uORF__compile.h
// Author      : Garin Newcomb
// Email       : gpnewcomb@live.com   
// Version     : See "Revision History" below
// Copyright   : Copyright 2014 University of Nebraska-Lincoln
// Description : Header file including function prototypes and object
//				 definitions used in uORF extraction and compilation
//==============================================================================
//
//  Revision History
//      v0.0.0 - 2014/06/05 - Garin Newcomb
//          Initial creation of file, pulling primarily from the previous "uORF_manip" used with
//			this program
//
//    	Appl Version at Last File Update::  v0.0.x - 2014/06/05 - Garin Newcomb
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
//      G. UNUSED Non-Member Function Declarations
//
////////////////////////////////////////////////////////////////////////////////



#ifndef uORF__COMPILE_H
#define uORF__COMPILE_H



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
#include "support__general.h"
#include "support__file_io.h"
#include "support__bioinformatics.h"

////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//
// C. Type (and Member Function) Declarations and Definitions
//
////////////////////////////////////////////////////////////////////////////////

//==============================================================================

// Define an enumeration for the various fields in a uORF .CSV file
// (Note:  global scope, so these values are accessible for any file that includes this header)
enum Tenum_uORF_CSV_columns
{
	uORF_CSV_COL_NUM_CHROM_NUM,		
	uORF_CSV_COL_NUM_SYST_NAME,	
	uORF_CSV_COL_NUM_GENE_CONTEXT,		
	uORF_CSV_COL_NUM_AVG_TSS_POS,		
	uORF_CSV_COL_NUM_TSS_REL_POS,		
	uORF_CSV_COL_NUM_SINGLE_PEAK_TSS,		
	uORF_CSV_COL_NUM_TL_SHAPE_INDEX,		
	uORF_CSV_COL_NUM_GENE_AUGCAI,		
	uORF_CSV_COL_NUM_GENE_CHANGE,		
	uORF_CSV_COL_NUM_CDS_RIB_RPKM,
	uORF_CSV_COL_NUM_CDS_mRNA_RPKM,
	uORF_CSV_COL_NUM_TRANSL_CORR,
	uORF_CSV_COL_NUM_RPM_READS,
	uORF_CSV_COL_NUM_TRANSL_EFF,
	uORF_CSV_COL_NUM_H_UPF1_RATIO,
	uORF_CSV_COL_NUM_H_UPF2_RATIO,
	uORF_CSV_COL_NUM_H_UPF3_RATIO,
	uORF_CSV_COL_NUM_H_UPF1_Q_VAL,
	uORF_CSV_COL_NUM_H_UPF2_Q_VAL,
	uORF_CSV_COL_NUM_H_UPF3_Q_VAL,
	uORF_CSV_COL_NUM_L_UPF1_RATIO,
	uORF_CSV_COL_NUM_L_UPF2_RATIO,
	uORF_CSV_COL_NUM_L_UPF3_RATIO,
	uORF_CSV_COL_NUM_L_UPF123_RATIO,
	uORF_CSV_COL_NUM_L_UPF1_Q_VAL,
	uORF_CSV_COL_NUM_L_UPF2_Q_VAL,
	uORF_CSV_COL_NUM_L_UPF3_Q_VAL,
	uORF_CSV_COL_NUM_L_UPF123_Q_VAL,
	uORF_CSV_COL_NUM_PUB1_ZSCORE,
	uORF_CSV_COL_NUM_W_PUB1_HL,
	uORF_CSV_COL_NUM_WO_PUB1_HL,
	uORF_CSV_COL_NUM_PUB1_STABILITY,
	//uORF_CSV_COL_NUM_NO_NMD_mRNA,
	uORF_CSV_COL_NUM_NMD_TARGET,
	uORF_CSV_COL_NUM_GUAN_W_NMD_HL,
	uORF_CSV_COL_NUM_GUAN_WO_NMD_HL,
	uORF_CSV_COL_NUM_GUAN_FCR,
	uORF_CSV_COL_NUM_GUAN_P_VAL,
	uORF_CSV_COL_NUM_HGN_UPF1_RATIO,
	uORF_CSV_COL_NUM_HGN_PUB1_RATIO,
	uORF_CSV_COL_NUM_HGN_UPF1_Q_VAL,
	uORF_CSV_COL_NUM_HGN_PUB1_Q_VAL,
	uORF_CSV_COL_NUM_JHNS_mRNA_BINDING,
	uORF_CSV_COL_NUM_JHNS_mRNA_DECAY,
 /*uORF_CSV_COL_NUM_RPF_PERC_CIS,
	uORF_CSV_COL_NUM_mRNA_PERC_CIS,
	uORF_CSV_COL_NUM_EFF_PERC_CIS,*/
	uORF_CSV_COL_NUM_GO_TERM_SORTED,
	uORF_CSV_COL_NUM_GO_BIO_PROC,
	uORF_CSV_COL_NUM_GO_CELL_COMP,
	uORF_CSV_COL_NUM_GO_MOL_FUNC,
	uORF_CSV_COL_NUM_NUM_uORFs,
	
	uORF_CSV_COL_NUM_SOURCE,
	uORF_CSV_COL_NUM_EVIDENCE_TYPE,
	uORF_CSV_COL_NUM_uORF_POS,		
	uORF_CSV_COL_NUM_uORF_LEN,		
	uORF_CSV_COL_NUM_uORF_REL_POS,	
	uORF_CSV_COL_NUM_uORF_CAP_DIST,	
	uORF_CSV_COL_NUM_uORF_IN_TRANSCR,	
	uORF_CSV_COL_NUM_uORF_TO_TSS,	
	uORF_CSV_COL_NUM_AUGCAI,	
	uORF_CSV_COL_NUM_CDI,	
	uORF_CSV_COL_NUM_PROB_OF_TRANSL,	
	uORF_CSV_COL_NUM_RIBOSOMES,	
	uORF_CSV_COL_NUM_mRNA,	
	uORF_CSV_COL_NUM_TRANSL_EFFECT,	
	uORF_CSV_COL_NUM_CLUSTER,	
	uORF_CSV_COL_NUM_CLUST_DIST,	
	uORF_CSV_COL_NUM_uORF_CONTEXT,
		
	uORF_CSV_COL_NUM_PROBLEM,
	uORF_CSV_COL_NUM_uORF_IN_FRAME,		
	uORF_CSV_COL_NUM_START_CODON,	
	uORF_CSV_COL_NUM_START_MOVED,	
	uORF_CSV_COL_NUM_PAST_GENE,	
	uORF_CSV_COL_NUM_LEN_CHANGE,	
  //uORF_CSV_COL_NUM_TSS_REL,	
		
	uORF_CSV_COL_NUM_EXT_uORF_CONTEXT,	
	uORF_CSV_COL_NUM_EXT_GENE_CONTEXT,	
	uORF_CSV_COL_NUM_uORF,	
	
	NUM_CSV_uORF_COLUMNS			
};
//==============================================================================



enum Tenum_Miura_TSS_CSV_columns
{
	TSS_CSV_COL_NUM_CLONE_ID,	
	TSS_CSV_COL_NUM_SYST_NAME,	
	TSS_CSV_COL_NUM_START_COORD,	
	TSS_CSV_COL_NUM_END_COORD,	

	NUM_CSV_TSS_COLUMNS
};	
//==============================================================================	
	
	
	
enum Tenum_uORF_exp_transl_tss
{
	uORF_DOWNSTREAM_OF_KNOWN_TSS,		
	uORF_UPSTREAM_OF_ALL_KNOWN_TSS,	
	uORF_TOO_CLOSE_TO_TSS,
	uORF_INEFFICIENTLY_TRANSLATED,
	uORF_EFFICIENTLY_TRANSLATED,
	NO_KNOWN_TSS,		
};
//==============================================================================



enum Tenum_Hogan_RBPs
{
	HOGAN_UPF1,
	HOGAN_PUB1,
	
	HOGAN_NUM_RBPs,
	
	HOGAN_PAB1
};
//==============================================================================



enum Tenum_NMD_factors
{
	UPF1,		
	UPF2,	
	UPF3,
	UPF123,		
};
//==============================================================================



class TData_Source
{
	private:
		std::string author;
		std::string evidence_type;
		
	public:
		std::string get_author  ( void ) const;
		std::string get_evidence( void ) const;
		
		TData_Source( const std::string & paper_author );
};
//==============================================================================



class TRatio_Stat
{
	public:
		std::string ratio_name;
		double ratio;
		double q_val;
	
		TRatio_Stat( std::string name, double ratio_stat, double q_value ) :
			ratio_name( name 	   ),
			ratio	  ( ratio_stat ),
			q_val	  ( q_value    )
		{ }
};
//==============================================================================



class TuORF_Effect
{
	private:
		std::string effect;
		std::string source;
		bool hypoth;
		bool specified;
		
	public:
		std::string get_source	 ( void ) const;
		std::string get_effect	 ( void ) const;
		bool 		get_hypoth	 ( void ) const;
		bool 		get_specified( void ) const;
		
		std::string print_effect_to_str( void ) const;
		
		
		TuORF_Effect( const std::string & uORF_effect, const std::string & data_source, const bool hypothesized, const bool spec ) :
			effect	 ( uORF_effect  ),
			source	 ( data_source  ),
			hypoth	 ( hypothesized ),
			specified( spec  		)
		{ }
		
		TuORF_Effect( void ) :
			effect	 ( ""   ),
			source	 ( ""   ),
			hypoth	 ( TRUE ),
			specified( TRUE )
		{ }
};
//==============================================================================



class TuORF_Data
{
	public:
		std::string content;
		std::string start_context;
		std::string ext_start_context;
		std::vector<unsigned int> context_profile;
		int rel_uORF_pos;
		unsigned int start_pos;
		unsigned int len;
		unsigned int exp_len;
		double AUGCAI;
		std::vector <TData_Source> source;
		int num_ribosomes;
		int num_mRNA;
		TuORF_Effect transl_effic_effect;
		int distance_to_tss;
		double cap_distance_index;
		double prob_of_translation;
		int cluster;
		double distortion;
		double avg_cap_dist;
		double prop_transcripts_incl;
		
		bool problem;
		
		bool in_frame;
		bool start_codon;
		int num_nt_start_moved;
		bool end_past_gene_start;
		int num_nt_len_changed;
		Tenum_uORF_exp_transl_tss exp_transl_tss;
		

		TuORF_Data( const std::string uORF_content,
					const std::string uORF_start_context,
					const std::string uORF_ext_start_context,
					const int relative_uORF_pos,
					const unsigned int uORF_start_pos,
					const unsigned int uORF_len,
					const unsigned int exp_uORF_len,
					const std::string & data_source,
					const bool uORF_in_frame,
					const bool start,  
					const unsigned int nt_start_moved,  
					const bool end_past_gene );
					
		TuORF_Data( const unsigned int uORF_start_pos );
};
//==============================================================================



class TORF_Data
{
	private:
		std::vector <TuORF_Data> uORFs;

	public:
		std::string UTR_content;
				
		std::string gene_name;
		std::string gene_start_context;
		std::string ext_gene_start_context;
		double gene_AUGCAI;
		unsigned int chrom_num;
		TFeature gene_CDS;
		unsigned int gene_intergen_start_coord;
		int gene_pos_change;
		bool opposite_strand;
		double avg_TSS_rel_pos;
		std::vector <int> tss_rel_pos;
		int single_peak_tss_rel_pos;
		double TL_shape_index;
		std::vector <TFeature> untransl_reg;
		TFeature fpUTR_intron;
		double CDS_rib_rpkM;
		double CDS_mRNA_rpkM;
		double transl_corr;
		int RPF_reads;
		double transl_eff;
		double RPF_perc_cis;
		double mRNA_perc_cis;
		double transl_eff_perc_cis;
		vector <TRatio_Stat> He_mRNA_changes;
		vector <TRatio_Stat> Lelivelt_mRNA_changes;
		double PUB1_binding_zscore;
		string stability_PUB1; 
		double w_PUB1_half_life;
		double wo_PUB1_half_life; 
		//unsigned int no_NMD_mRNA_abundance;
		unsigned int direct_NMD_target;
		double guan_w_NMD_half_life;
		double guan_wo_NMD_half_life;
		double guan_FCR;
		double guan_pval;
		vector <TRatio_Stat> Hogan_mRNA_changes;
		double Johansson_mRNA_binding_fold_change;
		double Johansson_mRNA_decay_fold_change;
		
		
		vector <TGO_Annotation> GO_Annotations;
		string GO_term_to_sort_curr_level;
		string GO_term_sorted_by;
		vector <TGO_Term> most_specific_GO_term;
		bool matches_GO_term;

		
		std::vector <TuORF_Data> get_uORFs( void ) const;
		TuORF_Data get_uORF( unsigned int uORF_it ) const;
		void delete_uORF   ( unsigned int uORF_it );
		void add_uORF( const TuORF_Data uORF );
		
		int extract_uORF( const int exp_rel_uORF_pos,
						  const unsigned int exp_uORF_len,
						  const std::string & exp_start_codon,
						  const std::string & chrom_seq,
						  const std::string & data_source,
						  unsigned int * const num_realigned_uORFs  = NULL,
						  unsigned int * const num_misaligned_uORFs = NULL,
						  const std::string & exp_start_context = "",
						  const unsigned int num_ribosomes = DEFAULT_NUM_RIBOSOMES,
						  const unsigned int num_mRNA 	   = DEFAULT_NUM_mRNA );
		
		void add_GO_Annotation( const std::string & name_space, const std::string & term, const std::string & evidence, const std::vector <TGO_Annotation> & All_GO_Annotations );
		void sort_tss( void );
		void add_tss( int new_tss );
		void add_tss( std::vector <int> new_tss );
		void calculate_AUGCAI_values( void );
		
		void update_AUGCAI ( const unsigned int uORF_it, double new_AUGCAI  );
		void update_num_rib( const unsigned int uORF_it, const int new_num_rib, const int new_num_mRNA );
		void add_source	   ( const unsigned int uORF_it, unsigned int pos_to_add, std::string added_source );
		void update_source ( const unsigned int uORF_it, std::vector <TData_Source> data_source );
		void update_context_profile( const unsigned int uORF_it, std::vector <unsigned int> new_context_profile );
		void update_cluster		   ( const unsigned int uORF_it, int new_cluster );
		void update_distortion	   ( const unsigned int uORF_it, double new_distortion );
		void clear_start_pos_change( const unsigned int uORF_it );
		void clear_length_change   ( const unsigned int uORF_it );
		void clear_problem   	   ( const unsigned int uORF_it );
		
		int add_uORF_transl_effect( const std::string & effect, const std::string & source, const bool hypthothesized, const int uORF_rel_pos );
		void check_uORF_upstream_of_tss( void );
		void get_CDI( const std::vector <unsigned int> TL_read_positions, const std::vector <double> TL_read_densities ); 
		
		void calc_uORF_prob_of_transl( void );

		unsigned int find_uORF( const int uORF_rel_pos ) const;
		unsigned int find_GO_term	   ( const int GO_namespace_it, const std::string & term, const bool search_parent_terms = FALSE ) const;
		bool   		 is_GO_term_present( const int GO_namespace_it, const std::string & term, const bool search_parent_terms = FALSE ) const;
		void sort_uORFs( void );	
		
	
		// Comparator Functor to be used with 'sort()'
		struct compare_uORFs
		{ 
		   const TORF_Data& m_ORF_Data;

		   bool operator()( const TuORF_Data & uORF_1, const TuORF_Data & uORF_2 );
		   
		   compare_uORFs( const TORF_Data& ORF_Data ) : m_ORF_Data( ORF_Data ) { }
	    };
		
		std::vector <std::vector <std::string> > form_vector_for_csv_rows( const std::vector <Tenum_uORF_CSV_columns> & col_to_write, const unsigned int select_uORFs ) const;
		int get_gene_start_context( const std::string & chrom_seq );


		TORF_Data( const unsigned int chr_num, 
				   const std::string gene, 
				   const unsigned int start, 
				   const unsigned int length,
				   const unsigned int intergen_start,
				   const int change, 
				   const bool strand, 
				   const std::vector <TFeature> untrans,
				   const TFeature fpUTR_in,
				   const double CDS_rib = DEFAULT_CDS_RIB_RPKM );
};
//==============================================================================

////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//
// D. Non-Member Function Declarations
//
////////////////////////////////////////////////////////////////////////////////

void compile_uORF_list( const TFasta_Content & S_Cerevisiae_Chrom, std::vector <TORF_Data> * const ORF_Data );
void compile_uORF_list_from_data( const TFasta_Content & S_Cerevisiae_Chrom, std::vector <TORF_Data> * const ORF_Data );
void add_GO_info_to_ORFs( std::vector <TORF_Data> * const ORF_Data, const std::vector <TGO_Annotation> & All_GO_Annotations );
void determine_ORF_and_uORF_characteristics( const TFasta_Content & S_Cerevisiae_Chrom, std::vector <TORF_Data> * const ORF_Data );
int  get_uORF_effects_on_gene_translation( std::vector <TORF_Data> * const ORF_Data );
void get_Miura_TSS_data( std::vector <TORF_Data> * const ORF_Data );
void get_Zhang_Dietrich_TSS_data( std::vector <TORF_Data> * const ORF_Data );
void get_Xu_TSS_data( std::vector <TORF_Data> * const ORF_Data );
void get_Yassour_TSS_data( std::vector <TORF_Data> * const ORF_Data );
void get_Nagalakshmi_TSS_data( std::vector <TORF_Data> * const ORF_Data );
void get_David_TSS_data( std::vector <TORF_Data> * const ORF_Data );
int  add_TSS_list( const std::vector <unsigned int> & tss_coord_list, const std::vector <std::string> & gene_name_list, std::vector <TORF_Data> * const ORF_Data );
void get_Brar_translation_data							( std::vector <TORF_Data> * const ORF_Data );
void get_McManus_translation_data						( std::vector <TORF_Data> * const ORF_Data );
void add_present_in_uORF_list_col_to_McManus_transl_data( const std::vector <TORF_Data> & ORF_Data );
void get_Arribere_TL_data								( std::vector <TORF_Data> * const ORF_Data );
void get_Arribere_SI_data								( std::vector <TORF_Data> * const ORF_Data );     
void get_Arribere_Cap_Distance_index					( std::vector <TORF_Data> * const ORF_Data );                      
void get_He_mRNA_change_data      						( std::vector <TORF_Data> * const ORF_Data );
void get_Lelivelt_mRNA_change_data						( std::vector <TORF_Data> * const ORF_Data );
void get_Duttagupta_PUB1_binding_data					( std::vector <TORF_Data> * const ORF_Data );
void get_Duttagupta_PUB1_effects_data					( std::vector <TORF_Data> * const ORF_Data );
void get_Duttagupta_PUB1_mRNA_half_lives_data			( std::vector <TORF_Data> * const ORF_Data );
void get_Guan_NMD_sensitive_transcripts_data			( std::vector <TORF_Data> * const ORF_Data );
void get_Guan_uORFs_in_NMD_path_data					( std::vector <TORF_Data> * const ORF_Data );
void get_Hogan_PUB1_data								( std::vector <TORF_Data> * const ORF_Data );
void get_Hogan_UPF1_data								( std::vector <TORF_Data> * const ORF_Data );
void get_Johansson_mRNA_binding_data					( std::vector <TORF_Data> * const ORF_Data );
void get_Johansson_mRNA_decay_data						( std::vector <TORF_Data> * const ORF_Data );



unsigned int find_ORF( const std::string & gene_name, const std::vector <TORF_Data> & ORF_Data );

TCSV_Contents * extract_uORF_rows_from_Miura_table( void );

int extract_gene_coord_Miura( const TCSV_Contents & uORF_table, const std::vector <std::string> & gff_annotations, std::vector <TORF_Data> * const ORF_Data, unsigned int * const uORF_data_start_it );

int extract_UTR_coord_from_Miura_table_row( const std::string & UTR_coord_raw, 
											const std::string & gene_name,
											const unsigned int UTR_exp_length, 
											unsigned int * const UTR_chrom_num,    
											unsigned int * const UTR_start_pos,  
											unsigned int * const UTR_end_pos,  
											bool 		 * const opposite_strand,
											unsigned int * const UTR_length = NULL );
									  

int extract_uORFs_Miura( const TCSV_Contents & uORF_table, const std::vector <std::string> & yeast_chromosomes, std::vector <TORF_Data> * const ORF_Data, const unsigned int uORF_data_start_it );


int extract_gene_coord_Ingolia( const TCSV_Contents & uORF_table, 
							    const std::vector <std::string> & gff_annotations, 
							    const std::vector <std::string> & yeast_chromosomes, 
							    std::vector <TORF_Data> * const ORF_Data, 
							    unsigned int * const uORF_data_start_it );
							   
int extract_uORFs_Ingolia			 ( const TCSV_Contents & uORF_table, const std::vector <std::string> & yeast_chromosomes, std::vector <TORF_Data> * const ORF_Data, const unsigned int   uORF_data_start_it );
int extract_gene_coord_Zhang_Dietrich( const TCSV_Contents & uORF_table, const std::vector <std::string> & gff_annotations,   std::vector <TORF_Data> * const ORF_Data, unsigned int * const uORF_data_start_it );								   
int extract_uORFs_Zhang_Dietrich_NAR ( const TCSV_Contents & uORF_table, const std::vector <std::string> & yeast_chromosomes, std::vector <TORF_Data> * const ORF_Data, const unsigned int   uORF_data_start_it );
int extract_uORFs_Zhang_Dietrich_CG  ( const TCSV_Contents & uORF_table, const std::vector <std::string> & yeast_chromosomes, std::vector <TORF_Data> * const ORF_Data, const unsigned int   uORF_data_start_it );									   
int extract_gene_coord_Nagalakshmi	 ( const TCSV_Contents & uORF_table, const std::vector <std::string> & gff_annotations,   std::vector <TORF_Data> * const ORF_Data, unsigned int * const uORF_data_start_it );
int extract_uORFs_Nagalakshmi     	 ( const TCSV_Contents & uORF_table, const std::vector <std::string> & yeast_chromosomes, std::vector <TORF_Data> * const ORF_Data, const unsigned int   uORF_data_start_it );
int extract_gene_coord_general		 ( const TCSV_Contents & uORF_table, const std::vector <std::string> & gff_annotations,   std::vector <TORF_Data> * const ORF_Data, unsigned int * const uORF_data_start_it );
								
int extract_uORFs_Cvijovic( const TCSV_Contents & uORF_table, const std::vector <std::string> & yeast_chromosomes, std::vector <TORF_Data> *  const ORF_Data, const unsigned int uORF_data_start_it );
int extract_uORFs_Guan    ( const TCSV_Contents & uORF_table, const std::vector <std::string> & yeast_chromosomes, std::vector <TORF_Data> *  const ORF_Data, const unsigned int uORF_data_start_it );
int extract_uORFs_Lawless ( const TCSV_Contents & uORF_table, const std::vector <std::string> & yeast_chromosomes, std::vector <TORF_Data> *  const ORF_Data, const unsigned int uORF_data_start_it );
int extract_uORFs_Selpi   ( const TCSV_Contents & uORF_table, const std::vector <std::string> & yeast_chromosomes, std::vector <TORF_Data> *  const ORF_Data, const unsigned int uORF_data_start_it );

								  

void realign_uORF( const std::string & chrom_seq,
				   const std::string & gene_name,
				   const bool opposite_strand,
				   const std::string & exp_start_codon,
				   const unsigned int uORF_len,
				   int * const rel_uORF_pos,
				   int * const uORF_first_coord_in_chrom_it,
				   std::string * const uORF_content,
				   int * const num_nt_start_moved,
				   bool * const start_codon_found  );

int uORF_analysis( const std::string & chrom_seq,
				   const std::string & gene_name,
				   const unsigned int gene_start_pos,
				   const bool opposite_strand,
				   const bool start_codon_found,
				   const int rel_uORF_pos,
				   int uORF_first_coord_in_chrom_it,
				   const std::vector <TFeature> untransl_reg,
				   std::string * const uORF_content,
				   std::string * const uORF_start_context,
				   std::string * const ext_uORF_start_context,
				   unsigned int * const uORF_start_pos,
				   unsigned int * const uORF_len,
				   bool * const end_past_gene_start,
				   bool * const uORF_in_frame );
									 

									 
void combine_uORFs_by_gene ( std::vector <TORF_Data> * const ORF_Data );
void delete_duplicate_uORFs( std::vector <TORF_Data> * const ORF_Data, const bool uORFs_to_delete = DELETE_ALL_DUPLICATES );

int parse_uORFs_from_list( std::vector <TORF_Data> * const ORF_Data, 
						   const std::vector <std::string> & gff_annotations, 
						   const std::vector <TGO_Annotation> & All_GO_Annotations, 
						   const bool & file_naming_method = GENERATE_FILE_NAME );

void write_Miura_SGD_annot_info_to_txt( void );
void write_Miura_TSS_data_to_csv( void );

////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//
// G. UNUSED Non-Member Function Declarations
//
////////////////////////////////////////////////////////////////////////////////

// int extract_gene_coord_Miura( const TCSV_Contents & uORF_table, std::vector <TORF_Data> * const ORF_Data, const std::vector <std::string> & yeast_genbank, unsigned int * const uORF_data_start_it );
// int extract_gene_coord_Miura( const TCSV_Contents & uORF_table, std::vector <TORF_Data> * const ORF_Data, unsigned int * const uORF_data_start_it );

// int extract_UTRs( const std::vector <std::string> yeast_chromosomes, std::vector <TuORF_Data> * const uORF_Data, const unsigned int uORF_data_start_it );

////////////////////////////////////////////////////////////////////////////////



#endif // uORF__COMPILE_H


