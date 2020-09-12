#include <unistd.h>
#include <list>
#include <tins/tins.h>
#include <fstream>
#include <string>
#include <iostream>

using namespace std;
using namespace Tins;

void usage() {
    printf("syntax : airodump <interface> <ssid-list-file>\n");
    printf("sample : beacon-flood mon0 ssid-list.txt\n");
}

int main(int argc, char* argv[]){
    if (argc != 3) {
        usage();
        return -1;
    }

    char* dev = argv[1];
    if (dev == nullptr) {
        fprintf(stderr, "couldn't open device %s\n", dev);
        return -1;
    }

    list<string> ssidList;
    ifstream slist(argv[2]);
    string buffer;

    if (!slist.is_open()) {
        cout << "No file" << endl;
        return -1;
    }

    while(slist) {
        getline(slist, buffer);
        cout << buffer << endl;
        ssidList.push_back(buffer);
    }

    slist.close();
    list<string>::iterator it = ssidList.begin();

    while (true) {
        RadioTap tap;

        Dot11::address_type ap        = "00:11:22:33:44:55";
        Dot11::address_type broadcast = "ff:ff:ff:ff:ff:ff";
        Dot11Beacon beacon(broadcast, ap);
        beacon.addr4(ap);
        beacon.ssid(*it);
        beacon.ds_parameter_set(10);
        beacon.supported_rates({ 1.0f, 5.5f, 11.0f });
        tap.inner_pdu(beacon);

        PacketSender sender(dev);
        sender.send(tap);
        usleep(10000);

        if (++it == ssidList.end())
            it = ssidList.begin();
    }
}

