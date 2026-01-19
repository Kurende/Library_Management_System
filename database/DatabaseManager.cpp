#include "DatabaseManager.h"
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>
#include <QFile>
#include "utils/Encryption.h"
#include <QSqlQuery>
#include <QDateTime>

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager() {
}

DatabaseManager::~DatabaseManager() {
    closeDatabase();
}

bool DatabaseManager::initialize(const QString& dbPath) {
    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(dbPath);
    
    if (!m_database.open()) {
        setLastError("Failed to open database: " + m_database.lastError().text());
        return false;
    }
    
    return createTables();
}

bool DatabaseManager::createTables() {
    QSqlQuery query(m_database);
    
    // Create Users table
    QString createUsersTable = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE NOT NULL,
            password_hash TEXT NOT NULL,
            name TEXT NOT NULL,
            surname TEXT NOT NULL,
            email TEXT UNIQUE NOT NULL,
            contact_no TEXT,
            school_name TEXT,
            role TEXT NOT NULL,
            security_question TEXT NOT NULL,
            security_answer TEXT NOT NULL,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    )";
    
    if (!query.exec(createUsersTable)) {
        setLastError("Failed to create users table: " + query.lastError().text());
        return false;
    }
    
    // Create Learners table
    QString createLearnersTable = R"(
        CREATE TABLE IF NOT EXISTS learners (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            surname TEXT NOT NULL,
            grade TEXT NOT NULL,
            date_of_birth DATE NOT NULL,
            contact_no TEXT,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    )";
    
    if (!query.exec(createLearnersTable)) {
        setLastError("Failed to create learners table: " + query.lastError().text());
        return false;
    }
    
    // Create Books table
    QString createBooksTable = R"(
        CREATE TABLE IF NOT EXISTS books (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            book_code TEXT UNIQUE NOT NULL,
            isbn TEXT NOT NULL,
            title TEXT NOT NULL,
            author TEXT NOT NULL,
            subject TEXT NOT NULL,
            grade TEXT NOT NULL,
            price REAL NOT NULL DEFAULT 0.0,
            status TEXT NOT NULL DEFAULT 'Available',
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    )";
    
    if (!query.exec(createBooksTable)) {
        setLastError("Failed to create books table: " + query.lastError().text());
        return false;
    }
    
    // Create Transactions table
    QString createTransactionsTable = R"(
        CREATE TABLE IF NOT EXISTS transactions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            learner_id INTEGER NOT NULL,
            book_id INTEGER NOT NULL,
            borrow_date DATE NOT NULL,
            due_date DATE NOT NULL,
            return_date DATE,
            status TEXT NOT NULL DEFAULT 'Active',
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (learner_id) REFERENCES learners(id),
            FOREIGN KEY (book_id) REFERENCES books(id)
        )
    )";
    
    if (!query.exec(createTransactionsTable)) {
        setLastError("Failed to create transactions table: " + query.lastError().text());
        return false;
    }

    // Create user_activity_logs table
    QString createActivityLogTable = R"(
        CREATE TABLE IF NOT EXISTS user_activity_logs (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER NOT NULL,
            action_type TEXT NOT NULL,
            action_details TEXT,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (user_id) REFERENCES users(id)
        )
    )";

    if (!query.exec(createActivityLogTable)) {
        qCritical() << "Failed to create user_activity_logs table:" << query.lastError().text();
        return false;
    }

    // Add password_changed_at and last_login columns to users table if they don't exist
    query.exec("ALTER TABLE users ADD COLUMN password_changed_at DATETIME");
    query.exec("ALTER TABLE users ADD COLUMN last_login DATETIME");

    
    return true;
}

void DatabaseManager::closeDatabase() {
    if (m_database.isOpen()) {
        m_database.close();
    }
}

// ==================== User Operations ====================

bool DatabaseManager::addUser(const User& user) {
    QSqlQuery query(m_database);
    query.prepare(R"(
        INSERT INTO users (username, password_hash, name, surname, email, 
                          contact_no, school_name, role, security_question, security_answer)
        VALUES (:username, :password_hash, :name, :surname, :email,
                :contact_no, :school_name, :role, :security_question, :security_answer)
    )");
    
    query.bindValue(":username", user.getUsername());
    query.bindValue(":password_hash", user.getPasswordHash());
    query.bindValue(":name", user.getName());
    query.bindValue(":surname", user.getSurname());
    query.bindValue(":email", user.getEmail());
    query.bindValue(":contact_no", user.getContactNo());
    query.bindValue(":school_name", user.getSchoolName());
    query.bindValue(":role", User::roleToString(user.getRole()));
    query.bindValue(":security_question", user.getSecurityQuestion());
    query.bindValue(":security_answer", user.getSecurityAnswer());
    
    return executeQuery(query);
}

bool DatabaseManager::updateUser(const User& user) {
    QSqlQuery query(m_database);
    query.prepare(R"(
        UPDATE users SET username = :username, password_hash = :password_hash,
                        name = :name, surname = :surname, email = :email,
                        contact_no = :contact_no, school_name = :school_name,
                        role = :role, security_question = :security_question,
                        security_answer = :security_answer
        WHERE id = :id
    )");
    
    query.bindValue(":id", user.getId());
    query.bindValue(":username", user.getUsername());
    query.bindValue(":password_hash", user.getPasswordHash());
    query.bindValue(":name", user.getName());
    query.bindValue(":surname", user.getSurname());
    query.bindValue(":email", user.getEmail());
    query.bindValue(":contact_no", user.getContactNo());
    query.bindValue(":school_name", user.getSchoolName());
    query.bindValue(":role", User::roleToString(user.getRole()));
    query.bindValue(":security_question", user.getSecurityQuestion());
    query.bindValue(":security_answer", user.getSecurityAnswer());
    
    return executeQuery(query);
}

bool DatabaseManager::deleteUser(int userId) {
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM users WHERE id = :id");
    query.bindValue(":id", userId);
    return executeQuery(query);
}

