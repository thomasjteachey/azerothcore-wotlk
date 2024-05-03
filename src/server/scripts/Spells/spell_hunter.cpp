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
  * Scripts for spells with SPELLFAMILY_HUNTER, SPELLFAMILY_PET and SPELLFAMILY_GENERIC spells used by hunter players.
  * Ordered alphabetically using scriptname.
  * Scriptnames of files in this file should be prefixed with "spell_hun_".
  */

#include "ScriptMgr.h"
#include "CellImpl.h"
#include "GridNotifiersImpl.h"
#include "Pet.h"
#include "SpellAuraEffects.h"
#include "SpellMgr.h"
#include "SpellScript.h"

enum HunterSpells
{
    SPELL_HUNTER_ASPECT_OF_THE_BEAST = 13161,
    SPELL_HUNTER_ASPECT_OF_THE_BEAST_PET = 61669,
    SPELL_HUNTER_ASPECT_OF_THE_VIPER = 34074,
    SPELL_HUNTER_ASPECT_OF_THE_VIPER_ENERGIZE = 34075,
    SPELL_HUNTER_BESTIAL_WRATH = 19574,
    SPELL_HUNTER_CHIMERA_SHOT_SERPENT = 53353,
    SPELL_HUNTER_CHIMERA_SHOT_VIPER = 53358,
    SPELL_HUNTER_CHIMERA_SHOT_SCORPID = 53359,
    SPELL_HUNTER_GLYPH_OF_ARCANE_SHOT = 61389,
    SPELL_HUNTER_GLYPH_OF_ASPECT_OF_THE_VIPER = 56851,
    SPELL_HUNTER_IMPROVED_MEND_PET = 24406,
    SPELL_HUNTER_INVIGORATION_TRIGGERED = 53398,
    SPELL_HUNTER_MASTERS_CALL_TRIGGERED = 62305,
    SPELL_HUNTER_MISDIRECTION = 34477,
    SPELL_HUNTER_MISDIRECTION_PROC = 35079,
    SPELL_HUNTER_PET_LAST_STAND_TRIGGERED = 53479,
    SPELL_HUNTER_PET_HEART_OF_THE_PHOENIX = 55709,
    SPELL_HUNTER_PET_HEART_OF_THE_PHOENIX_TRIGGERED = 54114,
    SPELL_HUNTER_PET_HEART_OF_THE_PHOENIX_DEBUFF = 55711,
    SPELL_HUNTER_PET_CARRION_FEEDER_TRIGGERED = 54045,
    SPELL_HUNTER_PIERCING_SHOTS = 63468,
    SPELL_HUNTER_READINESS = 23989,
    SPELL_HUNTER_SNIPER_TRAINING_R1 = 53302,
    SPELL_HUNTER_SNIPER_TRAINING_BUFF_R1 = 64418,
    SPELL_HUNTER_T9_4P_GREATNESS = 68130,
    SPELL_HUNTER_VICIOUS_VIPER = 61609,
    SPELL_HUNTER_VIPER_ATTACK_SPEED = 60144,
    SPELL_DRAENEI_GIFT_OF_THE_NAARU = 59543,
    SPELL_ROAR_OF_SACRIFICE_TRIGGERED = 67481,
    SPELL_HUNTER_LOCK_AND_LOAD_TRIGGER = 56453,
    SPELL_HUNTER_LOCK_AND_LOAD_MARKER = 67544,
    SPELL_HUNTER_KILL_COMMAND_HUNTER = 34027,
    SPELL_HUNTER_THRILL_OF_THE_HUNT_MANA = 34720,
    SPELL_REPLENISHMENT = 57669,
    SPELL_HUNTER_RAPID_RECUPERATION_MANA_R1 = 56654,
    SPELL_HUNTER_RAPID_RECUPERATION_MANA_R2 = 58882,
    SPELL_HUNTER_GLYPH_OF_MEND_PET_HAPPINESS = 57894,
    SPELL_HUNTER_EXPLOSIVE_SHOT_DAMAGE = 53352,
    SPELL_HUNTER_FEEDING_FRENZY_BUFF_R1 = 60096,
    SPELL_HUNTER_FEEDING_FRENZY_BUFF_R2 = 60097,
    SPELL_HUNTER_WYVERN_STING_DOT_R1 = 24131,
    SPELL_HUNTER_WYVERN_STING_DOT_R2 = 24134,
    SPELL_HUNTER_WYVERN_STING_DOT_R3 = 24135,
    SPELL_HUNTER_WYVERN_STING_DOT_R4 = 27069,
    SPELL_HUNTER_WYVERN_STING_DOT_R5 = 49009,
    SPELL_HUNTER_WYVERN_STING_DOT_R6 = 49010
};

