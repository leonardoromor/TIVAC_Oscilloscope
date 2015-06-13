#include <stdio.h>
#include <string.h>
#include <libusb-1.0/libusb.h>
#include <stdlib.h>
#include <unistd.h>

#define BOARD_VENDOR_ID   0x1cbe
#define BOARD_PRODUCT_ID  0x0003

#define ENDPOINT_IN 0x81
#define ENDPOINT_OUT 0x01


int main(int argc, char **argv){

    libusb_device * dev = NULL;
    libusb_context *context = NULL;
    libusb_device **list = NULL;
    struct libusb_device_descriptor desc;
    struct libusb_device **devs;
    struct libusb_device_handle *handle = NULL;
    unsigned char *str1, *str2;
    int found;
    int i,e,j;

    int rc = 0;
    ssize_t count = 0;

    str1 = malloc(sizeof(unsigned char) * 64);
    str2 = malloc(sizeof(unsigned char) * 64);
    //Init LIBUSB
    rc = libusb_init(&context);
    if(rc < 0)
    {
        printf("\nfailed to initialise libusb\n");
        return 1;
    }
    else
        printf("\nInit Successful!\n");


    //SET DEBUG LEVEL TO 0
    //libusb_set_debug(context, 0);


    //GET DEVICE LIST
    count = libusb_get_device_list(context, &devs);
    if (count < 0)
    {
        printf("\nThere are no USB devices on bus\n");
        return -1;
    }
    printf("\nDevice Count : %d\n-------------------------------\n",count);

    while ((dev = devs[i++]) != NULL)
    {
        rc = libusb_get_device_descriptor(dev, &desc);
        if (rc < 0)
            {
            printf("failed to get device descriptor\n");
            libusb_free_device_list(devs,1);
            libusb_close(handle);
            break;
        }

        if(desc.idVendor == BOARD_VENDOR_ID && desc.idProduct == BOARD_PRODUCT_ID)
        {
            rc = libusb_open(dev,&handle);
            if (rc < 0)
            {
                printf("error opening device\n");
                libusb_free_device_list(devs,1);
                libusb_close(handle);
                break;
            }
            found = 1;
            printf("****************DEVICE FOUND*****************\n");

            libusb_free_device_list(devs, 1);
            if(libusb_kernel_driver_active(handle, 0) == 1)
            {
                printf("\nKernel Driver Active");
                if(libusb_detach_kernel_driver(handle, 0) == 0)
                    printf("\nKernel Driver Detached!");
                else
                {
                    printf("\nCouldn't detach kernel driver!\n");
                    libusb_free_device_list(devs,1);
                    libusb_close(handle);
                    return -1;
                }
            }


            e = libusb_claim_interface(handle, 0);
            if(e < 0)
            {
                printf("\nCannot Claim Interface");
                libusb_free_device_list(devs,1);
                libusb_close(handle);
                return -1;
            }
            else
                printf("\nClaimed Interface\n");


            //COMMUNICATE


            char *my_string, *my_string1;
            int transferred = 0;
            int received = 0;
            int length = 4;

            my_string = (char *)malloc(sizeof(char)* 4);
            my_string1 = (char *)malloc(sizeof(char)*4);

            memset(my_string,'\0',64);
            memset(my_string1,'\0',64);

            strcpy(my_string,"CIAO");
            length = strlen(my_string);

            printf("\nTo be sent : %s\n",my_string);



            for(j = 0; j < length; j++)
            {
                printf("INDEX:%d\n",j);

                e = libusb_bulk_transfer(handle,ENDPOINT_OUT,my_string+j,64,&transferred,0);
                if(e == 0 && transferred == 64)
                {
                    printf("\nWrite successful!");
                    printf("\nSent %d bytes with string: %c\n", transferred, my_string[j]);
                }
                else
                    printf("\nError in write! e = %d and transferred = %d\n",e,transferred);


                e = libusb_bulk_transfer(handle,ENDPOINT_IN,my_string1,64,&transferred,0);
                if(e == 0)
                {
                    printf("\nReceived: ");
                    printf("%c",my_string1[j]);    //will read a string from lcp2148
                    printf("\n");
                }
                else
                {
                    printf("\nError in read! e = %d and received = %d\n",e,received);
                    return -1;
                }
            }


            e = libusb_release_interface(handle, 0);

            libusb_close(handle);
            libusb_exit(NULL);

            break;
        }




    }//end of while



    return 0;
}
