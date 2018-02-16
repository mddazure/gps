#include <stdlib.h>

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>


//#include "serializer.h"
#include "iothub_client.h"
#include "iothub_message.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/sastoken.h"
#include "azure_c_shared_utility/platform.h"
//#include "iothubtransportamqp.h"
//#include "iothub_client_ll.h"

#include "iothubtransportmqtt.h"


//static const char* connectionString = "HostName=rasppi.azure-devices.net;DeviceId=rasp-pi;SharedAccessKey=diTGPgz3pjjKYqwnTKi+8afq1z/iXJQDe98NET/Q7zQ=";

//static const char* connectionString = "HostName=rasppi.azure-devices.net;DeviceId=rasp-pi;SharedAccessKey=diTGPgz3pjjKYqwnTKi+8afq1z/iXJQDe98NET/Q7zQ=";


static const char* connectionString = "HostName=rasppi.azure-devices.net;DeviceId=rasp-pi;SharedAccessKey=diTGPgz3pjjKYqwnTKi+8afq1z/iXJQDe98NET/Q7zQ=";

static char msgText[1024];
int i=0;
int callbackCounter=0;
size_t iterator = 0;
int receiveContext = 0;

typedef struct EVENT_INSTANCE_TAG
{
    IOTHUB_MESSAGE_HANDLE messageHandle;
    size_t messageTrackingId;  // For tracking the messages within the user callback.
} EVENT_INSTANCE;


static void SendConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
	 {
		EVENT_INSTANCE* eventInstance = (EVENT_INSTANCE*)userContextCallback;
		(void)printf("Confirmation[%d] received for message tracking id = %d with result = %s\r\n", callbackCounter, eventInstance->messageTrackingId,ENUM_TO_STRING(IOTHUB_CLIENT_CONFIRMATION_RESULT, result));
		/* Some device specific action code goes here... */
		callbackCounter++;
		IoTHubMessage_Destroy(eventInstance->messageHandle);
	}

int main(void)
{


IOTHUB_CLIENT_HANDLE iotHubClientHandle;
EVENT_INSTANCE message;
IOTHUB_CLIENT_RESULT sendResult;

    if (platform_init() != 0)
    {
        (void)printf("Failed to initialize platform.\r\n");
	return (1);
    }

	if((iotHubClientHandle = IoTHubClient_CreateFromConnectionString(connectionString, MQTT_Protocol)) == NULL)
	{
		printf("ERROR: iotHubClientHandle is NULL! \r\n");
		return(1);
	}
            bool traceOn = true;
            IoTHubClient_SetOption(iotHubClientHandle, "logtrace", &traceOn);

	sprintf_s(msgText, sizeof(msgText), "Message_%d_From_IoTHubClient_Over_MQTT", i);
	printf("msgTxt %s created \n", msgText);
	if ((message.messageHandle = IoTHubMessage_CreateFromByteArray((const unsigned char*)msgText, strlen(msgText))) == NULL)
	{
		printf("ERROR: iotHubMessageHandle is NULL!\r\n");
		return(1);
	}
	printf("message with trackingid %d created \n", message.messageTrackingId);

	sendResult = IoTHubClient_SendEventAsync(iotHubClientHandle, message.messageHandle, SendConfirmationCallback , &message);
	if(sendResult != IOTHUB_CLIENT_OK)
	{
		printf("ERROR: IoTHubClient_SendEventAsync..........FAILED! with sendResult %d\r\n", sendResult);
	}
	else
	{
		printf("IoTHubClient_SendEventAsync accepted message for transmission to IoT Hub.\r\n");	
		printf("Message %s sent \n", msgText);
	}

	sleep(1);
	IoTHubClient_Destroy(iotHubClientHandle);
	platform_deinit();


/*	printf("Start IoTHubClient_LL_DoWork\n");
	IoTHubClient_LL_DoWork(iotHubClientHandle);
	ThreadAPI_Sleep(1);
	printf("Finished IoTHubClient_LL_DoWork\n");
*/
//	IoTHubClient_LL_Destroy	(iotHubClientHandle);	



return(0);
}
