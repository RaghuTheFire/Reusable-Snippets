
#include <iostream>
#include <string>
#include <vector>

using namespace std;

string getIPClass(string ipAddress) {
    int firstOctet = stoi(ipAddress.substr(0, ipAddress.find(".")));

    if (firstOctet >= 0 && firstOctet <= 127) {
        return "Class A";
    } else if (firstOctet >= 128 && firstOctet <= 191) {
        return "Class B";
    } else if (firstOctet >= 192 && firstOctet <= 223) {
        return "Class C";
    } else if (firstOctet >= 224 && firstOctet <= 239) {
        return "Class D (Multicast)";
    } else {
        return "Class E (Reserved)";
    }
}

int main() {
    string ipAddress;
    cout << "Enter an IPv4 address: ";
    cin >> ipAddress;

    string ipClass = getIPClass(ipAddress);
    cout << "The IP address " << ipAddress << " belongs to " << ipClass << "." << endl;

    return 0;
}
