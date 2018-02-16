all:
	gcc -Wall -std=c99 iotclient.c -o iotclient -rdynamic -liothub_client_amqp_transport -liothub_client -luamqp -laziotsharedutil -lserializer -lcurl -lssl -lcrypto -lpthread -luuid -lm
