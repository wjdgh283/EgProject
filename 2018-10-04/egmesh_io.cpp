#include "..\usr\include\egmesh.h"
#include <cstring>
#include <vector>
#include <map>
#include <direct.h>

/*!
*	\brief 경로명을 제외한 파일명(확장명 포함)을 반환한다.
*
*	\param full_path 경로명을 포함한 파일의 이름
*
*	\return \a full_path에서 경로명을 제외한 파일명(확장명)을 반환한다.
*/
std::string get_file_name_ext(std::string full_path)
{
	int idx = (int)full_path.rfind("\\");
	std::string ret = full_path.substr(idx + 1, full_path.length() - idx - 1);

	return ret;
}

/*!
*	\brief 경로명을 제외한 파일명(확장명 불포함)을 반환한다.
*
*	\param full_path 경로명을 포함한 파일의 이름
*
*	\return \a full_path에서 경로명을 제외한 파일명(확장명 불포함)을 반환한다.
*/
std::string get_file_name(std::string full_path)
{
	int idx0 = (int)full_path.rfind("\\");
	int idx1 = (int)full_path.rfind(".");
	std::string ret = full_path.substr(idx0 + 1, idx1 - idx0 - 1);

	return ret;
}

/*!
*	\brief 파일의 확장자를 반환한다.
*
*	\param full_path 경로명을 포함한 파일의 이름
*
*	\return \a full_path에서 파일 확장자를 반환한다.
*/
std::string get_file_ext(std::string full_path)
{
	int idx = (int)full_path.rfind(".");
	std::string ret = full_path.substr(idx + 1, full_path.length() - idx - 1);

	return ret;
}

/*!
*	\brief 파일의 디렉토리 경로명을 반환한다.
*
*	\param full_path 경로명을 포함한 파일의 이름
*
*	\return \a full_path에서 디렉토리 경로명을 반환한다.
*/
std::string get_file_path(std::string full_path)
{
	int idx = (int)full_path.rfind("\\");
	std::string ret = full_path.substr(0, idx);

	return ret;
}

/*!
*	\brief 정수를 문자열로 변환하여 반환한다.
*
*	\param i 문자열로 변환될 정수
*
*	\return 문자열로 변환된 정수
*/
std::string cast_str(int i)
{
	char buffer[128];
	_itoa_s(i, buffer, 10);

	return std::string(buffer);
}

/*!
*	\brief 실수를 문자열로 변환하여 반환한다.
*
*	\param v 문자열로 변환될 실수
*
*	\return 문자열로 변환된 실수
*/
std::string cast_str(float v)
{
	char buffer[128];
	sprintf_s(buffer, "%f", v);

	return std::string(buffer);
}

/*!
*	\brief 실수를 문자열로 변환하여 반환한다.
*
*	\param v 문자열로 변환될 실수
*
*	\return 문자열로 변환된 실수
*/
std::string cast_str(double v)
{
	char buffer[128];
	sprintf_s(buffer, "%lf", v);

	return std::string(buffer);
}

/*!
*	\brief 텍스처 좌표를 0과 1사이로 정규화 한다.
*
*	\param u 텍스처 좌표 u
*	\param v 텍스처 좌표 v
*/
void normalize_uv(double &u, double &v)
{
	u = (u > 1.0) ? (u - (int)(u)) : u;
	u = (u < 0.0) ? (u - (int)(u - 1.0)) : u;

	v = (v > 1.0) ? (v - (int)(v)) : v;
	v = (v < 0.0) ? (v - (int)(v - 1.0)) : v;
}

/*!
*	\brief 비트맵 파일을 로드한다.
*
*	\param filename 로드할 파일의 이름
*	\param info 비트맵 파일의 정보가 저장됨
*
*	\return 비트맵 파일의 이미지 데이터
*/
unsigned char *open_image_bmp(const char *filename, BITMAPINFO **info)
{
	// 현재 작업 디렉터리를 출력한다.
	char cwd[256];
	printf("CWD(open_image_bmp): %s\n", _getcwd(cwd, 256));

	FILE *fp;					/* Open file pointer */
	GLubyte *bits;				/* Bitmap pixel bits */
	unsigned int bitsize;		/* Size of bitmap */
	unsigned int infosize;		/* Size of header information */
	BITMAPFILEHEADER header;    /* File header */

	fopen_s(&fp, filename, "rb");
	if (!fp)
	{
		printf("%s does not exist...\n", filename);
		return NULL;
	}

	/* Read the file header and any following bitmap information... */
	if (fread(&header, sizeof(BITMAPFILEHEADER), 1, fp) < 1)
	{
		/* Couldn't read the file header - return NULL... */
		fclose(fp);
		return NULL;
	}

	if (header.bfType != 'MB')	/* Check for BM reversed... */
	{
		/* Not a bitmap file - return NULL... */
		fclose(fp);
		return NULL;
	}

	infosize = header.bfOffBits - sizeof(BITMAPFILEHEADER);
	if ((*info = (BITMAPINFO *)malloc(infosize)) == NULL)
	{
		/* Couldn't allocate memory for bitmap info - return NULL... */
		fclose(fp);
		return NULL;
	}

	if (fread(*info, 1, infosize, fp) < infosize)
	{
		/* Couldn't read the bitmap header - return NULL... */
		free(*info);
		fclose(fp);
		return NULL;
	}

	/* Now that we have all the header info read in, allocate memory for *
	* the bitmap and read *it* in...                                    */
	if ((bitsize = (*info)->bmiHeader.biSizeImage) == 0)
		bitsize = ((*info)->bmiHeader.biWidth * (*info)->bmiHeader.biBitCount + 7) / 8 * abs((*info)->bmiHeader.biHeight);

	if ((bits = (GLubyte*)malloc(bitsize)) == NULL)
	{
		/* Couldn't allocate memory - return NULL! */
		free(*info);
		fclose(fp);
		return NULL;
	}

	if (fread(bits, 1, bitsize, fp) < bitsize)
	{
		/* Couldn't read bitmap - free memory and return NULL! */
		free(*info);
		free(bits);
		fclose(fp);
		return NULL;
	}

	/* OK, everything went fine - return the allocated bitmap... */
	fclose(fp);
	return bits;
}

