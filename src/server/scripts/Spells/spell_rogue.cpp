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
  * Scripts for spells with SPELLFAMILY_ROGUE and SPELLFAMILY_GENERIC spells used by rogue players.
  * Ordered alphabetically using scriptname.
  * Scriptnames of files in this file should be prefixed with "spell_rog_".
  */

#include "ScriptMgr.h"
#include "Containers.h"
#include "DBCStores.h"
#include "Item.h"
#include "Log.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "SpellAuraEffects.h"
#include "SpellMgr.h"
#include "SpellScript.h"

enum RogueSpells
{
    SPELL_ROGUE_BLADE_FLURRY_EXTRA_ATTACK = 22482,
    SPELL_ROGUE_CHEAT_DEATH_COOLDOWN = 31231,
    SPELL_ROGUE_GLYPH_OF_PREPARATION = 56819,
    SPELL_ROGUE_KILLING_SPREE = 51690,
    SPELL_ROGUE_KILLING_SPREE_TELEPORT = 57840,
    SPELL_ROGUE_KILLING_SPREE_WEAPON_DMG = 57841,
    SPELL_ROGUE_KILLING_SPREE_DMG_BUFF = 61851,
    SPELL_ROGUE_PREY_ON_THE_WEAK = 58670,
    SPELL_ROGUE_SHIV_TRIGGERED = 5940,
    SPELL_ROGUE_TRICKS_OF_THE_TRADE = 57934,
    SPELL_ROGUE_TRICKS_OF_THE_TRADE_DMG_BOOST = 57933,
    SPELL_ROGUE_TRICKS_OF_THE_TRADE_PROC = 59628,
    SPELL_ROGUE_HONOR_AMONG_THIEVES = 51698,
    SPELL_ROGUE_HONOR_AMONG_THIEVES_PROC = 52916,
    SPELL_ROGUE_HONOR_AMONG_THIEVES_2 = 51699,
    SPELL_ROGUE_T10_2P_BONUS = 70804,
    SPELL_ROGUE_GLYPH_OF_BACKSTAB_TRIGGER = 63975,
    SPELL_ROGUE_QUICK_RECOVERY_ENERGY = 31663,
    SPELL_ROGUE_CRIPPLING_POISON = 3409,
    SPELL_ROGUE_MASTER_OF_SUBTLETY_BUFF = 31665,
    SPELL_ROGUE_OVERKILL_BUFF = 58427,
    SPELL_ROGUE_STEALTH = 1784,
    SPELL_ROGUE_IMPROVED_SAP = 14095
};

// 13877, 33735, (check 51211, 65956) - Blade Flurry
class spell_rog_blade_flurry : public AuraScript
{
    PrepareAuraScript(spell_rog_blade_flurry);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_ROGUE_BLADE_FLURRY_EXTRA_ATTACK });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        _procTarget = eventInfo.GetActor()->SelectNearbyTarget(eventInfo.GetProcTarget());
        return _procTarget != nullptr;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        if (DamageInfo* damageInfo = eventInfo.GetDamageInfo())
        {
            GetTarget()->CastCustomSpell(SPELL_ROGUE_BLADE_FLURRY_EXTRA_ATTACK, SPELLVALUE_BASE_POINT0, damageInfo->GetDamage());
        }
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_rog_blade_flurry::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_rog_blade_flurry::HandleProc, EFFECT_0, SPELL_AURA_MOD_MELEE_HASTE);
    }

    Unit* _procTarget = nullptr;
};

// -2818 - Deadly Poison
class spell_rog_deadly_poison : public SpellScript
{
    PrepareSpellScript(spell_rog_deadly_poison);

    bool Load() override
    {
        _stackAmount = 0;
        // at this point CastItem must already be initialized
        return GetCaster()->GetTypeId() == TYPEID_PLAYER && GetCastItem();
    }

    void HandleBeforeHit(SpellMissInfo missInfo)
    {
        if (missInfo != SPELL_MISS_NONE)
        {
            return;
        }

        if (Unit* target = GetHitUnit())
            // Deadly Poison
            if (AuraEffect const* aurEff = target->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_ROGUE, 0x10000, 0x80000, 0, GetCaster()->GetGUID()))
                _stackAmount = aurEff->GetBase()->GetStackAmount();
    }

    void HandleAfterHit()
    {
        if (_stackAmount < 5)
            return;

        Player* player = GetCaster()->ToPlayer();

        if (Unit* target = GetHitUnit())
        {
            Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);

            if (item == GetCastItem())
                item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);

            if (!item)
                return;

            // item combat enchantments
            for (uint8 slot = 0; slot < MAX_ENCHANTMENT_SLOT; ++slot)
            {
                SpellItemEnchantmentEntry const* enchant = sSpellItemEnchantmentStore.LookupEntry(item->GetEnchantmentId(EnchantmentSlot(slot)));
                if (!enchant)
                    continue;

                for (uint8 s = 0; s < 3; ++s)
                {
                    if (enchant->type[s] != ITEM_ENCHANTMENT_TYPE_COMBAT_SPELL)
                        continue;

                    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(enchant->spellid[s]);
                    if (!spellInfo)
                    {
                        LOG_ERROR("misc", "Player::CastItemCombatSpell Enchant {}, player (Name: {}, {}) cast unknown spell {}",
                            enchant->ID, player->GetName(), player->GetGUID().ToString(), enchant->spellid[s]);
                        continue;
                    }

                    // Proc only rogue poisons
                    if (spellInfo->SpellFamilyName != SPELLFAMILY_ROGUE || spellInfo->Dispel != DISPEL_POISON)
                        continue;

                    // Do not reproc deadly
                    if (spellInfo->SpellFamilyFlags.IsEqual(0x10000, 0x80000, 0))
                        continue;

                    if (spellInfo->IsPositive())
                        player->CastSpell(player, enchant->spellid[s], true, item);
                    else
                        player->CastSpell(target, enchant->spellid[s], true, item);
                }
            }
        }
    }

    void Register() override
    {
        BeforeHit += BeforeSpellHitFn(spell_rog_deadly_poison::HandleBeforeHit);
        AfterHit += SpellHitFn(spell_rog_deadly_poison::HandleAfterHit);
    }

    uint8 _stackAmount;
};


