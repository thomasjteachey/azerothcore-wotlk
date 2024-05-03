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
  * Scripts for spells with SPELLFAMILY_SHAMAN and SPELLFAMILY_GENERIC spells used by shaman players.
  * Ordered alphabetically using scriptname.
  * Scriptnames of files in this file should be prefixed with "spell_sha_".
  */

#include "ScriptMgr.h"
#include "GridNotifiers.h"
#include "Item.h"
#include "ObjectAccessor.h"
#include "Map.h"
#include "Player.h"
#include "SpellAuraEffects.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "Unit.h"

enum ShamanSpells
{
    SPELL_SHAMAN_ANCESTRAL_AWAKENING_DUMMY = 52759,
    SPELL_SHAMAN_ANCESTRAL_AWAKENING_PROC = 52752,
    SPELL_SHAMAN_BIND_SIGHT = 6277,
    SPELL_SHAMAN_CLEANSING_TOTEM_EFFECT = 52025,
    SPELL_SHAMAN_EARTH_SHIELD_HEAL = 379,
    SPELL_SHAMAN_ELEMENTAL_MASTERY = 16166,
    SPELL_SHAMAN_ELEMENTAL_OATH = 51466,
    SPELL_SHAMAN_EXHAUSTION = 57723,
    SPELL_SHAMAN_FIRE_NOVA_R1 = 1535,
    SPELL_SHAMAN_FIRE_NOVA_TRIGGERED_R1 = 8349,
    SPELL_SHAMAN_GLYPH_OF_EARTH_SHIELD = 63279,
    SPELL_SHAMAN_GLYPH_OF_HEALING_STREAM_TOTEM = 55456,
    SPELL_SHAMAN_GLYPH_OF_MANA_TIDE = 55441,
    SPELL_SHAMAN_GLYPH_OF_THUNDERSTORM = 62132,
    SPELL_SHAMAN_ITEM_LIGHTNING_SHIELD = 23552,
    SPELL_SHAMAN_ITEM_LIGHTNING_SHIELD_DAMAGE = 27635,
    SPELL_SHAMAN_ITEM_MANA_SURGE = 23571,
    SPELL_SHAMAN_LAVA_FLOWS_R1 = 51480,
    SPELL_SHAMAN_LAVA_FLOWS_TRIGGERED_R1 = 64694,
    SPELL_SHAMAN_LIGHTNING_SHIELD_R1 = 26364,
    SPELL_SHAMAN_MANA_SPRING_TOTEM_ENERGIZE = 52032,
    SPELL_SHAMAN_MANA_TIDE_TOTEM = 39609,
    SPELL_SHAMAN_NATURE_GUARDIAN = 31616,
    SPELL_SHAMAN_NATURE_GUARDIAN_THREAT = 39301, // Serverside
    SPELL_SHAMAN_SATED = 57724,
    SPELL_SHAMAN_STORM_EARTH_AND_FIRE = 51483,
    SPELL_SHAMAN_TOTEM_EARTHBIND_EARTHGRAB = 64695,
    SPELL_SHAMAN_TOTEM_EARTHBIND_TOTEM = 6474,
    SPELL_SHAMAN_TOTEM_EARTHEN_POWER = 59566,
    SPELL_SHAMAN_TOTEM_HEALING_STREAM_HEAL = 52042,
    SPELL_SHAMAN_TOTEMIC_MASTERY = 38437,
    SPELL_SHAMAN_TIDAL_FORCE_CRIT = 55166,
    SPELL_SHAMAN_TOTEMIC_POWER_MP5 = 28824,
    SPELL_SHAMAN_TOTEMIC_POWER_SPELL_POWER = 28825,
    SPELL_SHAMAN_TOTEMIC_POWER_ATTACK_POWER = 28826,
    SPELL_SHAMAN_TOTEMIC_POWER_ARMOR = 28827,
    SPELL_SHAMAN_WINDFURY_WEAPON_R1 = 8232,
    SPELL_SHAMAN_WINDFURY_ATTACK_MH = 25504,
    SPELL_SHAMAN_WINDFURY_ATTACK_OH = 33750,
    SPELL_SHAMAN_ENERGY_SURGE = 40465,
    SPELL_SHAMAN_POWER_SURGE = 40466,
    SPELL_SHAMAN_GLYPH_OF_HEALING_WAVE_HEAL = 55533,
    SPELL_SHAMAN_SPIRIT_HUNT_HEAL = 58879,
    SPELL_SHAMAN_ELECTRIFIED = 64930,
    SPELL_SHAMAN_LAVA_BURST_BONUS_DAMAGE = 71824,
    SPELL_SHAMAN_CHAINED_HEAL = 70809,
    SPELL_SHAMAN_TOTEM_OF_WRATH_SPELL_POWER = 63283,
    SPELL_SHAMAN_FREEZE = 63685,
    SPELL_SHAMAN_FLAMETONGUE_ATTACK = 10444,
    SPELL_SHAMAN_LIGHTNING_BOLT_OVERLOAD_R1 = 45284,
    SPELL_SHAMAN_CHAIN_LIGHTNING_OVERLOAD_R1 = 45297,
    SPELL_SHAMAN_LIGHTNING_SHIELD_DAMAGE_R1 = 26364,
    SPELL_SHAMAN_SHAMANISTIC_RAGE_PROC = 30824,
    SPELL_SHAMAN_STONECLAW_TOTEM = 55277,
    SPELL_SHAMAN_GLYPH_OF_STONECLAW_TOTEM = 63298,
    SPELL_SHAMAN_MAELSTROM_POWER = 70831,
    SPELL_SHAMAN_T10_ENHANCEMENT_4P_BONUS = 70832,
    SPELL_SHAMAN_BLESSING_OF_THE_ETERNALS_R1 = 51554,
};

