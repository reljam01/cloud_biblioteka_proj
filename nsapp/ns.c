#include <mongoc/mongoc.h>
#include <bson/bson.h>
#include <netinet/in.h> //structure for storing address information 
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/socket.h> //for socket APIs 
#include <sys/types.h> 
#include <time.h>
#include <string.h>

void run_server(mongoc_collection_t *);

int main(void)
{
    mongoc_client_t *client = NULL;
    bson_error_t error = {0};
    mongoc_server_api_t *api = NULL;
    mongoc_database_t *database = NULL;
    mongoc_collection_t *collection;
    bson_t *doc = NULL;
    bson_t *command = NULL;
    bson_t reply = BSON_INITIALIZER;
    int rc = 0;
    bool ok = true;

    /* Initialize the MongoDB C Driver. */
    mongoc_init();

    const char *connection_string = getenv("MONGO_CONNECTION_STRING");
    if (!connection_string)
    {
        fprintf(
            stderr,
            "environment variable 'MONGO_CONNECTION_STRING' is missing\n");
        rc = 1;
        goto cleanup;
    }

    client = mongoc_client_new(connection_string);
    if (!client)
    {
        fprintf(stderr, "failed to create a MongoDB client\n");
        rc = 1;
        goto cleanup;
    }

    /* Set the version of the Stable API on the client. */
    api = mongoc_server_api_new(MONGOC_SERVER_API_V1);
    if (!api)
    {
        fprintf(stderr, "failed to create a MongoDB server API\n");
        rc = 1;
        goto cleanup;
    }

    ok = mongoc_client_set_server_api(client, api, &error);
    if (!ok)
    {
        fprintf(stderr, "error: %s\n", error.message);
        rc = 1;
        goto cleanup;
    }

    /* Get a handle on the "admin" database. */
    database = mongoc_client_get_database(client, "admin");
    if (!database)
    {
        fprintf(stderr, "failed to get a MongoDB database handle\n");
        rc = 1;
        goto cleanup;
    }

    /* Ping the database. */
    command = BCON_NEW("ping", BCON_INT32(1));
    ok = mongoc_database_command_simple(
        database, command, NULL, &reply, &error);
    if (!ok)
    {
        fprintf(stderr, "error: %s\n", error.message);
        rc = 1;
        goto cleanup;
    }
    bson_destroy(&reply);

    printf(
        "Pinged your deployment using the MongoDB C Driver. "
        "You successfully connected to MongoDB!\n");

    collection = mongoc_database_create_collection(database, "users", NULL, &error);
    if (collection == NULL)
    {
        fprintf(stderr, "error: %s\n", error.message);
        rc = 1;
        goto cleanup;
    }
    
    //HERE STARTING UP BACKEND AS SERVER
    run_server(collection);

/*
    doc = BCON_NEW("ime", BCON_UTF8("Relja"), "prezime", BCON_UTF8("Mihic"),
                   "adresa", BCON_UTF8("NS"), "jmbg", BCON_UTF8("1"),
                   "clanski_broj", BCON_INT32(1));

    
    if (collection == NULL)
    {
        fprintf(stderr, "error: %s\n", error.message);
        rc = 1;
        goto cleanup;
    }

    printf("TESTTESTTEST\n");
    if (!mongoc_collection_insert_one(collection, doc, NULL, NULL, &error))
    {
        fprintf(stderr, "%s\n", error.message);
        rc = 1;
        goto cleanup;
    }
    else
    {
        printf("Document inserted!\n");
        char *str1 = bson_as_canonical_extended_json(doc, NULL);
        printf("%s\n", str1);
        bson_free(str1);
    }

    bson_t *query = BCON_NEW("clanski_broj", BCON_INT32(1));
    mongoc_cursor_t *cursor;

    cursor = mongoc_collection_find_with_opts(collection, query, NULL, NULL);
    printf("query for clanski_broj 1: ");
    while (mongoc_cursor_next(cursor, &query))
    {
        char *str2 = bson_as_canonical_extended_json(query, NULL);
        printf("%s\n", str2);
        bson_free(str2);
    }
    if (mongoc_cursor_error(cursor, &error))
    {
        fprintf(stderr, "%s\n", error.message);
        rc = 1;
        goto cleanup;
    }

    bson_t *noviquery = BCON_NEW("clanski_broj", BCON_INT32(1));
    if (!mongoc_collection_delete_many(collection,noviquery,NULL,NULL,&error)) {
    	fprintf(stderr, "%s\n", error.message);
        rc = 1;
        goto cleanup;
    } else {
        printf("Deleted everything!\n");
    }
*/  
cleanup:
    bson_destroy(command);
    //bson_destroy(query);
    //bson_destroy(noviquery);
    bson_destroy(doc);
    //mongoc_cursor_destroy(cursor);
    mongoc_collection_destroy(collection);
    mongoc_database_destroy(database);
    mongoc_server_api_destroy(api);
    mongoc_client_destroy(client);
    mongoc_cleanup();

    return rc;
}

