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
  * Scripts for spells with SPELLFAMILY_PALADIN and SPELLFAMILY_GENERIC spells used by paladin players.
  * Ordered alphabetically using scriptname.
  * Scriptnames of files in this file should be prefixed with "spell_pal_".
  */

#include "ScriptMgr.h"
#include "Containers.h"
#include "GameTime.h"
#include "Group.h"
#include "Player.h"
#include "Random.h"
#include "SpellAuraEffects.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "Item.h"
#include "Spell.h"

enum PaladinSpells
{
    SPELL_PALADIN_DIVINE_PLEA = 54428,
    SPELL_PALADIN_BLESSING_OF_SANCTUARY_BUFF = 67480,
    SPELL_PALADIN_BLESSING_OF_SANCTUARY_ENERGIZE = 57319,

    SPELL_PALADIN_HOLY_SHOCK_R1 = 20473,
    SPELL_PALADIN_HOLY_SHOCK_R1_DAMAGE = 25912,
    SPELL_PALADIN_HOLY_SHOCK_R1_HEALING = 25914,
    SPELL_PALADIN_ILLUMINATION_ENERGIZE = 20272,

    SPELL_PALADIN_BLESSING_OF_LOWER_CITY_DRUID = 37878,
    SPELL_PALADIN_BLESSING_OF_LOWER_CITY_PALADIN = 37879,
    SPELL_PALADIN_BLESSING_OF_LOWER_CITY_PRIEST = 37880,
    SPELL_PALADIN_BLESSING_OF_LOWER_CITY_SHAMAN = 37881,

    SPELL_PALADIN_BEACON_OF_LIGHT = 53563,
    SPELL_PALADIN_BEACON_OF_LIGHT_HEAL_1 = 53652,
    SPELL_PALADIN_BEACON_OF_LIGHT_HEAL_2 = 53653,
    SPELL_PALADIN_BEACON_OF_LIGHT_HEAL_3 = 53654,
    SPELL_PALADIN_HOLY_LIGHT = 635,

    SPELL_PALADIN_DIVINE_STORM = 53385,
    SPELL_PALADIN_DIVINE_STORM_DUMMY = 54171,
    SPELL_PALADIN_DIVINE_STORM_HEAL = 54172,

    SPELL_PALADIN_EYE_FOR_AN_EYE_DAMAGE = 25997,

    SPELL_PALADIN_FORBEARANCE = 25771,
    SPELL_PALADIN_AVENGING_WRATH_MARKER = 61987,
    SPELL_PALADIN_IMMUNE_SHIELD_MARKER = 61988,

    SPELL_PALADIN_ITEM_HEALING_TRANCE = 37706,

    SPELL_PALADIN_JUDGEMENT_DAMAGE = 54158,
    SPELL_PALADIN_JUDGEMENT_OF_JUSTICE = 20184,
    SPELL_PALADIN_JUDGEMENT_OF_LIGHT = 20185,
    SPELL_PALADIN_JUDGEMENT_OF_WISDOM = 20186,

    SPELL_PALADIN_JUDGEMENT_OF_LIGHT_HEAL = 20267,
    SPELL_PALADIN_JUDGEMENT_OF_WISDOM_MANA = 20268,

    SPELL_PALADIN_GLYPH_OF_SALVATION = 63225,

    SPELL_PALADIN_RIGHTEOUS_DEFENSE_TAUNT = 31790,

    SPELL_PALADIN_SANCTIFIED_WRATH = 57318,
    SPELL_PALADIN_SANCTIFIED_WRATH_TALENT_R1 = 53375,

    SPELL_PALADIN_SEAL_OF_RIGHTEOUSNESS = 25742,

    SPELL_PALADIN_CONCENTRACTION_AURA = 19746,
    SPELL_PALADIN_SANCTIFIED_RETRIBUTION_R1 = 31869,
    SPELL_PALADIN_SWIFT_RETRIBUTION_R1 = 53379,

    SPELL_PALADIN_IMPROVED_CONCENTRACTION_AURA = 63510,
    SPELL_PALADIN_IMPROVED_DEVOTION_AURA = 63514,
    SPELL_PALADIN_SANCTIFIED_RETRIBUTION_AURA = 63531,
    SPELL_PALADIN_AURA_MASTERY_IMMUNE = 64364,

