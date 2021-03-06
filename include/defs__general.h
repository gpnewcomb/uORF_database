//============================================================================
// Project	   : uORF
// Name        : defs.h
// Author      : Garin Newcomb
// Email       : gpnewcomb@live.com   
// Version     : See "Revision History" below
// Copyright   : Copyright 2014 University of Nebraska-Lincoln
// Description : Header file to define macros used for in the uORF project
//============================================================================


#ifndef DEFS_H
#define DEFS_H

// Definitions related to accessing the relevant FASTA files
//   Total file path for a FASTA file:  PATH_... + FOLDER_... + FILENAME_...
//   Example complete filepath:
//   "C:\Users\Garin\College\microORF\yeast"

#define PATH_FOLDER_SEPARATOR          		"\\"

// Note: For these file paths to work, the application must be located in a file within "C:\\Users\\Garin\\College\\Research_Sayood"
#define PATH_DATA_FOLDER					"..\\data"					   	// "C:\\Users\\Garin\\College\\Research_Sayood\\data"
#define PATH_DATA_SOURCE_FOLDER				"..\\data_source"				// "C:\\Users\\Garin\\College\\Research_Sayood\\data_source"
#define PATH_OUTPUT_FOLDER					"..\\output"				   	// "C:\\Users\\Garin\\College\\Research_Sayood\\output"
#define PATH_CLUSTER_OUTPUT_FOLDER			"..\\output\\clusters"			// "C:\\Users\\Garin\\College\\Research_Sayood\\output\\clusters"
#define PATH_PROGRAM_LOG_FOLDER				"..\\program_logs"				// "C:\\Users\\Garin\\College\\Research_Sayood\\output"
#define PATH_GO_DATABASE_FOLDER				"..\\GO_database"				// "C:\\Users\\Garin\\College\\Research_Sayood\\output"
#define PATH_S_CEREVISIA_FASTA_FOLDER		"..\\data\\S_cerevisiae_genome"	// "C:\\Users\\Garin\\College\\Research_Sayood\\data\\S_cerevisiae_genome"


#define PRAC_CSV_FILE_NAME 					"prac_csv.csv"
#define S_CEREVISIAE_2006_APR_GFF_FILE_NAME	"saccharomyces_cerevisiae_R53-1-1_20060415.gff"
#define S_CEREVISIAE_2006_JAN_GFF_FILE_NAME	"saccharomyces_cerevisiae_R52-1-1_20060121.gff"
#define S_CEREVISIAE_2011_GFF_FILE_NAME		"saccharomyces_cerevisiae_R64-1-1_20110208.gff"
#define S_CEREVISIAE_GFF_FILE_NAME 			"saccharomyces_cerevisiae.gff"


#define uORF_FILE_NAME_MIURA 				"Miura_uORF_list_modified.csv"
#define uORF_AUG_FILE_NAME_INGOLIA 			"Ingolia_uORF_AUG_list_modified.csv"
#define uORF_NON_AUG_FILE_NAME_INGOLIA 		"Ingolia_uORF_nonAUG_list.csv"
#define uORF_TRANSL_FILE_NAME_INGOLIA 		"Ingolia_uORF_translated_list.csv"
#define uORF_mRNA_FILE_NAME_INGOLIA 		"Ingolia_uORF_abundant_mRNA_list.csv"
#define uORF_ALL_FILE_NAME_INGOLIA 			"Ingolia_uORF_all_list_modified.csv"
#define uORF_FILE_NAME_ZHANG_DIETRICH_NAR 	"Zhang_Dietrich_NAR_uORF_list.csv"
#define uORF_FILE_NAME_ZHANG_DIETRICH_CG	"Zhang_Dietrich_CG_uORF_list.csv"
#define uORF_FILE_NAME_NAGALAKSHMI			"Nagalakshmi_uORF_list.csv"
#define uORF_FILE_NAME_CVIJOVIC				"Cvijovic_predicted_uORF_list_modified.csv"
#define uORF_FILE_NAME_GUAN					"Guan_predicted_uORF_list_modified.csv"
#define uORF_FILE_NAME_LAWLESS				"Lawless_predicted_uORF_list_modified.csv"
#define uORF_FILE_NAME_SELPI				"Selpi_predicted_uORF_list_modified.csv"

#define GENE_INFO_FILE_NAME 				"gene_info.csv"

#define uORFs_TRANSL_EFFIC_FILE_NAME		"uORF_effects_on_translational_efficiency.csv"

#define uORF_LIST_FILE_NAME_PREFIX 			"uORF_list"
#define GENE_LIST_FILE_NAME 				"gene_list.txt"

#define PROGRAM_LOG_FILE_NAME_SUFFIX		"__uORF_program_log"

#define GO_DATABASE_OBO_FILE_NAME			"go-basic.obo"

#define GO_HIERARCHY_BP_FILE_NAME  			"GO_hierarchy__biological_process.csv"
#define GO_HIERARCHY_CC_FILE_NAME  			"GO_hierarchy__cellular_component.csv"
#define GO_HIERARCHY_MF_FILE_NAME  			"GO_hierarchy__molecular_function.csv"

#define GO_CHILDREN_LIST_BP_FILE_NAME		"GO_children_list__biological_process.csv"
#define GO_CHILDREN_LIST_CC_FILE_NAME		"GO_children_list__cellular_component.csv"
#define GO_CHILDREN_LIST_MF_FILE_NAME		"GO_children_list__molecular_function.csv"

