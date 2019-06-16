/************************************************************************
analyzenet - for AddNetworkBdySegs
TWS May 2019
************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "nrutil.h"

void analyzenet()
{
	extern int nseg, nnod, nodsegm, nnodbc;
	extern int *nodtyp, *nodname, *segtyp, *ista, *iend, **segnodname, **nodnod, **nodseg;
	extern float *segvar;

	int iseg, inod, i, inod1, inod2, iseg1, iseg2;

	for (iseg = 1; iseg <= nseg; iseg++) {	//Search for nodes corresponding to this segment
		for (inod = 1; inod <= nnod; inod++) if (nodname[inod] == segnodname[1][iseg]) {
			ista[iseg] = inod;
			goto foundit1;
		}
		printf("*** Error: No matching node found for nodname %i\n", segnodname[1][iseg]);
	foundit1:;
		for (inod = 1; inod <= nnod; inod++) if (nodname[inod] == segnodname[2][iseg]) {
			iend[iseg] = inod;
			goto foundit2;
		}
		printf("*** Error: No matching node found for nodname %i\n", segnodname[2][iseg]);
	foundit2:;
	}

	//Setup nodtyp, nodseg and nodnod
	for (inod = 1; inod <= nnod; inod++) nodtyp[inod] = 0;
	for (iseg = 1; iseg <= nseg; iseg++) if (segtyp[iseg] == 4 || segtyp[iseg] == 5) {
		segvar[iseg] = 0.;
		inod1 = ista[iseg];
		inod2 = iend[iseg];
		nodtyp[inod1] = nodtyp[inod1] + 1;
		nodtyp[inod2] = nodtyp[inod2] + 1;
		if (nodtyp[inod1] > nodsegm) printf("*** Error: Too many segments connected to node %i, increase nodsegm in Network.dat\n", inod1);
		if (nodtyp[inod2] > nodsegm) printf("*** Error: Too many segments connected to node %i, increase nodsegm in Network.dat\n", inod2);
		nodseg[nodtyp[inod1]][inod1] = iseg;
		nodseg[nodtyp[inod2]][inod2] = iseg;
		nodnod[nodtyp[inod1]][inod1] = inod2;
		nodnod[nodtyp[inod2]][inod2] = inod1;
	}

	nnodbc = 0;
	for (inod = 1; inod <= nnod; inod++) if (nodtyp[inod] == 1) {
		nnodbc++;
		iseg = nodseg[1][inod];
		segvar[iseg] = 1.;
	}
}