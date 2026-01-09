#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "AuthManager.h"
#include "DatabaseManager.h"
#include <QMessageBox>
#include <QDate>
#include <QPrinter>
#include <QPrintDialog>
#include <QFileDialog>
#include <QTextDocument>
#include <User.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_selectedBookId(-1)
    , m_selectedLearnerId(-1)
    , m_selectedTransactionId(-1)
    , m_menuExpanded(true)
{
    ui->setupUi(this);
    initializeUI();
    setupConnections();
    showLoginPage();
}

MainWindow::~MainWindow() {
    delete ui;
}

// ==================== Initialization ====================

void MainWindow::initializeUI() {
    // Set window properties
    setWindowTitle("Library Management System");
    
    // Setup combo boxes
    setupComboBoxes();
    
    // Setup table headers
    setupTableHeaders();
    
    // Set default date values
    ui->dateEdit_borrowDate->setDate(QDate::currentDate());
    ui->dateEdit_dueDate->setDate(Transaction::calculateDueDate(QDate::currentDate()));
    ui->dateEdit_returnDate->setDate(QDate::currentDate());
    
    // Hide password fields
    ui->lineEdit_loginPassword->setEchoMode(QLineEdit::Password);
    ui->lineEdit_regPassword->setEchoMode(QLineEdit::Password);
    ui->lineEdit_regRepeatPassword->setEchoMode(QLineEdit::Password);
    ui->lineEdit_retrieveNewPassword->setEchoMode(QLineEdit::Password);
    ui->lineEdit_retrieveConfirmPassword->setEchoMode(QLineEdit::Password);
    
    // Initialize password reset frames
    ui->frame_retrieveSecurityQuestionCard->setVisible(false);
    ui->frame_newPassword->setVisible(false);
}

void MainWindow::setupConnections() {
    // Additional custom connections can be added here
    // Most connections are handled by auto-connect (on_objectName_signal pattern)
}

void MainWindow::setupComboBoxes() {
    // Grade combo boxes
    QStringList grades = {"8", "9", "10", "11", "12"};
    
    ui->comboBox_bookGrade->addItems(grades);
    ui->comboBox_editBookGrade->addItems(grades);
    ui->comboBox_addLearnerGrade->addItems(grades);
    ui->comboBox__editLearnerGrade->addItems(grades);
    ui->comboBox_filterLearnerGrade->addItem("All Grades");
    ui->comboBox_filterLearnerGrade->addItems(grades);
    
    // Subject combo boxes
    QStringList subjects = {"Mathematics", "Physical Sciences", "Life Sciences", 
                           "English", "Afrikaans", "History", "Geography", 
                           "Accounting", "Business Studies", "Economics"};
    
    ui->comboBox_bookSubject->addItems(subjects);
    ui->comboBox_editBookSubject->addItems(subjects);
    
    // Book status
    QStringList statuses = {"Available", "Borrowed", "Lost"};
    ui->comboBox_editBookStatus->addItems(statuses);
    
    // Sort options
    ui->comboBox_sortBooks->addItem("Sort by Title");
    ui->comboBox_sortBooks->addItem("Sort by Author");
    ui->comboBox_sortBooks->addItem("Sort by Grade");
    ui->comboBox_sortBooks->addItem("Sort by Subject");
    
    // Filter status
    ui->comboBox_filterStatus->addItem("All");
    ui->comboBox_filterStatus->addItem("Active");
    ui->comboBox_filterStatus->addItem("Returned");
    ui->comboBox_filterStatus->addItem("Lost");
    
    // Role combo box
    ui->QComboBox_regRole->addItem("Librarian");
    ui->QComboBox_regRole->addItem("Admin");
    ui->QComboBox_regRole->addItem("Finance");
    
    // Security questions
    QStringList securityQuestions = {
        "What is your mother's maiden name?",
        "What was the name of your first pet?",
        "What city were you born in?",
        "What is your favorite book?",
        "What was your first car?"
    };
    ui->QComboBox_securityQuestion->addItems(securityQuestions);
}

void MainWindow::setupTableHeaders() {
    // Books table
    ui->tableWidget_books->setColumnCount(8);
    ui->tableWidget_books->setHorizontalHeaderLabels({
        "ID", "Book Code", "Title", "Author", "Subject", "Grade", "Price", "Status"
    });
    ui->tableWidget_books->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_books->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget_books->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget_books->setColumnHidden(0, true); // Hide ID column
    
    // Learners table
    ui->tableWidget_viewLearnersList->setColumnCount(6);
    ui->tableWidget_viewLearnersList->setHorizontalHeaderLabels({
        "ID", "Name", "Surname", "Grade", "DOB", "Contact"
    });
    ui->tableWidget_viewLearnersList->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_viewLearnersList->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget_viewLearnersList->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget_viewLearnersList->setColumnHidden(0, true); // Hide ID column
    
    // Transactions table
    ui->tableWidget_transactionHistory->setColumnCount(7);
    ui->tableWidget_transactionHistory->setHorizontalHeaderLabels({
        "ID", "Book Title", "Borrow Date", "Due Date", "Return Date", "Status", "Days Overdue"
    });
    ui->tableWidget_transactionHistory->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_transactionHistory->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget_transactionHistory->setColumnHidden(0, true);
    
    // Return books table
    ui->tableWidget_returnBooks->setColumnCount(6);
    ui->tableWidget_returnBooks->setHorizontalHeaderLabels({
        "Transaction ID", "Book Code", "Book Title", "Borrow Date", "Due Date", "Status"
    });
    ui->tableWidget_returnBooks->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_returnBooks->horizontalHeader()->setStretchLastSection(true);
    
    // Dashboard recent transactions
    ui->tableWidget_homeRecentTransactions->setColumnCount(5);
    ui->tableWidget_homeRecentTransactions->setHorizontalHeaderLabels({
        "Learner", "Book", "Date", "Type", "Status"
    });
    ui->tableWidget_homeRecentTransactions->horizontalHeader()->setStretchLastSection(true);
    
    // Currently borrowed books (profile)
    ui->tableWidget_currentlyBorrowedBooks->setColumnCount(5);
    ui->tableWidget_currentlyBorrowedBooks->setHorizontalHeaderLabels({
        "Book Title", "Borrow Date", "Due Date", "Days Left", "Status"
    });
    ui->tableWidget_currentlyBorrowedBooks->horizontalHeader()->setStretchLastSection(true);
}

void MainWindow::applyPermissions() {
    // Apply role-based permissions
    bool isAdmin = AuthManager::instance().hasAdminPermission();
    bool isLibrarian = AuthManager::instance().hasLibrarianPermission();
    bool isFinance = AuthManager::instance().hasFinancePermission();
    
    // Show/hide menu items based on permissions
    // Only admins can see settings (user management)
    ui->pushButton_settingsSidebar->setVisible(isAdmin);
    ui->pushButton_settingsIconSection->setVisible(isAdmin);
    
    // Librarians and admins can manage books and learners
    ui->pushButton_booksSidebar->setEnabled(isLibrarian);
    ui->pushButton_learnersSidebar->setEnabled(isLibrarian);
    ui->pushButton_transactSidebar->setEnabled(isLibrarian);
    
    // Finance can view reports
    ui->pushButton_reportsSidebar->setEnabled(isLibrarian || isFinance);
}

// ==================== Login & Registration ====================

void MainWindow::on_pushButton_login_clicked() {
    QString username = ui->lineEdit_loginUsername->text().trimmed();
    QString password = ui->lineEdit_loginPassword->text();
    
    if (username.isEmpty() || password.isEmpty()) {
        showErrorMessage("Please enter both username and password");
        return;
    }
    
    if (AuthManager::instance().login(username, password)) {
        showHomePage();
        updateUserInfo();
        applyPermissions();
        loadDashboardData();
        
        ui->lineEdit_loginUsername->clear();
        ui->lineEdit_loginPassword->clear();
        
        showSuccessMessage("Welcome, " + AuthManager::instance().getCurrentUser().getFullName());
    } else {
        showErrorMessage(AuthManager::instance().getLastError());
    }
}

void MainWindow::on_pushButton_register_clicked() {
    showRegisterPage();
}

void MainWindow::on_pushButton_regBackToLogin_clicked() {
    showLoginPage();
    clearRegistrationForm();
}

