#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <ctime>
#include <iomanip>
#include <algorithm>
using namespace std;

enum BookStatus { AVAILABLE, BORROWED, RESERVED };

int getTodayAsInteger() {
    time_t now = time(nullptr);
    return static_cast<int>(now / (60 * 60 * 24));
}

class Book {
private:
    int id;
    string title;
    string author;
    string publisher;
    int year;
    string isbn;
    BookStatus status;

public:
    Book() : id(0), year(0), status(AVAILABLE) {}

    Book(int _id, const string &_title, const string &_author,
         const string &_publisher, int _year, const string &_isbn)
        : id(_id), title(_title), author(_author), publisher(_publisher),
          year(_year), isbn(_isbn), status(AVAILABLE) {}

    int getId() const { return id; }
    void setId(int _id) { id = _id; }

    string getTitle() const { return title; }
    void setTitle(const string &t) { title = t; }

    string getAuthor() const { return author; }
    void setAuthor(const string &a) { author = a; }

    string getPublisher() const { return publisher; }
    void setPublisher(const string &p) { publisher = p; }

    int getYear() const { return year; }
    void setYear(int y) { year = y; }

    string getISBN() const { return isbn; }
    void setISBN(const string &i) { isbn = i; }

    BookStatus getStatus() const { return status; }
    void setStatus(BookStatus s) { status = s; }

    void printDetails() const {
        cout << "Book ID: " << id << "\nTitle: " << title
             << "\nAuthor: " << author << "\nPublisher: " << publisher
             << "\nYear: " << year << "\nISBN: " << isbn
             << "\nStatus: "
             << (status == AVAILABLE ? "Available"
                                     : (status == BORROWED ? "Borrowed"
                                                           : "Reserved"))
             << "\n---------------------------" << endl;
    }

    string serialize() const {
        ostringstream oss;
        oss << id << "," << title << "," << author << "," << publisher << ","
            << year << "," << isbn << "," << status;
        return oss.str();
    }

    static Book deserialize(const string &csvLine) {
        istringstream iss(csvLine);
        vector<string> fields;
        string token;
        while (getline(iss, token, ',')) {
            fields.push_back(token);
        }
        if (fields.size() < 7) {
            return Book();
        }
        Book b(stoi(fields[0]), fields[1], fields[2], fields[3],
               stoi(fields[4]), fields[5]);
        b.setStatus(static_cast<BookStatus>(stoi(fields[6])));
        return b;
    }
};

class Account {
private:
    map<int, int> borrowedBooks;
    vector<int> borrowingHistory;
    double fineAmount;

public:
    Account() : fineAmount(0) {}

    void addBorrowedBook(int bookId, int dueDay) {
        borrowedBooks[bookId] = dueDay;
    }

    void removeBorrowedBook(int bookId) {
        borrowedBooks.erase(bookId);
    }

    const map<int, int> &getBorrowedBooks() const {
        return borrowedBooks;
    }

    void addToHistory(int bookId) {
        borrowingHistory.push_back(bookId);
    }

    const vector<int> &getHistory() const {
        return borrowingHistory;
    }

    void addFine(double amt) {
        fineAmount += amt;
    }

    void clearFine() {
        fineAmount = 0;
    }

    double getFine() const {
        return fineAmount;
    }

    string serialize() const {
        ostringstream oss;
        oss << fineAmount;
        for (auto &entry : borrowedBooks) {
            oss << "," << entry.first << ":" << entry.second;
        }
        oss << ",H:";
        for (size_t i = 0; i < borrowingHistory.size(); i++) {
            oss << borrowingHistory[i];
            if (i < borrowingHistory.size() - 1) {
                oss << "-";
            }
        }
        return oss.str();
    }

    static Account deserialize(const string &data) {
        Account acc;
        istringstream iss(data);
        string token;
        getline(iss, token, ',');
        acc.fineAmount = stod(token);
        while (getline(iss, token, ',')) {
            if (token.rfind("H:", 0) == 0) {
                string histStr = token.substr(2);
                istringstream his(histStr);
                string bid;
                while (getline(his, bid, '-')) {
                    if (!bid.empty()) {
                        acc.borrowingHistory.push_back(stoi(bid));
                    }
                }
            } else {
                size_t pos = token.find(":");
                if (pos != string::npos) {
                    int bookId = stoi(token.substr(0, pos));
                    int dueDay = stoi(token.substr(pos + 1));
                    acc.borrowedBooks[bookId] = dueDay;
                }
            }
        }
        return acc;
    }

