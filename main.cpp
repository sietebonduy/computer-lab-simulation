#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <queue>
#include <map>

using namespace std;

class Person {
    string name;
    string surname;

public:
    Person(string name_value, string surname_value) {
        name = name_value;
        surname = surname_value;
    }

    string getName() { return name; }
    string getSurname() { return surname; }
};

class Computer {
    int id;
    bool isFree;

public:
    Computer() {
        id = 0;
        isFree = true;
    }

    Computer(int id_value) {
        id = id_value;
        isFree = true;
    }

    int getId() { return id; }
    bool IsFree() { return isFree; }

    void setStatus(bool value) { isFree = value; }
};

class User : public Person {
    int using_time; // В минутах
    int compId;
    double payment;
    string time;
    string leaveTime;

public:
    User(string name_value, string surname_value, int using_time_value, double payment_value) : Person(name_value, surname_value) {
        using_time = using_time_value;
        payment = payment_value;
    }

    void setPayment(double price) { payment = price; }
    void setTime(string time_value) { time = time_value; }
    void setCompId(int id) { compId = id; }
    void setLeaveTime() {
        int hours, minutes;
        sscanf(time.c_str(), "%d:%d", &hours, &minutes);
        
        int totalMinutes = hours * 60 + minutes + using_time;
        
        int newHours = totalMinutes / 60 % 24;
        int newMinutes = totalMinutes % 60;
        
        ostringstream timeFormat;
        timeFormat << setw(2) << setfill('0') << newHours << ":" << setw(2) << setfill('0') << newMinutes;

        leaveTime = timeFormat.str();
    }
    
    string getTime() { return time; }
    int getUsingTime() { return using_time; }
    double getPayment() { return payment; }
    int getCompId() { return compId; }
    string getLeaveTime() { return leaveTime; }
};

int main() {
    queue<User> users;
    queue<User> activeUsers;
    queue<Computer> computers;
    
    map<int, unsigned> usage;
    
    int id;
    int maxUsingTime = 0;
    double totalPayment = 0;

    for (int i = 0; i < 10; ++i) {
        computers.push(Computer(i + 1));
    }
    
    for (int i = 1; i <= 10; ++i) {
        usage[i] = 0;
    }

    string line;
    ifstream peopleFile("/Users/skrach/Desktop/education/ООП/KR/KR/people.txt");
    
    ofstream result;
    result.open("/Users/skrach/Desktop/education/ООП/KR/KR/result.txt");

    if (!peopleFile.is_open()) {
        cerr << "Ошибка открытия файла с пользователями!" << endl;
        return 1;
    } else {
        while (getline(peopleFile, line)) {
            stringstream ss(line);
            string name;
            string surname;
            ss >> name >> surname;

            int usingTime = rand() % 60 + 5 ;
            double payment = rand() % 2;

            if (payment == 0) {
                payment = 200;
            } else {
                payment = 0;
            }
            
            if (maxUsingTime < usingTime) { maxUsingTime = usingTime; }

            totalPayment = totalPayment + payment;


            users.push(User(name, surname, usingTime, payment));
        }

        peopleFile.close();
    }

    // Симуляция дня с 9:00 до 18:00
    string currentTime;
    int hours = 9; int minutes = 0;
    for (int i = 0; i <= 540; ++i) {
        if (minutes == 60) {
            hours++;
            minutes = 0;
        }
        
        ostringstream time;
        time << setw(2) << setfill('0') << hours << ":" << setw(2) << setfill('0') << minutes;
        currentTime = time.str();

        int visitorChance = rand() % 100;
        
        if (visitorChance >= 0 && visitorChance <= 100) {
            if (computers.front().IsFree()) {
                User currentUser = users.front();
                Computer currentComp = computers.front();
                id = currentComp.getId();
                usage[id] = usage[id] + 1;
                
                currentUser.setCompId(id);
                currentUser.setTime(currentTime);
                currentUser.setLeaveTime();
                
                activeUsers.push(currentUser);
                users.pop();
                
                currentComp.setStatus(false);
                computers.push(currentComp);
                computers.pop();
                
                if (result.is_open()) {
                    result << "-*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*-" << endl;
                    result << "Номер компьютера: " << currentUser.getCompId() << endl;
                    result << "Имя: " << currentUser.getName() << endl;
                    result << "Фамилия: " << currentUser.getSurname() << endl;
                    result << "Время начала: " << currentUser.getTime() << endl;
                    result << "Время окончания: " << currentUser.getLeaveTime() << endl;
                    result << "Время сеанса: " << currentUser.getUsingTime() << " мин" << endl;
                    result << "Оплата: " << currentUser.getPayment() << endl;
                }
            } else {
                Computer currentComp = computers.front();
                computers.pop();
                computers.push(currentComp);
            }
        }
        
        for (int j = 0; j < activeUsers.size(); ++j) {
            User currentUser = activeUsers.front();
            
            if (currentUser.getLeaveTime() == currentTime) {
                id = currentUser.getCompId();
                
                for (int k = 0; k < computers.size(); ++k) {
                    if (computers.front().getId() == id) {
                        computers.front().setStatus(true);
                        break;
                    } else {
                        Computer currentComp = computers.front();
                        computers.pop();
                        computers.push(currentComp);
                    }
                }
            } else {
                activeUsers.pop();
                activeUsers.push(currentUser);
            }
        }
        
        minutes++;
    }
    
    int maxKey = 0;
    unsigned maxValue = 0;

    for (const auto& entry : usage) {
        if (entry.second > maxValue) {
            maxValue = entry.second;
            maxKey = entry.first;
        }
    }
    
    result << "-*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*-" << endl;
    result << endl << endl;
    result << "+--------------------------------------------------------+" << endl;
    result << "| Самый популярный компьютер: " << setw(27) << maxKey << "|" << endl;
    result << "| Самое большое время за компьютером: " << setw(19) << maxUsingTime << "|" << endl;
    result << "| Всего заплатили: " << setw(38) << totalPayment << "|" << endl;
    result << "+--------------------------------------------------------+" << endl;

    peopleFile.close();
    result.close();

    return 0;
}