/*!
*	\brief 비트맵 데이터를 파일로 저장한다.
*
*	\param filename 저장할 파일의 이름
*	\param info 비트맵 파일의 정보
*	\param bits 비트맵 데이터
*
*	\return 성공: 0, 실패: -1
*/
int save_image_bmp(const char *filename, BITMAPINFO *info, GLubyte *bits)
{
	FILE *fp;					/* Open file pointer */
	unsigned int size,			/* Size of file */
		infosize,		/* Size of bitmap info */
		bitsize;		/* Size of bitmap pixels */
	BITMAPFILEHEADER header;    /* File header */

								/* Try opening the file; use "wb" mode to write this *binary* file. */
	fopen_s(&fp, filename, "wb");
	if (fp == NULL)
		return -1;

	/* Figure out the bitmap size */
	if (info->bmiHeader.biSizeImage == 0)
		bitsize = (info->bmiHeader.biWidth * info->bmiHeader.biBitCount + 7) / 8 * abs(info->bmiHeader.biHeight);
	else
		bitsize = info->bmiHeader.biSizeImage;

	/* Figure out the header size */
	infosize = sizeof(BITMAPINFOHEADER);
	switch (info->bmiHeader.biCompression)
	{
	case BI_BITFIELDS:
		infosize += 12; /* Add 3 RGB doubleword masks */
		if (info->bmiHeader.biClrUsed == 0)
			break;
	case BI_RGB:
		if (info->bmiHeader.biBitCount > 8 && info->bmiHeader.biClrUsed == 0)
			break;
	case BI_RLE8:
	case BI_RLE4:
		if (info->bmiHeader.biClrUsed == 0)
			infosize += (1 << info->bmiHeader.biBitCount) * 4;
		else
			infosize += info->bmiHeader.biClrUsed * 4;
		break;
	}

	size = sizeof(BITMAPFILEHEADER) + infosize + bitsize;

	/* Write the file header, bitmap information, and bitmap pixel data... */
	header.bfType = 'MB'; /* Non-portable... sigh */
	header.bfSize = size;
	header.bfReserved1 = 0;
	header.bfReserved2 = 0;
	header.bfOffBits = sizeof(BITMAPFILEHEADER) + infosize;

	if (fwrite(&header, 1, sizeof(BITMAPFILEHEADER), fp) < sizeof(BITMAPFILEHEADER))
	{
		/* Couldn't write the file header - return... */
		fclose(fp);
		return -1;
	}

	if (fwrite(info, 1, infosize, fp) < infosize)
	{
		/* Couldn't write the bitmap header - return... */
		fclose(fp);
		return -1;
	}

	if (fwrite(bits, 1, bitsize, fp) < bitsize)
	{
		/* Couldn't write the bitmap - return... */
		fclose(fp);
		return -1;
	}

	/* OK, everything went fine - return... */
	fclose(fp);
	return 0;
}

unsigned char *getRGBA(FILE *s, int size)
{
	unsigned char *rgba = (unsigned char*)malloc(size * 4);
	if (rgba == NULL)
		return 0;

	int bread = (int)fread(rgba, sizeof(unsigned char), size * 4, s);
	if (bread != size * 4)
	{
		free(rgba);
		return 0;
	}

	/* TGA is stored in BGRA, make it RGBA */
	for (int i = 0; i < size * 4; i += 4)
	{
		unsigned char temp = rgba[i];
		rgba[i] = rgba[i + 2];
		rgba[i + 2] = temp;
	}

	return rgba;
}

unsigned char *getRGB(FILE *s, int size)
{
	unsigned char *rgb = (unsigned char*)malloc(size * 3);
	if (rgb == NULL)
		return 0;

	int bread = (int)fread(rgb, sizeof(unsigned char), size * 3, s);
	if (bread != size * 3)
	{
		free(rgb);
		return 0;
	}

	/* TGA is stored in BGR, make it RGB */
	for (int i = 0; i < size * 3; i += 3)
	{
		unsigned char temp = rgb[i];
		rgb[i] = rgb[i + 2];
		rgb[i + 2] = temp;
	}

	return rgb;
}

unsigned char *getGray(FILE *s, int size)
{
	unsigned char *grayData = (unsigned char *)malloc(size);
	if (grayData == NULL)
		return 0;

	int bread = (int)fread(grayData, sizeof(unsigned char), size, s);
	if (bread != size)
	{
		free(grayData);
		return 0;
	}

	return grayData;
}

unsigned char *getData(FILE *s, int sz, int iBits, GLenum *texFormat)
{
	if (iBits == 32)
	{
		*texFormat = GL_RGBA;
		return getRGBA(s, sz);
	}
	else if (iBits == 24)
	{
		*texFormat = GL_RGB;
		return getRGB(s, sz);
	}
	else if (iBits == 8)
	{
		*texFormat = GL_ALPHA;
		return getGray(s, sz);
	}

	*texFormat = GL_ALPHA;
	return getGray(s, sz);
}

/*!
*	\brief TGA 파일을 로드한다.
*
*	\param name 로드할 파일의 이름
*	\param width 이미지의 가로 길이
*	\param height 이미지의 세로 길이
*	\param format 이미지 포맷
*
*	\return 비트맵 파일의 이미지 데이터
*/
unsigned char *open_image_tga(const char *name, int *width, int *height, GLenum *format)
{
	// 현재 작업 디렉터리를 출력한다.
	//char cwd[256];
	//printf("CWD(open_image_tga): %s\n", _getcwd(cwd, 256));

	FILE *fp;
	fopen_s(&fp, name, "rb");
	if (!fp)
	{
		printf("%s does not exist...\n", name);
		return NULL;
	}

	unsigned char type[4];
	unsigned char info[7];
	unsigned char *imageData = NULL;
	int imageWidth, imageHeight;
	int imageBits, size;

	fread(&type, sizeof(char), 3, fp); // read in colormap info and image type, byte 0 ignored
	fseek(fp, 12, SEEK_SET);			// seek past the header and useless info
	fread(&info, sizeof(char), 6, fp);

	imageWidth = info[0] + info[1] * 256;
	imageHeight = info[2] + info[3] * 256;
	imageBits = info[4];
	size = imageWidth * imageHeight;

	/* make sure we are loading a supported type */
	if (imageBits != 32 && imageBits != 24 && imageBits != 8)
		return NULL;

	imageData = getData(fp, size, imageBits, format);

	fclose(fp);

	*width = imageWidth;
	*height = imageHeight;

	return imageData;
}

