#include "effectaction.h"
#include "BattleContext.h"
#include <QRandomGenerator>
// ── Damage ──
void DamageAction::execute(BattleContext& ctx) {
    int dmg = m_amount.evaluate(ctx);
    if (ctx.attacker){
        dmg += ctx.attacker->strength();
        if (ctx.attacker->hasStatus(StatusType::Weak)) {
            dmg = static_cast<int>(dmg * 0.75);
        }
    }
    ctx.defender->takeDamage(dmg);
}
QString DamageAction::description() const {
    return QString("造成 %1 点伤害").arg(m_amount.base);
}
// ── Block ──
void BlockAction::execute(BattleContext& ctx) {
    int val = m_amount.evaluate(ctx);
    ctx.attacker->setBlock(ctx.attacker->block() + val);
}
QString BlockAction::description() const {
    return QString("获得 %1 点格挡").arg(m_amount.base);
}
// ── Draw ──
void DrawAction::execute(BattleContext& ctx) {
    if (ctx.drawCards) {
        ctx.drawCards(m_count);   // ★ 一行搞定
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
            ctx.defender->takeDamage(dmg);
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
        if (ctx.attacker->hasStatus(StatusType::Weak)) {
            dmg = static_cast<int>(dmg * 0.75);
        }
        ctx.enemy->takeDamage(dmg);
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
    ctx.attacker->setHP(ctx.attacker->hp() + m_amount);
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
// ==================== ★ ApplyStatus ====================
void ApplyStatusAction::execute(BattleContext& ctx) {
    if (ctx.defender) {
        ctx.defender->addStatus(m_type, m_amount, m_duration);
    }
}
QString ApplyStatusAction::description() const {
    QString targetStr = (m_target == Enemy) ? "敌人" : "自身";
    QString typeStr;
    switch (m_type) {
    case StatusType::Vulnerable: typeStr = "易伤"; break;
    case StatusType::Weak:       typeStr = "虚弱"; break;
    default:                     typeStr = "?";     break;
    }
    return QString("给%1施加 %2 回合%3").arg(targetStr).arg(m_duration).arg(typeStr);
}
// ==================== ★ AddAbility ====================
void AddAbilityAction::execute(BattleContext& ctx) {
    Entity* target = (m_target == Self) ? ctx.attacker : ctx.defender;
    if (target) {
        target->addAbility(m_abilityId, m_duration);
    }
}
QString AddAbilityAction::description() const {
    const AbilityData* data = AbilityDatabase::instance().abilityById(m_abilityId);
    QString name = data ? data->name : m_abilityId;
    QString dur = (m_duration == 0) ? "永久" : QString::number(m_duration) + "回合";
    return QString("获得能力 [%1] %2").arg(dur).arg(name);
}
void GainEnergyAction::execute(BattleContext& ctx) {
    if (ctx.attacker == ctx.player) {
        ctx.player->setEnergy(ctx.player->energy() + m_amount);
    } // 暂时只处理玩家，敌人无能量
}
QString GainEnergyAction::description() const {
    return QString("回复 %1 点能量").arg(m_amount);
}
void SetMaxEnergyAction::execute(BattleContext& ctx) {
    if (ctx.attacker == ctx.player) {
        ctx.player->setMaxEnergy(m_newMax);
        ctx.player->setEnergy(m_newMax);   // 同时回满
    }
}
QString SetMaxEnergyAction::description() const {
    return QString("将最大能量提升至 %1").arg(m_newMax);
}
// ==================== DrawAndConditionalDamageAction ====================
DrawAndConditionalDamageAction::DrawAndConditionalDamageAction(int drawCount, int baseDamageFactor, int maxLen)
    : m_drawCount(drawCount), m_baseDamageFactor(baseDamageFactor), m_maxLen(maxLen) {}

void DrawAndConditionalDamageAction::execute(BattleContext& ctx) {
    // 抽牌
    if (ctx.drawCards) {
        ctx.drawCards(m_drawCount);
    }
    // 条件伤害
    int len = ctx.stringSpace->length();
    if (len <= m_maxLen) {
        int damage = m_baseDamageFactor * (m_maxLen - len);
        if (damage > 0) {
            // 使用现有 DamageAction 逻辑（复用）
            DamageAction dmgAct(EffectValue::fixed(damage));
            dmgAct.execute(ctx);
        }
    }
}

QString DrawAndConditionalDamageAction::description() const {
    return QString("抽 %1 张牌。若字符串长度 ≤ %2，造成 %3 × (%2 - 长度) 点伤害")
        .arg(m_drawCount).arg(m_maxLen).arg(m_baseDamageFactor);
}
// ==================== ChaosRemoveAction ====================
ChaosRemoveAction::ChaosRemoveAction(int minDamage, int maxDamage)
    : m_minDamage(minDamage), m_maxDamage(maxDamage) {}

void ChaosRemoveAction::execute(BattleContext& ctx) {
    // 1. 分析当前字符串内容
    QString content = ctx.stringSpace->content();
    int totalChars = content.length();
    if (totalChars == 0) return;  // 无字符则无效果

    int letterCount = 0, digitCount = 0;
    for (QChar ch : content) {
        if (ch.isLetter()) letterCount++;
        else if (ch.isDigit()) digitCount++;
    }

    // 2. 移除所有字符
    ctx.stringSpace->clear();

    // 3. 随机多段伤害，计算总伤害
    int totalDamage = 0;
    for (int i = 0; i < totalChars; ++i) {
        int dmg = QRandomGenerator::global()->bounded(m_minDamage, m_maxDamage + 1);
        totalDamage += dmg;
        if (dmg > 0) {
            // 对敌人造成每次伤害
            DamageAction dmgAct(EffectValue::fixed(dmg));
            // 注意：DamageAction 依赖 ctx.defender，需要确保 enemy 是 defender
            dmgAct.execute(ctx);
        }
    }

    // 4. 副作用：根据字母/数字数量差
    int diff = letterCount - digitCount;
    if (diff > 0) {
        // 移除自身所有格挡
        if (ctx.attacker) {
            ctx.attacker->setBlock(0);
        }
    } else if (diff < 0) {
        // 对自己造成总伤害的一半（向上取整）
        int selfDamage = (totalDamage + 1) / 2;  // 向上取整
        if (selfDamage > 0 && ctx.attacker) {
            // 临时交换 attacker/defender 来对自己造成伤害
            Entity* originalAttacker = ctx.attacker;
            Entity* originalDefender = ctx.defender;
            ctx.attacker = ctx.player;   // 自己攻击自己
            ctx.defender = ctx.player;
            DamageAction selfDmg(EffectValue::fixed(selfDamage));
            selfDmg.execute(ctx);
            // 恢复上下文
            ctx.attacker = originalAttacker;
            ctx.defender = originalDefender;
        }
    }
}

QString ChaosRemoveAction::description() const {
    return QString("移除所有字符，造成每字符 %1~%2 点随机伤害。若字母多于数字，移除自身所有格挡；若数字多于字母，自身受到总伤害一半。")
        .arg(m_minDamage).arg(m_maxDamage);
}