#define MIURA_SGD_ANNOTATIONS_FILE_NAME  	"Miura_SGD_annotations.gff3"
#define MIURA_TABLE_1_FILE_NAME  			"Miura_Table1.csv"
#define MIURA_SGD_FEATURES_LIST_FILE_NAME  	"Miura_SGD_features_list.txt"
#define MIURA_SGD_CLONE_ID_LIST_FILE_NAME  	"Miura_SGD_clone_id_list.txt"


#define MIURA_TSS_DATA_FILE_NAME  			"Miura_TSS_Data.csv"
#define ZHANG_DIETRICH_TSS_DATA_FILE_NAME	"Zhang_Dietrich_SGD_TSS_annotations.gff3"
#define XU_TSS_DATA_FILE_NAME				"Xu_SGD_TSS_annotations.gff3"
#define YASSOUR_TSS_DATA_FILE_NAME			"Yassour_SGD_TSS_annotations.gff3"
#define NAGALAKSHMI_TSS_DATA_FILE_NAME		"Nagalakshmi_SGD_TSS_annotations.gff3"
#define DAVID_TSS_DATA_FILE_NAME			"David_SGD_TSS_annotations.gff3"


#define BRAR_TRANSL_DATA_FILE_NAME			"Brar_translation_data.csv"
#define McMANUS_TRANSL_DATA_FILE_NAME		"McManus_translation_data.csv"


#define ARRIBERE_SI_SCORES_FILE_NAME		"Arribere_Gilbert_SI_scores.csv"
#define ARRIBERE_TL_LENGTHS_FILE_NAME		"Arribere_Gilbert_TL_lengths.csv"


#define LELIVELT_mRNA_ABUNDANCE_FILE_NAME	"Lelivelt_mRNA_abundance_data.csv"
#define HE_mRNA_ABUNDANCE_FILE_NAME			"He_mRNA_abundance_data.csv"


#define ARRIBERE_TATL_SEQ_FILE_NAME			"Arribere_TATL-Seq_read_densities.csv"


#define DUTTAGUPTA_PUB1_BINDING_FILE_NAME	"Duttagupta_PUB1_binding.csv"
#define DUTTAGUPTA_PUB1_EFFECTS_FILE_NAME	"Duttagupta_PUB1_effects.csv"
#define DUTTAGUPTA_PUB1_mRNA_HL_FILE_NAME	"Duttagupta_PUB1_mRNA_half_lives.csv"
#define GUAN_NMD_TRANSCRIPTS_FILE_NAME		"Guan_NMD_sensitive_transcripts.csv"
#define GUAN_UORFS_IN_NMD_PATH_FILE_NAME	"Guan_uORFs_in_NMD_path.csv"
#define HOGAN_PUB1_DATA_FILE_NAME			"Hogan_PUB1_data.csv"
#define HOGAN_UPF1_DATA_FILE_NAME			"Hogan_UPF1_data.csv"
#define JOHANSSON_mRNA_UPF1_BIND_FILE_NAME	"Johansson_mRNA_UPF1_binding.csv"
#define JOHANSSON_mRNA_DECAY_FILE_NAME		"Johansson_mRNA_decay.csv"



#define ORF_CONTEXT_FASTA_FILE_NAME			"ORF_start_contexts.fasta"
#define uORF_SEQUENCE_FASTA_FILE_NAME		"uORF_sequences.fasta"
#define uORF_CONTEXT_FASTA_FILE_NAME		"uORF_start_contexts.fasta"

#define uORF_SEQUENCE_CLUSTER				"uORF_sequences_cluster"
#define uORF_CONTEXT_CLUSTER				"uORF_start_contexts_cluster"



#define FASTA_SEQ_START_ID 					">"
	
#define FASTA_FILE_EXTENSION 				".fasta"
#define GFF_FILE_EXTENSION 					".gff"
#define CSV_FILE_EXTENSION 					".csv"
#define TXT_FILE_EXTENSION 					".txt"
	
#define GFF_FILE_FASTA_ID					"##FASTA\n"

#define DEFAULT_HEADER_ROW					1


#define MIURA_UTR_COORD_HEADER				"Coordinates of 5'-UTR"
#define MIURA_uORFS_PRESENT_HEADER			"Number of upstream ATG (uATG)"
#define MIURA_uORFS_INFO_HEADER				"Relative position of uATG to the annotated start codon / Length of uORFs / AUGCAI value (If incalculatable, the value is taken as -1.)"
#define MIURA_SYST_NAME_HEADER				"Systematic name"
#define MIURA_UTR_LENGTH_HEADER				"Length of 5'-UTR (nt)"
#define MIURA_IN_FRAME_uATG_HEADER			"In-frame upstream ATG"
#define MIURA_CLONE_ID_HEADER				"Clone ID"
#define MIURA_COMMENTS_HEADER				"Comments"

#define MIURA_UTR_INTRON_ID					"UTR-intron"

	
	
	
#define INGOLIA_uORF_COORD_HEADER			"uORF"		
#define INGOLIA_SYST_NAME_HEADER			"CDS"		
#define INGOLIA_GENE_NAME_HEADER			"Gene"		
#define INGOLIA_INIT_SITE_HEADER			"Initiator Site"				
#define INGOLIA_uORF_TO_CDS_HEADER			"uORF end to CDS"		
#define INGOLIA_NUM_RIB_HEADER				"# Rib"		
#define INGOLIA_NUM_mRNA_HEADER				"# mRNA"		
#define INGOLIA_UTR_NUM_RIB_HEADER			"5' UTR # Rib"		
#define INGOLIA_UTR_mRNA_HEADER				"5' UTR mRNA"		
#define INGOLIA_CDS_mRNA_HEADER				"CDS mRNA"		
#define INGOLIA_CDS_RIB_HEADER				"CDS Rib"	



