#ifndef WILCOX_FUNCS
#define WILCOX_FUNCS

#include <cfloat>
#include "npq.h"

/*CXXR $Id$
 *CXXR
 *CXXR This file is part of CXXR, a project to refactor the R interpreter
 *CXXR into C++.  It may consist in whole or in part of program code and
 *CXXR documentation taken from the R project itself, incorporated into
 *CXXR CXXR (and possibly MODIFIED) under the terms of the GNU General Public
 *CXXR Licence.
 *CXXR
 *CXXR CXXR is Copyright (C) 2008-12 Andrew R. Runnalls, subject to such other
 *CXXR copyrights and copyright restrictions as may be stated below.
 *CXXR
 *CXXR CXXR is not part of the R project, and bugs and other issues should
 *CXXR not be reported via r-bugs or other R project channels; instead refer
 *CXXR to the CXXR website.
 *CXXR */

/*
 *  Mathlib : A C Library of Special Functions
 *  Copyright (C) 1999-2007  The R Core Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, a copy is available at
 *  http://www.r-project.org/Licenses/
 *
 *  SYNOPSIS
 *
 *    #include <Rmath.h>
 *    double dsignrank(double x, double n, int give_log)
 *    double psignrank(double x, double n, int lower_tail, int log_p)
 *    double qsignrank(double x, double n, int lower_tail, int log_p)
 *    double rsignrank(double n)
 *
 *  DESCRIPTION
 *
 *    dsignrank	   The density of the Wilcoxon Signed Rank distribution.
 *    psignrank	   The distribution function of the Wilcoxon Signed Rank
 *		   distribution.
 *    qsignrank	   The quantile function of the Wilcoxon Signed Rank
 *		   distribution.
 *    rsignrank	   Random variates from the Wilcoxon Signed Rank
 *		   distribution.
 */

//#include "nmath.h"

static double *w;
static int allocated_n;

static void
w_free(void)
{
    if (!w) return;

    free((void *) w);
    w = 0;
    allocated_n = 0;
}

void signrank_free(void)
{
    w_free();
}

static void
w_init_maybe(int n)
{
    int u, c;

    u = n * (n + 1) / 2;
    c = (u / 2);

    if (w) {
        if(n != allocated_n) {
        w_free();
    }
    else return;
    }

    if(!w) {
    w = (double *) calloc((size_t) c + 1, sizeof(double));

    allocated_n = n;
    }
}

static double
csignrank(int k, int n)
{
    int c, u, j;

    u = n * (n + 1) / 2;
    c = (u / 2);

    if (k < 0 || k > u)
    return 0;
    if (k > c)
    k = u - k;

    if (n == 1)
        return 1.;
    if (w[0] == 1.)
        return w[k];

    w[0] = w[1] = 1.;
    for(j=2; j < n+1; ++j) {
        int i, end = imin2(j*(j+1)/2, c);
    for(i=end; i >= j; --i) {
        w[i] += w[i-j];
    }
    }

    return w[k];
}


double psignrank(double x, double n, int lower_tail, int log_p)
{
    int i;
    double f, p;

    n = floor(n + 0.5);

    x = floor(x + 1e-7);

    w_init_maybe(n);
    f = exp(- n * M_LN2);
    p = 0;
    if (x <= (n * (n + 1) / 4)) {
    for (i = 0; i <= x; i++)
        p += csignrank(i, n) * f;
    }
    else {
    x = n * (n + 1) / 2 - x;
    for (i = 0; i < x; i++)
        p += csignrank(i, n) * f;
    lower_tail = !lower_tail; /* p = 1 - p; */
    }

    return(R_DT_val(p));
} /* psignrank() */

void pnorm_both(double x, double *cum, double *ccum, int i_tail, int log_p);

double pnorm5(double x, double mu, double sigma, int lower_tail, int log_p)
{
    double p, cp;

    /* Note: The structure of these checks has been carefully thought through.
     * For example, if x == mu and sigma == 0, we get the correct answer 1.
     */
    p = (x - mu) / sigma;

    x = p;

    pnorm_both(x, &p, &cp, (lower_tail ? 0 : 1), log_p);

    return(lower_tail ? p : cp);
}

#define SIXTEN	16 /* Cutoff allowing exact "*" and "/" */