void MainWindow::on_pushButton_registerSubmit_clicked() {
    if (!validateRegistrationForm()) {
        return;
    }
    
    // Create user object
    User newUser;
    // Use email as username since it's unique and doesn't contain spaces
    QString email = ui->lineEdit_regEmail->text().trimmed();
    newUser.setUsername(email);
    newUser.setName(ui->lineEdit_regName->text().trimmed());
    newUser.setSurname(ui->lineEdit_regSurname->text().trimmed());
    newUser.setEmail(email);
    newUser.setContactNo(ui->lineEdit_regContactNo->text().trimmed());
    newUser.setSchoolName(ui->lineEdit_regSchoolName->text().trimmed());
    newUser.setRole(User::stringToRole(ui->QComboBox_regRole->currentText()));
    newUser.setSecurityQuestion(ui->QComboBox_securityQuestion->currentText());
    newUser.setSecurityAnswer(ui->lineEdit_regSecurityAnswer->text().trimmed());
    
    QString password = ui->lineEdit_regPassword->text();
    
    if (AuthManager::instance().registerUser(newUser, password)) {
        showSuccessMessage("Registration successful! You can now login using your email address: " + email);
        showLoginPage();
        clearRegistrationForm();
    } else {
        showErrorMessage(AuthManager::instance().getLastError());
    }
}

void MainWindow::on_pushButton_quit_clicked() {
    QApplication::quit();
}

void MainWindow::on_pushButton_forgotPassword_clicked() {
    showResetPasswordPage();
}

void MainWindow::on_pushButton_retrieveVerifyEmail_clicked() {
    QString email = ui->lineEdit_retrieveEmail->text().trimmed();
    
    if (email.isEmpty()) {
        showErrorMessage("Please enter your email address");
        return;
    }
    
    if (AuthManager::instance().verifyEmail(email)) {
        m_currentEmail = email;
        QString question = AuthManager::instance().getSecurityQuestion(email);
        
        ui->label_retrieveSecurityQuestion->setText(question);
        ui->frame_retrieveSecurityQuestionCard->setVisible(true);
        ui->lineEdit_retrieveEmail->setEnabled(false);
        ui->pushButton_retrieveVerifyEmail->setEnabled(false);
    } else {
        showErrorMessage(AuthManager::instance().getLastError());
    }
}

void MainWindow::on_pushButton_retrieveVerifyAnswer_clicked() {
    QString answer = ui->lineEdit_retrieveSecurityAnswer->text().trimmed();
    
    if (answer.isEmpty()) {
        showErrorMessage("Please enter your security answer");
        return;
    }
    
    if (AuthManager::instance().verifySecurityAnswer(m_currentEmail, answer)) {
        ui->frame_newPassword->setVisible(true);
        ui->frame_retrieveSecurityQuestionCard->setEnabled(false);
        ui->pushButton_retrieveVerifyAnswer->setEnabled(false);
    } else {
        showErrorMessage(AuthManager::instance().getLastError());
    }
}

void MainWindow::on_pushButton_retrieveSubmit_clicked() {
    if (!validatePasswordResetForm()) {
        return;
    }
    
    QString newPassword = ui->lineEdit_retrieveNewPassword->text();
    
    if (AuthManager::instance().resetPassword(m_currentEmail, newPassword)) {
        showSuccessMessage("Password reset successful! You can now login with your new password.");
        showLoginPage();
        
        // Clear reset form
        ui->lineEdit_retrieveEmail->clear();
        ui->lineEdit_retrieveSecurityAnswer->clear();
        ui->lineEdit_retrieveNewPassword->clear();
        ui->lineEdit_retrieveConfirmPassword->clear();
        ui->frame_retrieveSecurityQuestionCard->setVisible(false);
        ui->frame_newPassword->setVisible(false);
        ui->lineEdit_retrieveEmail->setEnabled(true);
        ui->pushButton_retrieveVerifyEmail->setEnabled(true);
        ui->pushButton_retrieveVerifyAnswer->setEnabled(true);
    } else {
        showErrorMessage(AuthManager::instance().getLastError());
    }
}

void MainWindow::on_pushButton_retrieveCancel_clicked() {
    showLoginPage();
    
    // Clear and reset form
    ui->lineEdit_retrieveEmail->clear();
    ui->lineEdit_retrieveSecurityAnswer->clear();
    ui->lineEdit_retrieveNewPassword->clear();
    ui->lineEdit_retrieveConfirmPassword->clear();
    ui->frame_retrieveSecurityQuestionCard->setVisible(false);
    ui->frame_newPassword->setVisible(false);
    ui->lineEdit_retrieveEmail->setEnabled(true);
    ui->pushButton_retrieveVerifyEmail->setEnabled(true);
    ui->pushButton_retrieveVerifyAnswer->setEnabled(true);
}

// ==================== Navigation ====================

void MainWindow::on_pushButton_homeIconSection_clicked() {
    showDashboardPage();
    loadDashboardData();
}

void MainWindow::on_pushButton_logoutSidebar_clicked() {
    AuthManager::instance().logout();
    showLoginPage();
    showInfoMessage("You have been logged out successfully");
}

void MainWindow::on_pushButton_logoutTopBar_clicked() {
    on_pushButton_logoutSidebar_clicked();
}

void MainWindow::on_pushButton_toogleMenu_clicked() {
    toggleSidebar();
}

void MainWindow::on_pushButton_booksIconSection_clicked() {
    showAddBookPage();
    loadAllBooks();
}

void MainWindow::on_pushButton_booksSidebar_clicked() {
    showAddBookPage();
    loadAllBooks();
}

void MainWindow::on_pushButton_learnersIconSection_clicked() {
    showAddLearnerPage();
    loadAllLearners();
}

void MainWindow::on_pushButton_learnersSidebar_clicked() {
    showAddLearnerPage();
    loadAllLearners();
}

void MainWindow::on_pushButton_transactionIconSection_clicked() {
    showBorrowBookPage();
}

void MainWindow::on_pushButton_transactSidebar_clicked() {
    showBorrowBookPage();
}

void MainWindow::on_pushButton_reportsIconSection_clicked() {
    showReportsPage();
}

void MainWindow::on_pushButton_reportsSidebar_clicked() {
    showReportsPage();
}

void MainWindow::on_pushButton_settingsIconSection_clicked() {
    // TODO: Implement settings page
    showInfoMessage("Settings page - Coming soon!");
}

void MainWindow::on_pushButton_settingsSidebar_clicked() {
    on_pushButton_settingsIconSection_clicked();
}

// ==================== Page Navigation Methods ====================

void MainWindow::showLoginPage() {
    ui->stackedWidget_mainPages->setCurrentWidget(ui->page_login);
}

void MainWindow::showRegisterPage() {
    ui->stackedWidget_mainPages->setCurrentWidget(ui->page_register);
}

void MainWindow::showResetPasswordPage() {
    ui->stackedWidget_mainPages->setCurrentWidget(ui->page_resetPassword);
}

void MainWindow::showHomePage() {
    ui->stackedWidget_mainPages->setCurrentWidget(ui->page_home);
    showDashboardPage();
}

void MainWindow::showDashboardPage() {
    ui->page_mainContent->setCurrentWidget(ui->page_dashboard);
}

void MainWindow::showBooksPage() {
    ui->stackedWidget_bookManagement->setCurrentWidget(ui->page_addViewBooks);
}

void MainWindow::showAddBookPage() {
    ui->stackedWidget_bookManagement->setCurrentWidget(ui->page_addViewBooks);
    ui->page_mainContent->setCurrentWidget(ui->page_booksManagement);
}

void MainWindow::showUpdateBookPage() {
    ui->stackedWidget_bookManagement->setCurrentWidget(ui->page_updateBookInfo);
    ui->page_mainContent->setCurrentWidget(ui->page_booksManagement);
}

void MainWindow::showLearnersPage() {
    ui->stackedWidget_learnersPages_2->setCurrentWidget(ui->page_addViewLearnerInfo);
}

void MainWindow::showAddLearnerPage() {
    ui->stackedWidget_learnersPages_2->setCurrentWidget(ui->page_addViewLearnerInfo);
    ui->page_mainContent->setCurrentWidget(ui->page_LearnerManagement);
}

void MainWindow::showLearnerProfilePage() {
    ui->stackedWidget_learnersPages->setCurrentWidget(ui->page_Profile);
    ui->stackedWidget_learnersPages_2->setCurrentWidget(ui->page_learnerProfile);
    ui->page_mainContent->setCurrentWidget(ui->page_LearnerManagement);
}

void MainWindow::showUpdateLearnerPage() {
    ui->stackedWidget_learnersPages_2->setCurrentWidget(ui->page_updateLearnerInfo);
    ui->page_mainContent->setCurrentWidget(ui->page_LearnerManagement);
}

void MainWindow::showBorrowBookPage() {
    ui->stackedWidget_transactionPages->setCurrentWidget(ui->page_borrowBook);
    ui->page_mainContent->setCurrentWidget(ui->page_transact);
}

