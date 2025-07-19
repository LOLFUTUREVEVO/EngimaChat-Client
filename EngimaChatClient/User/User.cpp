#include "User.h"
#include <fstream>




void User::setPassword(char uPass[1028]) {
	password = uPass;
}

void User::generateUserFile(char u[512], char p[1028]) {
	std::ofstream fs;
}


User User::readUserFile() {
	User rUser;

}