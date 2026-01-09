# Developer Guide - Library Management System

## Quick Start for Developers

### Project Overview
This is a Qt-based C++ desktop application using SQLite for data persistence. The architecture follows a layered approach with clear separation between UI, business logic, and data access.

---

## Architecture

### Layers

```
┌─────────────────────────────────────┐
│         UI Layer (MainWindow)       │  ← User interaction
├─────────────────────────────────────┤
│    Business Logic (AuthManager)     │  ← Authentication & validation
├─────────────────────────────────────┤
│   Data Access (DatabaseManager)     │  ← Database operations
├─────────────────────────────────────┤
│     Models (User, Book, etc.)       │  ← Data entities
├─────────────────────────────────────┤
│      Database (SQLite)              │  ← Data persistence
└─────────────────────────────────────┘
```

### Key Design Patterns

1. **Singleton Pattern**: `DatabaseManager`, `AuthManager`
2. **Model-View Pattern**: Qt's MVC for UI
3. **Repository Pattern**: Database operations abstracted in `DatabaseManager`

---

## Code Organization

### Models (`models/`)
Pure data classes representing entities:
- **User**: System users with roles
- **Learner**: Student information
- **Book**: Book inventory items
- **Transaction**: Borrow/return records

**Key Points:**
- Use enums for status fields (type-safe)
- Provide helper methods (e.g., `isOverdue()`, `getFullName()`)
- Keep models simple - no database logic

### Database (`database/`)
All database operations centralized:

```cpp
// Example: Adding a book
Book book;
book.setBookCode("000123");
book.setTitle("Example Book");
// ... set other fields

DatabaseManager::instance().addBook(book);
```

**Key Methods:**
- `add*()` - Insert new records
- `update*()` - Modify existing records
- `get*()` - Retrieve records
- `search*()` - Query with filters

### Authentication (`auth/`)
Handles login, registration, permissions:

```cpp
// Login
if (AuthManager::instance().login(username, password)) {
    // Success
}

// Check permissions
if (AuthManager::instance().hasAdminPermission()) {
    // Admin-only feature
}
```

### UI (`ui/`)
MainWindow handles all UI interactions:
- Organized by functional areas (Books, Learners, Transactions)
- Each page has dedicated slots (e.g., `on_pushButton_login_clicked()`)
- Use Qt Designer for UI layout

---

## Development Workflow

### 1. Setting Up Development Environment

```bash
# Install Qt Creator (recommended IDE)
# - Download from https://www.qt.io/download

# Or use VS Code with Qt extension
# - Install C/C++ extension
# - Install Qt Configure extension
```

### 2. Building in Debug Mode

```bash
mkdir build-debug && cd build-debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```

### 3. Running with Debugging

```bash
# Linux/macOS
gdb ./bin/LibraryManagementSystem

# Or use Qt Creator's built-in debugger
```

---

## Adding New Features

### Example: Adding a New Entity (e.g., "Publisher")

#### Step 1: Create Model
```cpp
// models/Publisher.h
class Publisher {
private:
    int m_id;
    QString m_name;
    QString m_country;
    
public:
    // Getters and setters
    int getId() const { return m_id; }
    void setId(int id) { m_id = id; }
    // ... etc
};
```

#### Step 2: Add Database Table
```cpp
// In DatabaseManager::createTables()
QString createPublishersTable = R"(
    CREATE TABLE IF NOT EXISTS publishers (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        name TEXT NOT NULL,
        country TEXT,
        created_at DATETIME DEFAULT CURRENT_TIMESTAMP
    )
)";
query.exec(createPublishersTable);
```

#### Step 3: Add Database Methods
```cpp
// In DatabaseManager.h
bool addPublisher(const Publisher& publisher);
Publisher getPublisherById(int id);
QVector<Publisher> getAllPublishers();

// In DatabaseManager.cpp - implement these methods
```

#### Step 4: Create UI Page
- Open MainWindow.ui in Qt Designer
- Add new page to stacked widget
- Add forms, tables, buttons

#### Step 5: Connect UI to Logic
```cpp
// In MainWindow.h
private slots:
    void on_pushButton_addPublisher_clicked();

// In MainWindow.cpp
void MainWindow::on_pushButton_addPublisher_clicked() {
    Publisher pub;
    pub.setName(ui->lineEdit_publisherName->text());
    
    if (DatabaseManager::instance().addPublisher(pub)) {
        showSuccessMessage("Publisher added!");
    }
}
```

---

## Common Tasks

### Adding a New Table Column

1. **Modify create table SQL**:
```cpp
// Add column to CREATE TABLE statement
"website TEXT,"
```

2. **Add to model**:
```cpp
QString m_website;
QString getWebsite() const { return m_website; }
```

3. **Update INSERT/UPDATE queries**:
```cpp
query.bindValue(":website", publisher.getWebsite());
```

4. **Update UI forms** to include new field

### Adding Business Logic Validation

```cpp
// In appropriate manager class
bool DatabaseManager::canBorrowBook(int learnerId, int bookId) {
    // Check if learner has overdue books
    if (hasOverdueBooks(learnerId)) {
        return false;
    }
    
    // Check if book is available
    Book book = getBookById(bookId);
    if (!book.isAvailable()) {
        return false;
    }
    
    // Check learner's borrow limit
    int activeBorrows = getActiveTransactionsByLearnerId(learnerId).size();
    if (activeBorrows >= MAX_BORROWS_PER_LEARNER) {
        return false;
    }
    
    return true;
}
```

---

## Testing

### Manual Testing Checklist

**Authentication:**
- [ ] Login with valid credentials
- [ ] Login with invalid credentials
- [ ] Register new user
- [ ] Password recovery flow
- [ ] Role-based access restrictions