void MainWindow::showReturnBookPage() {
    ui->stackedWidget_transactionPages->setCurrentWidget(ui->page_returnBook);
    ui->page_mainContent->setCurrentWidget(ui->page_transact);
}

void MainWindow::showTransactionHistoryPage() {
    ui->stackedWidget_learnersPages->setCurrentWidget(ui->page_transactionHistory);
    ui->page_mainContent->setCurrentWidget(ui->page_transact);
}

void MainWindow::showReportsPage() {
    ui->stackedWidget_transactionPages->setCurrentWidget(ui->page_reports);
    ui->page_mainContent->setCurrentWidget(ui->page_transact);
}
// ==================== Dashboard ====================

void MainWindow::loadDashboardData() {
    updateDashboardStats();
    loadRecentTransactions();
}

void MainWindow::updateDashboardStats() {
    auto stats = DatabaseManager::instance().getDashboardStats();
    
    ui->label_totalBooksDisplay->setText(QString::number(stats.totalBooks));
    ui->label_booksAvailableDisplay->setText(QString::number(stats.availableBooks));
    ui->label_BooksCurrentlyBorrowedDisplay->setText(QString::number(stats.borrowedBooks));
    ui->label_totalActiveLearnersDisplay->setText(QString::number(stats.activeLearners));
    ui->label_totalUsersDisplay->setText(QString::number(stats.totalUsers));
    ui->label_unreturnedBooksDisplay->setText(QString::number(stats.overdueBooks));
}

void MainWindow::loadRecentTransactions() {
    QVector<Transaction> transactions = DatabaseManager::instance().getRecentTransactions(10);
    populateDashboardTransactions(transactions);
}

void MainWindow::on_pushButton_addBookQuickButton_clicked() {
    showAddBookPage();
    loadAllBooks();
}

void MainWindow::on_pushButton_issueBookQuickButton_clicked() {
    showBorrowBookPage();
}

void MainWindow::on_pushButton_returnBookQuickButton_clicked() {
    showReturnBookPage();
}

// ==================== Book Management ====================

void MainWindow::on_pushButton_addBookSidebar_clicked() {
    showAddBookPage();
    loadAllBooks();
}

void MainWindow::on_pushButton_updateBookInfoSidebar_clicked() {
    if (m_selectedBookId == -1) {
        showErrorMessage("Please select a book first");
        return;
    }
    showUpdateBookPage();
    loadBookDetails(m_selectedBookId);
}

void MainWindow::on_pushButton_availableBooksSidebar_clicked() {
    showAddBookPage();
    QVector<Book> availableBooks = DatabaseManager::instance().getBooksByStatus(Book::Status::Available);
    populateBooksTable(availableBooks);
}

void MainWindow::on_pushButton_confirmAdd_clicked() {
    if (!validateBookForm()) {
        return;
    }
    
    Book book;
    book.setBookCode(ui->lineEdit_bookCode->text().trimmed());
    book.setIsbn(ui->lineEdit_bookISBN->text().trimmed());
    book.setTitle(ui->lineEdit_bookTitle->text().trimmed());
    book.setAuthor(ui->lineEdit_bookAuthor->text().trimmed());
    book.setSubject(ui->comboBox_bookSubject->currentText());
    book.setGrade(ui->comboBox_bookGrade->currentText());
    book.setPrice(ui->doubleSpinBox_bookPrice->value());
    book.setStatus(Book::Status::Available);
    
    if (DatabaseManager::instance().addBook(book)) {
        showSuccessMessage("Book added successfully!");
        clearBookForm();
        loadAllBooks();
    } else {
        showErrorMessage(DatabaseManager::instance().getLastError());
    }
}

void MainWindow::on_pushButton_clearForm_clicked() {
    clearBookForm();
}

void MainWindow::on_pushButton_copyFromISBN_clicked() {
    copyBookDataFromISBN();
}

void MainWindow::on_pushButton_refreshBooks_clicked() {
    loadAllBooks();
    ui->lineEdit_searchBooks->clear();
    ui->comboBox_sortBooks->setCurrentIndex(0);
}

void MainWindow::on_pushButton_bookDetails_clicked() {
    if (m_selectedBookId == -1) {
        showErrorMessage("Please select a book first");
        return;
    }
    
    Book book = DatabaseManager::instance().getBookById(m_selectedBookId);
    
    QString details = QString(
        "Book Details:\n\n"
        "Book Code: %1\n"
        "ISBN: %2\n"
        "Title: %3\n"
        "Author: %4\n"
        "Subject: %5\n"
        "Grade: %6\n"
        "Price: R%7\n"
        "Status: %8\n"
    ).arg(book.getBookCode())
     .arg(book.getIsbn())
     .arg(book.getTitle())
     .arg(book.getAuthor())
     .arg(book.getSubject())
     .arg(book.getGrade())
     .arg(book.getPrice())
     .arg(book.getStatusString());
    
    QMessageBox::information(this, "Book Details", details);
}

void MainWindow::on_pushButton_markAsLost_clicked() {
    if (m_selectedBookId == -1) {
        showErrorMessage("Please select a book first");
        return;
    }
    
    Book book = DatabaseManager::instance().getBookById(m_selectedBookId);
    
    if (book.getStatus() != Book::Status::Borrowed) {
        showErrorMessage("Only borrowed books can be marked as lost");
        return;
    }
    
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Mark Book as Lost",
        "Are you sure you want to mark this book as lost?\n\n" + book.getTitle(),
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        // Find active transaction for this book
        QVector<Transaction> transactions = DatabaseManager::instance().getTransactionsByBookId(m_selectedBookId);
        for (const Transaction& trans : transactions) {
            if (trans.isActive()) {
                if (DatabaseManager::instance().markBookAsLost(trans.getId())) {
                    showSuccessMessage("Book marked as lost");
                    loadAllBooks();
                } else {
                    showErrorMessage(DatabaseManager::instance().getLastError());
                }
                break;
            }
        }
    }
}

void MainWindow::on_lineEdit_searchBooks_textChanged(const QString &text) {
    searchBooks(text);
}

void MainWindow::on_comboBox_sortBooks_currentIndexChanged(int index) {
    loadAllBooks(); // For now, reload all books
    // TODO: Implement sorting logic
}

void MainWindow::on_tableWidget_books_cellClicked(int row, int column) {
    m_selectedBookId = ui->tableWidget_books->item(row, 0)->text().toInt();
}

// Update Book

void MainWindow::on_pushButton_saveChanges_clicked() {
    if (m_selectedBookId == -1) {
        showErrorMessage("No book selected for update");
        return;
    }
    
    Book book = DatabaseManager::instance().getBookById(m_selectedBookId);
    
    book.setBookCode(ui->label_editBookCode->text().trimmed());
    book.setIsbn(ui->lineEdit_editBookISBN->text().trimmed());
    book.setTitle(ui->lineEdit_editBookTitle->text().trimmed());
    book.setAuthor(ui->lineEdit_editBookAuthor->text().trimmed());
    book.setSubject(ui->comboBox_editBookSubject->currentText());
    book.setGrade(ui->comboBox_editBookGrade->currentText());
    book.setPrice(ui->doubleSpinBox_editBookPrice->value());
    book.setStatus(Book::stringToStatus(ui->comboBox_editBookStatus->currentText()));
    
    if (DatabaseManager::instance().updateBook(book)) {
        showSuccessMessage("Book updated successfully!");
        showAddBookPage();
        loadAllBooks();
        m_selectedBookId = -1;
    } else {
        showErrorMessage(DatabaseManager::instance().getLastError());
    }
}

void MainWindow::on_pushButton_cancelEdit_clicked() {
    showAddBookPage();
    m_selectedBookId = -1;
}

void MainWindow::on_pushButton_deleteBook_clicked() {
    //deletion must only be performed by admin
    if (m_selectedBookId == -1) {
        showErrorMessage("No book selected for deletion");
        return;
    }
    
    Book book = DatabaseManager::instance().getBookById(m_selectedBookId);
    
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Delete Book",
        "Are you sure you want to delete this book?\n\n" + book.getTitle() +
        "\n\nThis action cannot be undone!",
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        if (DatabaseManager::instance().deleteBook(m_selectedBookId)) {
            showSuccessMessage("Book deleted successfully!");
            showAddBookPage();
            loadAllBooks();
            m_selectedBookId = -1;
        } else {
            showErrorMessage(DatabaseManager::instance().getLastError());
        }
    }
}

// ==================== Learner Management ====================

void MainWindow::on_pushButton_addLearnerSidebar_clicked() {
    showAddLearnerPage();
    loadAllLearners();
}

