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
  * Scripts for spells with SPELLFAMILY_DRUID and SPELLFAMILY_GENERIC spells used by druid players.
  * Ordered alphabetically using scriptname.
  * Scriptnames of files in this file should be prefixed with "spell_dru_".
  */

#include "ScriptMgr.h"
#include "Containers.h"
#include "GameTime.h"
#include "Optional.h"
#include "Player.h"
#include "SpellAuraEffects.h"
#include "SpellMgr.h"
#include "SpellScript.h"

enum DruidSpells
{
    SPELL_DRUID_BEAR_FORM_PASSIVE = 1178,
    SPELL_DRUID_DIRE_BEAR_FORM_PASSIVE = 9635,
    SPELL_DRUID_ECLIPSE_LUNAR_PROC = 48518,
    SPELL_DRUID_ECLIPSE_SOLAR_PROC = 48517,
    SPELL_DRUID_FORMS_TRINKET_BEAR = 37340,
    SPELL_DRUID_FORMS_TRINKET_CAT = 37341,
    SPELL_DRUID_FORMS_TRINKET_MOONKIN = 37343,
    SPELL_DRUID_FORMS_TRINKET_NONE = 37344,
    SPELL_DRUID_FORMS_TRINKET_TREE = 37342,
    SPELL_DRUID_ENRAGE = 5229,
    SPELL_DRUID_ENRAGE_MOD_DAMAGE = 51185,
    SPELL_DRUID_ENRAGED_DEFENSE = 70725,
    SPELL_DRUID_GLYPH_OF_TYPHOON = 62135,
    SPELL_DRUID_IDOL_OF_FERAL_SHADOWS = 34241,
    SPELL_DRUID_IDOL_OF_WORSHIP = 60774,
    SPELL_DRUID_INCREASED_MOONFIRE_DURATION = 38414,
    SPELL_DRUID_ITEM_T8_BALANCE_RELIC = 64950,
    SPELL_DRUID_ITEM_T10_FERAL_4P_BONUS = 70726,
    SPELL_DRUID_KING_OF_THE_JUNGLE = 48492,
    SPELL_DRUID_LIFEBLOOM_ENERGIZE = 64372,
    SPELL_DRUID_LIFEBLOOM_FINAL_HEAL = 33778,
    SPELL_DRUID_LIVING_SEED_HEAL = 48503,
    SPELL_DRUID_LIVING_SEED_PROC = 48504,
    SPELL_DRUID_NATURES_SPLENDOR = 57865,
    SPELL_DRUID_SURVIVAL_INSTINCTS = 50322,
    SPELL_DRUID_SAVAGE_ROAR = 62071,
    SPELL_DRUID_T9_FERAL_RELIC_BEAR = 67354,
    SPELL_DRUID_T9_FERAL_RELIC_CAT = 67355,
    SPELL_DRUID_TIGER_S_FURY_ENERGIZE = 51178,
    SPELL_DRUID_T3_PROC_ENERGIZE_MANA = 28722,
    SPELL_DRUID_T3_PROC_ENERGIZE_RAGE = 28723,
    SPELL_DRUID_T3_PROC_ENERGIZE_ENERGY = 28724,
    SPELL_DRUID_BLESSING_OF_THE_CLAW = 28750,
    SPELL_DRUID_REVITALIZE_ENERGIZE_MANA = 48542,
    SPELL_DRUID_REVITALIZE_ENERGIZE_RAGE = 48541,
    SPELL_DRUID_REVITALIZE_ENERGIZE_ENERGY = 48540,
    SPELL_DRUID_REVITALIZE_ENERGIZE_RP = 48543,
    SPELL_DRUID_GLYPH_OF_INNERVATE_REGEN = 54833,
    SPELL_DRUID_GLYPH_OF_STARFIRE_SCRIPT = 54846,
    SPELL_DRUID_GLYPH_OF_RIP = 54818,
    SPELL_DRUID_RIP_DURATION_LACERATE_DMG = 60141,
    SPELL_DRUID_GLYPH_OF_RAKE_TRIGGERED = 54820,
    SPELL_DRUID_IMP_LEADER_OF_THE_PACK_R1 = 34297,
    SPELL_DRUID_IMP_LEADER_OF_THE_PACK_HEAL = 34299,
    SPELL_DRUID_IMP_LEADER_OF_THE_PACK_MANA = 68285,
    SPELL_DRUID_EXHILARATE = 28742,
    SPELL_DRUID_GLYPH_OF_REJUVENATION_HEAL = 54755,
    SPELL_DRUID_INFUSION = 37238,
    SPELL_DRUID_BLESSING_OF_REMULOS = 40445,
    SPELL_DRUID_BLESSING_OF_ELUNE = 40446,
    SPELL_DRUID_BLESSING_OF_CENARIUS = 40452,
    SPELL_DRUID_LANGUISH = 71023,
    SPELL_DRUID_REJUVENATION_T10_PROC = 70691,
    SPELL_DRUID_BALANCE_T10_BONUS = 70718,
    SPELL_DRUID_BALANCE_T10_BONUS_PROC = 70721,
    SPELL_DRUID_BARKSKIN_01 = 63058,
    SPELL_DRUID_RESTORATION_T10_2P_BONUS = 70658,
    SPELL_DRUID_FRENZIED_REGENERATION_HEAL = 22845,
    SPELL_DRUID_GLYPH_OF_NOURISH = 62971,
    SPELL_DRUID_NURTURING_INSTINCT_R1 = 47179,
    SPELL_DRUID_NURTURING_INSTINCT_R2 = 47180
};