void pnorm_both(double x, double *cum, double *ccum, int i_tail, int log_p)
{
/* i_tail in {0,1,2} means: "lower", "upper", or "both" :
   if(lower) return  *cum := P[X <= x]
   if(upper) return *ccum := P[X >  x] = 1 - P[X <= x]
*/
    const static double a[5] = {
    2.2352520354606839287,
    161.02823106855587881,
    1067.6894854603709582,
    18154.981253343561249,
    0.065682337918207449113
    };
    const static double b[4] = {
    47.20258190468824187,
    976.09855173777669322,
    10260.932208618978205,
    45507.789335026729956
    };
    const static double c[9] = {
    0.39894151208813466764,
    8.8831497943883759412,
    93.506656132177855979,
    597.27027639480026226,
    2494.5375852903726711,
    6848.1904505362823326,
    11602.651437647350124,
    9842.7148383839780218,
    1.0765576773720192317e-8
    };
    const static double d[8] = {
    22.266688044328115691,
    235.38790178262499861,
    1519.377599407554805,
    6485.558298266760755,
    18615.571640885098091,
    34900.952721145977266,
    38912.003286093271411,
    19685.429676859990727
    };
    const static double p[6] = {
    0.21589853405795699,
    0.1274011611602473639,
    0.022235277870649807,
    0.001421619193227893466,
    2.9112874951168792e-5,
    0.02307344176494017303
    };
    const static double q[5] = {
    1.28426009614491121,
    0.468238212480865118,
    0.0659881378689285515,
    0.00378239633202758244,
    7.29751555083966205e-5
    };

    double xden, xnum, temp, del, eps, xsq, y;
#ifdef NO_DENORMS
    double min = DBL_MIN;
#endif
    int i, lower, upper;

#ifdef IEEE_754
    if(ISNAN(x)) { *cum = *ccum = x; return; }
#endif

    /* Consider changing these : */
    eps = DBL_EPSILON * 0.5;

    /* i_tail in {0,1,2} =^= {lower, upper, both} */
    lower = i_tail != 1;
    upper = i_tail != 0;

    y = fabs(x);
    if (y <= 0.67448975) { /* qnorm(3/4) = .6744.... -- earlier had 0.66291 */
    if (y > eps) {
        xsq = x * x;
        xnum = a[4] * xsq;
        xden = xsq;
        for (i = 0; i < 3; ++i) {
        xnum = (xnum + a[i]) * xsq;
        xden = (xden + b[i]) * xsq;
        }
    } else xnum = xden = 0.0;

    temp = x * (xnum + a[3]) / (xden + b[3]);
    if(lower)  *cum = 0.5 + temp;
    if(upper) *ccum = 0.5 - temp;
    if(log_p) {
        if(lower)  *cum = log(*cum);
        if(upper) *ccum = log(*ccum);
    }
    }
    else if (y <= M_SQRT_32) {

    /* Evaluate pnorm for 0.674.. = qnorm(3/4) < |x| <= sqrt(32) ~= 5.657 */

    xnum = c[8] * y;
    xden = y;
    for (i = 0; i < 7; ++i) {
        xnum = (xnum + c[i]) * y;
        xden = (xden + d[i]) * y;
    }
    temp = (xnum + c[7]) / (xden + d[7]);

#define do_del(X)							\
    xsq = trunc(X * SIXTEN) / SIXTEN;				\
    del = (X - xsq) * (X + xsq);					\
    if(log_p) {							\
        *cum = (-xsq * xsq * 0.5) + (-del * 0.5) + log(temp);	\
        if((lower && x > 0.) || (upper && x <= 0.))			\
          *ccum = log1p(-exp(-xsq * xsq * 0.5) *		\
                exp(-del * 0.5) * temp);		\
    }								\
    else {								\
        *cum = exp(-xsq * xsq * 0.5) * exp(-del * 0.5) * temp;	\
        *ccum = 1.0 - *cum;						\
    }

#define swap_tail						\
    if (x > 0.) {/* swap  ccum <--> cum */			\
        temp = *cum; if(lower) *cum = *ccum; *ccum = temp;	\
    }

    do_del(y);
    swap_tail;
    }

/* else	  |x| > sqrt(32) = 5.657 :
 * the next two case differentiations were really for lower=T, log=F
 * Particularly	 *not*	for  log_p !

 * Cody had (-37.5193 < x  &&  x < 8.2924) ; R originally had y < 50
 *
 * Note that we do want symmetry(0), lower/upper -> hence use y
 */
    else if((log_p && y < 1e170) /* avoid underflow below */
    /*  ^^^^^ MM FIXME: can speedup for log_p and much larger |x| !
     * Then, make use of  Abramowitz & Stegun, 26.2.13, something like

     xsq = x*x;

     if(xsq * DBL_EPSILON < 1.)
        del = (1. - (1. - 5./(xsq+6.)) / (xsq+4.)) / (xsq+2.);
     else
        del = 0.;
     *cum = -.5*xsq - M_LN_SQRT_2PI - log(x) + log1p(-del);
     *ccum = log1p(-exp(*cum)); /.* ~ log(1) = 0 *./

     swap_tail;

     [Yes, but xsq might be infinite.]

    */
        || (lower && -37.5193 < x  &&  x < 8.2924)
        || (upper && -8.2924  < x  &&  x < 37.5193)
    ) {

    /* Evaluate pnorm for x in (-37.5, -5.657) union (5.657, 37.5) */
    xsq = 1.0 / (x * x); /* (1./x)*(1./x) might be better */
    xnum = p[5] * xsq;
    xden = xsq;
    for (i = 0; i < 4; ++i) {
        xnum = (xnum + p[i]) * xsq;
        xden = (xden + q[i]) * xsq;
    }
    temp = xsq * (xnum + p[4]) / (xden + q[4]);
    temp = (M_1_SQRT_2PI - temp) / y;

    do_del(x);
    swap_tail;
    } else { /* large x such that probs are 0 or 1 */
    if(x > 0) {	*cum = R_D__1; *ccum = R_D__0;	}
    else {	        *cum = R_D__0; *ccum = R_D__1;	}
    }


#ifdef NO_DENORMS
    /* do not return "denormalized" -- we do in R */
    if(log_p) {
    if(*cum > -min)	 *cum = -0.;
    if(*ccum > -min)*ccum = -0.;
    }
    else {
    if(*cum < min)	 *cum = 0.;
    if(*ccum < min)	*ccum = 0.;
    }
#endif
    return;
}

#endif // WILCOX_FUNCS

