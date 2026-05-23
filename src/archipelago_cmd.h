/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <https://www.gnu.org/licenses/old-licenses/gpl-2.0>.
 */

/** @file archipelago_cmd.h Command definitions related to Archipelago integration. */

#ifndef ARCHIPELAGO_CMD_H
#define ARCHIPELAGO_CMD_H

#include "command_type.h"
#include "station_type.h"
#include "industry_type.h"
#include "town_type.h"
#include "vehicle_type.h"
#include "company_type.h"
#include "engine_type.h"

/**

 * Unlock an engine by its in-game name (server-authoritative).
 * This is called from AP_OnItemReceived and broadcasts the unlock to all clients.
 * @param flags Command flags
 * @param engine_name The name of the engine to unlock
 */
CommandCost CmdAPUnlockEngine(DoCommandFlags flags, const std::string &engine_name);

/**

 * Boost a station's cargo rating for a specific cargo type.
 * This is called from AP_OnItemReceived when processing "Free Station Upgrade" items.
 * Boosts ALL stations owned by the company.
 * @param flags Command flags
 * @param company_id The company whose stations to boost
 */
CommandCost CmdAPBoostStationRating(DoCommandFlags flags, CompanyID company_id);

/**
 * Unlock infrastructure (tracks, roads, signals, bridges, tunnels, airports, etc.).
 * This is called from AP_OnItemReceived when processing infrastructure unlock items.
 * @param flags Command flags
 * @param infrastructure_type The type of infrastructure to unlock (0-N mapping)
 * @param param2 Additional parameter for infrastructure type
 */
CommandCost CmdAPUnlockInfrastructure(DoCommandFlags flags, uint8_t infrastructure_type, uint32_t param2);

/**
 * Change company money (for trap/bonus items).
 * This is called from AP_OnItemReceived for money-related items.
 * @param flags Command flags
 * @param amount Money to add/subtract
 */
CommandCost CmdAPChangeCompanyMoney(DoCommandFlags flags, Money amount);

/**
 * Change company loan amount (for Maintenance Surge, Bank Loan Forced traps).
 * This is called from AP_OnItemReceived for loan-related items.
 * @param flags Command flags
 * @param amount Loan amount to add/subtract
 */
CommandCost CmdAPChangeCompanyLoan(DoCommandFlags flags, Money amount);

/**
 * Set fast-forward speed limit (for Speed Boost items).
 * This is called from AP_OnItemReceived when processing speed boost items.
 * @param flags Command flags
 * @param speed_limit The new speed limit
 */
CommandCost CmdAPSetFastforwardSpeed(DoCommandFlags flags, uint16_t speed_limit);

/**
 * Boost all vehicles' reliability (for Reliability Boost items).
 * This sets all primary vehicles to max reliability and zero breakdown chance.
 * @param flags Command flags
 * @param company_id The company whose vehicles to boost
 */
CommandCost CmdAPBoostVehicleReliability(DoCommandFlags flags, CompanyID company_id);

/**
 * Apply breakdown to all vehicles (for Breakdown Wave trap).
 * This sets breakdown chance and reliability to force breakdowns.
 * @param flags Command flags
 * @param company_id The company whose vehicles to affect
 */
CommandCost CmdAPApplyBreakdown(DoCommandFlags flags, CompanyID company_id);

/**
 * Apply signal failure to trains (for Signal Failure trap).
 * @param flags Command flags
 * @param company_id The company whose trains to affect
 */
CommandCost CmdAPApplySignalFailure(DoCommandFlags flags, CompanyID company_id);

/**
 * Trigger immediate growth in all towns (for Town Growth Boost).
 * @param flags Command flags
 * @param unused Parameter (unused)
 */
CommandCost CmdAPTriggerTownGrowth(DoCommandFlags flags, uint32_t unused);

/**
 * Close industry production (for Industry Closure trap).
 * @param flags Command flags
 * @param industry_id The industry to close
 */
CommandCost CmdAPCloseIndustry(DoCommandFlags flags, IndustryID industry_id);

/**
 * Revoke vehicle type license (for License Revoke trap).
 * @param flags Command flags
 * @param vehicle_type The vehicle type to revoke (0=train, 1=road, 2=aircraft, 3=ship)
 */
CommandCost CmdAPRevokeLicense(DoCommandFlags flags, uint8_t vehicle_type);

/**
 * Start cargo payment bonus timer (for Cargo Bonus).
 * @param flags Command flags
 * @param duration_ticks Duration in ticks (240 ticks = 60 seconds)
 */
CommandCost CmdAPStartCargoBonus(DoCommandFlags flags, uint16_t duration_ticks);

/**
 * Start fuel shortage timer (for Fuel Shortage trap).
 * @param flags Command flags
 * @param duration_ticks Duration in ticks
 */
CommandCost CmdAPStartFuelShortage(DoCommandFlags flags, uint16_t duration_ticks);

DEF_CMD_TRAIT(CMD_AP_UNLOCK_ENGINE,            CmdAPUnlockEngine,            CommandFlag::Deity, CommandType::OtherManagement)
DEF_CMD_TRAIT(CMD_AP_BOOST_STATION_RATING,    CmdAPBoostStationRating,      CommandFlag::Deity, CommandType::OtherManagement)
DEF_CMD_TRAIT(CMD_AP_UNLOCK_INFRASTRUCTURE,   CmdAPUnlockInfrastructure,    CommandFlag::Deity, CommandType::OtherManagement)
DEF_CMD_TRAIT(CMD_AP_CHANGE_COMPANY_MONEY,    CmdAPChangeCompanyMoney,      CommandFlag::Deity, CommandType::MoneyManagement)
DEF_CMD_TRAIT(CMD_AP_CHANGE_COMPANY_LOAN,     CmdAPChangeCompanyLoan,       CommandFlag::Deity, CommandType::MoneyManagement)
DEF_CMD_TRAIT(CMD_AP_SET_FASTFORWARD_SPEED,   CmdAPSetFastforwardSpeed,     CommandFlag::Deity, CommandType::OtherManagement)
DEF_CMD_TRAIT(CMD_AP_BOOST_VEHICLE_RELIABILITY, CmdAPBoostVehicleReliability, CommandFlag::Deity, CommandType::OtherManagement)
DEF_CMD_TRAIT(CMD_AP_APPLY_BREAKDOWN,         CmdAPApplyBreakdown,          CommandFlag::Deity, CommandType::OtherManagement)
DEF_CMD_TRAIT(CMD_AP_APPLY_SIGNAL_FAILURE,    CmdAPApplySignalFailure,      CommandFlag::Deity, CommandType::OtherManagement)
DEF_CMD_TRAIT(CMD_AP_TRIGGER_TOWN_GROWTH,     CmdAPTriggerTownGrowth,       CommandFlag::Deity, CommandType::OtherManagement)
DEF_CMD_TRAIT(CMD_AP_CLOSE_INDUSTRY,          CmdAPCloseIndustry,           CommandFlag::Deity, CommandType::OtherManagement)
DEF_CMD_TRAIT(CMD_AP_REVOKE_LICENSE,          CmdAPRevokeLicense,           CommandFlag::Deity, CommandType::OtherManagement)
DEF_CMD_TRAIT(CMD_AP_START_CARGO_BONUS,       CmdAPStartCargoBonus,         CommandFlag::Deity, CommandType::OtherManagement)
DEF_CMD_TRAIT(CMD_AP_START_FUEL_SHORTAGE,     CmdAPStartFuelShortage,       CommandFlag::Deity, CommandType::OtherManagement)

#endif /* ARCHIPELAGO_CMD_H */
