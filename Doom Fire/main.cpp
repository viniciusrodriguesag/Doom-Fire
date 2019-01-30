/*
 *	Doom Fire
 *	main.cpp
 *
 *	
 *	by Vinicius Rodrigues
 *	30/01/2019
 *	
 *	Desc: This code implement an algorithmn that simulate fire.
 *			It's based in a similar algorithmn of Doom game
 *			Source: https://www.youtube.com/watch?v=fxm8cadCqbs
 */

#include <windows.h>
#include <gl/glew.h>
#include <glfw/glfw3.h>
#include <vector>

using std::vector;

// the color struct to use in palette
struct Color
{
	Color(float red, float green, float blue) {r=red;g=green;b=blue;}
	float r, g, b;
};

void ResizeCallback(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, w, h);
}

//---------------------------------
//	Main class in the program
//		I like to call it cppCore
//---------------------------------
class cppCore
{
	public:
		cppCore() {}
		~cppCore() {}
		
		void Init();
		void InitFire();
		void CalcPalette();
		void CalcFire();
		void RenderFire();
		
	private:
		GLFWwindow* m_pMainWindow;
		vector<vector<int> > m_matFireCells;// The matrix of fire cells
		vector<Color> m_vPalette;			// Palette of colors for fire (Black to Red to Yellow to White)
		
		static const int m_iWinWidth = 600;		// Window Width
		static const int m_iWinHeight = 350; 	// Window Height
		
		static const int m_iNumOfColors = 150; // Number of colors in the palette
		static const int m_iMaxIntensity = m_iNumOfColors-1;	// Max number of the intensity of a fire cell
		static const int m_iWidth = 200;	// Num of horizontal fire cells
		static const int m_iHeight = 100;	// Num of vertical fire cells
};

// Initialize and Execute the main loop
void cppCore::Init()
{
	glfwInit();
	m_pMainWindow = glfwCreateWindow(m_iWinWidth, m_iWinHeight, "Doom Fire", NULL, NULL);
	
	glfwSetFramebufferSizeCallback(m_pMainWindow, ResizeCallback);
	glfwMakeContextCurrent(m_pMainWindow);
	
	InitFire();
	CalcPalette();
	
	while(!glfwWindowShouldClose(m_pMainWindow))
	{
		glfwPollEvents();
		
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		CalcFire();
		RenderFire();
		
		glfwSwapBuffers(m_pMainWindow);
	}
}

void cppCore::InitFire()
{
	for(int i = 0; i < m_iWidth; i++)
		m_matFireCells.push_back(vector<int>(m_iHeight));
}

// Calculate all the colors of the palette
void cppCore::CalcPalette()
{
	// Creating the colors to gradient
	Color black(0.0f, 0.0f, 0.0f);
	Color red(1.0f, 0.0f, 0.0f);
	Color yellow(1.0f, 1.0f, 0.0f);
	Color white(1.0f, 1.0f, 1.0f);
	
	// Interval of the gradient between two colors
	int interval = m_iNumOfColors/3;
	
	// First gradient
	Color clr(0.0f, 0.0f, 0.0f);
	for(int i = interval-1; i >= 0; i--)
	{
		float shade = (float)(i%interval)/interval;
		clr.r = black.r * shade + red.r * (1.0f-shade);
		clr.g = black.g * shade + red.g * (1.0f-shade);
		clr.b = black.b * shade + red.b * (1.0f-shade);
		m_vPalette.push_back(clr);
	}
	
	// Second gradient
	for(int i = interval-1; i >= 0; i--)
	{
		float shade = (float)(i%interval)/interval;
		clr.r = red.r * shade + yellow.r * (1.0f-shade);
		clr.g = red.g * shade + yellow.g * (1.0f-shade);
		clr.b = red.b * shade + yellow.b * (1.0f-shade);
		m_vPalette.push_back(clr);
	}
	
	// Third gradient
	int remainder = m_iNumOfColors - interval*2;
	for(int i = remainder-1; i >= 0; i--)
	{
		float shade = (float)(i%remainder)/remainder;
		clr.r = yellow.r * shade + white.r * (1.0f-shade);
		clr.g = yellow.g * shade + white.g * (1.0f-shade);
		clr.b = yellow.b * shade + white.b * (1.0f-shade);
		m_vPalette.push_back(clr);
	}
}

void cppCore::CalcFire()
{
	// Set the first row intensity with maximum value
	for(int i = 0; i < m_iWidth; i++)
		m_matFireCells[i][0] = m_iMaxIntensity;
	
	// Calculate all others cells intensities
	for(int column = 0; column < m_iWidth; column++)
	{
		for(int row = 1; row < m_iHeight; row++)
		{
			// Calculate the intensity of the based on the cell below or its neighbors
			// with a randomic decay
			int decay = rand()%5;
			m_matFireCells[column][row] = m_matFireCells[column+decay<m_iWidth ? column+decay : column][row-1] - decay; 
			
			if(m_matFireCells[column][row] < 0) m_matFireCells[column][row] = 0; // Don't overcome the limit of zero
		}
	}
}

// Render the previous calculated matrix of fire cells
void cppCore::RenderFire()
{
	// Get the size of screen buffer
	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(m_pMainWindow, &bufferWidth, &bufferHeight);
	
	// Calculate the width and the height of all cells
	float cellWidth, cellHeight;
	cellWidth = (float) bufferWidth / m_iWidth;
	cellHeight = (float) bufferHeight / m_iHeight;
	
	// Iterate in all columns and rows in fire cells matrix
	for(int column = 0; column < m_iWidth; column++)
	{
		for(int row = 0; row < m_iHeight; row++)
		{
			// --------------------------------------------------
			//	Render the fire cell in this column and this row
			// --------------------------------------------------
			float x, y;
			x = (cellWidth*column/bufferWidth) * 2 - 1;
			y = (cellHeight*row/bufferHeight) * 2 - 1;
			
			int intensity = m_matFireCells[column][row];	//	Get a color in the palette 
			Color clr = m_vPalette[intensity];				//	based in the intensity of fire
			glColor3f(clr.r, clr.g, clr.b);					//
			
			glPointSize(cellHeight);
			glBegin(GL_POINTS);		// Set the graphic element as points
			glVertex2f(x, y);		// Set the position of element to draw
			glEnd();
		}
	}
}

// Principal function in the program
int main()
{
	// Calling the initialization of the main class of the program
	cppCore* core = new cppCore();
	core -> Init();	
}
