#ifndef NPC_SOGGYCAT_H
#define NPC_SOGGYCAT_H
#ifdef _WIN32
#pragma once
#endif

#include "ai_basenpc.h"
#include "ai_motor.h"
#include "ai_navigator.h"
#include "ai_senses.h"
#include "ai_schedule.h"
#include "ai_task.h"
#include "ai_behavior.h"
#include "player.h"
#include "shake.h"

// ===============================
//   CNPCSoggyCat
// ===============================
class CNPCSoggyCat : public CAI_BaseNPC
{
public:
    DECLARE_CLASS(CNPCSoggyCat, CAI_BaseNPC);
    DECLARE_DATADESC();

    CNPCSoggyCat();

    // Core overrides
    void Spawn() override;
    void Precache() override;
    void Think() override;
    Class_T Classify() override { return CLASS_NONE; }

    // Behavior
    void UpdatePlayerLookState();
    bool IsPlayerLookingAtMe(CBasePlayer* pPlayer);

    // Teleport
    void TeleportBehindPlayer(CBasePlayer* pPlayer);
    void TeleportAwayFromPlayer(CBasePlayer* pPlayer);

    // Effects
    void TriggerGlitchEffect(CBasePlayer* pPlayer);
    void TriggerJumpscare(CBasePlayer* pPlayer);

    // Navigation
    void MoveTowardsPlayer(CBasePlayer* pPlayer);
    void DirectorThink(CBasePlayer* pPlayer);
    // becomeMad in hammer editor
    void InputBecomeMad(inputdata_t &inputdata);

private:
    // Look detection
    float m_flLookStartTime;
    float m_flLastLookCheck;
    bool  m_bPlayerCurrentlyLooking;

    // Behavior tuning
    float m_flLookThreshold;      // stare time before jumpscare
    float m_flLookForgiveTime;    // look-away forgiveness
    float m_flLastTeleport;       // cooldown for teleport

    // Director movement
    float m_flNextDirectorThink;

    // Madness state variables
    bool  m_bIsMad;
    float m_flNextMadTeleport;
    int   m_nMadSkin;
};

#endif // NPC_SOGGYCAT_H