#define ZHANG_DIET_CG_CONTEXT_HEADER		"Start Codon Context"	
#define ZHANG_DIET_CG_REL_POS_HEADER		"Relative Position"	
#define ZHANG_DIET_CG_uORF_HEADER			"uORF"	


#define ZHANG_DIET_NAR_GENE_HEADER			"Gene"		
#define ZHANG_DIET_NAR_TSS_POS_HEADER		"TSSa"	
#define ZHANG_DIET_NAR_uORF_HEADER			"uORF"	
	
	

#define NAGALAKSHMI_SYST_NAME_HEADER		"GeneName"
#define NAGALAKSHMI_CHROM_NUM_HEADER		"Chrom"
#define NAGALAKSHMI_uORF_LEN_HEADER			"uORF_length"
#define NAGALAKSHMI_5PRIME_UTR_HEADER		"Sequence (Note the last ATG is the annotated starting codon)"
#define NAGALAKSHMI_5PRIME_UTR_START_HEADER "Start"

#define NAGALAKSHMI_HEADER_ROW				2



#define CVIJOVIC_INTERGENIC_SIZE_HEADER		"Intergenic Region Size"
#define CVIJOVIC_RELATIVE_POS_HEADER		"Relative Position"



#define GUAN_SYST_NAME_HEADER				"Name"
#define GUAN_uORF_START_HEADER				"start"
#define GUAN_uORF_END_HEADER				"stop"
#define GUAN_uORF_LENGTH_HEADER				"length"
#define GUAN_TSS_REL_POS_HEADER				"transcription start site"
#define GUAN_START_CONTEXT_HEADER			"AUG context"
#define GUAN_POS_NEG3_NT_HEADER				"-3"
#define GUAN_AUGCAI_HEADER					"AUG-CAI(r)"


	
#define SELPI_uORF_ID_HEADER				"uORF ID"
#define SELPI_DISTANCE_TO_CDS_HEADER		"Distance to CDS"
#define SELPI_uORF_LENGTH_HEADER			"uORF's Length"



#define LAWLESS_UTR_LENGTH_HEADER			"UTR Length"
#define LAWLESS_uORF_START_HEADER			"Start"
#define LAWLESS_uORF_LENGTH_HEADER			"uORF Length"
#define LAWLESS_uORF_SEQUENCE_HEADER		"uORF Sequence"



#define GENERAL_CHROMOSOME_HEADER			"Chromosome"
#define GENERAL_SYST_NAME_HEADER			"Systematic Name"
#define GENERAL_uORF_LENGTH_HEADER			"Length"



#define GENE_INFO_GO_NAMESPACE_HEADER		"Gene > GO Annotation > Ontology Term . Namespace"	
#define GENE_INFO_GO_NAME_HEADER			"Gene > GO Annotation > Ontology Term . Name"	
#define GENE_INFO_GO_EVIDENCE_HEADER		"Gene > GO Annotation > Evidence > Code > Code"	
#define GENE_INFO_SYST_NAME_HEADER			"Gene > Systematic Name"	
		
		
	
#define GO_HIERARCHY_NAME_HEADER			"Term Name"	
#define GO_HIERARCHY_LEVEL_HEADER			"Level"	
#define GO_HIERARCHY_PARENT_HEADER			"Parent"	



#define GO_CHILDREN_LIST_PARENT_HEADER		"Parent Term"
#define GO_CHILDREN_LIST_CHILDREN_HEADER	"Children Terms"



#define uORF_LIST_CHROM_NUM_HEADER			"Chromosome"								
#define uORF_LIST_SYST_NAME_HEADER			"Systematic Name" 							
#define uORF_LIST_GENE_CONTEXT_HEADER		"ORF Start Codon Context" 					
#define uORF_LIST_AVG_CAP_DIST_HEADER		"ORF Average TSS Relative Position" 					
#define uORF_LIST_TSS_REL_POS_HEADER		"ORF TSS Relative Positions" 					
#define uORF_LIST_SINGLE_PEAK_TSS_HEADER	"ORF Single Peak TSS [Arribere]" 					
#define uORF_LIST_TL_SHAPE_INDEX_HEADER		"TL Shape Index [Arribere]" 					
#define uORF_LIST_GENE_AUGCAI_HEADER		"ORF AugCAI [Guan]" 						
#define uORF_LIST_GENE_CHANGE_HEADER		"Gene Position Change" 						
#define uORF_LIST_CDS_RIB_RPKM_HEADER		"CDS Rib (rpkM) [Ingolia]" 					
#define uORF_LIST_CDS_mRNA_RPKM_HEADER		"CDS mRNA (rpkM) [Ingolia]" 					
#define uORF_LIST_TRANSL_CORR_HEADER		"TL and ORF TE Correlation [Brar]" 					
#define uORF_LIST_RPM_READS_HEADER			"RPM Reads [McManus]"		
#define uORF_LIST_TRANSL_EFF_HEADER			"Translational Efficiency [McManus]" 									
#define uORF_LIST_H_UPF1_RATIO_HEADER		"UPF1 KO Expression Ratio [He]" 	
#define uORF_LIST_H_UPF2_RATIO_HEADER		"UPF2 KO Expression Ratio [He]" 	
#define uORF_LIST_H_UPF3_RATIO_HEADER		"UPF3 KO Expression Ratio [He]" 	
#define uORF_LIST_H_UPF1_Q_VAL_HEADER		"UPF1 KO Expression q-value [He]" 	
#define uORF_LIST_H_UPF2_Q_VAL_HEADER		"UPF2 KO Expression q-value [He]" 	
#define uORF_LIST_H_UPF3_Q_VAL_HEADER		"UPF3 KO Expression q-value [He]" 	
#define uORF_LIST_L_UPF1_RATIO_HEADER		"UPF1 KO Expression Ratio [Lelivelt]" 	
#define uORF_LIST_L_UPF2_RATIO_HEADER		"UPF2 KO Expression Ratio [Lelivelt]" 	
#define uORF_LIST_L_UPF3_RATIO_HEADER		"UPF3 KO Expression Ratio [Lelivelt]" 	
#define uORF_LIST_L_UPF123_RATIO_HEADER		"UPF123 KO Expression Ratio [Lelivelt]" 	
#define uORF_LIST_L_UPF1_Q_VAL_HEADER		"UPF1 KO Expression q-value [Lelivelt]" 	
#define uORF_LIST_L_UPF2_Q_VAL_HEADER		"UPF2 KO Expression q-value [Lelivelt]" 	
#define uORF_LIST_L_UPF3_Q_VAL_HEADER		"UPF3 KO Expression q-value [Lelivelt]" 
#define uORF_LIST_L_UPF123_Q_VAL_HEADER		"UPF123 KO Expression q-value [Lelivelt]" 

