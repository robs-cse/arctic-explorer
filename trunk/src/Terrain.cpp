/********************************************************************************
 *  
 * Terrain.cpp
 * 
 * Created by Robert Sison - Friday 2/05/08
 * - Created starting framework
 * - setup subversion
 *
 * Modified by Melody Wang - Wednesday 7/05/08
 * #include "TerrainApplication.h"  wrote to set up an application from scatch
 * Currently not working with terrain yet
 * 
 *
 ********************************************************************************/

#include "Terrain.h"

#include "ExampleApplication.h"
#include <OgreStringConverter.h>

// #include "TerrainApplication.h"  // not working yet

#define CAMERA_HEIGHT 2
#define FLARE_LAUNCH_VELOCITY 66
#define WALK_SPEED 5
#define FLY_SPEED 100
#define CLOUD_HEIGHT 999
#define STAR_HEIGHT 1000

#define SUN_AXIS_RADIUS 800
#define MOON_AXIS_RADIUS 600
#define CLOUD_PLANE_SIZE 100*CLOUD_HEIGHT
#define STAR_PLANE_SIZE 100*STAR_HEIGHT
#define CLOUD_PLANE_REPEATS 10
#define STAR_PLANE_REPEATS 50

#define DEFAULT_SECONDS_IN_A_DAY 64.0
#define TIME_LAPSE_SECONDS_IN_A_DAY 12.0

#define START_X 1000
#define START_Z 800

#define LENS_FLARE_SIZE 3
#define SIG_FLARE_SIZE 30

#define MEL_SUNLIGHT
//#define ROB_DEBUG

#include "Beacon.h"
#include "Snow.h"

RaySceneQuery *raySceneQuery = 0;
BeaconManager *mBeaconManager = 0;


/********************************************************************************
 * Sky and weather frame listener
 ********************************************************************************/
 
class SkyWeatherFrameListener : public ExampleFrameListener
{
public:
    SkyWeatherFrameListener(RenderWindow* win, Camera* cam, SceneManager *sceneMgr)
        : ExampleFrameListener(win, cam, false, false)
    {
		mStatsOn = false;
		mDebugOverlay->hide();
		mMoveSpeed = WALK_SPEED;
		
        mSceneMgr = sceneMgr;
        mToggle = 0.0;
        
        // Set initial time of day
        // Time of day goes in cycles representing the 24 hours in a day
        timeOfDay = 0.0;
        starMoveTime = 0.0;
        secondsInADay = DEFAULT_SECONDS_IN_A_DAY; // default seconds in a day
        timeLapseMode = false;
        flyMode = false;
        snow = false;
        fog = false;
        lensFlare = false;


        // Sun currently just moves up and down. Work out how to get sun to go in a circular path
        sunMoveAxis = mSceneMgr->getRootSceneNode()->createChildSceneNode("SunMoveAxis");
        sunMoveAxis->translate(Vector3(2000.0, 0.0, 800.0));
#ifdef ROB_DEBUG
		sunMoveAxis->attachObject(mSceneMgr->createParticleSystem("SunMoveAxis", "Examples/JetEngine2"));
#endif
        sunNode = sunMoveAxis->createChildSceneNode("SunNode");
        sunNode->translate(Vector3(0.0, SUN_AXIS_RADIUS - 10, 0.0)); 
        // Note the -10 is a hack. Otherwise the sky colout texture doesn't seem to work correctly
        
        // Attach a light to the sun node
        sunLight = mSceneMgr->createLight("MainLight");
		sunLight->setType(Light::LT_POINT);
        sunNode->attachObject(sunLight);
#ifdef ROB_DEBUG
		sunNode->attachObject(mSceneMgr->createParticleSystem("SunNode", "Examples/JetEngine1"));
#endif
        // TODO: change the colour of the light depending on the position of the sun      
        sunLight->setDiffuseColour(1, 1, 0.5);
        sunLight->setSpecularColour(1, 1, 0.5);

        // TODO make moon mesh? texture? and a blinking star map
        
        // Moon Node to give some lighting at night
        // The moon rotates on the same plane as the sun (though the radius of the path is smaller
        SceneNode* moonNode = sunMoveAxis->createChildSceneNode("MoonNode");
        moonNode->translate(Vector3(0.0, - MOON_AXIS_RADIUS, 0.0)); 
        // Attach a dim diffuse light to the moon node
        Light* moonLight = mSceneMgr->createLight("MoonLight");
		moonLight->setType(Light::LT_POINT);
        moonNode->attachObject(moonLight);
        moonLight->setDiffuseColour(0.05, 0.05, 0.1);
        
        weatherNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("snowNode");
        arcticSnow = new ArcticSnowManager(weatherNode, mSceneMgr);
        weatherNode->setPosition(mCamera->getPosition());

        createSkyFeatures();

		createBeaconManager();
		createSignalFlare();
		createLensFlare();
	}
    