/*!
*	\brief OBJ 파일에서 사용하는 MTL(재질)정보를 읽어 메쉬에 추가한다.
*
*	\param pMesh 재질을 추가할 메쉬의 포인터
*	\param fname 재질 파일의 이름(현재 작업 경로에서 상대적인 경로로 표현됨)
*
*	\return 성공: true, 실패: flase
*/
bool import_obj_mtl(EgMesh *pMesh, const char *fname)
{
	// 생성할 재질 변수를 정의한다.
	EgMaterial *pMtl = NULL;

	// 파일을 열고, 
	FILE *fp;
	fopen_s(&fp, fname, "r");
	if (!fp) // 실패하면 디폴트 재질을 생성하여 메쉬에 추가하고 false를 반환한다.
	{
		printf("%s does not exist...\n", fname);
		pMtl = new EgMaterial("mtl_defalut");
		pMesh->AddMaterial(pMtl);
		return false;
	}

	// 파일의 끝까지 한 단어씩 읽어, tag 배열에 저장한다.
	char tag[256];
	while (fscanf_s(fp, "%s", tag, 256) != EOF)
	{
		// newmtl (new material) 태그라면,
		if (!strcmp(tag, "newmtl"))
		{
			// 재질의 이름을 읽고,
			char tmp[64];
			fscanf_s(fp, "%s", tmp, 64);

			// 재질을 생성하여, 메쉬의 재질 리스트에 추가한다.
			pMtl = new EgMaterial(tmp);
			pMesh->AddMaterial(pMtl);
			printf("Material: %s is created...\n", tmp);
		}

		// Ka (ambient coefficients) 태그라면,
		if (!strcmp(tag, "Ka"))
		{
			// ambient 성분을 읽어서 재질을 설정한다.
			GLfloat c[3];
			fscanf_s(fp, "%f%f%f", &c[0], &c[1], &c[2]);
			pMtl->SetAmbient(c[0], c[1], c[2]);
		}

		// Kd (diffuse coefficients) 태그라면,
		if (!strcmp(tag, "Kd"))
		{
			// diffuse 성분을 읽어서 재질을 설정한다.
			GLfloat c[3];
			fscanf_s(fp, "%f%f%f", &c[0], &c[1], &c[2]);
			pMtl->SetDiffuse(c[0], c[1], c[2]);
		}

		// Ks (specular coefficients) 태그라면,
		if (!strcmp(tag, "Ks"))
		{
			// specular 성분을 읽어서 재질을 설정한다.
			GLfloat c[3];
			fscanf_s(fp, "%f%f%f", &c[0], &c[1], &c[2]);
			pMtl->SetSpecular(c[0], c[1], c[2]);
		}

		// map_Kd (diffuse texture file) 태그라면,
		if (!strcmp(tag, "map_Kd"))
		{
			// 택스처 파일을 읽는다.
			char tex_name[64];
			fscanf_s(fp, "%s", tex_name, 64);
			pMtl->LoadTexture(tex_name);
		}
	}

	// 파일을 닫는다.
	fclose(fp);
	return true;
}