void MainWindow::on_pushButton_updateLearnerInfoSidebar_clicked() {
    if (m_selectedLearnerId == -1) {
        showErrorMessage("Please select a learner first");
        return;
    }
    showUpdateLearnerPage();
    
    Learner learner = DatabaseManager::instance().getLearnerById(m_selectedLearnerId);
    fillLearnerForm(learner);
}

void MainWindow::on_pushButton_AddLearner_clicked() {
    if (!validateLearnerForm()) {
        return;
    }
    
    Learner learner;
    learner.setName(ui->lineEdit_addLearnerName->text().trimmed());
    learner.setSurname(ui->lineEdit_addLearnerSurname->text().trimmed());
    learner.setGrade(ui->comboBox_addLearnerGrade->currentText());
    learner.setDateOfBirth(ui->dateEdit_addLearnerDOB->date());
    learner.setContactNo(ui->lineEdit_addLearnerContact->text().trimmed());
    
    if (DatabaseManager::instance().addLearner(learner)) {
        showSuccessMessage("Learner added successfully!");
        clearLearnerForm();
        loadAllLearners();
    } else {
        showErrorMessage(DatabaseManager::instance().getLastError());
    }
}

void MainWindow::on_pushButton_addLearnerClearForm_clicked() {
    clearLearnerForm();
}

void MainWindow::on_pushButton_refreshLearnerList_clicked() {
    loadAllLearners();
    ui->lineEdit_searchLearner->clear();
    ui->comboBox_filterLearnerGrade->setCurrentIndex(0);
}

void MainWindow::on_pushButton_viewLeanerProfile_clicked() {
    if (m_selectedLearnerId == -1) {
        showErrorMessage("Please select a learner first");
        return;
    }
    
    loadLearnerProfile(m_selectedLearnerId);
    showLearnerProfilePage();
}

void MainWindow::on_pushButton_viewLearnerHistory_clicked() {
    if (m_selectedLearnerId == -1) {
        showErrorMessage("Please select a learner first");
        return;
    }
    
    loadTransactionHistory(m_selectedLearnerId);
    showTransactionHistoryPage();
}

void MainWindow::on_lineEdit_searchLearner_textChanged(const QString &text) {
    searchLearners(text);
}

void MainWindow::on_comboBox_filterLearnerGrade_currentIndexChanged(int index) {
    if (index == 0) {
        loadAllLearners();
    } else {
        filterLearnersByGrade(ui->comboBox_filterLearnerGrade->currentText());
    }
}

void MainWindow::on_tableWidget_viewLearnersList_cellClicked(int row, int column) {
    m_selectedLearnerId = ui->tableWidget_viewLearnersList->item(row, 0)->text().toInt();
}

// Learner Profile

void MainWindow::on_pushButton_backToLearners_clicked() {
    showAddLearnerPage();
    m_selectedLearnerId = -1;
}

void MainWindow::on_pushButton_editLearnerProfile_clicked() {
    if (m_selectedLearnerId == -1) {
        showErrorMessage("No learner selected");
        return;
    }
    
    showUpdateLearnerPage();
    Learner learner = DatabaseManager::instance().getLearnerById(m_selectedLearnerId);
    fillLearnerForm(learner);
}

void MainWindow::on_pushButton_viewTransactionRecord_clicked() {
    if (m_selectedLearnerId == -1) {
        showErrorMessage("No learner selected");
        return;
    }
    
    loadTransactionHistory(m_selectedLearnerId);
    showTransactionHistoryPage();
}

// Update Learner

void MainWindow::on_pushButton__editLearnerConfirm_clicked() {
    if (m_selectedLearnerId == -1) {
        showErrorMessage("No learner selected for update");
        return;
    }
    
    Learner learner = DatabaseManager::instance().getLearnerById(m_selectedLearnerId);
    
    learner.setName(ui->lineEdit_editLearnerFirstName->text().trimmed());
    learner.setSurname(ui->lineEdit_editLearnerSurname->text().trimmed());
    learner.setGrade(ui->comboBox__editLearnerGrade->currentText());
    learner.setDateOfBirth(ui->dateEdit__editLearnerDOB->date());
    learner.setContactNo(ui->lineEdit__editLearnerContact->text().trimmed());
    
    if (DatabaseManager::instance().updateLearner(learner)) {
        showSuccessMessage("Learner updated successfully!");
        showAddLearnerPage();
        loadAllLearners();
        m_selectedLearnerId = -1;
    } else {
        showErrorMessage(DatabaseManager::instance().getLastError());
    }
}

void MainWindow::on_pushButton_editLearnerCancel_clicked() {
    showAddLearnerPage();
    m_selectedLearnerId = -1;
}

// ==================== Transaction Management ====================

void MainWindow::on_pushButton_borrowBookSidebar_clicked() {
    showBorrowBookPage();
    clearBorrowForm();
}

void MainWindow::on_pushButton_returnBookSidebar_clicked() {
    showReturnBookPage();
    clearReturnForm();
}

void MainWindow::on_pushButton_viewTransactionsSidebar_clicked() {
    // Show all transactions or require learner selection
    showInfoMessage("Please select a learner to view their transaction history");
}

// Borrow Book

void MainWindow::on_pushButton_findLearner_clicked() {
    QString learnerIdText = ui->lineEdit_borrowLearnerID->text().trimmed();
    
    if (learnerIdText.isEmpty()) {
        showErrorMessage("Please enter a learner ID");
        return;
    }
    
    bool ok;
    int learnerId = learnerIdText.toInt(&ok);
    
    if (!ok) {
        showErrorMessage("Invalid learner ID");
        return;
    }
    
    Learner learner = DatabaseManager::instance().getLearnerById(learnerId);
    
    if (learner.getId() == -1) {
        showErrorMessage("Learner not found");
        ui->label_borrowLearnerName->setText("Not Found");
        ui->label_borrowLearnerGrade->setText("-");
        return;
    }
    
    // Check if learner has overdue books
    if (DatabaseManager::instance().hasOverdueBooks(learnerId)) {
        showErrorMessage("This learner has overdue books and cannot borrow more books");
        return;
    }
    
    m_selectedLearnerId = learnerId;
    ui->label_borrowLearnerName->setText(learner.getFullName());
    ui->label_borrowLearnerGrade->setText(learner.getGrade());
    
    showSuccessMessage("Learner found: " + learner.getFullName());
}

void MainWindow::on_pushButton_findBook_clicked() {
    QString bookCode = ui->lineEdit_borrowBookCode->text().trimmed();
    
    if (bookCode.isEmpty()) {
        showErrorMessage("Please enter a book code");
        return;
    }
    
    Book book = DatabaseManager::instance().getBookByCode(bookCode);
    
    if (book.getId() == -1) {
        showErrorMessage("Book not found");
        return;
    }
    
    if (!book.isAvailable()) {
        showErrorMessage("Book is not available (Status: " + book.getStatusString() + ")");
        return;
    }
    
    m_selectedBookId = book.getId();
    ui->label_borrowBookDisplay->setText(book.getTitle());
    ui->label_borrowBookAuthorDisplay->setText(book.getAuthor());
    ui->label_borrowBookISBNDisplay->setText(book.getIsbn());
    ui->label_borrowBookStatusDisplay->setText(book.getStatusString());
    
    // Update count of this ISBN
    int count = DatabaseManager::instance().getBookCountByISBN(book.getIsbn());
    ui->label_borrowBookCount->setText(QString::number(count));
    
    showSuccessMessage("Book found: " + book.getTitle());
}

void MainWindow::on_pushButton_confirmBorrow_clicked() {
    if (!validateBorrowForm()) {
        return;
    }
    
    performBorrow();
}

void MainWindow::on_pushButton_clearBorrowForm_clicked() {
    clearBorrowForm();
}

void MainWindow::on_pushButton_goToReportsFromBorrow_clicked() {
    showReportsPage();
}

// Return Book

void MainWindow::on_radioButton_searchByLearner_clicked() {
    ui->label_searchBy->setText("Enter Learner ID:");
}

void MainWindow::on_radioButton_searchByBook_clicked() {
    ui->label_searchBy->setText("Enter Book Code:");
}

void MainWindow::on_pushButton_searchReturn_clicked() {
    QString searchText = ui->lineEdit_returnSearch->text().trimmed();
    
    if (searchText.isEmpty()) {
        showErrorMessage("Please enter a search term");
        return;
    }
    
    if (ui->radioButton_searchByLearner->isChecked()) {
        bool ok;
        int learnerId = searchText.toInt(&ok);
        
        if (!ok) {
            showErrorMessage("Invalid learner ID");
            return;
        }
        
        loadActiveTransactionsForReturn(learnerId);
    } else {
        loadTransactionByBookCode(searchText);
    }
}

