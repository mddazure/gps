#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>                     //Used for UART
#include <fcntl.h>                      //Used for UART
#include <termios.h>            //Used for UART
#include <stdint.h>
#include <string.h>

#include "iothub_client.h"
#include "iothub_message.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/platform.h"
#include "iothubtransportmqtt.h"
#include "serializer.h"
#include "gps.h"

/*String containing Hostname, Device Id & Device Key in the format:                         */
/*  "HostName=<host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"                */
/*  "HostName=<host_name>;DeviceId=<device_id>;SharedAccessSignature=<device_sas_token>"    */
//static const char* connectionString = "HostName=rasppi.azure-devices.net;DeviceId=rasp-pi;SharedAccessKey=diTGPgz3pjjKYqwnTKi+8afq1z/iXJQDe98NET/Q7zQ=";
static const char* connectionString = "HostName=rasppi2.azure-devices.net;DeviceId=rasp-pi;SharedAccessKey=+1iLQjDAHXW1cwMvdjFb6N006Ye/fJIWO/YleFoUc7k=";  


int uart0_filestream = -1;
FILE *fp;
char nmea_id[7];
char rx_buffer[1024];
unsigned char *destination;
size_t *destinationSize;
char *string;
char *temp;

void gps_run(void);

void parse_nmea(char buffer[256]);

void serialize_send(Gp735Type* model);

IOTHUB_CLIENT_HANDLE iotHubClientHandle;

IOTHUB_MESSAGE_HANDLE messageHandle;


void main(void)
{
    

	uart0_filestream = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY | O_NDELAY);          //Open in non blocking read/write mode

	fp = fopen("/dev/ttyAMA0", "r");

	struct termios options;
        tcgetattr(uart0_filestream, &options);
        options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;         //<Set baud rate
        options.c_iflag = IGNPAR;
        options.c_oflag = 0;
        options.c_lflag = 0;
        tcflush(uart0_filestream, TCIFLUSH);
        tcsetattr(uart0_filestream, TCSANOW, &options);

        platform_init();
        serializer_init(NULL);

	gps_run();

        platform_deinit();


    	return;
}

void gps_run(void)
{
	iotHubClientHandle = IoTHubClient_CreateFromConnectionString(connectionString, MQTT_Protocol);

	while(1)
	{

	if(fgets(rx_buffer,1024,fp) != NULL)
		{
			parse_nmea(rx_buffer);
                }
	}

        IoTHubClient_Destroy(iotHubClientHandle);
}

