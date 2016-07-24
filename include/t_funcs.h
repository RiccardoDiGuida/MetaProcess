#ifndef T_FUNCS
#define T_FUNCS


double gammln(const double xx) {
    //Returns the value lnŒ􏰒.xx/􏰔 for xx > 0.
    int j;
    double x,tmp,y,ser;
    static const double cof[14]={57.1562356658629235,-59.5979603554754912, 14.1360979747417471,-0.491913816097620199,.339946499848118887e-4, .465236289270485756e-4,-.983744753048795646e-4,.158088703224912494e-3, -.210264441724104883e-3,.217439618115212643e-3,-.164318106536763890e-3, .844182239838527433e-4,-.261908384015814087e-4,.368991826595316234e-5};
    if (xx <= 0) throw("bad arg in gammln");
    y=x=xx;
    tmp = x+5.24218750000000000; //Rational 671/128.
    tmp = (x+0.5)*log(tmp)-tmp;
    ser = 0.999999999999997092;
    for (j=0;j<14;j++) ser += cof[j]/++y;
    return tmp+log(2.5066282746310005*ser/x);
}

struct Gauleg18 {
//Abscissas and weights for Gauss-Legendre quadrature.
    static const int ngau = 18;
    static const double y[18];
    static const double w[18];
};
const double Gauleg18::y[18] = {0.0021695375159141994, 0.011413521097787704,0.027972308950302116,0.051727015600492421, 0.082502225484340941, 0.12007019910960293,0.16415283300752470, 0.21442376986779355, 0.27051082840644336, 0.33199876341447887, 0.39843234186401943, 0.46931971407375483, 0.54413605556657973, 0.62232745288031077, 0.70331500465597174, 0.78649910768313447, 0.87126389619061517, 0.95698180152629142};
const double Gauleg18::w[18] = {0.0055657196642445571, 0.012915947284065419,0.020181515297735382,0.027298621498568734, 0.034213810770299537,0.040875750923643261,0.047235083490265582, 0.053244713977759692,0.058860144245324798,0.064039797355015485,0.068745323835736408,0.072941885005653087,0.076598410645870640,0.079687828912071670,0.082187266704339706,0.084078218979661945,0.085346685739338721,0.085983275670394821};

struct Beta : Gauleg18 {
    //Object for incomplete beta function. Gauleg18 provides coefficients for Gauss-Legendre quadra- ture.
    static const int SWITCH=3000; //When to switch to quadrature method.
    static const double EPS, FPMIN; //See end of struct for initializations.
    double betai(const double a, const double b, const double x) {
        //Returns incomplete beta function Ix.a;b/ for positive a and b, and x between 0 and 1.
        double bt;
        if (a <= 0.0 || b <= 0.0) throw("Bad a or b in routine betai"); if (x < 0.0 || x > 1.0) throw("Bad x in routine betai");
        if (x == 0.0 || x == 1.0) return x;
        if (a > SWITCH && b > SWITCH) return betaiapprox(a,b,x);
        bt=exp(gammln(a+b)-gammln(a)-gammln(b)+a*log(x)+b*log(1.0-x));
        if (x < (a+1.0)/(a+b+2.0)) return bt*betacf(a,b,x)/a;
        else return 1.0-bt*betacf(b,a,1.0-x)/b;
    }

    double betacf(const double a, const double b, const double x) {
    //Evaluates continued fraction for incomplete beta function by modified Lentz’s method (􏰐5.2). User should not call directly.
        int m,m2;
        double aa,c,d,del,h,qab,qam,qap;
        qab=a+b;
        qap=a+1.0;
        qam=a-1.0;
        c=1.0;
        d=1.0-qab*x/qap;
        if (fabs(d) < FPMIN) d=FPMIN;
        d=1.0/d;
        h=d;
        for (m=1;m<10000;m++) {
            m2=2*m;
            aa=m*(b-m)*x/((qam+m2)*(a+m2));
            d=1.0+aa*d;
            if (fabs(d) < FPMIN) d=FPMIN;
            c=1.0+aa/c;
            if (fabs(c) < FPMIN) c=FPMIN;
            d=1.0/d;
            h *= d*c;
            aa = -(a+m)*(qab+m)*x/((a+m2)*(qap+m2));
            d=1.0+aa*d;
            if (fabs(d) < FPMIN) d=FPMIN;
            c=1.0+aa/c;
            if (fabs(c) < FPMIN) c=FPMIN;
            d=1.0/d;
            del=d*c;
            h *= del;
            if (fabs(del-1.0) <= EPS) break;
        }
        return h;
    }

    double betaiapprox(double a, double b, double x) {
        //Incomplete beta by quadrature. Returns Ix.a;b/. User should not call directly.
        int j;
        double xu,t,sum,ans;
        double a1 = a-1.0, b1 = b-1.0, mu = a/(a+b);
        double lnmu=log(mu),lnmuc=log(1.-mu);
        t = sqrt(a*b/(pow(a+b,2)*(a+b+1.0)));
        if (x > a/(a+b)) {              //Set how far to integrate into the tail:
            if (x >= 1.0) return 1.0;
            xu = std::min(1.,std::max(mu + 10.*t, x + 5.0*t));
        } else {
            if (x <= 0.0) return 0.0;
            xu = std::max(0.,std::min(mu - 10.*t, x - 5.0*t));
        }
        sum = 0;
        for (j=0;j<18;j++) {            //Gauss-Legendre.
            t = x + (xu-x)*y[j];
            sum += w[j]*exp(a1*(log(t)-lnmu)+b1*(log(1-t)-lnmuc));
        }
        ans = sum*(xu-x)*exp(a1*lnmu-gammln(a)+b1*lnmuc-gammln(b)+gammln(a+b));
        return ans>0.0? 1.0-ans : -ans;
    }
};

const double Beta::EPS = std::numeric_limits<double>::epsilon();
const double Beta::FPMIN = std::numeric_limits<double>::min()/EPS;

/*void ttest(Vecdouble_I &data1, Vecdouble_I &data2, double &t, double &prob)
//Given the arrays data1[0..n1-1] and data2[0..n2-1], returns Student’s t as t, and its p- value as prob, small values of prob indicating that the arrays have significantly different means. The data arrays are assumed to be drawn from populations with the same true variance.
{
    Beta beta;
    double var1,var2,svar,df,ave1,ave2;
    int n1=data1.size(), n2=data2.size();
    avevar(data1,ave1,var1);
    avevar(data2,ave2,var2);
    df=n1+n2-2;
    svar=((n1-1)*var1+(n2-1)*var2)/df;
    t=(ave1-ave2)/sqrt(svar*(1.0/n1+1.0/n2));
    prob=beta.betai(0.5*df,0.5,df/(df+t*t));
}
*/
#endif // T_FUNCS