    SPELL_GENERIC_ARENA_DAMPENING = 74410,
    SPELL_GENERIC_BATTLEGROUND_DAMPENING = 74411,

    SPELL_PALADIN_SACRED_SHIELD = 53601,
    SPELL_PALADIN_T9_HOLY_4P_BONUS = 67191,
    SPELL_PALADIN_FLASH_OF_LIGHT_PROC = 66922,

    SPELL_PALADIN_JUDGEMENTS_OF_THE_JUST_PROC = 68055,

    SPELL_PALADIN_GLYPH_OF_DIVINITY_PROC = 54986,

    SPELL_PALADIN_JUDGEMENTS_OF_THE_WISE_MANA = 31930,
    SPELL_REPLENISHMENT = 57669,
    SPELL_PALADIN_RIGHTEOUS_VENGEANCE_DAMAGE = 61840,
    SPELL_PALADIN_SHEATH_OF_LIGHT_HEAL = 54203,
    SPELL_PALADIN_SACRED_SHIELD_TRIGGER = 58597,
    SPELL_PALADIN_T8_HOLY_4P_BONUS = 64895,
    SPELL_PALADIN_HEART_OF_THE_CRUSADER_EFF_R1 = 21183,

    SPELL_PALADIN_HOLY_POWER_ARMOR = 28790,
    SPELL_PALADIN_HOLY_POWER_ATTACK_POWER = 28791,
    SPELL_PALADIN_HOLY_POWER_SPELL_POWER = 28793,
    SPELL_PALADIN_HOLY_POWER_MP5 = 28795,

    SPELL_PALADIN_HOLY_VENGEANCE = 31803,
    SPELL_PALADIN_SEAL_OF_VENGEANCE_DAMAGE = 42463,
    SPELL_PALADIN_BLOOD_CORRUPTION = 53742,
    SPELL_PALADIN_SEAL_OF_CORRUPTION_DAMAGE = 53739,

    SPELL_PALADIN_SPIRITUAL_ATTUNEMENT_MANA = 31786,

    SPELL_PALADIN_ENDURING_LIGHT = 40471,
    SPELL_PALADIN_ENDURING_JUDGEMENT = 40472,

    SPELL_PALADIN_GLYPH_OF_HOLY_LIGHT_HEAL = 54968,
    SPELL_PALADIN_HOLY_MENDING = 64891
};

enum PaladinSpellIcons
{
    PALADIN_ICON_ID_RETRIBUTION_AURA = 555,
    PALADIN_ICON_ID_HAMMER_OF_THE_RIGHTEOUS = 3023
};

enum MiscSpellIcons
{
    SPELL_ICON_ID_STRENGTH_OF_WRYNN = 1704,
    SPELL_ICON_ID_HELLSCREAM_WARSONG = 937
};

// -9799 - Eye for an Eye
class spell_pal_eye_for_an_eye : public AuraScript
{
    PrepareAuraScript(spell_pal_eye_for_an_eye);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PALADIN_EYE_FOR_AN_EYE_DAMAGE });
    }

    void OnProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        DamageInfo* damageInfo = eventInfo.GetDamageInfo();
        if (!damageInfo || !damageInfo->GetDamage())
            return;

        // return damage % to attacker but < 50% own total health
        int32 damage = std::min(CalculatePct(static_cast<int32>(damageInfo->GetDamage()), aurEff->GetAmount()), static_cast<int32>(GetTarget()->GetMaxHealth()) / 2);
        GetTarget()->CastCustomSpell(SPELL_PALADIN_EYE_FOR_AN_EYE_DAMAGE, SPELLVALUE_BASE_POINT0, damage);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pal_eye_for_an_eye::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 6940 - Hand of Sacrifice
class spell_pal_hand_of_sacrifice : public AuraScript
{
    PrepareAuraScript(spell_pal_hand_of_sacrifice);

    uint32 remainingAmount = 0;

    bool Load() override
    {
        if (Unit* caster = GetCaster())
        {
            remainingAmount = caster->GetMaxHealth();
            return true;
        }
        return false;
    }

