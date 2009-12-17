
#ifdef WIN32
#include <windows.h>
#endif
#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include <stdarg.h>
#include "BDScene.h"
#include "OSGNavigatorGLUT.h"

#include <gmtl/Vec.h>
#include <gmtl/Coord.h>
#include <gmtl/Xforms.h>
#include <gmtl/Math.h>
#include <gmtl/Matrix.h>
#include <gmtl/Point.h>
#include <gmtl/MatrixOps.h>
#include <gmtl/VecOps.h>
#include <gmtl/Xforms.h>
#include <gmtl/Output.h>
#include <gmtl/Generate.h>

int screenWidth = 1024;
int screenHeight = 768;
float gYaw = 45.0;
float gPitch = 45.0;
float gZoom = 1.0;
bool gRightClick = false;
bool gOrbitCam;		//are we using the orbit cam or the C6 cam?
float aspect;
bool gPaused = false;
bool gFullScreen;
int gMouseX, gMouseY;

bool gShowC6 = true;

OsgNavigatorGLUT _osgNavigator;
float _navSpeed = 1.0;
float _rotSpeed = 22;
bool _moveUp = false;
bool _moveDown = false;
float _totalTime = 0.0;

pthread_t _socketThread;


#include "CameraController.h"

CameraController gCamera;


osg::ref_ptr<osgViewer::Viewer> viewer;
osg::observer_ptr<osgViewer::GraphicsWindow> window;

void drawStringOnScreen(int x, int y, const char* format, ...)
{
	char stringData[1024];
	va_list args;
	va_start(args, format);
	vsprintf(stringData, format, args);
	glViewport(0, 0, screenWidth, screenHeight);
	glPushMatrix();
		glLoadIdentity();
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
			glLoadIdentity();
			glOrtho(0, screenWidth, 0, screenHeight, -1, 1);

			//glDisable(GL_LIGHTING);
			glDisable(GL_TEXTURE_2D);
			glRasterPos3i(x, screenHeight - y, 0);
			for ( int i=0; i<1024 && stringData[i] != 0; i++ )
				glutBitmapCharacter( GLUT_BITMAP_HELVETICA_12, stringData[i] );
			//glEnable(GL_LIGHTING);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	glPopMatrix();


}

void drawStatus()
{
	glColor3f(1,1,1);
	static float t=0;
	static float fps=2;
	static float deltat=.2;
	static int reps=0;
	float tnow=glutGet(GLUT_ELAPSED_TIME);
	reps++;
//printf("%f\n", tnow - t);
	if (tnow - t > 500)		//update every 500 ms
	{
	//	printf("upd\n");
		//float tnow=glutGet(GLUT_ELAPSED_TIME);
		deltat=tnow-t;
		t=tnow;
		fps=1.f * reps/deltat*1000;
		reps = 0;
	}
	
		drawStringOnScreen(20, 20, "Frame Rate:             %.2f", fps);
	
	glColor3f(1, 0, 0);
	

}

void drawTheFPSGuy()
{
	float red[] = {1, 0, 0, 1};
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, red);
	glPushMatrix();
		glTranslatef(gCamera.mFPPos.x, gCamera.mFPPos.y, gCamera.mFPPos.z);
		glRotatef(gCamera.mFPYaw * 180.0 / 3.142, 0, 1, 0);
		glRotatef(gCamera.mFPPitch * 180.0 / 3.1412, 1, 0, 0);
		
		//glEnable(GL_LIGHTING);
		
		//head!
		glPushMatrix();
			glTranslatef(-0, -0.2, 0);
			glutSolidSphere(0.4, 15, 15);
		glPopMatrix();

		//body!
		glPushMatrix();
			glTranslatef(0, -1.5, 0);
			glScalef(1.0, 2.0, 0.5);
			glutSolidCube(1.0);
		glPopMatrix();
		
		//legs!
		glPushMatrix();
			glTranslatef(-0.3, 1.5 - 5.5, 0);
			glScalef(0.3, 3.0, 0.3);
			glutSolidCube(1.0);
			glTranslatef(0.6/0.3, 0, 0);
			glutSolidCube(1.0);
		glPopMatrix();

		//axes!
		//glDisable(GL_LIGHTING);
		glBegin(GL_LINES);
			float length = 2.0;
			glColor3f(1, 0, 0);
			glVertex3f(0, 0, 0);
			glVertex3f(length, 0, 0);
			glColor3f(0, 1, 0);
			glVertex3f(0, 0, 0);
			glVertex3f(0, length, 0);
			glColor3f(0, 0, 1);
			glVertex3f(0, 0, 0);
			glVertex3f(0, 0, -length);
		glEnd();
	glPopMatrix();
	
}

