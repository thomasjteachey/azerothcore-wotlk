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
  * Scripts for spells with SPELLFAMILY_MAGE and SPELLFAMILY_GENERIC spells used by mage players.
  * Ordered alphabetically using scriptname.
  * Scriptnames of files in this file should be prefixed with "spell_mage_".
  */

#include "ScriptMgr.h"
#include "Creature.h"
#include "Player.h"
#include "Random.h"
#include "SpellAuraEffects.h"
#include "SpellMgr.h"
#include "SpellScript.h"

enum MageSpells
{
    SPELL_MAGE_BLAZING_SPEED = 31643,
    SPELL_MAGE_BURNOUT = 44450,
    SPELL_MAGE_COLD_SNAP = 12472,
    SPELL_MAGE_FOCUS_MAGIC_PROC = 54648,
    SPELL_MAGE_FROST_WARDING_R1 = 11189,
    SPELL_MAGE_FIRE_WARDING_R1 = 11094,
    SPELL_MAGE_FROST_WARDING_TRIGGERED = 57776,
    SPELL_MAGE_INCANTERS_ABSORBTION_R1 = 44394,
    SPELL_MAGE_INCANTERS_ABSORBTION_TRIGGERED = 44413,
    SPELL_MAGE_IGNITE = 12654,
    SPELL_MAGE_MASTER_OF_ELEMENTS_ENERGIZE = 29077,
    SPELL_MAGE_SQUIRREL_FORM = 32813,
    SPELL_MAGE_GIRAFFE_FORM = 32816,
    SPELL_MAGE_SERPENT_FORM = 32817,
    SPELL_MAGE_DRAGONHAWK_FORM = 32818,
    SPELL_MAGE_WORGEN_FORM = 32819,
    SPELL_MAGE_SHEEP_FORM = 32820,
    SPELL_MAGE_GLYPH_OF_ETERNAL_WATER = 70937,
    SPELL_MAGE_SHATTERED_BARRIER = 55080,
    SPELL_MAGE_SUMMON_WATER_ELEMENTAL_PERMANENT = 70908,
    SPELL_MAGE_SUMMON_WATER_ELEMENTAL_TEMPORARY = 70907,
    SPELL_MAGE_GLYPH_OF_BLAST_WAVE = 62126,
    SPELL_MAGE_CHILLED = 12484,
    SPELL_MAGE_MANA_SURGE = 37445,
    SPELL_MAGE_MAGIC_ABSORPTION_MANA = 29442,
    SPELL_MAGE_ARCANE_POTENCY_RANK_1 = 57529,
    SPELL_MAGE_ARCANE_POTENCY_RANK_2 = 57531,
    SPELL_MAGE_HOT_STREAK_PROC = 48108,
    SPELL_MAGE_ARCANE_SURGE = 37436,
    SPELL_MAGE_COMBUSTION = 11129,
    SPELL_MAGE_COMBUSTION_PROC = 28682,
    SPELL_MAGE_EMPOWERED_FIRE_PROC = 67545,
    SPELL_MAGE_T10_2P_BONUS = 70752,
    SPELL_MAGE_T10_2P_BONUS_EFFECT = 70753,
    SPELL_MAGE_T8_4P_BONUS = 64869,
    SPELL_MAGE_MISSILE_BARRAGE = 44401,
    SPELL_MAGE_FINGERS_OF_FROST_AURASTATE_AURA = 44544,
    SPELL_MAGE_PERMAFROST_AURA = 68391,
    SPELL_MAGE_ARCANE_MISSILES_R1 = 5143
};

enum MageSpellIcons
{
    SPELL_ICON_MAGE_SHATTERED_BARRIER = 2945,
    SPELL_ICON_MAGE_PRESENCE_OF_MIND = 139,
    SPELL_ICON_MAGE_CLEARCASTING = 212,
    SPELL_ICON_MAGE_LIVING_BOMB = 3000
};


// 12472 - Cold Snap
class spell_mage_cold_snap : public SpellScript
{
    PrepareSpellScript(spell_mage_cold_snap);

    bool Load() override
    {
        return GetCaster()->GetTypeId() == TYPEID_PLAYER;
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Player* caster = GetCaster()->ToPlayer();
        // immediately finishes the cooldown on Frost spells

        PlayerSpellMap const& spellMap = caster->GetSpellMap();
        for (PlayerSpellMap::const_iterator itr = spellMap.begin(); itr != spellMap.end(); ++itr)
        {
            SpellInfo const* spellInfo = sSpellMgr->AssertSpellInfo(itr->first);
            if (spellInfo->SpellFamilyName == SPELLFAMILY_MAGE && (spellInfo->GetSchoolMask() & SPELL_SCHOOL_MASK_FROST) && spellInfo->Id != SPELL_MAGE_COLD_SNAP && spellInfo->GetRecoveryTime() > 0)
            {
                SpellCooldowns::iterator citr = caster->GetSpellCooldownMap().find(spellInfo->Id);
                if (citr != caster->GetSpellCooldownMap().end() && citr->second.needSendToClient)
                    caster->RemoveSpellCooldown(spellInfo->Id, true);
                else
                    caster->RemoveSpellCooldown(spellInfo->Id, false);
            }
        }
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_mage_cold_snap::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 11129 - Combustion
class spell_mage_combustion : public AuraScript
{
    PrepareAuraScript(spell_mage_combustion);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_COMBUSTION_PROC });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        // Do not take charges, add a stack of crit buff
        if (!(eventInfo.GetHitMask() & PROC_HIT_CRITICAL))
        {
            eventInfo.GetActor()->CastSpell(nullptr, SPELL_MAGE_COMBUSTION_PROC);
            return false;
        }

