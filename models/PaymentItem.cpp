#include "PaymentItem.h"

PaymentItem::PaymentItem()
    : m_id(-1)
    , m_paymentId(-1)
    , m_transactionId(-1)
    , m_bookId(-1)
    , m_amount(0.0)
{
}

PaymentItem::PaymentItem(int id, int paymentId, int transactionId, int bookId, double amount)
    : m_id(id)
    , m_paymentId(paymentId)
    , m_transactionId(transactionId)
    , m_bookId(bookId)
    , m_amount(amount)
{
}