User DatabaseManager::getUserById(int userId) {
    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM users WHERE id = :id");
    query.bindValue(":id", userId);
    
    if (executeQuery(query) && query.next()) {
        return User(
            query.value("id").toInt(),
            query.value("username").toString(),
            query.value("password_hash").toString(),
            query.value("name").toString(),
            query.value("surname").toString(),
            query.value("email").toString(),
            query.value("contact_no").toString(),
            query.value("school_name").toString(),
            User::stringToRole(query.value("role").toString()),
            query.value("security_question").toString(),
            query.value("security_answer").toString(),
            query.value("created_at").toDateTime()
        );
    }
    
    return User();
}

User DatabaseManager::getUserByUsername(const QString& username) {
    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM users WHERE username = :username");
    query.bindValue(":username", username);
    
    if (executeQuery(query) && query.next()) {
        return User(
            query.value("id").toInt(),
            query.value("username").toString(),
            query.value("password_hash").toString(),
            query.value("name").toString(),
            query.value("surname").toString(),
            query.value("email").toString(),
            query.value("contact_no").toString(),
            query.value("school_name").toString(),
            User::stringToRole(query.value("role").toString()),
            query.value("security_question").toString(),
            query.value("security_answer").toString(),
            query.value("created_at").toDateTime()
        );
    }
    
    return User();
}

User DatabaseManager::getUserByEmail(const QString& email) {
    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM users WHERE email = :email");
    query.bindValue(":email", email);
    
    if (executeQuery(query) && query.next()) {
        return User(
            query.value("id").toInt(),
            query.value("username").toString(),
            query.value("password_hash").toString(),
            query.value("name").toString(),
            query.value("surname").toString(),
            query.value("email").toString(),
            query.value("contact_no").toString(),
            query.value("school_name").toString(),
            User::stringToRole(query.value("role").toString()),
            query.value("security_question").toString(),
            query.value("security_answer").toString(),
            query.value("created_at").toDateTime()
        );
    }
    
    return User();
}

QVector<User> DatabaseManager::getAllUsers() {
    QVector<User> users;
    QSqlQuery query("SELECT * FROM users ORDER BY name, surname", m_database);
    
    if (executeQuery(query)) {
        while (query.next()) {
            users.append(User(
                query.value("id").toInt(),
                query.value("username").toString(),
                query.value("password_hash").toString(),
                query.value("name").toString(),
                query.value("surname").toString(),
                query.value("email").toString(),
                query.value("contact_no").toString(),
                query.value("school_name").toString(),
                User::stringToRole(query.value("role").toString()),
                query.value("security_question").toString(),
                query.value("security_answer").toString(),
                query.value("created_at").toDateTime()
            ));
        }
    }
    
    return users;
}

QVector<Transaction> DatabaseManager::getTransactionsByBookId(int bookId) {
    QVector<Transaction> transactions;
    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM transactions WHERE book_id = :book_id ORDER BY created_at DESC");
    query.bindValue(":book_id", bookId);
    
    if (executeQuery(query)) {
        while (query.next()) {
            transactions.append(Transaction(
                query.value("id").toInt(),
                query.value("learner_id").toInt(),
                query.value("book_id").toInt(),
                query.value("borrow_date").toDate(),
                query.value("due_date").toDate(),
                query.value("return_date").toDate(),
                Transaction::stringToStatus(query.value("status").toString()),
                query.value("created_at").toDateTime()
            ));
        }
    }
    
    return transactions;
}

QVector<Transaction> DatabaseManager::getAllTransactions() {
    QVector<Transaction> transactions;
    QSqlQuery query("SELECT * FROM transactions ORDER BY created_at DESC", m_database);
    
    if (executeQuery(query)) {
        while (query.next()) {
            transactions.append(Transaction(
                query.value("id").toInt(),
                query.value("learner_id").toInt(),
                query.value("book_id").toInt(),
                query.value("borrow_date").toDate(),
                query.value("due_date").toDate(),
                query.value("return_date").toDate(),
                Transaction::stringToStatus(query.value("status").toString()),
                query.value("created_at").toDateTime()
            ));
        }
    }
    
    return transactions;
}

QVector<Transaction> DatabaseManager::getActiveTransactions() {
    QVector<Transaction> transactions;
    QSqlQuery query("SELECT * FROM transactions WHERE status = 'Active' ORDER BY created_at DESC", m_database);
    
    if (executeQuery(query)) {
        while (query.next()) {
            transactions.append(Transaction(
                query.value("id").toInt(),
                query.value("learner_id").toInt(),
                query.value("book_id").toInt(),
                query.value("borrow_date").toDate(),
                query.value("due_date").toDate(),
                query.value("return_date").toDate(),
                Transaction::stringToStatus(query.value("status").toString()),
                query.value("created_at").toDateTime()
            ));
        }
    }
    
    return transactions;
}

QVector<Transaction> DatabaseManager::getOverdueTransactions() {
    QVector<Transaction> transactions;
    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM transactions WHERE status = 'Active' AND due_date < :current_date ORDER BY due_date");
    query.bindValue(":current_date", QDate::currentDate());
    
    if (executeQuery(query)) {
        while (query.next()) {
            transactions.append(Transaction(
                query.value("id").toInt(),
                query.value("learner_id").toInt(),
                query.value("book_id").toInt(),
                query.value("borrow_date").toDate(),
                query.value("due_date").toDate(),
                query.value("return_date").toDate(),
                Transaction::stringToStatus(query.value("status").toString()),
                query.value("created_at").toDateTime()
            ));
        }
    }
    
    return transactions;
}

