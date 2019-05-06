#ifndef GIANT_TIME
#define GIANT_TIME

typedef struct  Time  {
                int           yr;     /* year         */
                int           mo;     /* month        */
                int           day;    /* day          */
                int           hr;     /* hour         */
                int           mn;     /* minute       */
                float         sec;    /* second       */
                int           base;   /* GMT, DST,etc.*/
} TIME;

#ifdef __cplusplus
extern "C" {
#endif

TIME do2date(double);
double base_diff(TIME);
double time_diff(TIME, TIME);

#ifdef __cplusplus
}
#endif

#endif
