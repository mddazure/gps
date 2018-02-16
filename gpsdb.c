#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>                     //Used for UART
#include <fcntl.h>                      //Used for UART
#include <termios.h>            //Used for UART
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "iothub_client.h"
#include "iothub_message.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/platform.h"
#include "iothubtransportmqtt.h"
//#include "serializer.h"
#include "gpsdb.h"

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
bool timeReceived = false;


int NumberOfMessages, MessageNumber,SatsInView;

void gps_run(void);

void parse_nmea(char buffer[256],Gp735Type* mySats);

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

        Gp735Type* modelSats = CREATE_MODEL_INSTANCE(RaspiNamespace, Gp735Type);
        if (modelSats == NULL)
        {
                (void)printf("Failed on CREATE_MODEL_INSTANCE\r\n");
        }

	while(1)
	{

	if(fgets(rx_buffer,1024,fp) != NULL)
		{
			parse_nmea(rx_buffer,modelSats);
                }
	}

        IoTHubClient_Destroy(iotHubClientHandle);
}

void parse_nmea(char buffer[256],Gp735Type* mySats)
{

                        strncpy(nmea_id,buffer,6);



                        if(strcmp(nmea_id,"$GPGGA")==0)
                        {

                                printf("nmea_id $GPGGA received %s\n",buffer);
                                string = strdup(buffer);

                                temp = strsep(&string,",");
                                printf("temp GGAHeader %s\n",temp);
                                mySats->sat.UTCTime = strsep(&string,",");
				printf(" mySats->sat.UTCTime %s\n", mySats->sat.UTCTime);
				timeReceived = true;
			}

                        if((strcmp(nmea_id,"$GPGSV")==0 && timeReceived))
                        {

                                printf("nmea_id $GPGSV received %s\n",buffer);
                                string = strdup(buffer);

                                temp = strsep(&string,",");
                                printf("temp GSVHeader %s\n",temp);

                                NumberOfMessages = atoi(strsep(&string,","));
                                printf("NumberOfMessages %i\n",NumberOfMessages);
                                MessageNumber = atoi(strsep(&string,","));
                                printf("MessageNumber %i\n",MessageNumber);
                                SatsInView = atoi(strsep(&string,","));
                                printf("SatsInView %i\n",SatsInView);


				mySats->sat.SatID = atoi(strsep(&string,","));
                                printf("mySats->sat.SatID %i\n",mySats->sat.SatID);
                                mySats->sat.El = atoi(strsep(&string,","));
                                printf("mySats->sat.El %i\n",mySats->sat.El);
                                mySats->sat.Az = atoi(strsep(&string,","));
                                printf("mySats->sat.Az %i\n",mySats->sat.Az);
                                mySats->sat.Snr = atoi(strsep(&string,","));
                                printf("mySats->sat.Snr %i\n",mySats->sat.Snr);

                                serialize_send(mySats);

                                if (((temp = strsep(&string,",")) == NULL))
				{
	                                return;
				}
				else
				{
        	                        mySats->sat.SatID = atoi(temp);
	                                printf("mySats->sat.SatID %i\n",mySats->sat.SatID);
                	                mySats->sat.El = atoi(strsep(&string,","));
                        	        printf("mySats->sat.El %i\n",mySats->sat.El);
                                	mySats->sat.Az = atoi(strsep(&string,","));
                               		printf("mySats->sat.Az %i\n",mySats->sat.Az);
                                	mySats->sat.Snr = atoi(strsep(&string,","));
                                	printf("mySats->sat.Snr %i\n",mySats->sat.Snr);

                                	serialize_send(mySats);
	                                if (((temp = strsep(&string,",")) == NULL))
	                                {
						return;
                	                }
                        	        else
                                	{
                                        	mySats->sat.SatID = atoi(temp);
                                        	printf("mySats->sat.SatID %i\n",mySats->sat.SatID);
                                        	mySats->sat.El = atoi(strsep(&string,","));
                                       		printf("mySats->sat.El %i\n",mySats->sat.El);
	                                        mySats->sat.Az = atoi(strsep(&string,","));
        	                                printf("mySats->sat.Az %i\n",mySats->sat.Az);
                	                        mySats->sat.Snr = atoi(strsep(&string,","));
                        	                printf("mySats->sat.Snr %i\n",mySats->sat.Snr);

                                	        serialize_send(mySats);

		                                if (((temp = strsep(&string,",")) == NULL))
                		                {
							return;
                                		}
                                		else
		                                {
                		                        mySats->sat.SatID = atoi(temp);
                                		        printf("mySats->sat.SatID %i\n",mySats->sat.SatID);
		                                        mySats->sat.El = atoi(strsep(&string,","));
                		                        printf("mySats->sat.El %i\n",mySats->sat.El);
                                		        mySats->sat.Az = atoi(strsep(&string,","));
		                                        printf("mySats->sat.Az %i\n",mySats->sat.Az);
                		                        mySats->sat.Snr = atoi(strsep(&string,","));
                                		        printf("mySats->sat.Snr %i\n",mySats->sat.Snr);

		                                        serialize_send(mySats);

							return;
						}
					}
				}
			}

}

void serialize_send(Gp735Type* model)
{

	if (SERIALIZE(&destination, &destinationSize, model->sat) != CODEFIRST_OK)
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