QVector<Transaction> DatabaseManager::getTransactionsByDateRange(const QDate& startDate, const QDate& endDate) {
    QVector<Transaction> transactions;
    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM transactions WHERE borrow_date BETWEEN :start_date AND :end_date ORDER BY borrow_date DESC");
    query.bindValue(":start_date", startDate);
    query.bindValue(":end_date", endDate);
    
    if (executeQuery(query)) {
        while (query.next()) {
            transactions.append(Transaction(
                query.value("id").toInt(),
                query.value("learner_id").toInt(),
                query.value("book_id").toInt(),
                query.value("borrow_date").toDate(),
                query.value("due_date").toDate(),
                query.value("return_date").toDate(),
                Transaction::stringToStatus(query.value("status").toString()),
                query.value("created_at").toDateTime()
            ));
        }
    }
    
    return transactions;
}

// ==================== Business Logic Operations ====================

bool DatabaseManager::borrowBook(int learnerId, int bookId, const QDate& borrowDate) {
    // Start transaction
    m_database.transaction();
    
    // Check if learner has overdue books
    if (hasOverdueBooks(learnerId)) {
        setLastError("Learner has overdue books and cannot borrow");
        m_database.rollback();
        return false;
    }
    
    // Check if book is available
    Book book = getBookById(bookId);
    if (book.getId() == -1 || !book.isAvailable()) {
        setLastError("Book is not available for borrowing");
        m_database.rollback();
        return false;
    }
    
    // Calculate due date
    QDate dueDate = Transaction::calculateDueDate(borrowDate);
    
    // Create transaction
    Transaction transaction;
    transaction.setLearnerId(learnerId);
    transaction.setBookId(bookId);
    transaction.setBorrowDate(borrowDate);
    transaction.setDueDate(dueDate);
    transaction.setStatus(Transaction::Status::Active);
    
    if (!addTransaction(transaction)) {
        m_database.rollback();
        return false;
    }
    
    // Update book status to Borrowed
    book.setStatus(Book::Status::Borrowed);
    if (!updateBook(book)) {
        m_database.rollback();
        return false;
    }
    
    // Commit transaction
    m_database.commit();
    return true;
}

bool DatabaseManager::returnBook(int transactionId, const QDate& returnDate) {
    // Start transaction
    m_database.transaction();
    
    // Get the transaction
    Transaction transaction = getTransactionById(transactionId);
    if (transaction.getId() == -1 || !transaction.isActive()) {
        setLastError("Transaction not found or not active");
        m_database.rollback();
        return false;
    }
    
    // Update transaction
    transaction.setReturnDate(returnDate);
    transaction.setStatus(Transaction::Status::Returned);
    
    if (!updateTransaction(transaction)) {
        m_database.rollback();
        return false;
    }
    
    // Update book status to Available
    Book book = getBookById(transaction.getBookId());
    if (book.getId() == -1) {
        setLastError("Book not found");
        m_database.rollback();
        return false;
    }
    
    book.setStatus(Book::Status::Available);
    if (!updateBook(book)) {
        m_database.rollback();
        return false;
    }
    
    // Commit transaction
    m_database.commit();
    return true;
}

bool DatabaseManager::markBookAsLost(int transactionId) {
    // Start transaction
    m_database.transaction();
    
    // Get the transaction
    Transaction transaction = getTransactionById(transactionId);
    if (transaction.getId() == -1 || !transaction.isActive()) {
        setLastError("Transaction not found or not active");
        m_database.rollback();
        return false;
    }
    
    // Update transaction status to Lost
    transaction.setStatus(Transaction::Status::Lost);
    
    if (!updateTransaction(transaction)) {
        m_database.rollback();
        return false;
    }
    
    // Update book status to Lost
    Book book = getBookById(transaction.getBookId());
    if (book.getId() == -1) {
        setLastError("Book not found");
        m_database.rollback();
        return false;
    }
    
    book.setStatus(Book::Status::Lost);
    if (!updateBook(book)) {
        m_database.rollback();
        return false;
    }
    
    // Commit transaction
    m_database.commit();
    return true;
}

bool DatabaseManager::hasOverdueBooks(int learnerId) {
    QSqlQuery query(m_database);
    query.prepare(R"(
        SELECT COUNT(*) FROM transactions 
        WHERE learner_id = :learner_id 
        AND status = 'Active' 
        AND due_date < :current_date
    )");
    query.bindValue(":learner_id", learnerId);
    query.bindValue(":current_date", QDate::currentDate());
    
    if (executeQuery(query) && query.next()) {
        return query.value(0).toInt() > 0;
    }
    
    return false;
}

double DatabaseManager::calculateUnreturnedBooksAmount(int learnerId) {
    double totalAmount = 0.0;
    
    // Get all active and lost transactions for the learner
    QSqlQuery query(m_database);
    query.prepare(R"(
        SELECT b.price 
        FROM transactions t
        JOIN books b ON t.book_id = b.id
        WHERE t.learner_id = :learner_id 
        AND (t.status = 'Active' OR t.status = 'Lost')
    )");
    query.bindValue(":learner_id", learnerId);
    
    if (executeQuery(query)) {
        while (query.next()) {
            totalAmount += query.value(0).toDouble();
        }
    }
    
    return totalAmount;
}

// ==================== Dashboard Statistics ====================

DatabaseManager::DashboardStats DatabaseManager::getDashboardStats() {
    DashboardStats stats;
    stats.totalBooks = getTotalBookCount();
    stats.availableBooks = getAvailableBookCount();
    stats.borrowedBooks = getBorrowedBookCount();
    stats.totalLearners = getLearnerCount();
    stats.activeLearners = getActiveLearnerCount();
    
    // Get total users count
    QSqlQuery userQuery("SELECT COUNT(*) FROM users", m_database);
    if (executeQuery(userQuery) && userQuery.next()) {
        stats.totalUsers = userQuery.value(0).toInt();
    }
    
    // Get overdue books count
    QSqlQuery overdueQuery(m_database);
    overdueQuery.prepare("SELECT COUNT(*) FROM transactions WHERE status = 'Active' AND due_date < :current_date");
    overdueQuery.bindValue(":current_date", QDate::currentDate());
    if (executeQuery(overdueQuery) && overdueQuery.next()) {
        stats.overdueBooks = overdueQuery.value(0).toInt();
    }
    
    return stats;
}

