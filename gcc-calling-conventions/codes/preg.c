/* IEE240 - Organizacion y Arquitectura de Computadoras */
/* C-wrapper for assembly functions */

#include <stdio.h>
#include <stdlib.h>
#include "readimglib.h"
void convertirAFloat(unsigned char *A,unsigned char *B,unsigned char *C,float *Aasm,float *Basm,float *Casm,int imgSize);
void effectASM_SIMD(unsigned char *A,unsigned char *B,unsigned char *CasmS,float alpha,int imgSize);
void effectC(unsigned char *A, unsigned char *B, unsigned char *Cc, float alpha, int imgSize);
extern void effectASM(unsigned char *A, unsigned char *B, unsigned char *Cc, float alpha, int imgSize);
extern void effectASMSIMD(float *A,float *B,float *C,double alpha, int imgSize);
int main (int argc, char **argv){ 

    char filenameA[] = "../data/baboon.pgm";
    char filenameB[] = "../data/lena.pgm";

    
    int Nrows, Ncols, imgSize,cont=10,cont2=10;
    char *A, *B, *Cc, *Casm, *CasmS;   
    float alpha = atof(argv[1]);
    struct timespec time1, time2; 

    read_headppm(filenameA, &Nrows, &Ncols,"P5");

    A = (unsigned char*) malloc(Nrows * Ncols * sizeof(unsigned char));
    B = (unsigned char*) malloc(Nrows * Ncols * sizeof(unsigned char));
    Cc = (unsigned char*) malloc(Nrows * Ncols * sizeof(unsigned char));
    Casm = (unsigned char*) malloc(Nrows * Ncols * sizeof(unsigned char));
    CasmS = (unsigned char*) malloc(Nrows * Ncols * sizeof(unsigned char));
    
    read_ppm2uchar(filenameA, A, Nrows*Ncols, "P5");
    read_ppm2uchar(filenameB, B, Nrows*Ncols, "P5");

    imgSize = Nrows * Ncols;
 
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);  
    effectC(A,B,Cc,alpha,imgSize);  
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);   
    double tC = (double)((time2.tv_sec-time1.tv_sec) + (time2.tv_nsec-time1.tv_nsec)/1e9); 

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
    effectASM(A,B,Casm,alpha,imgSize);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2); 
    double tASM = (double)((time2.tv_sec-time1.tv_sec) + (time2.tv_nsec-time1.tv_nsec)/1e9);
    
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
    effectASM_SIMD(A,B,CasmS,alpha,imgSize);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2); 
    double tASMS = (double)((time2.tv_sec-time1.tv_sec) + (time2.tv_nsec-time1.tv_nsec)/1e9);
    
    printf("El tiempo de ejecución en C es: %.5f ms\n", tC*1e3);
    printf("El tiempo de ejecución en ASM es: %.5f ms\n", tASM*1e3);
    printf("El tiempo de ejecución en ASM con SIMD es: %.5f ms\n", tASMS*1e3);
    printf("El speed-up de C/ASM es: %.2f \n", tC/tASM);

    write_uchar2ppm("outputImgC.pgm", Cc, Nrows, Ncols, "P5"); 
    write_uchar2ppm("outputImgASM.pgm", Casm, Nrows, Ncols, "P5");
    write_uchar2ppm("outputImgASM_SIMD.pgm", CasmS, Nrows, Ncols, "P5");

    free(A);
    free(B);
    free(Cc);
    free(Casm);
    free(CasmS);
    return 0;
}

void effectC(unsigned char *A, unsigned char *B, unsigned char *Cc, float alpha, int imgSize){
    unsigned char diff = 0;
    float aux = 0.0;
    for(int i=0; i<imgSize; i++){
        diff = B[i]-A[i];
        aux = (float)B[i] + alpha*((float)diff);
        Cc[i] = (unsigned char) (aux);
    }
}

void effectASM_SIMD(unsigned char *A, unsigned char *B, unsigned char *C, float alpha, int imgSize){
    float Aasm[imgSize],Basm[imgSize],Casm[imgSize];
    convertirAFloat(A,B,C,Aasm,Basm,Casm,imgSize);
    effectASMSIMD(Aasm,Basm,Casm,alpha,imgSize);
    for(int i=0;i<imgSize; i++){
        C[i]=(unsigned char)Casm[i];
    }
}
void convertirAFloat(unsigned char *A,unsigned char *B,unsigned char *C,float *Aasm,float *Basm,float *Casm,int imgSize){
    for(int i=0;i<imgSize; i++){
        Aasm[i]=(float)A[i];
        Basm[i]=(float)B[i];
        Casm[i]=(float)C[i];
    }
    posix_memalign((void**)&Aasm,16,imgSize*sizeof(float));
    posix_memalign((void**)&Basm,16,imgSize*sizeof(float));
    posix_memalign((void**)&Casm,16,imgSize*sizeof(float));
}