
#include <string>

using namespace std;

/* 
============================================================================
Clase Account la cual representa a las cuentas de mail de usarios.
============================================================================
*/
class Account {
private:
    string username;
    string password;
    string imap_server_ip;
public:
    Account(string username, string password, string imap_server_ip);
};