void run_server(mongoc_collection_t *collection) {

    bson_error_t error = {0};
    // create server socket similar to what was done in 
    // client program 
    int servSockD = socket(AF_INET, SOCK_STREAM, 0); 
  
    // define server address 
    struct sockaddr_in servAddr; 
  
    servAddr.sin_family = AF_INET; 
    servAddr.sin_port = htons(9001); 
    servAddr.sin_addr.s_addr = INADDR_ANY; 
  
    // bind socket to the specified IP and port 
    bind(servSockD, (struct sockaddr*)&servAddr, 
         sizeof(servAddr)); 
  
    // listen for connections 
    listen(servSockD, 1); 
  while(true) {
    printf("WAITING TO ACCEPT!\n");
  
    // integer to hold client socket. 
    int clientSocket = accept(servSockD, NULL, NULL); 
  
    // send's messages to client socket 
    //send(clientSocket, serMsg, sizeof(serMsg), 0); 
    char response[10] = {'\0'};
    char req[10] = {'\0'};
    recv(clientSocket, req, sizeof(req), 0);
    if (strcmp(req,"BOOK") == 0)
    {
    	char naziv[75] = {'\0'}; 
    	char pisac[35] = {'\0'};
    	char isbn[25] = {'\0'};
    	char id[10] = {'\0'};
    	char newdate[15] = {'\0'};
    	recv(clientSocket, naziv, sizeof(naziv), 0);
	recv(clientSocket, pisac, sizeof(pisac), 0);
	recv(clientSocket, isbn, sizeof(isbn), 0);
	recv(clientSocket, id, sizeof(id), 0);
	recv(clientSocket, newdate, sizeof(newdate), 0);
    
    	int newsocket = socket(AF_INET, SOCK_STREAM, 0); 
  
  	struct sockaddr_in servAddr2; \
  	servAddr2.sin_family = AF_INET; 
  	servAddr2.sin_port = htons(9004); // use some unused port number 
  	servAddr2.sin_addr.s_addr = INADDR_ANY; 
  	int connectStatus2  = connect(newsocket, (struct sockaddr*)&servAddr2, sizeof(servAddr2));\
  	if (connectStatus2 == -1) { 
      		printf("Error connecting!\n"); 
  	} 
  	else {
  		send(newsocket, req, sizeof(req), 0);
      		send(newsocket, id, sizeof(id), 0);
      		recv(newsocket, response, sizeof(response), 0);
  	} 
  	close(newsocket);
    	
    	send(clientSocket, response, sizeof(response), 0);
	close(clientSocket);
    	if (strcmp(response,"Y") == 0) {
		printf("Message: %s %s %s %s %s\n", naziv, pisac, isbn, id, newdate);
    		int day, month, year;
    		sscanf(newdate,"%d-%d-%d",&year,&month,&day);
    		struct tm parsedtime;
    		parsedtime.tm_year = year;
    		parsedtime.tm_mon = month;
    		parsedtime.tm_mday = day;
    		bson_t *newdoc = BCON_NEW("naziv", BCON_UTF8(naziv), "pisac", BCON_UTF8(pisac),
                   "isbn", BCON_UTF8(isbn), "datum", BCON_DATE_TIME(mktime(&parsedtime)*1000),
                   "clanski_broj", BCON_INT32(atoi(id)));
                 
    		if (!mongoc_collection_insert_one(collection, newdoc, NULL, NULL, &error))
    		{
        		fprintf(stderr, "%s\n", error.message);
        		return;
    		}
    		else
    		{
        		printf("Document inserted!\n");
        		bson_t *tempquery = bson_new();
 	    		mongoc_cursor_t *tempcursor;
 	    		tempcursor = mongoc_collection_find_with_opts(collection,tempquery,NULL,NULL);
 	    		while(mongoc_cursor_next(tempcursor, &tempquery)) {
 	    			char *str = bson_as_canonical_extended_json(tempquery,NULL);
 	    			printf("%s\n",str);
 	    			bson_free(str);
 	    		}
 	    		bson_destroy(tempquery);
 	    		mongoc_cursor_destroy(tempcursor);
    		}
    		bson_destroy(newdoc);
    	} else {
    		printf("Couldn't add more books!\n");
    	}
    } else if (strcmp(req,"USER") == 0) {
    	char ime[25] = {'\0'}; 
    	char prezime[25] = {'\0'};
    	char adresa[35] = {'\0'};
    	char jmbg[15] = {'\0'};
    	recv(clientSocket, ime, sizeof(ime), 0);
    	recv(clientSocket, prezime, sizeof(prezime), 0);
    	recv(clientSocket, adresa, sizeof(adresa), 0);
    	recv(clientSocket, jmbg, sizeof(jmbg), 0);
    	
    	int newsocket = socket(AF_INET, SOCK_STREAM, 0); 
  
  	struct sockaddr_in servAddr2; \
  	servAddr2.sin_family = AF_INET; 
  	servAddr2.sin_port = htons(9004); // use some unused port number 
  	servAddr2.sin_addr.s_addr = INADDR_ANY; 
  	int connectStatus2  = connect(newsocket, (struct sockaddr*)&servAddr2, sizeof(servAddr2));\
  	if (connectStatus2 == -1) { 
      		printf("Error connecting!\n"); 
  	} 
  	else {
  		send(newsocket, req, sizeof(req), 0);
      		send(newsocket, ime, sizeof(ime), 0);
      		send(newsocket, prezime, sizeof(prezime), 0);
      		send(newsocket, adresa, sizeof(adresa), 0);
      		send(newsocket, jmbg, sizeof(jmbg), 0);
      		recv(newsocket, response, sizeof(response), 0);
  	} 
  	close(newsocket);
  	
  	send(clientSocket, response, sizeof(response), 0);
  	close(clientSocket);
    } else if (strcmp(req,"DEL") == 0) {
    	close(clientSocket);
    	bson_t *noviquery = bson_new();
    	if (!mongoc_collection_delete_many(collection,noviquery,NULL,NULL,&error)) {
    		fprintf(stderr, "%s\n", error.message);
    	} else {
        	printf("Deleted everything!\n");
   	}
    
    	bson_destroy(noviquery);
    } else if(strcmp(req,"EXIT") == 0) {
    	printf("Exiting app...\n");
    	break;
    }
    }
    close(servSockD);
}
