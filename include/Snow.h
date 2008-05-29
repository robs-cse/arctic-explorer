/*
 *  Snow.h
 *  Arctic-Explorer
 *
 *  Created by MelodyWang on 28/05/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#define MAX_SNOW_EMIT 950
#define SNOW_INC 2

#define MAX_FOG_EMIT 600
#define FOG_INC 2

class ArcticSnowManager
{
public:
	ArcticSnowManager(SceneNode *n, SceneManager *sceneMgr)
	{
        node = n;
        mSceneMgr = sceneMgr;

        snowParticle = mSceneMgr->createParticleSystem("snowParticle", "ArcEx/SnowParticle");
		node->attachObject(snowParticle);

        fogParticle = mSceneMgr->createParticleSystem("fogParticle", "ArcEx/Fog");
		node->attachObject(fogParticle);

        snowEmmitter = snowParticle->getEmitter(0);
        fogEmmitter = fogParticle->getEmitter(0);
        
        currentSnowEmitRate = 0.0;
        currentFogEmitRate = 0.0;
        snowEmmitter->setEmissionRate (currentSnowEmitRate);
        fogEmmitter->setEmissionRate (currentFogEmitRate);
	}
    
    void adjustSnow(bool isSnow, Real sunHeight)
    {
        // If snow is turned on and snow is not emitting at max, then increase gradually
        if (isSnow && (currentSnowEmitRate < MAX_SNOW_EMIT))
        {
            currentSnowEmitRate ++;
            snowEmmitter->setEmissionRate (currentSnowEmitRate);
        }
        else if (!isSnow && (currentSnowEmitRate > 0.0))
        {
            currentSnowEmitRate --;
            snowEmmitter->setEmissionRate (currentSnowEmitRate);
        }
        snowEmmitter->setColour(ColourValue(1.0, 1.0, 1.0)*(sunHeight/1.7 + 0.4));
    }
    
    void adjustFog(bool isFog, Real sunHeight)
    {
        if (isFog && (currentFogEmitRate < MAX_FOG_EMIT))
        {

            currentFogEmitRate ++;
            fogEmmitter->setEmissionRate (currentFogEmitRate);
        }
        else if (!isFog && (currentFogEmitRate > 0.0))
        {
            currentFogEmitRate --;
            fogEmmitter->setEmissionRate (currentFogEmitRate);
        }
        fogEmmitter->setColour(ColourValue(1.0, 1.0, 1.0)*(sunHeight/1.7 + 0.4));
    }

private:
    Real currentSnowEmitRate;
    Real currentFogEmitRate;

    SceneManager *mSceneMgr;
    SceneNode *node;
    
    ParticleEmitter *snowEmmitter, *fogEmmitter;
    ParticleSystem *snowParticle, *fogParticle;
};

