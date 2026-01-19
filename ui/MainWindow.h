#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QStackedWidget>
#include "Learner.h"
#include "Book.h"
#include "Transaction.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // ==================== Login & Registration ====================
    void on_pushButton_login_clicked();
    void on_pushButton_register_clicked();
    void on_pushButton_quit_clicked();
    void on_pushButton_forgotPassword_clicked();
    void on_pushButton_regBackToLogin_clicked();
    void on_pushButton_registerSubmit_clicked();
    
    // Password Reset
    void on_pushButton_resetVerifyEmail_clicked();
    void on_pushButton_resetVerifyAnswer_clicked();
    void on_pushButton_resetSubmit_clicked();
    void on_pushButton_resetCancel_clicked();
    
    // ==================== Navigation ====================
    void on_pushButton_homeIconSection_clicked();
    void on_pushButton_settingsIconSection_clicked();
    void on_pushButton_usersIconSection_clicked();
    void on_pushButton_booksSidebar_clicked();
    void on_pushButton_learnersSidebar_clicked();
    void on_pushButton_transactSidebar_clicked();
    void on_pushButton_reportsSidebar_clicked();
    void on_pushButton_settingsSidebar_clicked();
    void on_pushButton_logoutSidebar_clicked();
    void on_pushButton_logoutTopBar_clicked();
    void on_pushButton_toogleMenu_clicked();
    void on_pushButton_makePaymentSidebar_clicked();
    
    // Sidebar sub-menus
    void on_pushButton_booksIconSection_clicked();
    void on_pushButton_learnersIconSection_clicked();
    void on_pushButton_transactionIconSection_clicked();
    void on_pushButton_reportsIconSection_clicked();
    void on_pushButton_usersSidebar_clicked();

    
    // ==================== Dashboard ====================
    void on_pushButton_addBookQuickButton_clicked();
    void on_pushButton_issueBookQuickButton_clicked();
    void on_pushButton_returnBookQuickButton_clicked();
    
    // ==================== Book Management ====================
    void on_pushButton_addBookSidebar_clicked();
    void on_pushButton_updateBookInfoSidebar_clicked();
    void on_pushButton_allBooksSidebar_clicked();
    void on_pushButton_confirmAdd_clicked();
    void on_pushButton_clearForm_clicked();
    void on_pushButton_copyFromISBN_clicked();
    void on_pushButton_refreshBooks_clicked();
    void on_pushButton_bookDetails_clicked();
    void on_pushButton_markAsLost_clicked();
    void on_lineEdit_searchBooks_textChanged(const QString &text);
    void on_comboBox_sortBooks_currentIndexChanged(int index);
    void on_tableWidget_books_cellClicked(int row, int column);
    
    // Update Book
    void on_pushButton_saveChanges_clicked();
    void on_pushButton_cancelEdit_clicked();
    void on_pushButton_deleteBook_clicked();

    //================== User Management =========================
    void on_pushButton_editUserProfile_clicked();
    void on_pushButton_confirmEditProfile_clicked();
    void on_pushButton_cancelEditProfile_clicked();
    void on_pushButton_changeUserPassword_clicked();
    void on_pushButton_confirmChangePassword_clicked();
    void on_pushButton_cancelChangePassword_clicked();
    
    // ==================== Learner Management ====================
    void on_pushButton_addLearnerSidebar_clicked();
    void on_pushButton_updateLearnerInfoSidebar_clicked();
    void on_pushButton_AddLearner_clicked();
    void on_pushButton_addLearnerClearForm_clicked();
    void on_pushButton_refreshLearnerList_clicked();
    void on_pushButton_viewLeanerProfile_clicked();
    void on_pushButton_viewLearnerHistory_clicked();
    void on_lineEdit_searchLearner_textChanged(const QString &text);
    void on_comboBox_filterLearnerGrade_currentIndexChanged(int index);
    void on_tableWidget_viewLearnersList_cellClicked(int row, int column);
    
    // Learner Profile
    void on_pushButton_backToLearners_clicked();
    void on_pushButton_editLearnerProfile_clicked();
    void on_pushButton_viewTransactionRecord_clicked();
    
    // Update Learner
    void on_pushButton_editLearnerConfirm_clicked();
    void on_pushButton_editLearnerCancel_clicked();
    
    // ==================== Transaction Management ====================
    void on_pushButton_borrowBookSidebar_clicked();
    void on_pushButton_returnBookSidebar_clicked();
    void on_pushButton_viewTransactionsSidebar_clicked();
    
    // Borrow Book
    void on_pushButton_findLearner_clicked();
    void on_pushButton_findBook_clicked();
    void on_pushButton_confirmBorrow_clicked();
    void on_pushButton_clearBorrowForm_clicked();
    void on_pushButton_goToReportsFromBorrow_clicked();
    
    // Return Book
    void on_pushButton_searchReturn_clicked();
    void on_pushButton_processReturn_clicked();
    void on_pushButton_clearReturnForm_clicked();
    void on_pushButton_markBookAsLost_clicked();
    void on_pushButton_goToReportsFromReturn_clicked();
    void on_radioButton_searchByLearner_clicked();
    void on_radioButton_searchByBook_clicked();
    void on_tableWidget_returnBooks_cellClicked(int row, int column);
    
    // Transaction History
    void on_pushButton_BackToLearnersList_clicked();
    void on_pushButton_backToProfile_clicked();
    void on_pushButton_filterHistory_clicked();
    void on_pushButton_goToReports_clicked();
    
    // ==================== Reports ====================
    void on_pushButton_printReportSidebar_clicked();
    void on_pushButton_loadLearnerData_clicked();
    void on_pushButton_generateReport_clicked();
    void on_pushButton_printReport_clicked();
    void on_pushButton_saveReportPDF_clicked();
    void on_pushButton_clearPreview_clicked();
    void on_pushButton_backToMenu_clicked();
    void on_radioButton_borrowReport_clicked();
    void on_radioButton_returnReport_clicked();

