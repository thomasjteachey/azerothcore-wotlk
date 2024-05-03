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
  * Scripts for spells with SPELLFAMILY_WARLOCK and SPELLFAMILY_GENERIC spells used by warlock players.
  * Ordered alphabetically using scriptname.
  * Scriptnames of files in this file should be prefixed with "spell_warl_".
  */

#include "ScriptMgr.h"
#include "Creature.h"
#include "GameObject.h"
#include "Log.h"
#include "ObjectMgr.h"
#include "Optional.h"
#include "Player.h"
#include "Random.h"
#include "SpellAuraEffects.h"
#include "SpellMgr.h"
#include "SpellScript.h"

enum WarlockSpells
{
    SPELL_WARLOCK_DRAIN_SOUL_R1 = 1120,
    SPELL_WARLOCK_CREATE_SOULSHARD = 43836,
    SPELL_WARLOCK_CURSE_OF_DOOM_EFFECT = 18662,
    SPELL_WARLOCK_DEMONIC_CIRCLE_SUMMON = 48018,
    SPELL_WARLOCK_DEMONIC_CIRCLE_TELEPORT = 48020,
    SPELL_WARLOCK_DEMONIC_CIRCLE_ALLOW_CAST = 62388,
    SPELL_WARLOCK_DEMONIC_EMPOWERMENT_SUCCUBUS = 54435,
    SPELL_WARLOCK_DEMONIC_EMPOWERMENT_VOIDWALKER = 54443,
    SPELL_WARLOCK_DEMONIC_EMPOWERMENT_FELGUARD = 54508,
    SPELL_WARLOCK_DEMONIC_EMPOWERMENT_FELHUNTER = 54509,
    SPELL_WARLOCK_DEMONIC_EMPOWERMENT_IMP = 54444,
    SPELL_WARLOCK_DEMONIC_PACT_PROC = 48090,
    SPELL_WARLOCK_FEL_SYNERGY_HEAL = 54181,
    SPELL_WARLOCK_GLYPH_OF_DRAIN_SOUL_AURA = 58070,
    SPELL_WARLOCK_GLYPH_OF_DRAIN_SOUL_PROC = 58068,
    SPELL_WARLOCK_GLYPH_OF_SHADOWFLAME = 63311,
    SPELL_WARLOCK_GLYPH_OF_SIPHON_LIFE = 56216,
    SPELL_WARLOCK_HAUNT = 48181,
    SPELL_WARLOCK_HAUNT_HEAL = 48210,
    SPELL_WARLOCK_IMPROVED_HEALTHSTONE_R1 = 18692,
    SPELL_WARLOCK_IMPROVED_HEALTHSTONE_R2 = 18693,
    SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_R1 = 18703,
    SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_R2 = 18704,
    SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_BUFF_R1 = 60955,
    SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_BUFF_R2 = 60956,
    SPELL_WARLOCK_LIFE_TAP_ENERGIZE = 31818,
    SPELL_WARLOCK_LIFE_TAP_ENERGIZE_2 = 32553,
    SPELL_WARLOCK_NETHER_PROTECTION_HOLY = 54370,
    SPELL_WARLOCK_NETHER_PROTECTION_FIRE = 54371,
    SPELL_WARLOCK_NETHER_PROTECTION_FROST = 54372,
    SPELL_WARLOCK_NETHER_PROTECTION_ARCANE = 54373,
    SPELL_WARLOCK_NETHER_PROTECTION_SHADOW = 54374,
    SPELL_WARLOCK_NETHER_PROTECTION_NATURE = 54375,
    SPELL_WARLOCK_SOULSHATTER_EFFECT = 32835,
    SPELL_WARLOCK_SIPHON_LIFE_HEAL = 63106,
    SPELL_WARLOCK_UNSTABLE_AFFLICTION_DISPEL = 31117,
    SPELL_WARLOCK_GLYPH_OF_LIFE_TAP_TRIGGERED = 63321,
    SPELL_WARLOCK_SEED_OF_CORRUPTION_DAMAGE_R1 = 27285,
    SPELL_WARLOCK_SEED_OF_CORRUPTION_GENERIC = 32865,
    SPELL_WARLOCK_SHADOW_TRANCE = 17941,
    SPELL_WARLOCK_SOUL_LEECH_HEAL = 30294,
    SPELL_WARLOCK_IMP_SOUL_LEECH_R1 = 54117,
    SPELL_WARLOCK_SOUL_LEECH_PET_MANA_1 = 54607,
    SPELL_WARLOCK_SOUL_LEECH_PET_MANA_2 = 59118,
    SPELL_WARLOCK_SOUL_LEECH_CASTER_MANA_1 = 54300,
    SPELL_WARLOCK_SOUL_LEECH_CASTER_MANA_2 = 59117,
    SPELL_REPLENISHMENT = 57669,
    SPELL_WARLOCK_SHADOWFLAME = 37378,
    SPELL_WARLOCK_FLAMESHADOW = 37379,
    SPELL_WARLOCK_GLYPH_OF_SUCCUBUS = 56250,
    SPELL_WARLOCK_IMPROVED_DRAIN_SOUL_R1 = 18213,
    SPELL_WARLOCK_IMPROVED_DRAIN_SOUL_PROC = 18371
};