enum MiscSpells
{
    SPELL_CATEGORY_MANGLE_BEAR = 971
};

// 22812 - Barkskin
class spell_dru_barkskin : public AuraScript
{
    PrepareAuraScript(spell_dru_barkskin);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_BARKSKIN_01 });
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->RemoveAurasDueToSpell(SPELL_DRUID_BARKSKIN_01);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_dru_barkskin::OnRemove, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL);
    }
};

// 1178 - Bear Form (Passive)
// 9635 - Dire Bear Form (Passive)
class spell_dru_bear_form_passive : public AuraScript
{
    PrepareAuraScript(spell_dru_bear_form_passive);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_DRUID_ENRAGE,
                SPELL_DRUID_ITEM_T10_FERAL_4P_BONUS
            });
    }

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        if (!GetUnitOwner()->HasAura(SPELL_DRUID_ENRAGE) || GetUnitOwner()->HasAura(SPELL_DRUID_ITEM_T10_FERAL_4P_BONUS))
            return;

        int32 mod = 0;
        switch (GetId())
        {
        case SPELL_DRUID_BEAR_FORM_PASSIVE:
            mod = -27;
            break;
        case SPELL_DRUID_DIRE_BEAR_FORM_PASSIVE:
            mod = -16;
            break;
        default:
            return;
        }
        amount += mod;
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dru_bear_form_passive::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_BASE_RESISTANCE_PCT);
    }
};

// -1850 - Dash
class spell_dru_dash : public AuraScript
{
    PrepareAuraScript(spell_dru_dash);

    void CalculateAmount(AuraEffect const*  /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        // do not set speed if not in cat form
        if (GetUnitOwner()->GetShapeshiftForm() != FORM_CAT)
            amount = 0;
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dru_dash::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_INCREASE_SPEED);
    }
};

