/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2006 Torus Knot Software Ltd
Also see acknowledgements in Readme.html

You may use this sample code for anything you like, it is not covered by the
LGPL like the rest of the engine.
-----------------------------------------------------------------------------
*/

/**
    \file 
        Terrain.h
    \brief
        Specialisation of OGRE's framework application to show the
        terrain rendering plugin 
*/

#include "ExampleApplication.h"

/*
#define CAMERA_HEIGHT 5

RaySceneQuery* raySceneQuery = 0;

// Event handler to add ability to alter curvature
class TerrainFrameListener : public ExampleFrameListener
{
public:
    TerrainFrameListener(RenderWindow* win, Camera* cam, SceneManager* sceneMgr)
        : ExampleFrameListener(win, cam, false, false)
    {
        // Reduce move speed
        mMoveSpeed = 50;
		mSceneMgr = sceneMgr;

    }

    bool frameStarted(const FrameEvent& evt)
    {
//        if( ExampleFrameListener::frameStarted(evt) == false )
//		return false;

        // clamp to terrain
        static Ray updateRay;
        updateRay.setOrigin(mCamera->getPosition());
        updateRay.setDirection(Vector3::NEGATIVE_UNIT_Y);
        raySceneQuery->setRay(updateRay);
        RaySceneQueryResult& qryResult = raySceneQuery->execute();
        RaySceneQueryResult::iterator i = qryResult.begin();
        if (i != qryResult.end() && i->worldFragment)
        {
            mCamera->setPosition(mCamera->getPosition().x, 
                i->worldFragment->singleIntersection.y + CAMERA_HEIGHT,//10, 
                mCamera->getPosition().z);
        }

	    // Rob added this to see if flare is fired

		if( !mMouse->buffered() )
			if( processUnbufferedMouseInput(evt) == false )
			{
				return false;
			}
        return true;
    }
	
	bool processUnbufferedMouseInput(const FrameEvent& evt)
	{
		using namespace OIS;

		// Clicking fires a flare.
		const MouseState &ms = mMouse->getMouseState();
		if( ms.buttonDown( MB_Left ) )
		{
		    if (!mMouseDown)
			{
			    // Fireworks!!!
				if (!flareNode)
				{
					flareNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
					flareNode->attachObject(mSceneMgr->createParticleSystem("SignalFlare", "ArcEx/SignalFlare"));
					flareNode->attachObject(mSceneMgr->createParticleSystem("SignalSmoke", "ArcEx/SignalSmoke"));
					Light *sigLight = mSceneMgr->createLight("SignalLight");
					sigLight->setDiffuseColour(1, 0.1, 0);
					flareNode->attachObject(sigLight);
		        }
				Ogre::Vector3 camDir = mCamera->getDirection();
				flareVel = 100*camDir;
				flareNode->setPosition(mCamera->getPosition());
				
				mMouseDown = true;
			}
		}
		else
		{
		    mMouseDown = false;
			if (flareNode)
			{
			    Real t = evt.timeSinceLastFrame;
				Ogre::Vector3 pos = flareNode->getPosition();
				flareVel.y -= 9.8 * t;
				pos.x += flareVel.x * t;
				pos.y += flareVel.y * t;
				pos.z += flareVel.z * t;
				flareNode->setPosition(pos);
				
			}
		}

		return true;
	}

protected:
    bool mMouseDown;
	SceneManager *mSceneMgr;
	SceneNode *flareNode;
	Vector3 flareVel;
	
};


class TerrainApplication : public ExampleApplication
{
public:
    TerrainApplication() {}

    ~TerrainApplication()
    {
        delete raySceneQuery;
    }

protected:
    virtual void chooseSceneManager(void)
    {
        // Get the SceneManager, in this case a generic one
        mSceneMgr = mRoot->createSceneManager("TerrainSceneManager");
    }

    virtual void createCamera(void)
    {
        // Create the camera
        mCamera = mSceneMgr->createCamera("PlayerCam");

        // Position it at 500 in Z direction
        mCamera->setPosition(Vector3(128,25,128));
        // Look back along -Z
        mCamera->lookAt(Vector3(0,0,-300));
        mCamera->setNearClipDistance( 1 );
        mCamera->setFarClipDistance( 1000 );

    }
   
    // Just override the mandatory create scene method
    void createScene(void)
    {
        Plane waterPlane;

        // Fog
        // NB it's VERY important to set this before calling setWorldGeometry 
        // because the vertex program picked will be different
        //ColourValue fadeColour(0.76, 0.86, 0.93);
		ColourValue fadeColour(0.1, 0.1, 0.2);
        //mSceneMgr->setFog( FOG_LINEAR, fadeColour, .001, 500, 1000);
        mWindow->getViewport(0)->setBackgroundColour(fadeColour);

        // Infinite far plane?
        if (mRoot->getRenderSystem()->getCapabilities()->hasCapability(RSC_INFINITE_FAR_PLANE))
        {
            mCamera->setFarClipDistance(0);
        }

        // Define the required skyplane
        Plane plane;
        // 5000 world units from the camera
        plane.d = 5000;
        // Above the camera, facing down
        plane.normal = -Vector3::UNIT_Y;

        // Set a nice viewpoint
        mCamera->setPosition(707,2500,528);
        mCamera->setOrientation(Quaternion(-0.3486, 0.0122, 0.9365, 0.0329));
        //mRoot -> showDebugOverlay( true );

        raySceneQuery = mSceneMgr->createRayQuery(
            Ray(mCamera->getPosition(), Vector3::NEGATIVE_UNIT_Y));
			
    }
    // Create new frame listener
    void createFrameListener(void)
    {
        mFrameListener= new TerrainFrameListener(mWindow, mCamera, mSceneMgr);
        mRoot->addFrameListener(mFrameListener);
    }

    void fireFlare(void)
	{

    }
	
};

*/