    void Split(AuraEffect* /*aurEff*/, DamageInfo& /*dmgInfo*/, uint32& splitAmount)
    {
        if (remainingAmount >= splitAmount)
            remainingAmount -= splitAmount;
        else
        {
            splitAmount = remainingAmount;
            Remove();
        }
    }

    void Register() override
    {
        OnEffectSplit += AuraEffectSplitFn(spell_pal_hand_of_sacrifice::Split, EFFECT_0);
    }
};

// 1038 - Hand of Salvation
class spell_pal_hand_of_salvation : public AuraScript
{
    PrepareAuraScript(spell_pal_hand_of_salvation);

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        if (Unit* caster = GetCaster())
        {
            // Glyph of Salvation
            if (caster->GetGUID() == GetUnitOwner()->GetGUID())
                if (AuraEffect const* aurEff = caster->GetAuraEffect(SPELL_PALADIN_GLYPH_OF_SALVATION, EFFECT_0))
                    amount -= aurEff->GetAmount();
        }
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pal_hand_of_salvation::CalculateAmount, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN);
    }
};

// -20473 - Holy Shock
class spell_pal_holy_shock : public SpellScript
{
    PrepareSpellScript(spell_pal_holy_shock);

    bool Validate(SpellInfo const* spellInfo) override
    {
        SpellInfo const* firstRankSpellInfo = sSpellMgr->GetSpellInfo(SPELL_PALADIN_HOLY_SHOCK_R1);
        if (!firstRankSpellInfo)
            return false;

        // can't use other spell than holy shock due to spell_ranks dependency
        if (!spellInfo->IsRankOf(firstRankSpellInfo))
            return false;

        uint8 rank = spellInfo->GetRank();
        if (!sSpellMgr->GetSpellWithRank(SPELL_PALADIN_HOLY_SHOCK_R1_DAMAGE, rank, true) || !sSpellMgr->GetSpellWithRank(SPELL_PALADIN_HOLY_SHOCK_R1_HEALING, rank, true))
            return false;

        return true;
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        if (Unit* unitTarget = GetHitUnit())
        {
            uint8 rank = GetSpellInfo()->GetRank();
            if (caster->IsFriendlyTo(unitTarget))
                caster->CastSpell(unitTarget, sSpellMgr->GetSpellWithRank(SPELL_PALADIN_HOLY_SHOCK_R1_HEALING, rank), true);
            else
                caster->CastSpell(unitTarget, sSpellMgr->GetSpellWithRank(SPELL_PALADIN_HOLY_SHOCK_R1_DAMAGE, rank), true);
        }
    }

    SpellCastResult CheckCast()
    {
        Unit* caster = GetCaster();
        if (Unit* target = GetExplTargetUnit())
        {
            if (!caster->IsFriendlyTo(target))
            {
                if (!caster->IsValidAttackTarget(target))
                    return SPELL_FAILED_BAD_TARGETS;

                if (!caster->isInFront(target))
                    return SPELL_FAILED_UNIT_NOT_INFRONT;
            }
        }
        else
            return SPELL_FAILED_BAD_TARGETS;
        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_pal_holy_shock::CheckCast);
        OnEffectHitTarget += SpellEffectFn(spell_pal_holy_shock::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// -20210 - Illumination
class spell_pal_illumination : public AuraScript
{
    PrepareAuraScript(spell_pal_illumination);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_PALADIN_HOLY_SHOCK_R1_HEALING,
                SPELL_PALADIN_ILLUMINATION_ENERGIZE,
                SPELL_PALADIN_HOLY_SHOCK_R1
            });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        // this script is valid only for the Holy Shock procs of illumination
        if (eventInfo.GetHealInfo() && eventInfo.GetHealInfo()->GetSpellInfo())
        {
            SpellInfo const* originalSpell = nullptr;

            // if proc comes from the Holy Shock heal, need to get mana cost of original spell - else it's the original heal itself
            if (eventInfo.GetHealInfo()->GetSpellInfo()->SpellFamilyFlags[1] & 0x00010000)
                originalSpell = sSpellMgr->GetSpellInfo(sSpellMgr->GetSpellWithRank(SPELL_PALADIN_HOLY_SHOCK_R1, eventInfo.GetHealInfo()->GetSpellInfo()->GetRank()));
            else
                originalSpell = eventInfo.GetHealInfo()->GetSpellInfo();

            if (originalSpell && aurEff->GetSpellInfo())
            {
                Unit* target = eventInfo.GetActor(); // Paladin is the target of the energize
                uint32 bp = originalSpell->CalcPowerCost(target, originalSpell->GetSchoolMask());
                target->CastCustomSpell(SPELL_PALADIN_ILLUMINATION_ENERGIZE, SPELLVALUE_BASE_POINT0, bp);
            }
        }
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pal_illumination::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

//   498 - Divine Protection
//   642 - Divine Shield
// -1022 - Hand of Protection
class spell_pal_immunities : public SpellScript
{
    PrepareSpellScript(spell_pal_immunities);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return true;
    }

