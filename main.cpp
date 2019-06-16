/************************************************************************
AddNetworkBdySegs - program to add boundary segments to networks
Reads Network.dat and writes NetworkNew.dat.
Searches for two-segment nodes with large discrepancy in diameters,
near domain boundary. If segments are not nearly parallel,
then the larger segment is extended to form a boundary node.
Also allows manual selection of nodes needing an additional segment.
TWS, May 2019
***********************************************************************/
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "nrutil.h"
#if defined(__linux__) 
//linux goes here
#elif defined(_WIN32)	//Windows version
#include <Windows.h>
#endif

void cmgui(float *segvar);
void input();
void analyzenet();
void writefile();
void addsegment(int inod);

int nseg, nnod, nnodbc, mxx, myy, mzz, nodsegm, max_n, i_n, nodname_max, segname_max; 
int *nodtyp, *nodname, *segname, *segtyp, *ista, *iend, **segnodname, **nodnod, **nodseg;
float alx, aly, alz, lb, maxl, q, hd;
float *diam, *lseg, *segvar, *nodvar, **cnode;
char title[100];
FILE *ofp1;

int main(int argc, char *argv[])
{
	int iseg, inod, i, inod1, inod2, iseg1, iseg2, cond1, cond2, cond3, nadd, name;
	float lseg1, lseg2, lseg12, boundary_width, diam_ratio, diam0;
	FILE *ifp;

	ofp1 = fopen("NewSegmentList.out", "w");
	if (ifp = fopen("AddBdySegs.dat", "r")) {
		fscanf(ifp, "%f%*[^\n]", &boundary_width);
		fscanf(ifp, "%f%*[^\n]", &diam_ratio);
		fscanf(ifp, "%f%*[^\n]", &diam0);
		fscanf(ifp, "%i%*[^\n]", &max_n);
	}
	else printf("*** Error: Missing file AddBdySegs.dat\n");

#if defined(__linux__) 
//linux code goes here
#elif defined(_WIN32) 			//Windows version
	bool NoOverwrite = false;
	DWORD ftyp = GetFileAttributesA("Current\\");
	if (ftyp != FILE_ATTRIBUTE_DIRECTORY) system("mkdir Current");	//Create a Current subdirectory if it does not already exist.
	CopyFile("Network.dat", "Current\\Network.dat", NoOverwrite);	//copy to "Current" directory
#endif

	input();

	analyzenet();

	//identify 2-segment nodes with large discrepancy in diameters
	fprintf(ofp1, "Segments added at 2-segment nodes with large discrepancy in diameters\n");
	i_n = 0;
	for (inod = 1; inod <= nnod; inod++) if (nodtyp[inod] == 2) {
		iseg1 = nodseg[1][inod];
		iseg2 = nodseg[2][inod];
		cond1 = 0;
		cond2 = 0;
		cond3 = 0;
		if (cnode[1][inod] < boundary_width || cnode[1][inod] > alx - boundary_width) cond1 = 1;
		if (cnode[2][inod] < boundary_width || cnode[2][inod] > aly - boundary_width) cond1 = 1;
		if (cnode[3][inod] < boundary_width || cnode[3][inod] > alz - boundary_width) cond1 = 1;
		if (cond1) {
			if (diam[iseg1] / diam[iseg2] > diam_ratio) cond2 = 1;		//large diameter mismatch
			else if (diam[iseg2] / diam[iseg1] > diam_ratio) cond2 = 1;
		}
		if (cond1*cond2) {
			lseg1 = 0;
			lseg2 = 0;
			lseg12 = 0.;
			if (inod == ista[iseg1]) inod1 = iend[iseg1];
			else inod1 = ista[iseg1];
			if (inod == ista[iseg2]) inod2 = iend[iseg2];
			else inod2 = ista[iseg2];
			for (i = 1; i <= 3; i++) {
				lseg1 += SQR(cnode[i][inod1] - cnode[i][inod]);
				lseg2 += SQR(cnode[i][inod2] - cnode[i][inod]);
				lseg12 += (cnode[i][inod1] - cnode[i][inod]) * (cnode[i][inod2] - cnode[i][inod]);
			}
			if (SQR(lseg12) / lseg1 / lseg2 < 0.75) cond3 = 1;		//at least 30 degrees angle between segments
		}
		if (cond1*cond2*cond3) {		//add a new segment
			i_n++;
			nnodbc++;
			if (i_n > 100) printf("*** Error: number of new nodes is too large, increase max_n\n");
			else addsegment(inod);		//add a new segment and node
		}
	}
	fprintf(ofp1, "\n");

	if (ifp) {
		fprintf(ofp1, "Segments added at nodes on user-supplied list\n");
		fscanf(ifp, "%i%*[^\n]", &nadd);
		for (i = 1; i <= nadd; i++) {
			fscanf(ifp, "%i%*[^\n]", &name);
			for (inod = 1; inod <= nnod; inod++) if (nodname[inod] == name) {
				i_n++;
				nnodbc++;
				if (i_n > 100) printf("*** Error: number of new nodes is too large, increase max_n\n");
				else addsegment(inod);		//add a new segment and node
			}
		}
		fprintf(ofp1, "\n");
		fclose(ifp);
	}


	nseg += i_n;		//update nseg, nnod and nnodbc
	nnod += i_n;
	nnodbc += i_n;

	//identify main existing feeding and draining vessels, useful for looking at cmgui images
	fprintf(ofp1,"Boundary segments with diameter above %f\n", diam0);
	for (inod = 1; inod <= nnod; inod++) if (nodtyp[inod] == 1) {
		iseg = nodseg[1][inod];
		if (diam[iseg] > diam0) fprintf(ofp1,"nodname %i segname %i diam %6.2f at (%7.2f %7.2f %7.2f)\n",
			nodname[inod], segname[iseg], diam[iseg], cnode[1][inod], cnode[2][inod], cnode[3][inod]);
	}
	fclose(ofp1);

	writefile();

#if defined(__linux__) 
	//linux code goes here
#elif defined(_WIN32) 			//Windows version
	CopyFile("NetworkNew.dat", "Current\\NetworkNew.dat", NoOverwrite);//copy to "Current" directory
	CopyFile("NewSegmentList.out", "Current\\NewSegmentList.out", NoOverwrite);//copy to "Current" directory
#endif

	cmgui(segvar);	//added boundary segments are dark red, existing boundary segments are light green

	return 0;
}