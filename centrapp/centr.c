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
    bson_destroy(command);

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

cleanup:
    bson_destroy(command);
    //bson_destroy(query);
    //bson_destroy(noviquery);
    //mongoc_cursor_destroy(cursor);
    mongoc_collection_destroy(collection);
    mongoc_database_destroy(database);
    mongoc_server_api_destroy(api);
    mongoc_client_destroy(client);
    mongoc_cleanup();

    return rc;
}

void run_server(mongoc_collection_t *collection) {

    int id = 0;

    bson_error_t error = {0};
    // create server socket similar to what was done in 
    // client program 
    int servSockD = socket(AF_INET, SOCK_STREAM, 0); 
  
    // define server address 
    struct sockaddr_in servAddr; 
  
    servAddr.sin_family = AF_INET; 
    servAddr.sin_port = htons(9004); 
    servAddr.sin_addr.s_addr = INADDR_ANY; 
  
    // bind socket to the specified IP and port 
    bind(servSockD, (struct sockaddr*)&servAddr, 
         sizeof(servAddr)); 
  
    // listen for connections 
    listen(servSockD, 4); 
    
    while(true) {
    
    printf("WAITING TO ACCEPT!\n");
  
    // integer to hold client socket. 
    int clientSocket = accept(servSockD, NULL, NULL); 
    
    char req[10] = {'\0'};
    char response[10] = {'\0'};
    recv(clientSocket, req, sizeof(req), 0);
  
    //DA LI SE DODAJE NOVI USER ILI DODAJE KNJIGA!!!
    if (strcmp(req, "BOOK") == 0) {
  	
  	char clanski_broj[10];
  	recv(clientSocket, clanski_broj, sizeof(clanski_broj), 0);
  	int cl_br = atoi(clanski_broj);	
  	int num_books = -1;
  	
  	printf("FIND HOW MANY BOOKS BELONG TO %d\n",cl_br);
  	mongoc_cursor_t *cursor;
  	bson_t *qu = BCON_NEW("clanski_broj", BCON_INT32(cl_br));
    	bson_t *opts = BCON_NEW("projection","{","broj_knjiga", BCON_BOOL (true),"}");
    	cursor = mongoc_collection_find_with_opts(collection,qu,opts,NULL);
    	while (mongoc_cursor_next(cursor, &qu))
    	{
        	bson_iter_t iter;
        	if (bson_iter_init_find(&iter, qu, "broj_knjiga")) {
        		//const bson_value_t *value;
        		bson_value_t *value;
        		value = bson_iter_value(&iter);
        		//printf("iter type: %d\n",value->value_type);
        		printf("value: %d\n",value->value.v_int32);
        		num_books = value->value.v_int32;
        		//bson_value_destroy(value);
        	}
    	}
    	if (mongoc_cursor_error(cursor, &error))
    	{
        	fprintf(stderr, "%s\n", error.message);
        	return;
    	}
    	bson_destroy(qu);
    	mongoc_cursor_destroy(cursor);
    	if(num_books > 2 || num_books < 0) {
    		response[0] = 'N';
    	} else {
    		num_books++;
    		response[0] = 'Y';
    		bson_t *qu2 = BCON_NEW("clanski_broj", BCON_INT32(cl_br));
    		bson_t *update;
    		update = BCON_NEW("$set","{","broj_knjiga",BCON_INT32(num_books),"}");
    		mongoc_collection_update(collection, MONGOC_UPDATE_NONE, qu2, update, NULL, &error);
    		printf("Updated!\n");
    		bson_destroy(update);
    		bson_destroy(qu2);
    	}
    	
    	//PRINT SVIH
    	bson_t *qu3 = bson_new();
    	mongoc_cursor_t *cursor3 = mongoc_collection_find_with_opts(collection, qu3, NULL, NULL);
    	while (mongoc_cursor_next(cursor3, &qu3))
    	{
        	char *str2 = bson_as_canonical_extended_json(qu3, NULL);
        	printf("%s\n", str2);
        	bson_free(str2);
    	}
    	if (mongoc_cursor_error(cursor3, &error))
    	{
        	fprintf(stderr, "%s\n", error.message);
    	}
    	send(clientSocket,response,sizeof(response),0);
    	close(clientSocket);
    	bson_destroy(qu3);
    	bson_destroy(opts);
    	mongoc_cursor_destroy(cursor3);
  	
    } else if(strcmp(req, "USER") == 0) {
  
    	printf("ADDING USER!\n");
    	
    	char ime[25] = {'\0'}; 
    	char prezime[25] = {'\0'};
    	char adresa[35] = {'\0'};
    	char jmbg[15] = {'\0'};
    	recv(clientSocket, ime, sizeof(ime), 0);
    	recv(clientSocket, prezime, sizeof(prezime), 0);
    	recv(clientSocket, adresa, sizeof(adresa), 0);
    	recv(clientSocket, jmbg, sizeof(jmbg), 0);
    
    	int jmbgcount;
    	bson_t *jmbgquery = bson_new();
    	BSON_APPEND_UTF8(jmbgquery, "jmbg", jmbg);
    	jmbgcount = mongoc_collection_count(collection,MONGOC_QUERY_NONE,jmbgquery,0,0,NULL,&error);
    	bson_destroy(jmbgquery);
    
    	if(jmbgcount == 0) {
    		bson_t *query = bson_new();
    		id = mongoc_collection_count(collection,MONGOC_QUERY_NONE,query,0,0,NULL,&error);
    		if (id < 0) {
    			fprintf(stderr, "Count failed: %s\n",error.message);
    		} else {
    			printf("%d documents counted.\n",id);
    		}
  		bson_destroy(query);
  	
  		sprintf(response,"Y%d",id);
  		send(clientSocket, response, sizeof(response), 0);
 		close(clientSocket);
  	  
 		bson_t *newdoc = BCON_NEW("ime", BCON_UTF8(ime), "prezime", BCON_UTF8(prezime),
                   "jmbg", BCON_UTF8(jmbg), "adresa", BCON_UTF8(adresa), 
                   "clanski_broj", BCON_INT32(id), "broj_knjiga", BCON_INT32(0));
                 
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
    		printf("JMBG vec postoji!!\n");
    		response[0] = 'N';
    		send(clientSocket, response, sizeof(response), 0);
 		close(clientSocket);
    	}
    } else if (strcmp(req,"RET") == 0) {
    
    	char clanski_broj[10];
  	recv(clientSocket, clanski_broj, sizeof(clanski_broj), 0);
  	close(clientSocket);
  	int cl_br = atoi(clanski_broj);	
  	int num_books = -1;
  	
  	mongoc_cursor_t *cursor;
  	bson_t *qu = BCON_NEW("clanski_broj", BCON_INT32(cl_br));
    	bson_t *opts = BCON_NEW("projection","{","broj_knjiga", BCON_BOOL (true),"}");
    	cursor = mongoc_collection_find_with_opts(collection,qu,opts,NULL);
    	while (mongoc_cursor_next(cursor, &qu))
    	{
        	bson_iter_t iter;
        	if (bson_iter_init_find(&iter, qu, "broj_knjiga")) {
        		//const bson_value_t *value;
        		bson_value_t *value;
        		value = bson_iter_value(&iter);
        		//printf("iter type: %d\n",value->value_type);
        		printf("value (ret): %d\n",value->value.v_int32);
        		num_books = value->value.v_int32;
        		bson_value_destroy(value); //mozda i ne
        	}
    	}
    	if (mongoc_cursor_error(cursor, &error))
    	{
        	fprintf(stderr, "%s\n", error.message);
        	return;
    	}
    	bson_destroy(qu);
    	bson_destroy(opts);
    	mongoc_cursor_destroy(cursor);
    	
    	if(num_books <= 0) {
    		printf("COULDN'T FIND USER IN CENTRAL DATABASE\n");
    	} else {
    		num_books--;
    		printf("LOWERING NUMBOOKS TO %d FOR ID %d\n",num_books,cl_br);
    		bson_t *qu2 = BCON_NEW("clanski_broj", BCON_INT32(cl_br));
    		bson_t *update;
    		update = BCON_NEW("$set","{","broj_knjiga",BCON_INT32(num_books),"}");
    		mongoc_collection_update(collection, MONGOC_UPDATE_NONE, qu2, update, NULL, &error);
    		bson_destroy(update);
    		bson_destroy(qu2);
    	}
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
    	close(clientSocket);
    	printf("Exiting app...\n");
    	break;
    }
    
    }
    close(servSockD);
}