/*!
*	\brief OBJ 파일을 임포트하여 장면에 추가한다.
*	\note 현재 작업 디렉터리는 임포트할 파일이 있는 디텍터리로 변경된다.
*
*	\param pScene 임포트할 Obj 메쉬가 포함될 장면에 대한 포인터
*	\param FileName 전체 경로가 포함된 파일의 이름
*
*	\return 성공이면 true, 실패면 false를 반환한다.
*/
bool import_obj(EgScene *pScene, const char *FileName)
{
	// 임포트할 파일이 있는 디렉터리를 구하여, 현재 작업 디렉터리로 설정한다.
	//std::string cwd = get_file_path(FileName);
	//_chdir(cwd.c_str());

	// 파일을 열고 실패하면 false를 반환한다.
	FILE *fp;
	fopen_s(&fp, FileName, "r");
	if (!fp)
	{
		printf("Obj file import failed...\n");
		return false;
	}

	// 파일의 이름(경로 및 확장자 제외)을 구하여 메쉬를 생성한다.
	std::string MeshName = get_file_name(FileName);
	EgMesh *pMesh = new EgMesh(MeshName);

	// 로딩에 필요한 임시 변수들을 선언한다.
	char tag[256];
	bool bTexture = false;
	bool bNormal = false;
	std::string GroupName;
	EgMaterial *pCurrMtl = NULL;
	std::vector<EgVertex *> vTable;
	std::vector<EgTexel *> tTable;
	std::vector<EgNormal *> nTable;

	// 파일의 끝까지 한 단어씩 읽으면서 tag 배열에 저장한다.
	while (fscanf_s(fp, "%s", tag, 256) != EOF)
	{
		// mtllib (material library) 태그라면,
		if (!strcmp(tag, "mtllib"))
		{
			// 재질 파일을 읽는다.
			char mtl_fname[64];
			fscanf_s(fp, "%s", mtl_fname, 64);
			if (!import_obj_mtl(pMesh, mtl_fname))
				printf("Mtl file import failed...\n");
		}

		// v (vertex) 태그라면,
		if (!strcmp(tag, "v"))
		{
			// 좌표(x, y, z)를 읽어 정점을 생성하고 메쉬에 추가한다.
			double x, y, z;
			fscanf_s(fp, "%lf%lf%lf", &x, &y, &z);
			EgVertex *pVert = new EgVertex(x, y, z);
			pMesh->AddVertex(pVert);
			vTable.push_back(pVert);
		}

		// vt (vertex texture) 태그라면,
		if (!strcmp(tag, "vt"))
		{
			// 텍스처 좌표(u, v)를 읽어 텍셀을 생성하고 메쉬에 추가한다.
			bTexture = true;
			double u, v;
			fscanf_s(fp, "%lf%lf", &u, &v);
			normalize_uv(u, v);
			EgTexel *pTexel = new EgTexel(u, v);
			pMesh->AddTexel(pTexel);
			tTable.push_back(pTexel);
		}

		// vn (vertex normal) 태그라면,
		if (!strcmp(tag, "vn"))
		{
			// 법선의 방향(nx, ny, nz)을 읽어 법선을 생성하고 메쉬에 추가한다.
			bNormal = true;
			double nx, ny, nz;
			fscanf_s(fp, "%lf%lf%lf", &nx, &ny, &nz);
			EgNormal *pNormal = new EgNormal(nx, ny, nz);
			pMesh->AddNormal(pNormal);
			nTable.push_back(pNormal);
		}

		// g (group) 태그라면,
		if (!strcmp(tag, "g"))
		{
			// 그룹의 이름을 읽는다.
			char grp_name[64];
			fscanf_s(fp, "%s", grp_name, 64);
			GroupName = std::string(grp_name);
		}

		// usemtl (use material) 태그라면,
		if (!strcmp(tag, "usemtl"))
		{
			// 메쉬의 재질 리스트에서 해당 재질을 찾는다.
			char mtl_name[128];
			fscanf_s(fp, "%s", mtl_name, 128);
			pCurrMtl = pMesh->GetMaterial(mtl_name);
		}

		// f (face) 태그라면,
		if (!strcmp(tag, "f"))
		{
			// 폴리곤을 구성하는 정점, 텍셀, 노말의 인덱스를 저장하는 변수를 선언하고,
			std::vector<int> vIdxs, tIdxs, nIdxs;

			// 한 줄을 읽어서,
			char line[128];
			fgets(line, 128, fp);
			char *pData = line;

			// 한 줄을 모두 읽을 때까지
			while (true)
			{
				// 불필요한 공백과 줄빠꿈을 건너띄고,
				while (*pData == ' ' || *pData == '\n')
					pData++;

				// 한 줄을 모두 읽었다면 반복문을 빠져 나온다.
				if (!strcmp(pData, ""))
					break;

				// 인덱스 정보를 buffer로 읽은 후,
				char buffer[64];	// buffer[] = "vidx/tidx/nidx" 
				sscanf_s(pData, "%s", buffer, 64);
				pData += strlen(buffer);

				// 인덱스 정보를 저장할 변수를 선언하고
				char sep;
				int vidx, tidx, nidx;

				// 정점, 텍셀, 노말 인덱스가 모두 있는 경우 (f 1/1/1/ 2/2/2 3/3/3 ...)
				if (bTexture && bNormal)
				{
					// 모든 인덱스를 읽어 리스트에 추가한다.
					sscanf_s(buffer, "%d%c%d%c%d", &vidx, &sep, 1, &tidx, &sep, 1, &nidx);
					vIdxs.push_back(vidx);
					tIdxs.push_back(tidx);
					nIdxs.push_back(nidx);
					continue;
				}

				// 정점과 텍셀 인덱스만 있는 경우 (f 1/1 2/2 3/3 ...)
				if (bTexture && !bNormal)
				{
					// 정점과 텍셀 인덱스만 읽어 리스트에 추가한다.
					sscanf_s(buffer, "%d%c%d", &vidx, &sep, 1, &tidx);
					vIdxs.push_back(vidx);
					tIdxs.push_back(tidx);
					continue;
				}

				// 정점과 노말 인덱스만 있는 경우 (f 1//1 2//2 3//3 ...)
				if (!bTexture && bNormal)
				{
					// 정점과 노말 인덱스만 읽어 리스트에 추가한다.
					sscanf_s(buffer, "%d%c%c%d", &vidx, &sep, 1, &sep, 1, &nidx);
					vIdxs.push_back(vidx);
					nIdxs.push_back(nidx);
					continue;
				}

				// 정점의 인덱스만 있는 경우 (f 1 2 3 ...)
				if (!bTexture && !bNormal)
				{
					// 정점의 인덱스만 읽어 리스트에 추가한다.
					sscanf_s(buffer, "%d", &vidx);
					vIdxs.push_back(vidx);
					continue;
				}
			}

			// 폴리곤을 삼각형으로 나누어 메쉬에 추가한다.
			int PolygonSize = (int)vIdxs.size(); // 삼각형(PolygonSize = 3), 사각형(PolygonSize = 4), ...
			for (int i = 0; i < PolygonSize - 2; ++i)
			{
				// 삼각형의 정점을 구하고,
				EgVertex *pVert[3] = { 0 };
				pVert[0] = vTable[vIdxs[0] - 1];
				pVert[1] = vTable[vIdxs[i + 1] - 1];
				pVert[2] = vTable[vIdxs[i + 2] - 1];

				// 각 정점의 텍셀 정보를 구하고,
				EgTexel *pTexel[3] = { 0 };
				if (bTexture)
				{
					pTexel[0] = tTable[tIdxs[0] - 1];
					pTexel[1] = tTable[tIdxs[i + 1] - 1];
					pTexel[2] = tTable[tIdxs[i + 2] - 1];
				}

				// 각 정점의 법선 정보를 구하고,
				EgNormal *pNormal[3] = { 0 };
				if (bNormal)
				{
					pNormal[0] = nTable[nIdxs[0] - 1];
					pNormal[1] = nTable[nIdxs[i + 1] - 1];
					pNormal[2] = nTable[nIdxs[i + 2] - 1];
				}
				else // 법선 정보가 없다면 삼각형 법선으로 대체한다.
				{
					GVector3 n = ((pVert[1]->m_Pos - pVert[0]->m_Pos) ^ (pVert[2]->m_Pos - pVert[0]->m_Pos)).Normalize();
					pNormal[0] = pNormal[1] = pNormal[2] = new EgNormal(n[0], n[1], n[2]);
					pMesh->AddNormal(pNormal[0]);
				}

				// 하프에지를 생성하고,
				EgEdge *pEdge[3] = { 0 };
				pEdge[0] = new EgEdge(pVert[0], pTexel[0], pNormal[0]);
				pEdge[1] = new EgEdge(pVert[1], pTexel[1], pNormal[1]);
				pEdge[2] = new EgEdge(pVert[2], pTexel[2], pNormal[2]);

				// 삼각형을 생성하여 메쉬에 추가한다.
				if (pCurrMtl == NULL)
					pCurrMtl = pMesh->m_pMaterials;
				EgFace *pFace = new EgFace(pEdge[0], pEdge[1], pEdge[2], pCurrMtl);
				pMesh->AddFace(pFace, GroupName);
			}
		}
	}

	// 파일을 닫는다.
	fclose(fp);

	// 하프에지에 반대편 에지의 정보를 갱신한다.
	pMesh->InitEdgeMate();

	// 임포트한 메쉬의 기본 정보를 출력한다.
	int num_rmv_verts = pMesh->DelUnusedVert();
	int num_rmv_texs = pMesh->DelUnusedTexel();
	int num_rmv_normals = pMesh->DelUnusedNormal();
	printf("# of Vertices = %d\n", pMesh->GetNumVert());
	printf("# of Texels = %d\n", pMesh->GetNumTexel());
	printf("# of Normals = %d\n", pMesh->GetNumNormal());
	printf("# of Faces = %d\n", pMesh->GetNumFace());
	printf("# of Unused Vertices = %d\n", num_rmv_verts);
	printf("# of Unused Texels = %d\n", num_rmv_texs);
	printf("# of Unused Normals = %d\n", num_rmv_normals);

	// 임포트한 메쉬를 장면에 추가하고 true를 반환한다.
	pScene->AddMesh(pMesh);
	return false;
}