    SpellCastResult CheckCast()
    {
        Unit* caster = GetCaster();

        // for HoP
        Unit* target = GetExplTargetUnit();
        if (!target)
            target = caster;

        // "Cannot be used within $61987d. of using Avenging Wrath."
        if (target->HasAura(SPELL_PALADIN_FORBEARANCE) || target->HasAura(SPELL_PALADIN_AVENGING_WRATH_MARKER))
            return SPELL_FAILED_TARGET_AURASTATE;

        return SPELL_CAST_OK;
    }

    void TriggerDebuffs()
    {
        if (Unit* target = GetHitUnit())
        {
            // Blizz seems to just apply aura without bothering to cast
            GetCaster()->AddAura(SPELL_PALADIN_FORBEARANCE, target);
            GetCaster()->AddAura(SPELL_PALADIN_AVENGING_WRATH_MARKER, target);
            GetCaster()->AddAura(SPELL_PALADIN_IMMUNE_SHIELD_MARKER, target);
        }
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_pal_immunities::CheckCast);
        AfterHit += SpellHitFn(spell_pal_immunities::TriggerDebuffs);
    }
};

// Maybe this is incorrect
// These spells should always be cast on login, regardless of whether the player has the talent or not

// -20254 - Improved Concentration Aura
// -20138 - Improved Devotion Aura
//  31869 - Sanctified Retribution
// -53379 - Swift Retribution
class spell_pal_improved_aura : public AuraScript
{
    PrepareAuraScript(spell_pal_improved_aura);

public:
    spell_pal_improved_aura(uint32 spellId) : AuraScript(), _spellId(spellId) { }

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                _spellId,
                SPELL_PALADIN_SANCTIFIED_RETRIBUTION_R1,
                SPELL_PALADIN_SWIFT_RETRIBUTION_R1
            });
    }

    void HandleEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();
        GetTarget()->RemoveOwnedAura(_spellId, GetCasterGUID()); // need to remove to reapply spellmods
        target->CastSpell(target, _spellId, true);
    }

    void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        uint32 spellId = GetSpellInfo()->GetFirstRankSpell()->Id;

        if ((spellId == SPELL_PALADIN_SANCTIFIED_RETRIBUTION_R1 && GetTarget()->GetAuraOfRankedSpell(SPELL_PALADIN_SWIFT_RETRIBUTION_R1))
            || (spellId == SPELL_PALADIN_SWIFT_RETRIBUTION_R1 && GetTarget()->GetAuraOfRankedSpell(SPELL_PALADIN_SANCTIFIED_RETRIBUTION_R1)))
            return;

        GetTarget()->RemoveOwnedAura(_spellId, GetCasterGUID());
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_pal_improved_aura::HandleEffectApply, EFFECT_FIRST_FOUND, SPELL_AURA_ADD_FLAT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_pal_improved_aura::HandleEffectRemove, EFFECT_FIRST_FOUND, SPELL_AURA_ADD_FLAT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
    }

private:
    uint32 _spellId;
};

