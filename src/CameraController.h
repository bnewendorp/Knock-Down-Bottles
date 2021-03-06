/*
 *  CameraController.h
 *  Hybrid Template
 *
 *  Created by Kenneth Kopecky on 8/17/09.
 *  Copyright 2009 Orange Element Studios. All rights reserved.
 *
 */


//a little controller for controlling a user or other sort of interactive camera


#ifndef KENTROLLERDAWTH
#define KENTROLLERDAWTH


//first, a little vector class to help us out
class Vec3
{
	public:
	Vec3()	{x = y = z = 0;}
	Vec3(float xx, float yy, float zz)	{x = xx; y = yy; z = zz;}
	void normalize()	{float len = x * x + y * y + z * z; len = sqrtf(len); x /= len; y /= len; z /= len;}
	float length()		{return sqrtf(x*x + y*y + z*z);}
	float length2()		{return (x*x + y*y + z*z);}		//lengthSquared
	Vec3 operator +(Vec3 v)	{return Vec3(x + v.x, y + v.y, z + v.z);}
	void operator += (Vec3 v) {x += v.x; y += v.y; z += v.z;};
	Vec3 operator -(Vec3 v)	{return Vec3(x - v.x, y - v.y, z - v.z);}
	Vec3 operator / (float m)	{return Vec3(x / m, y / m, z / m);}
	Vec3 operator + (float m)	{return Vec3(x + m, y + m, z + m);}
	Vec3 operator - (float m)	{return Vec3(x - m, y - m, z - m);}
	Vec3 operator * (float m)	{return Vec3(x * m, y * m, z * m);}
	float dot(Vec3 v)			{return x * v.x + y * v.y + z * v.z;}
	float x;
	float y;
	float z;
	float& operator[](int n)
	{
		if (n == 0) return x;
		if (n == 1) return y;
		if (n==2) return z;
		return x;
	}	
	
	float* ptr()
	{
		static float nums[3] = {x, y, z};
		nums[0] = x;
		nums[1] = y;
		nums[2] = z;
		return nums;
	}
	
	Vec3 cross(Vec3 p)
	{
		Vec3 v;
		v[0] = -p.y * z + p.z * y;
		v[1] = -p.z * x + p.x * z;
		v[2] = -p.x * y + p.y * x;
		return v;
	}
	
	Vec3 unProjected()		//unproject the vector by the current GL matrices
	{
			//get gluUnProject ready
		GLint viewPort[4];
		glGetIntegerv(GL_VIEWPORT, viewPort);
		double mod[16];
		glGetDoublev(GL_MODELVIEW_MATRIX, mod);
		double proj[16];
		glGetDoublev(GL_PROJECTION_MATRIX, proj);

		double wx=0;
		double wy=0;
		double wz=0;

		gluUnProject(x, y, z, 
			mod, proj, viewPort, &wx, &wy, &wz);
		return Vec3(wx,wy, wz);

	}
};

//tiny matrix class.
class KMatrix
{
public:
	KMatrix()	{makeIdentity();}
	KMatrix(float* ptr)	{for (int i=0; i <16; i++)	m[i] = ptr[i];}
	float m[16];
	void makeIdentity()	{for (int i=0; i<16; i++) m[i] =(i%5==0)?1:0;}

	//will only work if the matrix is orthogonal (ie no scaling)
	inline KMatrix getInverse();			//down below, cuz it's long
	//3x3 determinant
	inline float determ(	float a1, float a2, float a3,
					float b1, float b2, float b3,
					float c1, float c2, float c3);
	inline float determ(float a1, float a2,
				 float b1, float b2);
	//4x4 determinant
	inline float determ();
	void print()
	{
		printf("Matrix:\n");
		for (int i = 0; i < 4; i++)
			printf("%.2f	%.2f	%.2f	%.2f\n", m[i], m[i +4], m[i+8], m[i+12]);
	}
	void transpose()
	{
		KMatrix mat;
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				mat.m[i*4+j] = m[j*4+i];
		*this = mat;
	}
};

class CameraController
{
public:
	static const int CURRENT_VIEW = -1;
	enum ViewMode {FPS_VIEW, ORBIT_VIEW, NUM_VIEW_MODES};
	CameraController()
	{
		mOrbitYaw = 45;
		mOrbitZoom = 1.0;
		mOrbitPitch = 45;
		
		mFPPos = Vec3(0, 5.5, 12);
		mFPYaw = 0;
		mFPPitch = 0;
		mFPMaxSpeed = 10.0;
		mUp = mDown = mLeft = mRight = mStrafeLeft = mStrafeRight;
		
		mFPPeekX = 0;
		mFPPeekY = 0;
		mViewMode = FPS_VIEW;
		mRightClick = false;
	}

