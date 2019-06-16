/************************************************************************
addsegment - for AddNetworkBdySegs
Adds a boundary segment to node inod
TWS May 2019
************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "nrutil.h"

void addsegment(int inod)
{
	extern int nseg, nnod, nodsegm, nodname_max, segname_max, i_n;
	extern int *nodtyp, *nodname, *segname, *segtyp, *ista, *iend, **segnodname, **nodnod, **nodseg;
	extern float *segvar, *diam, **cnode;
	extern FILE *ofp1;
	
	int i, inod1, iseg1, imax;
	float lseg1, diam_max;

	nodname_max++;
	segname_max++;
	nodname[nnod + i_n] = nodname_max;
	nodtyp[nnod + i_n] = 1;
	nodseg[1][nnod + i_n] = nseg + i_n;
	nodnod[1][nnod + i_n] = inod;
	nodtyp[inod]++;
	if (nodtyp[inod] > nodsegm) printf("*** Error: Too many segments connected to node %i, increase nodsegm in Network.dat\n", inod);
	nodseg[nodtyp[inod]][inod] = nseg + i_n;
	nodnod[nodtyp[inod]][inod] = nnod + i_n;
	segname[nseg + i_n] = segname_max;
	segvar[nseg + i_n] = 2.;
	segtyp[nseg + i_n] = 5;
	ista[nseg + i_n] = inod;
	iend[nseg + i_n] = nnod + i_n;
	segnodname[1][nseg + i_n] = nodname[inod];
	segnodname[2][nseg + i_n] = nodname_max;
	diam_max = 0;
	for (i = 1; i <= nodtyp[inod]; i++) {	//choose direction according to maximum diameter segment at the node
		iseg1 = nodseg[i][inod];
		if (diam[iseg1] > diam_max) {
			diam_max = diam[nodseg[i][inod]];
			imax = i;
		}
	}
	inod1 = nodnod[imax][inod];
	iseg1 = nodseg[imax][inod];
	diam[nseg + i_n] = diam[iseg1];	//choose diameter equal to maximum of existing segments
	lseg1 = 0.;
	for (i = 1; i <= 3; i++) lseg1 += SQR(cnode[i][inod1] - cnode[i][inod]);
	for (i = 1; i <= 3; i++) cnode[i][nnod + i_n] = cnode[i][inod]
		+ (cnode[i][inod] - cnode[i][inod1]) * 2. * diam[iseg1] / sqrt(lseg1); //length = twice diameter
	fprintf(ofp1, "Segment added at node %i: nodname %i segname %i diam %6.2f at (%7.2f %7.2f %7.2f)\n",
		nodname[inod], nodname[nnod + i_n], segname[nseg + i_n], diam[nseg + i_n],
		cnode[1][nnod + i_n], cnode[2][nnod + i_n], cnode[3][nnod + i_n]);
}