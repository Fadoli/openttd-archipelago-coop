/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <https://www.gnu.org/licenses/old-licenses/gpl-2.0>.
 */

/** @file archipelago_cmd.cpp Commands related to Archipelago integration. */

#include "stdafx.h"
#include "archipelago_cmd.h"
#include "archipelago.h"
#include "company_func.h"
#include "command_func.h"
#include "engine_base.h"
#include "station_base.h"
#include "economy_func.h"
#include "vehicle_func.h"

extern bool AP_UnlockEngineByName(const std::string &name);

/**
 * Unlock an engine by its name - server-authoritative.
 * This command is executed on the server and broadcasts to all clients.
 * 
 * @param flags Command flags
 * @param engine_name The name of the engine to unlock
 * @return The cost of the command (always 0, cannot fail)
 */
CommandCost CmdAPUnlockEngine(DoCommandFlags flags, const std::string &engine_name)
{
	if (flags & DC_EXEC) {
		/* The actual unlock logic is in AP_UnlockEngineByName which handles:
		 * - Looking up the engine by name (with alias translation)
		 * - Enabling it for the company
		 * - Updating vehicle build and autoreplace windows
		 * 
		 * Since this is a server command executed in network mode, it will
		 * automatically propagate to all clients when executed on the server. */
		AP_UnlockEngineByName(engine_name);
	}
	
	return CommandCost();
}

/**
 * Boost all stations' cargo ratings for a company (Free Station Upgrade bonus).
 * Sets all rated cargo to MAX_STATION_RATING for all player stations.
 * 
 * @param flags Command flags
 * @param company_id The company whose stations to boost
 * @return The cost of the command (always 0)
 */
CommandCost CmdAPBoostStationRating(DoCommandFlags flags, CompanyID company_id)
{
	if (flags & DC_EXEC) {
		for (Station *st : Station::Iterate()) {
			if (st->owner != company_id) continue;
			for (CargoType ct = 0; ct < NUM_CARGO; ct++) {
				if (st->goods[ct].HasRating()) {
					st->goods[ct].rating = MAX_STATION_RATING;
				}
			}
		}
	}
	
	return CommandCost();
}

/**
 * Unlock infrastructure (tracks, roads, signals, etc.).
 * This command applies infrastructure unlocks server-side.
 * 
 * For now, this is a placeholder. Infrastructure unlocks can be applied
 * directly on the client (they don't modify the game state in a way that
 * causes desync, since they only affect what the player can build).
 * 
 * @param flags Command flags
 * @param infrastructure_type The type of infrastructure (0=track dirs, 1=roads, etc.)
 * @param param2 Additional parameter (rail type, direction bits, etc.)
 * @return The cost of the command (always 0, cannot fail)
 */
CommandCost CmdAPUnlockInfrastructure(DoCommandFlags flags, uint8_t infrastructure_type, uint32_t param2)
{
	/* Infrastructure unlocks are primarily client-side restrictions that don't
	 * affect multiplayer sync, as they only control what the player *can* build,
	 * not what they *did* build. These are applied via the regular item processing
	 * and don't need server synchronization.
	 * 
	 * If issues arise in multiplayer where clients disagree on what's unlocked,
	 * we can add server-side state here. */
	
	if (flags & DC_EXEC) {
		// Placeholder for future server-side infrastructure state tracking
	}
	
	return CommandCost();
}

/**
 * Change company money (for bonuses and traps).
 * This command is executed on the server to ensure synchronized money changes.
 * 
 * @param flags Command flags
 * @param amount Money amount to add (positive or negative)
 * @return The cost of the command
 */
CommandCost CmdAPChangeCompanyMoney(DoCommandFlags flags, Money amount)
{
	CompanyID cid = _current_company;
	if (cid >= MAX_COMPANIES) return CommandCost();
	
	Company *c = Company::GetIfValid(cid);
	if (c == nullptr) return CommandCost();
	
	if (flags & DC_EXEC) {
		/* Directly modify the company's money.
		 * In network mode, this is executed on the server and propagated to clients.
		 * 
		 * Note: In single-player mode, there's no command queue, but the command
		 * execution system handles this automatically. */
		c->money += amount;
		
		/* Update company statistics for bankruptcy/wealth tracking */
		InvalidateWindowClassesData(WC_FINANCES);
	}
	
	return CommandCost();
}

/**
 * Set the fast-forward speed limit.
 * This command updates client-side settings that are not replicated.
 * 
 * @param flags Command flags
 * @param speed_limit The new fast-forward speed limit (as percentage, e.g., 300 = 300%)
 * @return The cost of the command (always 0)
 */
CommandCost CmdAPSetFastforwardSpeed(DoCommandFlags flags, uint16_t speed_limit)
{
	if (flags & DC_EXEC) {
		// Currently this is handled in AP_OnItemReceived directly
		// This command exists for future expansion
	}
	
	return CommandCost();
}

/**
 * Boost all vehicles' reliability for a company (Reliability Boost bonus).
 * Sets all primary vehicles to max reliability and zero breakdown chance.
 * 
 * @param flags Command flags
 * @param company_id The company whose vehicles to boost
 * @return The cost of the command (always 0)
 */