    void createSkyFeatures(void)
    {
        // Parameter setting for sky plane shader
        MaterialPtr mat = (MaterialPtr)MaterialManager::getSingleton().getByName("ArcticSkyMaterial");
        skyVertParams = mat->getTechnique(0)->getPass(0)->getVertexProgramParameters();
        skyFragParams = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
        
        // star map shader
        mat = (MaterialPtr)MaterialManager::getSingleton().getByName("ArcticStarMapMaterial");
        starFragParams = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
        
        // cloud parameters
        mat = (MaterialPtr)MaterialManager::getSingleton().getByName("ArcticCloudsMaterial");
        cloudsVertParams = mat->getTechnique(0)->getPass(0)->getVertexProgramParameters();
        cloudsFragParams = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
    
		// Star plane
		Plane starPlane(-Vector3::UNIT_Y, 0);
		MeshManager::getSingleton().createCurvedPlane("stars",
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, starPlane,
			STAR_PLANE_SIZE,STAR_PLANE_SIZE, 0.3f,20, 20,true,1,
			STAR_PLANE_REPEATS,STAR_PLANE_REPEATS,Vector3::UNIT_Z);

		Entity *ent = mSceneMgr->createEntity("StarMapEntity", "stars");
		ent->setMaterialName("ArcticStarMapMaterial");

        StarPlaneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
        SceneNode *node = StarPlaneNode->createChildSceneNode();
        node->attachObject(ent);
        node->translate(Vector3(0.0, STAR_HEIGHT, 0.0));
        StarPlaneNode->setPosition(mCamera->getPosition());

        // Cloud plane
		Plane cloudPlane(-Vector3::UNIT_Y, 0);
		MeshManager::getSingleton().createCurvedPlane("clouds",
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, cloudPlane,
			CLOUD_PLANE_SIZE,CLOUD_PLANE_SIZE, 0.3f,20, 20,true,1,
			CLOUD_PLANE_REPEATS,CLOUD_PLANE_REPEATS,Vector3::UNIT_Z);

		ent = mSceneMgr->createEntity("CloudsMapEntity", "clouds");
		ent->setMaterialName("ArcticCloudsMaterial");

        CloudsPlaneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
        node = CloudsPlaneNode->createChildSceneNode();
        node->attachObject(ent);
        node->translate(Vector3(0.0, CLOUD_HEIGHT, 0.0));
        CloudsPlaneNode->setPosition(mCamera->getPosition());
    }
    
	void createBeaconManager(void)
	{
		mBeaconManager = new BeaconManager(mSceneMgr);
	}
	
	void createSignalFlare(void)
	{
		Billboard *bill;
		Real size = SIG_FLARE_SIZE;
		
		flareNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
					
		sigFlareParticle = mSceneMgr->createParticleSystem("SignalFlare", "ArcEx/SignalFlare");
		sigSmokeParticle = mSceneMgr->createParticleSystem("SignalSmoke", "ArcEx/SignalSmoke");
		sigLight = mSceneMgr->createLight("SignalLight");
		sigLight->setAttenuation(3250, 1, 0.0014, 0.000007);

		launchNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		launchLight = mSceneMgr->createLight("LaunchLight");
		launchLight->setAttenuation(600, 1, 0.007, 0.0002);
		//launchLight->setAttenuation(200, 1, 0.022, 0.0019);
		
		sigFlareSet = mSceneMgr->createBillboardSet("SignalHalo");
		sigFlareSet->setMaterialName("ArcEx/SigBurst");
		
		bill = sigFlareSet->createBillboard(0,0,0);
		bill->setDimensions(size, size);
		
		flareNode->attachObject(sigFlareParticle);
		flareNode->attachObject(sigSmokeParticle);
		flareNode->attachObject(sigLight);
		flareNode->attachObject(sigFlareSet);
		
		launchNode->attachObject(launchLight);
		
		flareNode->setVisible(false);        
    }