enum ShamanSpellIcons
{
    SHAMAN_ICON_ID_RESTORATIVE_TOTEMS = 338,
    SHAMAN_ICON_ID_SHAMAN_LAVA_FLOW = 3087,
    SHAMAN_ICON_ID_TOTEM_OF_WRATH = 2019
};

// -1064 - Chain Heal
class spell_sha_chain_heal : public SpellScript
{
    PrepareSpellScript(spell_sha_chain_heal);

public:
    spell_sha_chain_heal()
    {
        firstHeal = true;
        riptide = false;
    }

private:
    void HandleHeal(SpellEffIndex /*effIndex*/)
    {
        if (firstHeal)
        {
            // Check if the target has Riptide
            if (AuraEffect* aurEff = GetHitUnit()->GetAuraEffect(SPELL_AURA_PERIODIC_HEAL, SPELLFAMILY_SHAMAN, 0, 0, 0x10, GetCaster()->GetGUID()))
            {
                riptide = true;
                // Consume it
                GetHitUnit()->RemoveAura(aurEff->GetBase());
            }
            firstHeal = false;
        }
        // Riptide increases the Chain Heal effect by 25%
        if (riptide)
            SetHitHeal(GetHitHeal() * 1.25f);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sha_chain_heal::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
    }

    bool firstHeal;
    bool riptide;
};

// 8171 - Cleansing Totem (Pulse)
class spell_sha_cleansing_totem_pulse : public SpellScript
{
    PrepareSpellScript(spell_sha_cleansing_totem_pulse);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_CLEANSING_TOTEM_EFFECT });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        int32 bp = 1;
        if (GetCaster() && GetHitUnit() && GetOriginalCaster())
            GetCaster()->CastCustomSpell(GetHitUnit(), SPELL_SHAMAN_CLEANSING_TOTEM_EFFECT, nullptr, &bp, nullptr, true, nullptr, nullptr, GetOriginalCaster()->GetGUID());
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sha_cleansing_totem_pulse::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 16246 - Clearcasting
class spell_sha_clearcasting : public AuraScript
{
    PrepareAuraScript(spell_sha_clearcasting);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_ELEMENTAL_OATH });
    }

    // Elemental Oath bonus
    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        Unit const* owner = GetUnitOwner();
        if (Aura const* aura = owner->GetAuraOfRankedSpell(SPELL_SHAMAN_ELEMENTAL_OATH, owner->GetGUID()))
            amount = aura->GetSpellInfo()->GetEffect(EFFECT_1).CalcValue();
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_clearcasting::CalculateAmount, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
    }
};


