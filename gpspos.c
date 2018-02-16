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
#include "gpspos.h"

/*String containing Hostname, Device Id & Device Key in the format:                         */
/*  "HostName=<host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"                */
/*  "HostName=<host_name>;DeviceId=<device_id>;SharedAccessSignature=<device_sas_token>"    */
static const char* connectionString = "HostName=rasppi.azure-devices.net;DeviceId=rasp-pi;SharedAccessKey=diTGPgz3pjjKYqwnTKi+8afq1z/iXJQDe98NET/Q7zQ=";

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

                        if(strcmp(nmea_id,"$GPGGA")==0)
                                {

                                printf("nmea_id $GPGGA received %s\n",buffer);
                                string = strdup(buffer);
                                myGps->gga.GSVHeader = strsep(&string,",");
                                printf("myGps->gga.GSVHeader %s\n",myGps->gga.GSVHeader);

                                myGps->gga.UTCTime = atof(strsep(&string,","));
                                printf("myGps->gga.UTCTime %f\n",myGps->gga.UTCTime);

                                myGps->gga.Lat = (atof(strsep(&string,",")))/100;
                                printf("myGps->gga.Lat %f\n",myGps->gga.Lat);

                                myGps->gga.NorthSouth = strsep(&string,",");
                                printf("myGps->gga.NorthSouth \n",myGps->gga.NorthSouth);

				myGps->gga.Lon = (atof(strsep(&string,",")))/100;
                                printf("myGps->gga.Lon %f\n",myGps->gga.Lon);

                                myGps->gga.EastWest = strsep(&string,",");
                                printf("myGps->gga.EastWest \n",myGps->gga.EastWest);

                                temp = atoi(strsep(&string,","));
                                printf("Position Fix Indicator %i\n",temp);

                                temp = atoi(strsep(&string,","));
                                printf("Satellites used %i\n",temp);

                        	myGps->gga.Hdop = atof(strsep(&string,","));
                               	printf("myGps->gga.Hdop %f\n",myGps->gga.Hdop);

                                myGps->gga.Alt = atof(strsep(&string,","));
   	                        printf("myGps->gga.Alt %f\n",myGps->gga.Alt);

                                serialize_send(myGps);
                                return;

	                        }
}

void serialize_send(Gp735Type* model)
{

	if (SERIALIZE(&destination, &destinationSize, model->gga) != CODEFIRST_OK)
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