	void updateSignalFlare(Real t)
	{
		Ogre::Vector3 pos = flareNode->getPosition();
		flareVel.y -= 9.8 * t;
		pos.x += flareVel.x * t;
		pos.y += flareVel.y * t;
		pos.z += flareVel.z * t;
		flareNode->setPosition(pos);

		flareNode->setOrientation(mCamera->getOrientation());
		
		ColourValue col = launchLight->getDiffuseColour();
		launchLight->setDiffuseColour(col*0.9);
		
		col = sigLight->getDiffuseColour();
		col *= 1.1;
		col.saturate();
		sigLight->setDiffuseColour(col);
		
		// Test if the flare has hit the ground, if so, spawn a ground beacon
		if (mFlareAirborne)
		{
			static Ray flareImpactRay;
			flareImpactRay.setOrigin(flareNode->getPosition());
			flareImpactRay.setDirection(Vector3::UNIT_Y);
			raySceneQuery->setRay(flareImpactRay);
			RaySceneQueryResult& qryResult = raySceneQuery->execute();
			RaySceneQueryResult::iterator i = qryResult.begin();
			if (i != qryResult.end() && i->worldFragment)
			{
				beaconNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
				mBeaconManager->newBeacon
				(
					Vector3(flareNode->getPosition().x, 
					i->worldFragment->singleIntersection.y, 
					flareNode->getPosition().z)
				);
				mFlareAirborne = false;
				sigFlareSet->setVisible(false);
			}
		}
	}
    
    #define LENS_FLARE_LARGE_SIZE 10
    #define LENS_FLARE_MED_SIZE 5
    #define LENS_FLARE_SMALL_SIZE 2


	void createLensFlare(void)
	{
		Billboard *bill;
		Real size = LENS_FLARE_SIZE;
	
		lensNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();

		lensRainbowSet = mSceneMgr->createBillboardSet("LensRainbow");
		lensRainbowSet->setMaterialName("ArcEx/LensRainbow");
		bill = lensRainbowSet->createBillboard(0,0,0);
		bill->setDimensions(LENS_FLARE_LARGE_SIZE, LENS_FLARE_LARGE_SIZE);

		lensSimpleSet = mSceneMgr->createBillboardSet("LensSimple");
		lensSimpleSet->setMaterialName("ArcEx/LensSimple");
		bill = lensSimpleSet->createBillboard(0,0,0);
		bill->setDimensions(LENS_FLARE_SMALL_SIZE, LENS_FLARE_SMALL_SIZE);

		lensHaloSet = mSceneMgr->createBillboardSet("LensHalo");
		lensHaloSet->setMaterialName("ArcEx/LensHollow");
		bill = lensHaloSet->createBillboard(0,0,0);
		bill->setDimensions(LENS_FLARE_SMALL_SIZE, LENS_FLARE_SMALL_SIZE);
		
		lensBurstSet = mSceneMgr->createBillboardSet("LensBurst");
		lensBurstSet->setMaterialName("ArcEx/LensBurst");
		bill = lensBurstSet->createBillboard(0,0,0);
		bill->setDimensions(LENS_FLARE_MED_SIZE, LENS_FLARE_MED_SIZE);
		
		lensNode->attachObject(lensRainbowSet);
		lensNode->attachObject(lensSimpleSet);
		lensNode->attachObject(lensHaloSet);
		lensNode->attachObject(lensBurstSet);
		
		lensNode->setVisible(false);
	}
	void updateLensFlare(void)
	{
        if (!lensFlare)
        {
            lensNode->setVisible(false);
            return;
        }
        
		Vector3 camPosition = mCamera->getPosition();
		mCamera->setPosition(0,0,0);
        //std::cout<<"camera position"<<mCamera->getDerivedPosition()<<" sun "<<sunNode->getWorldPosition()<<"\n";

		if(!mCamera->isVisible(sunNode->getWorldPosition()))
		{
			mCamera->setPosition(camPosition);
			lensNode->setVisible(false);
			return;
		}
		mCamera->setPosition(camPosition);
		
		Vector3 sunPos = sunNode->getWorldPosition();
		Vector3 camDirection = mCamera->getDirection();
		sunPos.normalise();
		camDirection.normalise();
		
		// Test for obstacles blocking vision to the sun
		static Ray updateRay;
		updateRay.setOrigin(camPosition);
		updateRay.setDirection(sunPos);
		raySceneQuery->setRay(updateRay);
		RaySceneQueryResult& qryResult = raySceneQuery->execute();
		RaySceneQueryResult::iterator i;
		for (i = qryResult.begin(); i != qryResult.end(); i++)
		{
			if (i->worldFragment)
				break;
		}
		// If there are obstacles, abort
		if (i != qryResult.end())
		{
			lensNode->setVisible(false);
			return;
		}
		
        Vector3 lensFlareAxis(camDirection*40);
        Vector3 lensVect(sunNode->getWorldPosition() - lensFlareAxis);
        lensVect.normalise();
        
		lensBurstSet->getBillboard(0)->setPosition(lensFlareAxis + lensVect*40);
        lensHaloSet->getBillboard(0)->setPosition(lensFlareAxis + lensVect*20);
        lensSimpleSet->getBillboard(0)->setPosition(lensFlareAxis);
        lensRainbowSet->getBillboard(0)->setPosition(lensFlareAxis - lensVect*10);
//lensHaloSet->getBillboard(1)->setColour(ColourValue(1.0,0.0,0.0));

		// Apply lens flare effect
		lensNode->setPosition(camPosition);
		lensNode->setVisible(true);
	}




