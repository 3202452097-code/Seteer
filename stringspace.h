#ifndef STRINGSPACE_H
#define STRINGSPACE_H
#include <QString>
#include <functional>
class StringSpace {
public:
    QString content() const { return m_content; }
    int length() const { return m_content.length(); }
    bool contains(const QString& sub) const { return m_content.contains(sub); }
    bool isEmpty() const { return m_content.isEmpty(); }
    void setText(const QString& text);
    void append(const QString& text);
    void clear();
    void removeLast(int n = 1);
    // UI 更新回调（由 BattleScene 绑定）
    QChar lastChar() const;
    bool isLastDigit() const;
    bool isLastAlpha() const;

    std::function<void()> onChanged;
private:
    QString m_content;
};
#endif // STRINGSPACE_H