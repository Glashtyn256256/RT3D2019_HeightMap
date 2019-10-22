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
	void ReturnAverageNormal(int mapindex, XMFLOAT3 &averagenormal);
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
	m_TopNormals = new XMFLOAT3[m_HeightMapWidth - 1];
	int mapIndex = 0;
	XMFLOAT3 firstNormal;
	XMFLOAT3 secondNormal;
	for (int y = 0; y < m_HeightMapLength - 1; y++) //determines which row you are on
	{
		
		for (int x = 0; x < m_HeightMapWidth - 1; x++) //-1 to make it 255
		{
			if (y%2 == 0) //Works out if the row is odd or even
			{
				mapIndex = (m_HeightMapLength * y) + x; 

				XMFLOAT3 v0 = m_pHeightMap[mapIndex];  //Top left
				XMFLOAT3 v1 = m_pHeightMap[mapIndex + m_HeightMapWidth]; //bottom left
				XMFLOAT3 v2 = m_pHeightMap[mapIndex + 1];  //Top right 

				XMFLOAT3 averageNormalsOne;
				XMFLOAT3 averageNormalsTwo;

				ReturnAverageNormal(mapIndex, averageNormalsOne);
				ReturnAverageNormal(mapIndex + m_HeightMapWidth, averageNormalsTwo);

				if (y == 0)
				{
					m_pMapVtxs[vertex++] = Vertex_Pos3fColour4ubNormal3f(v0, MAP_COLOUR, averageNormalsOne);
					m_pMapVtxs[vertex++] = Vertex_Pos3fColour4ubNormal3f(v1, MAP_COLOUR, averageNormalsTwo);
				}
				else
				{
					m_pMapVtxs[vertex++] = Vertex_Pos3fColour4ubNormal3f(v0, MAP_COLOUR, averageNormalsOne);
					m_pMapVtxs[vertex++] = Vertex_Pos3fColour4ubNormal3f(v1, MAP_COLOUR, averageNormalsTwo);
				}
			}
			else 
			{
				mapIndex = (m_HeightMapLength * y) + (m_HeightMapWidth - x) - 2; //go backwards
				
				XMFLOAT3 v0 = m_pHeightMap[mapIndex];  //Top left
				XMFLOAT3 v1 = m_pHeightMap[mapIndex + m_HeightMapWidth]; //bottom left
				XMFLOAT3 v2 = m_pHeightMap[mapIndex + 1];  //Top right 

				XMFLOAT3 averageNormalsOne;
				XMFLOAT3 averageNormalsTwo;

				ReturnAverageNormal(mapIndex + m_HeightMapWidth, averageNormalsOne);
				ReturnAverageNormal(mapIndex, averageNormalsTwo);

				m_pMapVtxs[vertex++] = Vertex_Pos3fColour4ubNormal3f(v1, MAP_COLOUR, averageNormalsOne);
				m_pMapVtxs[vertex++] = Vertex_Pos3fColour4ubNormal3f(v0, MAP_COLOUR, averageNormalsTwo);
			}
		}
	}

	m_pHeightMapBuffer = CreateImmutableVertexBuffer(m_pD3DDevice, sizeof Vertex_Pos3fColour4ubNormal3f * m_HeightMapVtxCount, m_pMapVtxs);

	delete m_pMapVtxs;

	return true;
}