// -20234 - Improved Lay on Hands
class spell_pal_improved_lay_of_hands : public AuraScript
{
    PrepareAuraScript(spell_pal_improved_lay_of_hands);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ spellInfo->GetEffect(EFFECT_0).TriggerSpell });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        eventInfo.GetActionTarget()->CastCustomSpell(aurEff->GetSpellInfo()->GetEffect(EFFECT_0).TriggerSpell, SPELLVALUE_BASE_POINT0, aurEff->GetAmount(), GetTarget());

        //eventInfo.GetActionTarget()->CastSpell(eventInfo.GetActionTarget(), aurEff->GetSpellInfo()->GetEffect(EFFECT_0).TriggerSpell, { aurEff, GetTarget()->GetGUID() });
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pal_improved_lay_of_hands::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 53407 - Judgement of Justice
// 20271 - Judgement of Light
// 53408 - Judgement of Wisdom
class spell_pal_judgement : public SpellScript
{
    PrepareSpellScript(spell_pal_judgement);

public:
    spell_pal_judgement(uint32 spellId) : SpellScript(), _spellId(spellId) { }

private:
    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_PALADIN_JUDGEMENT_DAMAGE,
                _spellId
            });
    }

    void HandleScriptEffect(SpellEffIndex /*effIndex*/)
    {
        uint32 spellId2 = SPELL_PALADIN_JUDGEMENT_DAMAGE;

        // some seals have SPELL_AURA_DUMMY in EFFECT_2
        Unit::AuraEffectList const& auras = GetCaster()->GetAuraEffectsByType(SPELL_AURA_DUMMY);
        for (Unit::AuraEffectList::const_iterator i = auras.begin(); i != auras.end(); ++i)
        {
            if ((*i)->GetSpellInfo()->GetSpellSpecific() == SPELL_SPECIFIC_SEAL)
            {
                if ((*i)->GetEffIndex() == EFFECT_2)
                {
                    if (sSpellMgr->GetSpellInfo((*i)->GetAmount()))
                    {
                        spellId2 = (*i)->GetAmount();
                        //found seal remove and break
                        GetCaster()->RemoveAurasDueToSpell((*i)->GetSpellInfo()->Id);
                        break;
                    }
                }
            }
        }
        //GetCaster()->CastSpell(GetHitUnit(), _spellId, true);
        GetCaster()->CastSpell(GetHitUnit(), spellId2, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_pal_judgement::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }

    uint32 const _spellId;
};

// 20425 - Judgement of Command
class spell_pal_judgement_of_command : public SpellScript
{
    PrepareSpellScript(spell_pal_judgement_of_command);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Unit* unitTarget = GetHitUnit())
            GetCaster()->CastSpell(unitTarget, GetEffectValue(), true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_pal_judgement_of_command::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 20185 - Judgement of Light
class spell_pal_judgement_of_light_heal : public AuraScript
{
    PrepareAuraScript(spell_pal_judgement_of_light_heal);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PALADIN_JUDGEMENT_OF_LIGHT_HEAL });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        Unit* caster = eventInfo.GetProcTarget();
        uint32 triggerSpell = sSpellMgr->GetSpellWithRank(SPELL_PALADIN_JUDGEMENT_OF_LIGHT_HEAL, aurEff->GetSpellInfo()->GetRank());
        /*
        CastSpellExtraArgs args(aurEff);
        args.OriginalCaster = GetCasterGUID();
        args.AddSpellBP0(caster->CountPctFromMaxHealth(aurEff->GetAmount()));
        */
        caster->CastSpell(nullptr, triggerSpell);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pal_judgement_of_light_heal::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 20186 - Judgement of Wisdom
class spell_pal_judgement_of_wisdom_mana : public AuraScript
{
    PrepareAuraScript(spell_pal_judgement_of_wisdom_mana);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PALADIN_JUDGEMENT_OF_WISDOM_MANA });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetProcTarget()->getPowerType() == POWER_MANA;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        SpellInfo const* spellInfo = sSpellMgr->AssertSpellInfo(SPELL_PALADIN_JUDGEMENT_OF_WISDOM_MANA);

        Unit* caster = eventInfo.GetProcTarget();
        uint32 triggerSpell = sSpellMgr->GetSpellWithRank(SPELL_PALADIN_JUDGEMENT_OF_WISDOM_MANA, aurEff->GetSpellInfo()->GetRank());
        /*
        int32 const amount = CalculatePct(static_cast<int32>(caster->GetCreateMana()), spellInfo->GetEffect(EFFECT_0).CalcValue());
        CastSpellExtraArgs args(aurEff);
        args.OriginalCaster = GetCasterGUID();
        args.AddSpellBP0(amount);
        */
        caster->CastSpell(nullptr, spellInfo->Id);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pal_judgement_of_wisdom_mana::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_pal_judgement_of_wisdom_mana::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// -633 - Lay on Hands
