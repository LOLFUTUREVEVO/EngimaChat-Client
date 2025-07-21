#pragma once
#ifndef USER_H
#define USER_H
#include <string>


class User {
	
public:
	User();
	User(std::string userPubFile);
	User(std::string user, std::string userPubFile);
	User(std::string user, std::string cPassword, std::string userPubFile="user.pub");
	void generateUserFile(char u[512], char p[1028]); // This function will generate the user.private and user.public information
	
	User readUserFile();
	void setUserName(std::string user);
	void setPassword(std::string pass);



private:
	std::string username;
	std::string password;
};





#endif