        return true;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_combustion::CheckProc);
    }
};

// 28682 - Combustion proc
class spell_mage_combustion_proc : public AuraScript
{
    PrepareAuraScript(spell_mage_combustion_proc);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_COMBUSTION });
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->RemoveAurasDueToSpell(SPELL_MAGE_COMBUSTION);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_mage_combustion_proc::OnRemove, EFFECT_0, SPELL_AURA_ADD_FLAT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
    }
};

// -11185 - Improved Blizzard
class spell_mage_imp_blizzard : public AuraScript
{
    PrepareAuraScript(spell_mage_imp_blizzard);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_CHILLED });
    }

    void HandleChill(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        uint32 triggerSpellId = sSpellMgr->GetSpellWithRank(SPELL_MAGE_CHILLED, GetSpellInfo()->GetRank());
        eventInfo.GetActor()->CastSpell(eventInfo.GetProcTarget(), triggerSpellId, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_mage_imp_blizzard::HandleChill, EFFECT_0, SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
    }
};


// -543  - Fire Ward
// -6143 - Frost Ward
class spell_mage_fire_frost_ward : public AuraScript
{
    PrepareAuraScript(spell_mage_fire_frost_ward);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_FROST_WARDING_TRIGGERED,
                SPELL_MAGE_FROST_WARDING_R1
            });
    }

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = false;
        if (Unit* caster = GetCaster())
        {
            // +80.68% from sp bonus
            float bonus = 0.8068f;

            bonus *= caster->SpellBaseHealingBonusDone(GetSpellInfo()->GetSchoolMask());
            bonus *= caster->CalculateLevelPenalty(GetSpellInfo());

            amount += int32(bonus);
        }
    }

    void CalculateReflectChance(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = false;
        if (Unit* caster = GetCaster())
        {
            AuraApplication* frostWarding = caster->GetAuraApplicationOfRankedSpell(SPELL_MAGE_FROST_WARDING_R1);
            if (frostWarding)
            {
                //frost ward
                if (GetSpellInfo()->GetSchoolMask() & SPELL_SCHOOL_MASK_FROST)
                {
                    int32 chance = frostWarding->GetBase()->GetSpellInfo()->GetEffect(EFFECT_1).CalcValue();
                    amount += chance;
                }
            }
            AuraApplication* impFireWard = caster->GetAuraApplicationOfRankedSpell(SPELL_MAGE_FIRE_WARDING_R1);
            if (impFireWard)
            {
                //fire ward
                if (GetSpellInfo()->GetSchoolMask() & SPELL_SCHOOL_MASK_FIRE)
                {
                    int32 chance = impFireWard->GetBase()->GetSpellInfo()->GetEffect(EFFECT_0).CalcValue();
                    amount += chance;
                }
            }
        }
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_fire_frost_ward::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_fire_frost_ward::CalculateReflectChance, EFFECT_1, SPELL_AURA_REFLECT_SPELLS_SCHOOL);
    }
};

// -11426 - Ice Barrier
class spell_mage_ice_barrier : public AuraScript
{
    PrepareAuraScript(spell_mage_ice_barrier);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_SHATTERED_BARRIER });
    }

    void CalculateAmount(AuraEffect const* aurEff, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = false;
        if (Unit* caster = GetCaster())
        {
            // +80.68% from sp bonus
            float bonus = 0.8068f;

            bonus *= caster->SpellBaseHealingBonusDone(GetSpellInfo()->GetSchoolMask());

            // Glyph of Ice Barrier: its weird having a SPELLMOD_ALL_EFFECTS here but its blizzards doing :)
            // Glyph of Ice Barrier is only applied at the spell damage bonus because it was already applied to the base value in CalculateSpellDamage
            bonus = caster->ApplyEffectModifiers(GetSpellInfo(), aurEff->GetEffIndex(), bonus);

            bonus *= caster->CalculateLevelPenalty(GetSpellInfo());

            amount += int32(bonus);
        }
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_ice_barrier::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
    }
};

// -11119 - Ignite
class spell_mage_ignite : public AuraScript
{
    PrepareAuraScript(spell_mage_ignite);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_IGNITE });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetDamageInfo() && eventInfo.GetProcTarget();
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        SpellInfo const* igniteDot = sSpellMgr->AssertSpellInfo(SPELL_MAGE_IGNITE);
        int32 pct = 8 * GetSpellInfo()->GetRank();

        ASSERT(igniteDot->GetMaxTicks() > 0);
        int32 amount = int32(CalculatePct(eventInfo.GetDamageInfo()->GetDamage(), pct) / igniteDot->GetMaxTicks());

        eventInfo.GetProcTarget()->CastCustomSpell(SPELL_MAGE_IGNITE, SPELLVALUE_BASE_POINT0, amount);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_ignite::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_mage_ignite::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};


