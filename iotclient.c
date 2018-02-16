#include <stdlib.h>

#include <stdio.h>
#include <stdint.h>

#include "serializer.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/sastoken.h"
#include "azure_c_shared_utility/platform.h"
#include "iothub_client.h"
#include "iothubtransportamqp.h"
#include "iothub_client_ll.h"

#include "iothub_message.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "iothubtransportmqtt.h"

#define MAX_NUMBER_OF_MESSAGES 5

static const unsigned int SLEEP_IN_MILLISECONDS = 500;
static const int MESSAGE_BASE_TRACKING_ID = 42;
static int g_callbackInvoked;


typedef struct EVENT_INSTANCE_TAG
{
    IOTHUB_MESSAGE_HANDLE messageHandle;
    int messageTrackingId;  // For tracking the messages within the user callback.
} EVENT_INSTANCE;

static void ReceiveConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
{
    EVENT_INSTANCE* eventInstance = (EVENT_INSTANCE*)userContextCallback;
    (void)printf("Confirmation[%d] received for message tracking id = %d with result = %d\r\n", g_callbackInvoked, eventInstance->messageTrackingId, result);
    /* Some device specific action code goes here... */
    g_callbackInvoked++;
}

int main(void)
{
    const char* connectionString ="HostName=rasppi.azure-devices.net;DeviceId=rasp-pi;SharedAccessKey=diTGPgz3pjjKYqwnTKi+8afq1z/iXJQDe98NET/Q7zQ=";

//"HostName=rasppi.azure-devices.net;DeviceId=rasp-pi;SharedAccessKey=diTGPgz3pjjKYqwnTKi+8afq1z/iXJQDe98NET/Q7zQ=";
  
    IOTHUB_CLIENT_HANDLE iotHubClientHandle;

    EVENT_INSTANCE eventInstance[MAX_NUMBER_OF_MESSAGES];

    size_t msgLength = 1024;
    const char* msgText = malloc(msgLength);
    g_callbackInvoked = 0;

    (void)printf("Starting the IoTHub client sample to Send Event Asynchronously...\r\n");


    if ((iotHubClientHandle = IoTHubClient_CreateFromConnectionString(connectionString, AMQP_Protocol)) == NULL)
    {
        (void)printf("ERROR: iotHubClientHandle is NULL!\r\n");
    }
    else
    {
        IOTHUB_CLIENT_STATUS sendStatus;
        for (int i = 0; i < MAX_NUMBER_OF_MESSAGES; i++)
        {
            sprintf_s((char*)msgText, msgLength, "Message_%d_From_IoTHubClient", i);
            if ((eventInstance[i].messageHandle = IoTHubMessage_CreateFromByteArray((const unsigned char*)msgText, strlen(msgText))) == NULL)
            {
                (void)printf("ERROR: IoTHubMessageHandle is NULL!\r\n");
            }
            else
            {
                eventInstance[i].messageTrackingId = MESSAGE_BASE_TRACKING_ID + i;
                if (IoTHubClient_SendEventAsync(iotHubClientHandle, eventInstance[i].messageHandle, ReceiveConfirmationCallback, &eventInstance[i]) != IOTHUB_CLIENT_OK)
                {
                    (void)printf("ERROR: IoTHubClient_SendEventAsync..........FAILED!\r\n");
                }
                else
                {
                    (void)printf("IoTHubClient_SendEventAsync accepted data for transmission to IoT Hub.\r\n");
                }                    
                (void)printf("IoTHubClient_GetSendStatus() returns %d\r\n", IoTHubClient_GetSendStatus(iotHubClientHandle,&sendStatus));
                (void)printf("IoTHubClient_GetSendStatus indicated a client status of: %d\r\n", sendStatus);
            }
        }
        while (g_callbackInvoked < MAX_NUMBER_OF_MESSAGES)
        {
            ThreadAPI_Sleep(SLEEP_IN_MILLISECONDS);
        }
        (void)printf("IoTHubClient_GetSendStatus() returns %d\r\n", IoTHubClient_GetSendStatus(iotHubClientHandle, &sendStatus));
        (void)printf("IoTHubClient_GetSendStatus indicated a client status of: %d\r\n", sendStatus);
    }
    IoTHubClient_Destroy(iotHubClientHandle);
    return 0;
}

