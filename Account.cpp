
#include "Account.h"

#include <string>

using namespace std;

Account::Account(string username, string password, string imap_server_ip) {
    this->username = username;
    this->password = password;
    this->imap_server_ip = imap_server_ip;
}