#include "cbase.h"
#include "gameinterface.h"

int m_iPagesCollectedMask; // bit 0–7

class CEnvCollects8Pages : public CBaseEntity
{
public:
    DECLARE_CLASS(CEnvCollects8Pages, CBaseEntity);
    DECLARE_DATADESC();

    CEnvCollects8Pages();

    void Spawn() OVERRIDE;
    void Activate() OVERRIDE;

    void CollectPage(int iPage);

    void UpdateHUD();

    // Inputs
    void InputTake1(inputdata_t& inputdata) { CollectPage(1); }
    void InputTake2(inputdata_t& inputdata) { CollectPage(2); }
    void InputTake3(inputdata_t& inputdata) { CollectPage(3); }
    void InputTake4(inputdata_t& inputdata) { CollectPage(4); }
    void InputTake5(inputdata_t& inputdata) { CollectPage(5); }
    void InputTake6(inputdata_t& inputdata) { CollectPage(6); }
    void InputTake7(inputdata_t& inputdata) { CollectPage(7); }
    void InputTake8(inputdata_t& inputdata) { CollectPage(8); }

    // Output
    COutputEvent m_OnFinishedCollect;

private:
    int m_iPagesCollected;
};

LINK_ENTITY_TO_CLASS(env_collects8pages, CEnvCollects8Pages);

BEGIN_DATADESC(CEnvCollects8Pages)

DEFINE_FIELD(m_iPagesCollected, FIELD_INTEGER),

DEFINE_INPUTFUNC(FIELD_VOID, "take1", InputTake1),
DEFINE_INPUTFUNC(FIELD_VOID, "take2", InputTake2),
DEFINE_INPUTFUNC(FIELD_VOID, "take3", InputTake3),
DEFINE_INPUTFUNC(FIELD_VOID, "take4", InputTake4),
DEFINE_INPUTFUNC(FIELD_VOID, "take5", InputTake5),
DEFINE_INPUTFUNC(FIELD_VOID, "take6", InputTake6),
DEFINE_INPUTFUNC(FIELD_VOID, "take7", InputTake7),
DEFINE_INPUTFUNC(FIELD_VOID, "take8", InputTake8),

DEFINE_OUTPUT(m_OnFinishedCollect, "OnFinishedCollect"),

END_DATADESC()

CEnvCollects8Pages::CEnvCollects8Pages()
{
    m_iPagesCollected = 0;
}

void CEnvCollects8Pages::Spawn()
{
    BaseClass::Spawn();

    // default if no save exists
    m_iPagesCollected = 0;
}

void CEnvCollects8Pages::Activate()
{
    BaseClass::Activate();

    UpdateHUD();
}

void CEnvCollects8Pages::CollectPage(int iPage)
{
    if (iPage < 1 || iPage > 8)
        return;

    int bit = (1 << (iPage - 1));

    if (m_iPagesCollectedMask & bit)
        return; // already collected

    m_iPagesCollectedMask |= bit;

    int count = 0;
    for (int i = 0; i < 8; i++)
        if (m_iPagesCollectedMask & (1 << i))
            count++;

    DevMsg("Page %d collected (%d/8)\n", iPage, count);

    if (count >= 8)
        m_OnFinishedCollect.FireOutput(this, this);

    UpdateHUD();
}

void CEnvCollects8Pages::UpdateHUD()
{
    // Simple debug / placeholder (safe for server)
    DevMsg("HUD Update: Pages %d/8\n", m_iPagesCollected);

    // Later you can sync to client HUD via SendProp or network var
}