// -1535 - Fire Nova
class spell_sha_fire_nova : public SpellScript
{
    PrepareSpellScript(spell_sha_fire_nova);

    bool Validate(SpellInfo const* spellInfo) override
    {
        SpellInfo const* firstRankSpellInfo = sSpellMgr->GetSpellInfo(SPELL_SHAMAN_FIRE_NOVA_R1);
        if (!firstRankSpellInfo || !spellInfo->IsRankOf(firstRankSpellInfo))
            return false;

        uint8 rank = spellInfo->GetRank();
        if (!sSpellMgr->GetSpellWithRank(SPELL_SHAMAN_FIRE_NOVA_TRIGGERED_R1, rank, true))
            return false;
        return true;
    }

    SpellCastResult CheckFireTotem()
    {
        Unit* caster = GetCaster();
        // fire totem
        if (Creature* totem = caster->GetMap()->GetCreature(caster->m_SummonSlot[1]))
        {
            if (!caster->IsWithinDistInMap(totem, caster->GetSpellMaxRangeForTarget(totem, GetSpellInfo())))
                return SPELL_FAILED_OUT_OF_RANGE;
            return SPELL_CAST_OK;
        }
        else
        {
            SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_MUST_HAVE_FIRE_TOTEM);
            return SPELL_FAILED_CUSTOM_ERROR;
        }
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        if (Creature* totem = caster->GetMap()->GetCreature(caster->m_SummonSlot[1]))
        {
            uint8 rank = GetSpellInfo()->GetRank();
            if (totem->IsTotem())
                caster->CastSpell(totem, sSpellMgr->GetSpellWithRank(SPELL_SHAMAN_FIRE_NOVA_TRIGGERED_R1, rank), true);
        }
    }

    void Register() override
    {
        //OnCheckCast += SpellCheckCastFn(spell_sha_fire_nova::CheckFireTotem);
        //OnEffectHitTarget += SpellEffectFn(spell_sha_fire_nova::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// -8050 - Flame Shock
class spell_sha_flame_shock : public AuraScript
{
    PrepareAuraScript(spell_sha_flame_shock);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_LAVA_FLOWS_R1, SPELL_SHAMAN_LAVA_FLOWS_TRIGGERED_R1 });
    }

    void HandleDispel(DispelInfo* /*dispelInfo*/)
    {
        if (Unit* caster = GetCaster())
            // Lava Flows
            if (AuraEffect const* aurEff = caster->GetDummyAuraEffect(SPELLFAMILY_SHAMAN, SHAMAN_ICON_ID_SHAMAN_LAVA_FLOW, EFFECT_0))
            {
                if (SpellInfo const* firstRankSpellInfo = sSpellMgr->GetSpellInfo(SPELL_SHAMAN_LAVA_FLOWS_R1))
                    if (!aurEff->GetSpellInfo()->IsRankOf(firstRankSpellInfo))
                        return;

                uint8 rank = aurEff->GetSpellInfo()->GetRank();
                caster->CastSpell(caster, sSpellMgr->GetSpellWithRank(SPELL_SHAMAN_LAVA_FLOWS_TRIGGERED_R1, rank), true);
            }
    }

    void Register() override
    {
        AfterDispel += AuraDispelFn(spell_sha_flame_shock::HandleDispel);
    }
};

