/*
 *  Beacon.h
 *  Arctic-Explorer
 *
 *  Created by Robert on 21/05/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

class Beacon
{
public:
	Beacon(Light *l, SceneNode *n, ParticleSystem *p)
	{
		light = l;
		node = n;
		parts = p;
	}

	Light *light;
	SceneNode *node;
	ParticleSystem *parts;
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

		SceneNode *n = mSceneMgr->getRootSceneNode()->createChildSceneNode("beaconNode"+beaconNumStr);
		n->setPosition(position);
		
		Light *l = mSceneMgr->createLight("beaconLight"+beaconNumStr);
		//l->setAttenuation(3250, 1, 0.0014, 0.000007);
		l->setAttenuation(200, 1, 0.022, 0.0019);
		l->setDiffuseColour(1, 0, 0);
		
		ParticleSystem *p = mSceneMgr->createParticleSystem("beaconPart"+beaconNumStr, "ArcEx/SignalSmoke");

		n->attachObject(l);
		n->attachObject(p);

		Beacon b = Beacon(l, n, p);
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