void MainWindow::on_tableWidget_returnBooks_cellClicked(int row, int column) {
    m_selectedTransactionId = ui->tableWidget_returnBooks->item(row, 0)->text().toInt();
    
    Transaction trans = DatabaseManager::instance().getTransactionById(m_selectedTransactionId);
    Book book = DatabaseManager::instance().getBookById(trans.getBookId());
    
    ui->label_returnSelectedBook->setText(book.getTitle());
}

void MainWindow::on_pushButton_processReturn_clicked() {
    if (m_selectedTransactionId == -1) {
        showErrorMessage("Please select a transaction to return");
        return;
    }
    
    performReturn();
}

void MainWindow::on_pushButton_clearReturnForm_clicked() {
    clearReturnForm();
}

void MainWindow::on_pushButton_markBookAsLost_clicked() {
    if (m_selectedTransactionId == -1) {
        showErrorMessage("Please select a transaction first");
        return;
    }
    
    performMarkAsLost();
}

void MainWindow::on_pushButton_goToReportsFromReturn_clicked() {
    showReportsPage();
}

// Transaction History

void MainWindow::on_pushButton_BackToLearnersList_clicked() {
    showAddLearnerPage();
}

void MainWindow::on_pushButton_backToProfile_clicked() {
    loadLearnerProfile(m_selectedLearnerId);
    showLearnerProfilePage();
}

void MainWindow::on_pushButton_filterHistory_clicked() {
    // Apply date and status filters
    QDate startDate = ui->dateEdit_filterFrom->date();
    QDate endDate = ui->dateEdit_filterTo->date();
    
    QVector<Transaction> transactions = DatabaseManager::instance().getTransactionsByLearnerId(m_selectedLearnerId);
    
    // Filter by date range and status
    QVector<Transaction> filtered;
    QString statusFilter = ui->comboBox_filterStatus->currentText();
    
    for (const Transaction& trans : transactions) {
        bool dateMatch = trans.getBorrowDate() >= startDate && trans.getBorrowDate() <= endDate;
        bool statusMatch = (statusFilter == "All") || (trans.getStatusString() == statusFilter);
        
        if (dateMatch && statusMatch) {
            filtered.append(trans);
        }
    }
    
    populateTransactionsTable(filtered);
}

void MainWindow::on_pushButton_goToReports_clicked() {
    showReportsPage();
}

// ==================== Reports ====================

void MainWindow::on_pushButton_printReportSidebar_clicked() {
    showReportsPage();
}

void MainWindow::on_pushButton_loadLearnerData_clicked() {
    QString learnerIdText = ui->lineEdit_reportLearnerID->text().trimmed();
    
    if (learnerIdText.isEmpty()) {
        showErrorMessage("Please enter a learner ID");
        return;
    }
    
    bool ok;
    int learnerId = learnerIdText.toInt(&ok);
    
    if (!ok) {
        showErrorMessage("Invalid learner ID");
        return;
    }
    
    Learner learner = DatabaseManager::instance().getLearnerById(learnerId);
    
    if (learner.getId() == -1) {
        showErrorMessage("Learner not found");
        return;
    }
    
    m_selectedLearnerId = learnerId;
    ui->label_reportLearnerName->setText(learner.getFullName());
    ui->label_reportLearnerGrade->setText(learner.getGrade());
    
    showSuccessMessage("Learner loaded: " + learner.getFullName());
}

void MainWindow::on_pushButton_generateReport_clicked() {
    if (m_selectedLearnerId == -1) {
        showErrorMessage("Please load learner data first");
        return;
    }
    
    QString reportType;
    if (ui->radioButton_borrowReport->isChecked()) {
        reportType = "Borrow";
        generateBorrowReport(m_selectedLearnerId);
    } else if (ui->radioButton_returnReport->isChecked()) {
        reportType = "Return";
        generateReturnReport(m_selectedLearnerId);
    } else {
        showErrorMessage("Please select a report type");
        return;
    }
}

void MainWindow::on_pushButton_printReport_clicked() {
    printReport();
}

void MainWindow::on_pushButton_saveReportPDF_clicked() {
    saveReportAsPDF();
}

void MainWindow::on_pushButton_clearPreview_clicked() {
    ui->textEdit_reportPreview->clear();
    ui->lineEdit_reportLearnerID->clear();
    ui->label_reportLearnerName->clear();
    ui->label_reportLearnerGrade->clear();
    m_selectedLearnerId = -1;
}

void MainWindow::on_pushButton_backToMenu_clicked() {
    showDashboardPage();
}

void MainWindow::on_radioButton_borrowReport_clicked() {
    // Radio button selected
}

void MainWindow::on_radioButton_returnReport_clicked() {
    // Radio button selected
}

// ==================== Helper Methods ====================

void MainWindow::updateUserInfo() {
    User currentUser = AuthManager::instance().getCurrentUser();
    
    ui->label_usernameCurrentUser->setText(currentUser.getFullName());
    ui->label_roleCurrentUser->setText(currentUser.getRoleString());
    
    // Update date in top bar
    ui->label_greetingsDateTopBar->setText("Hello, " + currentUser.getName() + "\n" + QDate::currentDate().toString("dddd, MMMM d, yyyy"));
}

void MainWindow::toggleSidebar() {
    m_menuExpanded = !m_menuExpanded;
    
    // Toggle visibility of text labels in sidebar
    ui->frame_sideText->setVisible(m_menuExpanded);
    ui->frame_sidebarBooksOptions->setVisible(m_menuExpanded);
    ui->frame_sidebarLearnersOptions->setVisible(m_menuExpanded);
    ui->frame_sidebarTransactOptions->setVisible(m_menuExpanded);
    ui->frame_sidebarReportOptions->setVisible(m_menuExpanded);
    
    // Adjust width
    if (m_menuExpanded) {
        ui->frame_iconOnly->setMaximumWidth(200);
    } else {
        ui->frame_iconOnly->setMaximumWidth(60);
    }
}

void MainWindow::showSuccessMessage(const QString& message) {
    QMessageBox::information(this, "Success", message);
}

void MainWindow::showErrorMessage(const QString& message) {
    QMessageBox::critical(this, "Error", message);
}

void MainWindow::showInfoMessage(const QString& message) {
    QMessageBox::information(this, "Information", message);
}

// ==================== Validation ====================

bool MainWindow::validateBookForm() {
    if (ui->lineEdit_bookCode->text().trimmed().isEmpty()) {
        showErrorMessage("Book code is required");
        return false;
    }
    
    if (DatabaseManager::instance().bookCodeExists(ui->lineEdit_bookCode->text().trimmed())) {
        showErrorMessage("Book code already exists");
        return false;
    }
    
    if (ui->lineEdit_bookISBN->text().trimmed().isEmpty()) {
        showErrorMessage("ISBN is required");
        return false;
    }
    
    if (ui->lineEdit_bookTitle->text().trimmed().isEmpty()) {
        showErrorMessage("Book title is required");
        return false;
    }
    
    if (ui->lineEdit_bookAuthor->text().trimmed().isEmpty()) {
        showErrorMessage("Author is required");
        return false;
    }
    
    return true;
}

bool MainWindow::validateLearnerForm() {
    if (ui->lineEdit_addLearnerName->text().trimmed().isEmpty()) {
        showErrorMessage("Learner name is required");
        return false;
    }
    
    if (ui->lineEdit_addLearnerSurname->text().trimmed().isEmpty()) {
        showErrorMessage("Learner surname is required");
        return false;
    }
    
    if (!ui->dateEdit_addLearnerDOB->date().isValid()) {
        showErrorMessage("Valid date of birth is required");
        return false;
    }
    
    return true;
}

bool MainWindow::validateBorrowForm() {
    if (m_selectedLearnerId == -1) {
        showErrorMessage("Please find and select a learner first");
        return false;
    }
    
    if (m_selectedBookId == -1) {
        showErrorMessage("Please find and select a book first");
        return false;
    }
    
    return true;
}