    bool frameStarted(const FrameEvent& evt)
    {
		if( ExampleFrameListener::frameStarted(evt) == false )
			return false;

        mKeyboard->capture();

        // Spinning the Sky plane
        StarPlaneNode->yaw(-Degree(timeOfDay/secondsInADay*360));

        // Cycling through the hours in a day
		sunMoveAxis->pitch(-Degree(timeOfDay/secondsInADay*360)); // undo last rotate
		timeOfDay += evt.timeSinceLastFrame;
        sunMoveAxis->pitch(Degree(timeOfDay/secondsInADay*360)); // do new rotate

        StarPlaneNode->yaw(Degree(timeOfDay/secondsInADay*360));

        StarPlaneNode->setPosition(mCamera->getPosition());
        CloudsPlaneNode->setPosition(mCamera->getPosition());


        starMoveTime += evt.timeSinceLastFrame/2.0;

        if (timeOfDay >= secondsInADay)
            timeOfDay = 0.0;
        
		if (starMoveTime >= secondsInADay) // a slower changing timeOfDay (determines movement of starmap)
            starMoveTime = 0.0;

        // Passing parameters to the sky vertex shader
        skyVertParams->setNamedConstant("sunPosition", sunNode->getWorldPosition());
        skyVertParams->setNamedConstant("camPosition", mCamera->getWorldPosition()); // NOTE: This is always 0,0,0. Camera space
        skyVertParams->setNamedConstant("starMoveTime", starMoveTime/secondsInADay);
        
        Real sunHeight = (sunNode->getWorldPosition().y + SUN_AXIS_RADIUS - sunMoveAxis->getWorldPosition().y)/(2*SUN_AXIS_RADIUS);
        skyFragParams->setNamedConstant("sunHeightRel", sunHeight);
        starFragParams->setNamedConstant("sunHeightRelative", sunHeight);
        cloudsFragParams->setNamedConstant("sunHeightRelative", sunHeight);
        cloudsVertParams->setNamedConstant("cloudsMoveTime", starMoveTime/secondsInADay);

//#ifdef MEL_SUNLIGHT
		Real timeFactor = sunHeight;
//#endif
        // Set light changes according to time
        sunLight->setDiffuseColour(ColourValue(0.85, 0.85, 0.65)*timeFactor);
        sunLight->setSpecularColour(ColourValue(0.8, 0.8, 0.8)*timeFactor);
	    mSceneMgr->setAmbientLight(ColourValue(0.3, 0.4, 0.6)*timeFactor); 
  
        // key can only be pressed once every 0.5  (1 second?)
		mToggle -= evt.timeSinceLastFrame;
        
        // Lens flare
        if ((mToggle < 0.0f ) && mKeyboard->isKeyDown(OIS::KC_K)) // Snow
        {
            mToggle = 0.5f;
            lensFlare = !lensFlare;
        }

        // Setting snow, fog (particle fog)     see Snow.h
        if ((mToggle < 0.0f ) && mKeyboard->isKeyDown(OIS::KC_I)) // Snow
        {
            mToggle = 0.5f;
            snow = !snow;
        }
        if ((mToggle < 0.0f ) && mKeyboard->isKeyDown(OIS::KC_O)) //Fog 
        {
            mToggle = 0.5f;
            fog = !fog;
        }
        weatherNode->setPosition(mCamera->getPosition());
        arcticSnow->adjustSnow(snow, sunHeight);
        arcticSnow->adjustFog(fog, sunHeight);
        
        // Toggle superspeed
        if ((mToggle < 0.0f ) && mKeyboard->isKeyDown(OIS::KC_L)) //Lapse
        {
            mToggle = 0.5f;
            timeLapseMode = !timeLapseMode;
            if (timeLapseMode)
            {
                timeOfDay *= TIME_LAPSE_SECONDS_IN_A_DAY/DEFAULT_SECONDS_IN_A_DAY; 
                starMoveTime *= TIME_LAPSE_SECONDS_IN_A_DAY/DEFAULT_SECONDS_IN_A_DAY;
                secondsInADay = TIME_LAPSE_SECONDS_IN_A_DAY;
            }
            else
            {
                timeOfDay *= DEFAULT_SECONDS_IN_A_DAY/TIME_LAPSE_SECONDS_IN_A_DAY; 
                starMoveTime *= DEFAULT_SECONDS_IN_A_DAY/TIME_LAPSE_SECONDS_IN_A_DAY; 
                secondsInADay = DEFAULT_SECONDS_IN_A_DAY;
            }

        }
		
		// Toggle fly/walk
		if ((mToggle < 0.0f ) && mKeyboard->isKeyDown(OIS::KC_F))
        {
            mToggle = 0.5f;
            flyMode = !flyMode;
			mMoveSpeed = (flyMode ? FLY_SPEED : WALK_SPEED);
        }
	
	    // Clamp to terrain when in walk mode
		if (!flyMode)
		{
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
		}
		
		// Check if flare is fired
		if( !mMouse->buffered() )
			if( processUnbufferedMouseInput(evt) == false )
			{
				return false;
			}
			
		// Update the status of an airborne flare's particle system and associated lights
		Real t = evt.timeSinceLastFrame;
		updateSignalFlare(t);
		
		// Lens flare procedure
		updateLensFlare();
		
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
				flareNode->setVisible(true);
				sigFlareSet->setVisible(true);
				
				if (!mFlareAirborne)
				{
					Ogre::Vector3 camDir = mCamera->getDirection();
				
					sigFlareParticle->clear();
					sigSmokeParticle->clear();
				
					flareVel = FLARE_LAUNCH_VELOCITY*camDir;
					flareNode->setPosition(mCamera->getPosition());

					sigLight->setDiffuseColour(0.001, 0, 0);

					launchLight->setDiffuseColour(5,5,2.5);
					launchNode->setPosition(mCamera->getPosition());
					
					mFlareAirborne = true;
				}
				
				mMouseDown = true;
			}
		}
		else
		{
		    mMouseDown = false;
		}	
		return true;
	}