QVector<Transaction> DatabaseManager::getRecentTransactions(int limit) {
    QVector<Transaction> transactions;
    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM transactions ORDER BY created_at DESC LIMIT :limit");
    query.bindValue(":limit", limit);
    
    if (executeQuery(query)) {
        while (query.next()) {
            transactions.append(Transaction(
                query.value("id").toInt(),
                query.value("learner_id").toInt(),
                query.value("book_id").toInt(),
                query.value("borrow_date").toDate(),
                query.value("due_date").toDate(),
                query.value("return_date").toDate(),
                Transaction::stringToStatus(query.value("status").toString()),
                query.value("created_at").toDateTime()
            ));
        }
    }
    
    return transactions;
}

// ==================== Helper Methods ====================

void DatabaseManager::setLastError(const QString& error) {
    m_lastError = error;
    qDebug() << "DatabaseManager Error:" << error;
}

bool DatabaseManager::executeQuery(QSqlQuery& query) {
    if (!query.exec()) {
        setLastError(query.lastError().text());
        return false;
    }
    return true;
}

bool DatabaseManager::userExists(const QString& username) {
    QSqlQuery query(m_database);
    query.prepare("SELECT COUNT(*) FROM users WHERE username = :username");
    query.bindValue(":username", username);
    
    if (executeQuery(query) && query.next()) {
        return query.value(0).toInt() > 0;
    }
    
    return false;
}

// ==================== Learner Operations ====================

bool DatabaseManager::addLearner(const Learner& learner) {
    QSqlQuery query(m_database);
    query.prepare(R"(
        INSERT INTO learners (name, surname, grade, date_of_birth, contact_no)
        VALUES (:name, :surname, :grade, :dob, :contact_no)
    )");
    
    query.bindValue(":name", learner.getName());
    query.bindValue(":surname", learner.getSurname());
    query.bindValue(":grade", learner.getGrade());
    query.bindValue(":dob", learner.getDateOfBirth());
    query.bindValue(":contact_no", learner.getContactNo());
    
    return executeQuery(query);
}

bool DatabaseManager::updateLearner(const Learner& learner) {
    QSqlQuery query(m_database);
    query.prepare(R"(
        UPDATE learners SET name = :name, surname = :surname, grade = :grade,
                           date_of_birth = :dob, contact_no = :contact_no
        WHERE id = :id
    )");
    
    query.bindValue(":id", learner.getId());
    query.bindValue(":name", learner.getName());
    query.bindValue(":surname", learner.getSurname());
    query.bindValue(":grade", learner.getGrade());
    query.bindValue(":dob", learner.getDateOfBirth());
    query.bindValue(":contact_no", learner.getContactNo());
    
    return executeQuery(query);
}

bool DatabaseManager::deleteLearner(int learnerId) {
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM learners WHERE id = :id");
    query.bindValue(":id", learnerId);
    return executeQuery(query);
}

Learner DatabaseManager::getLearnerById(int learnerId) {
    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM learners WHERE id = :id");
    query.bindValue(":id", learnerId);
    
    if (executeQuery(query) && query.next()) {
        return Learner(
            query.value("id").toInt(),
            query.value("name").toString(),
            query.value("surname").toString(),
            query.value("grade").toString(),
            query.value("date_of_birth").toDate(),
            query.value("contact_no").toString(),
            query.value("created_at").toDateTime()
        );
    }
    
    return Learner();
}

QVector<Learner> DatabaseManager::getAllLearners() {
    QVector<Learner> learners;
    QSqlQuery query("SELECT * FROM learners ORDER BY surname, name", m_database);
    
    if (executeQuery(query)) {
        while (query.next()) {
            learners.append(Learner(
                query.value("id").toInt(),
                query.value("name").toString(),
                query.value("surname").toString(),
                query.value("grade").toString(),
                query.value("date_of_birth").toDate(),
                query.value("contact_no").toString(),
                query.value("created_at").toDateTime()
            ));
        }
    }
    
    return learners;
}

QVector<Learner> DatabaseManager::getLearnersByGrade(const QString& grade) {
    QVector<Learner> learners;
    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM learners WHERE grade = :grade ORDER BY surname, name");
    query.bindValue(":grade", grade);
    
    if (executeQuery(query)) {
        while (query.next()) {
            learners.append(Learner(
                query.value("id").toInt(),
                query.value("name").toString(),
                query.value("surname").toString(),
                query.value("grade").toString(),
                query.value("date_of_birth").toDate(),
                query.value("contact_no").toString(),
                query.value("created_at").toDateTime()
            ));
        }
    }
    
    return learners;
}

QVector<Learner> DatabaseManager::searchLearners(const QString& searchTerm) {
    QVector<Learner> learners;
    QSqlQuery query(m_database);
    query.prepare(R"(
        SELECT * FROM learners 
        WHERE name LIKE :search OR surname LIKE :search OR 
              CAST(id AS TEXT) LIKE :search
        ORDER BY surname, name
    )");
    
    QString searchPattern = "%" + searchTerm + "%";
    query.bindValue(":search", searchPattern);
    
    if (executeQuery(query)) {
        while (query.next()) {
            learners.append(Learner(
                query.value("id").toInt(),
                query.value("name").toString(),
                query.value("surname").toString(),
                query.value("grade").toString(),
                query.value("date_of_birth").toDate(),
                query.value("contact_no").toString(),
                query.value("created_at").toDateTime()
            ));
        }
    }
    
    return learners;
}

