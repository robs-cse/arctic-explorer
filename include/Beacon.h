/*
 *  Beacon.h
 *  Arctic-Explorer
 *
 *  Created by Robert on 21/05/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#define SMOKE_HEIGHT 1
#define FLAME_HEIGHT 0
#define LIGHT_HEIGHT 3

class Beacon
{
public:
	Beacon(Light *l, SceneNode *n, ParticleSystem *sp, ParticleSystem *fp)
	{
		light = l;
		node = n;
		smoke = sp;
		fire = fp;
	}

	Light *light;
	SceneNode *node;
	ParticleSystem *smoke;
	ParticleSystem *fire;
};

class BeaconManager
{
public:
	BeaconManager(SceneManager *sceneMgr)
	{
		mSceneMgr = sceneMgr;
		exists = false;
		beaconCounter = 0;
	}

	void
	newBeacon(Vector3 position)
	{
		String beaconNumStr = StringConverter::toString(beaconCounter);
		std::cout << beaconNumStr;

		SceneNode *bn = mSceneMgr->getRootSceneNode()->createChildSceneNode("beaconNode"+beaconNumStr);
		bn->setPosition(position);
		bn->translate(0, FLAME_HEIGHT, 0);

		SceneNode *bsn = bn->createChildSceneNode("beaconSmokeNode"+beaconNumStr);
		bsn->translate(0, LIGHT_HEIGHT, 0);
		
		Light *l = mSceneMgr->createLight("beaconLight"+beaconNumStr);
		//l->setAttenuation(3250, 1, 0.0014, 0.000007);
		l->setAttenuation(200, 1, 0.022, 0.0019);
		l->setDiffuseColour(1, 0, 0);
		
		ParticleSystem *sp = mSceneMgr->createParticleSystem("beaconSmoke"+beaconNumStr, "ArcEx/SignalSmoke");
		ParticleSystem *fp = mSceneMgr->createParticleSystem("beaconFire"+beaconNumStr, "ArcEx/SignalBeacon");

		bsn->attachObject(l);
		bsn->attachObject(sp);
		bn->attachObject(fp);

		sp->setVisible(false); // test

		Beacon b = Beacon(l, bn, sp, fp);
		mBeacons.push(b);
		
		beaconCounter++;		
	}

	bool
	BeaconManager::beaconExists()
	{
		return !(mBeacons.empty());
	}
	
protected:
	SceneManager *mSceneMgr;
	bool exists;
	std::queue<Beacon> mBeacons;
	int beaconCounter;
};
