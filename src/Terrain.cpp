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
// #include "TerrainApplication.h"  // not working yet


#define sunAxisRadius 600


/********************************************************************************
 * Sky and weather frame listener
 ********************************************************************************/
 
class SkyWeatherFrameListener : public ExampleFrameListener
{
public:
    SkyWeatherFrameListener(RenderWindow* win, Camera* cam, SceneManager *sceneMgr)
        : ExampleFrameListener(win, cam, false, false)
    {
        mSceneMgr = sceneMgr;
        mToggle = 0.0;
        
        // Set initial time of day
        // Time of day goes in cycles representing the 24 hours in a day
        timeOfDay = 0.0;
        secondsInADay = 4.0; // default seconds in a day

        // Parameter setting for sky plane shader
        MaterialPtr mat = (MaterialPtr)MaterialManager::getSingleton().getByName("ArcticSkyMaterial");
        skyVertParams = mat->getTechnique(0)->getPass(0)->getVertexProgramParameters();
        skyFragParams = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();


        // Sun currently just moves up and down. Work out how to get sun to go in a circular path
        sunMoveAxis = mSceneMgr->getRootSceneNode()->createChildSceneNode("SunMoveAxis");
        sunMoveAxis->translate(Vector3(2000.0, 0.0, 800.0));

        sunNode = sunMoveAxis->createChildSceneNode("SunNode");
        sunNode->translate(Vector3(0.0, sunAxisRadius-10, 0.0)); 
        // Note the -10 is a hack. Otherwise the sky colout texture doesn't seem to work correctly
        
        // Attach a light to the sun node
        sunLight = mSceneMgr->createLight("MainLight");
		sunLight->setType(Light::LT_POINT);
        sunNode->attachObject(sunLight);
  
        // TODO: change the colour of the light depending on the position of the sun      
        sunLight->setDiffuseColour(0.4, 0.4, 0.4);
        sunLight->setSpecularColour(0.4, 0.4, 0.4);
       
//		sunLight->setType(Light::LT_DIRECTIONAL);
//        sunLight->setDirection(0,-0.15,-1);			// TODO: Should reference variable for sun direction

    }

    bool frameStarted(const FrameEvent& evt)
    {
		if( ExampleFrameListener::frameStarted(evt) == false )
			return false;

        mKeyboard->capture();
        
        // Cycling through the hours in a day
        timeOfDay += evt.timeSinceLastFrame;
        sunMoveAxis->pitch(Degree(180/secondsInADay*evt.timeSinceLastFrame));

        if (timeOfDay >= secondsInADay)
        {
            timeOfDay = 0.0;
        }
        // Passing parameters to the sky vertex shader
        skyVertParams->setNamedConstant("sunPosition", sunNode->getWorldPosition());
        skyVertParams->setNamedConstant("camPosition", mCamera->getWorldPosition());
        
        Real sunHeight = (sunNode->getWorldPosition().y + sunAxisRadius - sunMoveAxis->getWorldPosition().y)/(2*sunAxisRadius);
        skyFragParams->setNamedConstant("sunHeightRel", sunHeight);

        
        // key can only be pressed once every 0.5  (1 second?)
		mToggle -= evt.timeSinceLastFrame;
        // ToDo:
        // toggle fog on/off
        // toggle snow on/off
        // toggle winter/summer
        // set secondsInADay (speed up mode)

        // Example, this not ready yet // change the position of the sun
        if ((mToggle < 0.0f ) && mKeyboard->isKeyDown(OIS::KC_1))
        {
            mToggle = 0.5f;
            summerWinter = !summerWinter;
        }
	
		/* Rob added this to see if flare is fired */
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
	//SceneManager *mSceneMgr;
	SceneNode *flareNode;
	Vector3 flareVel;
	
private:
    // The time left until next toggle
    Real mToggle;
    SceneManager *mSceneMgr;

    // time of day in seconds. between 0 and secondsInADay
    Real timeOfDay;
    // time in seconds taken to cycle through 1 day
    Real secondsInADay; 
    // parametners to pass to sky shaders
    GpuProgramParametersSharedPtr skyVertParams;
    GpuProgramParametersSharedPtr skyFragParams;
    
    bool summerWinter;
    bool snow;
    bool fog;
    
    SceneNode *sunMoveAxis;
    SceneNode *sunNode;
    Vector3 sunPosition;
    Light* sunLight;
    
    
    void adjustFog()
    {
    }
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
    }

private:
    OIS::Keyboard *mKeyboard;
    OIS::InputManager *mInputManager;
    SkyWeatherFrameListener* mSkyListener;
    
	// Note: this replaces the default function in the ExampleApplication
    void createCamera(void)
    {
		mCamera = mSceneMgr->createCamera("PlayerCam");

		mCamera->setPosition(Vector3(0,200,0));
		mCamera->lookAt(Vector3(1,200,0));
		mCamera->setNearClipDistance(5);
    }

    void createViewports(void)
    {
		// create viewport, entire window, set colour
		Viewport* vp = mWindow->addViewport(mCamera);
		vp->setBackgroundColour(ColourValue(0,0,0));
				
		// set camera aspect ratio - match that of viewport
		mCamera->setAspectRatio(Real(vp->getActualWidth()) / Real(vp->getActualHeight()));
    }


    void chooseSceneManager(void)
    {
		mSceneMgr = mRoot->createSceneManager(ST_EXTERIOR_CLOSE, "terrainManager");
    }

    void createScene(void)
    {
        //mSceneMgr->setAmbientLight( ColourValue(0.2,0.2,0.2) );
		mSceneMgr->setAmbientLight(ColourValue(0.7, 0.8, 1.0)); // robs: This colour is kind of nice
        
		// Set terrain
		mSceneMgr->setWorldGeometry("arcex_terrain.cfg");
		ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

        // Set sky plane
        // ArcticSkyMaterial includes a glsl vertex&fragment shader
		Plane plane;
		plane.d = 1000;
		plane.normal = Vector3::NEGATIVE_UNIT_Y;
		mSceneMgr->setSkyPlane(true, plane, "ArcticSkyMaterial", 1500, 40, true, 0.3f, 150, 150);
        

                
        // Add more here
    }
    void createFrameListener(void)
    {
        mFrameListener = new SkyWeatherFrameListener(mWindow, mCamera, mSceneMgr);
		mRoot->addFrameListener(mFrameListener);

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