protected:
    bool mMouseDown, mFlareAirborne;
	//SceneManager *mSceneMgr;
	SceneNode *flareNode, *launchNode, *beaconNode, *lensNode;
	Light *launchLight, *sigLight;
	ParticleSystem *sigFlareParticle, *sigSmokeParticle;
	BillboardSet *lensHaloSet, *lensBurstSet, *sigFlareSet, *lensRainbowSet, *lensSimpleSet;
	Vector3 flareVel;
	
private:
    SceneManager *mSceneMgr;

    // time of day in seconds. between 0 and secondsInADay
    Real timeOfDay;
    Real starMoveTime;
    // time in seconds taken to cycle through 1 day
    Real secondsInADay; 

    // parametners to pass to sky shaders
    GpuProgramParametersSharedPtr skyVertParams, skyFragParams;
    GpuProgramParametersSharedPtr starFragParams, cloudsFragParams, cloudsVertParams;
    
    // The time left until next toggle
    Real mToggle;
    // Toggle values
    bool snow;
    bool fog;
	bool flyMode;
    bool timeLapseMode;
    bool lensFlare;
    
    ArcticSnowManager *arcticSnow;
    SceneNode *weatherNode;
    SceneNode *sunMoveAxis;
    SceneNode *sunNode;
    SceneNode *StarPlaneNode, *CloudsPlaneNode;
    //Vector3 sunPosition;
    Light* sunLight;
    
};





/********************************************************************************
 * Overwriting ExampleApplication
 ********************************************************************************/