bool MainWindow::validateRegistrationForm() {
    if (ui->lineEdit_regName->text().trimmed().isEmpty()) {
        showErrorMessage("Name is required");
        return false;
    }
    
    if (ui->lineEdit_regSurname->text().trimmed().isEmpty()) {
        showErrorMessage("Surname is required");
        return false;
    }
    
    QString password = ui->lineEdit_regPassword->text();
    QString repeatPassword = ui->lineEdit_regRepeatPassword->text();
    
    if (password != repeatPassword) {
        showErrorMessage("Passwords do not match");
        return false;
    }
    
    auto passwordResult = AuthManager::instance().validatePassword(password);
    if (!passwordResult.isValid) {
        showErrorMessage(passwordResult.message);
        return false;
    }
    
    auto emailResult = AuthManager::instance().validateEmail(ui->lineEdit_regEmail->text());
    if (!emailResult.isValid) {
        showErrorMessage(emailResult.message);
        return false;
    }
    
    if (ui->lineEdit_regSecurityAnswer->text().trimmed().isEmpty()) {
        showErrorMessage("Security answer is required");
        return false;
    }
    
    return true;
}

bool MainWindow::validatePasswordResetForm() {
    QString newPassword = ui->lineEdit_retrieveNewPassword->text();
    QString confirmPassword = ui->lineEdit_retrieveConfirmPassword->text();
    
    if (newPassword != confirmPassword) {
        showErrorMessage("Passwords do not match");
        return false;
    }
    
    auto passwordResult = AuthManager::instance().validatePassword(newPassword);
    if (!passwordResult.isValid) {
        showErrorMessage(passwordResult.message);
        return false;
    }
    
    return true;
}

// ==================== Business Logic ====================

void MainWindow::performBorrow() {
    QDate borrowDate = ui->dateEdit_borrowDate->date();
    
    if (DatabaseManager::instance().borrowBook(m_selectedLearnerId, m_selectedBookId, borrowDate)) {
        showSuccessMessage("Book borrowed successfully!");
        clearBorrowForm();
        loadDashboardData();
    } else {
        showErrorMessage(DatabaseManager::instance().getLastError());
    }
}

void MainWindow::performReturn() {
    QDate returnDate = ui->dateEdit_returnDate->date();
    
    if (DatabaseManager::instance().returnBook(m_selectedTransactionId, returnDate)) {
        showSuccessMessage("Book returned successfully!");
        clearReturnForm();
        loadDashboardData();
    } else {
        showErrorMessage(DatabaseManager::instance().getLastError());
    }
}

void MainWindow::performMarkAsLost() {
    Transaction trans = DatabaseManager::instance().getTransactionById(m_selectedTransactionId);
    Book book = DatabaseManager::instance().getBookById(trans.getBookId());
    
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Mark Book as Lost",
        "Are you sure you want to mark this book as lost?\n\n" +
        book.getTitle() + "\n\nThe learner will be charged R" +
        QString::number(book.getPrice(), 'f', 2),
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        if (DatabaseManager::instance().markBookAsLost(m_selectedTransactionId)) {
            showSuccessMessage("Book marked as lost. Learner will be charged R" +
                             QString::number(book.getPrice(), 'f', 2));
            clearReturnForm();
            loadDashboardData();
        } else {
            showErrorMessage(DatabaseManager::instance().getLastError());
        }
    }
}

void MainWindow::calculateAndDisplayAmount(int learnerId) {
    double amount = DatabaseManager::instance().calculateUnreturnedBooksAmount(learnerId);
    // Display in appropriate label
    showInfoMessage("Total amount due: R" + QString::number(amount, 'f', 2));
}
// ==================== Data Loading ====================

void MainWindow::loadAllBooks() {
    QVector<Book> books = DatabaseManager::instance().getAllBooks();
    populateBooksTable(books);
}

void MainWindow::loadAllLearners() {
    QVector<Learner> learners = DatabaseManager::instance().getAllLearners();
    populateLearnersTable(learners);
}

void MainWindow::loadLearnerProfile(int learnerId) {
    Learner learner = DatabaseManager::instance().getLearnerById(learnerId);
    
    if (learner.getId() == -1) {
        showErrorMessage("Learner not found");
        return;
    }
    
    ui->label_profileFullName->setText(learner.getFullName());
    ui->label_profileInitialSurname->setText(learner.getInitialSurname());
    ui->label_profileGrade->setText(learner.getGrade());
    ui->label_profileDOB->setText(learner.getDateOfBirth().toString("dd MMMM yyyy"));
    ui->label_profileContact->setText(learner.getContactNo());
    
    // Load currently borrowed books
    populateCurrentlyBorrowedBooks(learnerId);
    
    // Calculate and display summary
    QVector<Transaction> activeTransactions = DatabaseManager::instance().getActiveTransactionsByLearnerId(learnerId);
    double totalDue = DatabaseManager::instance().calculateUnreturnedBooksAmount(learnerId);
    
    // Display in summary section (you may need to add labels for this)
    // ui->label_totalBooksBorrowed->setText(QString::number(activeTransactions.size()));
    // ui->label_totalAmountDue->setText("R" + QString::number(totalDue, 'f', 2));
}

void MainWindow::loadBookDetails(int bookId) {
    Book book = DatabaseManager::instance().getBookById(bookId);
    
    if (book.getId() == -1) {
        showErrorMessage("Book not found");
        return;
    }
    
    ui->label_editBookCode->setText(book.getBookCode());
    ui->lineEdit_editBookISBN->setText(book.getIsbn());
    ui->lineEdit_editBookTitle->setText(book.getTitle());
    ui->lineEdit_editBookAuthor->setText(book.getAuthor());
    ui->comboBox_editBookSubject->setCurrentText(book.getSubject());
    ui->comboBox_editBookGrade->setCurrentText(book.getGrade());
    ui->doubleSpinBox_editBookPrice->setValue(book.getPrice());
    ui->comboBox_editBookStatus->setCurrentText(book.getStatusString());
    
    // Show copy count
    int copies = DatabaseManager::instance().getBookCountByISBN(book.getIsbn());
    ui->label_copiesInfoDisplay->setText(QString::number(copies) + " copies with this ISBN");
}

void MainWindow::loadTransactionHistory(int learnerId) {
    Learner learner = DatabaseManager::instance().getLearnerById(learnerId);
    
    ui->label_historyLearnerName->setText(learner.getFullName());
    ui->label_historyLearnerId->setText(QString::number(learnerId));
    
    QVector<Transaction> transactions = DatabaseManager::instance().getTransactionsByLearnerId(learnerId);
    populateTransactionsTable(transactions);
}

void MainWindow::loadActiveTransactionsForReturn(int learnerId) {
    QVector<Transaction> transactions = DatabaseManager::instance().getActiveTransactionsByLearnerId(learnerId);
    populateReturnBooksTable(transactions);
}

void MainWindow::loadTransactionByBookCode(const QString& bookCode) {
    Book book = DatabaseManager::instance().getBookByCode(bookCode);
    
    if (book.getId() == -1) {
        showErrorMessage("Book not found");
        return;
    }
    
    QVector<Transaction> transactions = DatabaseManager::instance().getTransactionsByBookId(book.getId());
    
    // Filter for active transactions
    QVector<Transaction> activeTransactions;
    for (const Transaction& trans : transactions) {
        if (trans.isActive()) {
            activeTransactions.append(trans);
        }
    }
    
    populateReturnBooksTable(activeTransactions);
}

// ==================== Table Population ====================

void MainWindow::populateBooksTable(const QVector<Book>& books) {
    ui->tableWidget_books->setRowCount(0);
    
    for (const Book& book : books) {
        int row = ui->tableWidget_books->rowCount();
        ui->tableWidget_books->insertRow(row);
        
        ui->tableWidget_books->setItem(row, 0, new QTableWidgetItem(QString::number(book.getId())));
        ui->tableWidget_books->setItem(row, 1, new QTableWidgetItem(book.getBookCode()));
        ui->tableWidget_books->setItem(row, 2, new QTableWidgetItem(book.getTitle()));
        ui->tableWidget_books->setItem(row, 3, new QTableWidgetItem(book.getAuthor()));
        ui->tableWidget_books->setItem(row, 4, new QTableWidgetItem(book.getSubject()));
        ui->tableWidget_books->setItem(row, 5, new QTableWidgetItem(book.getGrade()));
        ui->tableWidget_books->setItem(row, 6, new QTableWidgetItem("R" + QString::number(book.getPrice(), 'f', 2)));
        ui->tableWidget_books->setItem(row, 7, new QTableWidgetItem(book.getStatusString()));
    }
}

