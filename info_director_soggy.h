#ifndef INFO_DIRECTOR_SOGGY_H
#define INFO_DIRECTOR_SOGGY_H
#ifdef _WIN32
#pragma once
#endif

#include "cbase.h"
#include "entityoutput.h"

class CInfoDirectorSoggy : public CBaseEntity
{
public:
    DECLARE_CLASS(CInfoDirectorSoggy, CBaseEntity);
    DECLARE_DATADESC();

    void Spawn() override;
    void Think() override;

    void SpawnSoggyCat();
    void RemoveExistingSoggyCat();

private:
    float m_flSpawnDelay;
    float m_flNextSpawnCheck;
    EHANDLE m_hSoggyCat;
};

#endif