// -10400 - Flametongue Weapon (Passive)
class spell_sha_flametongue_weapon : public AuraScript
{
    PrepareAuraScript(spell_sha_flametongue_weapon);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_FLAMETONGUE_ATTACK });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        Player* player = eventInfo.GetActor()->ToPlayer();
        if (!player)
            return false;

        Item* item = player->GetItemByGuid(GetAura()->GetCastItemGUID());
        if (!item || !item->IsEquipped())
            return false;

        WeaponAttackType attType = (WeaponAttackType)Player::GetAttackBySlot(item->GetSlot());
        if (attType != BASE_ATTACK && attType != OFF_ATTACK)
            return false;

        if (((attType == BASE_ATTACK) && !(eventInfo.GetTypeMask() & PROC_FLAG_DONE_MAINHAND_ATTACK)) ||
            ((attType == OFF_ATTACK) && !(eventInfo.GetTypeMask() & PROC_FLAG_DONE_OFFHAND_ATTACK)))
            return false;

        return true;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        Player* player = eventInfo.GetActor()->ToPlayer();
        Unit* target = eventInfo.GetProcTarget();
        WeaponAttackType attType = BASE_ATTACK;
        if (eventInfo.GetTypeMask() & PROC_FLAG_DONE_OFFHAND_ATTACK)
            attType = OFF_ATTACK;

        Item* item = ASSERT_NOTNULL(player->GetWeaponForAttack(attType));

        float basePoints = aurEff->GetSpellInfo()->Effects[EFFECT_0].CalcValue();

        // Flametongue max damage is normalized based on a 4.0 speed weapon
        // Tooltip says max damage = BasePoints / 25, so BasePoints / 25 / 4 to get base damage per 1.0s AS
        float attackSpeed = player->GetAttackTime(attType) / 1000.f;
        float fireDamage = basePoints / 100.0f;
        fireDamage *= attackSpeed;

        // clip value between (BasePoints / 77) and (BasePoints / 25) as the tooltip indicates
        RoundToInterval(fireDamage, basePoints / 77.0f, basePoints / 25.0f);

        // Calculate Spell Power scaling
        float spellPowerBonus = player->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_FIRE);
        spellPowerBonus += target->GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_DAMAGE_TAKEN, SPELL_SCHOOL_MASK_FIRE);

        // calculate penalty from passive aura as is the one with level
        float const factorMod = player->CalculateLevelPenalty(GetSpellInfo());

        float const spCoeff = 0.03811f;
        spellPowerBonus *= spCoeff * attackSpeed * factorMod;

        // All done, now proc damage
        int32 bp0 = fireDamage + spellPowerBonus;
        player->ToUnit()->CastCustomSpell(target, SPELL_SHAMAN_FLAMETONGUE_ATTACK, &bp0, nullptr, nullptr, true, item, nullptr, player->GetGUID());
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_flametongue_weapon::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_sha_flametongue_weapon::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 23551 - Lightning Shield T2 Bonus
class spell_sha_item_lightning_shield : public AuraScript
{
    PrepareAuraScript(spell_sha_item_lightning_shield);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_ITEM_LIGHTNING_SHIELD });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        GetTarget()->CastSpell(eventInfo.GetProcTarget(), SPELL_SHAMAN_ITEM_LIGHTNING_SHIELD, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_item_lightning_shield::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 23552 - Lightning Shield T2 Bonus
class spell_sha_item_lightning_shield_trigger : public AuraScript
{
    PrepareAuraScript(spell_sha_item_lightning_shield_trigger);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_ITEM_LIGHTNING_SHIELD_DAMAGE });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        GetTarget()->CastSpell(GetTarget(), SPELL_SHAMAN_ITEM_LIGHTNING_SHIELD_DAMAGE, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_item_lightning_shield_trigger::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 23572 - Mana Surge
class spell_sha_item_mana_surge : public AuraScript
{
    PrepareAuraScript(spell_sha_item_mana_surge);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_ITEM_MANA_SURGE });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        if (!spellInfo)
            return;

        int32 mana = spellInfo->CalcPowerCost(GetTarget(), eventInfo.GetSchoolMask());
        GetTarget()->CastCustomSpell(SPELL_SHAMAN_ITEM_MANA_SURGE, SPELLVALUE_BASE_POINT0, CalculatePct(mana, 35));
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_item_mana_surge::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// -324 - Lightning Shield
class spell_sha_lightning_shield : public AuraScript
{
    PrepareAuraScript(spell_sha_lightning_shield);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_LIGHTNING_SHIELD_R1 });
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
        uint32 triggerSpell = sSpellMgr->GetSpellWithRank(SPELL_SHAMAN_LIGHTNING_SHIELD_R1, aurEff->GetSpellInfo()->GetRank());

        eventInfo.GetActionTarget()->CastSpell(eventInfo.GetActor(), triggerSpell, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_lightning_shield::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_sha_lightning_shield::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 16191 - Mana Tide
class spell_sha_mana_tide : public AuraScript
{
    PrepareAuraScript(spell_sha_mana_tide);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ spellInfo->GetEffect(EFFECT_0).TriggerSpell });
    }

    void PeriodicTick(AuraEffect const* aurEff)
    {
        PreventDefaultAction();
        GetTarget()->CastCustomSpell(aurEff->GetSpellInfo()->GetEffect(EFFECT_0).TriggerSpell, SPELLVALUE_BASE_POINT0, aurEff->GetAmount());
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_sha_mana_tide::PeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// 6495 - Sentry Totem
class spell_sha_sentry_totem : public AuraScript
{
    PrepareAuraScript(spell_sha_sentry_totem);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_BIND_SIGHT });
    }

    void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* caster = GetCaster())
            if (caster->GetTypeId() == TYPEID_PLAYER)
                caster->ToPlayer()->StopCastingBindSight();
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_sha_sentry_totem::AfterRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};


