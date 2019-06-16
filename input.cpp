/************************************************************************
input - for AddNetworkBdySegs
TWS May 2019
************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "nrutil.h"

void input()
{
	extern int nodname_max, segname_max, nseg, nnod, max_n, mxx, myy, mzz, nodsegm;
	extern int *segname, *segtyp, *ista, *iend, **segnodname;
	extern int *nodname, *nodtyp, **nodnod, **nodseg;
	extern float alx, aly, alz, lb, maxl, q, hd;
	extern float *diam, *lseg, *segvar, *nodvar, **cnode;
	extern char title[100];
	extern FILE *ofp1;
	
	int max = 100;
	int iseg, inod;
	char bb[100];
	FILE *ifp;

	ifp = fopen("Network.dat", "r");
	fgets(title, max, ifp);
	fprintf(ofp1,"%s\n", title);
	fscanf(ifp, "%f %f %f%*[^\n]", &alx, &aly, &alz);	//dimensions of box in microns; vertex must be at origin
	fscanf(ifp, "%i %i %i%*[^\n]", &mxx, &myy, &mzz);
	fscanf(ifp, "%f%*[^\n]", &lb);
	fscanf(ifp, "%f%*[^\n]", &maxl);
	fscanf(ifp, "%i%*[^\n]", &nodsegm);
	fscanf(ifp, "%i%*[^\n]", &nseg);
	fgets(bb, max, ifp);
	fgets(bb, max, ifp);
	segname = ivector(1, nseg + max_n);
	segtyp = ivector(1, nseg + max_n);
	ista = ivector(1, nseg + max_n);
	iend = ivector(1, nseg + max_n);
	segnodname = imatrix(1, 2, 1, nseg + max_n);
	diam = vector(1, nseg + max_n);
	lseg = vector(1, nseg + max_n);
	segvar = vector(1, nseg + max_n);
	segname_max = 0;		//needed for numbering new segments
	for (iseg = 1; iseg <= nseg; iseg++) {
		fscanf(ifp, "%i %i %i %i %f %f %f%*[^\n]",
			&segname[iseg], &segtyp[iseg], &segnodname[1][iseg], &segnodname[2][iseg], &diam[iseg], &q, &hd);
		segname_max = IMAX(segname_max, segname[iseg]);
	}
	fgets(bb, max, ifp);
	fscanf(ifp, "%i%*[^\n]", &nnod);
	fgets(bb, max, ifp);
	fgets(bb, max, ifp);
	nodname = ivector(1, nnod + max_n);
	nodtyp = ivector(1, nnod + max_n);
	nodvar = vector(1, nnod + max_n);
	cnode = matrix(1, 3, 1, nnod + max_n);
	nodnod = imatrix(1, nodsegm, 1, nnod + max_n);
	nodseg = imatrix(1, nodsegm, 1, nnod + max_n);
	nodname_max = 0;		//needed for numbering new nodes
	for (inod = 1; inod <= nnod; inod++) {
		fscanf(ifp, "%i %f %f %f%*[^\n]", &nodname[inod], &cnode[1][inod], &cnode[2][inod], &cnode[3][inod]);
		nodname_max = IMAX(nodname_max, nodname[inod]);
	}
	fclose(ifp);	//don't read boundary segments
}