#include <iostream>
#include "gbufferedimage.h"
using namespace std;

// check for proportion balans
bool proportionCheking(auto bwModel, int ySize, int xSize){
    // count waist of object
    int y=ySize/2, firstX=-1, lastX=-1;
    for(int x=0;x<xSize;x++){
        if(bwModel[y][x]=='1'){
            if(firstX==-1){
                firstX=x;
                lastX=x;
                continue;
            }
            if(lastX<x)
                lastX=x;
        }
    }
    float dx=lastX-firstX+1;
    if((dx/ySize)>0.75) // count proportion
        return false;  // too waiste...
    return true;
}

// count mass centers of object
int countMassCenters(auto bwModel, int ySize, int xSize){
    double cm[xSize];
    for(int x=0;x<xSize;x++){
        cm[x]=0;
        for(int y=0; y<(ySize*0.75); y++){
            //sumPoLinii(y,x)
            double tmpSum=0;
            for(int z=0;z<xSize;z++){
                if(bwModel[y][z]=='1'){
                    double zx;
                    if(z==x)
                        zx=1;
                    else
                        zx=1/((z-x));
                    if(zx<0)
                        zx=-zx;
                    tmpSum+=zx;
                }
            }
            cm[x]+=tmpSum;
        }
    }
    // count first derivative
    double dcm[xSize]; int dx=xSize/14+2;
    for(int x=0;x<(xSize-dx);x++){
        double y1=cm[x];
        double y2=cm[x+dx];
        dcm[x]=y2-y1;
    }
    // look for silhouettes
    double sum=dcm[0], oldSum=sum;
    int quantityRandomPoints=15;
    int counterOfAnotherDirection=0, lastDirection=0;
    int sil=0;
    for(int x=1;x<xSize-dx;x++){
        sum+=dcm[x];
        if(sum>oldSum){
            if(lastDirection==0)
                lastDirection=1;
            else if(lastDirection==-1){
                counterOfAnotherDirection++;
                if(counterOfAnotherDirection==quantityRandomPoints){
                    counterOfAnotherDirection=0;
                    lastDirection=1;
                }
            }
        } else {
            if(lastDirection==1)
                counterOfAnotherDirection++;
            if(counterOfAnotherDirection==quantityRandomPoints){
                counterOfAnotherDirection=0;
                lastDirection=-1;
                sil++;
            }
        }
        oldSum=sum;
    }
    if(sil==0)
        sil=1; // any object has at less one mass-center.
    return sil;
}

// count legs of object
int countObjLegs(auto bwModel, int ySize, int xSize){
    // look for legs at 5%, 10%, 15%, 20%, 25% from bottom of the object
    int maxLegs=0;
    for(int i=1;i<=5;i++){
        int y=ySize*(1-(double)i*5/100);
        int legs=0;
        int x=0;
        while(x<xSize){
            if(bwModel[y][x]=='1'){
                int startX=x;
                while((x<xSize)&&(bwModel[y][x]=='1'))
                    x++;
                int legWide=x-startX;
                if(legWide>1) // to narrow! (less 1 px) do not count
                    legs++;
            }
            x++;
        }
        if(maxLegs<legs)
            maxLegs=legs;
    }

    int mc=countMassCenters(bwModel,ySize,xSize);
    cout << "Silhouettes by mass centers: " << mc << endl;
    cout << "legs: " << maxLegs << endl;
    
    return maxLegs;
}