int DatabaseManager::getLearnerCount() {
    QSqlQuery query("SELECT COUNT(*) FROM learners", m_database);
    if (executeQuery(query) && query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

int DatabaseManager::getActiveLearnerCount() {
    QSqlQuery query(R"(
        SELECT COUNT(DISTINCT learner_id) 
        FROM transactions 
        WHERE status = 'Active'
    )", m_database);
    
    if (executeQuery(query) && query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

// ==================== Book Operations ====================

bool DatabaseManager::addBook(const Book& book) {
    QSqlQuery query(m_database);
    query.prepare(R"(
        INSERT INTO books (book_code, isbn, title, author, subject, grade, price, status)
        VALUES (:book_code, :isbn, :title, :author, :subject, :grade, :price, :status)
    )");
    
    query.bindValue(":book_code", book.getBookCode());
    query.bindValue(":isbn", book.getIsbn());
    query.bindValue(":title", book.getTitle());
    query.bindValue(":author", book.getAuthor());
    query.bindValue(":subject", book.getSubject());
    query.bindValue(":grade", book.getGrade());
    query.bindValue(":price", book.getPrice());
    query.bindValue(":status", Book::statusToString(book.getStatus()));
    
    return executeQuery(query);
}

bool DatabaseManager::updateBook(const Book& book) {
    QSqlQuery query(m_database);
    query.prepare(R"(
        UPDATE books SET book_code = :book_code, isbn = :isbn, title = :title,
                        author = :author, subject = :subject, grade = :grade,
                        price = :price, status = :status
        WHERE id = :id
    )");
    
    query.bindValue(":id", book.getId());
    query.bindValue(":book_code", book.getBookCode());
    query.bindValue(":isbn", book.getIsbn());
    query.bindValue(":title", book.getTitle());
    query.bindValue(":author", book.getAuthor());
    query.bindValue(":subject", book.getSubject());
    query.bindValue(":grade", book.getGrade());
    query.bindValue(":price", book.getPrice());
    query.bindValue(":status", Book::statusToString(book.getStatus()));
    
    return executeQuery(query);
}

bool DatabaseManager::deleteBook(int bookId) {
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM books WHERE id = :id");
    query.bindValue(":id", bookId);
    return executeQuery(query);
}

Book DatabaseManager::getBookById(int bookId) {
    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM books WHERE id = :id");
    query.bindValue(":id", bookId);
    
    if (executeQuery(query) && query.next()) {
        return Book(
            query.value("id").toInt(),
            query.value("book_code").toString(),
            query.value("isbn").toString(),
            query.value("title").toString(),
            query.value("author").toString(),
            query.value("subject").toString(),
            query.value("grade").toString(),
            query.value("price").toDouble(),
            Book::stringToStatus(query.value("status").toString()),
            query.value("created_at").toDateTime()
        );
    }
    
    return Book();
}

Book DatabaseManager::getBookByCode(const QString& bookCode) {
    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM books WHERE book_code = :book_code");
    query.bindValue(":book_code", bookCode);
    
    if (executeQuery(query) && query.next()) {
        return Book(
            query.value("id").toInt(),
            query.value("book_code").toString(),
            query.value("isbn").toString(),
            query.value("title").toString(),
            query.value("author").toString(),
            query.value("subject").toString(),
            query.value("grade").toString(),
            query.value("price").toDouble(),
            Book::stringToStatus(query.value("status").toString()),
            query.value("created_at").toDateTime()
        );
    }
    
    return Book();
}

QVector<Book> DatabaseManager::getAllBooks() {
    QVector<Book> books;
    QSqlQuery query("SELECT * FROM books ORDER BY title", m_database);
    
    if (executeQuery(query)) {
        while (query.next()) {
            books.append(Book(
                query.value("id").toInt(),
                query.value("book_code").toString(),
                query.value("isbn").toString(),
                query.value("title").toString(),
                query.value("author").toString(),
                query.value("subject").toString(),
                query.value("grade").toString(),
                query.value("price").toDouble(),
                Book::stringToStatus(query.value("status").toString()),
                query.value("created_at").toDateTime()
            ));
        }
    }
    
    return books;
}

QVector<Book> DatabaseManager::getBooksByGrade(const QString& grade) {
    QVector<Book> books;
    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM books WHERE grade = :grade ORDER BY title");
    query.bindValue(":grade", grade);
    
    if (executeQuery(query)) {
        while (query.next()) {
            books.append(Book(
                query.value("id").toInt(),
                query.value("book_code").toString(),
                query.value("isbn").toString(),
                query.value("title").toString(),
                query.value("author").toString(),
                query.value("subject").toString(),
                query.value("grade").toString(),
                query.value("price").toDouble(),
                Book::stringToStatus(query.value("status").toString()),
                query.value("created_at").toDateTime()
            ));
        }
    }
    
    return books;
}

QVector<Book> DatabaseManager::getBooksBySubject(const QString& subject) {
    QVector<Book> books;
    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM books WHERE subject = :subject ORDER BY title");
    query.bindValue(":subject", subject);
    
    if (executeQuery(query)) {
        while (query.next()) {
            books.append(Book(
                query.value("id").toInt(),
                query.value("book_code").toString(),
                query.value("isbn").toString(),
                query.value("title").toString(),
                query.value("author").toString(),
                query.value("subject").toString(),
                query.value("grade").toString(),
                query.value("price").toDouble(),
                Book::stringToStatus(query.value("status").toString()),
                query.value("created_at").toDateTime()
            ));
        }
    }
    
    return books;
}

QVector<Book> DatabaseManager::getBooksByStatus(Book::Status status) {
    QVector<Book> books;
    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM books WHERE status = :status ORDER BY title");
    query.bindValue(":status", Book::statusToString(status));
    
    if (executeQuery(query)) {
        while (query.next()) {
            books.append(Book(
                query.value("id").toInt(),
                query.value("book_code").toString(),
                query.value("isbn").toString(),
                query.value("title").toString(),
                query.value("author").toString(),
                query.value("subject").toString(),
                query.value("grade").toString(),
                query.value("price").toDouble(),
                Book::stringToStatus(query.value("status").toString()),
                query.value("created_at").toDateTime()
            ));
        }
    }
    
    return books;
}

QVector<Book> DatabaseManager::searchBooks(const QString& searchTerm) {
    QVector<Book> books;
    QSqlQuery query(m_database);
    query.prepare(R"(
        SELECT * FROM books 
        WHERE title LIKE :search OR author LIKE :search OR 
              book_code LIKE :search OR isbn LIKE :search
        ORDER BY title
    )");
    
    QString searchPattern = "%" + searchTerm + "%";
    query.bindValue(":search", searchPattern);
    
    if (executeQuery(query)) {
        while (query.next()) {
            books.append(Book(
                query.value("id").toInt(),
                query.value("book_code").toString(),
                query.value("isbn").toString(),
                query.value("title").toString(),
                query.value("author").toString(),
                query.value("subject").toString(),
                query.value("grade").toString(),
                query.value("price").toDouble(),
                Book::stringToStatus(query.value("status").toString()),
                query.value("created_at").toDateTime()
            ));
        }
    }
    
    return books;
}

bool DatabaseManager::bookCodeExists(const QString& bookCode) {
    QSqlQuery query(m_database);
    query.prepare("SELECT COUNT(*) FROM books WHERE book_code = :book_code");
    query.bindValue(":book_code", bookCode);
    
    if (executeQuery(query) && query.next()) {
        return query.value(0).toInt() > 0;
    }
    
    return false;
}

int DatabaseManager::getBookCountByISBN(const QString& isbn) {
    QSqlQuery query(m_database);
    query.prepare("SELECT COUNT(*) FROM books WHERE isbn = :isbn");
    query.bindValue(":isbn", isbn);
    
    if (executeQuery(query) && query.next()) {
        return query.value(0).toInt();
    }
    
    return 0;
}

int DatabaseManager::getTotalBookCount() {
    QSqlQuery query("SELECT COUNT(*) FROM books", m_database);
    if (executeQuery(query) && query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

int DatabaseManager::getAvailableBookCount() {
    QSqlQuery query("SELECT COUNT(*) FROM books WHERE status = 'Available'", m_database);
    if (executeQuery(query) && query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

int DatabaseManager::getBorrowedBookCount() {
    QSqlQuery query("SELECT COUNT(*) FROM books WHERE status = 'Borrowed'", m_database);
    if (executeQuery(query) && query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

// ==================== Transaction Operations ====================

bool DatabaseManager::addTransaction(const Transaction& transaction) {
    QSqlQuery query(m_database);
    query.prepare(R"(
        INSERT INTO transactions (learner_id, book_id, borrow_date, due_date, 
                                 return_date, status)
        VALUES (:learner_id, :book_id, :borrow_date, :due_date, 
                :return_date, :status)
    )");
    
    query.bindValue(":learner_id", transaction.getLearnerId());
    query.bindValue(":book_id", transaction.getBookId());
    query.bindValue(":borrow_date", transaction.getBorrowDate());
    query.bindValue(":due_date", transaction.getDueDate());
    query.bindValue(":return_date", 
                   transaction.getReturnDate().isValid() ? 
                   transaction.getReturnDate() : QVariant());
    query.bindValue(":status", Transaction::statusToString(transaction.getStatus()));
    
    return executeQuery(query);
}

bool DatabaseManager::updateTransaction(const Transaction& transaction) {
    QSqlQuery query(m_database);
    query.prepare(R"(
        UPDATE transactions SET learner_id = :learner_id, book_id = :book_id,
                               borrow_date = :borrow_date, due_date = :due_date,
                               return_date = :return_date, status = :status
        WHERE id = :id
    )");
    
    query.bindValue(":id", transaction.getId());
    query.bindValue(":learner_id", transaction.getLearnerId());
    query.bindValue(":book_id", transaction.getBookId());
    query.bindValue(":borrow_date", transaction.getBorrowDate());
    query.bindValue(":due_date", transaction.getDueDate());
    query.bindValue(":return_date", 
                   transaction.getReturnDate().isValid() ? 
                   transaction.getReturnDate() : QVariant());
    query.bindValue(":status", Transaction::statusToString(transaction.getStatus()));
    
    return executeQuery(query);
}

Transaction DatabaseManager::getTransactionById(int transactionId) {
    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM transactions WHERE id = :id");
    query.bindValue(":id", transactionId);
    
    if (executeQuery(query) && query.next()) {
        return Transaction(
            query.value("id").toInt(),
            query.value("learner_id").toInt(),
            query.value("book_id").toInt(),
            query.value("borrow_date").toDate(),
            query.value("due_date").toDate(),
            query.value("return_date").toDate(),
            Transaction::stringToStatus(query.value("status").toString()),
            query.value("created_at").toDateTime()
        );
    }
    
    return Transaction();
}

QVector<Transaction> DatabaseManager::getTransactionsByLearnerId(int learnerId) {
    QVector<Transaction> transactions;
    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM transactions WHERE learner_id = :learner_id ORDER BY created_at DESC");
    query.bindValue(":learner_id", learnerId);
    
    if (executeQuery(query)) {
        while (query.next()) {
            transactions.append(Transaction(
                query.value("id").toInt(),
                query.value("learner_id").toInt(),
                query.value("book_id").toInt(),
                query.value("borrow_date").toDate(),
                query.value("due_date").toDate(),
                query.value("return_date").toDate(),
                Transaction::stringToStatus(query.value("status").toString()),
                query.value("created_at").toDateTime()
            ));
        }
    }
    
    return transactions;
}

QVector<Transaction> DatabaseManager::getActiveTransactionsByLearnerId(int learnerId) {
    QVector<Transaction> transactions;
    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM transactions WHERE learner_id = :learner_id AND status = 'Active' ORDER BY created_at DESC");
    query.bindValue(":learner_id", learnerId);
    
    if (executeQuery(query)) {
        while (query.next()) {
            transactions.append(Transaction(
                query.value("id").toInt(),
                query.value("learner_id").toInt(),
                query.value("book_id").toInt(),
                query.value("borrow_date").toDate(),
                query.value("due_date").toDate(),
                query.value("return_date").toDate(),
                Transaction::stringToStatus(query.value("status").toString()),
                query.value("created_at").toDateTime()
            ));
        }
    }
    
    return transactions;
}

bool DatabaseManager::createPaymentTables() {
    QSqlQuery query(m_database);

    // Create payments table
    QString createPaymentsTable = R"(
        CREATE TABLE IF NOT EXISTS payments (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            receipt_no TEXT UNIQUE NOT NULL,
            learner_id INTEGER NOT NULL,
            amount REAL NOT NULL,
            processed_by INTEGER NOT NULL,
            payment_date DATETIME DEFAULT CURRENT_TIMESTAMP,
            notes TEXT,
            FOREIGN KEY (learner_id) REFERENCES learners(id),
            FOREIGN KEY (processed_by) REFERENCES users(id)
        )
    )";

    if (!query.exec(createPaymentsTable)) {
        qDebug() << "Error creating payments table:" << query.lastError().text();
        return false;
    }

    // Create payment_items table
    QString createPaymentItemsTable = R"(
        CREATE TABLE IF NOT EXISTS payment_items (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            payment_id INTEGER NOT NULL,
            transaction_id INTEGER NOT NULL,
            book_id INTEGER NOT NULL,
            amount REAL NOT NULL,
            FOREIGN KEY (payment_id) REFERENCES payments(id),
            FOREIGN KEY (transaction_id) REFERENCES transactions(id),
            FOREIGN KEY (book_id) REFERENCES books(id)
        )
    )";

    if (!query.exec(createPaymentItemsTable)) {
        qDebug() << "Error creating payment_items table:" << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::processPayment(Payment& payment, const QVector<int>& transactionIds) {
    if (transactionIds.isEmpty()) {
        m_lastError = "No transactions selected for payment";
        return false;
    }

    // Start transaction
    if (!m_database.transaction()) {
        m_lastError = "Failed to start database transaction";
        return false;
    }

    QSqlQuery query(m_database);

    try {
        // Generate receipt number if not set
        if (payment.getReceiptNo().isEmpty()) {
            payment.setReceiptNo(payment.generateReceiptNo());
        }

        // Insert payment record
        query.prepare(R"(
            INSERT INTO payments (receipt_no, learner_id, amount, processed_by, notes)
            VALUES (:receipt_no, :learner_id, :amount, :processed_by, :notes)
        )");

        query.bindValue(":receipt_no", payment.getReceiptNo());
        query.bindValue(":learner_id", payment.getLearnerId());
        query.bindValue(":amount", payment.getAmount());
        query.bindValue(":processed_by", payment.getProcessedBy());
        query.bindValue(":notes", payment.getNotes());

        if (!query.exec()) {
            throw std::runtime_error(query.lastError().text().toStdString());
        }

        int paymentId = query.lastInsertId().toInt();
        payment.setId(paymentId);

        // Process each transaction
        for (int transId : transactionIds) {
            // Get transaction details
            Transaction trans = getTransactionById(transId);
            if (trans.getId() == -1) {
                throw std::runtime_error("Transaction not found");
            }

            // Get book price
            Book book = getBookById(trans.getBookId());
            if (book.getId() == -1) {
                throw std::runtime_error("Book not found");
            }

            // Insert payment item
            query.prepare(R"(
                INSERT INTO payment_items (payment_id, transaction_id, book_id, amount)
                VALUES (:payment_id, :transaction_id, :book_id, :amount)
            )");

            query.bindValue(":payment_id", paymentId);
            query.bindValue(":transaction_id", transId);
            query.bindValue(":book_id", trans.getBookId());
            query.bindValue(":amount", book.getPrice());

            if (!query.exec()) {
                throw std::runtime_error(query.lastError().text().toStdString());
            }

            // Update transaction status to "Paid" (we'll add this status)
            query.prepare(R"(
                UPDATE transactions
                SET status = 'Paid', notes = 'Payment processed - Receipt: ' || :receipt_no
                WHERE id = :id
            )");

            query.bindValue(":receipt_no", payment.getReceiptNo());
            query.bindValue(":id", transId);

            if (!query.exec()) {
                throw std::runtime_error(query.lastError().text().toStdString());
            }
        }

        // Commit transaction
        if (!m_database.commit()) {
            throw std::runtime_error("Failed to commit transaction");
        }

        return true;

    } catch (const std::exception& e) {
        m_database.rollback();
        m_lastError = QString("Payment processing failed: %1").arg(e.what());
        qDebug() << m_lastError;
        return false;
    }
}

QVector<Transaction> DatabaseManager::getUnpaidLostTransactionsByLearnerId(int learnerId) {
    QVector<Transaction> transactions;
    QSqlQuery query(m_database);

    query.prepare(R"(
        SELECT * FROM transactions
        WHERE learner_id = :learner_id
        AND status = 'Lost'
        ORDER BY return_date DESC
    )");

    query.bindValue(":learner_id", learnerId);

    if (!query.exec()) {
        qDebug() << "Error getting unpaid lost transactions:" << query.lastError().text();
        return transactions;
    }

    while (query.next()) {
        Transaction trans;
        trans.setId(query.value("id").toInt());
        trans.setLearnerId(query.value("learner_id").toInt());
        trans.setBookId(query.value("book_id").toInt());
        trans.setBorrowDate(query.value("borrow_date").toDate());
        trans.setDueDate(query.value("due_date").toDate());
        trans.setReturnDate(query.value("return_date").toDate());
        trans.setStatus(Transaction::stringToStatus(query.value("status").toString()));

        transactions.append(trans);
    }

    return transactions;
}

double DatabaseManager::getTotalOutstandingFees(int learnerId) {
    double total = 0.0;
    QSqlQuery query(m_database);

    query.prepare(R"(
        SELECT SUM(b.price) as total
        FROM transactions t
        JOIN books b ON t.book_id = b.id
        WHERE t.learner_id = :learner_id
        AND t.status = 'Lost'
    )");

    query.bindValue(":learner_id", learnerId);

    if (query.exec() && query.next()) {
        total = query.value("total").toDouble();
    }

    return total;
}

Payment DatabaseManager::getPaymentById(int id) {
    Payment payment;
    QSqlQuery query(m_database);

    query.prepare("SELECT * FROM payments WHERE id = :id");
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        payment.setId(query.value("id").toInt());
        payment.setReceiptNo(query.value("receipt_no").toString());
        payment.setLearnerId(query.value("learner_id").toInt());
        payment.setAmount(query.value("amount").toDouble());
        payment.setProcessedBy(query.value("processed_by").toInt());
        payment.setPaymentDate(query.value("payment_date").toDateTime());
        payment.setNotes(query.value("notes").toString());
    }

    return payment;
}

QVector<Payment> DatabaseManager::getPaymentsByLearnerId(int learnerId) {
    QVector<Payment> payments;
    QSqlQuery query(m_database);

    query.prepare(R"(
        SELECT * FROM payments
        WHERE learner_id = :learner_id
        ORDER BY payment_date DESC
    )");

    query.bindValue(":learner_id", learnerId);

    if (!query.exec()) {
        qDebug() << "Error getting payments:" << query.lastError().text();
        return payments;
    }

    while (query.next()) {
        Payment payment;
        payment.setId(query.value("id").toInt());
        payment.setReceiptNo(query.value("receipt_no").toString());
        payment.setLearnerId(query.value("learner_id").toInt());
        payment.setAmount(query.value("amount").toDouble());
        payment.setProcessedBy(query.value("processed_by").toInt());
        payment.setPaymentDate(query.value("payment_date").toDateTime());
        payment.setNotes(query.value("notes").toString());
        payments.append(payment);
    }

    return payments;
}

QVector<PaymentItem> DatabaseManager::getPaymentItems(int paymentId) {
    QVector<PaymentItem> items;
    QSqlQuery query(m_database);

    query.prepare("SELECT * FROM payment_items WHERE payment_id = :payment_id");
    query.bindValue(":payment_id", paymentId);

    if (!query.exec()) {
        qDebug() << "Error getting payment items:" << query.lastError().text();
        return items;
    }

    while (query.next()) {
        PaymentItem item;
        item.setId(query.value("id").toInt());
        item.setPaymentId(query.value("payment_id").toInt());
        item.setTransactionId(query.value("transaction_id").toInt());
        item.setBookId(query.value("book_id").toInt());
        item.setAmount(query.value("amount").toDouble());
        items.append(item);
    }

    return items;
}

// ============================================================================
// GET USER LAST LOGIN
// ============================================================================

QDateTime DatabaseManager::getUserLastLogin(int userId) {
    QSqlQuery query(m_database);
    query.prepare("SELECT last_login FROM users WHERE id = :id");
    query.bindValue(":id", userId);

    if (query.exec() && query.next()) {
        return query.value(0).toDateTime();
    }

    return QDateTime(); // Invalid datetime
}

// ============================================================================
// GET PASSWORD CHANGED DATE
// ============================================================================

QDateTime DatabaseManager::getPasswordChangedDate(int userId) {
    QSqlQuery query(m_database);
    query.prepare("SELECT password_changed_at FROM users WHERE id = :id");
    query.bindValue(":id", userId);

    if (query.exec() && query.next()) {
        return query.value(0).toDateTime();
    }

    return QDateTime(); // Invalid datetime
}


// ============================================================================
// CHANGE USER PASSWORD
// ============================================================================

bool DatabaseManager::changeUserPassword(int userId, const QString& newPassword) {
    QSqlQuery query(m_database);

    // Hash the new password
    QString hashedPassword = Encryption::hashPassword(newPassword);

    query.prepare(R"(
        UPDATE users
        SET password_hash = :password,
            password_changed_at = :changed_at
        WHERE id = :id
    )");

    query.bindValue(":password", hashedPassword);
    query.bindValue(":changed_at", QDateTime::currentDateTime());
    query.bindValue(":id", userId);

    if (!query.exec()) {
        m_lastError = "Failed to change password: " + query.lastError().text();
        qWarning() << m_lastError;
        return false;
    }

    return true;
}

// ============================================================================
// LOG USER ACTIVITY
// ============================================================================

bool DatabaseManager::logUserActivity(int userId, const QString& actionType,
                                      const QString& actionDetails) {
    QSqlQuery query(m_database);

    query.prepare(R"(
        INSERT INTO user_activity_logs (user_id, action_type, action_details, created_at)
        VALUES (:user_id, :action_type, :action_details, :created_at)
    )");

    query.bindValue(":user_id", userId);
    query.bindValue(":action_type", actionType);
    query.bindValue(":action_details", actionDetails);
    query.bindValue(":created_at", QDateTime::currentDateTime());

    if (!query.exec()) {
        m_lastError = "Failed to log activity: " + query.lastError().text();
        qWarning() << m_lastError;
        return false;
    }

    return true;
}

// ============================================================================
// GET USER ACTIVITY LOG
// ============================================================================

QVector<UserActivityLog> DatabaseManager::getUserActivityLog(int userId, int limit) {
    QVector<UserActivityLog> logs;
    QSqlQuery query(m_database);

    query.prepare(R"(
        SELECT id, user_id, action_type, action_details, created_at
        FROM user_activity_logs
        WHERE user_id = :user_id
        ORDER BY created_at DESC
        LIMIT :limit
    )");

    query.bindValue(":user_id", userId);
    query.bindValue(":limit", limit);

    if (!query.exec()) {
        m_lastError = "Failed to get activity log: " + query.lastError().text();
        qWarning() << m_lastError;
        return logs;
    }

    while (query.next()) {
        UserActivityLog log;
        log.id = query.value(0).toInt();
        log.userId = query.value(1).toInt();
        log.actionType = query.value(2).toString();
        log.actionDetails = query.value(3).toString();
        log.createdAt = query.value(4).toDateTime();

        logs.append(log);
    }

    return logs;
}

