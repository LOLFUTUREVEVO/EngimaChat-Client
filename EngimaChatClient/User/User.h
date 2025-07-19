#pragma once
#ifndef USER_H
#define USER_H

class User {
	
public:
	User();
	User(std::string userPubFile);
	User(std::string user, std::string userPubFile);
	User(std::string user, std::string cPassword);
	void generateUserFile(char u[512], char p[1028]); // This function will generate the user.private and user.public information
	
	User readUserFile();
	void setUserName(char uGoal[512]);
	void setPassword(char uPass[1028]);



private:
	char username[512];
	std::string password;
};





#endif