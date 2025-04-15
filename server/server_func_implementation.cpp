#include "../Templates/template.h"
#include <fstream>

using namespace std;

map<string, User> users;
map<string, Bus> buses = {
    {"Volvo", Bus("Volvo", 20, 500)},
    {"Express", Bus("Express", 30, 300)}};
mutex userMutex, busMutex;

void handleClient(int clientSocket)
{
    int code;
    while (read(clientSocket, &code, sizeof(code)) > 0)
    {
        switch (code)
        {
        case REGISTRATION_CODE:
            registerUser(clientSocket);
            break;
        case LOGIN_CODE:
            loginUser(clientSocket);
            break;
        case VIEW_BUSES_CODE:
            viewBuses(clientSocket);
            break;
        case BOOK_SEAT_CODE:
            bookSeat(clientSocket);
            break;
        case SHOW_TICKETS_CODE:
        {
            char name[100];
            read(clientSocket, name, sizeof(name));
            sendTickets(clientSocket, name);
            break;
        }
        default:
            break;
        }
    }
    close(clientSocket);
}

// void registerUser(int clientSocket) {
//     char name[100], pass[100];
//     read(clientSocket, name, sizeof(name));
//     read(clientSocket, pass, sizeof(pass));
//     lock_guard<mutex> lock(userMutex);
//     users[name] = {name, pass};
//     int success = 1;
//     write(clientSocket, &success, sizeof(success));
// }
string readString(int sock)
{
    string result;
    char ch;
    while (read(sock, &ch, 1) > 0 && ch != '\0')
    {
        result += ch;
    }
    return result;
}

void registerUser(int clientSocket)
{
    string name = readString(clientSocket);
    string pass = readString(clientSocket);
    lock_guard<mutex> lock(userMutex);
    users[name] = {name, pass};
    saveUsersToFile();
    int success = 1;
    write(clientSocket, &success, sizeof(success));
}

void loginUser(int clientSocket)
{
    // char name[100], pass[100];
    // read(clientSocket, name, sizeof(name));
    // read(clientSocket, pass, sizeof(pass));
    string name = readString(clientSocket);
    string pass = readString(clientSocket);
    lock_guard<mutex> lock(userMutex);
    int success = (users.count(name) && users[name].password == pass);
    write(clientSocket, &success, sizeof(success));
}

void viewBuses(int clientSocket)
{
    string result = "Available Buses:\n";
    for (auto &[name, bus] : buses)
    {
        result += "-----------------------------\n";
        result += "Bus: " + name + " | Price: Rs." + to_string(bus.price) + "\n";
        result += "Seat Matrix (X = Booked, O = Free):\n";
        int seatCount = bus.seats.size();
        for (int i = 0; i < seatCount; ++i)
        {
            result += (bus.seats[i] ? "X " : "O ");
            if ((i + 1) % 5 == 0)
                result += "\n"; // 5 seats per row
        }
        result += "\n-----------------------------\n";
    }
    send(clientSocket, result.c_str(), result.size(), 0);
}

void bookSeat(int clientSocket)
{
    char busname[100], username[100];
    int seat;
    read(clientSocket, busname, sizeof(busname));
    read(clientSocket, username, sizeof(username));
    read(clientSocket, &seat, sizeof(seat));
    lock_guard<mutex> lock(busMutex);

    // if (buses[busname].seats[seat] == false) {
    //     buses[busname].seats[seat] = true;
    //     users[username].bookedSeats.push_back(seat);
    //     int success = BOOKING_SUCCESS_CODE;
    //     write(clientSocket, &success, sizeof(success));
    // } else {
    //     int fail = SERVER_ERROR_CODE;
    //     write(clientSocket, &fail, sizeof(fail));
    // }
    // Check if the bus exists
    if (buses.find(busname) == buses.end())
    {
        int fail = SERVER_ERROR_CODE;
        write(clientSocket, &fail, sizeof(fail));
        return;
    }

    // Check if seat number is valid
    // if (seat < 0 || seat >= buses[busname].seats.size())
    if (seat < 0 || seat >= static_cast<int>(buses[busname].seats.size()))

    {
        int fail = SERVER_ERROR_CODE;
        write(clientSocket, &fail, sizeof(fail));
        return;
    }

    if (!buses[busname].seats[seat])
    {
        buses[busname].seats[seat] = true;

        // In case the user doesn’t exist, add them
        if (users.find(username) == users.end())
        {
            users[username] = User{username, {}, {}};
        }

        users[username].bookedSeats.push_back(seat);
        saveUsersToFile();
        saveBusesToFile();

        int success = BOOKING_SUCCESS_CODE;
        write(clientSocket, &success, sizeof(success));
    }
    else
    {
        int fail = SERVER_ERROR_CODE;
        write(clientSocket, &fail, sizeof(fail));
    }
    cout << "[BOOKING] User: " << username << " | Bus: " << busname << " | Seat: " << seat << endl;
}

void sendTickets(int clientSocket, const string &username)
{
    string msg = "Your Booked Seats:\n";
    for (int seat : users[username].bookedSeats)
    {
        msg += "Seat " + to_string(seat) + "\n";
    }
    send(clientSocket, msg.c_str(), msg.size(), 0);
}
void loadUsersFromFile()
{
    ifstream fin("users.txt");
    string uname, pass;
    int n;
    while (fin >> uname >> pass >> n)
    {
        User u;
        u.username = uname;
        u.password = pass;
        for (int i = 0; i < n; ++i)
        {
            int seat;
            fin >> seat;
            u.bookedSeats.push_back(seat);
        }
        users[uname] = u;
    }
    fin.close();
}

void saveUsersToFile()
{
    ofstream fout("users.txt");
    for (auto &[uname, user] : users)
    {
        fout << uname << " " << user.password << " " << user.bookedSeats.size();
        for (int seat : user.bookedSeats)
            fout << " " << seat;
        fout << "\n";
    }
    fout.close();
}

void loadBusesFromFile()
{
    ifstream fin("buses.txt");
    string name;
    int totalSeats, price;
    while (fin >> name >> totalSeats >> price)
    {
        Bus b(name, totalSeats, price);
        for (int i = 0; i < totalSeats; ++i)
        {
            int val;
            fin >> val;
            b.seats[i] = (val == 1);
        }
        buses[name] = b;
    }
    fin.close();
}

void saveBusesToFile()
{
    ofstream fout("buses.txt");
    for (auto &[name, bus] : buses)
    {
        fout << name << " " << bus.seats.size() << " " << bus.price;
        for (bool s : bus.seats)
            fout << " " << s;
        fout << "\n";
    }
    fout.close();
}