    void printAccount() const {
        cout << "Fine: " << fineAmount << "\nBorrowed Books:\n";
        for (auto &entry : borrowedBooks) {
            cout << "  Book ID " << entry.first << ", Due Day: " << entry.second
                 << "\n";
        }
        cout << "Borrowing History: ";
        for (auto &h : borrowingHistory) {
            cout << h << " ";
        }
        cout << "\n";
    }
};

class Library;

class User {
protected:
    int id;
    string name;
    Account account;

public:
    User() : id(0) {}
    User(int _id, const string &_name) : id(_id), name(_name) {}
    virtual ~User() {}

    int getId() const { return id; }
    string getName() const { return name; }

    Account &getAccount() { return account; }

    virtual void borrowBook(Library &lib, int bookId, int currentDay) = 0;
    virtual void returnBook(Library &lib, int bookId, int currentDay) = 0;

    virtual void displayDetails() const {
        cout << "User ID: " << id << "\nName: " << name << "\n";
    }

    virtual string serialize() const {
        ostringstream oss;
        oss << id << "," << name << "," << account.serialize();
        return oss.str();
    }

    virtual string getType() const = 0;
};

class Student : public User {
private:
    static const int BORROW_LIMIT = 3;
    static const int BORROW_PERIOD = 15;
    static const double FINE_RATE;

public:
    Student(int _id, const string &_name) : User(_id, _name) {}

    virtual void borrowBook(Library &lib, int bookId, int currentDay) override;
    virtual void returnBook(Library &lib, int bookId, int currentDay) override;

    int getBorrowLimit() const { return BORROW_LIMIT; }
    int getBorrowPeriod() const { return BORROW_PERIOD; }
    double getFineRate() const { return FINE_RATE; }

    virtual string getType() const override { return "Student"; }

    virtual void displayDetails() const override {
        cout << "Role: Student\n";
        User::displayDetails();
        account.printAccount();
    }
};

const double Student::FINE_RATE = 10.0;

class Faculty : public User {
private:
    static const int BORROW_LIMIT = 5;
    static const int BORROW_PERIOD = 30;

public:
    Faculty(int _id, const string &_name) : User(_id, _name) {}

    virtual void borrowBook(Library &lib, int bookId, int currentDay) override;
    virtual void returnBook(Library &lib, int bookId, int currentDay) override;

    int getBorrowLimit() const { return BORROW_LIMIT; }
    int getBorrowPeriod() const { return BORROW_PERIOD; }

    virtual string getType() const override { return "Faculty"; }

    virtual void displayDetails() const override {
        cout << "Role: Faculty\n";
        User::displayDetails();
        account.printAccount();
    }
};

class Librarian : public User {
public:
    Librarian(int _id, const string &_name) : User(_id, _name) {}

    virtual void borrowBook(Library &, int, int) override {
        cout << "Librarian accounts cannot borrow books." << endl;
    }

    virtual void returnBook(Library &, int, int) override {
        cout << "Librarian accounts cannot return books." << endl;
    }

    virtual string getType() const override { return "Librarian"; }

    virtual void displayDetails() const override {
        cout << "Role: Librarian\n";
        User::displayDetails();
    }
};

class Library {
private:
    vector<Book> books;
    vector<User *> users;

public:
    Library() {}
    ~Library() {
        for (auto *u : users) {
            delete u;
        }
    }

    void addBook(const Book &b) {
        books.push_back(b);
    }

    void removeBook(int bookId) {
        auto it = remove_if(books.begin(), books.end(),
                            [bookId](const Book &bk) { return bk.getId() == bookId; });
        if (it != books.end()) {
            books.erase(it, books.end());
            cout << "Book removed successfully.\n";
        } else {
            cout << "Book ID not found.\n";
        }
    }

