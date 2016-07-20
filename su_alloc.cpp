/* THis is a source program to alloc arrays */
/*
  Copyright (C) 2013 Geophysical Insitute of Sinopec,Nanjing,China
   
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

#include <stdlib.h>
#include "su_alloc.h"

static void *alloc1 (size_t n1, size_t size);
static void free1 (void *p);
static void **alloc2 (size_t n1, size_t n2, size_t size);
static void free2 (void **p);
static void ***alloc3 (size_t n1, size_t n2, size_t n3, size_t size);
static void free3 (void ***p);

/* Luo Heng Defined Arrays */
long long *alloc1longlong( size_t n1 )
{
	return ( long long * )alloc1( n1 , sizeof( long long ) );
}

void free1longlong( long long * p )
{
	free1( p );
}

long long **alloc2longlong(size_t n1, size_t n2)
{
	return (long long**)alloc2(n1,n2,sizeof( long long ));
}

void free2longlong(long long **p)
{
        free2((void**)p);
}

long long ***alloc3longlong(size_t n1, size_t n2, size_t n3)
{
        return (long long***)alloc3(n1,n2,n3,sizeof(long long));
}

void free3longlong(long long ***p)
{
        free3((void***)p);
}


int *alloc1int(size_t n1)
/*< allocate a 1-d array of ints >*/
{
        return (int*)alloc1(n1,sizeof(int));
}

void free1int(int *p)
/*< free a 1-d array of ints >*/
{
        free1(p);
}

int **alloc2int(size_t n1, size_t n2)
/*< allocate a 2-d array of ints >*/
{
        return (int**)alloc2(n1,n2,sizeof(int));
}

void free2int(int **p)
/*< free a 2-d array of ints >*/
{
        free2((void**)p);
}

int ***alloc3int(size_t n1, size_t n2, size_t n3)
/*< allocate a 3-d array of ints >*/
{
        return (int***)alloc3(n1,n2,n3,sizeof(int));
}

void free3int(int ***p)
/*< free a 3-d array of ints >*/
{
        free3((void***)p);
}

float *alloc1float(size_t n1)
/*< allocate a 1-d array of floats >*/
{
        return (float*)alloc1(n1,sizeof(float));
}

void free1float(float *p)
/*< free a 1-d array of floats >*/
{
        free1(p);
}

float **alloc2float(size_t n1, size_t n2)
/*< allocate a 2-d array of floats >*/
{
        return (float**)alloc2(n1,n2,sizeof(float));
}

void free2float(float **p)
/*< free a 2-d array of floats >*/
{
        free2((void**)p);
}

float ***alloc3float(size_t n1, size_t n2, size_t n3)
/*< allocate a 3-d array of floats >*/
{
        return (float***)alloc3(n1,n2,n3,sizeof(float));
}

void free3float(float ***p)
/*< free a 3-d array of floats >*/
{
        free3((void***)p);
}

double *alloc1double(size_t n1)
/*< allocate a 1-d array of doubles >*/
{
        return (double*)alloc1(n1,sizeof(double));
}

void free1double(double *p)
/*< free a 1-d array of doubles >*/
{
        free1(p);
}

double **alloc2double(size_t n1, size_t n2)
/*< allocate a 2-d array of doubles >*/
{
        return (double**)alloc2(n1,n2,sizeof(double));
}

void free2double(double **p)
/*< free a 2-d array of doubles >*/
{
        free2((void**)p);
}

double ***alloc3double(size_t n1, size_t n2, size_t n3)
/*< allocate a 3-d array of doubles >*/
{
        return (double***)alloc3(n1,n2,n3,sizeof(double));
}

void free3double(double ***p)
/*< free a 3-d array of doubles >*/
{
        free3((void***)p);
}

void zero1int(int *p, size_t n1)
/*< set the arrays to zero >*/
{
     int i;
     for(i=0;i<n1;i++) p[i]=0;
}

void zero2int(int **p, size_t n1, size_t n2)
/*< set the arrays to zero >*/
{
     int i, j;
     for(i=0;i<n2;i++)
       for(j=0;j<n1;j++)
         p[i][j]=0;
}

