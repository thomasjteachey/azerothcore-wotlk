/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

 /*
  * Scripts for spells with SPELLFAMILY_PRIEST and SPELLFAMILY_GENERIC spells used by priest players.
  * Ordered alphabetically using scriptname.
  * Scriptnames of files in this file should be prefixed with "spell_pri_".
  */

#include "ScriptMgr.h"
#include "Creature.h"
#include "Errors.h"
#include "GridNotifiers.h"
#include "Player.h"
#include "Random.h"
#include "SharedDefines.h"
#include "SpellAuraEffects.h"
#include "SpellDefines.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "TemporarySummon.h"

enum PriestSpells
{
    SPELL_PRIEST_BLESSED_RECOVERY_R1 = 27813,
    SPELL_PRIEST_DIVINE_AEGIS = 47753,
    SPELL_PRIEST_EMPOWERED_RENEW = 63544,
    SPELL_PRIEST_GLYPH_OF_CIRCLE_OF_HEALING = 55675,
    SPELL_PRIEST_GLYPH_OF_LIGHTWELL = 55673,
    SPELL_PRIEST_GLYPH_OF_PRAYER_OF_HEALING_HEAL = 56161,
    SPELL_PRIEST_GUARDIAN_SPIRIT_HEAL = 48153,
    SPELL_PRIEST_ITEM_EFFICIENCY = 37595,
    SPELL_PRIEST_LIGHTWELL_CHARGES = 59907,
    SPELL_PRIEST_MANA_LEECH_PROC = 34650,
    SPELL_PRIEST_PENANCE_R1 = 47540,
    SPELL_PRIEST_PENANCE_R1_DAMAGE = 47758,
    SPELL_PRIEST_PENANCE_R1_HEAL = 47757,
    SPELL_PRIEST_REFLECTIVE_SHIELD_TRIGGERED = 33619,
    SPELL_PRIEST_REFLECTIVE_SHIELD_R1 = 33201,
    SPELL_PRIEST_SHADOW_WORD_DEATH = 32409,
    SPELL_PRIEST_T9_HEALING_2P = 67201,
    SPELL_PRIEST_VAMPIRIC_TOUCH_DISPEL = 64085,
    SPELL_PRIEST_GLYPH_OF_SHADOWFIEND_MANA = 58227,
    SPELL_REPLENISHMENT = 57669,
    SPELL_PRIEST_BODY_AND_SOUL_POISON_TRIGGER = 64136,
    SPELL_PRIEST_ABOLISH_DISEASE = 552,
    SPELL_PRIEST_VAMPIRIC_EMBRACE_HEAL = 15290,
    SPELL_PRIEST_DIVINE_BLESSING = 40440,
    SPELL_PRIEST_DIVINE_WRATH = 40441,
    SPELL_PRIEST_GLYPH_OF_DISPEL_MAGIC_HEAL = 56131,
    SPELL_PRIEST_ORACULAR_HEAL = 26170,
    SPELL_PRIEST_ARMOR_OF_FAITH = 28810,
    SPELL_PRIEST_BLESSED_HEALING = 70772,
    SPELL_PRIEST_MIND_BLAST_R1 = 8092,
    SPELL_PRIEST_SHADOW_WORD_DEATH_R1 = 32379,
    SPELL_PRIEST_MIND_FLAY_DAMAGE = 58381,
    SPELL_PRIEST_LIGHTWELL_RENEW_R1 = 7001,
    SPELL_PRIEST_LIGHTWELL_RENEW_R2 = 27873,
    SPELL_PRIEST_LIGHTWELL_RENEW_R3 = 27874,
    SPELL_PRIEST_LIGHTWELL_RENEW_R4 = 28276,
    SPELL_PRIEST_LIGHTWELL_RENEW_R5 = 48084,
    SPELL_PRIEST_LIGHTWELL_RENEW_R6 = 48085,
    SPELL_PRIEST_TWIN_DISCIPLINE_R1 = 47586,
    SPELL_PRIEST_SPIRITUAL_HEALING_R1 = 14898,
    SPELL_PRIEST_DIVINE_PROVIDENCE_R1 = 47562,
    SPELL_PRIEST_SHADOW_GUARD_DAMAGE_R1 = 28377
};

enum PriestSpellIcons
{
    PRIEST_ICON_ID_FOCUSED_POWER = 2210,
    PRIEST_ICON_ID_BORROWED_TIME = 2899,
    PRIEST_ICON_ID_EMPOWERED_RENEW_TALENT = 3021,
    PRIEST_ICON_ID_PAIN_AND_SUFFERING = 2874,
};

enum PriestMisc
{
    PRIEST_LIGHTWELL_NPC_1 = 31897,
    PRIEST_LIGHTWELL_NPC_2 = 31896,
    PRIEST_LIGHTWELL_NPC_3 = 31895,
    PRIEST_LIGHTWELL_NPC_4 = 31894,
    PRIEST_LIGHTWELL_NPC_5 = 31893,
    PRIEST_LIGHTWELL_NPC_6 = 31883
};

enum MiscSpells
{
    SPELL_MAGE_ARCANE_POWER = 12042,
    SPELL_GENERIC_ARENA_DAMPENING = 74410,
    SPELL_GENERIC_BATTLEGROUND_DAMPENING = 74411
};

enum MiscSpellIcons
{
    SPELL_ICON_ID_STRENGTH_OF_WRYNN = 1704,
    SPELL_ICON_ID_HELLSCREAM_WARSONG = 937
};

