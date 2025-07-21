#include "User.h"
#include <fstream>
#include <iostream>
#include <string>




void User::setUserName(std::string user) {
	username = user;
}

void User::setPassword(std::string pass) {
	password = pass;
}

void User::generateUserFile(char u[512], char p[1028]) {
	std::ofstream fs;
}

User::User() {
	std::cout << "Created a User\n";
}

User User::readUserFile() {
	User rUser;
	return rUser;
}