#include "StringSpace.h"
void StringSpace::setText(const QString& text) {
    QString truncated = text.length() > 20 ? text.right(20) : text;
    if (m_content != truncated) {
        m_content = truncated;
        if (onChanged) onChanged();
    }
}
void StringSpace::append(const QString& text) {
    if (!text.isEmpty()) {
        m_content += text;
        if (m_content.length() > 20) {
            m_content = m_content.right(20);
        }
        if (onChanged) onChanged();
    }
}
void StringSpace::clear() {
    if (!m_content.isEmpty()) {
        m_content.clear();
        if (onChanged) onChanged();
    }
}
void StringSpace::removeLast(int n) {
    int remove = qMin(n, m_content.length());
    if (remove > 0) {
        m_content.chop(remove);
        if (onChanged) onChanged();
    }
}

QChar StringSpace::lastChar() const {
    if (m_content.isEmpty()) return QChar();
    return m_content.at(m_content.length() - 1);
}
bool StringSpace::isLastDigit() const {
    return lastChar().isDigit();
}
bool StringSpace::isLastAlpha() const {
    return lastChar().isLetter();
}