    int getNextBookId() const {
        int highest = 0;
        for (auto &bk : books) {
            if (bk.getId() > highest) {
                highest = bk.getId();
            }
        }
        return highest + 1;
    }

    Book *findBook(int bookId) {
        for (auto &bk : books) {
            if (bk.getId() == bookId) {
                return &bk;
            }
        }
        return nullptr;
    }

    void addUser(User *user) {
        users.push_back(user);
    }

    User *findUser(const string &userIdStr) {
        int userId = stoi(userIdStr);
        for (auto *u : users) {
            if (u->getId() == userId) {
                return u;
            }
        }
        return nullptr;
    }

    void removeUser(int userId) {
        auto it = remove_if(users.begin(), users.end(),
                            [userId](User *u) { return u->getId() == userId; });
        if (it != users.end()) {
            for (auto itr = it; itr != users.end(); ++itr) {
                delete *itr;
            }
            users.erase(it, users.end());
            cout << "User removed successfully.\n";
        } else {
            cout << "User not found.\n";
        }
    }

    void displayBooks() const {
        for (auto &bk : books) {
            bk.printDetails();
        }
    }

    void displayUsers() const {
        for (auto &u : users) {
            u->displayDetails();
            cout << "------------------------\n";
        }
    }

    void loadBooks(const string &filename) {
        ifstream fin(filename);
        if (!fin) {
            cout << "Books file not found. Using defaults.\n";
            return;
        }
        books.clear();
        string line;
        while (getline(fin, line)) {
            Book b = Book::deserialize(line);
            books.push_back(b);
        }
        fin.close();
    }

    void saveBooks(const string &filename) {
        ofstream fout(filename);
        for (auto &bk : books) {
            fout << bk.serialize() << "\n";
        }
        fout.close();
    }

    void loadUsers(const string &filename) {
        ifstream fin(filename);
        if (!fin) {
            cout << "Users file not found. Using defaults.\n";
            return;
        }
        for (auto *u : users) {
            delete u;
        }
        users.clear();
        string line;
        while (getline(fin, line)) {
            istringstream iss(line);
            string type, idStr, nm, accData;
            getline(iss, type, ',');
            getline(iss, idStr, ',');
            getline(iss, nm, ',');
            getline(iss, accData);
            int userId = stoi(idStr);
            User *user = nullptr;
            if (type == "Student") {
                user = new Student(userId, nm);
            } else if (type == "Faculty") {
                user = new Faculty(userId, nm);
            } else if (type == "Librarian") {
                user = new Librarian(userId, nm);
            }
            if (user) {
                user->getAccount() = Account::deserialize(accData);
                users.push_back(user);
            }
        }
        fin.close();
    }

    void saveUsers(const string &filename) {
        ofstream fout(filename);
        for (auto *u : users) {
            fout << u->getType() << "," << u->serialize() << "\n";
        }
        fout.close();
    }
};

void Student::borrowBook(Library &lib, int bookId, int currentDay) {
    if (account.getFine() > 0) {
        cout << "You have outstanding fines. Clear them before borrowing.\n";
        return;
    }
    if (account.getBorrowedBooks().size() >= (size_t)BORROW_LIMIT) {
        cout << "Borrowing limit reached.\n";
        return;
    }
    Book *bk = lib.findBook(bookId);
    if (!bk) {
        cout << "Book not found.\n";
        return;
    }
    if (bk->getStatus() != AVAILABLE) {
        cout << "That book is not available.\n";
        return;
    }
    int due = currentDay + BORROW_PERIOD;
    account.addBorrowedBook(bookId, due);
    bk->setStatus(BORROWED);
    cout << "Book borrowed. Due on day " << due << ".\n";
}

void Student::returnBook(Library &lib, int bookId, int currentDay) {
    Book *bk = lib.findBook(bookId);
    if (!bk) {
        cout << "Book not found.\n";
        return;
    }
    auto &borrowed = account.getBorrowedBooks();
    if (borrowed.find(bookId) == borrowed.end()) {
        cout << "You didn't borrow this book.\n";
        return;
    }
    int due = borrowed.at(bookId);
    if (currentDay > due) {
        int overdueDays = currentDay - due;
        double penalty = overdueDays * getFineRate();
        account.addFine(penalty);
        cout << "Late return. Overdue by " << overdueDays
             << " day(s). Fine: " << penalty << " rupees.\n";
    } else {
        cout << "Returned on time.\n";
    }
    account.removeBorrowedBook(bookId);
    account.addToHistory(bookId);
    bk->setStatus(AVAILABLE);
}