enum WarlockSpellIcons
{
    WARLOCK_ICON_ID_IMPROVED_LIFE_TAP = 208,
    WARLOCK_ICON_ID_MANA_FEED = 1982,
    WARLOCK_ICON_ID_DEMONIC_PACT = 3220
};

// -980 - Curse of Agony
class spell_warl_curse_of_agony : public AuraScript
{
    PrepareAuraScript(spell_warl_curse_of_agony);

    void ApplyEffect(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        _tick_amount = aurEff->GetAmount();
    }

    void HandleEffectPeriodicUpdate(AuraEffect* aurEff)
    {
        switch (aurEff->GetTickNumber())
        {
            // 1..4 ticks, 1/2 from normal tick damage
        case 1:
            aurEff->SetAmount(_tick_amount / 2);
            break;
            // 5..8 ticks have normal tick damage
        case 5:
            aurEff->SetAmount(_tick_amount);
            break;
            // 9..12 ticks, 3/2 from normal tick damage
        case 9:
            aurEff->SetAmount((_tick_amount + 1) * 3 / 2); // +1 prevent 0.5 damage possible lost at 1..4 ticks
            break;
            // 13 and 14 ticks (glyphed only), twice normal tick damage
        case 13:
            aurEff->SetAmount(_tick_amount * 2);
            break;
        }
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_warl_curse_of_agony::ApplyEffect, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        OnEffectUpdatePeriodic += AuraEffectUpdatePeriodicFn(spell_warl_curse_of_agony::HandleEffectPeriodicUpdate, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
private:
    uint32 _tick_amount = 0;
};

// -710 - Banish
class spell_warl_banish : public SpellScript
{
    PrepareSpellScript(spell_warl_banish);

public:
    spell_warl_banish() {}

private:
    void HandleBanish(SpellMissInfo missInfo)
    {
        if (missInfo != SPELL_MISS_IMMUNE)
            return;

        if (Unit* target = GetHitUnit())
        {
            // Casting Banish on a banished target will remove applied aura
            if (Aura* banishAura = target->GetAura(GetSpellInfo()->Id, GetCaster()->GetGUID()))
                banishAura->Remove();
        }
    }

    void Register() override
    {
        BeforeHit += BeforeSpellHitFn(spell_warl_banish::HandleBanish);
    }
};

// 6201 - Create Healthstone (and ranks)
class spell_warl_create_healthstone : public SpellScript
{
    PrepareSpellScript(spell_warl_create_healthstone);

    static uint32 const iTypes[8][3];

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return true;
    }

    SpellCastResult CheckCast()
    {
        if (Player* caster = GetCaster()->ToPlayer())
        {
            uint8 spellRank = GetSpellInfo()->GetRank();
            ItemPosCountVec dest;
            InventoryResult msg = caster->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, iTypes[spellRank - 1][0], 1, nullptr);
            if (msg != EQUIP_ERR_OK)
                return SPELL_FAILED_TOO_MANY_OF_ITEM;
        }
        return SPELL_CAST_OK;
    }