	void update(float dt)
	{
		
		Vec3 targetVelocity = Vec3((mStrafeRight - mStrafeLeft), 0, (mUp - mDown)) * mFPMaxSpeed;
		targetVelocity = getFPForward() * targetVelocity.z + getFPRight() * targetVelocity.x;
		mFPVelocity = mFPVelocity * (1.0 - 5.0 * dt) + targetVelocity * 5.0 * dt;

		//return the view to forward when we move
		if (targetVelocity.length())
		{	
			mFPPeekX *= 1.0 - dt * 1.5;
			mFPPeekY *= 1.0 - dt * 1.5;
			
		}
		mFPPos = mFPPos + mFPVelocity * dt;
		
		float omega = 3.0 * (mLeft - mRight);
		mFPYaw += omega * dt;

	}
	
	//get the modelview matrix necessary to replicate the view in question
	KMatrix getViewMatrix(int mode = CURRENT_VIEW)		//
	{
		if (mode == CURRENT_VIEW) mode = mViewMode;
		KMatrix view;
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			glLoadIdentity();
			switch(mode)
			{
				case FPS_VIEW:
					glRotatef(-mFPPeekY, 1, 0, 0);
					glRotatef(mFPPeekX, 0, 1, 0);

					glRotatef(-mFPPitch * 180.0 / 3.142, 1, 0, 0);
					glRotatef(-mFPYaw*180.0/3.142, 0, 1, 0);
					glTranslatef(-mFPPos.x, -mFPPos.y, -mFPPos.z);
				break;
				
				case ORBIT_VIEW:
					glTranslatef(0, 0, -30 * mOrbitZoom);
					glRotatef(mOrbitPitch, 1, 0, 0);
					glRotatef(mOrbitYaw, 0, 1, 0);
				break;

			}
			
			glGetFloatv(GL_MODELVIEW_MATRIX, view.m);
			
		glPopMatrix();
		return view;
	}
	
	void mouseMotion(int x, int y)
	{
		float dx = x - mLastMouseX;
		float dy = y - mLastMouseY;
		if (mRightClick)
		{
			mOrbitZoom *= 1.0 + dy * 0.01;
			if (mOrbitZoom < 0.1)   mOrbitZoom = 0.1;
			if (mOrbitZoom > 100.0) mOrbitZoom = 100.0;
			//printf("zoom:  %f\n", gZoom);
		}
		else
		{
			if (mViewMode == FPS_VIEW)
			{
				mFPPeekX += 0.1 * dx;
				mFPPeekY -= 0.1 * dy;
			}
			else
			{
				mOrbitYaw += 0.1 * dx;
				mOrbitPitch -= 0.1 * dy;
			}
		}
		mLastMouseX = x;
		mLastMouseY = y;
	}
	
	
	void passiveMouse(int x, int y)
	{
		mLastMouseX = x;
		mLastMouseY = y;
	}
	
	void mouseClick(int button, int state, int x, int y)
	{
		mLastMouseX = x;
		mLastMouseY = y;
		if (button == GLUT_RIGHT_BUTTON)
			mRightClick = true;
		else
			mRightClick = false;
	}
	
