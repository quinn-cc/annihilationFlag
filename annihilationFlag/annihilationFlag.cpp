/*
 Custom flag: ANnihilation (+AN)
 Shooting yourself causes everyone on the map to die.
 
 Server Variables:
 _annihilationRequireEligibility - whether or not the elgibility ratio is in use
 _annihilationEligibleRatio - a required wins to losses ratio for users to annihilate
 _annihilationSpecialEffects - bool on whether to display shock wave special effects on annihilation
 _annihilationFXSWCount - number of shock waves on special effect
 _annihilationFXSWMinHeight - min height at which shock waves special effects occur
 _annihilationFXSWMaxHeight - max height ... shock wave effects
 
 Extra notes:
 - Burrow (+BU) is immune to Annihihlation
 
 Copyright 2022 Quinn Carmack
 May be redistributed under either the LGPL or MIT licenses.
 
 ./configure --enable-custom-plugins=annihilationFlag
*/
 
#include "bzfsAPI.h"
#include <math.h>
#include <cstdlib>
#include <ctime>

using namespace std;

class AnnihilationFlag : public bz_Plugin {

    virtual const char* Name()
    {
        return "Annihilation Flag";
    }

    virtual void Init(const char*);
    virtual void Event(bz_EventData*);
    ~AnnihilationFlag();

    virtual void Cleanup(void)
    {
        Flush();
    }
};

BZ_PLUGIN(AnnihilationFlag)


void AnnihilationFlag::Init(const char*)
{
    bz_RegisterCustomFlag("AN", "Annihilation", "Shoot yourself and everyone dies.", 0, eGoodFlag);
    bz_registerCustomBZDBBool("_annihilationRequireEligibility", false);
    bz_registerCustomBZDBDouble("_annihilationEligibleRatio", 1.2);		// Wins to losses ratio
    bz_registerCustomBZDBBool("_annihilationSpecialEffects", true);
    bz_registerCustomBZDBInt("_annihilationFXSWCount", 20);
    bz_registerCustomBZDBInt("_annihilationFXSWMinHeight", 10);	
    bz_registerCustomBZDBInt("_annihilationFXSWMaxHeight", 20);					
    Register(bz_ePlayerDieEvent);
}

AnnihilationFlag::~AnnihilationFlag() {}

void AnnihilationFlag::Event(bz_EventData *eventData)
{
    if (eventData->eventType == bz_ePlayerDieEvent)
    {
        bz_PlayerDieEventData_V2 *data = (bz_PlayerDieEventData_V2*) eventData;

        if (data->flagKilledWith == "AN")
        {
        	// If they didn't shoot themselves, get out
            if (data->killerID != data->playerID) return;
            
        	// Check if the player is eligible to use Annihilation
        	if (bz_getBZDBBool("_annihilationRequireEligibility") && bz_getPlayerLosses(data->playerID) != 0 &&
        		bz_getPlayerWins(data->playerID)/bz_getPlayerLosses(data->playerID) >= bz_getBZDBDouble("_annihilationEligibleRatio"))
        	{
        		bz_sendTextMessage(BZ_SERVER, data->playerID, "Your wins to losses ratio makes you uneligible for annihilaiton.");
        		return;
        	}
        
            bz_APIIntList* playerList = bz_getPlayerIndexList();
			bz_sendTextMessagef(BZ_SERVER, BZ_ALLUSERS, "BOOM! %s just annihilated the map!", bz_getPlayerCallsign(data->playerID));
			
			// Kill all the players
            for (int i = 0; i < bz_getPlayerCount(); i++)
            {
            	bz_BasePlayerRecord* playerRecord = bz_getPlayerByIndex(playerList->get(i));
            	
				// If the player is alive, kill them.
				if (playerRecord && playerRecord->spawned)
				{
					// BUrrow (+BU) is immune to ANnihilation (+AN)
					if (playerRecord->currentFlag != "BUrrow (+BU)")
					{
						bz_killPlayer(playerList->get(i), false, data->playerID, "AN");
						// bz_killPlayer automatically decrements their score, so
						// we add it back by the following.
						bz_incrementPlayerLosses(playerList->get(i), -1);
						
						if (playerList->get(i) != data->playerID)
		            		bz_sendTextMessagef(playerList->get(i), playerList->get(i), "Map annihilated by player %s.", bz_getPlayerCallsign(data->playerID));
                	}
				}
				// Free the record.
				bz_freePlayerRecord(playerRecord);
            }
            
            // Special effects (shock waves)
            if (bz_getBZDBBool("_annihilationSpecialEffects"))
            {
            	std::srand(std::time(nullptr));
			
            	for (int i = 0; i < bz_getBZDBInt("_annihilationFXSWCount"); i++)
            	{
            		float pos[3];
            		pos[0] = (std::rand() % (int)bz_getBZDBDouble("_worldSize")) - (bz_getBZDBDouble("_worldSize")/2);
            		pos[1] = (std::rand() % (int)bz_getBZDBDouble("_worldSize")) - (bz_getBZDBDouble("_worldSize")/2);
            		pos[2] = (std::rand() % (bz_getBZDBInt("_annihilationFXSWMaxHeight") - bz_getBZDBInt("_annihilationFXSWMinHeight"))) + bz_getBZDBInt("_annihilationFXSWMinHeight");
            		
            		float vel[3] = { 0, 0, 0 };
            		
            		bz_fireServerShot("SW", pos, vel, eRogueTeam);
            	}
        	}
        	
        	// Reset the flag into the world
        	bz_resetFlag(data->flagHeldWhenKilled);
        }
    }
}

