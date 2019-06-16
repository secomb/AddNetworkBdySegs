/************************************************************************
writefile - for AddNetworkBdySegs
TWS May 2019
************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "nrutil.h"

void writefile()
{
	extern int nodname_max, segname_max, nseg, nnod, max_n, mxx, myy, mzz, nodsegm, i_n, nnodbc;
	extern int *segname, *segtyp, **segnodname;
	extern int *nodname, *nodtyp, **nodnod, **nodseg;
	extern float alx, aly, alz, lb, maxl;
	extern float *diam, *lseg, *segvar, *nodvar, **cnode;
	extern char title[100];

	int iseg, inod;
	char s1[50], *pos;
	FILE *ofp;

	ofp = fopen("NetworkNew.dat", "w");
	if ((pos = strchr(title, '\n')) != NULL) *pos = '\0'; //remove newline character from string
	sprintf(s1, " added %i boundary segments", i_n);	//add information
	fprintf(ofp, "%s %s\n", title, s1);
	fprintf(ofp, "%f %f %f box dimensions in microns\n", alx, aly, alz);
	fprintf(ofp, "%i %i %i number of tissue points in x, y, z directions\n", mxx, myy, mzz);
	fprintf(ofp, "%f lb, outer bound distance\n", lb);
	fprintf(ofp, "%f maxl, max.segment length\n", maxl);
	fprintf(ofp, "%i nodsegm, max. number of segments per node\n", nodsegm);
	fprintf(ofp, "%i total number of segments \n", nseg);
	fprintf(ofp, "name type from  to  diam flow HD\n");
	for (iseg = 1; iseg <= nseg; iseg++) {
		fprintf(ofp, "%i %i %i %i %8.3f  0.0  0.0", segname[iseg], segtyp[iseg], segnodname[1][iseg], segnodname[2][iseg], diam[iseg]);
		if (iseg > nseg - i_n) fprintf(ofp, " *** added segment");
		fprintf(ofp, "\n");
	}
	fprintf(ofp, " %i       total number of nodes\n", nnod);
	fprintf(ofp, "name    x       y       z\n");
	for (inod = 1; inod <= nnod; inod++) {
		fprintf(ofp, "%i %7.0f %7.0f %7.0f", nodname[inod], cnode[1][inod], cnode[2][inod], cnode[3][inod]);
		if (inod > nnod - i_n) fprintf(ofp, " *** added node");
		fprintf(ofp, "\n");
	}
	//boundary nodes
	fprintf(ofp, " %i  total number of boundary nodes\n", nnodbc);
	fprintf(ofp, "node bctyp press/flow   HD    PO2\n");
	for (inod = 1; inod <= nnod; inod++) if (nodtyp[inod] == 1) fprintf(ofp, "%i  0  0.0  0.0\n", nodname[inod]);
	fclose(ofp);
}