class PowerCheck
{
public:
    explicit PowerCheck(Powers const power) : _power(power) { }

    bool operator()(WorldObject* obj) const
    {
        if (Unit* target = obj->ToUnit())
            return target->getPowerType() != _power;

        return true;
    }

private:
    Powers const _power;
};

class RaidCheck
{
public:
    explicit RaidCheck(Unit const* caster) : _caster(caster) { }

    bool operator()(WorldObject* obj) const
    {
        if (Unit* target = obj->ToUnit())
            return !_caster->IsInRaidWith(target);

        return true;
    }

private:
    Unit const* _caster;
};

// 26169 - Oracle Healing Bonus
class spell_pri_aq_3p_bonus : public AuraScript
{
    PrepareAuraScript(spell_pri_aq_3p_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_ORACULAR_HEAL });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        Unit* caster = eventInfo.GetActor();
        if (caster == eventInfo.GetProcTarget())
            return;

        HealInfo* healInfo = eventInfo.GetHealInfo();
        if (!healInfo || !healInfo->GetHeal())
            return;

        caster->CastCustomSpell(SPELL_PRIEST_ORACULAR_HEAL, SPELLVALUE_BASE_POINT0, CalculatePct(healInfo->GetHeal(), 10));
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pri_aq_3p_bonus::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// -27811 - Blessed Recovery
class spell_pri_blessed_recovery : public AuraScript
{
    PrepareAuraScript(spell_pri_blessed_recovery);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_BLESSED_RECOVERY_R1 });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        DamageInfo* dmgInfo = eventInfo.GetDamageInfo();
        if (!dmgInfo || !dmgInfo->GetDamage())
            return;

        Unit* target = eventInfo.GetActionTarget();
        uint32 triggerSpell = sSpellMgr->GetSpellWithRank(SPELL_PRIEST_BLESSED_RECOVERY_R1, aurEff->GetSpellInfo()->GetRank());
        SpellInfo const* triggerInfo = sSpellMgr->AssertSpellInfo(triggerSpell);

        int32 bp = CalculatePct(static_cast<int32>(dmgInfo->GetDamage()), aurEff->GetAmount());

        ASSERT(triggerInfo->GetMaxTicks() > 0);
        bp /= triggerInfo->GetMaxTicks();

        target->CastCustomSpell(triggerSpell, SPELLVALUE_BASE_POINT0, bp);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pri_blessed_recovery::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// -17 - Power Word: Shield
class spell_pri_power_word_shield : public SpellScript
{
    PrepareSpellScript(spell_pri_power_word_shield);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ spellInfo->ExcludeTargetAuraSpell });
    }

    void WeakenSoul()
    {
        if (Unit* target = GetHitUnit())
            GetCaster()->CastSpell(target, GetSpellInfo()->ExcludeTargetAuraSpell, true);
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_pri_power_word_shield::WeakenSoul);
    }
};

// 15286 - Vampiric Embrace
class spell_pri_vampiric_embrace : public AuraScript
{
    PrepareAuraScript(spell_pri_vampiric_embrace);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_VAMPIRIC_EMBRACE_HEAL });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        DamageInfo* damageInfo = eventInfo.GetDamageInfo();
        if (!damageInfo || !damageInfo->GetDamage())
            return;

        int32 selfHeal = CalculatePct(static_cast<int32>(damageInfo->GetDamage()), aurEff->GetAmount());
        int32 partyHeal = selfHeal;
        GetCaster()->CastCustomSpell(GetCaster(), 15290, &partyHeal, &selfHeal, nullptr, true);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pri_vampiric_embrace::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};


// 28809 - Greater Heal
class spell_pri_t3_4p_bonus : public AuraScript
{
    PrepareAuraScript(spell_pri_t3_4p_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_ARMOR_OF_FAITH });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        eventInfo.GetActor()->CastSpell(eventInfo.GetProcTarget(), SPELL_PRIEST_ARMOR_OF_FAITH, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pri_t3_4p_bonus::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 10060 - Power Infusion
class spell_pri_power_infusion : public SpellScript
{
    PrepareSpellScript(spell_pri_power_infusion);

    SpellCastResult CheckCast()
    {
        if (Unit* target = GetExplTargetUnit())
            if (target->HasAura(SPELL_MAGE_ARCANE_POWER))
                return SPELL_FAILED_AURA_BOUNCED;

        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_pri_power_infusion::CheckCast);
    }
};


// -18137 - Shadow Guard
class spell_pri_shadow_guard : public AuraScript
{
    PrepareAuraScript(spell_pri_shadow_guard);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PRIEST_SHADOW_GUARD_DAMAGE_R1 });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetActionTarget())
            return true;
        return false;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        uint32 triggerSpell = sSpellMgr->GetSpellWithRank(SPELL_PRIEST_SHADOW_GUARD_DAMAGE_R1, aurEff->GetSpellInfo()->GetRank());

        eventInfo.GetActionTarget()->CastSpell(eventInfo.GetActor(), triggerSpell, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pri_shadow_guard::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_pri_shadow_guard::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};


void AddSC_priest_spell_scripts()
{
    RegisterSpellScript(spell_pri_aq_3p_bonus);
    RegisterSpellScript(spell_pri_blessed_recovery);
    RegisterSpellScript(spell_pri_vampiric_embrace);
    RegisterSpellScript(spell_pri_t3_4p_bonus);
    RegisterSpellScript(spell_pri_power_infusion);
    RegisterSpellScript(spell_pri_shadow_guard);
}