// 5229 - Enrage
class spell_dru_enrage : public AuraScript
{
    PrepareAuraScript(spell_dru_enrage);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_DRUID_KING_OF_THE_JUNGLE,
                SPELL_DRUID_ENRAGE_MOD_DAMAGE,
                SPELL_DRUID_ENRAGED_DEFENSE,
                SPELL_DRUID_ITEM_T10_FERAL_4P_BONUS
            });
    }

    void RecalculateBaseArmor()
    {
        // Recalculate modifies the list while we're iterating through it, so let's copy it instead
        Unit::AuraEffectList const& auras = GetTarget()->GetAuraEffectsByType(SPELL_AURA_MOD_BASE_RESISTANCE_PCT);
        std::vector<AuraEffect*> aurEffs(auras.begin(), auras.end());

        for (AuraEffect* aurEff : aurEffs)
        {
            SpellInfo const* spellInfo = aurEff->GetSpellInfo();
            // Dire- / Bear Form (Passive)
            if (spellInfo->SpellFamilyName == SPELLFAMILY_DRUID && spellInfo->SpellFamilyFlags.HasFlag(0x0, 0x0, 0x2))
                aurEff->RecalculateAmount();
        }
    }

    void HandleApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();
        if (AuraEffect const* aurEff = target->GetAuraEffectOfRankedSpell(SPELL_DRUID_KING_OF_THE_JUNGLE, EFFECT_0))
        {
            target->CastCustomSpell(SPELL_DRUID_ENRAGE_MOD_DAMAGE, SPELLVALUE_BASE_POINT0, aurEff->GetAmount(), target, true);
        }

        // Item - Druid T10 Feral 4P Bonus
        if (target->HasAura(SPELL_DRUID_ITEM_T10_FERAL_4P_BONUS))
            target->CastSpell(target, SPELL_DRUID_ENRAGED_DEFENSE, true);

        RecalculateBaseArmor();
    }

    void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->RemoveAurasDueToSpell(SPELL_DRUID_ENRAGE_MOD_DAMAGE);
        GetTarget()->RemoveAurasDueToSpell(SPELL_DRUID_ENRAGED_DEFENSE);

        RecalculateBaseArmor();
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_dru_enrage::HandleApply, EFFECT_0, SPELL_AURA_PERIODIC_ENERGIZE, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_dru_enrage::HandleRemove, EFFECT_0, SPELL_AURA_PERIODIC_ENERGIZE, AURA_EFFECT_HANDLE_REAL);
    }
};

// 22842 - Frenzied Regeneration
class spell_dru_frenzied_regeneration : public AuraScript
{
    PrepareAuraScript(spell_dru_frenzied_regeneration);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_FRENZIED_REGENERATION_HEAL });
    }

    void PeriodicTick(AuraEffect const* aurEff)
    {
        // Converts up to 10 rage per second into health for $d.  Each point of rage is converted into ${$m2/10}.1% of max health.
        if (GetTarget()->getPowerType() != POWER_RAGE)
            return;

        uint32 rage = GetTarget()->GetPower(POWER_RAGE);
        // Nothing to do
        if (!rage)
            return;

        int32 const mod = std::min(static_cast<int32>(rage), 100);

        //every 100 rage, heal for 200. it's usually 20 per rage point (so 20 health per 1 rage or 200 per 10) but rage is stored internally here with an extra digit. 10 rage comes in as 100 (awful).
        //regen variable is how much to heal per 10 rage (happy case) here.
        int32 const regen = ((200 * mod) / 100.f);
        GetTarget()->CastCustomSpell(SPELL_DRUID_FRENZIED_REGENERATION_HEAL, SPELLVALUE_BASE_POINT0, regen);
        GetTarget()->SetPower(POWER_RAGE, rage - mod);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_dru_frenzied_regeneration::PeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};


// 29166 - Innervate
class spell_dru_innervate : public AuraScript
{
    PrepareAuraScript(spell_dru_innervate);

    void CalculateAmount(AuraEffect const* aurEff, int32& amount, bool& /*canBeRecalculated*/)
    {
        if (!aurEff->GetTotalTicks())
        {
            amount = 0;
            return;
        }

        if (Unit* caster = GetCaster())
            amount = int32(CalculatePct(caster->GetCreatePowers(POWER_MANA), amount) / aurEff->GetTotalTicks());
        else
            amount = 0;
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dru_innervate::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_ENERGIZE);
    }
};

// -16972 - Predatory Strikes
class spell_dru_predatory_strikes : public AuraScript
{
    PrepareAuraScript(spell_dru_predatory_strikes);

    void UpdateAmount(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Player* target = GetTarget()->ToPlayer())
            target->UpdateAttackPowerAndDamage();
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_dru_predatory_strikes::UpdateAmount, EFFECT_ALL, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK);
        AfterEffectRemove += AuraEffectRemoveFn(spell_dru_predatory_strikes::UpdateAmount, EFFECT_ALL, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK);
    }
};


