#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define USERNAME_LEN 30
#define PASSWORD_LEN 30
#define MAX_USER_COUNT 20

struct userItem{
	char userName[USERNAME_LEN];
	char password[PASSWORD_LEN];
};

struct userItem userItemArray[MAX_USER_COUNT];
int userItemCount = 0;

// login in result
enum CHECK_USER_RESULT{
	CHECK_NO_USER,
	CHECK_WRONG_PASSWORD,
	CHECK_OK
};

// get the user list, for sample
void getUserItems(){
	strcpy(userItemArray[0].userName, "admin");
	strcpy(userItemArray[0].password, "123456");


	strcpy(userItemArray[1].userName, "guest");
	strcpy(userItemArray[1].password, "678910");

	userItemCount = 2;
}

// check the user login in
void checkUser(char * userName, char * password){
	enum CHECK_USER_RESULT result;
	// html content
 	printf("%s\n", "<!DOCTYPE html>");
	printf("%s\n", "<html>");
	printf("%s\n", "<head>");
	printf("%s\n", "<meta charset='utf-8'>");
	printf("%s\n", "<meta name='viewport' content='width=device-width, initial-scale=1.0'>");
	printf("%s\n", "<meta http-equiv='X-UA-Compatible' content = 'ie=edge'>");
	printf("%s\n", "<title>Http Server</title>");
	printf("%s\n", "</head>");
	printf("%s\n", "<body>");


	if(userName == NULL){
		result = CHECK_NO_USER;
	}else{
		int i = 0;
		for(; i < userItemCount; ++i){
			if(strcmp(userItemArray[i].userName, userName) == 0){
				if(strcmp(userItemArray[i].password, password) == 0){
					result = CHECK_OK;
				}else{
					result = CHECK_WRONG_PASSWORD;
				}
				break;
			}
		}

		if(i==userItemCount){
			result = CHECK_NO_USER;
		}
	}

	switch(result){
		case CHECK_OK:
			printf("<h1 style='color:lawngreen;'>Welcome, %s.</h1>", userName);
			break;
		case CHECK_WRONG_PASSWORD:
			printf("<h1 style='color:orange;'>Worng password.</h1>");
			break;
		case CHECK_NO_USER:
			printf("<h1 style='color:red;'>The user %s does not existed.</h1>", userName);
			break;
		default:
			printf("<h1 style='color:lawngreen;'>Welcome</h1>");
			break;
	}

	printf("%s\n", "</body>");
	printf("%s\n", "</html>");
}


void main(int argc, char * argv[]){
	getUserItems();

	if(argc < 2){
		checkUser(NULL, NULL);
	}else{
		// copy the query string to the memory
		int queryStringLen = strlen(argv[1]);
		char * queryString = (char *)malloc(queryStringLen + 1);
		memcpy(queryString, argv[1], queryStringLen);
		queryString[queryStringLen] = '\0';

		char userName[USERNAME_LEN] = {0};
		char password[PASSWORD_LEN] = {0};
		int andIndex = 0;		

		while(queryString[andIndex] && queryString[andIndex] != '&')
			++andIndex;
		queryString[andIndex] = '\0';
		sscanf(queryString, "username=%s", userName);
		sscanf(queryString+andIndex+1, "password=%s", password);

		checkUser(userName, password);
		free(queryString);
	}

	return;
}