/*!
*	\brief 메쉬를 파일(*.obj)로 저장한다.
*
*	\param pMesh 저장할 메쉬의 포인터
*	\param fname 파일의 이름
*
*	\return 성공: true, 실패: flase
*/
bool export_obj(EgMesh *pMesh, const char *fname)
{
	// 파일을 열고 실패하면 false를 반환한다.
	FILE *fp;
	fopen_s(&fp, fname, "w");
	if (!fp)
		return false;

	// 파일의 생성 정보 출력하고,
	fprintf_s(fp, "%s", "# Wavefront OBJ exported by EgTool\n");
	fprintf_s(fp, "%s", "# http://gaia.dongguk.edu \n\n");

	// 재질 라이브러리 파일을 출력하고,
	int NumMtrl = pMesh->GetNumFace();
	if (NumMtrl != 0)
	{
		// 재질 파일(*.mtl)의 이름을 출력하고,
		std::string file_name(fname);
		file_name = get_file_name(fname) + ".mtl";
		fprintf(fp, "mtllib %s\n\n", file_name.c_str());

		// 재질 정보 파일(*.mtl)을 생성하여 출력한다.
		export_obj_mtl(pMesh, file_name.c_str());
	}

	// 정점, 텍셀, 법선의 인덱스를 갱신한다.
	int NumVert = pMesh->UpdateVertIdx();
	int NumTexel = pMesh->UpdateTexIdx();
	int NumNormal = pMesh->UpdateNormalIdx();

	// 정점의 정보를 출력한다.
	printf("Exporting a mesh....\n");
	printf("\tNumber of Vertices = %d\n", NumVert);
	EgVertex *v = pMesh->m_pVerts;
	while (v != NULL)
	{
		GPoint3 p = v->m_Pos;
		fprintf(fp, "%s %lf %lf %lf\n", "v", p[0], p[1], p[2]);
		v = v->m_pNext;
	}
	fprintf(fp, "\n");

	// 텍셀의 정보를 출력한다.
	printf("\tNumber of Texels = %d\n", NumTexel);
	EgTexel *t = pMesh->m_pTexels;
	while (t != NULL)
	{
		GVector2 uv = t->m_Coord;
		fprintf(fp, "%s %lf %lf\n", "vt", uv[0], uv[1]);
		t = t->m_pNext;
	}
	fprintf(fp, "\n");

	// 법선의 정보를 출력한다.
	printf("\tNumber of Normals = %d\n", NumNormal);
	EgNormal *n = pMesh->m_pNormals;
	while (n != NULL)
	{
		GVector3 dir = n->m_Dir;
		fprintf(fp, "%s %lf %lf %lf\n", "vn", dir[0], dir[1], dir[2]);
		n = n->m_pNext;
	}
	fprintf(fp, "\n");

	// 삼각형의 그룹 정보가 없다면
	if (pMesh->m_GroupedFaces.empty())
	{
		// 순자척으로 모든 폴리곤에 대하여,
		EgMaterial *pPrevMtrl = NULL;
		EgFace *f = pMesh->m_pFaces;
		while (f != NULL)
		{
			if (pPrevMtrl == NULL || f->m_pMaterial != pPrevMtrl)
			{
				fprintf(fp, "usemtl %s\n", f->m_pMaterial->m_Name.c_str());
				pPrevMtrl = f->m_pMaterial;
			}
			int vidx0 = f->GetVertex(0)->m_Idx + 1;
			int vidx1 = f->GetVertex(1)->m_Idx + 1;
			int vidx2 = f->GetVertex(2)->m_Idx + 1;
			int nidx0 = f->GetNormal(0)->m_Idx + 1;
			int nidx1 = f->GetNormal(1)->m_Idx + 1;
			int nidx2 = f->GetNormal(2)->m_Idx + 1;

			if (f->GetTexel(0) != NULL)
			{
				int tidx0 = f->GetTexel(0)->m_Idx + 1;
				int tidx1 = f->GetTexel(1)->m_Idx + 1;
				int tidx2 = f->GetTexel(2)->m_Idx + 1;
				//fprintf_s(fp, "%s %d/%d %d/%d %d/%d\n", "f", vidx0, tidx0, vidx1, tidx1, vidx2, tidx2);
				fprintf_s(fp, "%s %d/%d/%d %d/%d/%d %d/%d/%d\n", "f",
					vidx0, tidx0, nidx0, vidx1, tidx1, nidx1, vidx2, tidx2, nidx2);
			}
			else
				fprintf_s(fp, "%s %d//%d %d//%d %d//%d\n", "f",
					vidx0, nidx0, vidx1, nidx1, vidx2, nidx2);

			f = f->m_pNext;
		}
		fprintf_s(fp, "\n");
	}
	else
	{
		std::map<std::string, std::vector<EgFace *>>::iterator it;
		// 각각의 그룹에 대하여,
		for (it = pMesh->m_GroupedFaces.begin(); it != pMesh->m_GroupedFaces.end(); ++it)
		{
			// 그룹의 이름을 출력하고,
			std::string GroupName = it->first;
			fprintf(fp, "g %s\n", GroupName.c_str());

			// 이전 재질의 인덱스를 위한 변수를 선언하고,
			EgMaterial *pPrevMtrl = NULL;

			// 그룹에 속한 각각의 폴리곤에 대하여,
			int NumFace = (int)it->second.size();
			for (int i = 0; i < NumFace; ++i)
			{
				// 삼각형 정보를 얻고,
				EgFace *f = it->second[i];

				// 삼각형의 재질 정보가 있다면 이전 재질정보와 비교하여 다를 경우 출력하고,
				if (pPrevMtrl == NULL || f->m_pMaterial != pPrevMtrl)
				{
					fprintf(fp, "usemtl %s\n", f->m_pMaterial->m_Name.c_str());
					pPrevMtrl = f->m_pMaterial;
				}

				// 정점의 인덱스를 구하고,
				int vidx0 = f->GetVertex(0)->m_Idx + 1;
				int vidx1 = f->GetVertex(1)->m_Idx + 1;
				int vidx2 = f->GetVertex(2)->m_Idx + 1;
				int nidx0 = f->GetNormal(0)->m_Idx + 1;
				int nidx1 = f->GetNormal(1)->m_Idx + 1;
				int nidx2 = f->GetNormal(2)->m_Idx + 1;

				if (f->GetTexel(0) != NULL)
				{
					int tidx0 = f->GetTexel(0)->m_Idx + 1;
					int tidx1 = f->GetTexel(1)->m_Idx + 1;
					int tidx2 = f->GetTexel(2)->m_Idx + 1;
					//fprintf_s(fp, "%s %d/%d %d/%d %d/%d\n", "f", vidx0, tidx0, vidx1, tidx1, vidx2, tidx2);
					fprintf_s(fp, "%s %d/%d/%d %d/%d/%d %d/%d/%d\n", "f",
						vidx0, tidx0, nidx0, vidx1, tidx1, nidx1, vidx2, tidx2, nidx2);
				}
				else
					fprintf_s(fp, "%s %d//%d %d//%d %d//%d\n", "f",
						vidx0, nidx0, vidx1, nidx1, vidx2, nidx2);
			}
			fprintf(fp, "\n");
		}
	}


	// 파일을 닫는다.
	fclose(fp);
	printf("Exporting completed\n\n");

	return true;
}

