#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d11.h>

#include "CommonApp.h"

#include <stdio.h>

#include <DirectXMath.h>
using namespace DirectX;

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

class HeightMapApplication : public CommonApp
{
  public:
  protected:
	bool HandleStart();
	void HandleStop();
	void HandleUpdate();
	void HandleRender();
	void ReturnAverageNormal(int mapindex, int x);
	bool LoadHeightMap(char* filename, float gridSize);

  private:
	ID3D11Buffer* m_pHeightMapBuffer;
	float m_rotationAngle;
	int m_HeightMapWidth;
	int m_HeightMapLength;
	int m_HeightMapVtxCount;
	XMFLOAT3* m_pHeightMap;
	Vertex_Pos3fColour4ubNormal3f* m_pMapVtxs;
	float m_cameraZ;
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

bool HeightMapApplication::HandleStart()
{
	this->SetWindowTitle("HeightMap");

	LoadHeightMap("HeightMap.bmp", 1.0f);

	m_cameraZ = 50.0f;

	m_pHeightMapBuffer = NULL;

	m_rotationAngle = 0.f;

	if(!this->CommonApp::HandleStart())
		return false;

	static const VertexColour MAP_COLOUR(200, 255, 255, 255);

	/////////////////////////////////////////////////////////////////
	// Clearly this code will need changing to render the heightmap
	/////////////////////////////////////////////////////////////////
	int vertex = 0;
	//m_HeightMapVtxCount = m_HeightMapWidth * m_HeightMapLength;
	//m_HeightMapVtxCount = (m_HeightMapLength - 1) * (m_HeightMapWidth - 1) * 6;  // * 6 due to number of vertex each time we go through loop
	m_HeightMapVtxCount = (m_HeightMapLength - 1) * m_HeightMapWidth * 2;
	m_pMapVtxs = new Vertex_Pos3fColour4ubNormal3f[m_HeightMapVtxCount];

	XMFLOAT3* m_TopNormals;
	XMFLOAT3* m_BottomNormals;
	m_TopNormals = new XMFLOAT3[m_HeightMapWidth - 1];
	m_BottomNormals = new XMFLOAT3[m_HeightMapWidth - 1];

	//int mapIndex = 0;
	//for (int x = 0; x < m_HeightMapLength - 1; x++) //determines which row you are on
	//{
	//	for (int z = 0; z < m_HeightMapWidth - 1; z++) //-1 to make it 255
	//	{
	//		mapIndex = (m_HeightMapLength * x) + z;

	//		XMFLOAT3 v0 = m_pHeightMap[mapIndex + m_HeightMapWidth]; //bottom left
	//		XMFLOAT3 v1 = m_pHeightMap[mapIndex];  //Top left
	//		XMFLOAT3 v2 = m_pHeightMap[mapIndex + m_HeightMapWidth + 1]; //bottom right;

	//		
	//		XMFLOAT3 c = XMFLOAT3(2.0f, 2.0f, 2.0f);
	//		XMFLOAT3 s = XMFLOAT3(3.0f, 3.0f, 3.0f);
	//		XMFLOAT3 m = XMFLOAT3(6.0f, 6.0f, 6.0f);
	//		
	//		XMVECTOR vCorner = XMLoadFloat3(&XMFLOAT3(c.x, c.y, c.z));
	//		XMVECTOR vSide = XMLoadFloat3(&XMFLOAT3(s.x, s.y, s.z));
	//		XMVECTOR vMiddle = XMLoadFloat3(&XMFLOAT3(m.x, m.y, m.z));


	//		XMVECTOR averageNormal;

	//		XMFLOAT3 secondNormal;
	//		XMVECTOR vector4 = XMLoadFloat3(&XMFLOAT3(v3.x, v3.y, v3.z));
	//		XMVECTOR vector5 = XMLoadFloat3(&XMFLOAT3(v1.x, v1.y, v1.z));
	//		XMVECTOR vector6 = XMLoadFloat3(&XMFLOAT3(v2.x, v2.y, v2.z));
	//		XMVECTOR norm = XMVector3Cross(XMVectorSubtract(vector1, vector2), XMVectorSubtract(vector2, vector3));
	//		XMStoreFloat3(&secondNormal, norm);

	//		if ((x==0) || (x ==255) && (z==0 || z == 255))
	//		{
	//			averageNormal = XMVectorAdd(n, norm);
	//			averageNormal = XMVectorDivide(n,vCorner);
	//		}
	//		if (((x == 0) && (z != 255 || z != 0)) || ((z == 0) && (x != 0 || x!= 255)))
	//		{
	//			averageNormal = XMVectorAdd(XMVectorAdd(n, norm), n);
	//			averageNormal = XMVectorDivide(averageNormal, vSide);
	//		}
	//		else 
	//		{
	//			averageNormal = XMVectorAdd(XMVectorAdd(XMVectorAdd(n, norm), XMVectorAdd(n, norm)), XMVectorAdd(n, norm));
	//			averageNormal = XMVectorDivide(averageNormal, vMiddle);
	//		}
	//		XMStoreFloat3(&firstNormal, averageNormal);
	//	}
	//}


	int mapIndex = 0;
	XMFLOAT3 firstNormal;
	XMFLOAT3 secondNormal;
	for (int x = 0; x < m_HeightMapLength - 1; x++) //determines which row you are on
	{
		//we're starting from the back of the array since odd rows go this way 0<<<<<254
		int odd = 254;
		for (int z = 0; z < m_HeightMapWidth - 1; z++) //-1 to make it 255
		{
			if (x%2 == 0) //Works out if the row is odd or even
			{
				mapIndex = (m_HeightMapLength * x) + z; 

				XMFLOAT3 v0 = m_pHeightMap[mapIndex];  //Top left
				XMFLOAT3 v1 = m_pHeightMap[mapIndex + m_HeightMapWidth]; //bottom left
				XMFLOAT3 v2 = m_pHeightMap[mapIndex + 1];  //Top right 
				//XMFLOAT3 v3 = m_pHeightMap[mapIndex + m_HeightMapWidth + 1]; //bottom right;
				//XMFLOAT3 v4 = m_pHeightMap[mapIndex + m_HeightMapWidth + m_HeightMapWidth]; //bottom bottom left

				XMVECTOR vector1 = XMLoadFloat3(&XMFLOAT3(v0.x, v0.y, v0.z));
				XMVECTOR vector2 = XMLoadFloat3(&XMFLOAT3(v1.x, v1.y, v1.z));
				XMVECTOR vector3 = XMLoadFloat3(&XMFLOAT3(v2.x, v2.y, v2.z));
				XMVECTOR n = XMVector3Cross(XMVectorSubtract(vector1, vector2), XMVectorSubtract(vector2, vector3));
				n = XMVector3Normalize(n);
				XMStoreFloat3(&firstNormal, n);

				if (x == 0)
				{			
					m_pMapVtxs[vertex++] = Vertex_Pos3fColour4ubNormal3f(v0, MAP_COLOUR, firstNormal);
					m_pMapVtxs[vertex++] = Vertex_Pos3fColour4ubNormal3f(v1, MAP_COLOUR, firstNormal);
				}
				else
				{
					m_pMapVtxs[vertex++] = Vertex_Pos3fColour4ubNormal3f(v0, MAP_COLOUR, m_TopNormals[z]);
					m_pMapVtxs[vertex++] = Vertex_Pos3fColour4ubNormal3f(v1, MAP_COLOUR, firstNormal);
				}				 
				m_TopNormals[z] = firstNormal;
			}
			else 
			{
				mapIndex = (m_HeightMapLength * x) + (m_HeightMapWidth - z) - 2; //go backwards
				//mapIndex = (m_HeightMapLength * x) + z;
				
				XMFLOAT3 v0 = m_pHeightMap[mapIndex];  //Top left
				XMFLOAT3 v1 = m_pHeightMap[mapIndex + m_HeightMapWidth]; //bottom left
				XMFLOAT3 v2 = m_pHeightMap[mapIndex + 1];  //Top right 
				//XMFLOAT3 v3 = m_pHeightMap[mapIndex + m_HeightMapWidth + 1]; //bottom right;
				//XMFLOAT3 v4 = m_pHeightMap[mapIndex + m_HeightMapWidth + m_HeightMapWidth]; //bottom bottom left
				
				//Anti clockwise 
				XMVECTOR vector1 = XMLoadFloat3(&XMFLOAT3(v0.x, v0.y, v0.z));
				XMVECTOR vector2 = XMLoadFloat3(&XMFLOAT3(v1.x, v1.y, v1.z));
				XMVECTOR vector3 = XMLoadFloat3(&XMFLOAT3(v2.x, v2.y, v2.z));
				XMVECTOR n = XMVector3Cross(XMVectorSubtract(vector1, vector2), XMVectorSubtract(vector2, vector3));
				n = XMVector3Normalize(n);
				XMStoreFloat3(&secondNormal, n);

				if (x == 255)
				{
					m_pMapVtxs[vertex++] = Vertex_Pos3fColour4ubNormal3f(v1, MAP_COLOUR, secondNormal);
					m_pMapVtxs[vertex++] = Vertex_Pos3fColour4ubNormal3f(v0, MAP_COLOUR, secondNormal);
				}
				else {
					m_pMapVtxs[vertex++] = Vertex_Pos3fColour4ubNormal3f(v1, MAP_COLOUR, secondNormal);
					m_pMapVtxs[vertex++] = Vertex_Pos3fColour4ubNormal3f(v0, MAP_COLOUR, m_TopNormals[odd]);
				}
				m_TopNormals[odd] = secondNormal;
				--odd; 
			}
		}
	}

	m_pHeightMapBuffer = CreateImmutableVertexBuffer(m_pD3DDevice, sizeof Vertex_Pos3fColour4ubNormal3f * m_HeightMapVtxCount, m_pMapVtxs);

	delete m_pMapVtxs;

	return true;
}

void HeightMapApplication::ReturnAverageNormal(int mapindex, int x) 
{
	XMFLOAT3 v0 = m_pHeightMap[mapindex];  //Middle
	XMFLOAT3 v1 = m_pHeightMap[mapindex - m_HeightMapWidth]; //Middle top
	XMFLOAT3 v2 = m_pHeightMap[mapindex + m_HeightMapWidth]; //Middle bottom
	XMFLOAT3 v3 = m_pHeightMap[mapindex + 1];  //Middle right
	XMFLOAT3 v4 = m_pHeightMap[mapindex - 1];  //Middle left
	XMFLOAT3 v5 = m_pHeightMap[mapindex - m_HeightMapWidth + 1]; //Middle top right;
	XMFLOAT3 v6 = m_pHeightMap[mapindex + m_HeightMapWidth - 1]; //Middle bottom left;
 
	bool topCorners = false;
	bool bottomCorners = false;
	bool leftSide = false;
	bool rightSide = false;
	bool topSide = false;
	bool bottomSide = false;
	bool middle = false;

	XMVECTOR vector0 = XMLoadFloat3(&XMFLOAT3(v0.x, v0.y, v0.z)); //middle
	XMVECTOR vector1 = XMLoadFloat3(&XMFLOAT3(v1.x, v1.y, v1.z)); // middle top
	XMVECTOR vector2 = XMLoadFloat3(&XMFLOAT3(v2.x, v2.y, v2.z)); // middle bottom
	XMVECTOR vector3 = XMLoadFloat3(&XMFLOAT3(v3.x, v3.y, v3.z)); //middle right
	XMVECTOR vector4 = XMLoadFloat3(&XMFLOAT3(v4.x, v4.y, v4.z)); //middle left
	XMVECTOR vector5 = XMLoadFloat3(&XMFLOAT3(v5.x, v5.y, v5.z)); //middle top right
	XMVECTOR vector6 = XMLoadFloat3(&XMFLOAT3(v6.x, v6.y, v6.z)); //middle bottom left

	XMVECTOR faceNormalOne;
	XMVECTOR faceNormalTwo;
	XMVECTOR faceNormalThree;
	XMVECTOR faceNormalFour;
	XMVECTOR faceNormalFive;
	XMVECTOR faceNormalSix;
	XMVECTOR faceNormalAverage;

	if(middle == true)
	{
		faceNormalOne = XMVector3Cross(XMVectorSubtract(vector0, vector5), XMVectorSubtract(vector5, vector1)); // Middle, middle top right, middle top // correct
		faceNormalTwo = XMVector3Cross(XMVectorSubtract(vector0, vector3), XMVectorSubtract(vector3, vector5)); // Middle, middle right, middle top right // correct
		faceNormalThree = XMVector3Cross(XMVectorSubtract(vector0, vector1), XMVectorSubtract(vector1, vector4)); // middle, middle top, middle left // correct
		faceNormalFour = XMVector3Cross(XMVectorSubtract(vector0, vector2), XMVectorSubtract(vector2, vector3)); // middle, middle bottom, middle right // correct
		faceNormalFive = XMVector3Cross(XMVectorSubtract(vector0, vector4), XMVectorSubtract(vector4, vector6)); //middle, middle left, middle bottom left // correct
		faceNormalSix = XMVector3Cross(XMVectorSubtract(vector0, vector6), XMVectorSubtract(vector6, vector2)); // middle, middle bottom left, middle bottom //correct

		faceNormalAverage = faceNormalOne + faceNormalTwo + faceNormalThree + faceNormalFour + faceNormalFive + faceNormalSix;
		faceNormalAverage = faceNormalAverage / 6;
	}

	if (x == 0)
	{
		faceNormalOne = XMVector3Cross(XMVectorSubtract(vector0, vector2), XMVectorSubtract(vector2, vector5)); // Middle, middle bottom, middle top right
		faceNormalAverage = faceNormalOne;
	}
	if (x == 244)
	{
		faceNormalOne = XMVector3Cross(XMVectorSubtract(vector0, vector1), XMVectorSubtract(vector1, vector6)); // Middle, middle top, middle bottom left
		faceNormalAverage = faceNormalOne;
	}
	if (x == 2)
	{
		faceNormalOne = XMVector3Cross(XMVectorSubtract(vector0, vector4), XMVectorSubtract(vector4, vector6)); //middle, middle left, middle bottom left
		faceNormalTwo = XMVector3Cross(XMVectorSubtract(vector0, vector6), XMVectorSubtract(vector6, vector2)); //Middle, middle bottom left, middle bottom
		faceNormalAverage = faceNormalOne + faceNormalTwo;
		faceNormalAverage = faceNormalAverage / 2;

	}
	if (x == 4)
	{
		faceNormalOne = XMVector3Cross(XMVectorSubtract(vector0, vector3), XMVectorSubtract(vector3, vector5)); //Middle, middle right, middle top right
		faceNormalTwo = XMVector3Cross(XMVectorSubtract(vector0, vector5), XMVectorSubtract(vector5, vector1)); //Middle, middle top right, middle top
		faceNormalAverage = faceNormalOne + faceNormalTwo;
		faceNormalAverage = faceNormalAverage / 2;
	}
	if (leftSide)
	{
		faceNormalOne = XMVector3Cross(XMVectorSubtract(vector0, vector5), XMVectorSubtract(vector5, vector1)); //middle, middle top right, middle top;
		faceNormalTwo = XMVector3Cross(XMVectorSubtract(vector0, vector3), XMVectorSubtract(vector3, vector5)); //Middle, middle right, middle top right
		faceNormalThree = XMVector3Cross(XMVectorSubtract(vector0, vector2), XMVectorSubtract(vector2, vector3)); //Middle, middle bottom, middle right
		faceNormalAverage = faceNormalOne + faceNormalTwo + faceNormalThree;
		faceNormalAverage = faceNormalAverage / 3;
	}
	if (rightSide)
	{
		faceNormalOne = XMVector3Cross(XMVectorSubtract(vector0, vector1), XMVectorSubtract(vector1, vector4)); //middle, middle top, middle left //correct
		faceNormalTwo = XMVector3Cross(XMVectorSubtract(vector0, vector4), XMVectorSubtract(vector4, vector6)); //Middle, middle left, middle bottom left //correct
		faceNormalThree = XMVector3Cross(XMVectorSubtract(vector0, vector6), XMVectorSubtract(vector6, vector4)); //Middle, middle bottom left, middle bottom //correct
		faceNormalAverage = faceNormalOne + faceNormalTwo + faceNormalThree;
		faceNormalAverage = faceNormalAverage / 3;
	}
	if (topSide)
	{
		faceNormalOne = XMVector3Cross(XMVectorSubtract(vector0, vector4), XMVectorSubtract(vector1, vector4)); //middle, middle left, middle bottom left //correct
		faceNormalTwo = XMVector3Cross(XMVectorSubtract(vector0, vector4), XMVectorSubtract(vector4, vector6)); //Middle, middle bottom left, middle bottom //correct
		faceNormalThree = XMVector3Cross(XMVectorSubtract(vector0, vector6), XMVectorSubtract(vector6, vector4)); //Middle, middle bottom left, middle bottom //correct
		faceNormalAverage = faceNormalOne + faceNormalTwo + faceNormalThree;
		faceNormalAverage = faceNormalAverage / 3;
	}
	if (bottomSide)
	{
		faceNormalOne = XMVector3Cross(XMVectorSubtract(vector0, vector1), XMVectorSubtract(vector1, vector4)); //middle, middle top, middle left //correct
		faceNormalTwo = XMVector3Cross(XMVectorSubtract(vector0, vector5), XMVectorSubtract(vector5, vector1)); //Middle, middle top right, middle top
		faceNormalThree = XMVector3Cross(XMVectorSubtract(vector0, vector3), XMVectorSubtract(vector3, vector5)); //Middle, middle right, middle top right
		faceNormalAverage = faceNormalOne + faceNormalTwo + faceNormalThree;
		faceNormalAverage = faceNormalAverage / 3;
	}
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void HeightMapApplication::HandleStop()
{
	Release(m_pHeightMapBuffer);

	this->CommonApp::HandleStop();
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void HeightMapApplication::HandleUpdate()
{
	if (this->IsKeyPressed('Q'))
	{
		m_rotationAngle += .01f;
	}

	if (this->IsKeyPressed('E'))
	{
		m_rotationAngle -= .01f;
	}

	if(this->IsKeyPressed('W'))
	{
		if(m_cameraZ > 20.0f)
			m_cameraZ -= 2.0f;
	}

	if(this->IsKeyPressed('S'))
	{
		m_cameraZ += 2.0f;
	}
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void HeightMapApplication::HandleRender()
{
	XMFLOAT3 vCamera(sin(m_rotationAngle) * m_cameraZ, m_cameraZ / 2, cos(m_rotationAngle) * m_cameraZ);
	XMFLOAT3 vLookat(0.0f, 0.0f, 0.0f);
	XMFLOAT3 vUpVector(0.0f, 1.0f, 0.0f);

	XMMATRIX matView;
	matView = XMMatrixLookAtLH(XMLoadFloat3(&vCamera), XMLoadFloat3(&vLookat), XMLoadFloat3(&vUpVector));

	XMMATRIX matProj;
	matProj = XMMatrixPerspectiveFovLH(float(XM_PI / 4), 2, 1.5f, 5000.0f);

	this->SetViewMatrix(matView);
	this->SetProjectionMatrix(matProj);

	this->EnablePointLight(0, XMFLOAT3(100.0f, 100.f, -100.f), XMFLOAT3(1.f, 1.f, 1.f));

	this->Clear(XMFLOAT4(.2f, .2f, .6f, 1.f));

	this->DrawUntexturedLit(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, m_pHeightMapBuffer, NULL, m_HeightMapVtxCount);
}

//////////////////////////////////////////////////////////////////////
// LoadHeightMap
// Original code sourced from rastertek.com
//////////////////////////////////////////////////////////////////////
bool HeightMapApplication::LoadHeightMap(char* filename, float gridSize)
{
	FILE* filePtr;
	int error;
	unsigned int count;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	int imageSize, i, j, k, index;
	unsigned char* bitmapImage;
	unsigned char height;

	// Open the height map file in binary.
	error = fopen_s(&filePtr, filename, "rb");
	if(error != 0)
	{
		return false;
	}

	// Read in the file header.
	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if(count != 1)
	{
		return false;
	}

	// Read in the bitmap info header.
	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if(count != 1)
	{
		return false;
	}

	// Save the dimensions of the terrain.
	m_HeightMapWidth = bitmapInfoHeader.biWidth;
	m_HeightMapLength = bitmapInfoHeader.biHeight;

	// Calculate the size of the bitmap image data.
	imageSize = m_HeightMapWidth * m_HeightMapLength * 3;

	// Allocate memory for the bitmap image data.
	bitmapImage = new unsigned char[imageSize];
	if(!bitmapImage)
	{
		return false;
	}

	// Move to the beginning of the bitmap data.
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Read in the bitmap image data.
	count = fread(bitmapImage, 1, imageSize, filePtr);
	if(count != imageSize)
	{
		return false;
	}

	// Close the file.
	error = fclose(filePtr);
	if(error != 0)
	{
		return false;
	}

	// Create the structure to hold the height map data.
	m_pHeightMap = new XMFLOAT3[m_HeightMapWidth * m_HeightMapLength];
	if(!m_pHeightMap)
	{
		return false;
	}

	// Initialize the position in the image data buffer.
	k = 0;

	// Read the image data into the height map.
	for(j = 0; j < m_HeightMapLength; j++)
	{
		for(i = 0; i < m_HeightMapWidth; i++)
		{
			height = bitmapImage[k];

			index = (m_HeightMapLength * j) + i;

			m_pHeightMap[index].x = (float)(i - (m_HeightMapWidth / 2)) * gridSize;
			m_pHeightMap[index].y = (float)height / 16 * gridSize;
			m_pHeightMap[index].z = (float)(j - (m_HeightMapLength / 2)) * gridSize;

			k += 3;
		}
	}

	// Release the bitmap image data.
	delete[] bitmapImage;
	bitmapImage = 0;

	return true;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	HeightMapApplication application;

	Run(&application);

	return 0;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