    void HandleScriptEffect(SpellEffIndex effIndex)
    {
        if (Unit* unitTarget = GetHitUnit())
        {
            uint32 rank = 0;
            // Improved Healthstone
            if (AuraEffect const* aurEff = unitTarget->GetDummyAuraEffect(SPELLFAMILY_WARLOCK, 284, 0))
            {
                switch (aurEff->GetId())
                {
                case SPELL_WARLOCK_IMPROVED_HEALTHSTONE_R1:
                    rank = 1;
                    break;
                case SPELL_WARLOCK_IMPROVED_HEALTHSTONE_R2:
                    rank = 2;
                    break;
                default:
                    LOG_ERROR("spells", "Unknown rank of Improved Healthstone id: {}", aurEff->GetId());
                    break;
                }
            }
            uint8 spellRank = GetSpellInfo()->GetRank();
            if (spellRank > 0 && spellRank <= 8)
                CreateItem(effIndex, iTypes[spellRank - 1][rank]);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_warl_create_healthstone::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        OnCheckCast += SpellCheckCastFn(spell_warl_create_healthstone::CheckCast);
    }
};

uint32 const spell_warl_create_healthstone::spell_warl_create_healthstone::iTypes[8][3] =
{
    { 5512, 19004, 19005},              // Minor Healthstone
    { 5511, 19006, 19007},              // Lesser Healthstone
    { 5509, 19008, 19009},              // Healthstone
    { 5510, 19010, 19011},              // Greater Healthstone
    { 9421, 19012, 19013},              // Major Healthstone
    {22103, 22104, 22105},              // Master Healthstone
    {36889, 36890, 36891},              // Demonic Healthstone
    {36892, 36893, 36894}               // Fel Healthstone
};

// -603 - Curse of Doom
class spell_warl_curse_of_doom : public AuraScript
{
    PrepareAuraScript(spell_warl_curse_of_doom);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_CURSE_OF_DOOM_EFFECT });
    }

    bool Load() override
    {
        return GetCaster() && GetCaster()->GetTypeId() == TYPEID_PLAYER;
    }

    void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        if (!GetCaster())
            return;

        AuraRemoveMode removeMode = GetTargetApplication()->GetRemoveMode();
        if (removeMode != AURA_REMOVE_BY_DEATH || !IsExpired())
            return;

        if (GetCaster()->ToPlayer()->isHonorOrXPTarget(GetTarget()))
            GetCaster()->CastSpell(GetTarget(), SPELL_WARLOCK_CURSE_OF_DOOM_EFFECT, aurEff);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_warl_curse_of_doom::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
    }
};


// -1120 - Drain Soul
class spell_warl_drain_soul : public AuraScript
{
    PrepareAuraScript(spell_warl_drain_soul);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_WARLOCK_IMPROVED_DRAIN_SOUL_R1,
                SPELL_WARLOCK_IMPROVED_DRAIN_SOUL_PROC,
                SPELL_WARLOCK_CREATE_SOULSHARD,
                SPELL_WARLOCK_GLYPH_OF_DRAIN_SOUL_AURA,
                SPELL_WARLOCK_GLYPH_OF_DRAIN_SOUL_PROC
            });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        // Drain Soul's proc tries to happen each time the warlock lands a killing blow on a unit while channeling.
        // Make sure that dying unit is afflicted by the caster's Drain Soul debuff in order to avoid a false positive.

        Unit* caster = GetCaster();
        Unit* victim = eventInfo.GetProcTarget();

        if (caster && victim)
            return victim->GetAuraApplicationOfRankedSpell(SPELL_WARLOCK_DRAIN_SOUL_R1, caster->GetGUID()) != 0;

        return false;
    }


    void HandleTick(AuraEffect const* aurEff)
    {
        Unit* caster = GetCaster();
        Unit* target = GetTarget();
        if (caster && caster->GetTypeId() == TYPEID_PLAYER && caster->ToPlayer()->isHonorOrXPTarget(target))
        {
            if (roll_chance_i(20))
            {
                caster->CastSpell(caster, SPELL_WARLOCK_CREATE_SOULSHARD, aurEff);
                // Glyph of Drain Soul - chance to create an additional Soul Shard
                if (AuraEffect* aur = caster->GetAuraEffect(SPELL_WARLOCK_GLYPH_OF_DRAIN_SOUL_AURA, EFFECT_0))
                    if (roll_chance_i(aur->GetMiscValue()))
                        caster->CastSpell(caster, SPELL_WARLOCK_GLYPH_OF_DRAIN_SOUL_PROC, aur);
            }
        }
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_warl_drain_soul::CheckProc);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_drain_soul::HandleTick, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

// -18094 - Nightfall
//  56218 - Glyph of Corruption
class spell_warl_glyph_of_corruption_nightfall : public AuraScript
{
    PrepareAuraScript(spell_warl_glyph_of_corruption_nightfall);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return true;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        Unit* caster = eventInfo.GetActor();
        caster->CastSpell(caster, SPELL_WARLOCK_SHADOW_TRANCE, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_warl_glyph_of_corruption_nightfall::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// -755 - Health Funnel
class spell_warl_health_funnel : public AuraScript
{
    PrepareAuraScript(spell_warl_health_funnel);

    void ApplyEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        Unit* target = GetTarget();
        if (caster->HasAura(SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_R2))
            target->CastSpell(target, SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_BUFF_R2, true);
        else if (caster->HasAura(SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_R1))
            target->CastSpell(target, SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_BUFF_R1, true);
    }

    void RemoveEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();
        target->RemoveAurasDueToSpell(SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_BUFF_R1);
        target->RemoveAurasDueToSpell(SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_BUFF_R2);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_warl_health_funnel::RemoveEffect, EFFECT_0, SPELL_AURA_PERIODIC_HEAL, AURA_EFFECT_HANDLE_REAL);
        OnEffectApply += AuraEffectApplyFn(spell_warl_health_funnel::ApplyEffect, EFFECT_0, SPELL_AURA_PERIODIC_HEAL, AURA_EFFECT_HANDLE_REAL);
    }
};