//class ArcticExplorerApplication : public TerrainApplication  // not ready yet
class ArcticExplorerApplication : public ExampleApplication
{
public:
    ArcticExplorerApplication()
    {
    }
    ~ArcticExplorerApplication() 
    {
	        delete raySceneQuery;
    }

private:
    OIS::Keyboard *mKeyboard;
    OIS::InputManager *mInputManager;
    SkyWeatherFrameListener* mSkyListener;
    //Viewport* viewPort;
    
	// Override ExampleApplication, manually specify configuration
	bool configure()
	{
		// Show the configuration dialog and initialise the system
        // You can skip this and use root.restoreConfig() to load configuration
        // settings if you were sure there are valid ones saved in ogre.cfg
        if(mRoot->showConfigDialog())
        {
            // If returned true, user clicked OK so initialise
            // Here we choose to let the system create a default rendering window by passing 'true'
			
			// robs: Since there is no config dialog for Mac OS, specify configuration manually
			// and resave the file using these settings.
			mRoot->getRenderSystem()->setConfigOption("FSAA", "4");
			mRoot->getRenderSystem()->setConfigOption("Video Mode", "1440 x 900");
			mRoot->saveConfig();
			
            mWindow = mRoot->initialise(true);
            return true;
        }
        else
        {
            return false;
        }
	}
	
	// Note: this replaces the default function in the ExampleApplication
    void createCamera(void)
    {
		mCamera = mSceneMgr->createCamera("PlayerCam");

		mCamera->setPosition(Vector3(START_X,200,START_Z));
		mCamera->lookAt(Vector3(START_X*2,200,START_Z*2));
		mCamera->setNearClipDistance(0.5);
    }

    void createViewports(void)
    {
		// create viewport, entire window, set colour
		Viewport* viewPort = mWindow->addViewport(mCamera);
		viewPort->setBackgroundColour(ColourValue(0,0,0));
				
		// set camera aspect ratio - match that of viewport
		mCamera->setAspectRatio(Real(viewPort->getActualWidth()) / Real(viewPort->getActualHeight()));
    }


    void chooseSceneManager(void)
    {
		mSceneMgr = mRoot->createSceneManager(ST_EXTERIOR_CLOSE, "terrainManager");
		mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);
    }

    void createScene(void)
    {
        //mSceneMgr->setAmbientLight( ColourValue(0,0,0) );
		mSceneMgr->setAmbientLight(ColourValue(0.7, 0.8, 1.0)); // robs: This colour is kind of nice
             
		// Set terrain
		mSceneMgr->setWorldGeometry("arcex_terrain.cfg");
		ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

        // Set sky plane
        // ArcticSkyMaterial includes a glsl vertex&fragment shader
		Plane plane;
		plane.d = 1000;
		plane.normal = Vector3::NEGATIVE_UNIT_Y;
		mSceneMgr->setSkyPlane(true, plane, "ArcticSkyMaterial", 1500, 20, true, 0.3f, 150, 150);

		// For terrain clamping in "walk" mode
        raySceneQuery = mSceneMgr->createRayQuery(
            Ray(mCamera->getPosition(), Vector3::NEGATIVE_UNIT_Y));
        
    }
    void createFrameListener(void)
    {
        mFrameListener = new SkyWeatherFrameListener(mWindow, mCamera, mSceneMgr);
		mRoot->addFrameListener(mFrameListener);

		// turn on bloom
		Ogre::Viewport* viewPort = mWindow->getViewport(0);
	
		CompositorManager::getSingleton().addCompositor(viewPort, "Bloom");
        CompositorManager::getSingleton().setCompositorEnabled(viewPort, "Bloom", true);
	
//		CompositorManager::getSingleton().addCompositor(viewPort, "Sharpen Edges");
//      CompositorManager::getSingleton().setCompositorEnabled(viewPort, "Sharpen Edges", true);
	}
};


 

/********************************************************************************
 * Main program
 ********************************************************************************/

#if OGRE_PLATFORM == PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
int main(int argc, char **argv)
#endif
{
    try
    {
        ArcticExplorerApplication app;
        app.go();
    }
    catch(Exception& e)
    {
#if OGRE_PLATFORM == PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        MessageBoxA(NULL, e.getFullDescription().c_str(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
        fprintf(stderr, "An exception has occurred: %s\n",
            e.getFullDescription().c_str());
#endif
    }

    return 0;
}