**Books:**
- [ ] Add new book
- [ ] Add book with duplicate code (should fail)
- [ ] Copy from ISBN
- [ ] Update book details
- [ ] Delete book
- [ ] Search books

**Learners:**
- [ ] Add new learner
- [ ] Update learner info
- [ ] View learner profile
- [ ] Search learners

**Transactions:**
- [ ] Borrow book (normal case)
- [ ] Borrow when learner has overdue books (should fail)
- [ ] Return book on time
- [ ] Return overdue book
- [ ] Mark book as lost

**Reports:**
- [ ] Generate borrow report
- [ ] Generate return report
- [ ] Print report
- [ ] Save as PDF

### Database Testing

```sql
-- Check database integrity
PRAGMA integrity_check;

-- View all tables
SELECT name FROM sqlite_master WHERE type='table';

-- Count records
SELECT COUNT(*) FROM books;
SELECT COUNT(*) FROM learners;
SELECT COUNT(*) FROM transactions;

-- Find orphaned records
SELECT * FROM transactions 
WHERE learner_id NOT IN (SELECT id FROM learners);
```

---

## Debugging Tips

### Common Issues

**1. Database Locked**
```cpp
// Solution: Ensure transactions are properly committed
m_database.commit();  // or rollback()
```

**2. NULL Pointer Access**
```cpp
// Always check if object was found
Learner learner = DatabaseManager::instance().getLearnerById(id);
if (learner.getId() == -1) {
    // Not found, handle error
    return;
}
// Safe to use learner
```

**3. UI Not Updating**
```cpp
// Force table refresh
ui->tableWidget_books->viewport()->update();

// Or reload data
loadAllBooks();
```

### Logging

Add debug output:
```cpp
#include <QDebug>

qDebug() << "Book ID:" << book.getId();
qDebug() << "Transaction status:" << trans.getStatusString();
```

View in Qt Creator's "Application Output" panel.

---

## Performance Considerations

### Database Optimization

**Use Indexes:**
```sql
CREATE INDEX idx_book_code ON books(book_code);
CREATE INDEX idx_learner_id ON transactions(learner_id);
CREATE INDEX idx_book_status ON books(status);
```

**Batch Operations:**
```cpp
// Instead of multiple inserts
m_database.transaction();
for (const Book& book : books) {
    addBook(book);
}
m_database.commit();
```

**Prepared Statements:**
```cpp
// Already used in DatabaseManager
query.prepare("SELECT * FROM books WHERE id = :id");
query.bindValue(":id", bookId);
```

### UI Performance

**Lazy Loading:**
```cpp
// Load only visible data
void MainWindow::loadBooksPage(int page, int pageSize) {
    int offset = page * pageSize;
    // Load books with LIMIT and OFFSET
}
```

**Asynchronous Operations:**
```cpp
// For long-running operations
QFuture<QVector<Book>> future = QtConcurrent::run([]{
    return DatabaseManager::instance().getAllBooks();
});
```

---

## Code Style Guidelines

### Naming Conventions

```cpp
// Classes: PascalCase
class DatabaseManager { };

// Methods: camelCase
void addBook();

// Member variables: m_ prefix + camelCase
int m_selectedBookId;

// Constants: UPPER_SNAKE_CASE
const int MAX_BOOKS_PER_LEARNER = 10;

// UI objects: match Qt Designer names
ui->pushButton_login
```

### Comments

```cpp
// Use comments for complex logic
// Calculate days overdue accounting for weekends
int daysOverdue = calculateBusinessDays(dueDate, returnDate);

// Document public APIs
/**
 * Borrows a book for a learner
 * @param learnerId The ID of the learner
 * @param bookId The ID of the book
 * @param borrowDate The date of borrowing
 * @return true if successful, false otherwise
 */
bool borrowBook(int learnerId, int bookId, const QDate& borrowDate);
```

### Error Handling

```cpp
// Always handle errors
if (!DatabaseManager::instance().addBook(book)) {
    QString error = DatabaseManager::instance().getLastError();
    QMessageBox::critical(this, "Error", error);
    return;
}

// Use try-catch for Qt exceptions
try {
    // Database operations
} catch (const std::exception& e) {
    qCritical() << "Exception:" << e.what();
}
```

---

## Useful Qt Resources

### Documentation
- [Qt Documentation](https://doc.qt.io/)
- [Qt Widgets](https://doc.qt.io/qt-5/qtwidgets-index.html)
- [Qt SQL Module](https://doc.qt.io/qt-5/qtsql-index.html)

### Qt Creator Shortcuts
- `Ctrl + B` - Build
- `F5` - Start Debugging
- `Ctrl + Space` - Auto-complete
- `F2` - Go to definition
- `Ctrl + Shift + R` - Rename symbol

### Debugging Tools
- Qt Creator's debugger (integrated GDB)
- Valgrind for memory leaks
- Cppcheck for static analysis

---

## Contributing Guidelines

1. **Create feature branch**: `git checkout -b feature/my-feature`
2. **Follow code style**: Use project's naming conventions
3. **Test thoroughly**: Manual testing checklist
4. **Document changes**: Update README and code comments
5. **Commit messages**: Use clear, descriptive messages
6. **Pull request**: Describe changes and testing done

---

## Future Enhancements

Potential features to add:

- [ ] User profile settings page
- [ ] Email notifications for overdue books
- [ ] Barcode scanner integration
- [ ] Advanced reporting (charts, graphs)
- [ ] Export data to CSV/Excel
- [ ] Backup and restore functionality
- [ ] Multi-language support
- [ ] Dark mode theme
- [ ] Audit log for all operations
- [ ] Fine calculation for overdue days

---

## Contact

For technical questions or contributions, contact the development team or create an issue on GitHub.

**Happy Coding! 🚀**