#define uORF_LIST_PUB1_ZSCORE_HEADER		"Pub1p Binding Z-Score [Duttagupta]" 
#define uORF_LIST_W_PUB1_HL_HEADER			"WT Half Life [Duttagupta]" 
#define uORF_LIST_WO_PUB1_HL_HEADER			"pub1- MT Half Life [Duttagupta]" 
#define uORF_LIST_PUB1_STABILITY_HEADER		"mRNA Stability Changes Due to Pub1p [Duttagupta]" 
#define uORF_LIST_NO_NMD_mRNA_HEADER		"mRNA Abundance Change without NMD [Guan]" 
#define uORF_LIST_NMD_TARGET_HEADER			"NMD Target [Guan]" 
#define uORF_LIST_GUAN_W_NMD_HL_HEADER		"WT Half Life [Guan]" 
#define uORF_LIST_GUAN_WO_NMD_HL_HEADER		"NMD- Half Life [Guan]" 
#define uORF_LIST_GUAN_FCR_HEADER	    	"Half Life FCR [Guan]" 
#define uORF_LIST_GUAN_P_VAL_HEADER			"Half Life Change p-value [Guan]" 
#define uORF_LIST_HGN_UPF1_RATIO_HEADER		"mRNA Upf1p Binding Fold Change [Hogan]" 
#define uORF_LIST_HGN_PUB1_RATIO_HEADER		"mRNA Pub1p Binding Fold Change [Hogan]" 
#define uORF_LIST_HGN_UPF1_Q_VAL_HEADER		"mRNA Upf1p Binding q-value [Hogan]" 
#define uORF_LIST_HGN_PUB1_Q_VAL_HEADER		"mRNA Pub1p Binding q-value [Hogan]" 
#define uORF_LIST_JHNS_mRNA_BINDING_HEADER	"mRNA Upf1 Binding Fold Change [Johansson]" 
#define uORF_LIST_JHNS_mRNA_DECAY_HEADER	"mRNA 1 Hour NMD +/- Fold Change [Johansson]" 



								
#define uORF_LIST_GO_TERM_SORTED_HEADER		"GO Term Sorted By" 					
#define uORF_LIST_GO_BIO_PROC_HEADER		"GO Annotation = biological_process"
#define uORF_LIST_GO_CELL_COMP_HEADER		"GO Annotation = cellular_component"
#define uORF_LIST_GO_MOL_FUNC_HEADER		"GO Annotation = molecular_function"
#define uORF_LIST_NUM_uORFs_HEADER			"Number of uORFs"
#define uORF_LIST_SOURCE_HEADER				"uORF Data Source" 								
#define uORF_LIST_EVIDENCE_TYPE_HEADER		"Evidence Type" 							
#define uORF_LIST_uORF_POS_HEADER			"uORF Start Coordinate" 					
#define uORF_LIST_uORF_LEN_HEADER			"uORF Length (Nucleotides)" 				
#define uORF_LIST_uORF_REL_POS_HEADER		"uORF Relative Position" 					
#define uORF_LIST_uORF_CAP_DIST_HEADER		"uORF Average Cap Distance" 					
#define uORF_LIST_uORF_IN_TRANSCR_HEADER	"Proportion of Transcripts Including uORF" 					
#define uORF_LIST_uORF_TO_TSS_HEADER		"uORF Distance to Farthest Upstream TSS" 					
#define uORF_LIST_AUGCAI_HEADER				"uORF AugCAI [Guan]" 						
#define uORF_LIST_CDI_HEADER				"uORF Cap Distance Index" 						
#define uORF_LIST_PROB_OF_TRANSL_HEADER		"uORF Probability of Translation" 						
#define uORF_LIST_RIBOSOMES_HEADER			"uORF # Rib [Ingolia]" 						
#define uORF_LIST_mRNA_HEADER				"uORF # mRNA [Ingolia]" 						
#define uORF_LIST_uORF_CONTEXT_HEADER		"uORF Start Codon Context" 					
#define uORF_LIST_TRANSL_EFFECTS_HEADER		"uORF Effects on Translational Efficiency" 					
#define uORF_LIST_CLUSTER_HEADER			"uORF Cluster" 					
#define uORF_LIST_CLUST_DIST_HEADER			"uORF Distance to Cluster Centroid" 					
#define uORF_LIST_PROBLEM_HEADER			"Problem with uORF?" 						
#define uORF_LIST_uORF_IN_FRAME_HEADER		"uORF In Frame with ORF?" 					
#define uORF_LIST_START_CODON_HEADER		"uORF Start Codon Absent?" 					
#define uORF_LIST_START_MOVED_HEADER		"Change in Start Position"  				
#define uORF_LIST_PAST_GENE_HEADER			"uORF Ends Within Gene?" 					
#define uORF_LIST_LEN_CHANGE_HEADER			"Change in Length" 							
#define uORF_LIST_TSS_REL_HEADER			"Expected uORF Tranlational Efficiency (based on TSS proximity)" 							
#define uORF_LIST_EXT_uORF_CONTEXT_HEADER	"Extended uORF Context" 					
#define uORF_LIST_EXT_GENE_CONTEXT_HEADER	"Extended ORF Context" 						
#define uORF_LIST_uORF_HEADER				"uORF Sequence" 							



