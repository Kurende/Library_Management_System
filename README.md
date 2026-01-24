# Library Management System

A comprehensive library management system designed for rural schools in the Makhutswe/Lepelle Circuits. Built with C++ and Qt Framework.

## 📋 Table of Contents
- [Features](#features)
- [System Requirements](#system-requirements)
- [Installation](#installation)
- [Building from Source](#building-from-source)
- [Usage](#usage)
- [Project Structure](#project-structure)
- [Database Schema](#database-schema)
- [User Roles](#user-roles)
- [Contributing](#contributing)

---

## ✨ Features

### Book Management
- Add, update, and delete books
- Track multiple copies with unique book codes
- Support for same ISBN with different book codes
- Search and filter books by title, author, grade, subject
- View book availability status
- Mark books as lost

### Learner Management
- Add, update learner information
- View learner profiles
- Track learner borrowing history
- Calculate fees for unreturned books
- Search and filter learners

### Transaction Management
- Borrow books with automatic due date calculation (November 28)
- Return books with overdue tracking
- Prevent borrowing if learner has overdue books
- Mark books as lost during return process
- View transaction history with filters

### Reports & Analytics
- Generate borrow reports
- Generate return reports
- Print reports or save as PDF
- Dashboard with statistics:
  - Total books, available books, borrowed books
  - Total learners, active learners
  - Overdue books count
  - Recent transactions

### Security
- User authentication with encrypted passwords
- Role-based access control (Admin, Librarian, Finance)
- Password recovery with security questions
- Secure password storage using SHA-256

---

## 💻 System Requirements

### Minimum Requirements
- **Operating System**: Windows 10/11, Linux, macOS
- **RAM**: 2 GB
- **Storage**: 100 MB free space
- **Display**: 1280x720 resolution

### Development Requirements
- **Qt Framework**: Qt 5.12 or higher
- **Compiler**: 
  - GCC 7+ (Linux)
  - MSVC 2017+ (Windows)
  - Clang (macOS)
- **CMake**: 3.5 or higher
- **SQLite3**: Included with Qt

---

## 📦 Installation

### Pre-built Binary (Recommended for Users)
1. Download the latest release from the releases page
2. Extract the archive
3. Run `LibraryManagementSystem.exe` (Windows) or `./LibraryManagementSystem` (Linux/macOS)

### First-Time Setup
1. Launch the application
2. Register the first admin user
3. The database will be created automatically
4. Start adding books and learners!

---

## 🔨 Building from Source

### Prerequisites
```bash
# Ubuntu/Debian
sudo apt-get install qt5-default qtbase5-dev qt5-qmake cmake build-essential

# Fedora
sudo dnf install qt5-qtbase-devel cmake gcc-c++

# macOS (using Homebrew)
brew install qt@5 cmake

# Windows
# Download and install Qt from https://www.qt.io/download
# Install Visual Studio or MinGW
```

### Build Steps

#### Linux/macOS
```bash
# Clone the repository
git clone https://github.com/yourusername/library-management-system.git
cd library-management-system

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build
make -j$(nproc)

# Run
./bin/LibraryManagementSystem
```

#### Windows (Visual Studio)
```powershell
# Open Qt command prompt or set Qt environment variables

# Clone the repository
git clone https://github.com/kurende/library-management-system.git
cd library-management-system

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake -G "Visual Studio 16 2019" ..

# Build
cmake --build . --config Release

# Run
.\bin\Release\LibraryManagementSystem.exe
```

#### Windows (MinGW)
```bash
mkdir build && cd build
cmake -G "MinGW Makefiles" ..
mingw32-make
.\bin\LibraryManagementSystem.exe
```

---

## 📖 Usage

### Initial Login
The system starts with no users. You must register the first admin account:

1. Click **Register**
2. Fill in all required fields
3. Select **Admin** as the role
4. Complete security question
5. Click **Register**
6. Login with your credentials

### Adding Books
1. Navigate to **Books** → **Add/View Books**
2. Enter book details:
   - **Book Code**: Unique identifier (e.g., 000037)
   - **ISBN**: Can be shared by multiple copies
   - **Title, Author, Subject, Grade**
   - **Price**: For calculating lost book fees
3. Click **Confirm Add**

**Tip**: Use **Copy from ISBN** button to auto-fill details for additional copies

### Adding Learners
1. Navigate to **Learners** → **Add/View Learners**
2. Enter learner information
3. Click **Add Learner**

### Borrowing Books
1. Navigate to **Transactions** → **Borrow Book**
2. Enter Learner ID and click **Find Learner**
3. Enter Book Code and click **Find Book**
4. Verify details and click **Confirm Borrow**
5. Due date is automatically set to November 28

### Returning Books
1. Navigate to **Transactions** → **Return Book**
2. Search by Learner ID or Book Code
3. Select the transaction from the table
4. Click **Process Return**
   - Or click **Mark as Lost** if book is not returned

### Generating Reports
1. Navigate to **Reports**
2. Enter Learner ID and click **Load Data**
3. Select report type (Borrow/Return)
4. Click **Generate Report**
5. **Print** or **Save as PDF**

---

## 📁 Project Structure

```
Library_Management_System/
│
├── main.cpp                          # Application entry point
├── CMakeLists.txt                    # Build configuration
│
├── ui/
│   ├── MainWindow.ui                 # Qt Designer UI file
│   ├── MainWindow.cpp               # Main window implementation
│   └── MainWindow.h                 # Main window header
│
├── database/
│   ├── DatabaseManager.cpp          # Database operations
│   ├── DatabaseManager.h
│   └── library_system.db            # SQLite database (created at runtime)
│
├── auth/
│   ├── AuthManager.cpp              # Authentication logic
│   └── AuthManager.h
│
├── models/
│   ├── User.cpp                     # User entity
│   ├── User.h
│   ├── Learner.cpp                  # Learner entity
│   ├── Learner.h
│   ├── Book.cpp                     # Book entity
│   ├── Book.h
│   ├── Payments.cpp                     # Payments entity
│   ├── Payments.h
│   ├── PaymentsItem.cpp                     # PaymentsItem entity
│   ├── PaymentsItem.h
│   ├── Transaction.cpp              # Transaction entity
│   └── Transaction.h
│
├── utils/
│   ├── Encryption.cpp               # Password encryption
│   └── Encryption.h
│
├── resources/
│   └── icons.qrc                    # Resource file for icons
│
└── documentation/
    ├── Developer Guide_Library Managem.md
    └── README.md                    # This file
```

---

## 🗄️ Database Schema

### Tables

#### users
- `id` (PRIMARY KEY)
- `username` (UNIQUE)
- `password_hash`
- `name`, `surname`, `email`, `contact_no`, `school_name`
- `role` (Admin/Librarian/Finance)
- `security_question`, `security_answer`
- `created_at`

#### learners
- `id` (PRIMARY KEY)
- `name`, `surname`, `grade`
- `date_of_birth`, `contact_no`
- `created_at`

#### books
- `id` (PRIMARY KEY)
- `book_code` (UNIQUE) - Physical book identifier
- `isbn` - Can be shared by multiple copies
- `title`, `author`, `subject`, `grade`
- `price`, `status` (Available/Borrowed/Lost)
- `created_at`

#### transactions
- `id` (PRIMARY KEY)
- `learner_id` (FOREIGN KEY)
- `book_id` (FOREIGN KEY)
- `borrow_date`, `due_date`, `return_date`
- `status` (Active/Returned/Lost)
- `created_at`

---

## 👥 User Roles

### Admin
- **Full system access**
- Create/delete users
- Manage all books and learners
- View all reports
- System settings

### Librarian
- Add/update books
- Add/update learners
- Process borrow/return transactions
- Generate reports
- Cannot manage users

### Finance
- View reports
- Track unreturned books and fees
- Cannot modify books/learners
- Read-only access to transactions

---

## 🔒 Security Features

- **Password Hashing**: SHA-256 encryption
- **Security Questions**: For password recovery
- **Role-Based Access**: Restricts features by user role
- **Session Management**: Automatic logout
- **Input Validation**: Prevents SQL injection and invalid data

---

## 🚀 Business Rules

### Borrowing
- Learner must exist in database
- Book must be available (not borrowed/lost)
- Learner cannot have overdue books
- Due date: November 28 of borrow year (or next year if borrowed after Nov 28)

### Returning
- Transaction must be active
- Overdue books are flagged automatically
- Lost books charge learner the book price

### Book Management
- Book codes must be unique
- Multiple books can share same ISBN (copies)
- Only available books can be borrowed

---

## 🐛 Troubleshooting

### Database Connection Error
- Ensure write permissions in application directory
- Database file will be created automatically on first run

### UI Elements Not Showing
- Check Qt version compatibility
- Ensure all required Qt modules are installed

### Build Errors
- Verify Qt installation path
- Check CMake can find Qt (`CMAKE_PREFIX_PATH`)
- Ensure C++17 compiler support

---

## 📝 License

This project is licensed under the MIT License - see LICENSE file for details.

---

## 👨‍💻 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the project
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

---


## 🙏 Acknowledgments

- Qt Framework Community
- All contributors

---

**Version**: 1.0.0  
**Last Updated**: 19 January 2026


## Author
**Kabelo Bruce Sebashe**  
University of South Africa (UNISA)

