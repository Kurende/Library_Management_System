#ifndef BOOK_H
#define BOOK_H

#include <QString>
#include <QDateTime>

class Book {
public:
    // Enums
    enum class Status {
        Available,
        Borrowed,
        Lost
    };

    // Constructors
    Book();
    Book(int id, const QString& bookCode, const QString& isbn,
         const QString& title, const QString& author, const QString& subject,
         const QString& grade, double price, Status status,
         const QDateTime& createdAt);

    // Getters
    int getId() const { return m_id; }
    QString getBookCode() const { return m_bookCode; }
    QString getIsbn() const { return m_isbn; }
    QString getTitle() const { return m_title; }
    QString getAuthor() const { return m_author; }
    QString getSubject() const { return m_subject; }
    QString getGrade() const { return m_grade; }
    double getPrice() const { return m_price; }
    Status getStatus() const { return m_status; }
    QString getStatusString() const;
    QDateTime getCreatedAt() const { return m_createdAt; }

    // Setters
    void setId(int id) { m_id = id; }
    void setBookCode(const QString& code) { m_bookCode = code; }
    void setIsbn(const QString& isbn) { m_isbn = isbn; }
    void setTitle(const QString& title) { m_title = title; }
    void setAuthor(const QString& author) { m_author = author; }
    void setSubject(const QString& subject) { m_subject = subject; }
    void setGrade(const QString& grade) { m_grade = grade; }
    void setPrice(double price) { m_price = price; }
    void setStatus(Status status) { m_status = status; }
    void setCreatedAt(const QDateTime& dateTime) { m_createdAt = dateTime; }

    // Status helper methods
    bool isAvailable() const { return m_status == Status::Available; }
    bool isBorrowed() const { return m_status == Status::Borrowed; }
    bool isLost() const { return m_status == Status::Lost; }

    // Static helper methods
    static Status stringToStatus(const QString& statusStr);
    static QString statusToString(Status status);

private:
    int m_id;
    QString m_bookCode;      // Unique identifier (e.g., 000037)
    QString m_isbn;          // ISBN can be same for multiple copies
    QString m_title;
    QString m_author;
    QString m_subject;
    QString m_grade;
    double m_price;
    Status m_status;
    QDateTime m_createdAt;
};

#endif // BOOK_H