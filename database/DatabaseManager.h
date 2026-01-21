#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QVector>
#include <QDate>
#include "User.h"
#include "Learner.h"
#include "Book.h"
#include "Transaction.h"
#include "Payments.h"
#include "PaymentItem.h"

class DatabaseManager {
public:
    static DatabaseManager& instance();
    QSqlDatabase& getDatabase() { return m_database; }

    // Database initialization
    bool initialize(const QString& dbPath = "library_system.db");
    bool createTables();
    void closeDatabase();
    
    // User operations
    bool addUser(const User& user);
    bool updateUser(const User& user);
    bool deleteUser(int userId);
    User getUserById(int userId);
    User getUserByUsername(const QString& username);
    User getUserByEmail(const QString& email);
    QVector<User> getAllUsers();
    bool userExists(const QString& username);
    QDateTime getUserLastLogin(int userId);
    QDateTime getPasswordChangedDate(int userId);
    bool changeUserPassword(int userId, const QString& newPassword);

    // Activity Logging
    bool logUserActivity(int userId, const QString& actionType, const QString& actionDetails);
    QVector<UserActivityLog> getUserActivityLog(int userId, int limit = 100);
    
    // Learner operations
    bool addLearner(const Learner& learner);
    bool updateLearner(const Learner& learner);
    bool deleteLearner(int learnerId);
    Learner getLearnerById(int learnerId);
    QVector<Learner> getAllLearners();
    QVector<Learner> getLearnersByGrade(const QString& grade);
    QVector<Learner> searchLearners(const QString& searchTerm);
    int getLearnerCount();
    int getActiveLearnerCount(); // Learners with active borrows
    
    // Book operations
    bool addBook(const Book& book);
    bool updateBook(const Book& book);
    bool deleteBook(int bookId);
    Book getBookById(int bookId);
    Book getBookByCode(const QString& bookCode);
    QVector<Book> getAllBooks();
    QVector<Book> getBooksByGrade(const QString& grade);
    QVector<Book> getBooksBySubject(const QString& subject);
    QVector<Book> getBooksByStatus(Book::Status status);
    QVector<Book> searchBooks(const QString& searchTerm);
    bool bookCodeExists(const QString& bookCode);
    int getBookCountByISBN(const QString& isbn);
    int getTotalBookCount();
    int getAvailableBookCount();
    int getBorrowedBookCount();
    
    // Transaction operations
    bool addTransaction(const Transaction& transaction);
    bool updateTransaction(const Transaction& transaction);
    Transaction getTransactionById(int transactionId);
    QVector<Transaction> getTransactionsByLearnerId(int learnerId);
    QVector<Transaction> getActiveTransactionsByLearnerId(int learnerId);
    QVector<Transaction> getTransactionsByBookId(int bookId);
    QVector<Transaction> getAllTransactions();
    QVector<Transaction> getActiveTransactions();
    QVector<Transaction> getOverdueTransactions();
    QVector<Transaction> getTransactionsByDateRange(const QDate& startDate, const QDate& endDate);
    
    // Business logic operations
    bool borrowBook(int learnerId, int bookId, const QDate& borrowDate);
    bool returnBook(int transactionId, const QDate& returnDate);
    bool markBookAsLost(int transactionId);
    bool hasOverdueBooks(int learnerId);
    double calculateUnreturnedBooksAmount(int learnerId);
    
    // Dashboard statistics
    struct DashboardStats {
        int totalBooks;
        int availableBooks;
        int borrowedBooks;
        int totalLearners;
        int activeLearners;
        int totalUsers;
        int overdueBooks;
    };
    DashboardStats getDashboardStats();

    //Payments
    // Create payment tables
    bool createPaymentTables();

    // Payment operations
    bool processPayment(Payments& payment, const QVector<int>& transactionIds);
    Payments getPaymentById(int id);
    QVector<Payments> getPaymentsByLearnerId(int learnerId);
    QVector<PaymentItem> getPaymentItems(int paymentId);

    // Get lost transactions that haven't been paid for
    QVector<Transaction> getUnpaidLostTransactionsByLearnerId(int learnerId);

    // Calculate total outstanding fees for a learner
    double getTotalOutstandingFees(int learnerId);


    
    // Recent transactions for dashboard
    QVector<Transaction> getRecentTransactions(int limit = 10);
    
    // Error handling
    QString getLastError() const { return m_lastError; }
    
private:
    DatabaseManager();
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
    
    QSqlDatabase m_database;
    QString m_lastError;
    
    // Helper methods
    void setLastError(const QString& error);
    bool executeQuery(QSqlQuery& query);
};

#endif // DATABASEMANAGER_H
