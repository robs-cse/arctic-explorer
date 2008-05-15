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

        // Sky plane shader
        MaterialPtr mat = (MaterialPtr)MaterialManager::getSingleton().getByName("ArcticSkyMaterial");
        skyParams = mat->getTechnique(0)->getPass(0)->getVertexProgramParameters();   
        skyParams->setNamedConstant("timeOfDay", timeOfDay);


        // Sun currently just moves up and down. Work out how to get sun to go in a circular path
        sunNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("SunNode");
        sunNode->translate(Vector3(800.0, 0.0, 0.0));

        skyParams->setNamedConstant("sunPosition", sunNode->getWorldPosition());
        //        Entity *ent1 = mSceneMgr->createEntity( "Robot", "robot.mesh" );
        //        sunNode->attachObject( ent1 ); // Should attach a sun shader or something to this.
        
    }

    bool frameStarted(const FrameEvent& evt)
    {
        mKeyboard->capture();
        
        // Cycling through the hours in a day
        timeOfDay += evt.timeSinceLastFrame;
        sunNode->translate(Vector3(0.0, 0.1, 0.0));

        if (timeOfDay >= secondsInADay)
        {
            timeOfDay = 0.0;
            sunNode->setPosition(Vector3(800.0, 0.0, 0.0));
        }
        // Passing parameters to the sky vertex shader
        skyParams->setNamedConstant("timeOfDay", timeOfDay/(secondsInADay));
        skyParams->setNamedConstant("sunPosition", sunNode->getWorldPosition());
        skyParams->setNamedConstant("camPosition", mCamera->getDerivedPosition());
        

        
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

        return true;
    }

private:
    // The time left until next toggle
    Real mToggle;
    SceneManager *mSceneMgr;

    // time of day in seconds. between 0 and secondsInADay
    Real timeOfDay;
    // time in seconds taken to cycle through 1 day
    Real secondsInADay; 
    // parametners to pass to sky shader (vertex shader)
    GpuProgramParametersSharedPtr skyParams;
    
    bool summerWinter;
    bool snow;
    bool fog;
    
    SceneNode *sunNode;
    Vector3 sunPosition;
    
    
    
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
        mSceneMgr->setAmbientLight( ColourValue(0.2,0.2,0.2) );

        // Set terrain
		mSceneMgr->setWorldGeometry("terrain.cfg");
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
        // Create the FrameListener (default one from example frame listener)
        mFrameListener = new ExampleFrameListener(mWindow, mCamera, false, false);
		mRoot->addFrameListener(mFrameListener);

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