void HeightMapApplication::ReturnAverageNormal(int mapindex, XMFLOAT3 &averagenormal) 
{

	int y = mapindex / m_HeightMapWidth;
	int x = mapindex % m_HeightMapWidth;
	if (y == 255)
	{
		bool test = true;
	}
	if (x == 255)
	{
		bool test = true;
	}
	XMVECTOR faceNormalOne;
	XMVECTOR faceNormalTwo;
	XMVECTOR faceNormalThree;
	XMVECTOR faceNormalFour;
	XMVECTOR faceNormalFive;
	XMVECTOR faceNormalSix;
	XMVECTOR faceNormalAverage;

	if (y == 0 && x == 0) //top left corner
	{
		XMFLOAT3 v0 = m_pHeightMap[mapindex];  //Middle
		XMFLOAT3 v2 = m_pHeightMap[mapindex + m_HeightMapWidth]; //Middle bottom
		XMFLOAT3 v3 = m_pHeightMap[mapindex + 1];  //Middle right

		XMVECTOR vector0 = XMLoadFloat3(&XMFLOAT3(v0.x, v0.y, v0.z)); //middle
		XMVECTOR vector2 = XMLoadFloat3(&XMFLOAT3(v2.x, v2.y, v2.z)); // middle bottom
		XMVECTOR vector3 = XMLoadFloat3(&XMFLOAT3(v3.x, v3.y, v3.z)); //middle right

		faceNormalOne = XMVector3Cross(XMVectorSubtract(vector0, vector2), XMVectorSubtract(vector2, vector3)); // Middle, middle bottom, middle right //correct
		faceNormalAverage = faceNormalOne;
	}
	else if (y == 255 && x == 254) //bottom right corner
	{
		XMFLOAT3 v0 = m_pHeightMap[mapindex];  //Middle
		XMFLOAT3 v1 = m_pHeightMap[mapindex - m_HeightMapWidth]; //Middle top
		XMFLOAT3 v4 = m_pHeightMap[mapindex - 1];  //Middle left

		XMVECTOR vector0 = XMLoadFloat3(&XMFLOAT3(v0.x, v0.y, v0.z)); //middle
		XMVECTOR vector1 = XMLoadFloat3(&XMFLOAT3(v1.x, v1.y, v1.z)); // middle top
		XMVECTOR vector4 = XMLoadFloat3(&XMFLOAT3(v4.x, v4.y, v4.z)); //middle left

		faceNormalOne = XMVector3Cross(XMVectorSubtract(vector0, vector1), XMVectorSubtract(vector1, vector4)); // Middle, middle top, middle left //correct
		faceNormalAverage = faceNormalOne;
	}
	else if (y == 0 && x == 254) //top right corner
	{
		XMFLOAT3 v0 = m_pHeightMap[mapindex];  //Middle
		XMFLOAT3 v2 = m_pHeightMap[mapindex + m_HeightMapWidth]; //Middle bottom
		XMFLOAT3 v4 = m_pHeightMap[mapindex - 1];  //Middle left
		XMFLOAT3 v6 = m_pHeightMap[mapindex + m_HeightMapWidth - 1]; //Middle bottom left;

		XMVECTOR vector0 = XMLoadFloat3(&XMFLOAT3(v0.x, v0.y, v0.z)); //middle
		XMVECTOR vector2 = XMLoadFloat3(&XMFLOAT3(v2.x, v2.y, v2.z)); // middle bottom
		XMVECTOR vector4 = XMLoadFloat3(&XMFLOAT3(v4.x, v4.y, v4.z)); //middle left
		XMVECTOR vector6 = XMLoadFloat3(&XMFLOAT3(v6.x, v6.y, v6.z)); //middle bottom left

		faceNormalOne = XMVector3Cross(XMVectorSubtract(vector0, vector4), XMVectorSubtract(vector4, vector6)); //middle, middle left, middle bottom left // correct
		faceNormalTwo = XMVector3Cross(XMVectorSubtract(vector0, vector6), XMVectorSubtract(vector6, vector2)); //Middle, middle bottom left, middle bottom //correct
		faceNormalAverage = faceNormalOne + faceNormalTwo;
		faceNormalAverage = faceNormalAverage / 2;

	}
	else if (y == 255 && x == 0) //bottom left corner
	{
		XMFLOAT3 v0 = m_pHeightMap[mapindex];  //Middle
		XMFLOAT3 v1 = m_pHeightMap[mapindex - m_HeightMapWidth]; //Middle top
		XMFLOAT3 v3 = m_pHeightMap[mapindex + 1];  //Middle right
		XMFLOAT3 v5 = m_pHeightMap[mapindex - m_HeightMapWidth + 1]; //Middle top right;

		XMVECTOR vector0 = XMLoadFloat3(&XMFLOAT3(v0.x, v0.y, v0.z)); //middle
		XMVECTOR vector1 = XMLoadFloat3(&XMFLOAT3(v1.x, v1.y, v1.z)); // middle top
		XMVECTOR vector3 = XMLoadFloat3(&XMFLOAT3(v3.x, v3.y, v3.z)); //middle right
		XMVECTOR vector5 = XMLoadFloat3(&XMFLOAT3(v5.x, v5.y, v5.z)); //middle top right

		faceNormalOne = XMVector3Cross(XMVectorSubtract(vector0, vector3), XMVectorSubtract(vector3, vector5)); //Middle, middle right, middle top right //correct
		faceNormalTwo = XMVector3Cross(XMVectorSubtract(vector0, vector5), XMVectorSubtract(vector5, vector1)); //Middle, middle top right, middle top //correct
		faceNormalAverage = faceNormalOne + faceNormalTwo;
		faceNormalAverage = faceNormalAverage / 2;
	}
	else if (x == 0) //leftside
	{
		XMFLOAT3 v0 = m_pHeightMap[mapindex];  //Middle
		XMFLOAT3 v1 = m_pHeightMap[mapindex - m_HeightMapWidth]; //Middle top
		XMFLOAT3 v2 = m_pHeightMap[mapindex + m_HeightMapWidth]; //Middle bottom
		XMFLOAT3 v3 = m_pHeightMap[mapindex + 1];  //Middle right
		XMFLOAT3 v5 = m_pHeightMap[mapindex - m_HeightMapWidth + 1]; //Middle top right;

		XMVECTOR vector0 = XMLoadFloat3(&XMFLOAT3(v0.x, v0.y, v0.z)); //middle
		XMVECTOR vector1 = XMLoadFloat3(&XMFLOAT3(v1.x, v1.y, v1.z)); // middle top
		XMVECTOR vector2 = XMLoadFloat3(&XMFLOAT3(v2.x, v2.y, v2.z)); // middle bottom
		XMVECTOR vector3 = XMLoadFloat3(&XMFLOAT3(v3.x, v3.y, v3.z)); //middle right
		XMVECTOR vector5 = XMLoadFloat3(&XMFLOAT3(v5.x, v5.y, v5.z)); //middle top right

		faceNormalOne = XMVector3Cross(XMVectorSubtract(vector0, vector5), XMVectorSubtract(vector5, vector1)); //middle, middle top right, middle top; //correct
		faceNormalTwo = XMVector3Cross(XMVectorSubtract(vector0, vector3), XMVectorSubtract(vector3, vector5)); //Middle, middle right, middle top right //correct
		faceNormalThree = XMVector3Cross(XMVectorSubtract(vector0, vector2), XMVectorSubtract(vector2, vector3)); //Middle, middle bottom, middle right //correct
		faceNormalAverage = faceNormalOne + faceNormalTwo + faceNormalThree;
		faceNormalAverage = faceNormalAverage / 3;
	}
	else if (x == 254) //rightside
	{
		XMFLOAT3 v0 = m_pHeightMap[mapindex];  //Middle
		XMFLOAT3 v1 = m_pHeightMap[mapindex - m_HeightMapWidth]; //Middle top
		XMFLOAT3 v2 = m_pHeightMap[mapindex + m_HeightMapWidth]; //Middle bottom
		XMFLOAT3 v4 = m_pHeightMap[mapindex - 1];  //Middle left
		XMFLOAT3 v6 = m_pHeightMap[mapindex + m_HeightMapWidth - 1]; //Middle bottom left;

		XMVECTOR vector0 = XMLoadFloat3(&XMFLOAT3(v0.x, v0.y, v0.z)); //middle
		XMVECTOR vector1 = XMLoadFloat3(&XMFLOAT3(v1.x, v1.y, v1.z)); // middle top
		XMVECTOR vector2 = XMLoadFloat3(&XMFLOAT3(v2.x, v2.y, v2.z)); // middle bottom
		XMVECTOR vector4 = XMLoadFloat3(&XMFLOAT3(v4.x, v4.y, v4.z)); //middle left
		XMVECTOR vector6 = XMLoadFloat3(&XMFLOAT3(v6.x, v6.y, v6.z)); //middle bottom left

		faceNormalOne = XMVector3Cross(XMVectorSubtract(vector0, vector1), XMVectorSubtract(vector1, vector4)); //middle, middle top, middle left //correct
		faceNormalTwo = XMVector3Cross(XMVectorSubtract(vector0, vector4), XMVectorSubtract(vector4, vector6)); //Middle, middle left, middle bottom left //correct
		faceNormalThree = XMVector3Cross(XMVectorSubtract(vector0, vector6), XMVectorSubtract(vector6, vector2)); //Middle, middle bottom left, middle bottom //correct
		faceNormalAverage = faceNormalOne + faceNormalTwo + faceNormalThree;
		faceNormalAverage = faceNormalAverage / 3;
	}
	else if (y == 0) //topside
	{
		XMFLOAT3 v0 = m_pHeightMap[mapindex];  //Middle
		XMFLOAT3 v2 = m_pHeightMap[mapindex + m_HeightMapWidth]; //Middle bottom
		XMFLOAT3 v3 = m_pHeightMap[mapindex + 1];  //Middle right
		XMFLOAT3 v4 = m_pHeightMap[mapindex - 1];  //Middle left
		XMFLOAT3 v6 = m_pHeightMap[mapindex + m_HeightMapWidth - 1]; //Middle bottom left;

		XMVECTOR vector0 = XMLoadFloat3(&XMFLOAT3(v0.x, v0.y, v0.z)); //middle
		XMVECTOR vector2 = XMLoadFloat3(&XMFLOAT3(v2.x, v2.y, v2.z)); // middle bottom
		XMVECTOR vector3 = XMLoadFloat3(&XMFLOAT3(v3.x, v3.y, v3.z)); //middle right
		XMVECTOR vector4 = XMLoadFloat3(&XMFLOAT3(v4.x, v4.y, v4.z)); //middle left
		XMVECTOR vector6 = XMLoadFloat3(&XMFLOAT3(v6.x, v6.y, v6.z)); //middle bottom left

		faceNormalOne = XMVector3Cross(XMVectorSubtract(vector0, vector4), XMVectorSubtract(vector4, vector6)); //middle, middle left, middle bottom left //correct
		faceNormalTwo = XMVector3Cross(XMVectorSubtract(vector0, vector6), XMVectorSubtract(vector6, vector2)); //Middle, middle bottom left, middle bottom //correct
		faceNormalThree = XMVector3Cross(XMVectorSubtract(vector0, vector2), XMVectorSubtract(vector2, vector3)); //Middle, middle bottom, middle right //correct
		faceNormalAverage = faceNormalOne + faceNormalTwo + faceNormalThree;
		faceNormalAverage = faceNormalAverage / 3;
	}
	else if (y == 255) //bottom side
	{
		XMFLOAT3 v0 = m_pHeightMap[mapindex];  //Middle
		XMFLOAT3 v1 = m_pHeightMap[mapindex - m_HeightMapWidth]; //Middle top
		XMFLOAT3 v3 = m_pHeightMap[mapindex + 1];  //Middle right
		XMFLOAT3 v4 = m_pHeightMap[mapindex - 1];  //Middle left
		XMFLOAT3 v5 = m_pHeightMap[mapindex - m_HeightMapWidth + 1]; //Middle top right;

		XMVECTOR vector0 = XMLoadFloat3(&XMFLOAT3(v0.x, v0.y, v0.z)); //middle
		XMVECTOR vector1 = XMLoadFloat3(&XMFLOAT3(v1.x, v1.y, v1.z)); // middle top
		XMVECTOR vector3 = XMLoadFloat3(&XMFLOAT3(v3.x, v3.y, v3.z)); //middle right
		XMVECTOR vector4 = XMLoadFloat3(&XMFLOAT3(v4.x, v4.y, v4.z)); //middle left
		XMVECTOR vector5 = XMLoadFloat3(&XMFLOAT3(v5.x, v5.y, v5.z)); //middle top right

		faceNormalOne = XMVector3Cross(XMVectorSubtract(vector0, vector1), XMVectorSubtract(vector1, vector4)); //middle, middle top, middle left //correct
		faceNormalTwo = XMVector3Cross(XMVectorSubtract(vector0, vector5), XMVectorSubtract(vector5, vector1)); //Middle, middle top right, middle top //correct
		faceNormalThree = XMVector3Cross(XMVectorSubtract(vector0, vector3), XMVectorSubtract(vector3, vector5)); //Middle, middle right, middle top right //correct
		faceNormalAverage = faceNormalOne + faceNormalTwo + faceNormalThree;
		faceNormalAverage = faceNormalAverage / 3;
	}
	else // middle
	{
		XMFLOAT3 v0 = m_pHeightMap[mapindex];  //Middle
		XMFLOAT3 v1 = m_pHeightMap[mapindex - m_HeightMapWidth]; //Middle top
		XMFLOAT3 v2 = m_pHeightMap[mapindex + m_HeightMapWidth]; //Middle bottom
		XMFLOAT3 v3 = m_pHeightMap[mapindex + 1];  //Middle right
		XMFLOAT3 v4 = m_pHeightMap[mapindex - 1];  //Middle left
		XMFLOAT3 v5 = m_pHeightMap[mapindex - m_HeightMapWidth + 1]; //Middle top right;
		XMFLOAT3 v6 = m_pHeightMap[mapindex + m_HeightMapWidth - 1]; //Middle bottom left;

		XMVECTOR vector0 = XMLoadFloat3(&XMFLOAT3(v0.x, v0.y, v0.z)); //middle
		XMVECTOR vector1 = XMLoadFloat3(&XMFLOAT3(v1.x, v1.y, v1.z)); // middle top
		XMVECTOR vector2 = XMLoadFloat3(&XMFLOAT3(v2.x, v2.y, v2.z)); // middle bottom
		XMVECTOR vector3 = XMLoadFloat3(&XMFLOAT3(v3.x, v3.y, v3.z)); //middle right
		XMVECTOR vector4 = XMLoadFloat3(&XMFLOAT3(v4.x, v4.y, v4.z)); //middle left
		XMVECTOR vector5 = XMLoadFloat3(&XMFLOAT3(v5.x, v5.y, v5.z)); //middle top right
		XMVECTOR vector6 = XMLoadFloat3(&XMFLOAT3(v6.x, v6.y, v6.z)); //middle bottom left

		faceNormalOne = XMVector3Cross(XMVectorSubtract(vector0, vector5), XMVectorSubtract(vector5, vector1)); // Middle, middle top right, middle top // correct
		faceNormalTwo = XMVector3Cross(XMVectorSubtract(vector0, vector3), XMVectorSubtract(vector3, vector5)); // Middle, middle right, middle top right // correct
		faceNormalThree = XMVector3Cross(XMVectorSubtract(vector0, vector1), XMVectorSubtract(vector1, vector4)); // middle, middle top, middle left // correct
		faceNormalFour = XMVector3Cross(XMVectorSubtract(vector0, vector2), XMVectorSubtract(vector2, vector3)); // middle, middle bottom, middle right // correct
		faceNormalFive = XMVector3Cross(XMVectorSubtract(vector0, vector4), XMVectorSubtract(vector4, vector6)); //middle, middle left, middle bottom left // correct
		faceNormalSix = XMVector3Cross(XMVectorSubtract(vector0, vector6), XMVectorSubtract(vector6, vector2)); // middle, middle bottom left, middle bottom //correct

		faceNormalAverage = faceNormalOne + faceNormalTwo + faceNormalThree + faceNormalFour + faceNormalFive + faceNormalSix;
		faceNormalAverage = faceNormalAverage / 6;
	}

	XMStoreFloat3(&averagenormal, faceNormalAverage);
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