void Faculty::borrowBook(Library &lib, int bookId, int currentDay) {
    if (account.getBorrowedBooks().size() >= (size_t)BORROW_LIMIT) {
        cout << "Borrow limit reached.\n";
        return;
    }
    for (auto &entry : account.getBorrowedBooks()) {
        if (currentDay - entry.second > 60) {
            cout << "Cannot borrow new books due to an item overdue > 60 days.\n";
            return;
        }
    }
    Book *bk = lib.findBook(bookId);
    if (!bk) {
        cout << "Book not found.\n";
        return;
    }
    if (bk->getStatus() != AVAILABLE) {
        cout << "That book is not available.\n";
        return;
    }
    int due = currentDay + BORROW_PERIOD;
    account.addBorrowedBook(bookId, due);
    bk->setStatus(BORROWED);
    cout << "Book borrowed. Due on day " << due << ".\n";
}

void Faculty::returnBook(Library &lib, int bookId, int) {
    Book *bk = lib.findBook(bookId);
    if (!bk) {
        cout << "Book not found.\n";
        return;
    }
    auto &borrowed = account.getBorrowedBooks();
    if (borrowed.find(bookId) == borrowed.end()) {
        cout << "You didn't borrow this book.\n";
        return;
    }
    cout << "Book returned.\n";
    account.removeBorrowedBook(bookId);
    account.addToHistory(bookId);
    bk->setStatus(AVAILABLE);
}

