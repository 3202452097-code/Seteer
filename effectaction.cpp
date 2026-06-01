#include "effectaction.h"
#include "BattleContext.h"
#include <QRandomGenerator>
// ── Damage ──
void DamageAction::execute(BattleContext& ctx) {
    int dmg = m_amount.evaluate(ctx);
    if (ctx.attacker) dmg += ctx.attacker->strength();
    ctx.enemy->takeDamage(dmg);
}
QString DamageAction::description() const {
    return QString("造成 %1 点伤害").arg(m_amount.base);
}
// ── Block ──
void BlockAction::execute(BattleContext& ctx) {
    int val = m_amount.evaluate(ctx);
    ctx.player->setBlock(ctx.player->block() + val);
}
QString BlockAction::description() const {
    return QString("获得 %1 点格挡").arg(m_amount.base);
}
// ── Draw ──
void DrawAction::execute(BattleContext& ctx) {
    for (int i = 0; i < m_count; i++) {
        if (ctx.drawPile->isEmpty())
            ctx.shuffleDiscardIntoDraw();
        if (ctx.drawPile->isEmpty()) break;
        ctx.hand->append(ctx.drawPile->takeLast());
    }
}
QString DrawAction::description() const {
    return QString("抽 %1 张牌").arg(m_count);
}
// ── WriteString ──
void WriteStringAction::execute(BattleContext& ctx) {
    if (m_append)
        ctx.stringSpace->append(m_text);
    else
        ctx.stringSpace->setText(m_text);
}
QString WriteStringAction::description() const {
    return m_append ? QString("写入 \"%1\"").arg(m_text)
                    : QString("覆盖为 \"%1\"").arg(m_text);
}
// ── ClearString ──
void ClearStringAction::execute(BattleContext& ctx) {
    ctx.stringSpace->clear();
}
QString ClearStringAction::description() const {
    return "清空字符串空间";
}

// ── WriteRandomLetter ──
void WriteRandomLetterAction::execute(BattleContext& ctx) {
    char letter = 'A' + QRandomGenerator::global()->bounded(26);
    ctx.stringSpace->append(QString(QChar(letter)));
}
QString WriteRandomLetterAction::description() const {
    return "写入一个随机字母";
}
// ── WriteRandomDigit ──
void WriteRandomDigitAction::execute(BattleContext& ctx) {
    char digit = '0' + QRandomGenerator::global()->bounded(10);
    ctx.stringSpace->append(QString(QChar(digit)));
}
QString WriteRandomDigitAction::description() const {
    return "写入一个随机数字";
}
// ── ConsumeLastDigitDamage ──
void ConsumeLastDigitDamageAction::execute(BattleContext& ctx) {
    if (ctx.stringSpace->isEmpty()) return;
    QChar c = ctx.stringSpace->lastChar();
    if (c.isDigit()) {
        int dmg = c.digitValue();
        if (ctx.attacker) dmg += ctx.attacker->strength();
        if (dmg > 0) {
            ctx.enemy->takeDamage(dmg);
        }
    }
    ctx.stringSpace->removeLast(1);   // 读取后移除
}
QString ConsumeLastDigitDamageAction::description() const {
    return "消耗最后一位数字并造成等量伤害";
}
// ── ConsumeLastAlphaBlock ──
void ConsumeLastAlphaBlockAction::execute(BattleContext& ctx) {
    if (ctx.stringSpace->isEmpty()) return;
    QChar c = ctx.stringSpace->lastChar();
    if (c.isLetter()) {
        ctx.player->setBlock(ctx.player->block() + m_blockAmount);
        ctx.stringSpace->removeLast(1);   // 读取后移除
    }
}
QString ConsumeLastAlphaBlockAction::description() const {
    return QString("若最后一位是字母，获得 %1 点格挡并移除").arg(m_blockAmount);
}
// ── ClearStringDamage ──
void ClearStringDamageAction::execute(BattleContext& ctx) {
    int n = ctx.stringSpace->length();   // 段数
    ctx.stringSpace->clear();            // 清空
    if (n == 0) return;
    int perHit = (n >= 5) ? 8 : 4;       // 每段伤害
    for (int i = 0; i < n; i++) {
        int dmg = perHit;
        if (ctx.attacker) dmg += ctx.attacker->strength();
        if (dmg > 0) {
            ctx.enemy->takeDamage(dmg);
        }
    }
}
QString ClearStringDamageAction::description() const {
    return "清空字符串空间，造成 长度×4 伤害（≥5 时翻倍为 ×8）";
}
// ==================== ★ GainStrength ====================
void GainStrengthAction::execute(BattleContext& ctx) {
    if (ctx.attacker) {
        ctx.attacker->addStrength(m_amount);
    }
}
QString GainStrengthAction::description() const {
    return QString("获得 %1 点力量").arg(m_amount);
}
// ==================== ★ Heal ====================
void HealAction::execute(BattleContext& ctx) {
    ctx.player->setHP(ctx.player->hp() + m_amount);
}
QString HealAction::description() const {
    return QString("回复 %1 点生命").arg(m_amount);
}
// ==================== ★ ConsumeLastTwoDigitsStrength ====================
void ConsumeLastTwoDigitsStrengthAction::execute(BattleContext& ctx) {
    QString s = ctx.stringSpace->content();
    if (s.length() < 2) return;
    // 取最后两个字符
    QChar c1 = s.at(s.length() - 2);
    QChar c2 = s.at(s.length() - 1);
    if (!c1.isDigit() || !c2.isDigit()) return;
    int d1 = c1.digitValue();
    int d2 = c2.digitValue();
    int gain = qAbs(d1 - d2);
    ctx.stringSpace->removeLast(2);   // 消耗
    if (gain > 0 && ctx.attacker) {
        ctx.attacker->addStrength(gain);
    }
}
QString ConsumeLastTwoDigitsStrengthAction::description() const {
    return "若最后两位均为数字，消耗之并获得差值力量";
}