void setLight(float x, float y, float z)
{
	//glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	float lightPosition[] = {x, y, z, 1};
	glLightfv (GL_LIGHT0, GL_POSITION, lightPosition);
}



void display(void)
{
    // update and render the scene graph
	osg::Camera* currentCam = viewer->getCamera();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	currentCam->setProjectionMatrixAsPerspective(60, aspect, 0.1, 5000.0);
//	currentCam->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
	gluPerspective(60, aspect, 0.1, 5000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//apply our chosen matrix!	
	KMatrix view = gCamera.getViewMatrix();

	glLoadMatrixf(view.m);

	//use this time to get the head and wand matrices and pass them to our app.  
	//this only works because GLUT uses a single OpenGL context.  if you send matrices to a C6 
	//app while you're drawing, bad things will happen	
	
	//which one should we send?  If we're view-aligning particles, it's useful to send whichever one we're actually using for our camera
	//but if we're using it to track the user's head and interact with it, we should send the one representing the user...the first person view matrix
	

	//here's a cool trick!
	//the head matrix we get from Juggler is actually the INVERSE of the modelview matrix that would be needed
	//for a camera at that same position
	//Right now, we can grab GL's modelview matrix and use that, but matrix inversions are hard
	//Fortunately, since we're not doing any scaling, our modelview matrix is an orthogonal matrix
	//this means that its inverse is the same as its transpose!  so....
	BDScene::instance().setHeadMatrix(osg::Matrixf(gCamera.getViewMatrix(CameraController::FPS_VIEW).getInverse().m));
	

	//next we'll send the app a wand matrix based on the mouse position

	KMatrix wandMat = gCamera.getWandMatrix(Vec3(-1.0 + 2.0 * gMouseX / screenWidth, -1.0 + 2.0 * gMouseY / screenHeight, -2));
	BDScene::instance().setWandMatrix(osg::Matrixf(wandMat.m));

	currentCam->setViewMatrix(osg::Matrixf(view.m));
    if (viewer.valid()) viewer->frame();
	
	//go back to our view so we can draw some extras
	glViewport(0, 0, screenWidth, screenHeight);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	osg::Matrixf proj = 	currentCam->getProjectionMatrix();
	glMultMatrixf(proj.ptr());
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(view.m);
//	setLight(50, 50, 50);
//	glEnable(GL_DEPTH_TEST);
//	if (gCamera.getViewMode() != CameraController::FPS_VIEW)
//		drawTheFPSGuy();
//	//glDisable(GL_LIGHTING);
	drawStatus();
	
    // Swap Buffers
    glutSwapBuffers();
}




void reshape( int w, int h )
{
	screenWidth = w;
	screenHeight = h;
	aspect = 1.0 * screenWidth / screenHeight;
	printf("reshape to %i, %i\n", screenWidth, screenHeight);
	glViewport(0, 0, screenWidth, screenHeight);
    // update the window dimensions, in case the window has been resized.
    if (window.valid()) 
    {
        window->resized(window->getTraits()->x, window->getTraits()->y, w, h);
        window->getEventQueue()->windowResize(window->getTraits()->x, window->getTraits()->y, w, h );
    }
}

void mousebutton( int button, int state, int x, int y )
{
  	gCamera.mouseClick(button, state, x, y);
	y = screenHeight - y;
	gMouseX = x;
	gMouseY = y;
	if (button == GLUT_RIGHT_BUTTON)
		gRightClick = true;
	else
		gRightClick = false;


}


void mouseMotion(int x, int y)
{
	gCamera.mouseMotion(x, y);
	y = screenHeight - y;
	gMouseX = x;
	gMouseY = y;
}

void mouser(int button, int state, int x, int y)
{
	y = screenHeight - y;
	gMouseX = x;
	gMouseY = y;
	if (button == GLUT_RIGHT_BUTTON)
		gRightClick = true;
	else
		gRightClick = false;

	glutPostRedisplay();
}

void passiveMotion(int x, int y)
{
	gCamera.passiveMouse(x, y);
	y = screenHeight - y;
	gMouseX = x;
	gMouseY = y;
}


void keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
		//handle key input	
		case '1': aq::KVReflector::instance()->didUpdateValueForKey((double)1.0, "Mass_1");	break;
		case '2': aq::KVReflector::instance()->didUpdateValueForKey((double)1.0, "Mass_2");	break;
		case '3': aq::KVReflector::instance()->didUpdateValueForKey((double)1.0, "Mass_3");	break;
		case 'R': aq::KVReflector::instance()->didUpdateValueForKey((double)1.0, "Reset_Scene");	break;
		case 'b': aq::KVReflector::instance()->didUpdateValueForKey((double)1.0, "Drop ball");	break;
			
		case 'q': gCamera.setStrafeLeft(true);	break;
		case 'w': gCamera.setUp(true);	break;
		case 'e': gCamera.setStrafeRight(true);	break;
		case 'a': gCamera.setLeft(true);	break;
		case 's': gCamera.setDown(true);	break;
		case 'd': gCamera.setRight(true);	break;
	
		case 'c': gShowC6 = !gShowC6; break;
		case ' ':	BDScene::instance().buttonInput(0, true);	break;		//space bar controls the main wand button
	
		case 'p':	gPaused = !gPaused;	break;		//pause/unpause
		//switch input modes with tab
		case '	':	gCamera.cycleViewMode();	break;
		case 'f':
			if (!gFullScreen)	glutFullScreen();
			else glutReshapeWindow(1024, 768);
			gFullScreen = !gFullScreen;
			break;
		case 't':
			_moveUp = true;
			glutForceJoystickFunc();
			break;
		case 'r':
			_moveDown = true;
			glutForceJoystickFunc();
			break;
		case 27:	exit(1);	break;
		default:
            if (window.valid())
            {
                window->getEventQueue()->keyPress( (osgGA::GUIEventAdapter::KeySymbol) key );
                window->getEventQueue()->keyRelease( (osgGA::GUIEventAdapter::KeySymbol) key );
            }
            break;
    }
	glutPostRedisplay();
}