CommandCost CmdAPBoostVehicleReliability(DoCommandFlags flags, CompanyID company_id)
{
	if (flags & DC_EXEC) {
		for (Vehicle *v : Vehicle::Iterate()) {
			if (v->owner == company_id && v->IsPrimaryVehicle()) {
				const Engine *e = v->GetEngine();
				if (e != nullptr) {
					v->reliability = e->reliability_max;
					v->breakdown_chance = 0;
				}
			}
		}
	}
	
	return CommandCost();
}

/**
 * Apply breakdown to all vehicles (Breakdown Wave trap).
 * Sets breakdown chance and low reliability to force breakdowns.
 * 
 * @param flags Command flags
 * @param company_id The company whose vehicles to affect
 * @return The cost of the command (always 0)
 */
CommandCost CmdAPApplyBreakdown(DoCommandFlags flags, CompanyID company_id)
{
	if (flags & DC_EXEC) {
		for (Vehicle *v : Vehicle::Iterate()) {
			if (v->owner == company_id && v->IsPrimaryVehicle()) {
				v->breakdown_chance = 255;
				v->reliability = 1;
			}
		}
	}
	
	return CommandCost();
}

/**
 * Apply signal failure to trains (Signal Failure trap).
 * Sets trains to breakdown state to simulate signal failure.
 * 
 * @param flags Command flags
 * @param company_id The company whose trains to affect
 * @return The cost of the command (always 0)
 */
CommandCost CmdAPApplySignalFailure(DoCommandFlags flags, CompanyID company_id)
{
	if (flags & DC_EXEC) {
		for (Vehicle *v : Vehicle::Iterate()) {
			if (v->owner == company_id && v->IsPrimaryVehicle() && v->type == VEH_TRAIN) {
				// ctr=2 is the "about to break down" trigger state
				if (v->breakdown_ctr == 0) {
					v->breakdown_ctr   = 2;
					v->breakdown_delay = 255;
				}
			}
		}
	}
	
	return CommandCost();
}

/**
 * Trigger immediate growth in all towns (Town Growth Boost).
 * Resets grow_counter to 0 for all towns to trigger next growth phase.
 * 
 * @param flags Command flags
 * @param unused Unused parameter
 * @return The cost of the command (always 0)
 */
CommandCost CmdAPTriggerTownGrowth(DoCommandFlags flags, uint32_t unused)
{
	if (flags & DC_EXEC) {
		for (Town *t : Town::Iterate()) {
			t->grow_counter = 0;
		}
	}
	
	return CommandCost();
}

/**
 * Close industry production (Industry Closure trap).
 * Sets production to 0 and clears production history.
 * 
 * @param flags Command flags
 * @param industry_id The industry to close
 * @return The cost of the command (always 0)
 */
CommandCost CmdAPCloseIndustry(DoCommandFlags flags, IndustryID industry_id)
{
	if (flags & DC_EXEC) {
		Industry *ind = Industry::GetIfValid(industry_id);
		if (ind != nullptr) {
			for (auto &produced : ind->produced) {
				produced.history[THIS_MONTH].production = 0;
			}
			ind->prod_level = 0;
		}
	}
	
	return CommandCost();
}

/**
 * Revoke vehicle type license (License Revoke trap).
 * Hides a vehicle type for the company for a duration.
 * 
 * @param flags Command flags
 * @param vehicle_type The vehicle type to revoke (0=train, 1=road, 2=aircraft, 3=ship)
 * @return The cost of the command (always 0)
 */
CommandCost CmdAPRevokeLicense(DoCommandFlags flags, uint8_t vehicle_type)
{
	if (flags & DC_EXEC) {
		CompanyID cid = _local_company;
		if (cid < MAX_COMPANIES) {
			for (Engine *e : Engine::Iterate()) {
				if ((int)e->type != (int)vehicle_type) continue;
				e->company_hidden.Set(cid);
			}
			MarkWholeScreenDirty();
		}
	}
	
	return CommandCost();
}

/**
 * Start cargo payment bonus timer (Cargo Bonus item).
 * Sets the timer that doubles cargo payments.
 * 
 * @param flags Command flags
 * @param duration_ticks Duration in ticks (240 ticks = ~60 seconds)
 * @return The cost of the command (always 0)
 */
CommandCost CmdAPStartCargoBonus(DoCommandFlags flags, uint16_t duration_ticks)
{
	if (flags & DC_EXEC) {
		extern int _ap_cargo_bonus_ticks;
		_ap_cargo_bonus_ticks = duration_ticks;
	}
	
	return CommandCost();
}

/**
 * Start fuel shortage timer (Fuel Shortage trap).
 * Sets the timer that applies speed cap to vehicles.
 * 
 * @param flags Command flags
 * @param duration_ticks Duration in ticks
 * @return The cost of the command (always 0)
 */
CommandCost CmdAPStartFuelShortage(DoCommandFlags flags, uint16_t duration_ticks)
{
	if (flags & DC_EXEC) {
		extern int _ap_fuel_shortage_ticks;
		_ap_fuel_shortage_ticks = duration_ticks;
	}
	
	return CommandCost();
}