void MainWindow::populateLearnersTable(const QVector<Learner>& learners) {
    ui->tableWidget_viewLearnersList->setRowCount(0);
    
    for (const Learner& learner : learners) {
        int row = ui->tableWidget_viewLearnersList->rowCount();
        ui->tableWidget_viewLearnersList->insertRow(row);
        
        ui->tableWidget_viewLearnersList->setItem(row, 0, new QTableWidgetItem(QString::number(learner.getId())));
        ui->tableWidget_viewLearnersList->setItem(row, 1, new QTableWidgetItem(learner.getName()));
        ui->tableWidget_viewLearnersList->setItem(row, 2, new QTableWidgetItem(learner.getSurname()));
        ui->tableWidget_viewLearnersList->setItem(row, 3, new QTableWidgetItem(learner.getGrade()));
        ui->tableWidget_viewLearnersList->setItem(row, 4, new QTableWidgetItem(learner.getDateOfBirth().toString("dd/MM/yyyy")));
        ui->tableWidget_viewLearnersList->setItem(row, 5, new QTableWidgetItem(learner.getContactNo()));
    }
}

void MainWindow::populateTransactionsTable(const QVector<Transaction>& transactions) {
    ui->tableWidget_transactionHistory->setRowCount(0);
    
    for (const Transaction& trans : transactions) {
        Book book = DatabaseManager::instance().getBookById(trans.getBookId());
        
        int row = ui->tableWidget_transactionHistory->rowCount();
        ui->tableWidget_transactionHistory->insertRow(row);
        
        ui->tableWidget_transactionHistory->setItem(row, 0, new QTableWidgetItem(QString::number(trans.getId())));
        ui->tableWidget_transactionHistory->setItem(row, 1, new QTableWidgetItem(book.getTitle()));
        ui->tableWidget_transactionHistory->setItem(row, 2, new QTableWidgetItem(trans.getBorrowDate().toString("dd/MM/yyyy")));
        ui->tableWidget_transactionHistory->setItem(row, 3, new QTableWidgetItem(trans.getDueDate().toString("dd/MM/yyyy")));
        
        QString returnDateStr = trans.getReturnDate().isValid() ? 
                               trans.getReturnDate().toString("dd/MM/yyyy") : "Not Returned";
        ui->tableWidget_transactionHistory->setItem(row, 4, new QTableWidgetItem(returnDateStr));
        ui->tableWidget_transactionHistory->setItem(row, 5, new QTableWidgetItem(trans.getStatusString()));
        
        QString overdueStr = trans.isOverdue() ? QString::number(trans.getDaysOverdue()) : "-";
        ui->tableWidget_transactionHistory->setItem(row, 6, new QTableWidgetItem(overdueStr));
    }
}

void MainWindow::populateReturnBooksTable(const QVector<Transaction>& transactions) {
    ui->tableWidget_returnBooks->setRowCount(0);
    
    for (const Transaction& trans : transactions) {
        Book book = DatabaseManager::instance().getBookById(trans.getBookId());
        
        int row = ui->tableWidget_returnBooks->rowCount();
        ui->tableWidget_returnBooks->insertRow(row);
        
        ui->tableWidget_returnBooks->setItem(row, 0, new QTableWidgetItem(QString::number(trans.getId())));
        ui->tableWidget_returnBooks->setItem(row, 1, new QTableWidgetItem(book.getBookCode()));
        ui->tableWidget_returnBooks->setItem(row, 2, new QTableWidgetItem(book.getTitle()));
        ui->tableWidget_returnBooks->setItem(row, 3, new QTableWidgetItem(trans.getBorrowDate().toString("dd/MM/yyyy")));
        ui->tableWidget_returnBooks->setItem(row, 4, new QTableWidgetItem(trans.getDueDate().toString("dd/MM/yyyy")));
        
        QString status = trans.isOverdue() ? "OVERDUE" : "Active";
        ui->tableWidget_returnBooks->setItem(row, 5, new QTableWidgetItem(status));
    }
}

void MainWindow::populateDashboardTransactions(const QVector<Transaction>& transactions) {
    ui->tableWidget_homeRecentTransactions->setRowCount(0);
    
    for (const Transaction& trans : transactions) {
        Learner learner = DatabaseManager::instance().getLearnerById(trans.getLearnerId());
        Book book = DatabaseManager::instance().getBookById(trans.getBookId());
        
        int row = ui->tableWidget_homeRecentTransactions->rowCount();
        ui->tableWidget_homeRecentTransactions->insertRow(row);
        
        ui->tableWidget_homeRecentTransactions->setItem(row, 0, new QTableWidgetItem(learner.getFullName()));
        ui->tableWidget_homeRecentTransactions->setItem(row, 1, new QTableWidgetItem(book.getTitle()));
        ui->tableWidget_homeRecentTransactions->setItem(row, 2, new QTableWidgetItem(trans.getBorrowDate().toString("dd/MM/yyyy")));
        
        QString type = trans.getReturnDate().isValid() ? "Return" : "Borrow";
        ui->tableWidget_homeRecentTransactions->setItem(row, 3, new QTableWidgetItem(type));
        ui->tableWidget_homeRecentTransactions->setItem(row, 4, new QTableWidgetItem(trans.getStatusString()));
    }
}

void MainWindow::populateCurrentlyBorrowedBooks(int learnerId) {
    ui->tableWidget_currentlyBorrowedBooks->setRowCount(0);
    
    QVector<Transaction> activeTransactions = DatabaseManager::instance().getActiveTransactionsByLearnerId(learnerId);
    
    for (const Transaction& trans : activeTransactions) {
        Book book = DatabaseManager::instance().getBookById(trans.getBookId());
        
        int row = ui->tableWidget_currentlyBorrowedBooks->rowCount();
        ui->tableWidget_currentlyBorrowedBooks->insertRow(row);
        
        ui->tableWidget_currentlyBorrowedBooks->setItem(row, 0, new QTableWidgetItem(book.getTitle()));
        ui->tableWidget_currentlyBorrowedBooks->setItem(row, 1, new QTableWidgetItem(trans.getBorrowDate().toString("dd/MM/yyyy")));
        ui->tableWidget_currentlyBorrowedBooks->setItem(row, 2, new QTableWidgetItem(trans.getDueDate().toString("dd/MM/yyyy")));
        
        int daysLeft = QDate::currentDate().daysTo(trans.getDueDate());
        QString daysLeftStr = daysLeft >= 0 ? QString::number(daysLeft) : "OVERDUE";
        ui->tableWidget_currentlyBorrowedBooks->setItem(row, 3, new QTableWidgetItem(daysLeftStr));
        
        QString status = trans.isOverdue() ? "Overdue" : "Active";
        ui->tableWidget_currentlyBorrowedBooks->setItem(row, 4, new QTableWidgetItem(status));
    }
}

// ==================== Form Management ====================

void MainWindow::clearBookForm() {
    ui->lineEdit_bookCode->clear();
    ui->lineEdit_bookISBN->clear();
    ui->lineEdit_bookTitle->clear();
    ui->lineEdit_bookAuthor->clear();
    ui->comboBox_bookSubject->setCurrentIndex(0);
    ui->comboBox_bookGrade->setCurrentIndex(0);
    ui->doubleSpinBox_bookPrice->setValue(0.0);
    m_selectedBookId = -1;
}

void MainWindow::clearLearnerForm() {
    ui->lineEdit_addLearnerName->clear();
    ui->lineEdit_addLearnerSurname->clear();
    ui->lineEdit_addLearnerContact->clear();
    ui->comboBox_addLearnerGrade->setCurrentIndex(0);
    ui->dateEdit_addLearnerDOB->setDate(QDate::currentDate());
    m_selectedLearnerId = -1;
}

void MainWindow::clearBorrowForm() {
    ui->lineEdit_borrowLearnerID->clear();
    ui->lineEdit_borrowBookCode->clear();
    ui->label_borrowLearnerName->clear();
    ui->label_borrowLearnerGrade->clear();
    ui->label_borrowBookDisplay->clear();
    ui->label_borrowBookAuthorDisplay->clear();
    ui->label_borrowBookISBNDisplay->clear();
    ui->label_borrowBookStatusDisplay->clear();
    ui->label_borrowBookCount->clear();
    ui->dateEdit_borrowDate->setDate(QDate::currentDate());
    ui->dateEdit_dueDate->setDate(Transaction::calculateDueDate(QDate::currentDate()));
    m_selectedLearnerId = -1;
    m_selectedBookId = -1;
}

void MainWindow::clearReturnForm() {
    ui->lineEdit_returnSearch->clear();
    ui->label_returnSelectedBook->clear();
    ui->tableWidget_returnBooks->setRowCount(0);
    ui->dateEdit_returnDate->setDate(QDate::currentDate());
    m_selectedTransactionId = -1;
}