enum HunterSpellIcons
{
    SPELL_ICON_HUNTER_PET_IMPROVED_COWER = 958
};


// 781 - Disengage
class spell_hun_disengage : public SpellScript
{
    PrepareSpellScript(spell_hun_disengage);

    SpellCastResult CheckCast()
    {
        Unit* caster = GetCaster();
        if (caster->GetTypeId() == TYPEID_PLAYER && !caster->IsInCombat())
            return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_hun_disengage::CheckCast);
    }
};


// -19572 - Improved Mend Pet
class spell_hun_improved_mend_pet : public AuraScript
{
    PrepareAuraScript(spell_hun_improved_mend_pet);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_HUNTER_IMPROVED_MEND_PET });
    }

    bool CheckProc(ProcEventInfo& /*eventInfo*/)
    {
        return roll_chance_i(GetEffect(EFFECT_0)->GetAmount());
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        GetTarget()->CastSpell(GetTarget(), SPELL_HUNTER_IMPROVED_MEND_PET, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_hun_improved_mend_pet::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_hun_improved_mend_pet::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 1742 - Pet Cower
class spell_hun_pet_cower : public AuraScript
{
    PrepareAuraScript(spell_hun_pet_cower);

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        if (AuraEffect const* improvedCower = GetUnitOwner()->GetDummyAuraEffect(SPELLFAMILY_PET, SPELL_ICON_HUNTER_PET_IMPROVED_COWER, EFFECT_0))
            AddPct(amount, improvedCower->GetAmount());
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_hun_pet_cower::CalculateAmount, EFFECT_1, SPELL_AURA_MOD_DECREASE_SPEED);
    }
};

// 1515 - Tame Beast
class spell_hun_tame_beast : public SpellScript
{
    PrepareSpellScript(spell_hun_tame_beast);

    SpellCastResult CheckCast()
    {
        Player* caster = GetCaster()->ToPlayer();
        if (!caster)
            return SPELL_FAILED_DONT_REPORT;

        if (!GetExplTargetUnit())
            return SPELL_FAILED_BAD_IMPLICIT_TARGETS;

        if (Creature* target = GetExplTargetUnit()->ToCreature())
        {
            if (target->GetLevel() > caster->GetLevel())
                return SPELL_FAILED_HIGHLEVEL;

            // use SMSG_PET_TAME_FAILURE?
            if (!target->GetCreatureTemplate()->IsTameable(caster->CanTameExoticPets()))
                return SPELL_FAILED_BAD_TARGETS;

            PetStable const* petStable = caster->GetPetStable();
            if (petStable)
            {
                if (petStable->CurrentPet)
                    return SPELL_FAILED_ALREADY_HAVE_SUMMON;

                if (petStable->GetUnslottedHunterPet())
                {
                    caster->SendTameFailure(PET_TAME_TOO_MANY);
                    return SPELL_FAILED_DONT_REPORT;
                }
            }

            if (caster->GetCharmGUID())
                return SPELL_FAILED_ALREADY_HAVE_CHARM;

            if (target->GetOwnerGUID())
            {
                caster->SendTameFailure(PET_TAME_CREATURE_ALREADY_OWNED);
                return SPELL_FAILED_DONT_REPORT;
            }
        }
        else
            return SPELL_FAILED_BAD_IMPLICIT_TARGETS;

        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_hun_tame_beast::CheckCast);
    }
};

// -19386 - Wyvern Sting
class spell_hun_wyvern_sting : public AuraScript
{
    PrepareAuraScript(spell_hun_wyvern_sting);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_HUNTER_WYVERN_STING_DOT_R1,
                SPELL_HUNTER_WYVERN_STING_DOT_R2,
                SPELL_HUNTER_WYVERN_STING_DOT_R3,
                SPELL_HUNTER_WYVERN_STING_DOT_R4,
                SPELL_HUNTER_WYVERN_STING_DOT_R5,
                SPELL_HUNTER_WYVERN_STING_DOT_R6
            });
    }

    void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        uint8 rank = sSpellMgr->GetSpellRank(GetId());
        uint32 spellId = sSpellMgr->GetSpellWithRank(SPELL_HUNTER_WYVERN_STING_DOT_R1, rank);
        if (Unit* caster = GetCaster())
            caster->CastSpell(GetTarget(), spellId, aurEff);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_hun_wyvern_sting::OnRemove, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
    }
};


void AddSC_hunter_spell_scripts()
{
    RegisterSpellScript(spell_hun_disengage);
    RegisterSpellScript(spell_hun_improved_mend_pet);
    RegisterSpellScript(spell_hun_pet_cower);
    RegisterSpellScript(spell_hun_tame_beast);
    RegisterSpellScript(spell_hun_wyvern_sting);
}