void keyboardUp(unsigned char key, int x, int y)
{
	switch(key)
	{
	
	}


}

void keySpecial(int key, int x, int y)
{
	switch(key)
	{
		case GLUT_KEY_LEFT: aq::KVReflector::instance()->didUpdateValueForKey((double)1.0, "Aim_Left");	break;
		case GLUT_KEY_RIGHT: aq::KVReflector::instance()->didUpdateValueForKey((double)1.0, "Aim_Right");	break;
		case GLUT_KEY_DOWN: aq::KVReflector::instance()->didUpdateValueForKey((double)1.0, "Aim_Down");	break;
		case GLUT_KEY_UP: aq::KVReflector::instance()->didUpdateValueForKey((double)1.0, "Aim_Up");	break;
			
		default: break;

	}
	

}

void timer(int bl)
{
	static int lastTime = glutGet(GLUT_ELAPSED_TIME);
	int thisTime = glutGet(GLUT_ELAPSED_TIME);
	float dt = 0.001 * ((float) thisTime - lastTime);
	lastTime = thisTime;
	_totalTime += dt;
	gCamera.update(dt);
	
	if (!gPaused)
		BDScene::instance().update(dt);		//send the timestep to the app class
		
	viewer->getCamera()->setClearColor(osg::Vec4f(0, 0, 0, 1.0));
		
	glutTimerFunc(0, timer, 0);
	glutPostRedisplay();

}