void MainWindow::clearRegistrationForm() {
    ui->lineEdit_regName->clear();
    ui->lineEdit_regSurname->clear();
    ui->lineEdit_regEmail->clear();
    ui->lineEdit_regContactNo->clear();
    ui->lineEdit_regSchoolName->clear();
    ui->lineEdit_regPassword->clear();
    ui->lineEdit_regRepeatPassword->clear();
    ui->lineEdit_regSecurityAnswer->clear();
    ui->QComboBox_regRole->setCurrentIndex(0);
    ui->QComboBox_securityQuestion->setCurrentIndex(0);
}

void MainWindow::fillBookForm(const Book& book) {
    // Used when editing
    ui->label_editBookCode->setText(book.getBookCode());
    ui->lineEdit_editBookISBN->setText(book.getIsbn());
    ui->lineEdit_editBookTitle->setText(book.getTitle());
    ui->lineEdit_editBookAuthor->setText(book.getAuthor());
    ui->comboBox_editBookSubject->setCurrentText(book.getSubject());
    ui->comboBox_editBookGrade->setCurrentText(book.getGrade());
    ui->doubleSpinBox_editBookPrice->setValue(book.getPrice());
    ui->comboBox_editBookStatus->setCurrentText(book.getStatusString());
}

void MainWindow::fillLearnerForm(const Learner& learner) {
    ui->lineEdit_editLearnerFirstName->setText(learner.getName());
    ui->lineEdit_editLearnerSurname->setText(learner.getSurname());
    ui->comboBox__editLearnerGrade->setCurrentText(learner.getGrade());
    ui->dateEdit__editLearnerDOB->setDate(learner.getDateOfBirth());
    ui->lineEdit__editLearnerContact->setText(learner.getContactNo());
}

void MainWindow::copyBookDataFromISBN() {
    QString isbn = ui->lineEdit_bookISBN->text().trimmed();
    
    if (isbn.isEmpty()) {
        showErrorMessage("Please enter an ISBN first");
        return;
    }
    
    // Find a book with this ISBN
    QVector<Book> allBooks = DatabaseManager::instance().getAllBooks();
    for (const Book& book : allBooks) {
        if (book.getIsbn() == isbn) {
            // Copy all fields except book code
            ui->lineEdit_bookTitle->setText(book.getTitle());
            ui->lineEdit_bookAuthor->setText(book.getAuthor());
            ui->comboBox_bookSubject->setCurrentText(book.getSubject());
            ui->comboBox_bookGrade->setCurrentText(book.getGrade());
            ui->doubleSpinBox_bookPrice->setValue(book.getPrice());
            
            showSuccessMessage("Book data copied from ISBN. Please enter a unique book code.");
            return;
        }
    }
    
    showInfoMessage("No existing book found with this ISBN. Please fill in all fields.");
}

// ==================== Search & Filter ====================

void MainWindow::searchBooks(const QString& searchTerm) {
    if (searchTerm.isEmpty()) {
        loadAllBooks();
        return;
    }
    
    QVector<Book> books = DatabaseManager::instance().searchBooks(searchTerm);
    populateBooksTable(books);
}

void MainWindow::filterBooksByGrade(const QString& grade) {
    QVector<Book> books = DatabaseManager::instance().getBooksByGrade(grade);
    populateBooksTable(books);
}

void MainWindow::searchLearners(const QString& searchTerm) {
    if (searchTerm.isEmpty()) {
        loadAllLearners();
        return;
    }
    
    QVector<Learner> learners = DatabaseManager::instance().searchLearners(searchTerm);
    populateLearnersTable(learners);
}

void MainWindow::filterLearnersByGrade(const QString& grade) {
    QVector<Learner> learners = DatabaseManager::instance().getLearnersByGrade(grade);
    populateLearnersTable(learners);
}

// ==================== Report Generation ====================

void MainWindow::generateBorrowReport(int learnerId) {
    QString html = generateReportHTML(learnerId, "Borrow");
    ui->textEdit_reportPreview->setHtml(html);
}

void MainWindow::generateReturnReport(int learnerId) {
    QString html = generateReportHTML(learnerId, "Return");
    ui->textEdit_reportPreview->setHtml(html);
}

QString MainWindow::generateReportHTML(int learnerId, const QString& reportType) {
    Learner learner = DatabaseManager::instance().getLearnerById(learnerId);
    QVector<Transaction> transactions = DatabaseManager::instance().getTransactionsByLearnerId(learnerId);
    
    QString html = "<html><head><style>";
    html += "body { font-family: Arial, sans-serif; }";
    html += "h1 { color: #2c3e50; }";
    html += "table { border-collapse: collapse; width: 100%; margin-top: 20px; margin-bottom: 20px; }";
    html += "th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }";
    html += "th { background-color: #3498db; color: white; }";
    html += "</style></head><body>";
    
    html += "<h1>Library " + reportType + " Report</h1>";
    html += "<p><strong>Learner:</strong> " + learner.getFullName() + "</p>";
    html += "<p><strong>Grade:</strong> " + learner.getGrade() + "</p>";
    html += "<p><strong>Date:</strong> " + QDate::currentDate().toString("dd MMMM yyyy") + "</p>";
    html+="<P><strong>Time:</strong> " + QTime::currentTime().toString("hh:mm") + "</p>";
    html += "<hr>";
    
    if (reportType == "Borrow") {
        // Show currently borrowed books
        html += "<h2>Currently Borrowed Books</h2>";
        QVector<Transaction> activeTransactions = DatabaseManager::instance().getActiveTransactionsByLearnerId(learnerId);
        
        if (activeTransactions.isEmpty()) {
            html += "<p>No books currently borrowed.</p>";
        } else {
            html += "<table><tr><th>Book Code</th><th>Book Title</th><th>Subject</th><th>Author</th><th>Borrow Date</th><th>Due Date</th><th>Status</th></tr>";
            
            for (const Transaction& trans : activeTransactions) {
                Book book = DatabaseManager::instance().getBookById(trans.getBookId());
                QString status = trans.isOverdue() ? "OVERDUE" : "Active";
                
                html += "<tr>";
                html += "<td>" + book.getBookCode() + "</td>";
                html += "<td>" + book.getTitle() + "</td>";
                html += "<td>" + book.getSubject() + "</td>";
                html += "<td>" + book.getAuthor() + "</td>";
                html += "<td>" + trans.getBorrowDate().toString("dd/MM/yyyy") + "</td>";
                html += "<td>" + trans.getDueDate().toString("dd/MM/yyyy") + "</td>";
                html += "<td>" + status + "</td>";
                html += "</tr>";
            }
            html += "</table>";
        }
    } else {
        // Show return history
        html += "<h2>Return History</h2>";
        QVector<Transaction> returnedTransactions;
        for (const Transaction& trans : transactions) {
            if (trans.isReturned() || trans.isLost()) {
                returnedTransactions.append(trans);
            }
        }
        
        if (returnedTransactions.isEmpty()) {
            html += "<p>No return history available.</p>";
        } else {
            html += "<table><tr><th>Book Code</th><th>Book Title</th><th>Subject</th><th>Borrow Date</th><th>Return Date</th><th>Status</th></tr>";
            
            for (const Transaction& trans : returnedTransactions) {
                Book book = DatabaseManager::instance().getBookById(trans.getBookId());
                
                html += "<tr>";
                html += "<td>" + book.getBookCode() + "</td>";
                html += "<td>" + book.getTitle() + "</td>";
                html += "<td>" + book.getSubject() + "</td>";
                html += "<td>" + trans.getBorrowDate().toString("dd/MM/yyyy") + "</td>";
                html += "<td>" + trans.getReturnDate().toString("dd/MM/yyyy") + "</td>";
                html += "<td>" + trans.getStatusString() + "</td>";
                html += "</tr>";
            }
            html += "</table>";
        }
    }
    
    // Add amount due
    double totalDue = DatabaseManager::instance().calculateUnreturnedBooksAmount(learnerId);
    if (totalDue > 0) {
        html += "<hr>";
        html += "<p><strong>Total Amount Due:</strong> R" + QString::number(totalDue, 'f', 2) + "</p>";
    }

    html += "<p> Assisted By:</p>"  ;

    html += "</body></html>";
    return html;
}

void MainWindow::printReport() {
    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    
    if (dialog.exec() == QDialog::Accepted) {
        ui->textEdit_reportPreview->document()->print(&printer);
        showSuccessMessage("Report printed successfully!");
    }
}

void MainWindow::saveReportAsPDF() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save Report as PDF", "", "PDF Files (*.pdf)");
    
    if (!fileName.isEmpty()) {
        if (!fileName.endsWith(".pdf", Qt::CaseInsensitive)) {
            fileName += ".pdf";
        }
        
        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(fileName);
        
        ui->textEdit_reportPreview->document()->print(&printer);
        showSuccessMessage("Report saved as PDF: " + fileName);
    }
}
