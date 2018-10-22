#include "..\usr\include\egmesh.h"
#include <cstring>
#include <vector>
#include <map>
#include <direct.h>

/*!
*	\brief ��θ��� ������ ���ϸ�(Ȯ��� ����)�� ��ȯ�Ѵ�.
*
*	\param full_path ��θ��� ������ ������ �̸�
*
*	\return \a full_path���� ��θ��� ������ ���ϸ�(Ȯ���)�� ��ȯ�Ѵ�.
*/
std::string get_file_name_ext(std::string full_path)
{
	int idx = (int)full_path.rfind("\\");
	std::string ret = full_path.substr(idx + 1, full_path.length() - idx - 1);

	return ret;
}

/*!
*	\brief ��θ��� ������ ���ϸ�(Ȯ��� ������)�� ��ȯ�Ѵ�.
*
*	\param full_path ��θ��� ������ ������ �̸�
*
*	\return \a full_path���� ��θ��� ������ ���ϸ�(Ȯ��� ������)�� ��ȯ�Ѵ�.
*/
std::string get_file_name(std::string full_path)
{
	int idx0 = (int)full_path.rfind("\\");
	int idx1 = (int)full_path.rfind(".");
	std::string ret = full_path.substr(idx0 + 1, idx1 - idx0 - 1);

	return ret;
}

/*!
*	\brief ������ Ȯ���ڸ� ��ȯ�Ѵ�.
*
*	\param full_path ��θ��� ������ ������ �̸�
*
*	\return \a full_path���� ���� Ȯ���ڸ� ��ȯ�Ѵ�.
*/
std::string get_file_ext(std::string full_path)
{
	int idx = (int)full_path.rfind(".");
	std::string ret = full_path.substr(idx + 1, full_path.length() - idx - 1);

	return ret;
}

/*!
*	\brief ������ ���丮 ��θ��� ��ȯ�Ѵ�.
*
*	\param full_path ��θ��� ������ ������ �̸�
*
*	\return \a full_path���� ���丮 ��θ��� ��ȯ�Ѵ�.
*/
std::string get_file_path(std::string full_path)
{
	int idx = (int)full_path.rfind("\\");
	std::string ret = full_path.substr(0, idx);

	return ret;
}

/*!
*	\brief ������ ���ڿ��� ��ȯ�Ͽ� ��ȯ�Ѵ�.
*
*	\param i ���ڿ��� ��ȯ�� ����
*
*	\return ���ڿ��� ��ȯ�� ����
*/
std::string cast_str(int i)
{
	char buffer[128];
	_itoa_s(i, buffer, 10);

	return std::string(buffer);
}

/*!
*	\brief �Ǽ��� ���ڿ��� ��ȯ�Ͽ� ��ȯ�Ѵ�.
*
*	\param v ���ڿ��� ��ȯ�� �Ǽ�
*
*	\return ���ڿ��� ��ȯ�� �Ǽ�
*/
std::string cast_str(float v)
{
	char buffer[128];
	sprintf_s(buffer, "%f", v);

	return std::string(buffer);
}

/*!
*	\brief �Ǽ��� ���ڿ��� ��ȯ�Ͽ� ��ȯ�Ѵ�.
*
*	\param v ���ڿ��� ��ȯ�� �Ǽ�
*
*	\return ���ڿ��� ��ȯ�� �Ǽ�
*/
std::string cast_str(double v)
{
	char buffer[128];
	sprintf_s(buffer, "%lf", v);

	return std::string(buffer);
}

/*!
*	\brief �ؽ�ó ��ǥ�� 0�� 1���̷� ����ȭ �Ѵ�.
*
*	\param u �ؽ�ó ��ǥ u
*	\param v �ؽ�ó ��ǥ v
*/
void normalize_uv(double &u, double &v)
{
	u = (u > 1.0) ? (u - (int)(u)) : u;
	u = (u < 0.0) ? (u - (int)(u - 1.0)) : u;

	v = (v > 1.0) ? (v - (int)(v)) : v;
	v = (v < 0.0) ? (v - (int)(v - 1.0)) : v;
}