// -29441 - Magic Absorption
class spell_mage_magic_absorption : public AuraScript
{
    PrepareAuraScript(spell_mage_magic_absorption);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_MAGIC_ABSORPTION_MANA });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        Unit* caster = eventInfo.GetActionTarget();
        caster->CastCustomSpell(SPELL_MAGE_MAGIC_ABSORPTION_MANA, SPELLVALUE_BASE_POINT0, CalculatePct(caster->GetMaxPower(POWER_MANA), aurEff->GetAmount()));
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_mage_magic_absorption::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// -1463 - Mana Shield
class spell_mage_mana_shield : public AuraScript
{
    PrepareAuraScript(spell_mage_mana_shield);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_ARCANE_SURGE });
    }

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = false;
        if (Unit* caster = GetCaster())
        {
            // +80.53% from sp bonus
            float bonus = 0.8053f;

            bonus *= caster->SpellBaseHealingBonusDone(GetSpellInfo()->GetSchoolMask());
            bonus *= caster->CalculateLevelPenalty(GetSpellInfo());

            amount += int32(bonus);
        }
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        Unit* caster = eventInfo.GetActionTarget();
        caster->CastSpell(caster, SPELL_MAGE_ARCANE_SURGE, aurEff);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_mana_shield::CalculateAmount, EFFECT_0, SPELL_AURA_MANA_SHIELD);

        OnEffectProc += AuraEffectProcFn(spell_mage_mana_shield::HandleProc, EFFECT_0, SPELL_AURA_MANA_SHIELD);
    }
};

// -29074 - Master of Elements
class spell_mage_master_of_elements : public AuraScript
{
    PrepareAuraScript(spell_mage_master_of_elements);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_MASTER_OF_ELEMENTS_ENERGIZE });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        DamageInfo* damageInfo = eventInfo.GetDamageInfo();
        if (!damageInfo || !damageInfo->GetSpellInfo())
            return false;

        return true;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        int32 mana = eventInfo.GetDamageInfo()->GetSpellInfo()->CalcPowerCost(GetTarget(), eventInfo.GetDamageInfo()->GetSchoolMask());
        mana = CalculatePct(mana, aurEff->GetAmount());

        if (mana > 0)
        {
            GetTarget()->CastCustomSpell(SPELL_MAGE_MASTER_OF_ELEMENTS_ENERGIZE, SPELLVALUE_BASE_POINT0, mana);
        }
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_master_of_elements::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_mage_master_of_elements::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

enum SilvermoonPolymorph
{
    NPC_AUROSALIA = 18744,
};

/// @todo move out of here and rename - not a mage spell
// 32826 - Polymorph (Visual)
class spell_mage_polymorph_cast_visual : public SpellScriptLoader
{
public:
    spell_mage_polymorph_cast_visual() : SpellScriptLoader("spell_mage_polymorph_visual") { }

    class spell_mage_polymorph_cast_visual_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_mage_polymorph_cast_visual_SpellScript);

        static const uint32 PolymorphForms[6];

        bool Validate(SpellInfo const* /*spellInfo*/) override
        {
            return ValidateSpellInfo(PolymorphForms);
        }

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            if (Unit* target = GetCaster()->FindNearestCreature(NPC_AUROSALIA, 30.0f))
                if (target->GetTypeId() == TYPEID_UNIT)
                    target->CastSpell(target, PolymorphForms[urand(0, 5)], true);
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_mage_polymorph_cast_visual_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_mage_polymorph_cast_visual_SpellScript();
    }
};

const uint32 spell_mage_polymorph_cast_visual::spell_mage_polymorph_cast_visual_SpellScript::PolymorphForms[6] =
{
    SPELL_MAGE_SQUIRREL_FORM,
    SPELL_MAGE_GIRAFFE_FORM,
    SPELL_MAGE_SERPENT_FORM,
    SPELL_MAGE_DRAGONHAWK_FORM,
    SPELL_MAGE_WORGEN_FORM,
    SPELL_MAGE_SHEEP_FORM
};

void AddSC_mage_spell_scripts()
{
    RegisterSpellScript(spell_mage_cold_snap);
    RegisterSpellScript(spell_mage_combustion);
    RegisterSpellScript(spell_mage_combustion_proc);
    RegisterSpellScript(spell_mage_imp_blizzard);
    RegisterSpellScript(spell_mage_fire_frost_ward);
    RegisterSpellScript(spell_mage_ice_barrier);
    RegisterSpellScript(spell_mage_ignite);
    RegisterSpellScript(spell_mage_magic_absorption);
    RegisterSpellScript(spell_mage_mana_shield);
    RegisterSpellScript(spell_mage_master_of_elements);
    new spell_mage_polymorph_cast_visual();
}