#define uORF_TRANSL_SYST_NAME_HEADER		"Systematic Name"
#define uORF_TRANSL_STANDARD_NAME_HEADER	"Standard Name"
#define uORF_TRANSL_EFFECT_HEADER			"Effect"
#define uORF_TRANSL_SOURCE_HEADER			"Source"
#define uORF_TRANSL_uORF_REL_POS_HEADER		"uORF Relative Position (if blank, uORF unknown)"
#define uORF_TRANSL_HYPOTHESIZED_HEADER		"Hypothesized?"



#define MIURA_TABLE1_CLONE_ID_HEADER		"Clone ID"
#define MIURA_TABLE1_CHROMOSOME_HEADER		"Chromosome"
#define MIURA_TABLE1_STRAND_HEADER			"Strand"
#define MIURA_TABLE1_HIT_START_HEADER		"Hit start"
#define MIURA_TABLE1_HIT_END_HEADER			"Hit end"
#define MIURA_TABLE1_SYST_NAME_HEADER		"Systematic name"
#define MIURA_TABLE1_GCAP_QUAL_HEADER		"Average quality of G-cap sequence"
#define MIURA_TABLE1_GCAP_JUDGE_HEADER		"G-cap judgment"
#define MIURA_TABLE1_ASSIGNMENT_HEADER		"Assignment"
#define MIURA_TABLE1_FEATURE_TYPE_HEADER	"Feature type"
#define MIURA_TABLE1_HIT_REL_POS_HEADER		"Relative position of hit start"



#define MIURA_TSS_CLONE_ID_HEADER			"Clone ID"
#define MIURA_TSS_SYST_NAME_HEADER			"Systematic Name"
#define MIURA_TSS_HIT_START_HEADER			"Hit Start Coordinate"
#define MIURA_TSS_HIT_END_HEADER			"Hit End Coordinate"



#define BRAR_TRANSL_SYST_NAME_HEADER		"Gene with apparent regulated leader"
#define BRAR_TRANSL_GENE_NAME_HEADER		"Gene with apparent regulated leader"
#define BRAR_TRANSL_CORRELATION_HEADER		"Corr. Between TE_ORF and TE_leader"
#define BRAR_TRANSL_uORFs_PRESET_HEADER		"AUG uORF"
#define BRAR_TRANSL_LEADER_LONGER_HEADER	"Leader longer in meiotic, vegetative or MATa/a?"



#define McMANUS_TRANSL_SYST_NAME_HEADER		"Gene"
#define McMANUS_TRANSL_IN_uORF_LIST_HEADER	"Present in uORF list"
#define McMANUS_TRANSL_RPF_REP_A_HEADER		"RPF (ribosome occupancy) read counts - Rep A"
#define McMANUS_TRANSL_RPF_REP_B_HEADER		"RPF (ribosome occupancy) read counts - Rep B"
#define McMANUS_TRANSL_mRNA_REP_A_HEADER	"mRNA read counts - Rep A"
#define McMANUS_TRANSL_mRNA_REP_B_HEADER	"mRNA read counts - Rep B"
#define McMANUS_TRANSL_PERC_CIS_RPF_HEADER	"Percent cis - RPF"
#define McMANUS_TRANSL_PERC_CIS_mRNA_HEADER "Percent cis - mRNA"
#define McMANUS_TRANSL_PERC_CIS_EFF_HEADER	"Percent cis - Eff"

#define McMANUS_TRANSL_HEADER_ROW			4


#define ARRIBERE_SI_SYST_NAME_HEADER		"gene"
#define ARRIBERE_SI_SCORE_HEADER			"SI"

#define ARRIBERE_TL_SYST_NAME_HEADER		"Gene"
#define ARRIBERE_TL_LENGTH_HEADER			"TL Length"


#define HE_mRNA_SYST_NAME_HEADER			"ORF"
#define HE_mRNA_RATIO_UPF1_HEADER			"Ratio upf1"
#define HE_mRNA_RATIO_UPF2_HEADER			"Ratio nmd2"
#define HE_mRNA_RATIO_UPF3_HEADER			"Ratio upf3"
#define HE_mRNA_Q_VAL_UPF1_HEADER			"q-value upf1"
#define HE_mRNA_Q_VAL_UPF2_HEADER			"q-value nmd2"
#define HE_mRNA_Q_VAL_UPF3_HEADER			"q-value upf3"

#define HE_mRNA_DATA_NUM_STRAINS			3