void keyUpBoard(unsigned char key, int x, int y)
{

	switch(key)
	{
		//handle key input
		case 'q': gCamera.setStrafeLeft(false);	break;
		case 'w': gCamera.setUp(false);	break;
		case 'e': gCamera.setStrafeRight(false);	break;
		case 'a': gCamera.setLeft(false);	break;
		case 's': gCamera.setDown(false);	break;
		case 'd': gCamera.setRight(false);	break;
		
		case ' ':	BDScene::instance().buttonInput(0, false);	break;		//space bar controls the main wand button
	}

	glutPostRedisplay();
}


void gamePadHandler(unsigned int buttonMask, int x, int y, int z)
{
	// Button 1
	static bool button1Pressed = false;
	if (buttonMask & 1)
	{
		if (button1Pressed == false)
			BDScene::instance().buttonInput(1, true);
		button1Pressed = true;
	}
	else
	{
		BDScene::instance().buttonInput(1, false);
		button1Pressed = false;
	}
	
	// Button 2
	static bool button2Pressed = false;
	if (buttonMask & 2)
	{
		if (button2Pressed == false)
			BDScene::instance().buttonInput(2, true);
		button2Pressed = true;
	}
	else
	{
		BDScene::instance().buttonInput(2, false);
		button2Pressed = false;
	}
	
	// Button 3
	static bool button3Pressed = false;
	if (buttonMask & 4)
	{
		if (button3Pressed == false)
			BDScene::instance().buttonInput(3, true);
		button3Pressed = true;
	}
	else
	{
		BDScene::instance().buttonInput(3, false);
		button3Pressed = false;
	}
	
	// Button 4
	static bool button4Pressed = false;
	if (buttonMask & 8)
	{
		if (button4Pressed == false)
			BDScene::instance().buttonInput(4, true);
		button4Pressed = true;
	}
	else
	{
		BDScene::instance().buttonInput(4, false);
		button4Pressed = false;
	}
	
	// Button 5
	static bool button5Pressed = false;
	if (buttonMask & 16)
	{
		if (button5Pressed == false)
			BDScene::instance().buttonInput(5, true);
		button5Pressed = true;
	}
	else
	{
		BDScene::instance().buttonInput(5, false);
		button5Pressed = false;
	}
	
	// Button 6
	static bool button6Pressed = false;
	if (buttonMask & 32)
	{
		if (button6Pressed == false)
			BDScene::instance().buttonInput(6, true);
		button6Pressed = true;
	}
	else
	{
		BDScene::instance().buttonInput(6, false);
		button6Pressed = false;
	}
	
	// Button 7
	static bool button7Pressed = false;
	if (buttonMask & 64)
	{
		if (button7Pressed == false)
			BDScene::instance().buttonInput(7, true);
		button7Pressed = true;
	}
	else
	{
		BDScene::instance().buttonInput(7, false);
		button7Pressed = false;
	}
	
	// Button 8
	static bool button8Pressed = false;
	if (buttonMask & 128)
	{
		if (button8Pressed == false)
			BDScene::instance().buttonInput(8, true);
		button8Pressed = true;
	}
	else
	{
		BDScene::instance().buttonInput(8, false);
		button8Pressed = false;
	}
	
	// Button 9
	static bool button9Pressed = false;
	if (buttonMask & 256)
	{
		if (button9Pressed == false)
			BDScene::instance().buttonInput(9, true);
		button9Pressed = true;
	}
	else
	{
		BDScene::instance().buttonInput(9, false);
		button9Pressed = false;
	}
	
	// Button 10
	static bool button10Pressed = false;
	if (buttonMask & 512)
	{
		if (button10Pressed == false)
			BDScene::instance().buttonInput(10, true);
		button10Pressed = true;
	}
	else
	{
		BDScene::instance().buttonInput(10, false);
		button10Pressed = false;
	}
	
	/*
	X is left/right range -1000 to 1000
	- left = -1000
	- right = 1000
	
	Y is forward/backward range 0 to 255
	- forward = 0
	- backward = 255
	
	Z is strafe left/right range -1000 to 1000
	- strafe left = -1000
	- strafe right = 1000
	*/
		
	// Convert the values to -0.5 to 0.5
	float axisX = (((float(x) + 1000) / 2000) - 0.5); // = axis0: l2r: -0.2 to 0.2
	float axisY = ((float(y) + 2000) / 2000) - 1.0;
	float axisZ = ((float(z) + 2000) / 2000) - 1.0;	// = axis3: l2r: -0.5 to 0.5
	
	// Fix for the drift since the normalization isn't quite perfect
	if (x == 3)
		axisX = 0.0;
	if (y == -3)
		axisY = 0.0;
	if (z == -3)
		axisZ = 0.0;

	// Set the up down value from the r and t keys
	float axisUpDown = 0.0;
	float step = 0.2;
	if (_moveUp == true)
	{
		axisUpDown = step;
		_moveUp = false;
	}
	else if (_moveDown == true)
	{
		axisUpDown = -step;
		_moveDown = false;
	}

	// Scale the values nicely
	axisX *= _navSpeed; axisY *= _navSpeed;
	
	// This ties rotation speed to navSpeed until navSpeed gets too high
	if (_navSpeed < 0.8)
		axisZ /= _rotSpeed * (1 - _navSpeed); 
	else
		axisZ /= _rotSpeed * (1 - 0.5); 
	
	// Only allow Yaw (rot y) for navigation
	gmtl::EulerAngleXYZf euler(0.0f, -axisZ, 0.0f);
	_osgNavigator.setRotationalVelocity(gmtl::makeRot<gmtl::Matrix44f>(euler));
	_osgNavigator.setVelocity(gmtl::Vec3f(axisX, axisUpDown, axisY));
	
	// Update the navigator matrices
	_osgNavigator.update(1.0);
	
	// Invert the osg navigator matrix
	gmtl::Matrix44f world_transform, currentPosition;
	currentPosition = _osgNavigator.getCurPos();
	gmtl::invertFull(world_transform, currentPosition);
	
	// Convert to osg matrix
	osg::Matrixf osg_current_matrix;
	osg_current_matrix.set(world_transform.getData());
	
	// Actually push out the new matrix
	BDScene::instance().setNavigationMatrix(osg_current_matrix);
}