/*!
*	\brief ��Ʈ�� ������ �ε��Ѵ�.
*
*	\param filename �ε��� ������ �̸�
*	\param info ��Ʈ�� ������ ������ �����
*
*	\return ��Ʈ�� ������ �̹��� ������
*/
unsigned char *open_image_bmp(const char *filename, BITMAPINFO **info)
{
	// ���� �۾� ���͸��� ����Ѵ�.
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
*	\brief ��Ʈ�� �����͸� ���Ϸ� �����Ѵ�.
*
*	\param filename ������ ������ �̸�
*	\param info ��Ʈ�� ������ ����
*	\param bits ��Ʈ�� ������
*
*	\return ����: 0, ����: -1
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
*	\brief TGA ������ �ε��Ѵ�.
*
*	\param name �ε��� ������ �̸�
*	\param width �̹����� ���� ����
*	\param height �̹����� ���� ����
*	\param format �̹��� ����
*
*	\return ��Ʈ�� ������ �̹��� ������
*/
unsigned char *open_image_tga(const char *name, int *width, int *height, GLenum *format)
{
	// ���� �۾� ���͸��� ����Ѵ�.
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
*	\brief OBJ ���Ͽ��� ����ϴ� MTL(����)������ �о� �޽��� �߰��Ѵ�.
*
*	\param pMesh ������ �߰��� �޽��� ������
*	\param fname ���� ������ �̸�(���� �۾� ��ο��� ������� ��η� ǥ����)
*
*	\return ����: true, ����: flase
*/
bool import_obj_mtl(EgMesh *pMesh, const char *fname)
{
	// ������ ���� ������ �����Ѵ�.
	EgMaterial *pMtl = NULL;

	// ������ ����, 
	FILE *fp;
	fopen_s(&fp, fname, "r");
	if (!fp) // �����ϸ� ����Ʈ ������ �����Ͽ� �޽��� �߰��ϰ� false�� ��ȯ�Ѵ�.
	{
		printf("%s does not exist...\n", fname);
		pMtl = new EgMaterial("mtl_defalut");
		pMesh->AddMaterial(pMtl);
		return false;
	}

	// ������ ������ �� �ܾ �о�, tag �迭�� �����Ѵ�.
	char tag[256];
	while (fscanf_s(fp, "%s", tag, 256) != EOF)
	{
		// newmtl (new material) �±׶��,
		if (!strcmp(tag, "newmtl"))
		{
			// ������ �̸��� �а�,
			char tmp[64];
			fscanf_s(fp, "%s", tmp, 64);

			// ������ �����Ͽ�, �޽��� ���� ����Ʈ�� �߰��Ѵ�.
			pMtl = new EgMaterial(tmp);
			pMesh->AddMaterial(pMtl);
			printf("Material: %s is created...\n", tmp);
		}

		// Ka (ambient coefficients) �±׶��,
		if (!strcmp(tag, "Ka"))
		{
			// ambient ������ �о ������ �����Ѵ�.
			GLfloat c[3];
			fscanf_s(fp, "%f%f%f", &c[0], &c[1], &c[2]);
			pMtl->SetAmbient(c[0], c[1], c[2]);
		}

		// Kd (diffuse coefficients) �±׶��,
		if (!strcmp(tag, "Kd"))
		{
			// diffuse ������ �о ������ �����Ѵ�.
			GLfloat c[3];
			fscanf_s(fp, "%f%f%f", &c[0], &c[1], &c[2]);
			pMtl->SetDiffuse(c[0], c[1], c[2]);
		}

		// Ks (specular coefficients) �±׶��,
		if (!strcmp(tag, "Ks"))
		{
			// specular ������ �о ������ �����Ѵ�.
			GLfloat c[3];
			fscanf_s(fp, "%f%f%f", &c[0], &c[1], &c[2]);
			pMtl->SetSpecular(c[0], c[1], c[2]);
		}

		// map_Kd (diffuse texture file) �±׶��,
		if (!strcmp(tag, "map_Kd"))
		{
			// �ý�ó ������ �д´�.
			char tex_name[64];
			fscanf_s(fp, "%s", tex_name, 64);
			pMtl->LoadTexture(tex_name);
		}
	}

	// ������ �ݴ´�.
	fclose(fp);
	return true;
}

/*!
*	\brief OBJ ������ ����Ʈ�Ͽ� ��鿡 �߰��Ѵ�.
*	\note ���� �۾� ���͸��� ����Ʈ�� ������ �ִ� �����͸��� ����ȴ�.
*
*	\param pScene ����Ʈ�� Obj �޽��� ���Ե� ��鿡 ���� ������
*	\param FileName ��ü ��ΰ� ���Ե� ������ �̸�
*
*	\return �����̸� true, ���и� false�� ��ȯ�Ѵ�.
*/
bool import_obj(EgScene *pScene, const char *FileName)
{
	// ����Ʈ�� ������ �ִ� ���͸��� ���Ͽ�, ���� �۾� ���͸��� �����Ѵ�.
	//std::string cwd = get_file_path(FileName);
	//_chdir(cwd.c_str());

	// ������ ���� �����ϸ� false�� ��ȯ�Ѵ�.
	FILE *fp;
	fopen_s(&fp, FileName, "r");
	if (!fp)
	{
		printf("Obj file import failed...\n");
		return false;
	}

	// ������ �̸�(��� �� Ȯ���� ����)�� ���Ͽ� �޽��� �����Ѵ�.
	std::string MeshName = get_file_name(FileName);
	EgMesh *pMesh = new EgMesh(MeshName);

	// �ε��� �ʿ��� �ӽ� �������� �����Ѵ�.
	char tag[256];
	bool bTexture = false;
	bool bNormal = false;
	std::string GroupName;
	EgMaterial *pCurrMtl = NULL;
	std::vector<EgVertex *> vTable;
	std::vector<EgTexel *> tTable;
	std::vector<EgNormal *> nTable;

	// ������ ������ �� �ܾ �����鼭 tag �迭�� �����Ѵ�.
	while (fscanf_s(fp, "%s", tag, 256) != EOF)
	{
		// mtllib (material library) �±׶��,
		if (!strcmp(tag, "mtllib"))
		{
			// ���� ������ �д´�.
			char mtl_fname[64];
			fscanf_s(fp, "%s", mtl_fname, 64);
			if (!import_obj_mtl(pMesh, mtl_fname))
				printf("Mtl file import failed...\n");
		}

		// v (vertex) �±׶��,
		if (!strcmp(tag, "v"))
		{
			// ��ǥ(x, y, z)�� �о� ������ �����ϰ� �޽��� �߰��Ѵ�.
			double x, y, z;
			fscanf_s(fp, "%lf%lf%lf", &x, &y, &z);
			EgVertex *pVert = new EgVertex(x, y, z);
			pMesh->AddVertex(pVert);
			vTable.push_back(pVert);
		}

		// vt (vertex texture) �±׶��,
		if (!strcmp(tag, "vt"))
		{
			// �ؽ�ó ��ǥ(u, v)�� �о� �ؼ��� �����ϰ� �޽��� �߰��Ѵ�.
			bTexture = true;
			double u, v;
			fscanf_s(fp, "%lf%lf", &u, &v);
			normalize_uv(u, v);
			EgTexel *pTexel = new EgTexel(u, v);
			pMesh->AddTexel(pTexel);
			tTable.push_back(pTexel);
		}

		// vn (vertex normal) �±׶��,
		if (!strcmp(tag, "vn"))
		{
			// ������ ����(nx, ny, nz)�� �о� ������ �����ϰ� �޽��� �߰��Ѵ�.
			bNormal = true;
			double nx, ny, nz;
			fscanf_s(fp, "%lf%lf%lf", &nx, &ny, &nz);
			EgNormal *pNormal = new EgNormal(nx, ny, nz);
			pMesh->AddNormal(pNormal);
			nTable.push_back(pNormal);
		}

		// g (group) �±׶��,
		if (!strcmp(tag, "g"))
		{
			// �׷��� �̸��� �д´�.
			char grp_name[64];
			fscanf_s(fp, "%s", grp_name, 64);
			GroupName = std::string(grp_name);
		}

		// usemtl (use material) �±׶��,
		if (!strcmp(tag, "usemtl"))
		{
			// �޽��� ���� ����Ʈ���� �ش� ������ ã�´�.
			char mtl_name[128];
			fscanf_s(fp, "%s", mtl_name, 128);
			pCurrMtl = pMesh->GetMaterial(mtl_name);
		}

		// f (face) �±׶��,
		if (!strcmp(tag, "f"))
		{
			// �������� �����ϴ� ����, �ؼ�, �븻�� �ε����� �����ϴ� ������ �����ϰ�,
			std::vector<int> vIdxs, tIdxs, nIdxs;

			// �� ���� �о,
			char line[128];
			fgets(line, 128, fp);
			char *pData = line;

			// �� ���� ��� ���� ������
			while (true)
			{
				// ���ʿ��� ����� �ٺ����� �ǳʶ��,
				while (*pData == ' ' || *pData == '\n')
					pData++;

				// �� ���� ��� �о��ٸ� �ݺ����� ���� ���´�.
				if (!strcmp(pData, ""))
					break;

				// �ε��� ������ buffer�� ���� ��,
				char buffer[64];	// buffer[] = "vidx/tidx/nidx" 
				sscanf_s(pData, "%s", buffer, 64);
				pData += strlen(buffer);

				// �ε��� ������ ������ ������ �����ϰ�
				char sep;
				int vidx, tidx, nidx;

				// ����, �ؼ�, �븻 �ε����� ��� �ִ� ��� (f 1/1/1/ 2/2/2 3/3/3 ...)
				if (bTexture && bNormal)
				{
					// ��� �ε����� �о� ����Ʈ�� �߰��Ѵ�.
					sscanf_s(buffer, "%d%c%d%c%d", &vidx, &sep, 1, &tidx, &sep, 1, &nidx);
					vIdxs.push_back(vidx);
					tIdxs.push_back(tidx);
					nIdxs.push_back(nidx);
					continue;
				}

				// ������ �ؼ� �ε����� �ִ� ��� (f 1/1 2/2 3/3 ...)
				if (bTexture && !bNormal)
				{
					// ������ �ؼ� �ε����� �о� ����Ʈ�� �߰��Ѵ�.
					sscanf_s(buffer, "%d%c%d", &vidx, &sep, 1, &tidx);
					vIdxs.push_back(vidx);
					tIdxs.push_back(tidx);
					continue;
				}

				// ������ �븻 �ε����� �ִ� ��� (f 1//1 2//2 3//3 ...)
				if (!bTexture && bNormal)
				{
					// ������ �븻 �ε����� �о� ����Ʈ�� �߰��Ѵ�.
					sscanf_s(buffer, "%d%c%c%d", &vidx, &sep, 1, &sep, 1, &nidx);
					vIdxs.push_back(vidx);
					nIdxs.push_back(nidx);
					continue;
				}

				// ������ �ε����� �ִ� ��� (f 1 2 3 ...)
				if (!bTexture && !bNormal)
				{
					// ������ �ε����� �о� ����Ʈ�� �߰��Ѵ�.
					sscanf_s(buffer, "%d", &vidx);
					vIdxs.push_back(vidx);
					continue;
				}
			}

			// �������� �ﰢ������ ������ �޽��� �߰��Ѵ�.
			int PolygonSize = (int)vIdxs.size(); // �ﰢ��(PolygonSize = 3), �簢��(PolygonSize = 4), ...
			for (int i = 0; i < PolygonSize - 2; ++i)
			{
				// �ﰢ���� ������ ���ϰ�,
				EgVertex *pVert[3] = { 0 };
				pVert[0] = vTable[vIdxs[0] - 1];
				pVert[1] = vTable[vIdxs[i + 1] - 1];
				pVert[2] = vTable[vIdxs[i + 2] - 1];

				// �� ������ �ؼ� ������ ���ϰ�,
				EgTexel *pTexel[3] = { 0 };
				if (bTexture)
				{
					pTexel[0] = tTable[tIdxs[0] - 1];
					pTexel[1] = tTable[tIdxs[i + 1] - 1];
					pTexel[2] = tTable[tIdxs[i + 2] - 1];
				}

				// �� ������ ���� ������ ���ϰ�,
				EgNormal *pNormal[3] = { 0 };
				if (bNormal)
				{
					pNormal[0] = nTable[nIdxs[0] - 1];
					pNormal[1] = nTable[nIdxs[i + 1] - 1];
					pNormal[2] = nTable[nIdxs[i + 2] - 1];
				}
				else // ���� ������ ���ٸ� �ﰢ�� �������� ��ü�Ѵ�.
				{
					GVector3 n = ((pVert[1]->m_Pos - pVert[0]->m_Pos) ^ (pVert[2]->m_Pos - pVert[0]->m_Pos)).Normalize();
					pNormal[0] = pNormal[1] = pNormal[2] = new EgNormal(n[0], n[1], n[2]);
					pMesh->AddNormal(pNormal[0]);
				}

				// ���������� �����ϰ�,
				EgEdge *pEdge[3] = { 0 };
				pEdge[0] = new EgEdge(pVert[0], pTexel[0], pNormal[0]);
				pEdge[1] = new EgEdge(pVert[1], pTexel[1], pNormal[1]);
				pEdge[2] = new EgEdge(pVert[2], pTexel[2], pNormal[2]);

				// �ﰢ���� �����Ͽ� �޽��� �߰��Ѵ�.
				if (pCurrMtl == NULL)
					pCurrMtl = pMesh->m_pMaterials;
				EgFace *pFace = new EgFace(pEdge[0], pEdge[1], pEdge[2], pCurrMtl);
				pMesh->AddFace(pFace, GroupName);
			}
		}
	}

	// ������ �ݴ´�.
	fclose(fp);

	// ���������� �ݴ��� ������ ������ �����Ѵ�.
	pMesh->InitEdgeMate();

	// ����Ʈ�� �޽��� �⺻ ������ ����Ѵ�.
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

	// ����Ʈ�� �޽��� ��鿡 �߰��ϰ� true�� ��ȯ�Ѵ�.
	pScene->AddMesh(pMesh);
	return false;
}

/*!
*	\brief �޽��� ����(*.obj)�� �����Ѵ�.
*
*	\param pMesh ������ �޽��� ������
*	\param fname ������ �̸�
*
*	\return ����: true, ����: flase
*/
bool export_obj(EgMesh *pMesh, const char *fname)
{
	// ������ ���� �����ϸ� false�� ��ȯ�Ѵ�.
	FILE *fp;
	fopen_s(&fp, fname, "w");
	if (!fp)
		return false;

	// ������ ���� ���� ����ϰ�,
	fprintf_s(fp, "%s", "# Wavefront OBJ exported by EgTool\n");
	fprintf_s(fp, "%s", "# http://gaia.dongguk.edu \n\n");

	// ���� ���̺귯�� ������ ����ϰ�,
	int NumMtrl = pMesh->GetNumFace();
	if (NumMtrl != 0)
	{
		// ���� ����(*.mtl)�� �̸��� ����ϰ�,
		std::string file_name(fname);
		file_name = get_file_name(fname) + ".mtl";
		fprintf(fp, "mtllib %s\n\n", file_name.c_str());

		// ���� ���� ����(*.mtl)�� �����Ͽ� ����Ѵ�.
		export_obj_mtl(pMesh, file_name.c_str());
	}

	// ����, �ؼ�, ������ �ε����� �����Ѵ�.
	int NumVert = pMesh->UpdateVertIdx();
	int NumTexel = pMesh->UpdateTexIdx();
	int NumNormal = pMesh->UpdateNormalIdx();

	// ������ ������ ����Ѵ�.
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

	// �ؼ��� ������ ����Ѵ�.
	printf("\tNumber of Texels = %d\n", NumTexel);
	EgTexel *t = pMesh->m_pTexels;
	while (t != NULL)
	{
		GVector2 uv = t->m_Coord;
		fprintf(fp, "%s %lf %lf\n", "vt", uv[0], uv[1]);
		t = t->m_pNext;
	}
	fprintf(fp, "\n");

	// ������ ������ ����Ѵ�.
	printf("\tNumber of Normals = %d\n", NumNormal);
	EgNormal *n = pMesh->m_pNormals;
	while (n != NULL)
	{
		GVector3 dir = n->m_Dir;
		fprintf(fp, "%s %lf %lf %lf\n", "vn", dir[0], dir[1], dir[2]);
		n = n->m_pNext;
	}
	fprintf(fp, "\n");

	// �ﰢ���� �׷� ������ ���ٸ�
	if (pMesh->m_GroupedFaces.empty())
	{
		// ����ô���� ��� �����￡ ���Ͽ�,
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
		// ������ �׷쿡 ���Ͽ�,
		for (it = pMesh->m_GroupedFaces.begin(); it != pMesh->m_GroupedFaces.end(); ++it)
		{
			// �׷��� �̸��� ����ϰ�,
			std::string GroupName = it->first;
			fprintf(fp, "g %s\n", GroupName.c_str());

			// ���� ������ �ε����� ���� ������ �����ϰ�,
			EgMaterial *pPrevMtrl = NULL;

			// �׷쿡 ���� ������ �����￡ ���Ͽ�,
			int NumFace = (int)it->second.size();
			for (int i = 0; i < NumFace; ++i)
			{
				// �ﰢ�� ������ ���,
				EgFace *f = it->second[i];

				// �ﰢ���� ���� ������ �ִٸ� ���� ���������� ���Ͽ� �ٸ� ��� ����ϰ�,
				if (pPrevMtrl == NULL || f->m_pMaterial != pPrevMtrl)
				{
					fprintf(fp, "usemtl %s\n", f->m_pMaterial->m_Name.c_str());
					pPrevMtrl = f->m_pMaterial;
				}

				// ������ �ε����� ���ϰ�,
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


	// ������ �ݴ´�.
	fclose(fp);
	printf("Exporting completed\n\n");

	return true;
}

/*!
*	\brief ���� ������ ����(*.mtl)�� �����Ѵ�.
*
*	\param pMesh ������ �޽��� ������
*	\param fname ������ �̸�
*
*	\return ����: true, ����: flase
*/
bool export_obj_mtl(EgMesh *pMesh, const char *fname)
{
	// ������ ���� �����ϸ� false�� ��ȯ�Ѵ�.
	FILE *fp;
	fopen_s(&fp, fname, "w");
	if (!fp)
		return false;

	// ������ ���� ���� ����ϰ�,
	fprintf(fp, "%s", "# Wavefront MTL exported by EgTool\n");
	fprintf(fp, "%s", "# http://gaia.dongguk.edu \n\n");

	// ������ ������ ���Ͽ�,
	EgMaterial *pMtrl = pMesh->m_pMaterials;
	while (pMtrl != NULL)
	{
		// ������ ������ ����ϰ�,
		fprintf(fp, "newmtl %s\n", pMtrl->m_Name.c_str());
		fprintf(fp, "Ka %f %f %f\n", pMtrl->m_Ambient[0], pMtrl->m_Ambient[1], pMtrl->m_Ambient[2]);
		fprintf(fp, "Kd %f %f %f\n", pMtrl->m_Diffuse[0], pMtrl->m_Diffuse[1], pMtrl->m_Diffuse[2]);
		fprintf(fp, "Ks %f %f %f\n", pMtrl->m_Specular[0], pMtrl->m_Specular[1], pMtrl->m_Specular[2]);

		// �ؽ�ó�� �ִٸ� ���ϸ��� ����Ѵ�.
		if (!pMtrl->m_TexFileName.empty())
			fprintf(fp, "map_Kd %s\n", pMtrl->m_TexFileName.c_str());

		fprintf(fp, "\n");
		pMtrl = pMtrl->m_pNext;
	}

	// ������ �ݴ´�.
	fclose(fp);

	return true;
}

/*!
*	\brief FBX ������ ����Ʈ�Ͽ� ��鿡 �߰��Ѵ�.
*
*	\param pEgScene ����Ʈ�� ������ ����� ��鿡 ���� ������
*	\param fname ����Ʈ�� ������ �̸�
*	\param pFbxManager ����Ʈ�� ������ Fbx �Ŵ����� ���� �����Ͱ� ����ȴ�.
*	\param pFbxScene ����Ʈ�� ������ Fbx ��鿡 ���� �����Ͱ� ����ȴ�.
*
*	\return �����̸� true, ���и� false�� ��ȯ�Ѵ�.
*/
bool import_fbx(EgScene *pEgScene, const char *fname, FbxManager **pFbxManager, FbxScene **pFbxScene)
{
	// FBX �Ŵ����� �����Ѵ�.
	FbxManager *pManager = FbxManager::Create();
	if (pManager == NULL)
	{
		FBXSDK_printf("Error: Unable to create FBX Manager!\n");
		return false;
	}
	else
		FBXSDK_printf("Autodesk FBX SDK version %s\n\n", pManager->GetVersion());

	// IO ���� ��ü�� �����Ѵ�.
	FbxIOSettings *ios = FbxIOSettings::Create(pManager, IOSROOT);

	// �Ŵ����� IO ���� ��ü�� �����Ѵ�.
	pManager->SetIOSettings(ios);

	// ���ο� ���� �����Ѵ�.
	FbxScene *pScene = FbxScene::Create(pManager, "FbxScene");
	if (pFbxScene == NULL)
	{
		FBXSDK_printf("Error: Unable to create FBX scene!\n");
		return false;
	}

	// �����͸� �����Ѵ�.
	FbxImporter *pImporter = FbxImporter::Create(pManager, "");

	// �ε��� ���� �̸����� �����͸� �ʱ�ȭ �Ѵ�.
	if (!pImporter->Initialize(fname, -1, pManager->GetIOSettings()))
	{
		printf("Call to FbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", pImporter->GetStatus().GetErrorString());
		return false;
	}

	// ��ü�� ����Ʈ�Ͽ� ���� �߰��Ѵ�.
	pImporter->Import(pScene);

	// ����� �����͸� �����Ѵ�.
	pImporter->Destroy();

	// ����� ��� ��带 �ﰢ��ȭ �Ѵ�.
	FbxGeometryConverter geometryConverter(pManager);
	geometryConverter.Triangulate(pScene, true);

	// ���� ��Ʈ ��带 ���Ͽ�
	FbxNode *pRootNode = pScene->GetRootNode();
	if (pRootNode)
	{
		// ��Ʈ ����� �ڽ� ��带 ������� �� ����Ʈ �Ѵ�.
		for (int i = 0; i < pRootNode->GetChildCount(); i++)
			import_fbx_mesh(pEgScene, pRootNode->GetChild(i));
	}

	// Fbx �Ŵ����� ��鿡 ���� �����͸� �����Ѵ�.
	*pFbxManager = pManager;
	*pFbxScene = pScene;

	return true;
}

/*!
*	\brief FBX�� �޽� ��带 ����Ʈ�Ѵ�.
*
*	\param pScene ����Ʈ�� �޽��� ����� ��鿡 ���� ������
*	\param pNode FBX ������ ���
*/
void import_fbx_mesh(EgScene *pScene, FbxNode *pNode)
{
	// ������ �ؼ��� �����͸� ������ �迭
	std::vector<EgVertex *> vTable;
	std::vector<EgTexel *> tTable;
	std::vector<EgNormal *> nTable;

	// ���� ��尡 �޽����,
	fbxsdk::FbxMesh *pFbxMesh = pNode->GetMesh();
	if (pFbxMesh != NULL)
	{
		printf("Importing FBX mesh...\n");

		// ��ü�� �����Ͽ� ��鿡 �߰��Ѵ�.
		EgMesh *pEgMesh = new EgMesh(pFbxMesh->GetName());

		// ��忡 ���Ե� ������ ����Ʈ�Ͽ� �޽��� �߰��Ѵ�.
		int NumMtl = pNode->GetMaterialCount();
		if (NumMtl > 0)
		{
			printf("\t# of materials = %d\n", NumMtl);
			import_fbx_mtl(pEgMesh, pNode);
			pEgMesh->UpdateMaterialIdx();
		}

		// ���� ����Ʈ�� �����Ѵ�.
		int NumVert = pFbxMesh->GetControlPointsCount();
		for (int i = 0; i < NumVert; ++i)
		{
			FbxVector4 v = pFbxMesh->mControlPoints.GetAt(i);
			EgVertex *pVert = new EgVertex(v[0], v[1], v[2]);
			pEgMesh->AddVertex(pVert);
			vTable.push_back(pVert);
		}

		// �ؼ� ����Ʈ�� �����Ѵ�.
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

		// ���� ����Ʈ�� �����Ѵ�.
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

		// ������ �ﰢ���� ����ϴ� ������ �ε����� ���Ѵ�.
		FbxLayerElementArrayTemplate<int> *MtlIdxs;
		pFbxMesh->GetMaterialIndices(&MtlIdxs);

		// ������ �ﰢ���� ���Ͽ�
		int VertCount = 0;
		int NumFace = pFbxMesh->GetPolygonCount();
		for (int i = 0; i < NumFace; ++i)
		{
			// �ﰢ���� �����ϴ� ����, �ؽ�ó ����, ������ �ε����� �����Ѵ�.
			std::vector<int> vIdxs;
			std::vector<int> tIdxs;
			std::vector<int> nIdxs;
			for (int j = 0; j < 3; ++j)
			{
				// i ��° �ﰢ���� j ��° ������ �ε����� �߰��ϰ�,
				int CtlPtIdx = pFbxMesh->GetPolygonVertex(i, j);
				vIdxs.push_back(CtlPtIdx);

				// i ��° �ﰢ���� j ��° ������ �ؽ�ó ������ �ε����� �߰��ϰ�,
				if (pElementUV != NULL)
				{
					int tidx = pFbxMesh->GetTextureUVIndex(i, j);
					tIdxs.push_back(tidx);
				}

				// i ��° �ﰢ���� j ��° ������ ������ �ε����� �߰��ϰ�,
				if (pElementNormal != NULL)
				{
					int nidx = fbx_normal_idx(pElementNormal, CtlPtIdx, VertCount);
					nIdxs.push_back(nidx);
				}
				VertCount++;
			}

			// �ﰢ���� �޽��� �߰��Ѵ�.
			EgVertex *pVert[3] = { 0 };
			EgTexel *pTexel[3] = { 0 };
			EgNormal *pNormal[3] = { 0 };
			EgEdge *pEdge[3] = { 0 };

			// �ﰢ���� ������ ���ϰ�,
			pVert[0] = vTable[vIdxs[0]];
			pVert[1] = vTable[vIdxs[1]];
			pVert[2] = vTable[vIdxs[2]];

			// �ﰢ���� �ؼ��� ���ϰ�,
			if (!tIdxs.empty())
			{
				pTexel[0] = tTable[tIdxs[0]];
				pTexel[1] = tTable[tIdxs[1]];
				pTexel[2] = tTable[tIdxs[2]];
			}

			// �ﰢ���� ������ ���ϰ�,
			if (!nIdxs.empty())
			{
				pNormal[0] = nTable[nIdxs[0]];
				pNormal[1] = nTable[nIdxs[1]];
				pNormal[2] = nTable[nIdxs[2]];
			}

			// �ﰢ���� ����ϴ� ������ �ε����� ���Ͽ�
			int MtlIdx = MtlIdxs->GetAt(i);
			EgMaterial *pMtl = pEgMesh->GetMaterial(MtlIdx);

			// ���������� �����ϰ�,
			pEdge[0] = new EgEdge(pVert[0], pTexel[0], pNormal[0]);
			pEdge[1] = new EgEdge(pVert[1], pTexel[1], pNormal[1]);
			pEdge[2] = new EgEdge(pVert[2], pTexel[2], pNormal[2]);

			// �ﰢ���� �����Ͽ� �޽��� �߰��Ѵ�.
			EgFace *pFace = new EgFace(pEdge[0], pEdge[1], pEdge[2], pMtl);
			pEgMesh->AddFace(pFace, "");
		}

		// ���� ������ ���ٸ� �����ϰ�
		if (pElementNormal == NULL)
			pEgMesh->UpdateNormal(false);

		// �ݴ��� ���� ������ �ʱ�ȭ�Ѵ�.
		pEgMesh->InitEdgeMate();

		// ����Ʈ�� �޽��� ������ ����Ѵ�.
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

		// �޽��� ��鿡 �߰��Ѵ�.
		pScene->AddMesh(pEgMesh);
	}
	else
	{
		// �ڽ� ��带 ó���Ѵ�.
		for (int j = 0; j < pNode->GetChildCount(); j++)
			import_fbx_mesh(pScene, pNode->GetChild(j));
	}
}

/*!
*	\brief FBX�� �޽� ��忡 ���Ե� ������ ����Ʈ�Ѵ�.
*
*	\param pMesh ����Ʈ�� ������ ����� �޽��� ���� ������
*	\param pNode FBX ������ ���
*/
void import_fbx_mtl(EgMesh *pMesh, FbxNode *pNode)
{
	//printf("\tImporting Material node...\n");

	int NumMtl = pNode->GetMaterialCount();
	for (int i = 0; i < NumMtl; ++i)
	{
		FbxSurfaceMaterial *pMtl = pNode->GetMaterial(i);
		if (pMtl->GetClassId().Is(FbxSurfacePhong::ClassId)) // Phong ���̶��
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
		else if (pMtl->GetClassId().Is(FbxSurfaceLambert::ClassId)) // Lambert ���̶��
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
*	\brief ������ ����ϴ� ������ �ε����� ���Ѵ�.
*
*	\param pElementNormal ���� ���̾ ���� ������
*	\param CtlPtIdx ������ �ε���
*	\param VertCount ������ ī��Ʈ
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