#define LELIVELT_mRNA_SYST_NAME_HEADER		"ORF"
#define LELIVELT_mRNA_RATIO_UPF1_HEADER		"Ratio upf1"
#define LELIVELT_mRNA_RATIO_UPF2_HEADER		"Ratio upf2"
#define LELIVELT_mRNA_RATIO_UPF3_HEADER		"Ratio upf3"
#define LELIVELT_mRNA_RATIO_UPF123_HEADER	"Ratio upf123"
#define LELIVELT_mRNA_Q_VAL_UPF1_HEADER		"q-value upf1"
#define LELIVELT_mRNA_Q_VAL_UPF2_HEADER		"q-value upf2"
#define LELIVELT_mRNA_Q_VAL_UPF3_HEADER		"q-value upf3"
#define LELIVELT_mRNA_Q_VAL_UPF123_HEADER	"q-value upf123"

#define LELIVELT_mRNA_DATA_NUM_STRAINS		4

	
	
#define DUTTAGUPTA_SYST_NAME_HEADER			"ORF"	
#define DUTTAGUPTA_MED_ZSCORE_HEADER		"Median z-scores"	
#define DUTTAGUPTA_STABILIZED_HEADER		"573 genes stabilized by Pub1p"	
#define DUTTAGUPTA_DESTABILIZED_HEADER		"78 genes destabilized by Pub1p"	
#define DUTTAGUPTA_NO_CHANGE_HEADER			"896 genes with no  difference in stability"		
#define DUTTAGUPTA_WT_PREFIX_HEADER			"WT"	
#define DUTTAGUPTA_MUTANT_PREFIX_HEADER		"MUT"	
#define DUTTAGUPTA_PVAL_SUFFIX_HEADER		" p-value"	
#define DUTTAGUPTA_HL_SUFFIX_HEADER			" T1/2"	
	
#define DUTTAGUPTA_PUB1_BINDING_HEADER_ROW	3	
#define DUTTAGUPTA_PUB1_EFFECTS_HEADER_ROW	2
#define DUTTAGUPTA_PUB1_HL_HEADER_ROW		3

#define DUTTAGUPTA_NUM_HL_EXPERIMENTS		3

#define STABILIZED							"Stabilized"
#define DESTABILIZED						"Destabilized"
#define SAME_STABILITY						"Same_Stability"
	
	
#define GUAN_NMD_SYST_NAME_HEADER			"ORF"	
#define GUAN_NMD_ABUNDANCE_CHANGE_HEADER	"Higher or Lower Abundance in NMD-"	
#define GUAN_NMD_DIRECT_TARGET_HEADER		"Direct/Indirect"	
#define GUAN_NMD_MUT_HL_HEADER				"mut t1/2"	
#define GUAN_NMD_WT_HL_HEADER				"wt t1/2"	
#define GUAN_NMD_FCR_HEADER					"FCR"	
#define GUAN_NMD_TESTING_STATISTIC_HEADER	"'-2lnl"	
#define GUAN_NMD_PVAL_HEADER				"p-value"	
	
#define GUAN_NMD_GENES_HEADER_ROW			2

#define GUAN_NMD_GENE_DIRECT_TARGET			"Direct"
#define GUAN_NMD_GENE_INDIRECT_TARGET		"Indirect"
#define GUAN_NMD_GENE_HIGHER_ABUNDANCE		"Higher"
#define GUAN_NMD_GENE_LOWER_ABUNDANCE		"Lower"


#define GUAN_NMD_uORF_SYST_NAME_HEADER		"ORF"


#define HOGAN_SYST_NAME_HEADER				"Gene ID"
#define HOGAN_QVAL_HEADER					"q-value(%)"
#define HOGAN_FOLD_CHANGE_HEADER			"Fold Change"


#define JOHANSSON_SYST_NAME_HEADER			"Description"
#define JOHANSSON_FOLD_INCREASE_HEADER		"Fold increase"
#define JOHANSSON_60MIN_NMD2_POS_HEADER		"PGAL1-NMD2 60'"
#define JOHANSSON_60MIN_NMD2_NEG_HEADER		"nmd2- 60'"



	
	
#define INGOLIA_uORF_COORD_SEP				"to"		

#define DATA_SOURCE_MIURA					"Miura"
#define DATA_SOURCE_INGOLIA					"Ingolia"
#define DATA_SOURCE_ZHANG_DIETRICH_NAR		"Zhang_Dietrich_NAR"
#define DATA_SOURCE_ZHANG_DIETRICH_CG		"Zhang_Dietrich_CG"
#define DATA_SOURCE_NAGALAKSHMI				"Nagalakshmi"
#define DATA_SOURCE_CVIJOVIC				"Cvijovic"
#define DATA_SOURCE_GUAN					"Guan"
#define DATA_SOURCE_LAWLESS					"Lawless"
#define DATA_SOURCE_SELPI					"Selpi"

#define EVIDENCE_TYPE_MIURA					"cDNA_Analysis"
#define EVIDENCE_TYPE_INGOLIA				"Ribosome_Footprinting"
#define EVIDENCE_TYPE_ZHANG_DIETRICH_NAR	"Zhang_Dietrich_NAR_Evidence"
#define EVIDENCE_TYPE_ZHANG_DIETRICH_CG		"Zhang_Dietrich_CG_Evidence"
#define EVIDENCE_TYPE_NAGALAKSHMI			"Nagalakshmi_Evidence"
#define EVIDENCE_TYPE_CVIJOVIC				"Computationally_Predicted"
#define EVIDENCE_TYPE_GUAN					"Computationally_Predicted"
#define EVIDENCE_TYPE_LAWLESS				"Computationally_Predicted"
#define EVIDENCE_TYPE_SELPI					"Computationally_Predicted"		

#define DEFAULT_DELIMITER					", "