/*!
*	\brief 재질 정보를 파일(*.mtl)에 저장한다.
*
*	\param pMesh 저장할 메쉬의 포인터
*	\param fname 파일의 이름
*
*	\return 성공: true, 실패: flase
*/
bool export_obj_mtl(EgMesh *pMesh, const char *fname)
{
	// 파일을 열고 실패하면 false를 반환한다.
	FILE *fp;
	fopen_s(&fp, fname, "w");
	if (!fp)
		return false;

	// 파일의 생성 정보 출력하고,
	fprintf(fp, "%s", "# Wavefront MTL exported by EgTool\n");
	fprintf(fp, "%s", "# http://gaia.dongguk.edu \n\n");

	// 각각의 재질에 대하여,
	EgMaterial *pMtrl = pMesh->m_pMaterials;
	while (pMtrl != NULL)
	{
		// 재질의 정보를 출력하고,
		fprintf(fp, "newmtl %s\n", pMtrl->m_Name.c_str());
		fprintf(fp, "Ka %f %f %f\n", pMtrl->m_Ambient[0], pMtrl->m_Ambient[1], pMtrl->m_Ambient[2]);
		fprintf(fp, "Kd %f %f %f\n", pMtrl->m_Diffuse[0], pMtrl->m_Diffuse[1], pMtrl->m_Diffuse[2]);
		fprintf(fp, "Ks %f %f %f\n", pMtrl->m_Specular[0], pMtrl->m_Specular[1], pMtrl->m_Specular[2]);

		// 텍스처가 있다면 파일명을 출력한다.
		if (!pMtrl->m_TexFileName.empty())
			fprintf(fp, "map_Kd %s\n", pMtrl->m_TexFileName.c_str());

		fprintf(fp, "\n");
		pMtrl = pMtrl->m_pNext;
	}

	// 파일을 닫는다.
	fclose(fp);

	return true;
}

/*!
*	\brief FBX 파일을 임포트하여 장면에 추가한다.
*
*	\param pEgScene 임포트할 파일이 저장될 장면에 대한 포인터
*	\param fname 임포트할 파일의 이름
*	\param pFbxManager 임포트할 파일의 Fbx 매니저에 대한 포인터가 저장된다.
*	\param pFbxScene 임포트할 파일의 Fbx 장면에 대한 포인터가 저장된다.
*
*	\return 성공이면 true, 실패면 false를 반환한다.
*/
bool import_fbx(EgScene *pEgScene, const char *fname, FbxManager **pFbxManager, FbxScene **pFbxScene)
{
	// FBX 매니저를 생성한다.
	FbxManager *pManager = FbxManager::Create();
	if (pManager == NULL)
	{
		FBXSDK_printf("Error: Unable to create FBX Manager!\n");
		return false;
	}
	else
		FBXSDK_printf("Autodesk FBX SDK version %s\n\n", pManager->GetVersion());

	// IO 세팅 객체를 생성한다.
	FbxIOSettings *ios = FbxIOSettings::Create(pManager, IOSROOT);

	// 매니저에 IO 세팅 객체를 설정한다.
	pManager->SetIOSettings(ios);

	// 새로운 씬을 생성한다.
	FbxScene *pScene = FbxScene::Create(pManager, "FbxScene");
	if (pFbxScene == NULL)
	{
		FBXSDK_printf("Error: Unable to create FBX scene!\n");
		return false;
	}

	// 임포터를 생성한다.
	FbxImporter *pImporter = FbxImporter::Create(pManager, "");

	// 로드할 파일 이름으로 임포터를 초기화 한다.
	if (!pImporter->Initialize(fname, -1, pManager->GetIOSettings()))
	{
		printf("Call to FbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", pImporter->GetStatus().GetErrorString());
		return false;
	}

	// 객체를 임포트하여 씬에 추가한다.
	pImporter->Import(pScene);

	// 사용한 임포터를 제거한다.
	pImporter->Destroy();

	// 장면의 모든 노드를 삼각형화 한다.
	FbxGeometryConverter geometryConverter(pManager);
	geometryConverter.Triangulate(pScene, true);

	// 씬의 루트 노드를 구하여
	FbxNode *pRootNode = pScene->GetRootNode();
	if (pRootNode)
	{
		// 루트 노드의 자식 노드를 재귀적으 로 임포트 한다.
		for (int i = 0; i < pRootNode->GetChildCount(); i++)
			import_fbx_mesh(pEgScene, pRootNode->GetChild(i));
	}

	// Fbx 매니저와 장면에 대한 포인터를 저장한다.
	*pFbxManager = pManager;
	*pFbxScene = pScene;

	return true;
}