class spell_pal_lay_on_hands : public SpellScript
{
    PrepareSpellScript(spell_pal_lay_on_hands);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_PALADIN_FORBEARANCE,
                SPELL_PALADIN_AVENGING_WRATH_MARKER,
                SPELL_PALADIN_IMMUNE_SHIELD_MARKER
            });
    }

    SpellCastResult CheckCast()
    {
        Unit* caster = GetCaster();
        if (Unit* target = GetExplTargetUnit())
            if (caster == target)
                if (target->HasAura(SPELL_PALADIN_FORBEARANCE) || target->HasAura(SPELL_PALADIN_AVENGING_WRATH_MARKER) || target->HasAura(SPELL_PALADIN_IMMUNE_SHIELD_MARKER))
                    return SPELL_FAILED_TARGET_AURASTATE;

        return SPELL_CAST_OK;
    }

    void HandleScript()
    {
        Unit* caster = GetCaster();
        if (caster == GetHitUnit())
        {
            caster->CastSpell(caster, SPELL_PALADIN_FORBEARANCE, true);
            caster->CastSpell(caster, SPELL_PALADIN_AVENGING_WRATH_MARKER, true);
            caster->CastSpell(caster, SPELL_PALADIN_IMMUNE_SHIELD_MARKER, true);
        }
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_pal_lay_on_hands::CheckCast);
        AfterHit += SpellHitFn(spell_pal_lay_on_hands::HandleScript);
    }
};

// 21082 - Seal of the Crusader
class spell_seal_crusader : public AuraScript
{
    PrepareAuraScript(spell_seal_crusader);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return true;
    }

    void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        
        Aura* aura = aurEff->GetBase();
        float amount = aura->GetEffect(1)->GetAmount();
        float reduction = (-100.0f * amount) / (amount + 100.0f);
        Unit* caster = GetCaster();
        //apply custom seal of crusader damage reduction spell
        caster->CastSpell(nullptr, 80866);
        caster->ApplyPercentModFloatValue(UNIT_MOD_DAMAGE_MAINHAND, TOTAL_PCT, reduction);
        
    }

    void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        Unit* caster = GetCaster();
        GetCaster()->RemoveAurasDueToSpell(80866);
        caster->UpdateDamagePhysical(BASE_ATTACK);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_seal_crusader::OnApply, EFFECT_2, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_seal_crusader::OnRemove, EFFECT_2, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};


// 1826 - Judgement of Wisdom Intermediate
class spell_pal_jud_wis_intermediate : public SpellScript
{
    PrepareSpellScript(spell_pal_jud_wis_intermediate);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return true;
    }

    void HandleProc(SpellEffIndex /*effIndex*/)
    {
        SpellInfo const* info = GetTriggeringSpell();
        if (info == nullptr)
            return;

        uint32 triggerSpell = 0;
        switch (info->Id)
        {
        case 20186: triggerSpell = 20268; break; // Rank 1
        case 20354: triggerSpell = 20352; break; // Rank 2
        case 20355: triggerSpell = 20353; break; // Rank 3
        }
        if (triggerSpell)
        {
            TriggerCastFlags triggerFlags = TriggerCastFlags(TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CAST_IN_PROGRESS);
            GetSpell()->m_targets.GetUnitTarget()->CastSpell(nullptr, triggerSpell, triggerFlags);
        }
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_pal_jud_wis_intermediate::HandleProc, EFFECT_0, SPELL_EFFECT_DUMMY);
    }

};

