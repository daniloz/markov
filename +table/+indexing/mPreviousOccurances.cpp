#include <math.h>
#include "mex.h"

/* Input Arguments */
#define	KEY_IN		prhs[0]	// Column Vector
#define	LAGS_IN		prhs[1]	// Column Vector
#define	NKINDS_IN	prhs[2]	// Scalar
#define	MAXLAGS_IN	prhs[3]	// Scalar
#define C_IN		prhs[4] // Column Vector

/* Output Arguments */
#define	I_OUT			plhs[0]

/* Debug Flags */
#ifndef DEBUG_VERBOSITY
#define DEBUG_VERBOSITY		0	// 0 = off
								// 1 = verbose
#endif // DEBUG_VERBOSITY

// Check if v is a real (not-complex) column vector
// returns false if check fails and true otherwise
bool checkRealColumnVector(const mxArray* v)
{
	size_t m = mxGetM(v);
    size_t n = mxGetN(v);
    if (!mxIsDouble(v) || mxIsComplex(v) ||	(n != 1))
		return false;

	return true;
}

// Check if s is a real (not-complex) scalar
// returns false if check fails and true otherwise
bool checkRealScalar(const mxArray* s)
{
	size_t m = mxGetM(s);
    size_t n = mxGetN(s);
    if (!mxIsDouble(s) || mxIsComplex(s) ||	(n != 1) || (m != 1))
		return false;

	return true;
}

void mexFunction( int nlhs, mxArray *plhs[],
		  int nrhs, const mxArray*prhs[] )