/*!
*	\brief FBX의 메쉬 노드를 임포트한다.
*
*	\param pScene 임포트한 메쉬가 저장될 장면에 대한 포인터
*	\param pNode FBX 파일의 노드
*/
void import_fbx_mesh(EgScene *pScene, FbxNode *pNode)
{
	// 정점과 텍셀의 포인터를 저장할 배열
	std::vector<EgVertex *> vTable;
	std::vector<EgTexel *> tTable;
	std::vector<EgNormal *> nTable;

	// 현재 노드가 메쉬라면,
	fbxsdk::FbxMesh *pFbxMesh = pNode->GetMesh();
	if (pFbxMesh != NULL)
	{
		printf("Importing FBX mesh...\n");

		// 객체를 생성하여 장면에 추가한다.
		EgMesh *pEgMesh = new EgMesh(pFbxMesh->GetName());

		// 노드에 포함된 재질을 임포트하여 메쉬에 추가한다.
		int NumMtl = pNode->GetMaterialCount();
		if (NumMtl > 0)
		{
			printf("\t# of materials = %d\n", NumMtl);
			import_fbx_mtl(pEgMesh, pNode);
			pEgMesh->UpdateMaterialIdx();
		}

		// 정점 리스트를 생성한다.
		int NumVert = pFbxMesh->GetControlPointsCount();
		for (int i = 0; i < NumVert; ++i)
		{
			FbxVector4 v = pFbxMesh->mControlPoints.GetAt(i);
			EgVertex *pVert = new EgVertex(v[0], v[1], v[2]);
			pEgMesh->AddVertex(pVert);
			vTable.push_back(pVert);
		}

		// 텍셀 리스트를 생성한다.
		const FbxGeometryElementUV *pElementUV = NULL;
		if (pFbxMesh->GetElementUVCount() > 0)
		{
			pElementUV = pFbxMesh->GetElementUV(0);
			int NumTexel = pElementUV->GetDirectArray().GetCount();
			for (int i = 0; i < NumTexel; ++i)
			{
				FbxVector2 uv = pElementUV->GetDirectArray().GetAt(i);
				EgTexel *pTexel = new EgTexel(uv[0], uv[1]);
				pEgMesh->AddTexel(pTexel);
				tTable.push_back(pTexel);
			}
		}

		// 법선 리스트를 생성한다.
		const FbxGeometryElementNormal *pElementNormal = NULL;
		if (pFbxMesh->GetElementNormalCount() > 0)
		{
			pElementNormal = pFbxMesh->GetElementNormal(0);
			int NumNormal = pElementNormal->GetDirectArray().GetCount();
			for (int i = 0; i < NumNormal; ++i)
			{
				FbxVector4 n = pElementNormal->GetDirectArray().GetAt(i);
				EgNormal *pNormal = new EgNormal(n[0], n[1], n[2]);
				pEgMesh->AddNormal(pNormal);
				nTable.push_back(pNormal);
			}
		}

		// 각각의 삼각형이 사용하는 재질의 인덱스를 구한다.
		FbxLayerElementArrayTemplate<int> *MtlIdxs;
		pFbxMesh->GetMaterialIndices(&MtlIdxs);

		// 각각의 삼각형에 대하여
		int VertCount = 0;
		int NumFace = pFbxMesh->GetPolygonCount();
		for (int i = 0; i < NumFace; ++i)
		{
			// 삼각형을 구성하는 정점, 텍스처 정점, 법선의 인덱스를 저장한다.
			std::vector<int> vIdxs;
			std::vector<int> tIdxs;
			std::vector<int> nIdxs;
			for (int j = 0; j < 3; ++j)
			{
				// i 번째 삼각형의 j 번째 정점의 인덱스를 추가하고,
				int CtlPtIdx = pFbxMesh->GetPolygonVertex(i, j);
				vIdxs.push_back(CtlPtIdx);

				// i 번째 삼각형의 j 번째 정점의 텍스처 정점의 인덱스를 추가하고,
				if (pElementUV != NULL)
				{
					int tidx = pFbxMesh->GetTextureUVIndex(i, j);
					tIdxs.push_back(tidx);
				}

				// i 번째 삼각형의 j 번째 정점의 법선의 인덱스를 추가하고,
				if (pElementNormal != NULL)
				{
					int nidx = fbx_normal_idx(pElementNormal, CtlPtIdx, VertCount);
					nIdxs.push_back(nidx);
				}
				VertCount++;
			}

			// 삼각형을 메쉬에 추가한다.
			EgVertex *pVert[3] = { 0 };
			EgTexel *pTexel[3] = { 0 };
			EgNormal *pNormal[3] = { 0 };
			EgEdge *pEdge[3] = { 0 };

			// 삼각형의 정점을 구하고,
			pVert[0] = vTable[vIdxs[0]];
			pVert[1] = vTable[vIdxs[1]];
			pVert[2] = vTable[vIdxs[2]];

			// 삼각형의 텍셀을 구하고,
			if (!tIdxs.empty())
			{
				pTexel[0] = tTable[tIdxs[0]];
				pTexel[1] = tTable[tIdxs[1]];
				pTexel[2] = tTable[tIdxs[2]];
			}

			// 삼각형의 법선을 구하고,
			if (!nIdxs.empty())
			{
				pNormal[0] = nTable[nIdxs[0]];
				pNormal[1] = nTable[nIdxs[1]];
				pNormal[2] = nTable[nIdxs[2]];
			}

			// 삼각형이 사용하는 재질의 인덱스를 구하여
			int MtlIdx = MtlIdxs->GetAt(i);
			EgMaterial *pMtl = pEgMesh->GetMaterial(MtlIdx);

			// 하프에지를 생성하고,
			pEdge[0] = new EgEdge(pVert[0], pTexel[0], pNormal[0]);
			pEdge[1] = new EgEdge(pVert[1], pTexel[1], pNormal[1]);
			pEdge[2] = new EgEdge(pVert[2], pTexel[2], pNormal[2]);

			// 삼각형을 생성하여 메쉬에 추가한다.
			EgFace *pFace = new EgFace(pEdge[0], pEdge[1], pEdge[2], pMtl);
			pEgMesh->AddFace(pFace, "");
		}

		// 법선 정보가 없다면 갱신하고
		if (pElementNormal == NULL)
			pEgMesh->UpdateNormal(false);

		// 반대편 에지 정보를 초기화한다.
		pEgMesh->InitEdgeMate();

		// 임포트된 메쉬의 정보를 출력한다.
		int num_rmv_verts = pEgMesh->DelUnusedVert();
		int num_rmv_texs = pEgMesh->DelUnusedTexel();
		int num_rmv_normals = pEgMesh->DelUnusedNormal();
		printf("\tMesh Name = %s\n", pFbxMesh->GetName());
		printf("\t# of Vertices = %d\n", pEgMesh->GetNumVert());
		printf("\t# of Texels = %d\n", pEgMesh->GetNumTexel());
		printf("\t# of Normals = %d\n", pEgMesh->GetNumNormal());
		printf("\t# of Faces = %d\n", pEgMesh->GetNumFace());
		printf("\t# of Removed Vertices = %d\n", num_rmv_verts);
		printf("\t# of Removed Texels = %d\n", num_rmv_texs);
		printf("\t# of Removed Normals = %d\n", num_rmv_normals);
		printf("Importing FBX mesh completed...\n\n");

		// 메쉬를 장면에 추가한다.
		pScene->AddMesh(pEgMesh);
	}
	else
	{
		// 자식 노드를 처리한다.
		for (int j = 0; j < pNode->GetChildCount(); j++)
			import_fbx_mesh(pScene, pNode->GetChild(j));
	}
}