int main( int argc, char **argv )
{
    glutInit(&argc, argv);

    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_ALPHA | GLUT_MULTISAMPLE );
    glutInitWindowPosition( 100, 100 );
    glutInitWindowSize( 800, 600 );
    glutCreateWindow( argv[0] );
    glutDisplayFunc(display);
    glutReshapeFunc( reshape );
    glutMouseFunc( mousebutton );
    glutMotionFunc( mouseMotion);
    glutPassiveMotionFunc(passiveMotion);
	glutKeyboardFunc( keyboard );
	glutKeyboardUpFunc(keyboardUp);
	glutSpecialFunc(keySpecial);
	glutKeyboardUpFunc(keyUpBoard);
	
	// Register the joystick
	glutJoystickFunc(gamePadHandler, 10);
	
	// Initialize the navigator
	_osgNavigator.init();

    // create the view of the scene.
    viewer = new osgViewer::Viewer;
    window = viewer->setUpViewerAsEmbeddedInWindow(100,100,800,600);
	BDScene::instance().setMaster(true);
	BDScene::instance().init();
    viewer->setSceneData(BDScene::instance().getRootNode());
	viewer->getCamera()->setClearColor(osg::Vec4f(0.0, 0.0, 0.0, 1.0));

    viewer->addEventHandler(new osgViewer::StatsHandler);
    viewer->realize();
	glutTimerFunc(100, timer, 0);
	
    glutMainLoop();
    
    return 0;
}