// 28823 - Totemic Power
class spell_sha_t3_6p_bonus : public AuraScript
{
    PrepareAuraScript(spell_sha_t3_6p_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_SHAMAN_TOTEMIC_POWER_ARMOR,
                SPELL_SHAMAN_TOTEMIC_POWER_ATTACK_POWER,
                SPELL_SHAMAN_TOTEMIC_POWER_SPELL_POWER,
                SPELL_SHAMAN_TOTEMIC_POWER_MP5
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
            spellId = SPELL_SHAMAN_TOTEMIC_POWER_MP5;
            break;
        case CLASS_MAGE:
        case CLASS_WARLOCK:
            spellId = SPELL_SHAMAN_TOTEMIC_POWER_SPELL_POWER;
            break;
        case CLASS_HUNTER:
        case CLASS_ROGUE:
            spellId = SPELL_SHAMAN_TOTEMIC_POWER_ATTACK_POWER;
            break;
        case CLASS_WARRIOR:
            spellId = SPELL_SHAMAN_TOTEMIC_POWER_ARMOR;
            break;
        default:
            return;
        }

        caster->CastSpell(target, spellId, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_t3_6p_bonus::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 28820 - Lightning Shield
class spell_sha_t3_8p_bonus : public AuraScript
{
    PrepareAuraScript(spell_sha_t3_8p_bonus);

    void PeriodicTick(AuraEffect const* /*aurEff*/)
    {
        PreventDefaultAction();

        // Need remove self if Lightning Shield not active
        if (!GetTarget()->GetAuraEffect(SPELL_AURA_PROC_TRIGGER_SPELL, SPELLFAMILY_SHAMAN, 0x400, 0, 0))
            Remove();
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_sha_t3_8p_bonus::PeriodicTick, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// 33757 - Windfury Weapon (Passive)
class spell_sha_windfury_weapon : public AuraScript
{
    PrepareAuraScript(spell_sha_windfury_weapon);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_SHAMAN_WINDFURY_WEAPON_R1,
                SPELL_SHAMAN_WINDFURY_ATTACK_MH,
                SPELL_SHAMAN_WINDFURY_ATTACK_OH
            });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        Player* player = eventInfo.GetActor()->ToPlayer();
        if (!player)
            return false;

        Item* item = player->GetItemByGuid(GetAura()->GetCastItemGUID());
        if (!item || !item->IsEquipped())
            return false;

        WeaponAttackType attType = static_cast<WeaponAttackType>(player->GetAttackBySlot(item->GetSlot()));
        if (attType != BASE_ATTACK && attType != OFF_ATTACK)
            return false;

        if (((attType == BASE_ATTACK) && !(eventInfo.GetTypeMask() & PROC_FLAG_DONE_MAINHAND_ATTACK)) ||
            ((attType == OFF_ATTACK) && !(eventInfo.GetTypeMask() & PROC_FLAG_DONE_OFFHAND_ATTACK)))
            return false;

        return true;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        Player* player = eventInfo.GetActor()->ToPlayer();

        uint32 spellId = 0;
        WeaponAttackType attType = BASE_ATTACK;
        if (eventInfo.GetTypeMask() & PROC_FLAG_DONE_MAINHAND_ATTACK)
            spellId = SPELL_SHAMAN_WINDFURY_ATTACK_MH;

        if (eventInfo.GetTypeMask() & PROC_FLAG_DONE_OFFHAND_ATTACK)
        {
            spellId = SPELL_SHAMAN_WINDFURY_ATTACK_OH;
            attType = OFF_ATTACK;
        }
        //player->AddExtraAttacks(2);

        Item* item = ASSERT_NOTNULL(player->GetWeaponForAttack(attType));

        int32 enchantId = static_cast<int32>(item->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT));
        int32 extraAttackPower = 0;
        SpellInfo const* spellInfo = sSpellMgr->AssertSpellInfo(SPELL_SHAMAN_WINDFURY_WEAPON_R1);
        while (spellInfo)
        {
            if (spellInfo->GetEffect(EFFECT_0).MiscValue == enchantId)
            {
                extraAttackPower = spellInfo->GetEffect(EFFECT_1).CalcValue(player);
                break;
            }
            spellInfo = spellInfo->GetNextRankSpell();
        }

        if (!extraAttackPower)
            return;

        // Value gained from additional AP
        int32 amount = static_cast<int32>(extraAttackPower / 14.f * player->GetAttackTime(attType) / 1000.f);
        // Attack twice
        for (uint8 i = 0; i < 2; ++i)
            player->CastCustomSpell(spellId, SPELLVALUE_BASE_POINT0, amount, eventInfo.GetProcTarget());

    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_windfury_weapon::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_sha_windfury_weapon::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

//healing wave
class spell_sha_healing_wave : public SpellScript
{
    PrepareSpellScript(spell_sha_healing_wave);

    void HandleHeal(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        if (Unit* unitTarget = GetHitUnit())
        {
            //get healing way aura
            AuraApplication* aa = unitTarget->GetAuraApplication(29203);
            if (aa)
            {
                float percentHeal = 1 + (aa->GetBase()->GetEffect(0)->GetAmount() * .01f);
                SetHitHeal(GetHitHeal() * percentHeal);
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sha_healing_wave::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
    }
};

void AddSC_shaman_spell_scripts()
{
    RegisterSpellScript(spell_sha_chain_heal);
    RegisterSpellScript(spell_sha_cleansing_totem_pulse);
    RegisterSpellScript(spell_sha_clearcasting);
    RegisterSpellScript(spell_sha_fire_nova);
    RegisterSpellScript(spell_sha_flame_shock);
    RegisterSpellScript(spell_sha_flametongue_weapon);
    RegisterSpellScript(spell_sha_item_lightning_shield);
    RegisterSpellScript(spell_sha_item_lightning_shield_trigger);
    RegisterSpellScript(spell_sha_item_mana_surge);
    RegisterSpellScript(spell_sha_lightning_shield);
    RegisterSpellScript(spell_sha_mana_tide);
    RegisterSpellScript(spell_sha_sentry_totem);
    RegisterSpellScript(spell_sha_t3_6p_bonus);
    RegisterSpellScript(spell_sha_t3_8p_bonus);
    RegisterSpellScript(spell_sha_windfury_weapon);
    RegisterSpellScript(spell_sha_healing_wave);
}
