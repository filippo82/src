/* 1D weighted median filtering. */
/*
  Copyright (C) 2009 University of Texas at Austin
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <rsf.h>
#include <stdio.h>
#include <math.h>

#include "weightmf.h"
#include "median.h"
#include "boundary.h"

int main (int argc, char* argv[]) 
{
    int n1, n2, wn1, wn2;
    int nfw, m, i, j, k;
    bool boundary, data;
    
    float *trace, *tempt, *temp1, *wei, *tempw1;
    sf_file in, out, weights=NULL;
    
    sf_init (argc, argv); 
    in = sf_input("in");
    out = sf_output("out");
    if (NULL != sf_getstring ("weights")) {
	weights = sf_input("weights");
    } else {
	weights = NULL;
    }
    
    if (!sf_histint(in,"n1",&n1)) sf_error("No n1= in input");
    n2 = sf_leftsize(in,1);
    

    if (NULL != weights) {
	if (!sf_histint(weights,"n1",&wn1)) sf_error("No n1= in weights");
	wn2 = sf_leftsize(weights,1);
	if (n1!=wn1 || n2!=wn2) sf_error("Different dimension of inputs");
    }
    
    if (!sf_getint("nfw",&nfw)) sf_error("Need integer input");
    /* filter-window length (positive and odd integer)*/
    
    if (!sf_getbool("boundary",&boundary)) boundary=false;
    /* if y, boundary is data, whereas zero*/

    if (!sf_getbool("data",&data)) data=true;
    /* if y, output data, whereas weighted data */
    
    if (nfw < 1)  sf_error("Need positive integer input"); 
    if (nfw%2 == 0)  nfw = (nfw+1);
    m=(nfw-1)/2;

    trace = sf_floatalloc(n1);
    wei = sf_floatalloc(n1);
    tempt = sf_floatalloc(n1);
    temp1 = sf_floatalloc(nfw);
    tempw1 = sf_floatalloc(nfw);
    
    for(j=0; j < n2; j++) {
	sf_floatread(trace,n1,in);
	if (NULL != weights) {
	    sf_floatread(wei,n1,weights);
	} else {
	    for (i=0; i < n1; i++) {
		wei[i] = 1.;
	    }
	}
	
	for(i=0; i < n1; i++) {
	    tempt[i] = trace[i];
	}
	
	for(i=0; i < n1; i++) {
	    for(k=0; k < nfw; k++) {
		if ((i+k-m) >= 0 && (i+k-m) < n1) {
		    temp1[k] = tempt[i+k-m];
		    tempw1[k] = wei[i+k-m];
		} else if ((i+k-m) < 0) {
		    if (boundary) {
			temp1[k] = tempt[0];
			tempw1[k] = wei[0];
		    } else {
			temp1[k] = 0.;
			tempw1[k] = 0.;
		    }
		} else {
		    if (boundary) {
			temp1[k] = tempt[n1-1];
			tempw1[k] = wei[n1-1];
		    } else {
			temp1[k] = 0.;
			tempw1[k] = 0.;
		    }
		}
		if (!data) temp1[k] *= tempw1[k];
	    }
	    if (data) {
		trace[i] = wmedian(temp1,tempw1,nfw);
	    } else {
		trace[i] = medianfilter(temp1,nfw);
	    }
	}
	
	sf_floatwrite(trace,n1,out);
    }

    exit (0);
}

/* 	$Id$	 */