int main() {
    Library lib;
    int today = getTodayAsInteger();
    lib.loadBooks("books.txt");
    lib.loadUsers("users.txt");
    if (!lib.findBook(1)) {
        lib.addBook(Book(1, "Deep Learning",
                         "Ian Goodfellow", "MIT Press", 2016,
                         "9780262035613"));
        lib.addBook(Book(2, "Python Crash Course", "Eric Matthes",
                         "No Starch Press", 2019, "978159327928"));
        lib.addBook(Book(3, "The Pragmatic Programmer", "Andy Hunt", "Addison-Wesley", 1999,
                         "9780201616224"));
        lib.addBook(Book(4, "Computer Vision", "Richard Szeliski",
                         "Springer", 2022, "9783030343729"));
        lib.addBook(Book(5, "Artificial Intelligence",
                         "Stuart Russell", "Prentice Hall", 2020,
                         "9780134610993"));
        lib.addBook(Book(6, "The Art of Computer Programming", "Donald Knuth",
                         "Addison-Wesley", 2011, "9780201896830"));
        lib.addBook(Book(7, "Structure and Interpretation of Computer Programs", "Harold Abelson",
                         "MIT Press", 1996, "9780262510875"));
        lib.addBook(Book(8, "Code Complete", "Steve McConnell", "Microsoft Press",
                         2004, "9780735619678"));
        lib.addBook(Book(9, "Introduction to the Theory of Computation", "Michael Sipser",
                         "Cengage", 2012, "9781133187790"));
        lib.addBook(Book(10, "Computer Architecture", "David A. Patterson", "Morgan Kaufmann",
                         2017, "9780128119051"));
    }
    if (!lib.findUser("101")) {
        lib.addUser(new Student(101, "Dharvi"));
        lib.addUser(new Student(102, "Deepak"));
        lib.addUser(new Student(103, "Diksha"));
        lib.addUser(new Student(104, "Tushar"));
        lib.addUser(new Student(105, "Saksham"));
        lib.addUser(new Faculty(201, "Professor X"));
        lib.addUser(new Faculty(202, "Professor Y"));
        lib.addUser(new Faculty(203, "Professor Z"));
        lib.addUser(new Librarian(301, "Librarian A"));
    }
    while (true) {
        cout << "\nEnter User ID to log in (or 'exit' to quit): ";
        string userIdStr;
        cin >> userIdStr;
        if (userIdStr == "exit") {
            cout << "Saving data and exiting...\n";
            break;
        }
        User *user = lib.findUser(userIdStr);
        if (!user) {
            cout << "User not found.\n";
            continue;
        }
        cout << "Logged in as " << user->getName() << " ("
             << user->getType() << ")\n";
        if (user->getType() == "Student" || user->getType() == "Faculty") {
            cout << "Borrowed books: "
                 << user->getAccount().getBorrowedBooks().size() << "\n";
            cout << "Outstanding fines: "
                 << user->getAccount().getFine() << " rupees\n";
            int choice;
            while (true) {
                cout << "\n1. Borrow Book\n2. Return Book\n3. Pay Fine\n"
                     << "4. View Account\n5. Logout\nChoice: ";
                cin >> choice;
                if (choice == 1) {
                    int bkId;
                    cout << "Enter Book ID: ";
                    cin >> bkId;
                    user->borrowBook(lib, bkId, today);
                } else if (choice == 2) {
                    int bkId;
                    cout << "Enter Book ID: ";
                    cin >> bkId;
                    user->returnBook(lib, bkId, today);
                } else if (choice == 3) {
                    double currentFine = user->getAccount().getFine();
                    if (currentFine <= 0) {
                        cout << "No outstanding fines.\n";
                    } else {
                        cout << "Paying " << currentFine << " rupees...\n";
                        user->getAccount().clearFine();
                        cout << "Fines cleared.\n";
                    }
                } else if (choice == 4) {
                    user->displayDetails();
                } else if (choice == 5) {
                    cout << "Logging out...\n";
                    break;
                } else {
                    cout << "Invalid choice.\n";
                }
            }
        }
        else if (user->getType() == "Librarian") {
            int choice;
            while (true) {
                cout << "\n1. Display Books\n2. Display Users\n3. Add Book\n"
                     << "4. Remove Book\n5. Add User\n6. Remove User\n"
                     << "7. Logout\nChoice: ";
                cin >> choice;
                if (choice == 1) {
                    lib.displayBooks();
                } else if (choice == 2) {
                    lib.displayUsers();
                } else if (choice == 3) {
                    string title, author, pub, isbn;
                    int year;
                    cout << "Title: ";
                    cin.ignore();
                    getline(cin, title);
                    cout << "Author: ";
                    getline(cin, author);
                    cout << "Publisher: ";
                    getline(cin, pub);
                    cout << "Year: ";
                    cin >> year;
                    cout << "ISBN: ";
                    cin >> isbn;
                    int newId = lib.getNextBookId();
                    lib.addBook(Book(newId, title, author, pub, year, isbn));
                    cout << "Book added.\n";
                } else if (choice == 4) {
                    int bkId;
                    cout << "Enter Book ID to remove: ";
                    cin >> bkId;
                    lib.removeBook(bkId);
                } else if (choice == 5) {
                    int uid;
                    string nm, role;
                    cout << "Enter User ID: ";
                    cin >> uid;
                    cout << "Name: ";
                    cin.ignore();
                    getline(cin, nm);
                    cout << "Role (Student/Faculty/Librarian): ";
                    cin >> role;
                    if (role == "Student" || role == "student") {
                        lib.addUser(new Student(uid, nm));
                    } else if (role == "Faculty" || role == "faculty") {
                        lib.addUser(new Faculty(uid, nm));
                    } else if (role == "Librarian" || role == "librarian") {
                        lib.addUser(new Librarian(uid, nm));
                    } else {
                        cout << "Invalid role.\n";
                    }
                    cout << "User added.\n";
                } else if (choice == 6) {
                    int removeId;
                    cout << "Enter User ID to remove: ";
                    cin >> removeId;
                    lib.removeUser(removeId);
                } else if (choice == 7) {
                    cout << "Logging out...\n";
                    break;
                } else {
                    cout << "Invalid choice.\n";
                }
            }
        }
    }
    lib.saveBooks("books.txt");
    lib.saveUsers("users.txt");
    cout << "Library data saved. Goodbye.\n";
    return 0;
}