// -1079 - Rip
class spell_dru_rip : public AuraScript
{
    PrepareAuraScript(spell_dru_rip);

    bool Load() override
    {
        Unit* caster = GetCaster();
        return caster && GetCaster()->GetTypeId() == TYPEID_PLAYER;
    }

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = false;

        if (Unit* caster = GetCaster())
        {
            // 0.01 * $AP * cp
            uint8 cp = caster->ToPlayer()->GetComboPoints();

            amount += int32(CalculatePct(caster->GetTotalAttackPowerValue(BASE_ATTACK), cp));
        }
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dru_rip::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

// 28716 - Rejuvenation
class spell_dru_t3_2p_bonus : public AuraScript
{
    PrepareAuraScript(spell_dru_t3_2p_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_DRUID_T3_PROC_ENERGIZE_MANA,
                SPELL_DRUID_T3_PROC_ENERGIZE_RAGE,
                SPELL_DRUID_T3_PROC_ENERGIZE_ENERGY
            });
    }

    bool CheckProc(ProcEventInfo& /*eventInfo*/)
    {
        if (!roll_chance_i(50))
            return false;
        return true;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        Unit* target = eventInfo.GetProcTarget();
        uint32 spellId;

        switch (target->getPowerType())
        {
        case POWER_MANA:
            spellId = SPELL_DRUID_T3_PROC_ENERGIZE_MANA;
            break;
        case POWER_RAGE:
            spellId = SPELL_DRUID_T3_PROC_ENERGIZE_RAGE;
            break;
        case POWER_ENERGY:
            spellId = SPELL_DRUID_T3_PROC_ENERGIZE_ENERGY;
            break;
        default:
            return;
        }

        eventInfo.GetActor()->CastSpell(target, spellId, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_dru_t3_2p_bonus::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_dru_t3_2p_bonus::HandleProc, EFFECT_0, SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
    }
};

// 28744 - Regrowth
class spell_dru_t3_6p_bonus : public AuraScript
{
    PrepareAuraScript(spell_dru_t3_6p_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_BLESSING_OF_THE_CLAW });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        eventInfo.GetActor()->CastSpell(eventInfo.GetProcTarget(), SPELL_DRUID_BLESSING_OF_THE_CLAW, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_dru_t3_6p_bonus::HandleProc, EFFECT_0, SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
    }
};

// 28719 - Healing Touch
class spell_dru_t3_8p_bonus : public AuraScript
{
    PrepareAuraScript(spell_dru_t3_8p_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DRUID_EXHILARATE });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        if (!spellInfo)
            return;

        Unit* caster = eventInfo.GetActor();
        int32 amount = CalculatePct(spellInfo->CalcPowerCost(caster, spellInfo->GetSchoolMask()), aurEff->GetAmount());

        GetTarget()->CastCustomSpell(SPELL_DRUID_EXHILARATE, SPELLVALUE_BASE_POINT0, amount);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_dru_t3_8p_bonus::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

class RaidCheck
{
public:
    explicit RaidCheck(Unit const* caster) : _caster(caster)
    {}

    bool operator()(WorldObject* obj) const
    {
        if (Unit* target = obj->ToUnit())
            return !_caster->IsInRaidWith(target);

        return true;
    }

private:
    Unit const* _caster;
};

void AddSC_druid_spell_scripts()
{
    RegisterSpellScript(spell_dru_barkskin);
    RegisterSpellScript(spell_dru_bear_form_passive);
    RegisterSpellScript(spell_dru_dash);
    RegisterSpellScript(spell_dru_enrage);
    RegisterSpellScript(spell_dru_frenzied_regeneration);
    RegisterSpellScript(spell_dru_innervate);
    RegisterSpellScript(spell_dru_predatory_strikes);
    RegisterSpellScript(spell_dru_rip);
    RegisterSpellScript(spell_dru_t3_2p_bonus);
    RegisterSpellScript(spell_dru_t3_6p_bonus);
    RegisterSpellScript(spell_dru_t3_8p_bonus);
}