void zero3int(int ***p, size_t n1, size_t n2, size_t n3)
/*< set the arrays to zero >*/
{
     int i, j, k;
     for(i=0;i<n3;i++)
       for(j=0;j<n2;j++)
          for(k=0;k<n1;k++)
            p[i][j][k]=0;
}

void zero1float(float *p, size_t n1)
/*< set the arrays to zero >*/
{
     int i;
     for(i=0;i<n1;i++) p[i]=0.0;
}

void zero2float(float **p, size_t n1, size_t n2)
/*< set the arrays to zero >*/
{
     int i, j;
     for(i=0;i<n2;i++)
       for(j=0;j<n1;j++)
         p[i][j]=0.0;
}

void zero3float(float ***p, size_t n1, size_t n2, size_t n3)
/*< set the arrays to zero >*/
{
     int i, j, k;
     for(i=0;i<n3;i++)
       for(j=0;j<n2;j++)
          for(k=0;k<n1;k++)
            p[i][j][k]=0.0;
}

void zero1double(double *p, size_t n1)
/*< set the arrays to zero >*/
{
     int i;
     for(i=0;i<n1;i++) p[i]=0.0;
}

void zero2double(double **p, size_t n1, size_t n2)
/*< set the arrays to zero >*/
{
     int i, j;
     for(i=0;i<n2;i++)
       for(j=0;j<n1;j++)
         p[i][j]=0.0;
}

void zero3double(double ***p, size_t n1, size_t n2, size_t n3)
/*< set the arrays to zero >*/
{
     int i, j, k;
     for(i=0;i<n3;i++)
       for(j=0;j<n2;j++)
          for(k=0;k<n1;k++)
            p[i][j][k]=0.0;
}




static void *alloc1 (size_t n1, size_t size)
/* initialize */
{
        void *p;

        if ((p=malloc(n1*size))==NULL)
                return NULL;
        return p;
}

static void free1 (void *p)
/* free a 1-d array */
{
        free(p);
}

static void **alloc2 (size_t n1, size_t n2, size_t size)
/* allocate a 2-d array */
{
        size_t i2;
        void **p;

        if ((p=(void**)malloc(n2*sizeof(void*)))==NULL)
                return NULL;
        if ((p[0]=(void*)malloc(n2*n1*size))==NULL) {
                free(p);
                return NULL;
        }
        for (i2=0; i2<n2; i2++)
                p[i2] = (char*)p[0]+size*n1*i2;
        return p;
}

static void free2 (void **p)
/* free a 2-d array */
{
        free(p[0]);
        free(p);
}

static void ***alloc3 (size_t n1, size_t n2, size_t n3, size_t size)
/* allocate a 3-d array */
{
        size_t i3,i2;
        void ***p;

        if ((p=(void***)malloc(n3*sizeof(void**)))==NULL)
                return NULL;
        if ((p[0]=(void**)malloc(n3*n2*sizeof(void*)))==NULL) {
                free(p);
                return NULL;
        }
        if ((p[0][0]=(void*)malloc(n3*n2*n1*size))==NULL) {
                free(p[0]);
                free(p);
                return NULL;
        }
        for (i3=0; i3<n3; i3++) {
                p[i3] = p[0]+n2*i3;
                for (i2=0; i2<n2; i2++)
                        p[i3][i2] = (char*)p[0][0]+size*n1*(i2+n2*i3);
        }
        return p;
}

static void free3 (void ***p)
/* free a 3-d array */
{
        free(p[0][0]);
        free(p[0]);
        free(p);
}

void zero4float(float ****p, size_t n1, size_t n2, size_t n3, size_t n4)
/*< Zeror the 4d float array >*/
{
     int m, i, j, k;
     for(m=0;m<n4;m++)
       for(i=0;i<n3;i++)
          for(j=0;j<n2;j++)
            for(k=0;k<n1;k++)
               p[m][i][j][k]=0.0;
}

void zero5float(float *****p, size_t n1, size_t n2, size_t n3, size_t n4 , size_t n5 )
/*< Zeror the 5d float array >*/
{
     int m, i, j, k, z;
    for( z=0 ; z<n5 ; z++ )
     for(m=0;m<n4;m++)
       for(i=0;i<n3;i++)
          for(j=0;j<n2;j++)
            for(k=0;k<n1;k++)
               p[z][m][i][j][k]=0.0;
}