/*!
*	\brief FBX의 메쉬 노드에 포함된 재질을 임포트한다.
*
*	\param pMesh 임포트한 재질이 저장될 메쉬에 대한 포인터
*	\param pNode FBX 파일의 노드
*/
void import_fbx_mtl(EgMesh *pMesh, FbxNode *pNode)
{
	//printf("\tImporting Material node...\n");

	int NumMtl = pNode->GetMaterialCount();
	for (int i = 0; i < NumMtl; ++i)
	{
		FbxSurfaceMaterial *pMtl = pNode->GetMaterial(i);
		if (pMtl->GetClassId().Is(FbxSurfacePhong::ClassId)) // Phong 모델이라면
		{
			EgMaterial *pNewMtl = new EgMaterial();

			//pNewMtl->Phong = true;
			//pNewMtl->Lambert = false;
			pNewMtl->m_Name = pMtl->GetName();

			// Amibent Color
			FbxDouble3 Ambient = reinterpret_cast<FbxSurfacePhong *>(pMtl)->Ambient;
			pNewMtl->SetAmbient((GLfloat)Ambient[0], (GLfloat)Ambient[1], (GLfloat)Ambient[2]);

			// Diffuse Color
			FbxDouble3 Diffuse = reinterpret_cast<FbxSurfacePhong *>(pMtl)->Diffuse;
			pNewMtl->SetDiffuse((GLfloat)Diffuse[0], (GLfloat)Diffuse[1], (GLfloat)Diffuse[2]);

			// Specular Color
			FbxDouble3 Specular = reinterpret_cast<FbxSurfacePhong *>(pMtl)->Specular;
			pNewMtl->SetSpecular((GLfloat)Specular[0], (GLfloat)Specular[1], (GLfloat)Specular[2]);

			// Shininess
			double Shiness = reinterpret_cast<FbxSurfacePhong *>(pMtl)->Shininess;
			pNewMtl->SetShiness((GLfloat)Shiness);
			pMesh->AddMaterial(pNewMtl);
		}
		else if (pMtl->GetClassId().Is(FbxSurfaceLambert::ClassId)) // Lambert 모델이라면
		{
			EgMaterial *pNewMtl = new EgMaterial();
			//pNewMtl->Phong = false;
			//pNewMtl->Lambert = true;

			pNewMtl->m_Name = pMtl->GetName();

			// Amibent Color
			FbxDouble3 Ambient = reinterpret_cast<FbxSurfacePhong *>(pMtl)->Ambient;
			pNewMtl->SetAmbient((GLfloat)Ambient[0], (GLfloat)Ambient[1], (GLfloat)Ambient[2]);

			// Diffuse Color
			FbxDouble3 Diffuse = reinterpret_cast<FbxSurfacePhong *>(pMtl)->Diffuse;
			pNewMtl->SetDiffuse((GLfloat)Diffuse[0], (GLfloat)Diffuse[1], (GLfloat)Diffuse[2]);

			// Specular Color
			FbxDouble3 Specular = reinterpret_cast<FbxSurfacePhong *>(pMtl)->Specular;
			pNewMtl->SetSpecular((GLfloat)Specular[0], (GLfloat)Specular[1], (GLfloat)Specular[2]);

			// Shininess
			double Shiness = reinterpret_cast<FbxSurfacePhong *>(pMtl)->Shininess;
			pNewMtl->SetShiness((GLfloat)Shiness);

			pMesh->AddMaterial(pNewMtl);
		}
	}
}

/*!
*	\brief 정점이 사용하는 법선의 인덱스를 구한다.
*
*	\param pElementNormal 법선 레이어에 대한 포인터
*	\param CtlPtIdx 정점의 인덱스
*	\param VertCount 정점의 카운트
*/
int fbx_normal_idx(const FbxGeometryElementNormal *pElementNormal, int CtlPtIdx, int VertCount)
{
	int NormalIdx = -1;

	switch (pElementNormal->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (pElementNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			NormalIdx = CtlPtIdx;
			break;

		case FbxGeometryElement::eIndexToDirect:
			NormalIdx = pElementNormal->GetIndexArray().GetAt(CtlPtIdx);
			break;
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch (pElementNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			NormalIdx = VertCount;
			break;

		case FbxGeometryElement::eIndexToDirect:
			NormalIdx = pElementNormal->GetIndexArray().GetAt(VertCount);
			break;
		}
		break;
	}

	return NormalIdx;
}