{
    // Check for proper number of arguments
    if (nrhs != 5) {
	    mexErrMsgIdAndTxt( "MATLAB:table_indexing_previousOccurancesMEX:invalidNumInputs",
                "Five input arguments required.");
    }
    if (nlhs > 1) {
	    mexErrMsgIdAndTxt( "MATLAB:table_indexing_previousOccurancesMEX:maxlhs",
                "Too many output arguments. One allowed.");
    }

    // Check if key and lags are column vectors
    if (!checkRealColumnVector(KEY_IN))
	{
	    mexErrMsgIdAndTxt( "MATLAB:table_indexing_previousOccurancesMEX:invalidY",
                "previousOccurancesMEX requires that key be a column vector.");
    }

    if (!checkRealColumnVector(LAGS_IN))
	{
	    mexErrMsgIdAndTxt( "MATLAB:table_indexing_previousOccurancesMEX:invalidY",
                "previousOccurancesMEX requires that lags be a column vector.");
    }

	if (!checkRealScalar(NKINDS_IN))
	{
	    mexErrMsgIdAndTxt( "MATLAB:table_indexing_previousOccurancesMEX:invalidY",
                "previousOccurancesMEX requires that nKinds be a scalar.");
    }

    if (!checkRealScalar(MAXLAGS_IN))
	{
	    mexErrMsgIdAndTxt( "MATLAB:table_indexing_previousOccurancesMEX:invalidY",
                "previousOccurancesMEX requires that maxLags be a scalar.");
    }

    if (!checkRealColumnVector(C_IN))
	{
	    mexErrMsgIdAndTxt( "MATLAB:table_indexing_previousOccurancesMEX:invalidY",
                "previousOccurancesMEX requires that C be a column vector.");
    }

    double * const key = mxGetPr(KEY_IN);
    double * const lags = mxGetPr(LAGS_IN);
    size_t nKinds = static_cast<size_t>(mxGetScalar(NKINDS_IN));
    int const maxLags = static_cast<int>(mxGetScalar(MAXLAGS_IN));
	double * const C = mxGetPr(C_IN);
	size_t const cRows = mxGetM(C_IN);

	// Get NAN from Matlab
	mxArray *pNanValue = mxCreateDoubleScalar(mxGetNaN());
	double const nanValue = mxGetScalar(pNanValue);

	//[a,b] = size(key);
	size_t const a = mxGetM(KEY_IN);
	size_t const nLags = mxGetM(LAGS_IN);

    // Create a matrix for the return argument
	// I = nan(a,length(lags));
    I_OUT = mxCreateDoubleMatrix( static_cast<mwSize>(a), static_cast<mwSize>(nLags), mxREAL);
    // Assign pointer to the output parameter
    double * const I = mxGetPr(I_OUT);
	for (int el = 0; el < a*nLags; ++el)
		I[el] = nanValue;
	// Filling in is done column-wise, i.e. I[0]=I(1,1), I[1]=I(2,1) etc...

	//prevI = nan(nKinds,maxLags);
	double * prevI = new double[nKinds*maxLags];
	if (prevI == NULL)
	{	// Catch Null pointer = Out of memory
		mexErrMsgIdAndTxt( "MATLAB:table_indexing_previousOccurancesMEX:OutOfMemory",
			"Out-of-memory while allocating prevI with %d elements", nKinds);
	}
	for (int el = 0; el < nKinds*maxLags; ++el)
		prevI[el] = nanValue;

	double * const l_ = new double[maxLags-1];

	// for k=1:a,
	for (int k = 0; k < a; ++k)
	{
		#if DEBUG_VERBOSITY
		mexWarnMsgIdAndTxt("MATLAB:table_indexing_previousOccurancesMEX:Debug",
			"[DEBUG] k = %d", k);
		#endif // DEBUG_VERBOSITY

		unsigned int key_k = static_cast<unsigned int>( key[k] ) -1; // -1 to correct from Matlab to C index
		if (key_k >= cRows)
		{	// catch index out of bounds
			mexErrMsgIdAndTxt( "MATLAB:table_indexing_previousOccurancesMEX:IndexOutOfBounds",
				"Attempted to access C[%d] (C-style indexing); index out of bounds because numel(C)=%d", key_k,cRows);
		}
		unsigned int C_key_k = static_cast<unsigned int>( C[key_k] ) -1; // -1 to correct from Matlab to C index

		// if key(k)>size(prevI,1),
		if (key_k >= nKinds)
		{	// prevI = [prevI;nan(key(k)+10-size(prevI,1),maxLags)]; % expand lookup if necessary
			unsigned int newNrows = key_k+10;

			#if DEBUG_VERBOSITY
			mexWarnMsgIdAndTxt("MATLAB:table_indexing_previousOccurancesMEX:Debug",
				"[DEBUG] Expanding from %d to %d", nKinds, newNrows);
			#endif // DEBUG_VERBOSITY

			double * biggerPrevI = new double[newNrows*maxLags];
			if (biggerPrevI == NULL)
			{	// Catch Null pointer = Out of memory
				mexErrMsgIdAndTxt( "MATLAB:table_indexing_previousOccurancesMEX:OutOfMemory",
					"Out-of-memory while expanding prevI from %d to %d elements", nKinds, newNrows);
			}
			for (int r = 0; r < newNrows ; ++r)
			{
				for (int c = 0 ; c < maxLags ; ++c)
				{
					if (r < nKinds)
						biggerPrevI[r + c*newNrows] = prevI[r + c*nKinds];
					else
						biggerPrevI[r + c*newNrows] = nanValue;
				}
			}
			delete prevI;
			prevI = biggerPrevI;
			nKinds = newNrows;
		}

		#if DEBUG_VERBOSITY
		mexWarnMsgIdAndTxt("MATLAB:table_indexing_previousOccurancesMEX:Debug",
			"[DEBUG] key[k] = %d | C[key[k]] = %d", key_k, C_key_k);
		#endif // DEBUG_VERBOSITY

		// I(k,:) = prevI(C(key(k)),lags);
		for (int col = 0; col < nLags; ++col)
			I[k + col*a] = prevI[C_key_k + (static_cast<unsigned int>(lags[col])-1)*nKinds] +1; // -1/+1 to correct between Matlab & C indexing

		// l_ = prevI(key(k),1:(maxLags-1));
		for (int col = 0; col < maxLags-1; ++col)
			l_[col] = prevI[key_k + col*nKinds];

		// prevI(key(k),2:maxLags) = l_;
		for (int col = 1; col < maxLags;  ++col)
			prevI[key_k + col*nKinds] = l_[col-1];

		// prevI(key(k),1) = k;
		prevI[key_k] = k;
	}

    return;
}
