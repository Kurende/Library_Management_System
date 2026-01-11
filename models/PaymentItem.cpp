#include "PaymentItem.h"

// Constructor
PaymentItem::PaymentItem()
    : m_id(-1),
    m_paymentId(-1),
    m_transactionId(-1),
    m_bookId(-1),
    m_amount(0.0)
{
}

// Getters
int PaymentItem::getId() const
{
    return m_id;
}

int PaymentItem::getPaymentId() const
{
    return m_paymentId;
}

int PaymentItem::getTransactionId() const
{
    return m_transactionId;
}

int PaymentItem::getBookId() const
{
    return m_bookId;
}

double PaymentItem::getAmount() const
{
    return m_amount;
}

// Setters
void PaymentItem::setId(int id)
{
    m_id = id;
}

void PaymentItem::setPaymentId(int id)
{
    m_paymentId = id;
}

void PaymentItem::setTransactionId(int id)
{
    m_transactionId = id;
}

void PaymentItem::setBookId(int id)
{
    m_bookId = id;
}

void PaymentItem::setAmount(double amount)
{
    m_amount = amount;
}