// -1454 - Life Tap
class spell_warl_life_tap : public SpellScript
{
    PrepareSpellScript(spell_warl_life_tap);

    bool Load() override
    {
        return GetCaster()->GetTypeId() == TYPEID_PLAYER;
    }

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_LIFE_TAP_ENERGIZE, SPELL_WARLOCK_LIFE_TAP_ENERGIZE_2 });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Player* caster = GetCaster()->ToPlayer();
        if (Unit* target = GetHitUnit())
        {
            int32 spellEffect = GetEffectValue();
            int32 mana = int32(spellEffect + (caster->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_SHADOW) * 0.5f));

            // Shouldn't Appear in Combat Log
            target->ModifyHealth(-spellEffect);

            // Improved Life Tap mod
            if (AuraEffect const* aurEff = caster->GetDummyAuraEffect(SPELLFAMILY_WARLOCK, WARLOCK_ICON_ID_IMPROVED_LIFE_TAP, 0))
                AddPct(mana, aurEff->GetAmount());

            caster->CastCustomSpell(target, SPELL_WARLOCK_LIFE_TAP_ENERGIZE, &mana, nullptr, nullptr, false);

            // Mana Feed
            int32 manaFeedVal = 0;
            if (AuraEffect const* aurEff = caster->GetAuraEffect(SPELL_AURA_ADD_FLAT_MODIFIER, SPELLFAMILY_WARLOCK, WARLOCK_ICON_ID_MANA_FEED, 0))
                manaFeedVal = aurEff->GetAmount();

            if (manaFeedVal > 0)
            {
                ApplyPct(manaFeedVal, mana);
                caster->CastCustomSpell(caster, SPELL_WARLOCK_LIFE_TAP_ENERGIZE_2, &manaFeedVal, nullptr, nullptr, true, nullptr);
            }
        }
    }

    SpellCastResult CheckCast()
    {
        if ((int32(GetCaster()->GetHealth()) > int32(GetSpellInfo()->Effects[EFFECT_0].CalcValue())))
            return SPELL_CAST_OK;
        return SPELL_FAILED_FIZZLE;
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_warl_life_tap::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnCheckCast += SpellCheckCastFn(spell_warl_life_tap::CheckCast);
    }
};

// 18541 - Ritual of Doom Effect
class spell_warl_ritual_of_doom_effect : public SpellScript
{
    PrepareSpellScript(spell_warl_ritual_of_doom_effect);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        caster->CastSpell(caster, GetEffectValue(), true);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_warl_ritual_of_doom_effect::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 6358 - Seduction
class spell_warl_seduction : public SpellScript
{
    PrepareSpellScript(spell_warl_seduction);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_GLYPH_OF_SUCCUBUS });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* owner = GetCaster()->GetOwner();
        if (!owner || !owner->HasAura(SPELL_WARLOCK_GLYPH_OF_SUCCUBUS))
            return;

        Unit* target = GetHitUnit();
        target->RemoveAurasByType(SPELL_AURA_PERIODIC_DAMAGE, ObjectGuid::Empty, target->GetAura(32409)); // SW:D shall not be removed.
        target->RemoveAurasByType(SPELL_AURA_PERIODIC_DAMAGE_PERCENT);
        target->RemoveAurasByType(SPELL_AURA_PERIODIC_LEECH);
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_warl_seduction::HandleDummy, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

// -6229 - Shadow Ward
class spell_warl_shadow_ward : public AuraScript
{
    PrepareAuraScript(spell_warl_shadow_ward);

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

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_shadow_ward::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
    }
};

void AddSC_warlock_spell_scripts()
{
    RegisterSpellScript(spell_warl_curse_of_agony);
    RegisterSpellScript(spell_warl_banish);
    RegisterSpellScript(spell_warl_create_healthstone);
    RegisterSpellScript(spell_warl_curse_of_doom);
    RegisterSpellScript(spell_warl_drain_soul);
    RegisterSpellScript(spell_warl_health_funnel);
    RegisterSpellScript(spell_warl_glyph_of_corruption_nightfall);
    RegisterSpellScript(spell_warl_life_tap);
    RegisterSpellScript(spell_warl_ritual_of_doom_effect);
    RegisterSpellScript(spell_warl_seduction);
    RegisterSpellScript(spell_warl_shadow_ward);
}
