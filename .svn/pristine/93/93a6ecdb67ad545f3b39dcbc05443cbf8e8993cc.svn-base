/**********************************************************************
*                                                                      *
*  File: lbg_cluster.h                                                 *
*  modified from amivq.c on 11/10/2014 by G. Newcomb                   *
*  	split into header and source file for use in uORF project          *
*  Function:  trains a codebook using the splitting approach of Linde, *
*             Buzo, and Gray                                           *
*  Author (aka person to blame) : K. Sayood                            *
*  Last mod: 5/12/95                                                   *
*  Usage:  see usage().  For more details see trvqsplit.doc or the man *
*          page                                                        *
***********************************************************************/

#ifndef LBG_CLUSTERING_H
#define LBG_CLUSTERING_H

#define maxit 100



void lbg_clustering( int dimension, int codebook_size, int ts_size, float **training_set, float ***codebook );
void usage( void );
int   fvqe( float *input, float **codebook, int codebook_size, int dimension, float *distortion );

# endif // LBG_CLUSTERING_H