#define uORF_DOWNSTREAM_OF_KNOWN_TSS_MSG	"" //"Downstream"		// Only show anomalous results
#define uORF_UPSTREAM_OF_ALL_KNOWN_TSS_MSG	"Not_Transcribed"	
#define uORF_TOO_CLOSE_TO_TSS_TSS_MSG		"Not_Translated"	
#define uORF_INEFFICIENTLY_TRANSLATED_MSG	"Inefficiently_Translated"	
#define uORF_EFFICIENTLY_TRANSLATED_MSG		"Efficiently_Translated"	
#define NO_KNOWN_TSS_MSG					"No_Known_TSS"	
	
#define NUM_S_CEREVISIAE_CHROM				16
	

#define STRAND_INDICATOR_POS				6

#define START_CONTEXT_LENGTH_NT				12
#define START_CONTEXT_NT_BEFORE_AUG			6
#define START_CONTEXT_NT_AFTER_AUG			5
	
#define EXT_START_CONTEXT_LENGTH_NT			100
#define EXT_START_CONTEXT_NT_BEFORE_AUG		50
#define EXT_START_CONTEXT_NT_AFTER_AUG		49
	
	
#define NUM_NT_TO_SEARCH_AROUND_EXP			3
#define NUM_NT_TO_COMPLETE_CODON			2
#define NUM_NT_MATCH_uORF_SEQUENCE			9


#define MIN_NUM_BASES_APART					5
#define MAX_NUM_BASES_APART					500

#define MIN_NUM_BASES_APART_CONTEXT			1
#define MAX_NUM_BASES_APART_CONTEXT			16

#define MARGINAL_PROB_NT_A					.309806
#define MARGINAL_PROB_NT_T					.308715
#define MARGINAL_PROB_NT_C					.190882
#define MARGINAL_PROB_NT_G					.190596
	
#define NUM_NT_PAST_GENE_START				300
	
	
#define DEFAULT_AUGCAI						-1					// If incalculable or unknown, the AUGCAI is marked as -1
#define DEFAULT_CDI							-1					// If incalculable or unknown, the CDI is marked as -1
#define DEFAULT_CDS_RIB_RPKM				-1					// If unknown, the density of sequencing reads for CDS ribosomes is marked as -1
#define DEFAULT_CDS_mRNA_RPKM				-1					// If unknown, the density of sequencing reads for CDS mRNA is marked as -1
#define DEFAULT_NUM_RIBOSOMES				-1					// If unknown, the number of sequencing reads for uORF ribosomes is marked as -1
#define DEFAULT_NUM_mRNA					-1					// If unknown, the number of sequencing reads for uORF mRNA is marked as -1
#define DEFAULT_DISTANCE_TO_TSS				-1					// If unknown, the number of nucleotides between TSS and uORF start is marked as -1
#define DEFAULT_TSS_REL_POS					1					// If unknown, the position of the TSS relative to the position of the gene's start codon is marked as 1
#define DEFAULT_CORRELATION					-2					// If unknown, correlations are marked as -2 (since correlations can vary from -1 to 1)
#define DEFAULT_RPF_READS					-1					// If unknown, number of RPF reads is marked as -1
#define DEFAULT_TRANSL_EFF					-1					// If unknown, translational efficiency is marked as -1
#define DEFAULT_PERCENTAGE					-1					// If unknown, percentages are marked as -1
#define DEFAULT_PROBABILITY					-1					// If unknown, probabilities are marked as -1
#define DEFAULT_CLUSTER						-1					// If unknown, clusters are marked as -1
#define DEFAULT_DISTORTION					-1					// If unknown, distortions are marked as -1
#define DEFAULT_SHAPE_INDEX					1					// If unknown, TL shape indexes are marked as 1
#define DEFAULT_RATIO						-1					// If unknown, ratios are marked as -1
#define DEFAULT_Q_VAL						-1					// If unknown, q values are marked as -1
#define DEFAULT_P_VAL						-1					// If unknown, p values are marked as -1
#define DEFAULT_Z_SCORE						-1					// If unknown, Z scores are marked as -1
#define DEFAULT_NAN							999999999			// If not actually a number, mark as 999999999
#define DEFAULT_mRNA_CHANGE					2					// If unknown, mark mRNA abundance change as neither higher or lower
#define DEFAULT_NMD_TARGET					2					// If unknown, mark NMD targeting as neither direct or indirect
#define DEFAULT_HALF_LIFE					-1					// If unknown, mark half lives as -1
	
	



#define GO_BIO_PROC_IT						0
#define GO_CELL_COMP_IT						1
#define GO_MOL_FUNC_IT						2
#define NUM_GO_NAMESPACES					3


#define GO_BIO_PROC_MAX_LEVEL				12
#define GO_CELL_COMP_MAX_LEVEL				9
#define GO_MOL_FUNC_MAX_LEVEL				11


#define GO_BIO_PROC_ID						"biological_process"
#define GO_CELL_COMP_ID						"cellular_component"
#define GO_MOL_FUNC_ID						"molecular_function"


#define DUMMY_GO_LEVEL						0
#define DEFAULT_LARGE_VAL					999999999

#define GO_TERM_LIST_DELIMITER				"; "


#define AUGCAI_WEIGHT_MATRIX_POSN_NEG6		{ 0.038, 0.015, 0.025, 0.044 }
#define AUGCAI_WEIGHT_MATRIX_POSN_NEG5		{ 0.069, 0.062, 0.013, 0.075 }
#define AUGCAI_WEIGHT_MATRIX_POSN_NEG4		{ 0.273, 0.254, 0.009, 0.073 }
#define AUGCAI_WEIGHT_MATRIX_POSN_NEG3		{ 1.043, 0.019, 0.193, 0.039 }
#define AUGCAI_WEIGHT_MATRIX_POSN_NEG2		{ 0.514, 0.14,  0.012, 0.117 }
#define AUGCAI_WEIGHT_MATRIX_POSN_NEG1		{ 0.558, 0.099, 0.161, 0.012 }
#define AUGCAI_WEIGHT_MATRIX_POSN_POS4		{ 0.034, 0.034, 0.155, 0.102 }
#define AUGCAI_WEIGHT_MATRIX_POSN_POS5		{ 0.039, 0.177, 0.044, 0.069 }
#define AUGCAI_WEIGHT_MATRIX_POSN_POS6		{ 0.057, 0.052, 0.038, 0.17  }