	KMatrix getWandMatrix(Vec3 offset)
	{
//		return getViewMatrix(FPS_VIEW).m;
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			glLoadIdentity();
			glMultMatrixf(getViewMatrix(FPS_VIEW).getInverse().m);
			glTranslatef(offset.x, offset.y, offset.z);			
			KMatrix w;
			glGetFloatv(GL_MODELVIEW_MATRIX, w.m);
		glPopMatrix();
		return w;
	}
	
	
	void cycleViewMode()	{mViewMode = (mViewMode+1)%NUM_VIEW_MODES;}
	int getViewMode()		{return mViewMode;}
	int mViewMode;
	void setUp(bool up)	{mUp = up;}
	void setDown(bool down) {mDown = down;}
	void setLeft(bool left)	{mLeft = left;}
	void setRight(bool r)	{mRight = r;}
	void setStrafeLeft(bool l)	{mStrafeLeft = l;}
	void setStrafeRight( bool r)	{mStrafeRight = r;}
	Vec3 getFPForward(){return Vec3(cosf(mFPPitch)*-cosf(mFPYaw - 3.14158*0.5), sinf(mFPPitch), cosf(mFPPitch)*sinf(mFPYaw - 3.14158*0.5));}
	Vec3 getFPRight()	{return Vec3(cosf(mFPYaw), 0, -sinf(mFPYaw));}
	Vec3 getFPUp()	{return getFPRight().cross(getFPForward());}
	void setPeekAngles(float x, float y)	{mFPPeekX = x; mFPPeekY = y;}
	float getPeekX()	{return mFPPeekX;}
	float getPeekY()	{return mFPPeekY;}
	float mOrbitYaw, mOrbitPitch, mOrbitZoom;
	float mFPYaw, mFPPitch;
	Vec3 mFPPos, mFPVelocity;
	float mFPMaxSpeed;
	float mFPPeekX, mFPPeekY;
	int mLastMouseX, mLastMouseY;
	bool mRightClick;
	//first person controls
	bool mUp, mDown, mLeft, mRight, mStrafeLeft, mStrafeRight;
};


	inline KMatrix KMatrix::getInverse()
	{
		KMatrix inv;
		float det1=determ();
		if (fabs(det1)<1e-5)
		{
		//	printf("Singular matrix (det = %f)!  Returning identity.\n", det1);
		//	inv.loadIdentity();
			return *this;
		}
		//these make the code easier
		//to manage
		float a1, a2, a3, a4,
			b1, b2, b3, b4,
			c1, c2, c3, c4,
			d1, d2, d3, d4;
		transpose();
		a1=m[0];
		b1=m[1];
		c1=m[2];
		d1=m[3];
		a2=m[4];
		b2=m[5];
		c2=m[6];
		d2=m[7];
		a3=m[8];
		b3=m[9];
		c3=m[10];
		d3=m[11];
		a4=m[12];
		b4=m[13];
		c4=m[14];
		d4=m[15];
		transpose();
		det1=1.0/det1;
		
		inv.m[0]  =   determ(b2, b3, b4, c2, c3,
								c4, d2, d3, d4) * det1;
		inv.m[1] = - determ(a2, a3, a4, c2, c3, c4,
								d2, d3, d4) * det1;
		inv.m[2]  =   determ(a2, a3, a4, b2, b3, b4,
								d2, d3, d4) * det1;
		inv.m[3]  = - determ(a2, a3, a4, b2, b3, b4,
								c2, c3, c4) * det1;
		inv.m[4]  = - determ(b1, b3, b4, c1, c3, c4,
								d1, d3, d4) * det1;
		inv.m[5]  =   determ(a1, a3, a4, c1, c3, c4,
								d1, d3, d4) * det1;
		inv.m[6]  = - determ(a1, a3, a4, b1, b3, b4,
								d1, d3, d4) * det1;
		inv.m[7]  =   determ(a1, a3, a4, b1, b3, b4,
								c1, c3, c4) * det1;

		inv.m[8]  =   determ(b1, b2, b4, c1, c2, c4,
								d1, d2, d4) * det1;
		inv.m[9]  = - determ(a1, a2, a4, c1, c2, c4,
								d1, d2, d4) * det1;
		inv.m[10]  =   determ(a1, a2, a4, b1, b2, b4,
								d1, d2, d4) * det1;
		inv.m[11]  = - determ(a1, a2, a4, b1, b2, b4,
								c1, c2, c4) * det1;
		inv.m[12]  = - determ(b1, b2, b3, c1, c2, c3,
								d1, d2, d3) * det1;
		inv.m[13]  =   determ(a1, a2, a3, c1, c2, c3,
								d1, d2, d3) * det1;
		inv.m[14]  = - determ(a1, a2, a3, b1, b2, b3,
								d1, d2, d3) * det1;
		inv.m[15]  =   determ(a1, a2, a3, b1, b2, b3,
								c1, c2, c3) * det1;

		return inv;
	}

		//3x3 determinant
	inline float KMatrix::determ(	float a1, float a2, float a3,
					float b1, float b2, float b3,
					float c1, float c2, float c3)
	{
		//downright minus downleft
		float det=0;
		det+=a1*b2*c3;
		det+=a2*b3*c1;
		det+=a3*b1*c2;
		det-=a1*b3*c2;
		det-=a2*b1*c3;
		det-=a3*b2*c1;
		return det;
	}
	inline float KMatrix::determ(float a1, float a2,
				 float b1, float b2)
	{
		float det = 0;
		det += a1 * b2;
		det -= a2 * b1;
		return det;
	}
	//4x4 determinant
	inline float KMatrix::determ()
	{
		float det=0;
		det+=m[0]*determ(m[5], m[9], m[13],
						m[6], m[10], m[14],
						m[7], m[11], m[15]);
		det-=m[1]*determ(m[4], m[8], m[12],
						m[6], m[10], m[14],
						m[7], m[11], m[15]);
		det+=m[2]*determ(m[4], m[8], m[12],
							m[5], m[9], m[13],
							m[7], m[11], m[15]);
		det-=m[3]*determ(m[4], m[8], m[12],
							m[5], m[9], m[13],
							m[6], m[10], m[14]);
		return det;
	}
#endif

