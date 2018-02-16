#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>                     //Used for UART
#include <fcntl.h>                      //Used for UART
#include <termios.h>            //Used for UART
#include <stdint.h>

#include "iothub_client.h"
#include "iothub_message.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/platform.h"
#include "iothubtransportmqtt.h"

/*String containing Hostname, Device Id & Device Key in the format:                         */
/*  "HostName=<host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"                */
/*  "HostName=<host_name>;DeviceId=<device_id>;SharedAccessSignature=<device_sas_token>"    */
static const char* connectionString = "HostName=rasppi.azure-devices.net;DeviceId=rasp-pi;SharedAccessKey=diTGPgz3pjjKYqwnTKi+8afq1z/iXJQDe98NET/Q7zQ=";


static char msgText[1024];

unsigned char rx_buffer[1024] = "Test message";

typedef struct EVENT_INSTANCE_TAG
{
    IOTHUB_MESSAGE_HANDLE messageHandle;
    size_t messageTrackingId;  // For tracking the messages within the user callback.
} EVENT_INSTANCE;


int main(void)
{

	IOTHUB_CLIENT_HANDLE iotHubClientHandle;

	EVENT_INSTANCE message;	

	platform_init();


        iotHubClientHandle = IoTHubClient_CreateFromConnectionString(connectionString, MQTT_Protocol);


	sprintf_s(msgText, sizeof(msgText), " %s",rx_buffer);
	message.messageHandle = IoTHubMessage_CreateFromByteArray((const unsigned char*)msgText, strlen(msgText));


        if (IoTHubClient_SendEventAsync(iotHubClientHandle, message.messageHandle, NULL, NULL) != IOTHUB_CLIENT_OK)
        {
		(void)printf("ERROR: IoTHubClient_SendEventAsync..........FAILED!\r\n");
        	return(1);
        }
        else
        {
		(void)printf("IoTHubClient_SendEventAsync accepted message for transmission to IoT Hub.\r\n");
        }


	sleep(1);

	IoTHubClient_Destroy(iotHubClientHandle);
        
	platform_deinit();
}
