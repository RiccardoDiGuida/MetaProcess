#ifndef WILCOX_FUNC_2
#define WILCOX_FUNC_2

#include "npq.h"

#define WILCOX_MAX 50

static double ***w_1; /* to store  cwilcox(i,j,k) -> w[i][j][k] */
static int allocated_m, allocated_n_1;

unsigned long long
choose(unsigned long long n, unsigned long long k) {
    if (k > n) {
        return 0;
    }
    unsigned long long r = 1;
    for (unsigned long long d = 1; d <= k; ++d) {
        r *= n--;
        r /= d;
    }
    return r;
}

static void
w_free(int m, int n)
{
    int i, j;

    for (i = m; i >= 0; i--) {
    for (j = n; j >= 0; j--) {
        if (w_1[i][j] != 0)
        free((void *) w_1[i][j]);
    }
    free((void *) w_1[i]);
    }
    free((void *) w_1);
    w_1 = 0; allocated_m = allocated_n_1 = 0;
}

static void
w_init_maybe(int m, int n)
{
    int i;

    if (m > n) {
    i = n; n = m; m = i;
    }
    if (w_1 && (m > allocated_m || n > allocated_n_1))
    w_free(allocated_m, allocated_n_1); /* zeroes w */

    if (!w_1) { /* initialize w[][] */
    m = imax2(m, WILCOX_MAX);
    n = imax2(n, WILCOX_MAX);
    w_1 = (double ***) calloc((size_t) m + 1, sizeof(double **));

    for (i = 0; i <= m; i++) {
        w_1[i] = (double **) calloc((size_t) n + 1, sizeof(double *));
    }
    allocated_m = m; allocated_n_1 = n;
    }
}

static void
w_free_maybe(int m, int n)
{
    if (m > WILCOX_MAX || n > WILCOX_MAX)
    w_free(m, n);
}


/* This counts the number of choices with statistic = k */
static double
cwilcox(int k, int m, int n)
{
    int c, u, i, j, l;

    u = m * n;
    if (k < 0 || k > u)
    return(0);
    c = (int)(u / 2);
    if (k > c)
    k = u - k; /* hence  k <= floor(u / 2) */
    if (m < n) {
    i = m; j = n;
    } else {
    i = n; j = m;
    } /* hence  i <= j */

    if (j == 0) /* and hence i == 0 */
    return (k == 0);


    /* We can simplify things if k is small.  Consider the Mann-Whitney
       definition, and sort y.  Then if the statistic is k, no more
       than k of the y's can be <= any x[i], and since they are sorted
       these can only be in the first k.  So the count is the same as
       if there were just k y's.
    */
    if (j > 0 && k < j) return cwilcox(k, i, k);

    if (w_1[i][j] == 0) {
    w_1[i][j] = (double *) calloc((size_t) c + 1, sizeof(double));

    for (l = 0; l <= c; l++)
        w_1[i][j][l] = -1;
    }
    if (w_1[i][j][k] < 0) {
    if (j == 0) /* and hence i == 0 */
        w_1[i][j][k] = (k == 0);
    else
        w_1[i][j][k] = cwilcox(k - j, i - 1, j) + cwilcox(k, i, j - 1);

    }
    return(w_1[i][j][k]);
}

/* args have the same meaning as R function pwilcox */
double pwilcox(double q, double m, double n, int lower_tail, int log_p)
{
    int i;
    double c, p;

    m = round(m);
    n = round(n);

    q = floor(q + 1e-7);

    if (q < 0.0)
    return(R_DT_0);
    if (q >= m * n)
    return(R_DT_1);

    int mm = (int) m, nn = (int) n;
    w_init_maybe(mm, nn);
    c = choose(m + n, n);
    p = 0;
    /* Use summation of probs over the shorter range */
    if (q <= (m * n / 2)) {
    for (i = 0; i <= q; i++)
        p += cwilcox(i, mm, nn) / c;
    }
    else {
    q = m * n - q;
    for (i = 0; i < q; i++)
        p += cwilcox(i, mm, nn) / c;
    lower_tail = !lower_tail; /* p = 1 - p; */
    }

    return(R_DT_val(p));
} /* pwilcox */



void wilcox_free(void)
{
    w_free_maybe(allocated_m, allocated_n_1);
}
#endif // WILCOX_FUNC_2

