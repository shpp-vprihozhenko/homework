#include "gbufferedimage.h"

// here is defined constant parameters
const int colorWhite=0xffffff;
const int DEPTH=4000;
const int imgWidth=800;
const int imgHeight=600;

int main()
{
    GWindow gw;
    gw.setCanvasSize(imgWidth,imgHeight);

    GBufferedImage *image = new GBufferedImage(imgWidth, imgHeight,0);
    gw.add(image);

    float zi, zr, ci, cr, tmp;
    int x, y, k, m;

    double Xmax, Xmin, Ymax, Ymin,Yshift;
    Xmax=imgWidth/2;   Xmin=-Xmax;
    Ymax=imgHeight/2;  Ymin=-Ymax;
    Yshift=imgHeight/3.75;

    for(x = Xmin; x < Xmax; x++) {
        ci = ((float)x) / (imgWidth/2) * 1.2;
        for(y = Ymin; y < Ymax; y++) {
            cr = ((float)y-Yshift) / (imgHeight/2) * 1.34;
            zi = zr = 0;
            for(k=0;k<DEPTH;k++){
                tmp = zr*zr - zi*zi;
                zi = 2*zr*zi + ci;
                zr = tmp + cr;
                if (zr*zr + zi*zi > 1.0E16)
                    break;
            }
            if (k < DEPTH) {
                int colorShift=(k%8 + 1)*0x0f0000;
                image->setRGB(x+(imgWidth/2),y+(imgHeight/2),(colorWhite-colorShift));
            }
        }
    }
    return 0;
}
