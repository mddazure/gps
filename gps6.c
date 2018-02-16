#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>                     //Used for UART
#include <fcntl.h>                      //Used for UART
#include <termios.h>            //Used for UART
#include <stdint.h>
#include <string.h>
#include <unistd.h>


#include "iothub_client.h"
#include "iothub_message.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/shared_util_options.h"
#include "iothub_client_options.h"
#include "iothubtransportmqtt.h"
#include "serializer.h"
#include "gps6.h"

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
size_t destSize;
size_t *ptr_destSize;
unsigned char *iotMessage;
char *string;
char *stringbase;
char *temp;
char *UtcTime;
char* dummystring;
const char* initMessage = "{\"UTCTime\":\"";
const char* midMessage = "\",\"Sats\":[";
const char* comma = "},";
const char* endMessage = "]}"; 
int dummyint;
int NumberOfMessages;
int MessageNumber;
int SatsInView;
int counter;

SatModel* mySat;

void gps_run(void);

void parse_nmea(char buffer[1024]);

void serialize(SatModel* model);

void send();

//void serialize_send(SatModel* model);

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

	destination = malloc(sizeof(char) * 50000);
	iotMessage = malloc(sizeof(char) * 50000);
	iotMessage[0] = '\0';

	stringbase = malloc(1000);
	temp = malloc(1000);
	UtcTime = malloc(1000);
	dummystring = malloc(1000);

	gps_run();

        platform_deinit();


    	return;
}

void gps_run(void)
{
	if((iotHubClientHandle = IoTHubClient_CreateFromConnectionString(connectionString, MQTT_Protocol)) == NULL)
	{
		(void)printf("ERROR: iotHubClientHandle is NULL!\r\n");
		return;
	}

	printf("iotHubClientHandle %p\n",(void *)iotHubClientHandle);


	mySat = CREATE_MODEL_INSTANCE(RaspiNamespace, SatModel);
        if (mySat == NULL)
        {
                (void)printf("Failed on CREATE_MODEL_INSTANCE\r\n");
        }



	while(1)
	{

	rx_buffer[0] = '\0';
	if(fgets(rx_buffer,1024,fp) != NULL)
		{
			parse_nmea(rx_buffer);
                }
	}

        IoTHubClient_Destroy(iotHubClientHandle);
}

