#include "../Templates/template.h"

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    while (true) {
        cout << "\n1. Register\n2. Login\n3. View Buses\n4. Book Seat\n5. Show Tickets\nChoice: ";
        int ch;
        cin >> ch;

        if (ch == 1) {
            int code = REGISTRATION_CODE;
            write(sock, &code, sizeof(code));
            string name, pass;
            cout << "Enter username: "; cin >> name;
            cout << "Enter password: "; cin >> pass;
            write(sock, name.c_str(), name.size()+1);
            write(sock, pass.c_str(), pass.size()+1);
            int res; read(sock, &res, sizeof(res));
            cout << (res ? "Registered\n" : "Failed\n");

        } else if (ch == 2) {
            int code = LOGIN_CODE;
            write(sock, &code, sizeof(code));
            string name, pass;
            cout << "Enter username: "; cin >> name;
            cout << "Enter password: "; cin >> pass;
            write(sock, name.c_str(), name.size()+1);
            write(sock, pass.c_str(), pass.size()+1);
            int res; read(sock, &res, sizeof(res));
            cout << (res ? "Login success\n" : "Login failed\n");

        } else if (ch == 3) {
            int code = VIEW_BUSES_CODE;
            write(sock, &code, sizeof(code));
            char buffer[1024] = {0};
            read(sock, buffer, sizeof(buffer));
            cout << buffer;

        } else if (ch == 4) {
            int code = BOOK_SEAT_CODE;
            write(sock, &code, sizeof(code));
            string bus, user;
            int seat;
            cout << "Enter username: "; cin >> user;
            cout << "Enter bus name: "; cin >> bus;
            cout << "Enter seat number (0-based): "; cin >> seat;
            write(sock, bus.c_str(), bus.size()+1);
            write(sock, user.c_str(), user.size()+1);
            write(sock, &seat, sizeof(seat));
            int res; read(sock, &res, sizeof(res));
            cout << (res == BOOKING_SUCCESS_CODE ? "Booked!\n" : "Failed. Try another seat.\n");

        } else if (ch == 5) {
            int code = SHOW_TICKETS_CODE;
            write(sock, &code, sizeof(code));
            string name;
            cout << "Enter username: "; cin >> name;
            write(sock, name.c_str(), name.size()+1);
            char buffer[1024] = {0};
            read(sock, buffer, sizeof(buffer));
            cout << buffer;
        }
    }

    return 0;
}