#define NUM_AUGCAI_POSITIONS				9
#define MAX_AUGCAI							.221



#define MIN_GCAP_QUALITY			20
#define PERFECT_GCAP				"Perfect G-cap sequence"
#define SINGLE_FEATURE				"single feature"
#define SINGLE_MEATURE_MANUAL		"single feature(selected manually from multiple features)"
#define ORF							"ORF"
#define PSEUDOGENE					"pseudogene"


#define LOW_AUGCAI_CUTOFF			.259	// Minimum AugCAI explicitly included in the probability model 
#define HIGH_AUGCAI_CUTOFF			.693	// Maximum AugCAI explicitly included in the probability model 

#define NUM_CLUSTERS				64


#define FLOAT_OUTPUT_METHOD         fixed  // "fixed" to output doubles using fixed point notation, "scientific" to output using floating-point notation


#define ASCII_UPPER_LOWER_DIFF		32		// ASCII text table difference between lower case and upper case alphabet (97 - 65)



#define NO							0
#define YES							1

#define SUCCESSFUL					0		// Represents that an operation succeeded
#define NOT_SUCCESSFUL				1		// Represents that an operation failed

#define UNINITIALIZED				0
#define INITIALIZED					1

#define ERROR_NOT_IN_PROGRESS    	1       // Default state of global variable, indicating that 'error_response()' should respond to any error that's detected
#define ERROR_IN_PROGRESS        	0       // Set by 'error_response()' when an error is encountered so that future calls to 'error_response()' don't actually result in more errors

#define ORF_1_BEFORE_ORF_2			1
#define ORF_2_BEFORE_ORF_1			0

#define uORF_1_BEFORE_uORF_2		1
#define uORF_2_BEFORE_uORF_1		0

#define TERM_1_BEFORE_TERM_2		1
#define TERM_2_BEFORE_TERM_1		0

#define UNCAPITALIZE				0		// Represents that all letters in a passed string should be uncapitalised
#define CAPITALIZE					1		// Represents that all letters in a passed string should be capitalized

#define DONT_REMOVE_SPACES			0	
#define REMOVE_SPACES				1		

#define INCREMENT					0		
#define DECREMENT					1	

#define BY_COUNT					0		
#define ALPHABETICALLY				1	

#define DECREASING					0		
#define INCREASING					1	

#define LOWER						0		
#define HIGHER						1

#define DELETE_ALL_DUPLICATES		0	
#define DELETE_SAME_SOURCE_ONLY		1	

#define ALL_uORFS					0	
#define CANONICAL_uORFS_ONLY		1	
#define EXP_TRANSLATED_uORFs_ONLY	2
#define ORFS_MATCHING_GO_TERMS		3

#define GENERATE_FILE_NAME			0	
#define REQUEST_FILE_NAME			1

#define FROM_DATA_SOURCES			0
#define FROM_CURRENT_LIST			1

#define READ						0
#define WRITE						1

#define DONT_PRINT_PROMPT_TO_LOG	0
#define PRINT_PROMPT_TO_LOG			1

#define UNSPECIFIED					0
#define SPECIFIED					1

#define GO_TERMS_DONT_MATCH			0
#define GO_TERMS_MATCH				1

#define STRAND_W					0
#define STRAND_C					1

#define INDIRECT					0
#define DIRECT						1


#define SORT_ORFS_BY_POSITION		0
#define SORT_ORFS_BY_GO_TERMS		1
#define DONT_SORT_ORFS				2

#define ALL_COLUMNS					0
#define ALL_PERTINENT_COLUMNS		1
#define CANONICAL_uORF_COLUMNS		2

#define CHARACTER					0
#define DIGIT						1
#define DIGIT_OR_CHARACTER			2

#define NUMBER						0
#define LOWER_CASE					1
#define UPPER_CASE					2
#define SYMBOL						3

#define MILLISECONDS				0
#define SECONDS						1
#define MINUTES						2
#define HOURS						3

#define ASCII_NUMBER_START			48		// ASCII text table value for the first number ('0')
#define ASCII_NUMBER_END			57		// ASCII text table value for the last number ('9')
#define ASCII_LOWER_CASE_START		97		// ASCII text table value for the first lower case letter ('a')
#define ASCII_LOWER_CASE_END		122		// ASCII text table value for the last lower case letter ('z')
#define ASCII_UPPER_CASE_START		65		// ASCII text table value for the first upper case letter ('A')
#define ASCII_UPPER_CASE_END		90		// ASCII text table value for the last upper case letter ('Z')
#define ASCII_UPPER_LOWER_DIFF		32		// ASCII text table difference between lower case and upper case alphabet (97 - 65)


#define NOT_APPLICABLE				"NA"

#define CLEAR_ERROR_MSG_GLOBAL      ""              // When calling 'update_error_msg_global()', used to detect if the error message should be cleared instead of being appended to

#define FATAL						0				// Represents that an error should end the program
#define NONFATAL					1				// Represents that an error shouldn't end the program
#define PASS_UP_ONE_LEVEL			2				// Represents that an error should be passed up to the calling function

#endif // DEFS_H


