#pragma once
#include<iostream>
#include<string>
#include<vector>
#include<map>
#include<mutex>
#include<thread>
#include<algorithm>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
using namespace std;

#define REGISTRATION_CODE 101
#define LOGIN_CODE 102
#define VIEW_BUSES_CODE 103
#define BOOK_SEAT_CODE 104
#define BOOKING_SUCCESS_CODE 105
#define SHOW_TICKETS_CODE 106
#define SERVER_ERROR_CODE 500

void loadUsersFromFile();
void saveUsersToFile();
void loadBusesFromFile();
void saveBusesToFile();

struct User {
    string username;
    string password;
    vector<int> bookedSeats;
};

struct Bus {
    string busName;
    vector<bool> seats;
    int price;
    // Default constructor
    Bus() : busName(""), seats(0, false), price(0) {}
    
    Bus(string name, int totalSeats, int price) : busName(name), seats(totalSeats, false), price(price) {}
};

extern map<string, User> users;
extern map<string, Bus> buses;
extern mutex userMutex, busMutex;

void handleClient(int clientSocket);
void registerUser(int clientSocket);
void loginUser(int clientSocket);
void viewBuses(int clientSocket);
void bookSeat(int clientSocket);
void sendTickets(int clientSocket, const string& username);

