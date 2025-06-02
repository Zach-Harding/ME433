#include <stdio.h>
#include "pico/stdlib.h"
#include "cam.h"

int main()
{
    stdio_init_all();

    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    //printf("Hello, camera!\n");

    init_camera_pins();
 
    while (true) {
        // uncomment these and printImage() when testing with python 
        //char m[10];
        //scanf("%s",m);

        setSaveImage(1);
        while(getSaveImage()==1){}
        convertImage();
        int com = findLine(IMAGESIZEY/2); // calculate the position of the center of the ine
        setPixel(IMAGESIZEY/2,com,0,255,0); // draw the center so you can see it in python
        //printImage();
        int offset = com -  36;
        // make the offset into duty cycles for the motor such that positive turns right and negative turns left
        


        //printf("pixel: %d\r\n",com); // comment this when testing with python
        printf("offset: %d\r\n",offset);
    }
}