void parse_nmea(char buffer[256])
        {

        Gp735Type* myGps = CREATE_MODEL_INSTANCE(RaspiNamespace, Gp735Type);
        if (myGps == NULL)
        {
                (void)printf("Failed on CREATE_MODEL_INSTANCE\r\n");
        }


                        strncpy(nmea_id,buffer,6);

                        if(strcmp(nmea_id,"$GPGSV")==0)
                                {

                                printf("nmea_id $GPGSV received %s\n",buffer);
                                string = strdup(buffer);
                                myGps->gsv.GSVHeader = strsep(&string,",");
                                printf("myGps->gsv.GSVHeader %s\n",myGps->gsv.GSVHeader);
                                myGps->gsv.NumberOfMessages = atoi(strsep(&string,","));
                                printf("myGps->gsv.NumberOfMessages %i\n",myGps->gsv.NumberOfMessages);
                                myGps->gsv.MessageNumber = atoi(strsep(&string,","));
                                printf("myGps->gsv.MessageNumber %i\n",myGps->gsv.MessageNumber);
                                myGps->gsv.SatsInView = atoi(strsep(&string,","));
                                printf("myGps->gsv.SatsInView %i\n",myGps->gsv.SatsInView);


				myGps->gsv.Sat1ID = atoi(strsep(&string,","));
                                printf("myGps->gsv.Sat1ID %i\n",myGps->gsv.Sat1ID);
                                myGps->gsv.Sat1El = atoi(strsep(&string,","));
                                printf("myGps->gsv.Sat1El %i\n",myGps->gsv.Sat1El);
                                myGps->gsv.Sat1Az = atoi(strsep(&string,","));
                                printf("myGps->gsv.Sat1Az %i\n",myGps->gsv.Sat1Az);
                                myGps->gsv.Sat1SNR = atoi(strsep(&string,","));
                                printf("myGps->gsv.Sat1SNR %i\n",myGps->gsv.Sat1SNR);

                                if (((temp = strsep(&string,",")) == NULL))
				{
	                        	myGps->gsv.Sat2ID = 0;
                                	printf("myGps->gsv.Sat2ID %i\n",myGps->gsv.Sat2ID);
	                                myGps->gsv.Sat2El = 0;
        	                        printf("myGps->gsv.Sat2El %i\n",myGps->gsv.Sat2El);
                	                myGps->gsv.Sat2Az = 0;
                        	        printf("myGps->gsv.Sat2Az %i\n",myGps->gsv.Sat2Az);
                                	myGps->gsv.Sat2SNR = 0;
	                                printf("myGps->gsv.Sat2SNR %i\n",myGps->gsv.Sat2SNR);
        	                        
					myGps->gsv.Sat3ID = 0;
                	                printf("myGps->gsv.Sat3ID %i\n",myGps->gsv.Sat3ID);
                        	        myGps->gsv.Sat3El = 0;
                                	printf("myGps->gsv.Sat3El %i\n",myGps->gsv.Sat3El);
	                                myGps->gsv.Sat3Az = 0;
        	                        printf("myGps->gsv.Sat3Az %i\n",myGps->gsv.Sat3Az);
                	                myGps->gsv.Sat3SNR = 0;
                        	        printf("myGps->gsv.Sat3SNR %i\n",myGps->gsv.Sat3SNR);
                                	
					myGps->gsv.Sat4ID = 0;
	                                printf("myGps->gsv.Sat4ID %i\n",myGps->gsv.Sat4ID);
        	                        myGps->gsv.Sat4El = 0;
                	                printf("myGps->gsv.Sat4El %i\n",myGps->gsv.Sat4El);
                        	        myGps->gsv.Sat4Az = 0;
                                	printf("myGps->gsv.Sat4Az %i\n",myGps->gsv.Sat4Az);
	                                myGps->gsv.Sat4SNR = 0;
        	                        printf("myGps->gsv.Sat4SNR %i\n",myGps->gsv.Sat4SNR);

                	                myGps->gsv.ChSum = 0;
                        	        printf("myGps->gsv.ChSum %i\n",myGps->gsv.ChSum);

					serialize_send(myGps);
					return;
				}
				myGps->gsv.Sat2ID = atoi(temp);
                                printf("myGps->gsv.Sat2ID %i\n",myGps->gsv.Sat2ID);
                                myGps->gsv.Sat2El = atoi(strsep(&string,","));
                                printf("myGps->gsv.Sat2El %i\n",myGps->gsv.Sat2El);
                                myGps->gsv.Sat2Az = atoi(strsep(&string,","));
                                printf("myGps->gsv.Sat2Az %i\n",myGps->gsv.Sat2Az);
                                myGps->gsv.Sat2SNR = atoi(strsep(&string,","));
                                printf("myGps->gsv.Sat2SNR %i\n",myGps->gsv.Sat2SNR);

                                if (((temp = strsep(&string,",")) == NULL))
                                {
                                        myGps->gsv.Sat3ID = 0;
                                        printf("myGps->gsv.Sat3ID %i\n",myGps->gsv.Sat3ID);
                                        myGps->gsv.Sat3El = 0;
                                        printf("myGps->gsv.Sat3El %i\n",myGps->gsv.Sat3El);
                                        myGps->gsv.Sat3Az = 0;
                                        printf("myGps->gsv.Sat3Az %i\n",myGps->gsv.Sat3Az);
                                        myGps->gsv.Sat3SNR = 0;
                                        printf("myGps->gsv.Sat3SNR %i\n",myGps->gsv.Sat3SNR);

                                        myGps->gsv.Sat4ID = 0;
                                        printf("myGps->gsv.Sat4ID %i\n",myGps->gsv.Sat4ID);
                                        myGps->gsv.Sat4El = 0;
                                        printf("myGps->gsv.Sat4El %i\n",myGps->gsv.Sat4El);
                                        myGps->gsv.Sat4Az = 0;
                                        printf("myGps->gsv.Sat4Az %i\n",myGps->gsv.Sat4Az);
                                        myGps->gsv.Sat4SNR = 0;
                                        printf("myGps->gsv.Sat4SNR %i\n",myGps->gsv.Sat4SNR);

                                        myGps->gsv.ChSum = 0;
                                        printf("myGps->gsv.ChSum %i\n",myGps->gsv.ChSum);

					serialize_send(myGps);
                                        return;
                                }
                                myGps->gsv.Sat3ID = atoi(temp);
                                printf("myGps->gsv.Sat3ID %i\n",myGps->gsv.Sat3ID);
                                myGps->gsv.Sat3El = atoi(strsep(&string,","));
                                printf("myGps->gsv.Sat3El %i\n",myGps->gsv.Sat3El);
                                myGps->gsv.Sat3Az = atoi(strsep(&string,","));
                                printf("myGps->gsv.Sat3Az %i\n",myGps->gsv.Sat3Az);
                                myGps->gsv.Sat3SNR = atoi(strsep(&string,","));
                                printf("myGps->gsv.Sat3SNR %i\n",myGps->gsv.Sat3SNR);

                                if (((temp = strsep(&string,",")) == NULL))
                                {
                                        myGps->gsv.Sat4ID = 0;
                                        printf("myGps->gsv.Sat4ID %i\n",myGps->gsv.Sat4ID);
                                        myGps->gsv.Sat4El = 0;
                                        printf("myGps->gsv.Sat4El %i\n",myGps->gsv.Sat4El);
                                        myGps->gsv.Sat4Az = 0;
                                        printf("myGps->gsv.Sat4Az %i\n",myGps->gsv.Sat4Az);
                                        myGps->gsv.Sat4SNR = 0;
                                        printf("myGps->gsv.Sat4SNR %i\n",myGps->gsv.Sat4SNR);

                                        myGps->gsv.ChSum = 0;
                                        printf("myGps->gsv.ChSum %i\n",myGps->gsv.ChSum);
 
					serialize_send(myGps);
                                        return;
                                }
                                myGps->gsv.Sat4ID = atoi(temp);
                                printf("myGps->gsv.Sat4ID %i\n",myGps->gsv.Sat4ID);
                                myGps->gsv.Sat4El = atoi(strsep(&string,","));
                                printf("myGps->gsv.Sat4El %i\n",myGps->gsv.Sat4El);
                                myGps->gsv.Sat4Az = atoi(strsep(&string,","));
                                printf("myGps->gsv.Sat4Az %i\n",myGps->gsv.Sat4Az);
                                myGps->gsv.Sat4SNR = atoi(strsep(&string,"*"));
                                printf("myGps->gsv.Sat4SNR %i\n",myGps->gsv.Sat4SNR);

                                myGps->gsv.ChSum = atoi(strsep(&string,"\n"));
	                        printf("myGps->gsv.ChSum %i\n",myGps->gsv.ChSum);

                                serialize_send(myGps);
                                return;

                        }
}

void serialize_send(Gp735Type* model)
{

	if (SERIALIZE(&destination, &destinationSize, model->gsv) != CODEFIRST_OK)
	{
        	printf("Failed to serialize\r\n");
		return;
	}
        printf("Serialized %s\n\n",destination);

	if ((messageHandle = IoTHubMessage_CreateFromByteArray(destination,destinationSize)) == NULL)
	{
		printf("unable to create a new IoTHubMessage\r\n");
				return;
	}

	IoTHubClient_SendEventAsync(iotHubClientHandle, messageHandle, NULL, NULL);

	IoTHubMessage_Destroy(messageHandle);
}




