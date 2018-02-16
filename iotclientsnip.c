#include <stdint.h>

#include "serializer.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/sastoken.h"
#include "azure_c_shared_utility/platform.h"
#include "iothub_client.h"
#include "iothubtransportamqp.h"
#include "iothub_client_ll.h"

static const char* connectionString = "HostName=rasppi.azure-devices.net;DeviceId=rasp-pi;SharedAccessKey=diTGPgz3pjjKYqwnTKi+8afq1z/iXJQDe98NET/Q7zQ=";
static char msgText[1024];

static IOTHUBMESSAGE_DISPOSITION_RESULT ReceiveMessageCallback(IOTHUB_MESSAGE_HANDLE message, void* userContextCallback)
{
  const char* buffer;
  size_t size;
  IoTHubMessage_GetByteArray(message, (const unsigned char**)&buffer, &size);
  (void)printf("Received Message with Data: <<<%.*s>>> & Size=%d\r\n", (int)size, buffer, (int)size);
  /* Some device specific action code goes here... */
  return IOTHUBMESSAGE_ACCEPTED;
}
static void SendConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
{
  IOTHUB_MESSAGE_HANDLE* message = (IOTHUB_MESSAGE_HANDLE*)userContextCallback;
  (void)printf("Confirmation received for message tracking id = %d with result = %s\r\n", message->messageTrackingId, ENUM_TO_STRING(IOTHUB_CLIENT_CONFIRMATION_RESULT, result));
  /* Some device specific action code goes here... */
  IoTHubMessage_Destroy(*message);
}

void main(void){
  IOTHUB_CLIENT_HANDLE iotHubClientHandle;
  IOTHUB_MESSAGE_HANDLE message;
  /* Create IoT Hub Client instance */
  iotHubClientHandle = IoTHubClient_CreateFromConnectionString(connectionString, AMQP_Protocol);
  /* Setting Message call back, so we can receive Commands. */
  IoTHubClient_SetMessageCallback(iotHubClientHandle, ReceiveMessageCallback, &receiveContext);
  /* Now that we are ready to receive commands, let's send a message */
  sprintf_s(msgText, sizeof(msgText), "{\"deviceId\":\"myFirstDevice\",\"data\":%.2f}", rand()%4+2 );
  message.messageHandle = IoTHubMessage_CreateFromByteArray((const unsigned char*)msgText, strlen(msgText));
  IoTHubClient_SendEventAsync(iotHubClientHandle, message, SendConfirmationCallback, &message);
  /* Add your code here ... */
  /* When everything is done and the app is closing, clean up resources */
  IoTHubClient_Destroy(iotHubClientHandle);
}

