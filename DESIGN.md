# Project Design — Full Vision

> This document captures the complete long-term design for the hack.
> CLAUDE.md describes only what is actively being built (currently v1).
> When a feature graduates from "planned" to "in development", move its
> spec from this file into CLAUDE.md.

## Core Concept

A challenge ROM hack of Pokémon Platinum (US, rev 1) where:
- The player never catches Pokémon — every team member is acquired via a "roll" mechanic.
- Death is permanent; fainted Pokémon are removed from play forever.
- Combat resource management is restricted to held items only.
- The economy is reshaped around free Rare Candies and trainer money.

## Pokémon Pool (used by all rolls)

- Source: full National Dex (493 species).
- Rules: base-form only OR single-stage species. No fully-evolved forms in the pool.
- Excluded: legendaries, mythicals, shinies (shiny encounters disabled entirely for v1; revisit later).
- Pool size: roughly 90–120 species after exclusions (to be confirmed at implementation).
- Once acquired, normal evolution is allowed via standard gameplay (level, stone, trade, etc.).

## Roll Mechanic (universal primitive)

A "roll" presents the player with **one pool of 6 species** drawn from the Pokémon Pool.

- 2 species are banned (random selection in v1; stat-based ban in a future version).
- The player picks 1 of the remaining 4.
- The chosen species is given to the player at an appropriate level (TBD: scale to current badge count? fixed level? — design decision needed before implementation).
- Bans are per-roll only; banned species can reappear in future rolls.
- Repeats across rolls are allowed.
- If a Pokemon is already in the active team it can't appear in a roll

### When a roll happens

1. **Initial draft (game start, replaces starter briefcase):** 7 sequential rolls, 7 different pools. Player builds their starting party of 7 from these.
   *Note: party size of 7 exceeds the vanilla cap of 6. Implementation must either expand party storage or use a hybrid (6 active + 1 reserve). To be decided.*
2. **Post-gym roll:** After each of the 8 gyms, player gets one roll. Player selects a target party slot **first** (empty or occupied — including dead/locked slots), then rolls. Roll cannot be rejected once started.
3. **In-game trade:** If the player has the species the NPC wants, they may initiate the trade. The given species is consumed, and the player receives a roll instead of the NPC's intended Pokémon. Net result: -1 mon, +1 rolled mon.
4. **Forced gifts and event captures** (Eevee from Bebe, Riolu egg, Happiny egg, Porygon, Giratina capture, etc.): the event still completes for story purposes, but the gifted/captured species is replaced by a rolled species.

## Permadeath

- When a Pokémon's HP reaches 0 in battle, instead of applying STATUS_FAINTED:
  - Mark the Pokémon as permanently dead.
  - Move its full struct to a "locked" PC box.
  - The Pokémon cannot be revived, withdrawn, or used in any way.
  - Dead Pokémon do not gain XP from subsequent battles.
- v1: locked box is hidden from the PC UI (simpler).
- Future: locked box is visible as a read-only memorial in the PC UI.

## Catching & Wild Encounters

- Poké Balls (all variants) are removed from the game entirely. Not in shops, not as field items, not from NPCs.
- Wild battles still occur normally (grass/cave/surf/fish encounters trigger battles for XP).
- The "Catch" action in battle is unreachable because the bag is disabled in combat (see below).

## Combat Bag Restriction

- The player's bag menu is disabled inside battles.
- Held items continue to function normally (Berries auto-trigger, Choice items, etc.).
- AI trainers retain full bag access (gym leaders using Full Restores etc. — vanilla behavior preserved).

## Economy

- Every Poké Mart sells Rare Candies for 0 ¥ (free).
- Rare Candies cannot be sold (greyed out / unsellable in shop sell menu).
- Other items unchanged. Player still earns money from trainer battles.

## HM Overhaul

- HMs are replaced with Key Items (e.g., "Machete" replaces Cut, similar for Surf, Strength, Rock Smash, Waterfall, Rock Climb, Defog).
- Field obstacles check for the relevant Key Item instead of an HM move on a party Pokémon.
- HM moves themselves: TBD (could be removed entirely, repurposed, or left as regular damaging moves with their attack stats).

## Open Questions / Decide Before Implementing

- **Roll level:** What level does a rolled Pokémon arrive at? Scaled to badge count? Fixed list per roll-type? Match player's lowest party level?
- **Party size > 6:** Expand party slots, or cap initial draft at 6 with one in reserve, or some other compromise?
- **HM moves themselves:** What happens to Cut/Surf/etc. as moves once they're no longer needed for traversal?
- **Pool exact size:** Once base-form/single-stage filtering is applied to the National Dex, get an exact count and confirm it's enough for 7 initial rolls + 8 post-gym rolls without forced repeats becoming awkward.
- **Stat-based ban algorithm (post-v1):** Whose stats? Base stats sum? Specific stat thresholds? Banning highest BST = harder run; banning lowest = easier run.
- **In-game trades full list:** Audit every in-game trade in Platinum and confirm the "consume mon, receive roll" rule applies cleanly to all of them.
- **Forced events full list:** Audit every forced gift/capture (Eevee, eggs, Porygon, Giratina, Spiritomb, Rotom, etc.) and confirm replacement strategy for each.
- **Underground:** Does the Sinnoh Underground need any changes? (Spheres, fossils, secret bases — likely irrelevant but verify.)

## Versioning Plan

- **v1 (current):** Permadeath + Initial 7-roll Draft + Poké Ball removal.
- **v2:** Add post-gym rolls + bag-in-combat disable + free Rare Candies + unsellable Rare Candies.
- **v3:** Add HM Overhaul + forced-event replacement + in-game trade replacement.
- **v4 (polish):** Visible locked box (memorial), stat-based ban algorithm, balance pass.