// 5373 - Judgement of Light Intermediate
class spell_pal_jud_light_intermediate : public SpellScript
{
    PrepareSpellScript(spell_pal_jud_light_intermediate);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return true;
    }

    void HandleProc(SpellEffIndex /*effIndex*/)
    {
        SpellInfo const* info = GetTriggeringSpell();
        if (info == nullptr)
            return;

        uint32 triggerSpell = 0;
        switch (info->Id)
        {
        case 20185: triggerSpell = 20267; break; // Rank 1
        case 20344: triggerSpell = 20341; break; // Rank 2
        case 20345: triggerSpell = 20342; break; // Rank 3
        case 20346: triggerSpell = 20343; break; // Rank 4
        }
        if (triggerSpell)
        {
            TriggerCastFlags triggerFlags = TriggerCastFlags(TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_CAST_IN_PROGRESS);
            GetSpell()->m_targets.GetUnitTarget()->CastSpell(nullptr, triggerSpell, triggerFlags);
        }
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_pal_jud_light_intermediate::HandleProc, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 20154, 21084 - Seal of Righteousness - melee proc dummy (addition ${$MWS*(0.022*$AP+0.044*$SPH)} damage)
class spell_pal_seal_of_righteousness : public AuraScript
{
    PrepareAuraScript(spell_pal_seal_of_righteousness);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PALADIN_SEAL_OF_RIGHTEOUSNESS });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetProcTarget() != nullptr;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        Unit* victim = eventInfo.GetProcTarget();
        Unit* caster = eventInfo.GetActor();

        uint32 sealId = aurEff->GetSpellInfo()->GetRank();
        uint32 procId = SPELL_PALADIN_SEAL_OF_RIGHTEOUSNESS;
        /*
        switch (sealId)
        {
            case 20154: procId = 25742; break;     // Rank 1
            case 21084: procId = 25741; break;     // Rank 1.5
            case 20287: procId = 25740; break;     // Rank 2
            case 20288: procId = 25739; break;     // Rank 3
            case 20289: procId = 25738; break;     // Rank 4
            case 20290: procId = 25737; break;     // Rank 5
            case 20291: procId = 25736; break;     // Rank 6
            case 20292: procId = 25735; break;     // Rank 7
            case 20293: procId = 25713; break;     // Rank 8
            default: break;
        }
        */

        float speed = GetTarget()->GetAttackTime(BASE_ATTACK);
        speed /= 1000.0f;

        float damageBasePoints;
        float coeff;
        float triggerAmount = aurEff->GetAmount();

        if (((Player*)caster)->IsTwoHandUsed())
        {
            // two hand weapon
            damageBasePoints = 1.20f * triggerAmount * 1.2f * 1.03f * speed / 100.0f + 1;
            coeff = .108f * speed;
        }
        else
        {
            // one hand weapon/no weapon
            damageBasePoints = 0.85f * ceil(triggerAmount * 1.2f * 1.03f * speed / 100.0f) - 1;
            coeff = .092f * speed;
        }

        int32 damagePoint = int32(damageBasePoints + 0.03f * (caster->GetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE) + caster->GetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE)) / 2.0f) + 1;

        // apply damage bonuses manually
        if (damagePoint >= 0)
        {
            // currently uses same spell damage fetch as flametongue - need to verify whether SP is supposed to be applied pre-triggered spell bonuses or post
            int32 bonusDamage = caster->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_HOLY);
            damagePoint += bonusDamage * coeff;
        }

        GetTarget()->CastCustomSpell(SPELL_PALADIN_ILLUMINATION_ENERGIZE, SPELLVALUE_BASE_POINT0, damagePoint);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_pal_seal_of_righteousness::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_pal_seal_of_righteousness::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};


// 20375 - Seal of Command
// 21084 - Seal of Righteousness
// 31801 - Seal of Vengeance
// 31892 - Seal of Blood
// 33127 - Seal of Command
// 38008 - Seal of Blood
// 41459 - Seal of Blood
// 53720 - Seal of the Martyr
// 53736 - Seal of Corruption
class spell_pal_seals : public AuraScript
{
    PrepareAuraScript(spell_pal_seals);

    // Effect 2 is used by Judgement code, we prevent the proc to avoid console logging of unknown spell trigger
    bool CheckDummyProc(AuraEffect const* /*aurEff*/, ProcEventInfo& /*eventInfo*/)
    {
        return false;
    }