void parse_nmea(char buffer[1024])
{

/*        Gp735Type* myGps = CREATE_MODEL_INSTANCE(RaspiNamespace, Gp735Type);
        if (myGps == NULL)
        {
                (void)printf("Failed on CREATE_MODEL_INSTANCE\r\n");
        }
*/


	strncpy(nmea_id,buffer,6);

	if (strcmp(nmea_id, "$GPGGA")==0)
	{
		string = stringbase;
		printf("nmea_id $GPGGA received %s\n",buffer);
		strcpy(string,buffer);
		temp = strsep(&string,",");
                printf("temp %s\n",temp);
		UtcTime = strsep(&string,",");
                printf("UtcTime %s\n",UtcTime);

		strcat(iotMessage,initMessage);
		strcat(iotMessage,UtcTime);
		strcat(iotMessage,midMessage);

		return;

	}

        if(strcmp(nmea_id,"$GPGSV")==0 && UtcTime != NULL)
        {
		string = stringbase;
		printf("nmea_id $GPGSV received %s\n",buffer);
                strcpy(string,buffer);

		dummystring = strsep(&string,",");
	        printf("GSVHeader %s\n",dummystring);
                NumberOfMessages = atoi(strsep(&string,","));
                printf("NumberOfMessages %i\n",NumberOfMessages);
	        MessageNumber = atoi(strsep(&string,","));
                printf("MessageNumber %i\n",MessageNumber);
                SatsInView = atoi(strsep(&string,","));
                printf("SatsInView %i\n",SatsInView);

			if (MessageNumber < NumberOfMessages)
			{
				for(counter=0;counter<4;counter=counter+1)
				{

					printf("loop 1 counter = %i\n",counter);

	                       		mySat->Sat1.SatID = atoi(strsep(&string,","));
	                               	printf("mySat->Sat1.SatID %i\n",mySat->Sat1.SatID);
		                        mySat->Sat1.SatData.El = atoi(strsep(&string,","));
        				printf("mySat->Sat1.SatData.El %i\n",mySat->Sat1.SatData.El);
                		        mySat->Sat1.SatData.Az = atoi(strsep(&string,","));
                        	        printf("mySat->Sat1.SatData.Az %i\n",mySat->Sat1.SatData.Az);
                                	mySat->Sat1.SatData.Snr = atoi(strsep(&string,","));
	                               	printf("mySat->Sat1.SatData.Snr %i\n",mySat->Sat1.SatData.Snr);

					serialize(mySat);

					strcat(iotMessage,comma);
					printf("iotMessage is now: %s\n",iotMessage);

				}

				return;

			}

			if (SatsInView%4 == 0 && SatsInView != 0)
			{
				for(counter=0;counter<4;counter=counter+1)
                               	{

                                       	printf("loop 2 counter = %i\n",counter);

                                       	mySat->Sat1.SatID = atoi(strsep(&string,","));
                                        printf("mySat->Sat1.SatID %i\n",mySat->Sat1.SatID);
               	                        mySat->Sat1.SatData.El = atoi(strsep(&string,","));
                       	                printf("mySat->Sat1.SatData.El %i\n",mySat->Sat1.SatData.El);
                               	        mySat->Sat1.SatData.Az = atoi(strsep(&string,","));
                                       	printf("mySat->Sat1.SatData.Az %i\n",mySat->Sat1.SatData.Az);
                                        mySat->Sat1.SatData.Snr = atoi(strsep(&string,","));
       	                                printf("mySat->Sat1.SatData.Snr %i\n",mySat->Sat1.SatData.Snr);

               	                        serialize(mySat);
                                        strcat(iotMessage,comma);

                               	}
			}
			else
			{
				for(counter=0;counter<(SatsInView%4);counter=counter+1)
       	                        {
	
      	                                printf("loop 2 counter = %i\n",counter);
	
       	                                mySat->Sat1.SatID = atoi(strsep(&string,","));
               	                        printf("mySat->Sat1.SatID %i\n",mySat->Sat1.SatID);
                                       	mySat->Sat1.SatData.El = atoi(strsep(&string,","));
                                        printf("mySat->Sat1.SatData.El %i\n",mySat->Sat1.SatData.El);
       	                                mySat->Sat1.SatData.Az = atoi(strsep(&string,","));
               	                        printf("mySat->Sat1.SatData.Az %i\n",mySat->Sat1.SatData.Az);
                       	                mySat->Sat1.SatData.Snr = atoi(strsep(&string,","));
                               	        printf("mySat->Sat1.SatData.Snr %i\n",mySat->Sat1.SatData.Snr);

       	                                serialize(mySat);
                                        strcat(iotMessage,destination);
                                        strcat(iotMessage,comma);

               	                }
			}

			iotMessage[strlen(iotMessage)-1]='\0';

			strcat(iotMessage,endMessage);
			printf("\niotMessage before sending: %s\n", iotMessage);

		        if ((messageHandle = IoTHubMessage_CreateFromByteArray(iotMessage,strlen(iotMessage))) == NULL)
       			{
	                	printf("unable to create a new IoTHubMessage\r\n");
              	        	return;
		        }

		        printf("IoTHubClient_SendEventAsync reached\nmessageHandle = %p\niotHubClientHandle = %p\n",(void *) messageHandle,(void *) iotHubClientHandle);

	
   			IoTHubClient_SendEventAsync(iotHubClientHandle, messageHandle, NULL, NULL);

			iotMessage[0] = '\0';

			 IoTHubMessage_Destroy(messageHandle);

//			free(messageHandle);

/*				IoTHubClient_LL_SendEventAsync(iotHubClientHandle, messageHandle, NULL, NULL);

			        IoTHubClient_LL_DoWork(iotHubClientHandle);

			        ThreadAPI_Sleep(1000);
*/


			return;
	}
}
			



void serialize(SatModel* model)
{

printf("serialize reached\n");

        
if (SERIALIZE(&destination, &destinationSize, model->Sat1) != CODEFIRST_OK)
        {
                printf("Failed to serialize\r\n");
                return;
        }

	destination[(int) destinationSize-1]='\0';

        printf("Serialized: %s\n",destination);

        strcat(iotMessage,destination);

        free(destination);


	return;

}




