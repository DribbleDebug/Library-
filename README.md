# Library Management System – CS253 Assignment

This is a **Library Management System (LMS)** built using **Object‐Oriented Programming (OOP) concepts** in C++. The system demonstrates the principles of **encapsulation**, **inheritance**, **polymorphism**, and **abstraction**, supporting three user roles: **Students**, **Faculty**, and **Librarians**.

---

## Roles and Permissions

### Students
- Borrow up to **3** books at a time.
- Must return books within **15 days**.
- Overdue fines: **10 rupees per day**.
- Cannot add or remove books/users.

### Faculty
- Borrow up to **5** books at a time.
- Must return books within **30 days**.
- **No overdue fines**, but cannot borrow new books if any are overdue by more than **60 days**.

### Librarians
- Manage books (add, remove, or update).
- Manage users (add or remove).
- Cannot borrow books.

---

## Books
- Attributes: **title**, **author**, **publisher**, **year**, **ISBN**, and **status**.
- Begin with at least **5** books.
- A book can be borrowed only if its status is **“Available.”**

---

## Account
Each user has an **Account** that tracks:
- Currently borrowed books and their due dates.
- Overdue calculation and fines (if applicable).
- Borrowing history.

---

## System Constraints and Rules

### 1. File Persistence  
   - All data is saved to files (e.g., `books.txt`, `users.txt`) and reloaded on startup.

### 2. Borrowing Rules  
   - Students or faculty can borrow books according to their limits.
   - If a user exceeds the borrowing limit or has unpaid fines, borrowing is denied.

### 3. Fines  
   - Students: **10 rupees/day** overdue fine.  
   - Faculty: **no fine**, but restricted from borrowing if a book is more than 60 days overdue.

### 4. Returning and Updating  
   - Upon return, a book’s status is updated to **“Available.”**
   - Overdue days are calculated if returned after the allowed period (15 days for students, 30 days for faculty).
   - Fines must be cleared to resume borrowing.

---

## Implementation Details

### Classes
1. **User (Base Class)**  
   - Derived classes: **Student**, **Faculty**, **Librarian**.  
2. **Book**  
   - Tracks basic book info and current status.  
3. **Account**  
   - Maintains borrowed books, fines, and borrowing history.  
4. **Library**  
   - Coordinates users, books, and file I/O.

### Polymorphism
- **borrowBook()** and **returnBook()** are virtual in `User` and overridden by `Student` and `Faculty` to apply different rules.

### File Handling
- **books.txt** and **users.txt** store serialized book/user data.
- The program reads these files on startup and writes updated data on exit.

---

## Usage

### 1. Compilation
```bash
g++ main.cpp -o LMS
```

### 2. Run the Program
```bash
./LMS