    void Register() override
    {
        DoCheckEffectProc += AuraCheckEffectProcFn(spell_pal_seals::CheckDummyProc, EFFECT_2, SPELL_AURA_DUMMY);
    }
};

// 28789 - Holy Power
class spell_pal_t3_6p_bonus : public AuraScript
{
    PrepareAuraScript(spell_pal_t3_6p_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_PALADIN_HOLY_POWER_ARMOR,
                SPELL_PALADIN_HOLY_POWER_ATTACK_POWER,
                SPELL_PALADIN_HOLY_POWER_SPELL_POWER,
                SPELL_PALADIN_HOLY_POWER_MP5
            });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        uint32 spellId;
        Unit* caster = eventInfo.GetActor();
        Unit* target = eventInfo.GetProcTarget();

        switch (target->getClass())
        {
        case CLASS_PALADIN:
        case CLASS_PRIEST:
        case CLASS_SHAMAN:
        case CLASS_DRUID:
            spellId = SPELL_PALADIN_HOLY_POWER_MP5;
            break;
        case CLASS_MAGE:
        case CLASS_WARLOCK:
            spellId = SPELL_PALADIN_HOLY_POWER_SPELL_POWER;
            break;
        case CLASS_HUNTER:
        case CLASS_ROGUE:
            spellId = SPELL_PALADIN_HOLY_POWER_ATTACK_POWER;
            break;
        case CLASS_WARRIOR:
            spellId = SPELL_PALADIN_HOLY_POWER_ARMOR;
            break;
        default:
            return;
        }

        caster->CastSpell(target, spellId, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_pal_t3_6p_bonus::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

void AddSC_paladin_spell_scripts()
{
    RegisterSpellScript(spell_pal_eye_for_an_eye);
    RegisterSpellScript(spell_pal_hand_of_sacrifice);
    RegisterSpellScript(spell_pal_hand_of_salvation);
    RegisterSpellScript(spell_pal_holy_shock);
    RegisterSpellScript(spell_pal_illumination);
    RegisterSpellScript(spell_pal_immunities);
    RegisterSpellScriptWithArgs(spell_pal_improved_aura, "spell_pal_improved_concentraction_aura", SPELL_PALADIN_IMPROVED_CONCENTRACTION_AURA);
    RegisterSpellScriptWithArgs(spell_pal_improved_aura, "spell_pal_improved_devotion_aura", SPELL_PALADIN_IMPROVED_DEVOTION_AURA);
    RegisterSpellScriptWithArgs(spell_pal_improved_aura, "spell_pal_sanctified_retribution", SPELL_PALADIN_SANCTIFIED_RETRIBUTION_AURA);
    RegisterSpellScriptWithArgs(spell_pal_improved_aura, "spell_pal_swift_retribution", SPELL_PALADIN_SANCTIFIED_RETRIBUTION_AURA);
    RegisterSpellScript(spell_pal_improved_lay_of_hands);
    RegisterSpellScriptWithArgs(spell_pal_judgement, "spell_pal_judgement_of_justice", SPELL_PALADIN_JUDGEMENT_OF_JUSTICE);
    RegisterSpellScriptWithArgs(spell_pal_judgement, "spell_pal_judgement_of_light", SPELL_PALADIN_JUDGEMENT_OF_LIGHT);
    RegisterSpellScriptWithArgs(spell_pal_judgement, "spell_pal_judgement_of_wisdom", SPELL_PALADIN_JUDGEMENT_OF_WISDOM);
    RegisterSpellScript(spell_pal_judgement_of_command);
    RegisterSpellScript(spell_pal_judgement_of_light_heal);
    RegisterSpellScript(spell_pal_judgement_of_wisdom_mana);
    RegisterSpellScript(spell_pal_lay_on_hands);
    RegisterSpellScript(spell_pal_seal_of_righteousness);
    RegisterSpellScript(spell_pal_seals);
    RegisterSpellScript(spell_pal_t3_6p_bonus);
    RegisterSpellScript(spell_pal_jud_wis_intermediate);
    RegisterSpellScript(spell_pal_jud_light_intermediate);
    RegisterSpellScript(spell_seal_crusader);
}