// 14185 - Preparation
class spell_rog_preparation : public SpellScript
{
    PrepareSpellScript(spell_rog_preparation);

    bool Load() override
    {
        return GetCaster()->GetTypeId() == TYPEID_PLAYER;
    }

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return true;
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Player* caster = GetCaster()->ToPlayer();
        //immediately finishes the cooldown on certain Rogue abilities

        bool hasGlyph = caster->HasAura(SPELL_ROGUE_GLYPH_OF_PREPARATION);
        PlayerSpellMap const& spellMap = caster->GetSpellMap();
        for (PlayerSpellMap::const_iterator itr = spellMap.begin(); itr != spellMap.end(); ++itr)
        {
            SpellInfo const* spellInfo = sSpellMgr->AssertSpellInfo(itr->first);
            if (spellInfo->SpellFamilyName == SPELLFAMILY_ROGUE)
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
        OnEffectHitTarget += SpellEffectFn(spell_rog_preparation::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// -1943 - Rupture
class spell_rog_rupture : public SpellScriptLoader
{
public:
    static char constexpr const ScriptName[] = "spell_rog_rupture";

    spell_rog_rupture() : SpellScriptLoader(ScriptName) { }

    class spell_rog_rupture_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_rog_rupture_AuraScript);

        bool Load() override
        {
            Unit* caster = GetCaster();
            BonusDuration = 0;
            return caster && caster->GetTypeId() == TYPEID_PLAYER;
        }

        void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
        {
            if (Unit* caster = GetCaster())
            {
                canBeRecalculated = false;

                // Dmg/tick = $AP*min(0.01*$cp, 0.03) [Like Rip: only the first three CP increase the contribution from AP]
                uint8 cp = caster->ToPlayer()->GetComboPoints();
                if (cp > 3)
                    cp = 3;

                amount += int32(caster->GetTotalAttackPowerValue(BASE_ATTACK) * (cp / 100));
            }
        }

        void ResetDuration(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            BonusDuration = 0;
        }

        void Register() override
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_rog_rupture_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            AfterEffectApply += AuraEffectApplyFn(spell_rog_rupture_AuraScript::ResetDuration, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAPPLY);
        }

    public:
        // For Glyph of Backstab use
        uint32 BonusDuration;
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_rog_rupture_AuraScript();
    }
};
char constexpr const spell_rog_rupture::ScriptName[];

// -13983 - Setup
class spell_rog_setup : public AuraScript
{
    PrepareAuraScript(spell_rog_setup);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (Player* target = GetTarget()->ToPlayer())
            if (eventInfo.GetActor() == target->GetSelectedUnit())
                return true;

        return false;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_rog_setup::CheckProc);
    }
};

// -11327 - Vanish
class spell_rog_vanish : public AuraScript
{
    PrepareAuraScript(spell_rog_vanish);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_ROGUE_STEALTH });
    }

    void ApplyStealth(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* unitTarget = GetTarget();
        unitTarget->RemoveAurasByType(SPELL_AURA_MOD_STALKED);

        // See if we already are stealthed. If so, we're done.
        if (unitTarget->HasAura(SPELL_ROGUE_STEALTH))
            return;

        // Reset cooldown on stealth if needed
        if (GetCaster()->IsPlayer() && GetCaster()->HasSpellCooldown(SPELL_ROGUE_STEALTH))
            GetCaster()->ToPlayer()->RemoveSpellCooldown(SPELL_ROGUE_STEALTH);

        GetCaster()->CastSpell(GetCaster(), SPELL_ROGUE_STEALTH, true);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_rog_vanish::ApplyStealth, EFFECT_1, SPELL_AURA_MOD_STEALTH, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
    }
};

//14076, 14094, 14095 Improved Sap
class spell_rog_imp_sap : public AuraScript
{
    PrepareAuraScript(spell_rog_imp_sap);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ SPELL_ROGUE_IMPROVED_SAP });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        Unit* unitTarget = GetTarget();
        unitTarget->RemoveAurasByType(SPELL_AURA_MOD_STALKED);

        // See if we already are stealthed. If so, we're done.
        if (unitTarget->HasAura(SPELL_ROGUE_STEALTH))
            return;

        // Reset cooldown on stealth if needed
        if (GetCaster()->IsPlayer() && GetCaster()->HasSpellCooldown(SPELL_ROGUE_STEALTH))
            GetCaster()->ToPlayer()->RemoveSpellCooldown(SPELL_ROGUE_STEALTH);

        GetCaster()->CastSpell(GetCaster(), SPELL_ROGUE_STEALTH, true);
    }


    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_rog_imp_sap::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

void AddSC_rogue_spell_scripts()
{
    RegisterSpellScript(spell_rog_blade_flurry);
    RegisterSpellScript(spell_rog_deadly_poison);
    RegisterSpellScript(spell_rog_preparation);
    new spell_rog_rupture();
    RegisterSpellScript(spell_rog_setup);
    RegisterSpellScript(spell_rog_vanish);
    RegisterSpellScript(spell_rog_imp_sap);
}