private:
    Ui::MainWindow *ui;
    
    // Current selections
    int m_selectedBookId;
    int m_selectedLearnerId;
    int m_selectedTransactionId;
    QString m_currentEmail; // For password reset
    
    // UI State
    bool m_menuExpanded;
    
    // ==================== Initialization ====================
    void initializeUI();
    void setupConnections();
    void loadDashboardData();
    void applyPermissions();
    
    // ==================== Page Navigation ====================
    void navigateToPage(QWidget* targetPage);
    void showLoginPage();
    void showRegisterPage();
    void showResetPasswordPage();
    void showHomePage();
    void showDashboardPage();
    void showBooksPage();
    void showAddBookPage();
    void showUpdateBookPage();
    void showLearnersPage();
    void showAddLearnerPage();
    void showLearnerProfilePage();
    void showUpdateLearnerPage();
    void showBorrowBookPage();
    void showReturnBookPage();
    void showTransactionHistoryPage();
    void showReportsPage();
    void showPaymentsPage();
    void showUsersPage();
    void showSettingsPage();

    
    // ==================== Data Loading ====================
    void loadAllBooks();
    void loadAllLearners();
    void loadRecentTransactions();
    void loadLearnerProfile(int learnerId);
    void loadBookDetails(int bookId);
    void loadTransactionHistory(int learnerId);
    void loadActiveTransactionsForReturn(int learnerId);
    void loadTransactionByBookCode(const QString& bookCode);
    
    // ==================== Table Population ====================
    void populateBooksTable(const QVector<Book>& books);
    void populateLearnersTable(const QVector<Learner>& learners);
    void populateTransactionsTable(const QVector<Transaction>& transactions);
    void populateReturnBooksTable(const QVector<Transaction>& transactions);
    void populateDashboardTransactions(const QVector<Transaction>& transactions);
    void populateCurrentlyBorrowedBooks(int learnerId);
    
    // ==================== Form Management ====================
    void clearBookForm();
    void clearLearnerForm();
    void clearBorrowForm();
    void clearReturnForm();
    void clearRegistrationForm();
    
    void fillBookForm(const Book& book);
    void fillLearnerForm(const Learner& learner);
    void copyBookDataFromISBN();
    
    // ==================== Validation ====================
    bool validateBookForm();
    bool validateLearnerForm();
    bool validateBorrowForm();
    bool validateRegistrationForm();
    bool validatePasswordResetForm();
    
    // ==================== Business Logic ====================
    void performBorrow();
    void performReturn();
    void performMarkAsLost();
    void calculateAndDisplayAmount(int learnerId);
    
    // ==================== UI Updates ====================
    void updateDashboardStats();
    void updateUserInfo();
    void toggleSidebar();
    void showSuccessMessage(const QString& message);
    void showErrorMessage(const QString& message);
    void showInfoMessage(const QString& message);

    //================== Users ====================================
    void loadUserProfile();
    void updateProfileIcon();
    void updateSecurityInfo();
    void initializeProfilePage();

    
    // ==================== Report Generation ====================
    void generateBorrowReport(int learnerId);
    void generateReturnReport(int learnerId);
    QString generateReportHTML(int learnerId, const QString& reportType);
    void printReport();
    void saveReportAsPDF();
    
    // ==================== Search & Filter ====================
    void searchBooks(const QString& searchTerm);
    void filterBooksByGrade(const QString& grade);
    void searchLearners(const QString& searchTerm);
    void filterLearnersByGrade(const QString& grade);
    
    // ==================== Helper Methods ====================
    QString getGradeText(int index);
    QString getSubjectText(int index);
    void setupComboBoxes();
    void setupTableHeaders();
};

#endif // MAINWINDOW_H
