#pragma once

#include "egmath.h"
#include "GL\freeglut.h"
#include <fbxsdk.h>

/* EgMesh �ڷ� ���� */
class EgVertex;
class EgTexel;
class EgNormal;
class EgEdge;
class EgFace;
class EgMaterial;
class EgMesh;
class EgScene;

// ���ϸ�/���ڿ� ó���Լ�
std::string get_file_name_ext(std::string full_path);
std::string get_file_name(std::string full_path);
std::string get_file_ext(std::string full_path);
std::string get_file_path(std::string full_path);
std::string cast_str(int i);
std::string cast_str(double v);
std::string cast_str(float v);
void normalize_uv(double &u, double &v);

// TGA �̹��� �Լ�
unsigned char *open_image_tga(const char *, int *, int *, GLenum *);
unsigned char *open_image_bmp(const char *filename, BITMAPINFO **info);
int save_image_bmp(const char *filename, BITMAPINFO *info, GLubyte *bits);

// OBJ ���� �Լ�
bool import_obj(EgScene *pScene, const char *fname);
bool export_obj(EgMesh *pMesh, const char *fname);

// FBX ���� ���� �Լ�
bool import_fbx(EgScene *pEgScene, const char *fname, FbxManager **pFbxManager, FbxScene **pFbxScene);
//bool export_fbx(EgMesh *pMesh, const char *fname);

static bool import_obj_mtl(EgMesh *pMesh, const char *fname);
static bool export_obj_mtl(EgMesh *pMesh, const char *fname);
static void import_fbx_mesh(EgScene *pScene, FbxNode *pNode);
static void import_fbx_mtl(EgMesh *pMesh, FbxNode *pNode);
static int fbx_normal_idx(const FbxGeometryElementNormal *pElementNormal, int CtlPtIdx, int VertCount);
